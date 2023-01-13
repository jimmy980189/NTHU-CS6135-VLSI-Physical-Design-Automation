#!/bin/bash

tar -zcvf CS6135_HW5_111062517.tar.gz HW5/
mv CS6135_HW5_111062517.tar.gz HW5_grading/student/111062517/
cd HW5_grading/
bash HW5_grading.sh
