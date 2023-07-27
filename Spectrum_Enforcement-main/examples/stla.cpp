#include <stdio.h>
#include <vector>

// Defining STL will rename some CSIM functions that have
// a name conflict with STL classes. For example, the
// CSIM alloc function will be renamed to csim_alloc,
// and the name 'alloc' will be #define'ed to csim_alloc.
#define STL
#include "cpp.h"

const long numFac = 2;
const double iarTime = 2.0;
const double srvTime = 3.0;
const double simTime = 100000.0;

using namespace std;

vector<csim_facility*> fac(numFac);

void initRun();
void gen();
void job();
void initIO();


extern "C" void sim()
{
	initIO();
	create("sim");
	initRun();
	gen();
	hold(simTime);
	report();
}

void gen()
{
	create("gen");
	while(clock < simTime) {
		hold(exponential(iarTime));
		job();
	}
}

void job()
{
	long n;

	create("job");
	n = uniform_int(0, numFac-1);
	fac[n]->use(exponential(srvTime));
}


void initRun()
{
	long i;
	char str[32];

	for(i = 0; i < numFac; i++) {
		sprintf(str, "fac%ld", i);
		fac[i] = new csim_facility(str);
	}
}


FILE *fp;

void initIO()
{
	fp = fopen("stla.out", "w");
	set_output_file(fp);
	set_trace_file(fp);
}
