void startup(void){
  short sval;
  printk("entering startup()\n");
  /* Startup Function */
  vme_define_intlevel(INTLEVEL);
  vme_read_intvector();

  /* LUPO pulse width set */
  lupo_pulse_width(LUPO_ADR, 30);
  /* Visual Scaler Reset */
  lupo_pout(LUPO_ADR, LUPO_IO_SCA_CLR);  

  /* Disable Interrupt except for Interrupt Module */

  lupo_dis_interrupt(LUPO_ADR);
  //  lupo_pulse_width(LUPO_ADR, 5);
  lupo_pout(LUPO_ADR, LUPO_IO_BUSYCL);
  //  lupo_pulse_width(LUPO_ADR, 30);

#ifdef USE_MADC32
  madc32_stop_acq(MADC32ADR);
  madc32_resol_4k(MADC32ADR); //set resolution
  madc32_input_range(MADC32ADR, 0x1); //input range (0:4V, 1:10V, 2:8V)
  madc32_use_gg(MADC32ADR, 0x1);  // use internal gate
  madc32_hold_delay(MADC32ADR, 0, 0);  // gate delay (*50+25ns) 
  madc32_hold_width(MADC32ADR, 0, 160);  // gate width (*50 ns) 
  madc32_nim_busy(MADC32ADR, 0x1); // NIM3 output conf (output internal gate)
  madc32_module_id(MADC32ADR, 0); //set module id
  unsigned int ich;
  for(ich = 0; ich < MADC32_NCH; ich++){
    madc32_threshold(MADC32ADR, ich, 0);
  }

  madc32_stop_acq(MADC32p2ADR);
  madc32_resol_4k(MADC32p2ADR); //set resolution
  madc32_input_range(MADC32p2ADR, 0x1); //input range (0:4V, 1:10V, 2:8V)
  madc32_use_gg(MADC32p2ADR, 0x1);  // use internal gate
  madc32_hold_delay(MADC32p2ADR, 0, 0);  // gate delay 
  madc32_hold_width(MADC32p2ADR, 0, 160);  // gate width (*50 ns) 
  madc32_nim_busy(MADC32p2ADR, 0x1); // NIM3 output conf (output internal gate)
  madc32_module_id(MADC32p2ADR, 1); //set module id
  for(ich = 0; ich < MADC32_NCH; ich++){
    madc32_threshold(MADC32p2ADR, ich, 0);
  }

#ifdef TEST
  madc32_irq_level(MADC32ADR, 3);
#else
  madc32_irq_level(MADC32ADR, 0);
  madc32_irq_level(MADC32p2ADR, 0);
#endif
  madc32_irq_threshold(MADC32ADR, 1); //number of events/irq
  madc32_max_transfer_data(MADC32ADR, 1);
  /* for time stamp from here */
  madc32_marking_type(MADC32ADR, 0x1);  //00->event counter, 01->time stamp 11->extended time stamp
  madc32_NIM_gat1_osc(MADC32ADR, 0x1);  //0->gate1 input, 1->clock input
  madc32_ts_sources(MADC32ADR, 0x1);    //0->VME, 1->external
  //  madc32_fifo_reset(MADC32ADR,1); //reset FIFO
  madc32_fifo_reset(MADC32ADR,0); 
  //  madc32_readout_reset(MADC32ADR,1); //reset FIFO  
  madc32_reset_ctr_ab(MADC32ADR); // reset event counter or timestamp
  madc32_start_acq(MADC32ADR); //start MADC32
  madc32_readout_reset(MADC32ADR,1); 
  printk("MADC32 initialize done.\n");
  /* to here */
 madc32_irq_threshold(MADC32p2ADR, 1); //number of events/irq
  madc32_max_transfer_data(MADC32p2ADR, 1);
  /* for time stamp from here */
  madc32_marking_type(MADC32p2ADR, 0x1);  //00->event counter, 01->time stamp 11->extended time stamp
  madc32_NIM_gat1_osc(MADC32p2ADR, 0x1);  //0->gate1 input, 1->clock input
  madc32_ts_sources(MADC32p2ADR, 0x1);    //0->VME, 1->external
  //  madc32_fifo_reset(MADC32p2ADR,1); //reset FIFO
  madc32_fifo_reset(MADC32p2ADR,0); 
  //  madc32_readout_reset(MADC32p2ADR,1); //reset FIFO  
  madc32_reset_ctr_ab(MADC32p2ADR); // reset event counter or timestamp
  madc32_start_acq(MADC32p2ADR); //start MADC32
  madc32_readout_reset(MADC32p2ADR,1); 
  printk("MADC32 initialize done.\n");
  /* to here */
#endif  

#ifdef USE_CAEN
  v1X90_evt_reset(V1190_ADR);
#ifdef TEST
  v1X90_int_level(V1190_ADR,0x0);
#else 
  v1X90_int_level(V1190_ADR,0x3);
#endif
  v1X90_almost_full(V1190_ADR, 0xffff);
  v1X90_cnt_reg(V1190_ADR, 0x028);
#endif

#ifdef USE_SCALER
  v560_scaler_clear(V560_ADR);


#endif

  /* Visual Scaler Start */
  lupo_pout(LUPO_ADR, LUPO_IO_SCA_START);
  lupo_pout(LUPO_ADR, LUPO_IO_BUSYCL);
  printk("DAQ start.\n");


}
