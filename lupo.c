#include "lupo.h"

void lupo_pout(unsigned int maddr, short sval){
  vwrite16(maddr+LUPO_IO_POUT, &sval);
}

void lupo_dis_interrupt(unsigned int maddr){
  short sval = 1;
  vwrite16(maddr+LUPO_IO_DISINT, &sval);
}

void lupo_pulse_width(unsigned int maddr, short sval){
  vwrite16(maddr+LUPO_IO_PWID, &sval);
}

