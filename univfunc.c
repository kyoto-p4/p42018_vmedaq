extern void univ_init_window(unsigned int vaddr, unsigned int size, int am, int n);
extern void univ_end_window(int n);
extern void univ_map_read16(unsigned int off, short *data, int n);
extern void univ_map_read32(unsigned int off, int *data, int n);
extern void univ_map_write16(unsigned int off, short *data, int n);
extern void univ_map_write32(unsigned int off, int *data, int n);
extern void univ_init_dma(unsigned int vaddr, unsigned int size, int n);
extern void univ_end_dma(int n);
extern void univ_dma_read(char *data, unsigned int size, int n);

int univ_dma_segdata(int size, int n){
  univ_dma_read((char *)(data+mp), size, n);

  segmentsize += size/2;
  mp += size/2;
  
  return segmentsize;
}
