void stop(void){

#ifdef USE_MADC32
  madc32_stop_acq(MADC32ADR);
  madc32_stop_acq(MADC32p2ADR);
#endif


  /* Visual Scaler Stop */
  lupo_pout(LUPO_ADR, LUPO_IO_SCA_STOP);
  lupo_pout(LUPO_ADR, LUPO_IO_SCA_STOP);
  printk("DAQ stop\n\n");

}
