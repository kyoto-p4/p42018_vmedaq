typedef struct _vme_bus_handle *vme_bus_handle_t;
typedef struct _vme_master_handle *vme_master_handle_t;
typedef struct _vme_dma_handle *vme_dma_handle_t;
typedef struct _vme_interrupt_handle *vme_interrupt_handle_t;

struct vmectl_window_t
{
    void *id;                     /* Handle id, assigned by the driver */
    uint64_t vaddr;               /* VMEbus address */
    unsigned long size;
    int am;                       /* Address modifier */
    long flags;                   /* If 0, driver default value is used */
    void *paddr;                  /* Physical address to use */
};

struct _vme_master_handle {                                                     
  struct vmectl_window_t ctl;
  int magic;                                                                  
  void *vptr;                                                                 
};                                                                              

                                                                                
struct vmectl_dma_t
{
  void *id;                     /* Handle id, assigned by the driver */
  uint64_t vaddr;               /* VMEbus address */
  unsigned long size;           /* Number of bytes to transfer */
  unsigned long offset;         /* Offset from the base physical address for
								 *                                    transfers */
  int am;                       /* Address modifer */
  long flags;
  void *paddr;                  /* Physical address to use */
};

struct _vme_dma_handle {
  struct vmectl_dma_t dctl;    /* ioctl structure, must be first */
  int magic;
  size_t size;
  void *vptr;
};

struct __vme_interrupt {
	int level;
	int count;
	vme_interrupt_handle_t handles;
};

struct _vme_interrupt_handle {
	int magic;
	struct __vme_interrupt *interrupt;
	uint8_t vector;
	int flags;
	union {
		int data;
		siginfo_t siginfo;
	} notify;
	pid_t pid;
	void *id;
	vme_interrupt_handle_t next;
	vme_interrupt_handle_t prev;
};

vme_bus_handle_t bus;
vme_dma_handle_t dhandle;
struct vme_master_handle_t *handle;
struct vmectl_window_t window;

extern int vme_master_window_create(vme_bus_handle_t bus_handle,
		   vme_master_handle_t * handle, uint64_t vme_addr,
		   int am, size_t size, int flags, void *phys_addr);
extern void *vme_master_window_phys_addr(vme_bus_handle_t bus_handle,
					 vme_master_handle_t handle);
extern int vme_master_window_release(vme_bus_handle_t bus_handle,				    vme_master_handle_t handle);

extern int vme_dma_buffer_create(vme_bus_handle_t bus_handle,								 vme_dma_handle_t * handle, size_t size,
				 int flags, void *phys_addr);
extern int vme_dma_buffer_release(vme_bus_handle_t bus_handle, vme_dma_handle_t handle);

extern void *vme_dma_buffer_phys_addr(vme_bus_handle_t bus_handle,
				      vme_dma_handle_t handle);
extern int vme_dma_read2(vme_bus_handle_t bus_handle, vme_dma_handle_t handle,
			unsigned long offset, uint64_t vme_addr, int am,
			size_t nbytes, int flags);

extern int vme_init(vme_bus_handle_t *handle);
extern int vme_term(vme_bus_handle_t handle);
extern int vme_interrupt_attach(vme_bus_handle_t bus_handle,			                          vme_interrupt_handle_t * handle, int level, int vector,
				int flags, void *data);
extern int vme_interrupt_release(vme_bus_handle_t bus_handle,
				 vme_interrupt_handle_t handle);
extern int vme_interrupt_irq(vme_bus_handle_t bus_handle, int *irq);
extern int vme_interrupt_clear(vme_bus_handle_t bus_handle,
			       vme_interrupt_handle_t handle);
extern int vme_interrupt_asserted(vme_bus_handle_t bus_handle,
				  vme_interrupt_handle_t handle);
extern int vme_interrupt_vector(vme_bus_handle_t bus_handle,
				vme_interrupt_handle_t handle, int *vector);
extern int vme_interrupt_enable(vme_bus_handle_t bus_handle,
			       vme_interrupt_handle_t handle);
extern int vme_interrupt_disable(vme_bus_handle_t bus_handle,
			       vme_interrupt_handle_t handle);

#define UNIV_DCTL__L2V                      0x80000000   
typedef enum
{
    VME_INTERRUPT_BLOCKING,       /* Interrupt attach should block */
      VME_INTERRUPT_SIGEVENT,       /* Generate a signal on interrupt */
      VME_INTERRUPT_RESERVE         /* Assume control of the interrupt level */
}
vme_interrupt_reply_t;
