// C++/CSIM Model of M/M/1 queue

#include "cpp.h"			// class definitions
FILE *fp;					// file pointer

#define NARS 5000
#define IAR_TM 2.0
#define SRV_TM 1.0

event done("done");			// the event named done
facility f("facility");			// the facility named f
table tbl("resp tms");			// table of response times
qhistogram qtbl("num in sys", 10);	// qhistogram of number in system
int cnt;				// count of remaining processes
stream *serv_tm;			// service time random number stream
stream *inter_tm;			// interarrival time random number stream

void customer();

extern "C" void sim(int, char*[]);

extern "C" void sim(int argc, char *argv[])
{
	fp = fopen("ex1_stre.out", "w");
	set_output_file(fp);
	set_model_name("M/M/1 Queue");
	create("sim");
	serv_tm = new stream(1);
	inter_tm = new stream(1);
	cnt = NARS;
	for(int i = 1; i <= NARS; i++) {
		hold(inter_tm->expntl(IAR_TM)); // interarrival interval
		customer();		// generate next customer
		}
	done.wait();			// wait for last customer to depart
	report();			// model report
	mdlstat();			// model statistics
}

void customer()				// arriving customer
{
	double t1;

	create("cust");
	t1 = clock;			// record start time
	qtbl.note_entry();		// note arrival
	f.reserve();			// reserve facility
		hold(serv_tm->expntl(SRV_TM)); // service interval
	f.release();			// release facility
	tbl.record(clock - t1);		// record response time
	qtbl.note_exit();		// note departure
	if(--cnt == 0)
		done.set();		// if last customer, set done
}
