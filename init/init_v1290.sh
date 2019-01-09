#!/bin/sh

# module base address
V1X90ADR=0x52000000

# configuration parameters
GEOADR=9
WIN_WID=60    #25ns unit
WIN_OFS=-40   #25ns unit
SW_MRG=0x8    #25ns unit
REJ_MRG=0x4   #25ns unit
EDGE_DET=0x3  #0:pair, 1:trading, 2:leading, 3:lead & trad
LSB=0x2       #0:800ps, 1:200ps, 2:100ps, 3:25ps(only for V1290)
READ_RES=0x2  #0:800ps, 1:200ps, 2:100ps, 3:25ps(only for V1290)

# module VME registers
V1X90_GEO_REG=$((V1X90ADR+0x100E))
V1X90_MOD_RST_REG=$((V1X90ADR+0x1014))
V1X90_SOFT_CLE=$((V1X90ADR+0x1016))
V1X90_MICRO_REG=$((V1X90ADR+0x102E))
V1X90_HAND_REG=$((V1X90ADR+0x1030))


# OPCODE
V1X90OP_TRG_MATCH=0x0000
V1X90OP_SET_CONT=0x0100
V1X90OP_SET_WIN_WID=0x1000
V1X90OP_SET_WIN_OFS=0x1100
V1X90OP_SET_SW_MRG=0x1200
V1X90OP_SET_REJ_MRG=0x1300
V1X90OP_EN_SUB_TRG=0x1400
V1X90OP_DIS_SUB_TRG=0x1500
V1X90OP_SET_DETECT=0x2200
V1X90OP_SET_LSB=0x2400
V1X90OP_EN_HEAD_TRAIL=0x3000
V1X90OP_DIS_HEAD_TRAIL=0x3100
V1X90OP_EN_CH8=0x4008
V1X90OP_EN_ALL_CH=0x4200
V1X90OP_DIS_ALL_CH=0x4300

# function to check whether the module is ready for configuration
function chk_status(){
cmd=$(printf "cmdvme -wr 0x%x" $V1X90_HAND_REG)
cnt=0
res=0
while [ $res -ne 1 ]
do
    usleep 1000
    res=`$cmd|awk '{print substr($0, 6, 1)}'`

    # timeout in 2sec
    cnt=$((cnt+1))
    if [ $cnt -gt 2000 ]
    then
	echo "Error, cannot conigure the module."
	exit 1
    fi
done
}

echo "------------------------------"
echo "V1X90 initialization start!!"
printf "base address= %s\n" $V1X90ADR
echo "------------------------------"

# geo address
printf "GEO address= %s\n" $GEOADR
cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_GEO_REG $GEOADR)
$cmd
cmd=$(printf "cmdvme -ww 0x%x 1" $V1X90_MOD_RST_REG)
$cmd
cmd=$(printf "cmdvme -ww 0x%x 1" $V1X90_SOFT_CLE)
$cmd

# Trigger matching mode 
echo "configure to trigger matching mode..."
chk_status $V1X90ADR
cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $V1X90OP_TRG_MATCH)
$cmd

# Set continuous storage mode
#echo "configure to continuous storage mode..."
#chk_status $V1X90ADR
#cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $V1X90OP_SET_CONT)
#$cmd


# Set the window width
echo "setting window width..."
chk_status $V1X90ADR
cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $V1X90OP_SET_WIN_WID)
$cmd

chk_status $V1X90ADR
cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $WIN_WID)
$cmd


# Set the window offset
echo "setting window offset..."
chk_status $V1X90ADR
cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $V1X90OP_SET_WIN_OFS)
$cmd

chk_status $V1X90ADR
cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $WIN_OFS)
$cmd


# Set the extra search margin
echo "setting the extra search margin..."
chk_status $V1X90ADR
cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $V1X90OP_SET_SW_MRG)
$cmd

chk_status $V1X90ADR
cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $SW_MRG)
$cmd


# Set the reject margin
echo "setting the reject margin..."
chk_status $V1X90ADR
cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $V1X90OP_SET_REJ_MRG)
$cmd

chk_status $V1X90ADR
cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $REJ_MRG)
$cmd


# Enable subtraction of trigger time
echo "enabling subtraction of trigger time..."
chk_status $V1X90ADR
cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $V1X90OP_EN_SUB_TRG)
#cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $V1X90OP_DIS_SUB_TRG)
$cmd


# Set edge detection configuration
echo "setting edge detection configuration..."
chk_status $V1X90ADR
cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $V1X90OP_SET_DETECT)
$cmd

chk_status $V1X90ADR
cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $EDGE_DET)
$cmd

# Disable all channels
#echo "disabling all channels..."
#chk_status $V1X90ADR
#cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $V1X90OP_DIS_ALL_CH)
#$cmd
#
## Enable CH8
#echo "enabling only CH8..."
#chk_status $V1X90ADR
#cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $V1X90OP_EN_CH8)
#$cmd


# Enable all channels
echo "enabling all channels..."
chk_status $V1X90ADR
cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $V1X90OP_EN_ALL_CH)
$cmd


# Set LSB of leading/trading edge
echo "setting LSB of leading/trading edge..."
chk_status $V1X90ADR
cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $V1X90OP_SET_LSB)
$cmd

chk_status $V1X90ADR
cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $LSB)
$cmd


# Enable TDC header and trailer in readout
echo "enabling TDC header and trailer in readout..."
chk_status $V1X90ADR
cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $V1X90OP_EN_HEAD_TRAIL)
$cmd

# Disable TDC header and trailer in readout
#echo "disabling TDC header and trailer in readout..."
#chk_status $V1X90ADR
#cmd=$(printf "cmdvme -ww 0x%x %s" $V1X90_MICRO_REG $V1X90OP_DIS_HEAD_TRAIL)
#$cmd


# Bit set
#cmdvme -ww 0x52001000 0x128
chk_status $V1X90ADR

# clear data
cmd=$(printf "cmdvme -ww 0x%x 0x1" $V1X90_SOFT_CLE)
$cmd

echo "------------------------------"
echo "Done!!!"
echo "------------------------------"
