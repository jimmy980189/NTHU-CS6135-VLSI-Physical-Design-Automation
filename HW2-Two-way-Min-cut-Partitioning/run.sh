#!/bin/bash

tar -zcvf CS6135_HW2_111062517.tar.gz HW2/
mv CS6135_HW2_111062517.tar.gz HW2_grading/student/111062517/
cd HW2_grading/
bash HW2_grading.sh
