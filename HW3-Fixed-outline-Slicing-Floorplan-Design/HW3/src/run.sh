#!/bin/bash
#valgrind --leak-check=yes ../bin/hw3 ../testcases/$1.hardblocks ../testcases/$1.nets ../testcases/$1.pl ../output/$1.floorplan $2
#../bin/hw3 ../testcases/$1.hardblocks ../testcases/$1.nets ../testcases/$1.pl ../output/$1.floorplan $2
../verifier/verifier ../testcases/$1.hardblocks ../testcases/$1.nets ../testcases/$1.pl ../output/$1.floorplan $2
#cp -Rv ../output ../../processing-4.0.1/HW3_printer/
