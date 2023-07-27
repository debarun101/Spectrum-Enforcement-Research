// demonstrate timed_queue() statement

#include "cpp.h"
#include <stdio.h>
FILE *fp;

event ev("ev");

void proc(long);

extern "C" void sim()
{
	long i;

	fp = fopen("tmd_wait.out", "w");
	set_output_file(fp);
	create("sim");
	for(i = 0; i < 10; i++)
		proc(i);
	dump_status();
	hold(5.0);
	ev.set();
	dump_status();
	hold(1.0);
}

void proc(long n)
{
	long st;

	create("proc");
	st = ev.timed_wait((float)n);
	fprintf(fp, "proc.%ld: event %s\n",
		n, (st == EVENT_OCCURRED) ? "occurred" : "timed out");
}

