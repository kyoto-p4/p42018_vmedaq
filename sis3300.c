#include "sis3300.h"

int sis3300_segdata_pre(unsigned int maddr,int gr,int max){
  int wordcnt;
  int graddr;
  int adptr;

  wordcnt = 0;
  graddr = SIS3300_BANKSIZE*gr;
  if(max > 3500){
    max = 3500;
  }

  vread32(maddr+SIS3300_TRIGGER_EVT_DIR_BANK1,&adptr);
  adptr = (adptr & 0x001ffff) - max;

  if(adptr < 0){
    adptr = 0x00020000 + adptr;
  }

  while(wordcnt < max){
    adptr += 1;
    if(adptr >= 0x00020000){
      adptr -= 0x00020000;
    }
    vread32(maddr+SIS3300_MEMBASE_BANK1+graddr+(adptr*4),
	    (int *)(data+mp));
    mp += 2;
    wordcnt++;
    segmentsize += 2;

    if(data[mp-2] & (SIS3300_ORBIT_S | SIS3300_USRBIT_S)){
      break;
    }
  }
  
  return segmentsize;
}


void sis3300_set_mode(unsigned int maddr, int mode){
  int lval;

  lval = mode;
  vwrite32(maddr+SIS3300_EVENT_CONFIG_ADC12,&lval);
  vwrite32(maddr+SIS3300_EVENT_CONFIG_ADC34,&lval);
  vwrite32(maddr+SIS3300_EVENT_CONFIG_ADC56,&lval);
  vwrite32(maddr+SIS3300_EVENT_CONFIG_ADC78,&lval);
  vwrite32(maddr+SIS3300_EVENT_CONFIG_ALL_ADC,&lval);
}

void sis3300_acq_ctrl(unsigned int maddr, int mode){
  int lval;

  lval = mode;
  vwrite32(maddr+SIS3300_ACQ_CTRL,&lval);
}


void sis3300_stop_delay(unsigned int maddr, int size){
  int lval = size;
  vwrite32(maddr+SIS3300_STOP_DELAY,&lval);
}


void sis3300_writeth(unsigned int maddr,int gr,int val){
  int graddr;
  int lval;

  graddr = SIS3300_TRIGGER_THRESHOLD_ADC12+SIS3300_BANKSIZE*gr;

  lval = val;
  vwrite32(maddr+graddr,&lval);
}

int sis3300_readth(unsigned int maddr,int gr){
  int graddr;
  int lval;

  graddr = SIS3300_TRIGGER_THRESHOLD_ADC12+SIS3300_BANKSIZE*gr;

  vread32(maddr+graddr,(int *)(&lval));
  
  return lval;
}
