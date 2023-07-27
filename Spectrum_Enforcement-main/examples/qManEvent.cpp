//	qmanage event

#include "cpp.h"
#include <stdio.h>
#include <stdio.h>
FILE *fp;

const double simTime = 100.0;
const long numProcs1 = 5;
const long numProcs2 = 6;

event *ev;

void init();
void gen();
void proc1(long n);
void proc2(long n);
void proc3();
void eventListProcs(event *e);
void reorderWait(event *e);
void reorderQueue(event *e);

extern "C" void sim()
{
	init();
	create("sim");
	gen();
	hold(simTime);
}

void init()
{
	fp = fopen("qmanEvent.out", "w");
	set_output_file(fp);
	ev = new event("ev");
}

void gen()
{
	long i;

	for(i = 1; i <= numProcs1; i++)
		proc1(i);
	for(i = 1; i <= numProcs2; i++)
		proc2(i);
	proc3();
}

void proc1(long n)
{
	create("proc1");
	set_priority(n);
	hold(1.0);
	ev->wait();
	fprintf(fp, "%8.3lf proc1.%ld id %ld pr %ld done\n",
		clock, n, identity(), priority());
}

void proc2(long n)
{
	create("proc2");
	set_priority(n);
	hold(1.0);
	ev->queue();
	fprintf(fp, "%8.3lf proc2.%ld id %ld pr %ld done\n",
		clock, n, identity(), priority());
}

void proc3()
{
	long i;

	create("proc3");
	hold(2.0);
	eventListProcs(ev);
	reorderWait(ev);
	reorderQueue(ev);
	eventListProcs(ev);
	ev->set();
	for(i = 2; i <= numProcs2; i++) {
		hold(1.0);
		ev->set();
	}
}

void eventListProcs(event *e)
{
	process_t pp;

	fprintf(fp, "%8.3lf event %s:\n", clock, e->name());
	fprintf(fp, "  wait list:\n");
	pp = ev->first_wait_process();
	while(pp != NIL) {
		fprintf(fp, "%s.%ld pr %ld\n", pp->name(), pp->identity(), pp->priority());
		pp = pp->next();
	}
	fprintf(fp, "  queue list:\n");
	pp = ev->first_queue_process();
	while(pp != NIL) {
		fprintf(fp, "%s.%ld pr %ld\n", pp->name(), pp->identity(), pp->priority());
		pp = pp->next();
	}
}

void reorderWait(event *e)
{
	process_t pp;
	process_t *ppArr;
	long n, numProcs;

	numProcs = e->wait_cnt();
	ppArr = new process_t[numProcs];
	n = 0;
	pp = e->first_wait_process();
	while(pp != NIL) {
		if(n >= numProcs) {
			printf("reorder wait error\n");
			exit(1);
		}
		ppArr[n++] = e->remove_wait_process(pp);
		pp = pp->next();
	}
	for(n = 0; n < numProcs; n++) {
		ppArr[n]->set_priority(n);
		e->insert_wait_process(ppArr[n]);
	}
	delete ppArr;
}

void reorderQueue(event *e)
{
	process_t pp;
	process_t *ppArr;
	long n, numProcs;

	numProcs = e->queue_cnt();
	ppArr = new process_t[numProcs];
	n = 0;
	pp = e->first_queue_process();
	while(pp != NIL) {
		if(n >= numProcs) {
			printf("reorder queue error\n");
			exit(1);
		}
		ppArr[n++] = e->remove_queue_process(pp);
		pp = pp->next();
	}
	for(n = 0; n < numProcs; n++) {
		ppArr[n]->set_priority(n);
		e->insert_queue_process(ppArr[n]);
	}
	delete ppArr;
}
