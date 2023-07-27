//	qmanMailbox.cpp - test qman mailbox routines

#include "cpp.h"
#include <stdio.h>
#include <stdlib.h>
FILE *fp;

const double simTime = 100.0;
const long numSenders = 1;
const long numReceivers = 5;

mailbox *mb;

void init();
void gen();
void sender(long);
void receiver(long);
void listMailboxProcs(mailbox *m);
void reorderMailboxProcList(mailbox *m);

extern "C" void sim()
{
	init();
	create("sim");
	gen();
	hold(simTime);
}

void init()
{
	fp = fopen("qmanMailbox.out","w");
	set_output_file(fp);
	mb = new mailbox("mb");
}

void gen()
{
	long i;

	for(i = 1; i <= numSenders; i++)
		sender(i);
	for(i = 1; i <= numReceivers; i++)
		receiver(i);
}

void sender(long n)
{
	long i;

	create("sender");
	hold(1.0);
	listMailboxProcs(mb);
	reorderMailboxProcList(mb);
	listMailboxProcs(mb);
	for(i = 1; i <= numReceivers; i++) {
		fprintf(fp, "%8.3lf sender.%ld id %ld sends %ld\n",
			clock, n, identity(), i);
		mb->send(i);
	}
}

void receiver(long n)
{
	long msg;

	create("recvr");
//	set_priority(numReceivers - n + 1);
	set_priority(n);
	mb->receive(&msg);
	fprintf(fp, "%8.3lf receiver.%ld id %ld received %ld\n",
		clock, n, identity(), msg);
}

void reorderMailboxProcList(mailbox *m)
{
	 process_t pp;
	 process_t *ppArr;
	 long numProcs, n;

	 numProcs = m->queue_cnt();
	 ppArr = (process_t*)do_malloc(sizeof(process_t)*numProcs);
	 pp = m->first_process();
	 n = 0;
	 while(pp != NIL) {
		 if(n >= numProcs) {
			 printf("mailbox reorder error\n");
			 exit(1);
		 }
		 ppArr[n++] = m->remove_process(pp);
		 pp = pp->next();
	 }
	 for(n = 0; n < numProcs; n++) {
		 ppArr[n]->set_priority(n);
		 m->insert_process(ppArr[n]);
	 }
	 delete ppArr;
}

void listMailboxProcs(mailbox *m)
{
	process_t pp;

	fprintf(fp, "%8.3lf mailbox %s process list\n", clock, m->name());
	pp = m->first_process();
	while(pp != NIL) {
		fprintf(fp, "process %s id %ld priority %ld\n", pp->name(),
			pp->identity(), pp->priority());
		pp = pp->next();
	}
}

