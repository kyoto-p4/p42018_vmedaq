#define BBRL
#define MAXBUFF 0x1e00
#define DBUFF

#define VME
#define VMEINT
#define INTLEVEL 3
#define SBS620

#define USE_MADC32
#define USE_CAEN
#define USE_SCALER
#define USE_V560
#define USE_LUPO

// define base address 
#define MADC32ADR 0x20020000
#define MADC32p2ADR 0x32000000
#define V1190_ADR 0xee0d0000
#define V560_ADR 0x50000000
#define LUPO_ADR 0x11100000

// define scaler ID
#define V560_NCH 16
#define V560_SCRID 20
#define V560_REGSIZE 0x2000

#define TEST // TEST FOR MADC INTERRUPT, OTHERWISE V1190 INTERRUPT
