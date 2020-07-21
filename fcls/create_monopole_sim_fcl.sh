#!/bin/bash

#
# This script will generate a set of FHICL files that can be used
# for MC mass generation.
#

echo "Deleting contents of tmp_fcls ..."
rm tmp_fcls/*

SEED=0
N_EVENTS=420
OUTPUT_DIR=""
# OUTPUT_DIR="/nova/ana/users/mfrank/mono/gen/"

# N_SECTIONS=2
# BETA_VALUES="1.00000000e-03"

N_SECTIONS=48
BETA_VALUES="1.00000000e-04 \
             1.25892541e-04 \
             1.58489319e-04 \
             1.99526231e-04 \
             2.51188643e-04 \
             1.00000000e-02"

# BETA_VALUES="3.16227766e-04 \
#              3.98107171e-04 \
#              5.01187234e-04 \
#              6.30957344e-04 \
#              7.94328235e-04 \
#              1.00000000e-03 \
#              1.25892541e-03 \
#              1.58489319e-03 \
#              1.99526231e-03 \
#              2.51188643e-03 \
#              3.16227766e-03 \
#              3.98107171e-03 \
#              5.01187234e-03 \
#              6.30957344e-03 \
#              7.94328235e-03"

for BETA in $BETA_VALUES; do
    echo ${BETA}
    for SECTION in `seq -w 01 ${N_SECTIONS}`; do
        FCL_FILE=Monopole_MC_Beta${BETA}_0.9dEdx_${SECTION}.fcl
	POSTFIX="Beta${BETA}_0.9dEdx_${SECTION}.root"
	OUTPUT_FILE=${OUTPUT_DIR}"Monopole_MC_${POSTFIX}"
        SEED=`echo "$SEED+1" | bc -l`

	# calculate relativistic momentum from beta:
        value=`echo ${BETA} | sed -e 's/[eE]+*/\\*10\\^/'`
        P0=`echo "1*10^16*$value/sqrt(1-$value*$value)" | bc -l`

	echo '#include "monopole_isogen_job.fcl"'                    > $FCL_FILE
        echo "physics.producers.generator.Seed: ${SEED}"            >> $FCL_FILE
        echo "physics.producers.generator.P0:   [${P0}]"            >> $FCL_FILE
        echo "outputs.writer.fileName:          \"${OUTPUT_FILE}\"" >> $FCL_FILE
	echo "source.maxEvents:                 ${N_EVENTS}"        >> $FCL_FILE

        mv $FCL_FILE tmp_fcls/
    done
done


#
# Beta Ranges:
#
#for beta in `echo  1.00000000e-04 1.25892541e-04 1.58489319e-04 1.99526231e-04 2.51188643e-04 3.16227766e-04 3.98107171e-04 5.01187234e-04 6.30957344e-04 7.94328235e-04 1.00000000e-03 1.25892541e-03 1.58489319e-03 1.99526231e-03 2.51188643e-03 3.16227766e-03  3.98107171e-03 5.01187234e-03 6.30957344e-03 7.94328235e-03 1.00000000e-02`; do

#for beta in `echo  1.00000000e-04 1.25892541e-04 1.58489319e-04 1.99526231e-04 2.51188643e-04 3.16227766e-04 3.98107171e-04 5.01187234e-04 6.30957344e-04 7.94328235e-04 1.00000000e-03 1.25892541e-03 1.58489319e-03 1.99526231e-03 2.51188643e-03 3.16227766e-03  3.98107171e-03 5.01187234e-03 6.30957344e-03 7.94328235e-03 1.00000000e-02 1.25892541e-02 1.58489319e-02 1.99526231e-02 2.51188643e-02 3.16227766e-02 3.98107171e-02 5.01187234e-02 6.30957344e-02 7.94328235e-02 1.00000000e-01 1.25892541e-01 1.58489319e-01 1.99526231e-01 2.51188643e-01 3.16227766e-01  3.98107171e-01 5.01187234e-01 6.30957344e-01 7.94328235e-01`; do
