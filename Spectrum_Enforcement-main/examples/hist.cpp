// example of using lots of histogram functions

#include "cpp.h"
#include <stdio.h>
FILE *fp;

table h1("h1", 10l, 0.0, 10.0);

void report_histogram(table*);

extern "C" void sim()
{
	table* h2;
	long i; double x;

	fp = fopen("hist.out","w");
	set_output_file(fp);
	create("sim");
	h2 = new table("h2", 10l, 0.0, 10.0);
	for(i = 0; i < 1000; i++) {
		x = expntl(1.0);
		h1.record(x);
		h2->record(x);
		}
	report_tables();
	report_histogram(&h1);
	report_histogram(h2);
}

void report_histogram(table* h)
{
	long i, n, sum;
	double low, width, b;

	n = h->hist_num();
	sum = h->cnt();
	low = h->hist_low();
	width = h->hist_width();

	fprintf(fp, "\n\thistogram %s\n", h->name());
	fprintf(fp, "number of buckets %d\n", n);
	fprintf(fp, "value of lowest   %.3f\n", low);
	fprintf(fp, "value of highest  %.3f\n", h->hist_high());
	fprintf(fp, "width of bucket   %.3f\n", width);
	fprintf(fp, "number of entries %d\n", sum);
	b = low;
	for(i = 0; i < n+1; i++) {
		fprintf(fp, "number < %.3f = %ld (%.3f %%)\n", 
			b, h->hist_bucket(i), 100.0*h->hist_bucket(i)/sum);
		b += width;
		}
	fprintf(fp, "number >= %.3f = %ld (%.3f %%)\n",
		h->hist_high(), h->hist_bucket(n+1), 100.0*h->hist_bucket(n+1)/sum);
}
