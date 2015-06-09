#!/bin/sh
max_rounds=3
max_idle_rounds=1
max_idle_iteration_for_ins=500
max_idle_iteration_for_others=300
in="./test-cases/"$1".vrp"
for k in $(seq 0 $2); do
    ./vrptw -f $1 -i $k -c 2 -w 500 -tr_TokenRing -mr $max_rounds -mir $max_idle_rounds \
        -ts_InsMoveTabuSearch -mii $max_idle_iteration_for_ins -tt 38 42 \
        -ts_InterSwapTabuSearch -mii $max_idle_iteration_for_others -tt 8 12 \
        -ts_IntraSwapTabuSearch -mii $max_idle_iteration_for_others -tt 13 17 # &> /dev/null &
done
#../valid $in "./300/"$1"0.out.0" | grep '^Sum'
