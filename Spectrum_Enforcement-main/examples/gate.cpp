// test gates

#include "cpp.h"
FILE *fp;

#define OPEN 0
#define CLOSED 1

class gate  : public event {
protected:
	int g_state;
public:
	gate(char *s) : event(s) { g_state = CLOSED;}
	void wait()	{ if(g_state == CLOSED) event::wait(); }
	void set()	{ if(g_state == CLOSED)
				{ g_state = OPEN; event::set(); }}
	void clear()	{ g_state = CLOSED; event::clear(); }
	int state()	{ return(g_state);}
};

gate go("go");

void job(int);

extern "C" void sim()
{
	int i;

	fp = fopen("gate.out", "w");
	set_output_file(fp);
	create("sim");
	for(i = 1; i <= 5; i++)
		job(i);
	hold(3.0);
	fprintf(fp, "before set: ");
	fprintf(fp, "state = %d, wait_cnt = %d\n",
		go.state(), go.wait_cnt());
	go.set();
	fprintf(fp, "after set: ");
	fprintf(fp, "state = %d, wait_cnt = %d\n",
		go.state(), go.wait_cnt());
	event_list_empty.wait();
	fprintf(fp, "after done: ");
	fprintf(fp, "state = %d, wait_cnt = %d\n",
		go.state(), go.wait_cnt());
	report();
}

void job(int i)
{
	create("job");
	hold((float)i);
	fprintf(fp, "%8.3lf: job.%d waits\n", clock, i);
	go.wait();
	fprintf(fp, "%8.3lf: job.%d leaves\n", clock, i);
}
