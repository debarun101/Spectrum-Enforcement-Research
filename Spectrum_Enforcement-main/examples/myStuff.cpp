//	illustrate process private data pointer 
//  and managing processes at storages

#include "cpp.h"
#include <stdio.h>
#include <stdlib.h>
FILE *fp;

#define NUM_STORES 1
#define NUM_PROCESSES 10

storage* stArr[NUM_STORES];
event* evArr[NUM_STORES];
char str[48];

void init();
void triangle(long);
void gen();
void storeManager(long);
void sample();
void stuffReport();

struct myStuff {
	long lookCnt;
	long attrib;
	long newAttrib;
	double startTime;
	double endTime;
};

extern "C" void sim()
{
	create("sim");
	init();
	sample();
	gen();
	hold(5.0);
	report();
}

void init()
{
	long i;

	fp = fopen("myStuff.out", "w");
	set_output_file(fp);
	set_trace_file(fp);
//	trace_on();
	for(i = 0; i < NUM_STORES; i++) {
		sprintf(str, "str[%ld]", i);
		stArr[i] = new storage(str, 2);
		sprintf(str, "ev[%ld", i);
		evArr[i] = new event(str);
		storeManager(i);
	}
}

void storeManager(long n)
{
	process_t pp;
	struct myStuff *ptr;

	sprintf(str, "mgr.%ld", n);
	create(str);
	while(1) {
		evArr[n]->wait();
		fprintf(fp, "%8.3lf storeManager.%ld ran\n", clock, n);
		pp = stArr[n]->first_process();
		while(pp != NIL) {
			if(pp != NIL) {
				ptr = (struct myStuff*)pp->get_struct();
				if(ptr->lookCnt++ == 0)
					ptr->attrib = ptr->newAttrib;
				pp = pp->next();
			}
		}
	}
}

void gen()
{
	long i;

	for(i = 0; i < NUM_PROCESSES; i++) {
		triangle(i);
		hold(1.0);
	}
}

void triangle(long n)
{
	long i;
	struct myStuff *ptr;

	create("triangle");
	i = uniform_int(0, NUM_STORES-1);
	ptr = (struct myStuff*)malloc(sizeof(struct myStuff));
	ptr->lookCnt = 0;
	ptr->attrib = 1;
	ptr->newAttrib = n;
	set_this_struct(ptr);
	evArr[i]->set();
	stArr[i]->allocate(1);
	hold(4.0);
	stArr[i]->deallocate(1);
	free(ptr);
}

void sample()
{
	create("sample");
	hold(0.5);
	while(1) {
		stuffReport();
		hold(1.0);
	}
}

void stuffReport()
{
	long i;
	process_t pp;
	struct myStuff* ptr;

	for(i = 0; i < NUM_STORES; i++) {
		fprintf(fp, "%8.3lf queued processes at %s:\n", clock, stArr[i]->name());
		pp = stArr[i]->first_process();
		while(pp != NIL) {
			ptr = (struct myStuff*)pp->get_struct();
			fprintf(fp, "\tprocess %s.%ld, lookcnt %ld, attrib %ld\n",
				pp->name(), pp->identity(), ptr->lookCnt, ptr->attrib);
			pp = pp->next();
		}
	}
}

