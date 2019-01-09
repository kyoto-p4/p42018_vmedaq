#define set_amsr univ_set_amsr
#define vread16  univ_vread16
#define vread32  univ_vread32
#define vwrite16 univ_vwrite16
#define vwrite32 univ_vwrite32
#define vme_define_intlevel   univ_define_intlevel
#define vme_enable_interrupt  univ_enable_interrupt
#define vme_disable_interrupt univ_disable_interrupt
#define vme_check_interrupt   univ_check_interrupt
#define vme_read_intvector    univ_read_intvector

#ifdef VMEINT
#define get_irq  univ_get_irq
#endif

#define vme_dma_vread32_start univ_dma_vread32_start
#define vme_dma_vread32_store univ_dma_vread32_store
