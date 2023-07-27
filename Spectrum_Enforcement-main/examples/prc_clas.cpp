// process classes and current class

#include "cpp.h"
FILE *fp;

#define N 3l
#define NJ 9l

process_class *cl[N];

void job(long);

extern "C" void sim()
{
	long i;
	char str[12];

	fp = fopen("prc_clas.out", "w");
	set_output_file(fp);
	create("sim");
	for(i = 0; i < N; i++) {
		sprintf(str, "class.%ld", i);
		cl[i]  = new process_class(str);
		}
	for(i = 0; i < NJ; i++)
		job(i);
	event_list_empty.wait();
	report();
}

void job(long n)
{
	long c;

	create("job");
	c = random(0, N-1);
	cl[c]->set_process_class();
	fprintf(fp, "%10.3f - start job.%ld: c = %ld, class = %s\n",
		clock, n, c, current_class()->name());
	hold(1.0);
	fprintf(fp, "%10.3f - end job.%ld: c = %ld, class = %s\n",
		clock, n, c, current_class()->name());
}
