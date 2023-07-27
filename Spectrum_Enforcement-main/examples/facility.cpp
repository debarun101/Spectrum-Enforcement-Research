// test facility_set

#include "cpp.h"
FILE *fp;

facility_set fs1("fs1", 3l);
facility* fs2[3];
facility_set* fs3;

extern "C" void sim()
{
	int i;
	char str[16];

	fp = fopen("facility.out", "w");
	set_output_file(fp);
	create("sim");
	for(i = 0; i < 3; i++) {
		sprintf(str, "fs2[%d]", i);
		fs2[i] = new facility(str);
		}

	fs3 = new facility_set("fs3", 3l);

	for(i = 0; i < 3; i++) {
		fs1[i].use((float)(i+1));
		fs2[i]->use((float)(i+1));
		(*fs3)[i].use((float)(i+1));
		}

	report();
}
