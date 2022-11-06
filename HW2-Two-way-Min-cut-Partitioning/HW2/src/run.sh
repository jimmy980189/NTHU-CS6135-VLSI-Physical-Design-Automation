#!/bin/bash

echo "../bin/hw2 ../testcases/$1.cells ../testcases/$1.nets ../output/$1.out"
/usr/bin/time -p ../bin/hw2 ../testcases/$1.cells ../testcases/$1.nets ../output/$1.out
../verifier/verify  ../testcases/$1.cells ../testcases/$1.nets ../output/$1.out
