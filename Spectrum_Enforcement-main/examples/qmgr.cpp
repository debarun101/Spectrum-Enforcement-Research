// queue management routines 

#include "cpp.h"
#include <stdio.h>
FILE *fp;

#define numJobs 10

void init();
void gen();
void print_queue(facility* f);
void reorder_queue(facility* f);
void reorder_queue_again(facility *f);
facility *f;
event *go;
event *done;
long nact;
void job(long n);

extern "C" void sim()
{
	create("sim");
	init();
	gen();
	hold(10.0);
	print_queue(f);
	reorder_queue(f);
	print_queue(f);
	reorder_queue_again(f);
	print_queue(f);
	done->wait();
	report();
}

void init()
{
	fp = fopen("qmgr.out", "w");
	set_output_file(fp);
	set_trace_file(fp);
//	trace_on();
	f = new facility("f");
	go = new event("go");
	done = new event("done");
	nact = numJobs;
}

void gen()
{
	long i;

	for(i = 1; i <= numJobs; i++)
		job(i);
}

void job(long n)
{
	char str[32];

	sprintf(str, "job.%ld", n);
	create(str);
	set_priority(n);
	f->reserve();
	hold((double)n);
	f->release();
	if(--nact == 0)
		done->set();
}

void print_queue(facility* fac)
{
	process_t pp;
	long n;
	long pri, id;

	fprintf(fp, "        pcb  priority\n");
	n = 0;
	pp = fac->first_process();
	while(pp != NIL) {
		pri = pp->priority();
		id = pp->identity();
		fprintf(fp, "%3ld id %2ld, pri %2ld\n", n, id, pri);
		n++;
		pp = pp->next();
	}
	fflush(fp);
}

void reorder_queue(facility* fac)
{
	long i;
	process_t xpp;
	process_t pp[numJobs];

	for(i = 0; i < numJobs; i++) {
		xpp = fac->first_process();
		pp[i] = fac->remove_process(xpp);
	}
	for(i = 0; i < numJobs; i++)
		pp[i]->set_priority(100 - pp[i]->priority());
	for(i = 0; i < numJobs; i++)
		fac->insert_process(pp[i]);
}

void reorder_queue_again(facility *fac)
{
	process_t xpp, ypp;
	long n;

	n = 0;
	xpp = fac->first_process();
	while(n < numJobs) {
		ypp = fac->remove_process(xpp);
		xpp = xpp->next();
		ypp->set_priority(1000 + ypp->priority() + 2*n);
		fac->insert_process(ypp);
		n++;
	}
}

