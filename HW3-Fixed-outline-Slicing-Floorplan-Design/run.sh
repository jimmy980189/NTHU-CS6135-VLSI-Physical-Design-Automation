#!/bin/bash

tar -zcvf CS6135_HW3_111062517.tar.gz HW3/
mv CS6135_HW3_111062517.tar.gz HW3_grading/student/111062517/
cd HW3_grading/
bash HW3_grading.sh
