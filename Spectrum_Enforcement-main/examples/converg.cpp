// convergence demo

#include "cpp.h"
#include <stdio.h>

const long numRuns = 3;
const double iarrTm = 1.0;
const double servTm = 0.75;
const double simTm = 2000.0; 

permanent_histogram *respTm;
table *rrTm;
facility *que;

void init();
void run_init();
void generator();
void customer();
FILE *fp;


extern "C" void sim()
{
	long runNum;
	
	init();
	for(runNum = 1; runNum <= numRuns; runNum++) {
		create("sim");
		run_init();
		generator();
		converged.wait();
		respTm->record(rrTm->mean());
		report_hdr();
		rrTm->report();
		rerun();
		}
	respTm->report();
}

void init()
{
	fp = fopen("xxx.out", "w");
	set_output_file(fp);
	respTm = new permanent_histogram("resp tm", 20L, 0.0, 20.0);
	respTm->confidence();
}

void run_init()
{
	que = new facility("queue");
	rrTm = new table("run resp tm");
	rrTm->run_length(0.05, 0.90, simTm);
}

void generator()
{
	create("gen");
	while(1) {
		hold(expntl(iarrTm));
		customer();
		}
}

void customer()
{
	TIME t;
	create("cust");
	t = clock;
	que->use(expntl(servTm));
	rrTm->record(clock - t);
}

		
