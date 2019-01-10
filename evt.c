void evt(void){
  int i, segid = 1;

#ifdef TEST
  //  madc32_irq_level(MADC32ADR, 0);
#endif
  /* Initialize */
  init_event(1);

#ifdef USE_MADC32
  init_segment(segid);
  madc32_segdata(MADC32ADR);
  end_segment();
  madc32_readout_reset(MADC32ADR, 1); // added on 16/10/05

  init_segment(segid++);
  madc32_segdata(MADC32p2ADR);
  end_segment();
  madc32_readout_reset(MADC32p2ADR, 1); // added on 16/10/05
#endif

#ifdef USE_CAEN
  init_segment(segid++);
  v1X90_segdata(V1190_ADR);
  end_segment();
#endif

#ifdef USE_SCALER
//  init_segment(segid++);
//  v560_segdata(V560_ADR);
//  end_segment();
#endif

  /* Busy Clear */
  if(mp<MAXBUFF){
    lupo_pout(LUPO_ADR, LUPO_IO_BUSYCL);
    mpflag = 0;
  }else mpflag = 1;

  end_event();


}
