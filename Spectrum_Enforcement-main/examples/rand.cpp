// test random numberss and streams

#include "cpp.h"
#include <stdlib.h>
#include <stdio.h>

#define NUM_SAMPLES 10000
#define MEAN 1.0
#define MAX_DISTR 10.0
#define NUM_ENTRIES 10L
#define NUM_PROBS 5L
#define UNIF_LOW 0.1
#define UNIF_HIGH 10000.0

table *exp_dist;
table *empir_dist;
table *unif_dist;
double probv[NUM_PROBS + 1] = {0.3, 0.2, 0.1, 0.1, 0.3, 0.0};
double values[NUM_PROBS + 1] = {100.0, 300, 500, 700.0, 900.0, 0.0};
double cutoff[NUM_PROBS+1];
long alias[NUM_PROBS+1];
FILE *fp;

void init(int, char*[]);
void gen();

long num_samples;
extern "C" long _qsize;

extern "C" void sim(int argc, char *argv[])
{
	create("sim");
	init(argc, argv);
	gen();
	converged.wait();
	report();
	mdlstat();
}

void gen()
{
	long i;
	
	create("gen");
	i = 0;
	while(++i < num_samples)  {
		exp_dist->record(exponential(MEAN));
		empir_dist->record(empirical(NUM_PROBS, cutoff, alias, values));
		unif_dist->record(uniform(UNIF_LOW, UNIF_HIGH));
		hold(0.0);
		}
}

void init(int argc, char* argv[])
{
	fp = fopen("xxx.out", "w");
	set_output_file(fp);
	num_samples = NUM_SAMPLES;
	if(argc >= 2) {
		num_samples = atoi(argv[1]);
		}
	exp_dist = new table("exponential");
	exp_dist->add_histogram(NUM_ENTRIES, 0.0, MAX_DISTR);
	empir_dist = new histogram("empirical", NUM_ENTRIES, 100.0, 1100.0);
	setup_empirical(NUM_PROBS, probv, cutoff, alias);
	empir_dist->confidence();
	unif_dist = new histogram("uniform", 10L, UNIF_LOW, UNIF_HIGH);
	unif_dist->confidence();
	unif_dist->run_length(0.05, 0.95, 10000.0);
}
			
