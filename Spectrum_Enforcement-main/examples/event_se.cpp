// test event_set

#include "cpp.h"
#include <stdio.h>
FILE *fp;

event done("done");
event_set e1("ev1", 3);
event_set* e2;

void cust();

extern "C" void sim()
{
	fp = fopen("event_se.out", "w");
	set_output_file(fp);
	create("sim");
	e2 = new event_set("e2", 3); 
	e1.monitor();
	e2->monitor();
	cust();
    hold(1.0);
	e1[1].set();
    hold(4.0);
  	(*e2)[2].set();
  	hold(0.1);
	report();
}

void cust()
{
	long n;
	
	create("cust");
	n = e1.wait_any();
	fprintf(fp, "e1[%ld] occurred\n", n);
	hold(2.0);
	n = e2->wait_any();
	fprintf(fp, "e2[%ld] occurred\n", n);
}
