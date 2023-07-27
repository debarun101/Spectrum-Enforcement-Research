// test clock synchronized facility

#include "cpp.h"

#define NJ 3
#define NC 2

facility f("f");

void job(int);

extern "C" void sim()
{
	int i;

	create("sim");
	f.synchronous(0.5, 1.0);

	for(i = 1; i <= NJ; i++)
		job(i);
	hold(10.0);
	report();
}

void job(int n)
{
	int i;

	create("job");
	set_priority((long)n);
	for(i = 0; i < NC; i++)
		f.use(0.250);
}
