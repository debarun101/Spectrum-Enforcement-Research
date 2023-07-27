//	test removing and restarting processes at facilities

#include "cpp.h"
#include <stdio.h>
FILE *fp;

const long numJobs = 5;
const double srvTime = 100.0;

class jobInfo_c {
protected:
	long serverId;
public:
	jobInfo_c(long si)		{serverId = si;}
	void setServerId(long si){serverId = si;}
	long getServerId()		{return serverId;}
};

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
	hold(1.0);
	removeJob();
	hold(1.0);
	removeJob();
	done->wait();
	report();
}

void init()
{
	fp = fopen("qManFac.out", "w");
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
	long si, si1;
	jobInfo_c *jobInfo;

	create("job");
	set_priority(n);
	fprintf(fp, "%8.3lf job.%ld reserve\n", clock, identity());
	jobInfo = new jobInfo_c(0);
	set_this_struct(jobInfo);
	si = fac->reserve();
	si1 = jobInfo->getServerId();
	fprintf(fp, "%8.3lf job.%ld end reserve, si %ld, si1 %ld\n",
		clock, identity(), si, si1);
	if(si1 != -1) {
		hold(exponential(srvTime));
		fprintf(fp, "%8.3lf job.%ld release\n", clock, identity());
		fac->release();
	}
	if(--numAct == 0)
		done->set();
}

void removeJob()
{
	process_t pp;
	jobInfo_c *jobInfo;

	pp = fac->first_process();
	if(pp != NULL) {
		fac->remove_process(pp);
		fprintf(fp, "%8.3lf remove %s.%ld\n", clock, pp->name(), pp->identity());
		jobInfo = (jobInfo_c*)pp->get_struct();
		jobInfo->setServerId(-1);
		pp->restart();
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
