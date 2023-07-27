// illustrate use of my main

#include "cpp.h"

#define N 10l
#define POP 10l
 
facility* cpu;
facility_set* disk;
process_class* job_cl;
process_class* io_cl;

void my_sim();
void job(int);

int main(int argc, char** argv)
{
	void sim();

	proc_csim_args(&argc, &argv);
	my_sim();
	my_sim();
	my_sim();

	return 0;
}

void my_sim()
{
	int i;

	create("sim");
	cpu = new facility("cpu");
	disk = new facility_set("disk", N);
	job_cl = new process_class("job class");
	io_cl = new process_class("io class");
	collect_class_facility_all();
	for(i = 0; i < POP; i++) {
		job(i);
		hold(exponential(1.0));
		}
	event_list_empty.wait();
	report();
	delete cpu;
	delete disk;
	delete job_cl;
	delete io_cl;
	rerun();
	reset_prob(1);
}

void job(int n)
{
	void doio(event*);
	event* iodone;

	create("job");
	job_cl->set_process_class();
	iodone = new event("iodone");
	doio(iodone);
	cpu->use(exponential(0.7));
	iodone->wait();
}

void doio(event* ev)
{
	create("io");
	io_cl->set_process_class();
	(*disk)[random(0l, N-1)].use(expntl(1.0));
	ev->set();
}

extern "C" void sim()
{
}
