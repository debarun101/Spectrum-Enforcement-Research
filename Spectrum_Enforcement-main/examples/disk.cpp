// a disk facility with a scheduling discipline

#include "cpp.h"
#include <string.h>
FILE *fp;

#define SEEK_START 0.005
#define SEEK_TRACK 0.0001

struct dsk : public facility {
protected:
	long current_track;
	char nm[24];
public:
	dsk(char*);
	void use(double, long);
};

dsk::dsk(char *s) : facility(s)
{
	strcpy(nm, s);
	current_track = 0;
}

void dsk::use(double t, long tr)
{
	long delta;
	double x;

	facility::reserve();
	fprintf(fp, "%.3f get disk %s: curr tr = %ld, new tr = %d\n", 
		clock, nm, current_track, tr);
	delta = current_track - tr;
	delta = (delta < 0) ? -delta : delta;
	x = SEEK_START + SEEK_TRACK*delta + t;
	hold(x);
	current_track = tr;
	fprintf(fp, "%.3f rel disk %s, curr tr = %ld\n",
		clock, nm, current_track);
	facility::release();
}

//--------------------------

dsk disk("disk");

extern "C" void sim();

void sim()
{
	long i;
	void job(long);

	fp = fopen("disk.out", "w");
	set_output_file(fp);
	create("sim");
	for(i = 0; i < 5; i++)
		job(i);
	event_list_empty.wait();
	report();
}

void job(long n)
{
	long i;

	create("job");
	i = 0;
	while(clock < 1.0) {
		hold(expntl(0.050));
		disk.use(0.010, ++i);
		}
}
