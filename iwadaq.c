/*
  iwadaq.c
  Feb 24, 2002

*/

void init_mem(void);
void init_block(void);
void init_segment(void);
int end_block(void);
int end_event(void);
int end_segment(void);
int read_segdata(short c,short n,short a,short f);
int read_segndata(short len,short c,short n,short a,short f);
int read_segsdata(short len,short c,short n,short a,short f);
int read_segbdata(short len,short c,short n,short a,short f);
void write_mod(short c,short n,short a,short f,short *data);
void write_data(short c,short n,short a,short f,short data);
void control_mod(short c,short n,short a,short f);
int get_mp(void);

static int mp,eventsize,segmenthmp,segmentsize;
int i;

#define DMAMAX 4096
#define DMADELAYMAX 10000 // > 10ms
#ifdef VME
static unsigned int dmadata[DMAMAX];
#endif
#ifndef DBUFF
static unsigned short data[BLKSIZE/2];
#else
unsigned short *data;
static unsigned short databuff[BLKSIZE];
volatile int dn=0;
#endif

int blksize = BLKSIZE;

#ifndef DBUFF
void init_mem(void){
  memset(data,0,blksize);                   /* Memory set to '0' */
}
#else
void chmem(void){
  if(dn==0){
    data = databuff + BLKSIZE/2;
    dn = 1;
  }else{
    data = databuff;
    dn = 0;
  }
}

void init_mem(void){
  dn = 1;
  chmem();
  memset(databuff,0,sizeof(databuff));             /* Memory set to '0' */
}
#endif

void end_driver(void){
}


void init_block(void){
  eventsize = 0;
  memset(data,0,8);

  data[0] = 0;
  data[1] = 0x0020;
  data[2] = 0x2000;
  mp = 4;
}

void init_segment(void){
  data[mp++] = 0xcc77;
  segmenthmp = mp;                         /* Store mp for segment size */
  segmentsize = 0;                         /* Initialize segment size */
  mp++;
}

int end_block(void){
  data[mp++] = 0xffff;                     /* Write END */
  data[mp++] = 0xffff;                     /* Write END */
  data[3] = eventsize + 4;

  return mp;
}

int end_segment(void){
  data[segmenthmp] = segmentsize+2;          /* Write segment size */
  eventsize += segmentsize+2;              /* Count up event size */

  return mp;
}

int end_event(void){
  return mp;
}

int read_segdata(short c,short n,short a,short f){
  read16(CNAFGEN(c,n,a,f),data+mp);        /* Read data */
  mp++;
  segmentsize++;                           /* Count up segment size */

  return segmentsize;
}

int read_segndata(short len,short c,short n,short a,short f){
  for(i=0;i<len;i++){
    read16(CNAFGEN(c,n,a,f),data+mp);        /* Read data */
    a++;
    mp++;
    segmentsize++;                           /* Count up segment size */
  }

  return segmentsize;
}

int read_segsdata(short len,short c,short n,short a,short f){
#ifdef K2915
  block_read16(QSCAN,CNAFGEN(c,n,a,f),data+mp,len); /* Read data */
  mp += len;
  segmentsize += len;
#else
  for(i=0;i<len;i++){
    read16(CNAFGEN(c,n,a,f),data+mp);        /* Read data */
    a++;
    mp++;
    segmentsize++;                           /* Count up segment size */
  }
#endif

  return segmentsize;
}

int read_segbdata(short len,short c,short n,short a,short f){
  block_read16(QIGNORE,CNAFGEN(c,n,a,f),data+mp,len); /* Read data */
  mp += len;
  segmentsize += len;

  return segmentsize;
}

void write_mod(short c,short n,short a,short f,short *data){
  write16(CNAFGEN(c,n,a,f),data);
}

void write_data(short c,short n,short a,short f,short data){
  write16(CNAFGEN(c,n,a,f),&data);
}

void control_mod(short c,short n,short a,short f){
  control(CNAFGEN(c,n,a,f));
}


void delay_us(void){
  outb(1,0x80);
}

int get_mp(void){
  return mp;
}

