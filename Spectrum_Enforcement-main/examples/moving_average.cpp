// moving average 

#include "cpp.h"
FILE *fp;

#define NSAMPLES 1000

table t1("moving", 10, 1.0, 1001.0);
table t2("summary");
int nact;

extern "C" void sim()
{
	int i, tc;

	fp = fopen("moving_average.out", "w");
	set_output_file(fp);
	t1.set_moving(100);
	for(i = 1, tc = 1; i <= NSAMPLES; i++, tc++) {
		t1.record((float) i);
		t2.record((float) i);
		if(tc == 100) {
			report_tables();
			tc = 0;
			}
		}
	fprintf(fp, "\nTable %s is a %ld point moving window table\n",
		t1.name(), t1.moving_window());
	fprintf(fp, "Table %s is a %ld point moving window table\n",
		t2.name(), t2.moving_window());
}
