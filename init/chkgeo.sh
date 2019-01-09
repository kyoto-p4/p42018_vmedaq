#!/bin/sh

V1X90_MAIKO_ADR=0x00050000
V1X90_BDC1_ADR=0x00060000
V1X90_BDC2_ADR=0x00070000

MAIKO_GEO_REG=$((V1X90_MAIKO_ADR+0x100E))
BDC1_GEO_REG=$((V1X90_BDC1_ADR+0x100E))
BDC2_GEO_REG=$((V1X90_BDC2_ADR+0x100E))


echo " "
echo "//////////////////////////////////////"
echo "Cheking GEO addresses..."
echo " "

echo "MAIKo V1190:"
cmd=$(printf "cmdvme -wr 0x%x" $MAIKO_GEO_REG)
$cmd
echo " "

echo "BDC1 V1190:"
cmd=$(printf "cmdvme -wr 0x%x" $BDC1_GEO_REG)
$cmd
echo " "

echo "BDC2 V1190:"
cmd=$(printf "cmdvme -wr 0x%x" $BDC2_GEO_REG)
$cmd
echo "//////////////////////////////////////"
echo " "