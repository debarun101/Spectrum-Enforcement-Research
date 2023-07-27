// round robin service

#include "cpp.h"

facility f("f");
table st("serv tm");

void high_task();
void low_task();

extern "C" void sim()
{
	int i;

	create("sim");

	f.set_servicefunc(rnd_pri);
	f.set_timeslice(0.5);

	high_task();
	for(i = 0; i < 3; i++)
		low_task();
	event_list_empty.wait();
	report();
}

void high_task()
{
	int i;

	create("high");
	set_priority(2l);
	for(i = 0; i < 5; i++) {
		hold(1.0);
		f.use(1.0);
		st.record(1.0);
		}
}

void low_task()
{
	int i;

	create("low");
	set_priority(1l);
	for(i = 0; i < 5; i++) {
		f.use(1.0);
		st.record(1.0);
		hold(1.0);
		}
}
