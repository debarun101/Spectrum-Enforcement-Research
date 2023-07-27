// test random number streams

#include "cpp.h"
#include <stdio.h>

stream s1(1);
stream s2(NIL);

table t1("t1", 10L, 0.0, 5.0);
table t2("t2", 10L, 0.0, 5.0);

FILE *fp;

extern "C" void sim()
{
	int i;
	double x1, x2;
	
	create("sim");
	fp = fopen("xxx.out", "w");
	set_output_file(fp);
	t2.confidence();
	for(i = 0; i < 10000; i++) {
		x1 = s1.exponential(1.0);
		x2 = s2.exponential(1.0);
		t1.record(x1);
		t2.record(x2);
		}
	report_tables();
}
