// jockeying example - move customer from longer queue to shorter queue
//					 - uses process structure to hold facility index
// this is based on an example in Simulation Modeling and Analysis (2nd ed.),
//	 by Law & Kelton, McGraw-Hill, p. 170

#include "cpp.h"
#include <math.h>
#include <stdio.h>
FILE *fp;

const long NUM_TELLERS = 5;
const TIME iarrTm = 0.25;
const TIME srvTm = 3.5;
//const TIME simTime = 1*20.0;
const long numCust = 15;

facility* teller[NUM_TELLERS];		// array of tellers
long procId;
event *done;
long numAct;

void customer(long);
void qmanager(long);
void snapShot(char* s);
void gen();
void init();
long select_shortest_queue();

extern "C" void sim()
{
	create("sim");
	init();
	gen();
	done->wait();
	report();
}

void init()
{
	long i;
	char str[32];

	fp = fopen("jock.out", "w");
	set_output_file(fp);
	for(i = 0; i < NUM_TELLERS; i++) {
		sprintf(str, "teller%ld", i);
		teller[i] = new facility(str);
	}
	procId = 1;
	done = new event("done");
	numAct = numCust;
}

void gen()					// generate arriving customers
{
	long i;

	create("gen");
	for(i = 0; i < numCust; i++) {
		customer(procId++);
		hold(exponential(iarrTm));
	}
}

void customer(long id)		// customer process
{
	long n, n1;
	char str[16];

	sprintf(str, "cust.%ld", id);
	create(str);
	snapShot("arrival");
	n = select_shortest_queue();	// select shortest queeu
	set_this_struct((void*)n);
	fprintf(fp, "%8.3lf: process %s selects teller[%ld]\n", clock, process_name(), n);
	teller[n]->reserve();
	n1 = (long)get_this_struct();	// check current queue id
	if(n1 != n)
		fprintf(fp, "%8.3lf: %s has moved to teller[%ld]\n", clock, process_name(), n1);
	hold(exponential(srvTm));
	teller[n1]->release();
	fprintf(fp, "%8.3lf: %s depart teller[%ld]\n", clock, process_name(), n1);
	snapShot("depart");
	qmanager(n1);					// call qmanager on depart
	if(--numAct == 0)
		done->set();
}

long select_shortest_queue()		// return index of facility with shortest queue
{
	long i, min_qlen, ql, n;

	n = 0;
	min_qlen = teller[n]->num_busy() + teller[n]->qlength();
	for(i = 1; i < NUM_TELLERS; i++) {
		ql = teller[i]->num_busy() + teller[i]->qlength();
		if(ql < min_qlen) {
			min_qlen = ql;
			n = i;
		}
	}
	return n;
}

void snapShot(char *s)		// snapshot and print status of facilities in array
{
	long i;
	process_t pp;

	fprintf(fp, "%8.3lf: teller status %s\n", clock, s);
	for(i = 0; i < NUM_TELLERS; i++) {
		fprintf(fp, "%s (%ld):", teller[i]->name(), teller[i]->num_busy());
		pp = teller[i]->first_process();
		while(pp != NIL) {
			fprintf(fp, " %s,", pp->name());
			pp = pp->next();
		}
		fprintf(fp, "\n");
	}
}

void qmanager(long i)		// queue manager routine 
{
	long j, dis, mindis, n, numi, numj;
	process_t pp;

	n = -1;
	mindis = NUM_TELLERS + 1;
	numi = teller[i]->num_busy() + teller[i]->qlength(); 
	for(j = 0; j < NUM_TELLERS; j++) {	// search for nearest longer queue
		if(j != i) {
			numj = teller[j]->num_busy() + teller[j]->qlength();
			dis = abs(i-j);
			if((numj > numi+1) && (dis < mindis)) {
				n = j;
				mindis = dis;
			}
		}
	}
	if(n != -1) {
		pp = teller[n]->last_process();
		if(pp != NIL) {				// remove process from queue n and move to queue i
			teller[n]->remove_process(pp);
			pp->set_struct((void*)i);		// set current queue id
			teller[i]->insert_process(pp);
			fprintf(fp, "%8.3lf: last customer at teller[%ld] jockeys to teller[%ld]\n",
				clock, n, i);
		}
	}

}
