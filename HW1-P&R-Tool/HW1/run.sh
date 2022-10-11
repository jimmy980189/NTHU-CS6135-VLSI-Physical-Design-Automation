#!/bin/sh

rm -rf reports/*
sdc="design.sdc"
r="./reports/clk"

# 11 create_clock [get_ports {clk}] -name VCLK -period 10.0 -waveform {0.0 5.0} design.sdc
for i in $(seq 4 -0.1 3);
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
    sed -i '42s/timingDriven 0/timingDriven 1/' apr.tcl
    mv *.rpt "$fdir/low_off"

    # low on
    innovus -no_gui -execute "source apr.tcl"
    mkdir "$fdir/low_on"
    sed -i '42s/low/medium/' apr.tcl
    sed -i '42s/timingDriven 1/timingDriven 0/' apr.tcl
    mv *.rpt "$fdir/low_on"

    # medium off
    innovus -no_gui -execute "source apr.tcl"
    mkdir "$fdir/medium_off"
    sed -i '42s/timingDriven 0/timingDriven 1/' apr.tcl
    mv *.rpt "$fdir/medium_off"
    
    # medium on
    innovus -no_gui -execute "source apr.tcl"
    mkdir "$fdir/medium_on"
    sed -i '42s/medium/high/' apr.tcl
    sed -i '42s/timingDriven 1/timingDriven 0/' apr.tcl
    mv *.rpt "$fdir/medium_on"

    # high off
    innovus -no_gui -execute "source apr.tcl"
    mkdir "$fdir/high_off"
    sed -i '42s/timingDriven 0/timingDriven 1/' apr.tcl
    mv *.rpt "$fdir/high_off"
    
    # high on
    innovus -no_gui -execute "source apr.tcl"
    mkdir "$fdir/high_on"
    sed -i '42s/high/low/' apr.tcl
    sed -i '42s/timingDriven 1/timingDriven 0/' apr.tcl
    mv *.rpt "$fdir/high_on"

done
