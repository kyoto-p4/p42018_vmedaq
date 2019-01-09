
void sca(void){
#ifdef USE_SCALER
  //  v560_read_scaler32(V560_ADR);
  v560_read_scaler32_mod(V560_ADR);
  //  v560_scaler_clear(V560_ADR);
#endif
}
