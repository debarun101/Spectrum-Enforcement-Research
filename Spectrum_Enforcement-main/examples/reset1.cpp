// an example of using reset

#include "cpp.h"
#include <stdio.h>

FILE *fp;

facility_ms *cpu;
facility_set *disk;
process_class *job_cl1;
process_class *job_cl2;
storage *st;
table *resp_time;
histogram *cpu_time;
qhistogram *cpu_queue;
qtable *sys_queue;

void my_sim(int);
void init();

int main()
{
	fp = fopen("xxx.out", "w");
	set_output_file(fp);
	my_sim(0);
	my_sim(1);

	return 0;
}
 
void gen_jobs(int);

void my_sim(int sw)
{
	create("sim");
	init();
	collect_class_facility_all();
	gen_jobs(100);
	if(sw == 1) {
		hold(5.0);
		reset();
		}
	event_list_empty.wait();
	report();
	rerun();
	reset_prob(1);
}

void init()
{
	
	cpu = new facility_ms("cpu", 2l);
	disk = new facility_set("disk", 4l);
	job_cl1 = new process_class("class 1");
	job_cl2 = new process_class("class 2");
	st = new storage("st", 0x100l);
	resp_time = new table("resp time");
	cpu_time = new histogram("cpu time", 10l, 0.0, 2.0);
	cpu_queue = new qhistogram("cpu queue", 10l);
	sys_queue = new qtable("system queue");
}

void gen_jobs(int n)
{
	void job_1(), job_2();
	int i;

	create("gen_jobs");
	for(i = 0; i < n; i++) {
		if(prob() < 0.5)
			job_1();
		else
			job_2();
		hold(expntl(1.0));
		}
}

void job_1()
{
	long i, j;
	double x, t;

	create("job");
	job_cl1->set_process_class();
	
	t = clock;
	sys_queue->note_entry();
	st->allocate(0x28l);
	for(i = 0; i < 5; i++) {
		cpu_queue->note_entry();
		x = expntl(0.500);
		cpu_time->record(x);
		cpu->use(x);
		cpu_queue->note_exit();
		j = random(0, 3);
		(*disk)[j].use(expntl(0.600));
		}
	st->deallocate(0x28l);
	sys_queue->note_exit();
	resp_time->record(clock -t);
}

void job_2()
{
	long i, j;
	double x, t;

	create("job");
	job_cl2->set_process_class();
	
	t = clock;
	sys_queue->note_entry();
	st->allocate(0x28l);
	for(i = 0; i < 5; i++) {
		cpu_queue->note_entry();
		x = expntl(0.050);
		cpu_time->record(x);
		cpu->use(x);
		cpu_queue->note_exit();
		j = random(0, 3);
		(*disk)[j].use(expntl(0.060));
		}
	st->deallocate(0x28l);
	sys_queue->note_exit();
	resp_time->record(clock -t);
}

extern "C" void sim()
{
}
