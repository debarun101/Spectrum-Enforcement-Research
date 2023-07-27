// test clock synchronized facility

#include "cpp.h"
#include <stdio.h>
FILE *fp;

#define NJ 3
#define NC 2

storage s("s", 1);

void job(int);

extern "C" void sim()
{
	int i;

	fp = fopen("syn_stor.out", "w");
	set_output_file(fp);
	set_trace_file(fp);
//	trace_on();
	create("sim");
	s.synchronous(0.5, 1.0);

	for(i = 1; i <= NJ; i++)
		job(i);
	hold(100.0);
	report();
}

void job(int n)
{
	int i;

	create("job");
	set_priority(n);
	for(i = 0; i < NC; i++) {
		fprintf(fp, "%8.3lf job.%ld allocate\n", clock, n);
		s.allocate(1);
		fprintf(fp, "%8.3lf job.%ld got it\n", clock, n);
			hold(0.250);
		fprintf(fp, "%8.3lf job.%ld deallocate\n", clock, n);
		s.deallocate(1);
		}
}
