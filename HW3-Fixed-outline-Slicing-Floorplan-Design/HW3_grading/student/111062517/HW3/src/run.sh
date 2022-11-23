#!/bin/bash
./hw3 ../testcases/$1.hardblocks ../testcases/$1.nets ../testcases/$1.pl ../output/$1.floorplan 0.15
cp -Rv ../output ../../processing-4.0.1/HW3_printer/
