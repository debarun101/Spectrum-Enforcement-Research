// example using storages 

#include "cpp.h"
#include <stdio.h>
FILE *fp;

#define N 100

#define MAXSTORE 0x1000l
#define MINREQ 0x100l
#define MAXREQ 0x800l

storage s("s", MAXSTORE);
table amt_tbl("amt requested");
table srv_tm("service times");
qtable wait_tm("waiting times");

void job();
void make_storage_report();

extern "C" void sim()
{
	int i;

	fp = fopen("storages.out", "w");
	set_output_file(fp);
	create("sim");
	for(i = 0; i < N; i++) {
		job();
		hold(expntl(10.0));
		}
	event_list_empty.wait();
	report();
	make_storage_report();
	mdlstat();
}

void job()
{
	long amt;
	double x;

	create("job");
	amt = random(MINREQ, MAXREQ);
	amt_tbl.record((double) amt);
	wait_tm.note_entry();
	s.allocate(amt);
		x = expntl(20.0);
		srv_tm.record(x);
		hold(x);
	s.deallocate(amt);
	wait_tm.note_exit();
}
	
void make_storage_report()
{
	long req_cnt, cap;
	double t, w;

	fprintf(fp, "\n\tStorage Report: %s\n", s.name());
	cap = s.capacity();
	fprintf(fp, "    capacity  %ld\n", cap);
	req_cnt = s.request_cnt();
	fprintf(fp, "    amt       %.3f\n",
		 (double)s.request_amt()/req_cnt);
	t = s.time();
	fprintf(fp, "    util      %.3f\n",
		(double)s.busy_amt()/(t*cap));
	fprintf(fp, "    srv_tm    %.3f\n",
		(double)s.number_amt()/req_cnt);
	w = s.waiting_amt();
	fprintf(fp, "    qlen      %.3f\n",
		(double)w/t);
	fprintf(fp, "    resp      %.3f\n",
		(double)w/req_cnt);
	fprintf(fp, "    completes %ld\n", s.release_cnt());
	fprintf(fp, "    queued    %ld\n", s.queue_cnt());
}
