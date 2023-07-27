// client-server: with mailbox and messages 

#include "cpp.h"

#define SIMTIME 10.0
#define NS 2l
#define NC 4l

#define SERV_REQ 1l
#define ACK 2l
#define QUIT 3l
#define QUIT_ACK 4l

long msg_ct, msg_cur;
void msg_rep();

//---------------------- msg class ------------------------

class msg {
protected:
	mailbox *rply;
	long typ;
	long sndr;
public:
	msg(long, long);
	msg(long, long, mailbox*);
	~msg();
	mailbox *reply()	{return(rply);}
	long type()		{return(typ);}
}; 

msg::msg(long n, long t, mailbox *m)
{
	rply = m;
	typ = t;
	sndr = n;
	msg_ct++;
	msg_cur++;
}

msg::msg(long n, long t)
{
	typ = t;
	sndr = n;
	msg_ct++;
	msg_cur++;
}

msg::~msg()
{
	msg_cur--;
}

//--------------------- end msg class ---------------------------

FILE *fp;

mailbox *serv_mb[NS];

char str[24];
void error(char *, long);
void server(long), client(long);
void stop_servers();

extern "C" void sim()
{
	long i;

	fp = fopen("client_s.out", "w");
	set_output_file(fp);
	create("sim");
	for(i = 0; i < NS; i++)
		serv_mb[i] = new mailbox("serv");
	msg_ct = msg_cur = 0;
	for(i = 0; i < NS; i++)
		server(i);
	for(i = 0; i < NC; i++)
		client(i);
	event_list_empty.wait();
	stop_servers();
	msg_rep();
}

void server(long n)
{
	msg *r;
	msg *s;
	long type;

	create("server");
	do {
		serv_mb[n]->receive((long *)&r);
		hold(1.0);
		fprintf(fp, "server.%ld sends back to mb %s\n",
			n, r->reply()->name());
		switch(r->type()) {
		case SERV_REQ:
			type = ACK;
			break;
		case QUIT:
			type = QUIT_ACK;
			break;
		default:
			error("server %ld: unexpected msg type", n);
		}
		s = new msg(n, type);
		r->reply()->send((long)s);
		delete r;
	} while(type != QUIT);
}

void client(long n)
{
	mailbox *recv;
	msg *s;
	msg *r;
	long i;

	create("client");
	sprintf(str, "cli.%d", n);
	recv = new mailbox(str);
	while(clock < SIMTIME) {
		s = new msg(n, SERV_REQ, recv);
		i = random(0l, NS-1l);
		fprintf(fp, "client.%ld sends to server.%ld\n", n, i);
		serv_mb[i]->send((long)s);
		recv->receive((long*)&r);
		delete r;
		}
	delete recv;
}

void stop_servers()
{
	long i;

	msg *s;
	msg *r;
	mailbox *recv;

	recv = new mailbox("stop");
	for(i = 0; i < NS; i++) {
		s = new msg(i, QUIT, recv);
		serv_mb[i]->send((long)s);
		}
	for(i = 0; i < NS; i++) {
		recv->receive((long*)&r);
		delete r;
		}
	delete recv;
}

void error(char *f, long n)
{
	fprintf(fp, f,n);
	fprintf(fp, "\n");
	exit(1);
}

void msg_rep()
{
	fprintf(fp, "messages created %4ld\n", msg_ct);
	fprintf(fp, "current messages %4ld\n", msg_cur);
}
