/* nbbqdrv.c
 *
 * Dec. 8, 2010
 *
 *  Hidetada Baba (baba@ribf.riken.jp)
 *
*/


#include <linux/version.h>
#if LINUX_VERSION_CODE >=  0x020600
#if defined(USE_MODVERSIONS) && USE_MODVERSIONS
#  define MODVERSIONS
#  include <config/modversions.h>
#endif
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#if LINUX_VERSION_CODE >=  0x020600
#include <linux/init.h>
#include <linux/interrupt.h>
#endif
#include <linux/ioctl.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/sched.h>
#if LINUX_VERSION_CODE <  0x020600
#include <linux/config.h>
#endif
#include <linux/pci.h>
#include <linux/errno.h>
#include <linux/poll.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "bbmodules.h"

#ifdef VMIVME
#include "bbuniverse.h"
#endif

#include "nbbqctrl.h"
#include "nbbqdrv.h"
#include "nbbqio.h"

#if LINUX_VERSION_CODE >=  0x020410
MODULE_LICENSE("GPL");
#endif
#if LINUX_VERSION_CODE >=  0x020600
MODULE_AUTHOR("Hidetada Baba");
#ifndef MOD_IN_USE
#define MOD_IN_USE module_refcount(THIS_MODULE)
#endif
#ifndef MOD_INC_USE_COUNT
#define MOD_INC_USE_COUNT
#endif
#ifndef MOD_DEC_USE_COUNT
#define MOD_DEC_USE_COUNT
#endif
#endif

#include "./common.h"
#include "segidlist.h"
#ifndef BLKSIZE
#define BLKSIZE 0x4000
#endif

#ifdef BBRL
#include "bbrl.c"
#endif
#ifdef BBDAQ
#include "bbdaq.c"
#endif
#ifdef IWADAQ
#include "iwadaq.c"
#endif

#ifdef VMIVME
#include "univfunc.c"
#endif

#ifdef CCNET
//#include "ccnetevt.c"
#endif

#include "usemodules.h"

#include "startup.c"
#include "evt.c"
#include "stop.c"
#include "sca.c"
#include "clear.c"

#ifdef VMIVME
// for vmeuniv
vme_master_handle_t *master_p;
vme_interrupt_handle_t interrupt;
#ifndef INTVEC
#define INTVEC 0
#endif
#endif

static int nbbqdrv_major = NBBQ_MAJOR;
static const char *nbbqdrv_name = "nbbqdrv";
#if LINUX_VERSION_CODE >= 0x020600
static int dev_id;
#else
static char *irq_name = "irq_nbbq";
#endif

static volatile int startflag = 0;

ssize_t nbbqdrv_read(struct file *file,char *buff,size_t count,loff_t *pos);
static int nbbqdrv_open(struct inode* inode, struct file* filep);
static int nbbqdrv_release(struct inode* inode, struct file* filep);
static int nbbqdrv_ioctl(struct inode *inode, struct file *filep,
			 unsigned int cmd, unsigned long arg);
static unsigned int nbbqdrv_poll(struct file *file,poll_table *wait);
#if LINUX_VERSION_CODE >= 0x020600
static irqreturn_t nbbqdrv_interrupt(int irq, void* dev_id, struct pt_regs* regs);
#else
static void nbbqdrv_interrupt(int irq,void *dev_id,struct pt_regs* regs);
#endif

static int girq,chkbuff,chkblk;
#if LINUX_VERSION_CODE >=  0x020400
wait_queue_head_t nbbq_wait_queue;
#else
struct wait_queue *nbbq_wait_queue = NULL;
#endif


#if LINUX_VERSION_CODE >=  0x020600
static struct file_operations nbbqdrv_fops = {
  .read = nbbqdrv_read,
  .poll = nbbqdrv_poll,
  .ioctl = nbbqdrv_ioctl,
  .open = nbbqdrv_open,
  .release = nbbqdrv_release,
};
#else
#if LINUX_VERSION_CODE >=  0x020400
static struct file_operations nbbqdrv_fops = {
  read: nbbqdrv_read,
  poll: nbbqdrv_poll,
  ioctl: nbbqdrv_ioctl,
  open: nbbqdrv_open,
  release: nbbqdrv_release,
};
#else
static struct file_operations nbbqdrv_fops = {
  NULL,             // loff_t  llseek
  nbbqdrv_read,     // ssize_t read
  NULL,             // ssize_t write
  NULL,             // int     readdir
  nbbqdrv_poll,     // uint    poll
  nbbqdrv_ioctl,    // int     ioctl
  NULL,             // int     mmap
  nbbqdrv_open,     // int     open
  NULL,             // int     flush
  nbbqdrv_release,  // int     release
};
#endif
#endif

#ifdef DBUFF
volatile static int stopflag = 0;
volatile int drn = 0;
#endif

#if LINUX_VERSION_CODE >= 0x020600
int nbbqdrv_init_module(void){
#else
int init_module(void){
#endif
  int ret;

  ret = register_chrdev(nbbqdrv_major,nbbqdrv_name,&nbbqdrv_fops);
  if(ret < 0){
    printk("%s : can't regist.\n",nbbqdrv_name);
    return ret;
  }
  if(nbbqdrv_major == 0) {
    nbbqdrv_major = ret;
  }

#ifdef VMIVME
  vme_init(&bus);
  vme_interrupt_irq(bus, &girq);
#else
  girq = get_irq();
#endif

  chkbuff = 0;

  

#if LINUX_VERSION_CODE >=  0x020400
  init_waitqueue_head(&nbbq_wait_queue);
#endif

#ifdef VMEINT
    vme_disable_interrupt();
#else
    rfs_disable_interrupt();
#endif

#if LINUX_VERSION_CODE >= 0x020600
#if LINUX_VERSION_CODE >= 0x020615
  ret = request_irq(girq,(irq_handler_t)nbbqdrv_interrupt,IRQF_SHARED,nbbqdrv_name, &dev_id);
#else
  ret = request_irq(girq,nbbqdrv_interrupt,SA_SHIRQ,nbbqdrv_name,&dev_id);
#endif
#else
  ret = request_irq(girq,nbbqdrv_interrupt,SA_SHIRQ,nbbqdrv_name,irq_name);
#endif

#ifdef VMIVME
  vme_interrupt_attach(bus, &interrupt, INTLEVEL, INTVEC,
		       VME_INTERRUPT_RESERVE, NULL);
#endif

  
  printk("%s : nbbqdrv was installed (irq %d).\n",nbbqdrv_name,girq);

  return 0;
}

#if LINUX_VERSION_CODE >= 0x020600
void nbbqdrv_cleanup_module(void){
#else
void cleanup_module(void){
#endif

#ifdef VMIVME  
  vme_interrupt_release(bus, interrupt);
#endif

#if LINUX_VERSION_CODE >= 0x020600
  free_irq(girq,&dev_id);
#else
  free_irq(girq,irq_name);
#endif
  
#ifdef VMIVME
  vme_term(bus);
#endif


  unregister_chrdev(nbbqdrv_major,nbbqdrv_name);
  printk("%s: nbbqdrv was unregistered.\n", nbbqdrv_name);
}

static int nbbqdrv_open(struct inode* inode, struct file* filep){

  if(MOD_IN_USE){
    return -EBUSY;
  }

#ifdef CCNET
  ccnet_check_lam();
#endif

#ifdef VMIVME
  vme_interrupt_clear(bus, interrupt);
  vme_interrupt_enable(bus, interrupt);
#endif

  chkblk = 0;
  chkbuff = 0;
#ifdef DBUFF
  stopflag = 0;
  drn = 0;
#endif

  init_mem();

  MOD_INC_USE_COUNT;
  startflag = 1;
  startup();

#ifdef VMIVME
  
#else
#ifdef VMEINT
  vme_enable_interrupt();
#else
  rfs_enable_interrupt();
#endif
#endif

  return 0;
}

static int nbbqdrv_release(struct inode* inode, struct file* filep){

  chkbuff = 0;

#ifndef VMIVME
#ifdef VMEINT
  vme_disable_interrupt();
#else
  rfs_disable_interrupt();
#endif
#endif

  MOD_DEC_USE_COUNT;

  return 0;
}

ssize_t nbbqdrv_read(struct file *file,char *buff,size_t count,loff_t *pos){
  unsigned long x;

#ifndef DBUFF
  chkbuff = 0;
  x = copy_to_user(buff,data,sizeof(data));
#else
  x = copy_to_user(buff,(char *)(databuff+BLKSIZE/2*drn),BLKSIZE);
  drn++;
  if(drn == 2) drn = 0;

  if(chkbuff == 2){
    if(stopflag == 0){
#ifndef VMIVME
#ifdef VMEINT
      vme_enable_interrupt();
#else
      rfs_enable_interrupt();
#endif
#endif
    }
    clear();
    if(stopflag == 0){
#ifdef K2915
      rfs_enable_interrupt();
#endif
    }
  }

  chkbuff--;
  if(chkbuff < 0){
    chkbuff = 0;
  }
#endif

  return 1;
}


static int nbbqdrv_ioctl(struct inode* inode, struct file *filep,
			 unsigned int cmd, unsigned long arg){

  char flag;
  unsigned long x;

  switch(cmd){
  case NBBQ_STOP:
  case NBBQ_STOPB:
#ifndef VMIVME
#ifdef VMEINT
    vme_disable_interrupt();
#else
    rfs_disable_interrupt();
#endif
#else
#endif
    startflag = 0;

#ifdef DBUFF
    stopflag = 1;
#endif
    if(chkblk == 1){
      end_block();
#ifdef BBRL
#ifdef VMEINT
      /* none */
#else
      //crate_seti(0);
#endif
      sca();
#endif
#ifndef DBUFF
      flag = 1;
#else
      chkbuff++;
      flag = chkbuff;
#endif
      x = copy_to_user((void *)arg,&flag,1);
#ifndef DBUFF
      chkbuff = 1;
#endif
      chkblk = 0;
    }else{
      flag = 0;
      x = copy_to_user((void *)arg,&flag,1);
    }
    stop();

#ifdef VMIVME
    vme_interrupt_disable(bus, interrupt);
    vme_interrupt_clear(bus, interrupt);
#endif
    break;
  case NBBQ_EI:
#ifndef DBUFF
#ifndef VMIVME
#ifdef VMEINT
    vme_define_intlevel(INTLEVEL);
    vme_enable_interrupt();
#else
    crate_define_lam(LAMN);
    rfs_enable_interrupt();
#endif
    clear();
#else
    clear();
    vme_interrupt_enable(bus, interrupt);
    vme_interrupt_clear(bus, interrupt);
#endif
#ifdef K2915
    rfs_enable_interrupt();
#endif
#endif
    break;
  }

  return 1;
}

static unsigned int nbbqdrv_poll(struct file *file,poll_table *wait){
  poll_wait(file,&nbbq_wait_queue,wait);
  if(chkbuff > 0){
    return POLLIN;
  }else{
    return 0;
  }
}

#if LINUX_VERSION_CODE >= 0x020600
static irqreturn_t nbbqdrv_interrupt(int irq, void* dev_id, struct pt_regs* regs){
#else
static void nbbqdrv_interrupt(int irq, void* dev_id, struct pt_regs* regs){
#endif

#ifdef VMIVME
  int vector;
#endif

  // When driver is not opened by nbbqcom,
  // return IRQ_HANDLED
  if(!startflag){
#ifdef VMEINT
    vme_disable_interrupt();
#else
    rfs_disable_interrupt();
#endif
    wake_up_interruptible(&nbbq_wait_queue);

#if LINUX_VERSION_CODE >= 0x020600
    return IRQ_HANDLED;
#else
    return;
#endif
  }

#ifndef VMIVME
  if(dev_id == NULL){
#if LINUX_VERSION_CODE >= 0x020600
    return IRQ_NONE;
#else
    return;
#endif
  }
#endif

#ifdef VMIVME
  if(vme_interrupt_asserted(bus, interrupt)){
    vme_interrupt_vector(bus, interrupt, &vector);
    vme_interrupt_disable(bus, interrupt);
    //vme_interrupt_clear(bus, interrupt);
    
#endif

  if(chkblk == 0){
    init_block();
    chkblk = 1;
  }
#ifndef VMIVME
  if(
#ifdef VMEINT
     vme_check_interrupt() != 0
#else
     check_lam() != 0
#endif
     ){
#ifdef VMEINT
    vme_disable_interrupt();
#else
    rfs_disable_interrupt();
#endif
#endif
    evt();
    if(end_event() > MAXBUFF){
      end_block();
#ifdef BBRL
#ifdef VMEINT
      /* none */
#else
      //crate_seti(0);
#endif
      sca();
#endif
      chkblk = 0;
#ifndef DBUFF
      chkbuff = 1;
#else
      chkbuff++;
      chmem();
      if(chkbuff != 2){
#ifndef VMIVME
#ifdef VMEINT
	vme_enable_interrupt();
#else
	rfs_enable_interrupt();
#endif
	clear();
#else
	clear();
	vme_interrupt_enable(bus, interrupt);
	vme_interrupt_clear(bus, interrupt);
#endif
#ifdef K2915
	rfs_enable_interrupt();
#endif
      }
#endif
    }else{
#ifndef VMIVME
#ifdef VMEINT
      vme_enable_interrupt();
#else
      rfs_enable_interrupt();
#endif
      clear();
#else
      clear();
      vme_interrupt_enable(bus, interrupt);
      vme_interrupt_clear(bus, interrupt);
#endif
#ifdef K2915
      rfs_enable_interrupt();
#endif
    }
  }else{
#if LINUX_VERSION_CODE >= 0x020600
    return IRQ_NONE;
#else
    return;
#endif
  }
  
  wake_up_interruptible(&nbbq_wait_queue);

#if LINUX_VERSION_CODE >= 0x020600
  return IRQ_HANDLED;
#endif
}

#if LINUX_VERSION_CODE >= 0x020600
module_init(nbbqdrv_init_module);
module_exit(nbbqdrv_cleanup_module);
#endif
