//	test mailbox receive any

#include "cpp.h"
#include <stdio.h>
FILE *fp;

#define numMboxes 2
const long numSenders = 1;
const long numReceivers = 4;

const double simTime = 6.1;
const double rcvDelay = 1.0;
const double sndDelay = 4.0;

mailbox_set *mbs;
event *done;
long numAct;

void init();
void gen();
void sender(long i);
void receiver(long i);

extern "C" void sim()
{
	create("sim");
	init();
	gen();
	hold(simTime);
	done->wait();
	report();
}

void init()
{
	long i;

	fp = fopen("receiveAny.out", "w");
	set_output_file(fp);
	set_trace_file(fp);
//	trace_on();
	mbs = new mailbox_set("mbs", numMboxes);
	mbs->monitor_event();
	for(i = 0; i < numMboxes; i++)
		(*mbs)[i].monitor();
	done = new event("done");
	numAct = 0;
}

void gen()
{
	long i;

	for(i = 0; i < numReceivers; i++) {
		numAct++;
		receiver(i);
	}
	for(i = 0; i < numSenders; i++) {
		sender(i);
	}
}

void sender(long n)
{
	long i, msg;

	create("sender");
	set_priority(2);
	while(clock < simTime) {
		for(i = 0; i < numMboxes; i++) {
			msg = n*numMboxes + i;
			fprintf(fp, "%8.3lf snd%ld send %ld to mbs[%ld]\n",
				clock, n, msg, i);
			(*mbs)[i].send(msg);
			fprintf(fp, "%8.3lf mbs %s contains %ld msgs\n",
				clock, mbs->name(), mbs->num_msgs());
		}
		hold(sndDelay);
	}
}

void receiver(long n)
{
	long msg, st;

	create("recvr");
	do {
		hold(rcvDelay);
		st = mbs->receive_any(&msg);
		fprintf(fp, "%8.3lf rcv%ld receives %ld in mbs[%ld]\n",
			clock, n, msg, st);
		fprintf(fp, "%8.3lf msb %s has %ld messages\n",
			clock, mbs->name(), mbs->num_msgs());
	} while(mbs->num_msgs() > 0);
	numAct--;
	done->set();
}

