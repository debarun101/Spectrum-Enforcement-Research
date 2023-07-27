// CSIM Example of General Computer System 
// The model simulates a system with memory,
//	a cpu, and disk drives.  Jobs arrive (are
//	generated), queue for memory, and then compete
//	for the cpu and disk drives.


#include "cpp.h"
#include <stdio.h>

#define NJOBS	20000
#define NCPUS	2
#define NDISKS	4
#define AMTMEM	100

#define INTARV	1.0
#define MNCPU	0.25
#define MNDSK	0.030

#define TOTCPU	1.0
#define EPS	0.0005

facility_ms cpu("cpu", NCPUS);		// cpu facility 
facility_set disk("disk", NDISKS);	// disk drives
storage mem("memory", AMTMEM);		// memory 
event done("event");			// done event 
box systemBox("system");
int act;						// count of active jobs
FILE *fp;

void job(long);
void generate_jobs();

extern "C" void sim()				//1st process - job generator 
{
	enable_separate_stacks(2000);
	create("sim");
	fp = fopen("xxx.out", "w");
	set_output_file(fp);
	max_events(1000);
	cpu.set_servicefunc(pre_res);
	systemBox.add_number_histogram(20, 0, 40);
	systemBox.add_time_histogram(15, 0.0, 30.0);
	systemBox.time_confidence();
	systemBox.time_run_length(0.10, 0.90, 90.0);
	act = NJOBS;
	generate_jobs();
	converged.wait(); 			// wait until all jobs completed
//	done.wait();
	report();					// print report 
}

void generate_jobs()
{
	long i;

	create("gen");
	i = 1;
	while(1) {
		job(i++);			//initiate job process 
		hold(expntl(INTARV));	//hold interarrival interval 
		}
}

void job(long i)			// a job process 
{
	void io(long, event*);
	long j, amt;
	event* iodone;
	double cpt, x;
	TIME t;

	create("job");
	set_priority(i);			//set job priority 
	iodone = new event("iodone");

	t = systemBox.enter();			//enter system box			//entry system queue
	amt = random(5, AMTMEM/2);		//select size memory req. 
	mem.allocate(amt);			//request (allocate) memory
	cpt = erlang(TOTCPU, 0.5*TOTCPU); 	//select cpu time req.
	while(cpt > EPS) {
		j = random(0, NDISKS-1);	//select disk drive 
		io(j, iodone);			//initiate I/O process 
		x = hyperx(MNCPU, 4.0*MNCPU); 	//select next cpu intrvl
		cpt -= x;
		cpu.use(x);			//use cpu x time units
		iodone->wait();			//wait for I/O to complete
		}
	mem.deallocate(amt);			//return memory
	systemBox.exit(t);
	if(--act == 0)
		done.set();
//	if((act % 5000) == 0) {
//		printf("%8.3lf - numAct = %ld, respTm = %.3lf\n",
//			clock, act, systemBox.time_table()->mean());
//		systemBox.report();
//		fflush(fp);
//	}
	delete iodone;
}

void io(long d, event* ev)			// I/O process 
{
	create("io");

	disk[d].reserve();			//reserve disk 
		hold(expntl(MNDSK));		//disk service interval
	disk[d].release();			//release disk
	ev->set();				//signal I/O complete
}
