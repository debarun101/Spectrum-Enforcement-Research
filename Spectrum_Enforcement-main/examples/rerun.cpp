// test rerun and statis object

#include "cpp.h"
#include <stdio.h>

FILE *fp;

facility_ms *cpu;
mailbox *mb;
event *done;
storage *st;
process_class *job_cl;
table *resp_tm;
histogram *resp_h;
qtable *mem_q;
qhistogram *cpu_q;

void init();
void gen_jobs();

extern "C" void sim()
{
	int i;
	
	fp = fopen("xxx.out", "w");
	set_output_file(fp);
	for(i = 0; i < 3; i++) {
		create("sim");
		init();
		gen_jobs();
		event_list_empty.wait();
		report();
		rerun();
		reset_prob(1);
		}
}

void init()
{
	cpu = new facility_ms("cpu", 2);
	mb = new mailbox("mb");
	done = new event("done");
	st = new storage("st", 0x1000);
	job_cl = new process_class("job_cl");
	resp_tm = new table("resp_tm");
	resp_h = new histogram("resp_h", 10, 0.0, 10.0);
	mem_q = new qtable("mem queue");
	cpu_q = new qhistogram("cpu queue", 10);
}	

void gen_jobs()
{
	int i;
	void job();

	create("gen_jobs");
	for(i = 0; i < 3; i++) {
		job();
		hold(expntl(1.0));
		}
}

void job()
{
	double t;
	long amt;

	create("job");
	job_cl->set_process_class();
	t = clock;
	amt = random(0x100, 0x800);
	mem_q->note_entry();
	st->allocate(amt);
		mem_q->note_exit();
		cpu_q->note_entry();
		cpu->use(expntl(2.0));
		cpu_q->note_exit();
	st->deallocate(amt);
	resp_tm->record(clock-t);
	resp_h->record(clock-t);
}
