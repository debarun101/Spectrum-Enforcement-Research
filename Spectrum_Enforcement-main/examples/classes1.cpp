// another examples using classes

#include "cpp.h"
#include <stdio.h>

FILE *fp;

#define ACCUR 0.01
#define CONF_LEV 0.95
#define MAX_RUN_TM 100.0


#define N 3
#define K 3

facility_set *f;
facility *term;
process_class* c[K];
box *resp_tm;

double  serv_tm[K][N] =
	{ { 0.1, 0.2, 0.3 },
	  { 0.4, 0.8, 0.0 },
	  { 0.6, 0.0, 1.2 } };

double term_tm[K] = {5.0, 10.0, 15.0};

double branch_prob[K][N][N] =
	{ { {0.5, 0.3, 0.2}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0} },
	  { {0.1, 0.9, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0} },
	  { {0.2, 0.0, 0.8}, {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0} } };

long n[K] = {2, 3, 4};

char str[48];

void init();
void job(long);
void print_parms();
void make_class_report();

extern "C" void sim()
{
	long i, k;
    
	init();
	create("sim");
	print_parms();
	for(k = 0; k < K; k++)
		for(i = 0; i < n[k]; i++)
			job(k);
	converged.wait();
	report();
	make_class_report();
	mdlstat();
}

void init()
{
	long k;

    fp = fopen("xxx.out", "w");
    set_output_file(fp);
	f = new facility_set("f", N);
	term = new facility("term");
	for(k = 0; k < K; k++) {
		sprintf(str, "c%d", k);
		c[k] = new process_class(str);
		}
	collect_class_facility_all();
	resp_tm = new box("resp tm");
	resp_tm->time_run_length(ACCUR, CONF_LEV, MAX_RUN_TM);
}

void job(long k)
{
	long i, j;
	long select(long, long);
	TIME ts;

	create("job");
	c[k]->set_process_class();
	ts = resp_tm->enter();
	j = 0;
	while(1) {
		i = j;
		(*f)[i].use(serv_tm[k][i]);
		j = select(i, k);
		if(j == 0) {
			resp_tm->exit(ts);
			term->use(term_tm[k]);
			ts = resp_tm->enter();
			}
	};
}

long select(long i, long k)
{
	long j;
	double x, y;

	x = prob();
	j = 0;
	y = branch_prob[k][i][j];
	while(y < x) {
		j++;
		y += branch_prob[k][i][j];
		}
	return(j);
}

void print_parms()
{
	long i, j, k;

	fprintf(fp, "service times\n");
	for(k = 0; k < K; k++) {
		fprintf(fp, "class %ld: ", k);
		for(i = 0; i < N; i++)
			fprintf(fp, "%5.3f ", serv_tm[k][i]);
		fprintf(fp, "\n");
		}
	fprintf(fp, "\nbranching probabilities\n");
	for(k = 0; k < K; k++) {
		fprintf(fp, "class %d:\n", k);
		for(i = 0; i < N; i++) {
			fprintf(fp, "from dev[%ld] to: ", i);
			for(j = 0; j < N; j++)
				fprintf(fp, "%5.3f ", branch_prob[k][i][j]);
			fprintf(fp, "\n");
			}
		}
	fprintf(fp, "\nclass populations: ");
	for(k = 0; k < K; k++)
		fprintf(fp, "%5ld ", n[k]);
	fprintf(fp, "\n");
	fprintf(fp, "\nterminal delalys: ");
	for(k = 0; k < K; k++)
		fprintf(fp, "%6.3f ", term_tm[k]);
	fprintf(fp, "\n");
}

void make_class_report()
{
	long k, ct;

	fprintf(fp, "\n\tClass Report\n");
	for(k = 0; k < K; k++) {
		fprintf(fp, "%ld %s\n", c[k]->id(), c[k]->name());
		ct = c[k]->cnt();
		fprintf(fp, "    number of instances     %ld\n", ct);
		fprintf(fp, "    hold count per instance %.3f\n", (double) c[k]->holdcnt()/ct);
		fprintf(fp, "    life time per instance  %.3f\n", c[k]->lifetime()/ct);
		fprintf(fp, "    hold time per instance  %.3f\n", c[k]->holdtime()/ct);
		fprintf(fp, "    wait time per instance  %.3f\n",
			(c[k]->lifetime() - c[k]->holdtime())/ct);
		fprintf(fp, "\n");
		}
}
