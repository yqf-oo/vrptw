#!/bin/sh
max_rounds=3
max_idle_rounds=1
max_idle_iteration_for_ins=500
max_idle_iteration_for_others=300
prefix='tr_'
file=$1
args=''
for i in $(seq 0 1); do
	args+="-${prefix}${file: -1}$i -mr $max_rounds -mir $max_idle_rounds \
	       -to 500 -ts_InsMoveTabuSearch -mii $max_idle_iteration_for_ins \
		   -tt 38 42 -ts_InterSwapTabuSearch -mii $max_idle_iteration_for_others -tt 8 12\
          -ts_IntraSwapTabuSearch -mii $max_idle_iteration_for_others \
		  -tt 13 17 "
done
echo $args
./vrptw -f $file -pn $2 $args
