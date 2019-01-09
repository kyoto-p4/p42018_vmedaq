extern void k2915_init_register(int irq,int ior,int pbor);
extern int k2915_check_done(int try);
extern int k2915_check_emp_fifo(extern void);
extern int k2915_check_full_fifo(extern void);
extern int k2915_check_lam(extern void);
extern void k2915_read_lam(int *data);
extern void k2915_control(int cnaf);
extern void k2915_read16(int cnaf,short *data);
extern void k2915_read24(int cnaf,short *data);
extern void k2915_write16(int cnaf,short *data);
extern void k2915_write24(int cnaf,int *data);
extern int k2915_block_read16(int mode,int cnaf,short *data,int count);
extern int k2915_block_read24(int mode,int cnaf,unsigned int *data,int count);
extern int k2915_dma_block_read16(int mode,int cnaf,int count);
extern int k2915_dma_block_read24(int mode,int cnaf,int count);
extern void k2915_crate_reset(extern void);
extern void k2915_rfs_enable_interrupt(extern void);
extern void k2915_rfs_disable_interrupt(extern void);
extern void k2915_pci_enable_interrupt(extern void);
extern void k2915_pci_clear_interrupt(extern void);
extern void k2915_crate_enable_lam(int lammask);
extern void k2915_crate_disable_lam(extern void);
extern void k2915_crate_z(int c);
extern void k2915_crate_c(int c);
extern void k2915_crate_seti(int c);
extern void k2915_crate_deli(int c);
extern void k2915_get_csrdata(int *ret);
extern void k2915_get_bmcsdata(int *ret);
