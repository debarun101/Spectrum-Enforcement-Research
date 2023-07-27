// moving window qtable example

#include "cpp.h"
FILE *fp;

#define NJOBS 100

qtable qt1("moving");
qtable qt2("summary");
facility f("f");
event done("done");
int nact;

void job(int);

extern "C" void sim()
{
	int i;

	fp = fopen("moving_q.out", "w");
	set_output_file(fp);
	create("sim");
	qt1.moving_window(10);
	nact = NJOBS;
	for(i = 1; i <= NJOBS; i++) {
		hold(1.0);
		job(i);
		}
	done.wait();
	fprintf(fp, "\nQtable %s is %ld point moving window qtable\n",
		qt1.name(), qt1.window_size());
	fprintf(fp, "Qtable %s is %ld point moving window qtable\n",
		qt2.name(), qt2.window_size());
	report();
}

void job(int n)
{
	create("job");
	qt1.note_entry(); qt2.note_entry();
		f.use((float) n);
	qt1.note_exit(); qt2.note_exit();
	if(n % 50 == 0)
		report_qtables();
	if(--nact == 0)
		done.set();
}
