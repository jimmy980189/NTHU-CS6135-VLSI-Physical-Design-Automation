#!/bin/bash

tar -zcvf CS6135_HW4_111062517.tar.gz HW4/
mv CS6135_HW4_111062517.tar.gz HW4_grading/student/111062517/
cd HW4_grading/
bash HW4_grading.sh
