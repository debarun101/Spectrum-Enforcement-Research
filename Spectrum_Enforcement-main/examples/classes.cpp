// illustrate use of process classes and simulate an M/M/1 queue
//  (an open queue with exponential service times and interarrival intervals)

#include "cpp.h"
#include <stdio.h>
FILE *fp;

#define	SVTM	1.0	/*mean of service time distribution */
#define IATM	2.0	/*mean of inter-arrival time distribution */
#define NARS	1000	/*number of arrivals to be simulated*/

facility_ms *f;
event *done;
table *tbl;
qtable *qtbl;
process_class* cl[2];
int cnt;		/*number of active tasks*/

void init();
void cust();
void make_facility_report(facility*);

extern "C" void sim()		/*1st process - named sim */
{
	int i;
    
	init();
	create("sim");				/*required create statement*/
	for(i = 1; i <= NARS; i++) {
		hold(expntl(IATM));		/*hold interarrival*/
		cust();				/*initiate process cust*/
		}
	done->wait();				/*wait until all done*/
	report();				/*print report*/
	make_facility_report(f);
}

void init()
{
    fp = fopen("xxx.out", "w");
    set_output_file(fp);
	set_model_name("M/M/1 Queue");
	f = new facility_ms("f", 2);
	f->collect_class_facility();		/*collect class statistics*/
	done =  new event("done");			/*initialize event*/
	cl[0] = new process_class("class 0");	/*initialize cust class */
	cl[1] = new process_class("class 1");
	tbl = new table("tbl");
	qtbl = new qtable("qtbl", 10, 0, 10);
	cnt = NARS;							/*initialize cnt*/
}

void cust()				/*process customer*/
{
	double t1;
	process_class *c;
 
	create("cust");				/*required create statement*/
	cl[random(0l,1l)]->set_process_class();
    c = current_class();
	t1 = clock;				/*time of request */
	qtbl->note_entry();			/*note arrival */
	f->reserve();				/*reserve facility f*/
		hold(expntl(SVTM));		/*hold service time*/
	f->release();				/*release facility f*/
	tbl->record(clock-t1);			/*record response time*/
	qtbl->note_exit();			/*note departure */
	cnt--;					/*decrement cnt*/
	if(cnt == 0)
		done->set();			/*if last arrival, signal*/
}

void make_facility_report(facility* f)
{
	long i, n;

	fprintf(fp, "\n\tfacility %s\n", f->name());
	fprintf(fp, "service disp    %s\n", f->service_disp());
	fprintf(fp, "service time    %.3f\n", f->serv());
	fprintf(fp, "utilization     %.3f\n", f->util());
	fprintf(fp, "throughput      %.3f\n", f->tput());
	fprintf(fp, "queue length    %.3f\n", f->qlen());
	fprintf(fp, "response time   %.3f\n", f->resp());
	fprintf(fp, "completions     %ld\n",  f->completions());
	fprintf(fp, "preempts        %ld\n", f->preempts());
	n = f->num_servers();
	fprintf(fp, "num servers     %d\n", n);
	if(n > 1) {
		for(i = 0; i < n; i++) {
			fprintf(fp, "\nserver %d\n", i);
			fprintf(fp, "    service time    %.3f\n",
				f->server_serv(i));
			fprintf(fp, "    utilization     %.3f\n",
				f->server_util( i));	
			fprintf(fp, "    throughput      %.3f\n",
				f->server_tput(i));
			fprintf(fp, "    completions     %d\n",
				f->server_completions(i));
			}
		}
	for(i = 0; i < 2; i++) {
		fprintf(fp, "\nclass %d\n", i);
		fprintf(fp, "    service time     %.3f\n", f->class_serv(cl[i]));
		fprintf(fp, "    utilization      %.3f\n", f->class_util(cl[i]));
		fprintf(fp, "    throughput       %.3f\n", f->class_tput(cl[i]));
		fprintf(fp, "    queue length     %.3f\n", f->class_qlen(cl[i]));
		fprintf(fp, "    response time    %.3f\n", f->class_resp(cl[i]));
		fprintf(fp, "    completions      %ld\n", f->class_completions(cl[i]));
		}
}
	
