// random number streams

#include "cpp.h"
#include <stdio.h>

FILE *fp;

stream s_exp(1);
stream s_hyp(1);
stream s_erl(1);
stream s_nor(1);
stream s_prob(1);
stream s_ran(1);
stream s_unif(1);

table h_exp("exp", 10, 0.0, 5.0);
table h_hyp("hyp", 10, 0.0, 5.0);
table h_erl("erl", 10, 0.0, 5.0);
table h_nor("nor", 20, -5.0, 5.0);
table h_prob("prob", 10, 0.0, 1.0);
table h_ran("ran", 10, 0.0, 10.0);
table h_unif("unif", 10, 0.0, 10.0);

extern "C" void sim()
{
	long i, j;
  
  fp = fopen("xxx.out", "w");
  set_output_file(fp);
  for(j = 0; j < 2; j++) {
	for(i = 0; i < 10000; i++) {
		h_exp.record(s_exp.expntl(1.0));
		h_hyp.record(s_hyp.hyperx(1.0, 4.0));
		h_erl.record(s_erl.erlang(1.0, 0.5));
		h_nor.record(s_nor.normal(0.0, 1.0));
		h_prob.record(s_prob.prob());
		h_ran.record((float) s_ran.random(0, 10));
		h_unif.record(s_unif.uniform(0.0, 10.0));
		}
	report_tables();
	reset();
	s_exp.reset_prob(1);
	s_hyp.reset_prob(1);
	s_erl.reset_prob(1);
	s_nor.reset_prob(1);
	s_prob.reset_prob(1);
	s_ran.reset_prob(1);
	s_unif.reset_prob(1);
  }
}
		
