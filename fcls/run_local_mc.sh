#!/bin/bash

run_mono_S16-10

FCL_DIR=/nova/app/users/mfrank/src/gen/191114_Monopole_MC_FCL
LOG_DIR=/nova/app/users/mfrank/src/gen/191114_Monopole_MC_LOG

N_SECTIONS=48

case $1 in
    a)
	BETA_VALUES="3.16227766e-04 \
                     3.98107171e-04 \
                     5.01187234e-04"
	;;
    b)
        BETA_VALUES="6.30957344e-04 \
                     7.94328235e-04 \
                     1.00000000e-03"
	;;
    c)
	BETA_VALUES="1.25892541e-03 \
                     1.58489319e-03 \
                     1.99526231e-03"
	;;
    d)
	BETA_VALUES="2.51188643e-03 \
                     3.16227766e-03 \
                     3.98107171e-03"
	;;
    e)
        BETA_VALUES="5.01187234e-03 \
                     6.30957344e-03 \
                     7.94328235e-03"
	;;
    *)
	echo "usage:"
	echo "  source run_MC.sh <group>"
	echo
	echo "  available groups: a b c d e"
	exit
	;;
esac
    

for BETA in $BETA_VALUES; do
    echo "Beta = ${BETA}"
    for SECTION in `seq -w 01 ${N_SECTIONS}`; do
	FILE_NAME=Monopole_MC_Beta${BETA}_0.9dEdx_${SECTION}
        FCL_FILE=${FCL_DIR}/${FILE_NAME}.fcl
	LOG_FILE=${LOG_DIR}/${FILE_NAME}.log

	echo "  Processing: ${FILE_NAME}"
	COMMAND="nova -c ${FCL_FILE}"
	# echo $COMMAND
	$COMMAND >> $LOG_FILE 2>&1
    done
done



