// timed_reserve

#include "cpp.h"
FILE *fp;

facility f("f");
event done("done");

#define RUNTIME 10.0
#define IARTM 1.0
#define SERVTM 2.0
#define TIME_LIMIT 1.0

int num_act;
int num_rej;

void call();

extern "C" void sim()
{
	fp = fopen("tmd_res.out", "w");
	set_output_file(fp);
	create("sim");
	num_rej = 0;
	num_act = 0;
	while(clock < RUNTIME) {
		num_act++;
		call();
		hold((IARTM));
		}
	done.wait();
	report();
	fprintf(fp, "\nnum rej %d\n", num_rej);
}

void call()
{
	create("call");
	if(f.timed_reserve(TIME_LIMIT) != TIMED_OUT) {
		hold(SERVTM);
		f.release();
		}
	else {
		num_rej++;
		}
	if(--num_act == 0 && clock >= RUNTIME)
		done.set();
}
