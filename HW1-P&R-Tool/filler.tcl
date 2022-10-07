getCTSMode -engine -quiet
getFillerMode -quiet
addFiller -cell {FILLCELL_X1 FILLCELL_X2 FILLCELL_X4
FILLCELL_X8 FILLCELL_X16 FILLCELL_X32} -prefix FILL
saveDesign final
report_timing > timing.rpt
report_area > area.rpt
report_route -summary > route.rpt
verify_drc > drc.rpt
exit
