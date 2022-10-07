for f in ./*/*/rout*;do echo $f `sed -n 59p $f`;done; for f in ./*/*/tim*; do echo $f `sed -n 20p $f`;done
