// example 11 - exception handling

#include "cpp.h"
#include <stdio.h>
FILE *fp;

const long numJobs = 100;
const TIME iarTime = 1.0;
const TIME srvTime = 0.5;

event *done;
facility *fac;

void init();
void myError(long n);
void gen();
void cust();

extern "C" void sim()
{
	create("sim");
	init();
	try {
		gen();
		done->wait();
	}
	catch(char *str) {
		printf("%.3lf: catch error %s\n", clock, str);
		report();
	}
}

void init()
{
	fp = fopen("except.out", "w");
	set_output_file(fp);
	done = new event("done");
	fac = new facility("fac");
	set_err_handler(myError);
}

void gen()
{
	long i;

	create("gen");
	for(i = 1; i <= numJobs; i++) {
		cust();
		hold(exponential(iarTime));
	}
}

void cust()
{
	create("cust");
	fac->use(exponential(srvTime));
}

char myMsg[128];

void myError(long n)
{
	sprintf(myMsg, "error #%ld: %s", n, csim_error_msg(n));
	throw myMsg;
}
