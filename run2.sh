#!/bin/sh
max_rounds=3
max_idle_rounds=1
max_idle_iteration_for_ins=500
max_idle_iteration_for_others=300
in='./test-cases/'$1'.vrp'
out='./test-cases/'$1'.out'
args="-tr_TokenRing -mr $max_rounds -mir $max_idle_rounds -mit 10 \
	   -to 500 -ts_InsMoveTabuSearch -mii $max_idle_iteration_for_ins \
	   -tt 38 42 -ts_InterSwapTabuSearch -mii $max_idle_iteration_for_others \
	   -tt 8 12 \
       -ts_IntraSwapTabuSearch -mii $max_idle_iteration_for_others \
	   -tt 13 17 "
./vrptw -f $1 $2 $args
sleep 1.0
../valid $in $out | grep '^Sum'

