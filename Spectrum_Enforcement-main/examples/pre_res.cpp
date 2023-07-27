// example of preempt-resume service discipline

#include "cpp.h"

facility_ms f("f", 2);

void job(int);

extern "C" void sim()
{
	int i;

	create("sim");
	f.set_servicefunc(pre_res);
	for(i = 0; i < 1000; i++) {
		job(i);
		hold(expntl(2.0));
		}
	event_list_empty.wait();
	report();
}

void job(int n)
{
	create("job");
	set_priority((long)n);
	f.use(expntl(1.5));
}
