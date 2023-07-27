// example 11 - exception handling

#include "cpp.h"
#include <stdio.h>
FILE *fp;

const long numJobs = 100000;
const TIME iarTime = 1.0;
const TIME srvTime = 0.8;
const double prob1 = 0.8;
const double prob2 = 0.8;
const double prob3 = 0.8;
const double probFailure = 0.000025;

event *done;
facility *fac;
facility *fac1;
facility *fac2;

void init();
void myError(long n);
void gen();
void cust();
void cust1();
void cust2();
void cust3();
extern "C" void sim();

int main()
{	
	long i;

	fp = fopen("except.out", "w");
	set_output_file(fp);
	for(i = 0; i < 3; i++) {
		printf("%2ld. start sim\n", i);
		sim();
		printf("    end sim\n");
	}

	return 0;
}

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
		fprintf(fp, "%.3lf: catch error %s\n", clock, str);
	}
	report();
	rerun();
}

void init()
{
	done = new event("done");
	fac = new facility("fac");
	fac1 = new facility("fac1");
	fac2 = new facility("fac2");
	set_err_handler(myError);
	reseed(NIL, 1);
}

void gen()
{
	long i;

	create("gen");
	for(i = 1; i <= numJobs; i++) {
		if(i % 2 == 0)
			cust();
		else
			cust1();
		hold(exponential(iarTime));
	}
	done->set();
}

void cust()
{
	create("cust");
	hold(exponential(iarTime));
	if(uniform(0.0, 1.0) < prob1)
		cust1();
	else
		cust3();
	fac->use(exponential(srvTime));
}

void cust1()
{
	long arr[64];

	create("cust1");
	if(uniform(0.0, 1.0) < prob2)
		cust2();
	fac1->use(exponential(srvTime));
}

void cust2()
{
	long arr[1];

	create("cust2");
	hold(exponential(iarTime));
	if(uniform(0.0, 1.0) < probFailure)
		hold(-1.0);
	fac2->use(exponential(srvTime));
}

void cust3()
{
	long arr[1024];

	create("cust3");
	hold(exponential(iarTime));
	if(uniform(0.0, 1.0) < probFailure)
		hold(-1.0);
	fac2->use(exponential(srvTime));
}

char myMsg[128];

void myError(long n)
{
	sprintf(myMsg, "error #%ld: %s", n, csim_error_msg(n));
	throw myMsg;
}
