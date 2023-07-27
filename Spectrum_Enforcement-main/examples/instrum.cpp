// instrumenting a facility

#include "cpp.h"
#include <stdio.h>    

#define SIMTIME 10000.0

facility f("center");
meter arrivals("arrivals");
meter departures("completions");
box queue_box("queue");
box service_box("in service");

FILE *fp;

void init();
void customer();
extern "C" long _qsize;

extern "C" void sim()
{
	create("sim");
	init();
	while(clock < SIMTIME) {
	    hold(expntl(1.0));
		customer();
	    }
	event_list_empty.wait();
	report();
}

void customer()
{
	TIME timestamp1, timestamp2;
	
	create("customer");
	arrivals.note_passage();
	timestamp1 = queue_box.enter();
	f.reserve();
		timestamp2 = service_box.enter();
		hold(expntl(0.8));
	f.release();
	service_box.exit(timestamp2);
	queue_box.exit(timestamp1);
	departures.note_passage();
}

#include <math.h>

void init()
{
	fp = fopen("xxx.out", "w");
	set_output_file(fp);
	set_trace_file(fp);
	queue_box.add_time_histogram(10L, 0.0, 10.0);
	queue_box.add_number_histogram(20L, 0L, 20L);
}
	
