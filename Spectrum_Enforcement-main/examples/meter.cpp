// test meter

#include "cpp.h"
FILE *fp;

#define SIM_TIME 10000.0
#define IAR_TIME 2.0
#define SRV_TIME 3.0
#define NUM_FACS 2

facility_set *f;
meter *m;
box *b;

void init();
void cust();

extern "C" void sim()
{
	fp = fopen("meter.out", "w");
	set_output_file(fp);
	create("sim");
	init();
	while(clock < SIM_TIME) {
		hold(exponential(IAR_TIME));
		cust();
		}
	report();
}

void init()
{
	f = new facility_set("fac", NUM_FACS);
	m = new meter("meter");
	m->add_histogram(10, 0.0, 10.0);
	b = new box("fac box");
	b->time_confidence();
	b->add_time_histogram(10, 0.0, 100.0);
	b->add_number_histogram(10, 0, 50);
}

void cust()
{
	long i;
	TIME ts;
	
	create("cust");
	i = random(0, NUM_FACS - 1);
	ts = b->enter();
	(*f)[i].use(exponential(SRV_TIME));
	b->exit(ts);
	m->note_passage();
}
	
