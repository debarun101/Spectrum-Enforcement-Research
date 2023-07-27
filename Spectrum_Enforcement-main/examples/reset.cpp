// illustrate the reset statement

#include "cpp.h"
#include <stdio.h>
FILE *fp;

#define NUM_SERVERS 2l
#define WARM_UP 100.0
#define BATCH  1000.0
#define NUM_BATCHES 10

facility_ms f("f", NUM_SERVERS);
table t("t");
permanent_table batch_means("batch_means");

void  generator();

extern "C" void sim()
{
	int ibatch;
	char str[36];

	fp = fopen("reset.out", "w");
	set_output_file(fp);
	create("sim");
	generator();
	hold(WARM_UP);
	reset();
	for(ibatch = 1; ibatch <= NUM_BATCHES; ibatch++) {
		hold(BATCH);
		sprintf(str, "batch %d", ibatch);
		set_model_name(str);
		batch_means.record(t.mean());
		report();
		reset();
		}
	batch_means.report();
}

void generator()
{
	void job();

	create("generator");
	do {
		hold(expntl(1.0));
		job();
	} while(1);
}

void job()
{
	TIME t1;
	
	create("job");
	t1 = clock;
	f.use(uniform(0.5, 1.5));
	t.record(clock - t1);
}
