#include "v775.h"

int v775_segdata(unsigned int maddr){
  int wordcnt;

  wordcnt = 0;
  vread32(maddr+V775_OUTBUFF,(int *)(data+mp));
  mp += 2;
  segmentsize += 2;
  wordcnt++;

  if((data[mp-1] & V775_TYPE_MASK_S) == V775_HEADER_BIT_S){
    while(wordcnt < 34){
      /* vread32(maddr+V775_OUTBUFF+wordcnt*32,(int *)(data+mp)); */
      vread32(maddr+V775_OUTBUFF,(int *)(data+mp));
      mp += 2;
      segmentsize += 2;
      wordcnt++;
      if((data[mp-1] & (V775_TYPE_MASK_S)) != V775_DATA_BIT_S){
        break;
      }
    }
  }

  return segmentsize;
}


int v775_segmod(short segid,unsigned int maddr){
  int tmp,wordcnt;

  wordcnt = 0;
  tmp = mp;

  mp += 2;

  vread32(maddr+V775_OUTBUFF,(int *)(data+mp));
  mp += 2;
  wordcnt++;

  if((data[mp-1] & V775_TYPE_MASK_S) == V775_HEADER_BIT_S){
    while(wordcnt < 34){
      vread32(maddr+V775_OUTBUFF+wordcnt*32,(int *)(data+mp));
      mp += 2;
      wordcnt++;
      if((data[mp-1] & (V775_TYPE_MASK_S)) != V775_DATA_BIT_S){
        break;
      }
    }
  }
  mp = tmp;
  data[mp++] = wordcnt*2+2;                  /* Write word count */
  data[mp++] = segid;                      /* Write segment ID */

  mp += wordcnt*2;

  eventsize += wordcnt*2 + 2;                    /* Count up event size */

  return wordcnt*2 + 2;
}
