#!/bin/bash
#valgrind --leak-check=yes ../bin/hw4 ../testcase/$1/$1.aux ../output/$1.result
../bin/hw4 ../testcase/$1/$1.aux ../output/$1.result

echo ""
echo "[START VERIFY]"
../verifier/verify ../testcase/$1/$1.aux ../output/$1.result
