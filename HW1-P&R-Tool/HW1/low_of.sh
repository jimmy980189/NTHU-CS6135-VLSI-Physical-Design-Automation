#!/bin/sh

#rm -rf reports/*
sdc="design.sdc"
r="./reports/clk"

# 11 create_clock [get_ports {clk}] -name VCLK -period 10.0 -waveform {0.0 5.0} design.sdc
for i in $(seq 3.5 -0.1 3.4);
do 
    j=`echo "scale=2; $i / 2" | bc`

    clock="create_clock [get_ports {clk}] -name VCLK -period $i -waveform {0.0 $j}"
    sed -i "11d" $sdc && sed -i "10a $clock" $sdc

    fdir=$r$i
    echo $fdir
    mkdir $fdir
    
    # low off
    innovus -no_gui -execute "source apr.tcl"
    mkdir "$fdir/low_off"
    mv *.rpt "$fdir/low_off"

done
