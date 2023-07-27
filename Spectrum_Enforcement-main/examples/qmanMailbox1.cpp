//	qmanMailbox.c - test qman mailbox routines

#include "cpp.h"
#include <stdio.h>
#include <stdlib.h>
FILE *fp;

const double simTime = 100.0;
const long numSenders = 10;
const long numReceivers = 2;

mailbox *mb;

void init();
void gen();
void sender(long);
void receiver(long);
void listMailboxMsgs(mailbox *m);
void reorderMailboxMsgList(mailbox *m);

extern "C" void sim()
{
	init();
	create("sim");
	gen();
	hold(simTime);
}

void init()
{
	fp = fopen("qmanMailbox1.out","w");
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
	for(i = 1; i <= numReceivers; i++) {
		fprintf(fp, "%8.3lf sender.%ld id %ld sends %ld\n",
			clock, n, identity(), i);
		mb->send(i);
	}
	hold(1.0);
	if(n == 1) {
		listMailboxMsgs(mb);
		reorderMailboxMsgList(mb);
		listMailboxMsgs(mb);
	}
}

void receiver(long n)
{
	long msg, i;

	create("recvr");
	hold(5.0);
	for(i = 0; i < numSenders; i++)  {
		mb->receive(&msg);
		fprintf(fp, "%8.3lf receiver.%ld id %ld received %ld\n",
			clock, n, identity(), msg);
	}
}

void listMailboxMsgs(mailbox *m)
{
	message_t mp;
	long n, numMsgs;

	numMsgs = m->msg_cnt();
	fprintf(fp, "%8.3lf mailbox %s msg list numMsgs %ld\n", clock, m->name(), numMsgs);
	mp = m->first_message();
	n = 0;
	while(mp != NIL) {
		fprintf(fp, "msg %ld content %ld\n", n, mp->content());
		n++;
		mp = mp->next();
	}
}

void reorderMailboxMsgList(mailbox *m)
{
	message_t mp;
	message_t *mArr;
	long n, numMsgs, nr;

	numMsgs = m->msg_cnt();
	mArr = new message_t[numMsgs];
	n = 0;
	for(nr = 1; nr <= numReceivers; nr++) {
		mp = m->first_message();
		while(mp != NIL) {
			if(mp->content() == nr) {
				if(n >= numMsgs) {
					printf("msg reorder error\n");
					exit(1);
				}
				mArr[n++] = m->remove_message(mp);
			}
			mp = mp->next();
		}
	}
	mp = m->first_message();
	for(n = 0; n < numMsgs; n++)
		m->insert_message(mArr[n]);
	delete mArr;
}

