// multi-server facility

#include "cpp.h"
FILE *fp;

facility_ms cpu("cpu", 3l);

void proc();

extern "C" void sim()
{
	int i;

	fp = fopen("fac_ms.out", "w");
	set_output_file(fp);
	create("sim");
	for(i = 0; i < 3; i++)
		proc();
	event_list_empty.wait();
	report();
}

void proc()
{
	create("proc");

	cpu.reserve();
		hold(uniform(1.0, 5.0));
	cpu.release();
}
