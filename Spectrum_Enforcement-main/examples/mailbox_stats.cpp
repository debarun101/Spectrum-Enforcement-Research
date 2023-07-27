//	mailbox_stats.cpp

#include "cpp.h"
#include <stdio.h>
FILE *fp;

const long NUM_MBS = 2;
const double simTime = 6.0;
const double warmup = simTime/2;
const long numSenders = 2;
const long numReceivers = 2;
const double receiveDelay = 0.0;
const double sendDelay = 1.0;

mailbox **mbs;
event *done;
long numAct;

void init();
void gen();
void sender(long n);
void receiver(long n);

extern "C" void sim()
{
	create("sim");
	init();
	gen();
	hold(warmup);
	reset();
	done->wait();
	report();
}

void init()
{
	long i;
	char str[32];

	fp = fopen("mailbox_stats.out", "w");
	set_output_file(fp);
	set_trace_file(fp);
//	trace_on();
	mbs = new mailbox*[NUM_MBS];
	for(i = 0; i < NUM_MBS; i++) {
		sprintf(str, "mb%ld", i);
		mbs[i] = new mailbox(str);
		mbs[i]->monitor();
	}
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

void receiver(long n)
{
	long msg;

	create("receiver");
	while(clock < simTime) {
		mbs[n]->receive(&msg);
		hold(receiveDelay);
	}
	numAct--;
	if(numAct == 0)
		done->set();
}

void sender(long n)
{
	long msg;

	create("sender");
	msg = 1;
	while(clock < simTime) {
		hold(sendDelay);
		mbs[n]->send(msg);
	}
}
