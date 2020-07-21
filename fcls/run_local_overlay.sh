#!/bin/bash

run_mono_S16-10

# it takes 10 hours per beta point (i.e. 48 files)

# 191115 contains all of the FHICLs for the 1.2589254e-03 sample
# 191116 contains all of the FHICLs for the currently generated MC files
#        minus the 19115 FHICLs
#        (the only MC sample not finished yet is 7.94e-03)
# 191118 contains the 7.94e-03 FHICLs

FCL_DIR=/nova/app/users/mfrank/src/gen/191118_Monopole_Overlay_FCL
LOG_DIR=/nova/app/users/mfrank/src/gen/191118_Monopole_Overlay_LOG

case $1 in
    a)
	START_SECTION=0
	END_SECTION=47
	;;
    b)
	START_SECTION=151
	END_SECTION=300
	;;
    c)
	START_SECTION=301
	END_SECTION=450
	;;
    d)
	START_SECTION=451
	END_SECTION=600
	;;
    e)
	START_SECTION=601
	END_SECTION=679
	;;
    *)
	echo "usage:"
	echo "  source run_Overlay.sh <group>"
	echo
	echo "  available groups: a b c d e"
	return
	;;
esac
    

for SECTION in `seq ${START_SECTION} ${END_SECTION}`; do
	FILE_NAME=Monopole_MC_Overlay_${SECTION}
        FCL_FILE=${FCL_DIR}/${FILE_NAME}.fcl
	LOG_FILE=${LOG_DIR}/${FILE_NAME}.log

	if [ -f $FCL_FILE ]
	then
	    echo "  Processing: ${FILE_NAME}"
	    COMMAND="nova -c ${FCL_FILE}"
	    # echo $COMMAND
	    $COMMAND >> $LOG_FILE 2>&1
	else
	    echo "  ${FCL_FILE} does not exist, skipping to next."
	fi
done



