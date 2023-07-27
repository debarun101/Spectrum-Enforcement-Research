//	test removing and restarting processes at facilities

#include "cpp.h"
#include <stdio.h>
FILE *fp;

const long numJobs = 5;
const double srvTime = 100.0;

long numAct;
event *done;

facility *fac;

void init();
void gen();
void job(long n);
void reportQueue();
void removeJob();

extern "C" void sim()
{
	create("sim");
	init();
	gen();
	hold(srvTime/3);
	removeJob();
	hold(srvTime/3);
	removeJob();
	done->wait();
	report();
}

void init()
{
	fp = fopen("qManFac2.out", "w");
	set_output_file(fp);
	fac = new facility("fac");
	done = new event("done");
	numAct = numJobs;
}

void gen()
{
	long i;

	for(i = 0; i < numJobs; i++) {
		job(i);
	}
}

void job(long n)
{
	long srvrIndx;

	create("job");
	set_priority(n);
	fprintf(fp, "%8.3lf job.%ld reserve\n", clock, identity());
	srvrIndx = fac->reserve();
	fprintf(fp, "%8.3lf job.%ld end reserve, srvrIndx %ld\n",
		clock, identity(), srvrIndx);
	if(srvrIndx != -1) {
		hold(srvTime+n);
		fprintf(fp, "%8.3lf job.%ld release\n", clock, identity());
		fac->release();
	}
	if(--numAct == 0)
		done->set();
}

void removeJob()
{
	process_t pp;

	create("remove");
	pp = fac->first_process();
	if(pp != NULL) {
		fac->remove_process(pp);
		fprintf(fp, "%8.3lf %s.%ld removed\n", clock, pp->name(), pp->identity());
		hold(srvTime/4);
		pp->set_priority(1);
		fac->insert_process(pp);
		fprintf(fp, "%8.3lf %s.%ld inserted\n", clock, pp->name(), pp->identity());
	}
	reportQueue();
}

void reportQueue()
{
	process_t pp;

	fprintf(fp, "%s queue report\n", fac->name());
	pp = fac->first_process();
	while(pp != NULL) {
		fprintf(fp, "%s.%ld, priority %ld\n", pp->name(), pp->identity(), pp->priority());
		pp = pp->next();
	}
}
