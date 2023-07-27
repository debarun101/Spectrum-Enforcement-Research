// example using qhistogram

#include "cpp.h"
#include <stdio.h>

#define N 1000

facility f("f");
qtable *ql;
FILE *fp;

void job();
void make_qhist(qtable*);

extern "C" void sim()
{
	int i;

	fp = fopen("qhist.out", "w");
	set_output_file(fp);
	create("sim");
	ql = new qtable("qlength");
	ql->add_histogram(10, 0, 10);
	for(i = 0; i < N; i++) {
		hold(expntl(2.0));
		job();
		}
	report_hdr();
	make_qhist(ql);
	report_qtables();
}

void job()
{
	create("job");

	ql->note_entry();
		f.use(expntl(1.5));
	ql->note_exit();
}

void make_qhist(qtable* q)

{
	long i;
	long num, cnt;
	double  b_cnt, sum;
	double e_time;
	char str[8];

	fprintf(fp, "\n\tqhistogram: %s\n", q->name());
	num = q->hist_num();
	cnt = q->cnt();
	e_time = clock;
	fprintf(fp, "elapsed time      %.3f\n", e_time);
	fprintf(fp, "current number    %ld\n", q->cur());
	fprintf(fp, "mean queue length %.3f\n", q->qlen());
	fprintf(fp, "mean queue time   %.3f\n", q->qtime());
	fprintf(fp, "max queue length  %ld\n", q->max());
	fprintf(fp, "number of entries %d\n", cnt);
	fprintf(fp, "number of buckets %d\n", num);
	sum = 0;
	for(i = 0; i <= num+1; i++) {
		b_cnt = q->hist_bucket(i);
		sum += b_cnt;
		if(i < num+1)
			sprintf(str,"%5ld", i-1);
		else
			sprintf(str, ">= %ld", i-1);
		fprintf(fp, "%5s\t%.3lf\t%.2lf\t%.3lf\n",
			str, b_cnt, b_cnt/e_time, sum/e_time);
		}
	fprintf(fp, "sum = \t%.3lf\n", sum);
}

