#!/bin/bash
#valgrind --leak-check=yes ../bin/hw4 
../bin/hw4 ../testcase/$1.aux ../output/$1.result
#../verifier/verifier 
