#! /usr/bin/env bash

#######################################################
# SAM submit script originally based on               #
#   novasoft/Utilities/batch/SAMSubmitTemplate_ups.sh #
# Later updated using                                 #
#   novasoft/AnalysisSkimmer/scripts/SAM_Skim.sh      #
#######################################################

####################################
# Frequently-used boolean switches #
####################################
MAKE_NEW_TARBALL="yes"
PRESTAGE_DATASET="no"

################################
# Parse command line arguments #
################################
AVAILABLE_TYPES="datacheck \
                 ddslowmono \
                 mc_0.9dEdx \
                 mc_superlowE \
                 mc_lowE \
                 mc_highE \
	         mc_1e-4 \
	         mc_1e-4_highE \
	         mc_3e-4_lowE \
	         mc_3e-4_highE \
	         mc_5e-4 \
	         mc_5e-4_highE \
	         mc_1e-3 \
	         mc_1e-3_highE \
	         mc_5e-3 \
	         mc_5e-3_highE \
	         mc_1e-2 \
	         mc_1e-2_highE"

case $1 in
    datacheck)
	DATATYPE='DataCheck'
	 DATASET='melashri_slowmonopole_epoch2_150_gain'
	#DATASET='melashri_MC_overlay_High_gain'
	;;
    ddslowmono)
	DATATYPE='DDslowmono'
	# DATASET='mfrank_ddslowmono_19728to20752_sX7'
	# DATASET='mfrank_ddslowmono_19728to20752'
	# DATASET='mfrank_ddslowmono_core_dump_180530'
	# DATASET='mfrank_ddslowmono_run_again_180625'
	# DATASET='mfrank_ddslowmono_run_again_180626'
	# DATASET='reprocess_artdaq_S15-03-11_fd_ddslomono'
	# DATASET='mfrank_ddslowmono_run_again_180720'
	DATASET='melashri_slowmonopole_epoch2_150_gain'
	;;
    mc_0.9dEdx)
	DATATYPE='MC_0.9dEdx'
	# DATASET='mfrank_191119_Monopole_MC_Overlay_Beta3.1e-4to7.9e-3_0.9dEdx'
	# DATASET='mfrank_200206_Monopole_MC_Overlay_Beta3.1e-4to7.9e-3_0.9dEdx'
	DATASET='mfrank_200218_Monopole_MC_Overlay_Beta1e-4to2.5e-4_and_Beta1e-2'
	;;
    mc_superlowE)
	DATATYPE='MC_SuperlowE'
	# DATASET='esong_monopole_isogen_extra_low_dedx_gain_100_20190207'
	# DATASET='esong_monopole_isogen_extra_low_dedx_gain_100_20190209'
	DATASET='esong_monopole_isogen_extra_low_dedx_gain_100_20190209_new'
	;;
    mc_lowE)
	DATATYPE='MC_LowE'
	DATASET='mfrank_monopole_mc_beta1e-4to1e-2_lowE_180629'
	;;
    mc_highE)
	DATATYPE='MC_HighE'
	DATASET='mfrank_monopole_mc_beta1e-4to1e-2_highE_180629'
	;;
    mc_1e-4)
	DATATYPE='MC_1e-4'
	DATASET='mfrank_monopole_mc_beta1e-4_170720'
	;;
    mc_1e-4_highE)
	DATATYPE='MC_1e-4_HighE'
	DATASET='mfrank_monopole_mc_highE_beta1e-4_170917'
	;;
    mc_3e-4_lowE)
	DATATYPE='MC_3e-4_LowE'
	DATASET='mfrank_monopole_mc_lowE_beta3e-4_171003'
	;;
    mc_3e-4_highE)
	DATATYPE='MC_3e-4_HighE'
	DATASET='mfrank_monopole_mc_highE_beta3e-4_170917'
	;;
    mc_5e-4)
	DATATYPE='MC_5e-4'
	DATASET='mfrank_monopole_mc_beta5e-4_170720'
	;;
    mc_5e-4_highE)
	DATATYPE='MC_5e-4_HighE'
	DATASET='mfrank_monopole_mc_highE_beta5e-4_170917'
	;;
    mc_1e-3)
	DATATYPE='MC_1e-3'
	DATASET='mfrank_monopole_mc_beta1e-3_170517'
	;;
    mc_1e-3_highE)
	DATATYPE='MC_1e-3_HighE'
	DATASET='mfrank_monopole_mc_highE_beta1e-3_170915'
	;;
    mc_5e-3)
	DATATYPE='MC_5e-3'
	DATASET='mfrank_monopole_mc_beta5e-3_170724'
	;;
    mc_5e-3_highE)
	DATATYPE='MC_5e-3_HighE'
	DATASET='mfrank_monopole_mc_highE_beta5e-3_170917'
	;;
    mc_1e-2)
	DATATYPE='MC_1e-2'
	DATASET='mfrank_monopole_mc_beta1e-2_170724'
	;;
    mc_1e-2_highE)
	DATATYPE='MC_1e-2_HighE'
	DATASET='mfrank_monopole_mc_highE_beta1e-2_170917'
	;;
    *)
	echo "usage:"
	echo "  ./monogrid.sh <type>"
	echo
	echo "  available types:"
	for type in $AVAILABLE_TYPES
	do
	    echo "    " $type
	done
	exit
	;;
esac

#####################################
# Variable configuration parametes  #
#####################################
if [[ $DATATYPE == "DataCheck" ]]
then
    LIFETIME=20h
    N_SECTIONS=-1
    N_FILES_PER_SECTION=50
    N_EVENTS_PER_SECTION=-1
elif [[ $DATATYPE == "DDslowmono" ]]
then
    # short job
    LIFETIME=2h
    N_FILES_PER_SECTION=1
    N_SECTIONS=-1
    N_EVENTS_PER_SECTION=-1
    # long job
    # LIFETIME=7h
    # N_FILES_PER_SECTION=3
    # N_SECTIONS=-1
    # N_EVENTS_PER_SECTION=-1
elif [[ $DATATYPE == "MC"* ]]
then
    # test job
    # LIFETIME=2h
    # N_SECTIONS=1
    # N_FILES_PER_SECTION=1
    # N_EVENTS_PER_SECTION=1
    # full job
    LIFETIME=7h
    N_SECTIONS=-1
    N_FILES_PER_SECTION=1
    N_EVENTS_PER_SECTION=-1
else
    echo "Invalid Data Type:" $DATATYPE
    exit
fi

# FHICL file:
if [[ $DATATYPE == "DataCheck" ]]
then
    JOBFCL=dcmonojob.fcl
    TREE_TYPE=DataCheck_Tree
else
    JOBFCL=monojob.fcl
    TREE_TYPE=Monopole_Tree_${DATATYPE}
fi

# novasoft:
NOVASOFTVER='develop'

# e14 build:
# QUALS='e14:grid:prof'
# LOCAL_PRODUCTS=/nova/ana/users/melashri/libs/mono_180612_S18-05-29/localProducts_nova_develop_e14_prof
# TAR_EXCLUDE_DIR='novasoft/develop/slf6.x86_64.e14.prof'

# e9 build (180531):
# QUALS='e9:s28:grid:prof'
# LOCAL_PRODUCTS=/nova/app/users/melashri/src/mono_180531/localProducts_nova_develop_e9_s28_prof
# TAR_EXCLUDE_DIR='novasoft/develop/slf6.x86_64.e9.s28.prof'
# SETUP_EXTRA_PRODUCTS='setup jobsub_client v1_2_6_2'

# S16-09-13 build (180619):
QUALS='e9:s28:grid:prof'
LOCAL_PRODUCTS=/nova/app/users/melashri/src/mono_180619_S16-09-13/localProducts_nova_develop_e9_s28_prof
TAR_EXCLUDE_DIR='novasoft/develop/slf6.x86_64.e9.s28.prof'
# SETUP_EXTRA_PRODUCTS='setup jobsub_client v1_2_6_2'
SETUP_EXTRA_PRODUCTS='setup jobsub_client v1_3'

# Grid resources:
# default disk seems to be ~34 GB
# DISK=2000MB
# 2 GB is the default
MEMORY=2000MB

echo "Dataset parameters:"
echo "  DATATYPE           =" $DATATYPE
echo "  DATASET            =" $DATASET
# echo "  Disk/Section       =" $DISK
echo "  Memory/Section     =" $MEMORY
echo "  Time/Section       =" $LIFETIME
echo "  Files/Section      =" $N_FILES_PER_SECTION
echo "  Number of Sections =" $N_SECTIONS
echo "  Number of Events   =" $N_EVENTS_PER_SECTION
echo "  Job FHICL File     =" $JOBFCL
echo "  Local Products     =" $LOCAL_PRODUCTS
echo "  novasoft version   =" $NOVASOFTVER
echo "  novasoft quals     =" $QUALS
echo
echo

###################################
# Static configuration parametes  #
###################################
GROUP=nova
USER=melashri
ROLE="Analysis"
export GRID_USER=${USER}

OUTPUT_PATH=/pnfs/nova/scratch/users/melashri/plain
GRID_SCRIPT_PATH=/pnfs/nova/scratch/users/${USER}/grid_scripts

export DATE=$(date +%y%m%d_%H%M)
JOBNAME=SlowMonopole_${DATATYPE}
TFILEOUT_NAME=${DATE}_${TREE_TYPE}

TAR_LABEL=mono_180619_S16-09-13_reco
TAR_LOCAL_PRODUCTS=$(basename ${LOCAL_PRODUCTS})_${TAR_LABEL}.tar.bz2

##############################################
# Setup local environment for job submission #
##############################################
source /cvmfs/fermilab.opensciencegrid.org/products/common/etc/setups.sh
export PRODUCTS=${PRODUCTS}:/cvmfs/nova.opensciencegrid.org/externals/:/cvmfs/nova.opensciencegrid.org/pidlibs/products
export PRODUCTS=${LOCAL_PRODUCTS}:${PRODUCTS}
setup novasoft $NOVASOFTVER -q $QUALS
$SETUP_EXTRA_PRODUCTS

###########################################################
# Make a tar file of the local products area if requested #
###########################################################
echo
echo "Tarball    =" $TAR_LOCAL_PRODUCTS
echo "Exclude    =" $TAR_EXCLUDE_DIR
echo "New Tarball?" $MAKE_NEW_TARBALL

if [[ ${MAKE_NEW_TARBALL} == "yes" ]]; then

  echo "  generating new tarball ... please wait"

  CURDIR=${PWD}
  cd ${LOCAL_PRODUCTS}/..
  localProdDir=`basename ${LOCAL_PRODUCTS}`
  if [ -e ${TAR_LOCAL_PRODUCTS} ]; then
     rm -f ${TAR_LOCAL_PRODUCTS}
  fi

  tar --exclude=${TAR_EXCLUDE_DIR} -cjf ${TAR_LOCAL_PRODUCTS} ${localProdDir}
  ls -lh *.bz2

  # now check that the tar file you want exists and can be copied
  if [ -e ${TAR_LOCAL_PRODUCTS} ]; then

    # make sure you have a place to put it on pnfs
    if [ ! -e '/pnfs/nova/scratch/users/'${USER} ]; then
      mkdir '/pnfs/nova/scratch/users/'${USER}
    fi

    # remove the current copy if there is one
    if [ -e '/pnfs/nova/scratch/users/'${USER}'/'${TAR_LOCAL_PRODUCTS}  ]; then
      rm -f '/pnfs/nova/scratch/users/'${USER}'/'${TAR_LOCAL_PRODUCTS}
    fi

    # copy the newly made file into the pnfs area
    ifdh cp ${TAR_LOCAL_PRODUCTS} '/pnfs/nova/scratch/users/'${USER}'/'${TAR_LOCAL_PRODUCTS}
    # rm ${TAR_LOCAL_PRODUCTS}
  fi

  # go back to where we started
  cd ${CURDIR}
fi

# check that the tarfile is in the correct location
echo
echo "checking that tarball made it:"
echo ls -lh '/pnfs/nova/scratch/users/'${USER}'/'${TAR_LOCAL_PRODUCTS}
ls -lh '/pnfs/nova/scratch/users/'${USER}'/'${TAR_LOCAL_PRODUCTS}

# if [ ! -e '/pnfs/nova/scratch/users/'${USER}'/'${TAR_LOCAL_PRODUCTS}  ]; then
#    echo 'tar file not found in pnfs area, bail'
#    exit
# fi

############################################
# Setup the SAM project here               #
############################################
if [[ "x$PROJECTNAME" == "x" ]]; then
    # DATE="`date +%Y%m%d_%H%M`"
    PROJECTNAME="${USER}-${JOBNAME}-${DATE}"
    ifdh startProject "${PROJECTNAME}" "${SAM_STATION}" "${DATASET}" "${USER}" "${SAM_STATION}"
    if [ $? -eq 0 ]; then
	export SAM_PROJECT_NAME="${PROJECTNAME}"
    fi
else
  export SAM_PROJECT_NAME="${PROJECTNAME}"
fi

export NUMPROJECTFILES=`samweb -e nova count-definition-files ${DATASET}`
echo
echo "The dataset (${DATASET}) contains ${NUMPROJECTFILES} files."

if [ ${N_SECTIONS} -le 0 ]; then
  if [ ${N_FILES_PER_SECTION} -eq 1 ]; then
      export N_SECTIONS=${NUMPROJECTFILES}
  else
      export N_SECTIONS=`echo "(${NUMPROJECTFILES}/ ${N_FILES_PER_SECTION}) + 1" | bc`
  fi
fi

############################################
# Give log output names a sensible name    #
# based on the PROJECTNAME                 #
############################################
# SCRIPT is set to the $IFDH_ART_FQ_DIR/bin version of art_sam_wrap.sh by
# my novasoft product that I set up before invoking this script
# use global script for proper running:
JOBSCRIPT=/nova/app/condor-exec/${USER}/${PROJECTNAME}.sh
# ln -s ${SCRIPT} ${JOBSCRIPT}
# Hack from Brian's AnalysisSkimmer/scripts/SAM_Skim.sh to fix the typos in
# art_sam_wrap.sh:
cat ${SCRIPT} > ${JOBSCRIPT} 
sed -i "s/cvmvs/cvmfs/g" ${JOBSCRIPT} 
echo
echo JOBSCRIPT is ${JOBSCRIPT}

############################################
# Print out some useful information        #
############################################
echo
echo "N_SECTIONS is ${N_SECTIONS} with ${N_FILES_PER_SECTION} per job"
echo "DATASET: " ${DATASET}
echo "Project name is: ${SAM_PROJECT_NAME}"
echo "Station Monitor: http://samweb.fnal.gov:8480/station_monitor/nova/stations/nova/projects/${SAM_PROJECT_NAME}"
echo "${SAM_PROJECT_NAME}" >> $0.projectname.log

############################################
# Make copyOut.sh to copy completed files  #
############################################
COPYOUT=copyOut_${PROJECTNAME}.sh
echo '#! /usr/bin/env bash'                                                           >  ${COPYOUT}
echo ' '                                                                              >> ${COPYOUT}
echo 'main(){'                                                                        >> ${COPYOUT}
echo '    echo pwd'                                                                   >> ${COPYOUT}
echo '    pwd'                                                                        >> ${COPYOUT}
echo '    echo ls -lth'                                                               >> ${COPYOUT}
echo '    ls -lth'                                                                    >> ${COPYOUT}
echo ' '                                                                              >> ${COPYOUT}
echo '    if ls core.* 1> /dev/null 2>&1; then'                                       >> ${COPYOUT}
echo '      echo "export CORE_NAME=`ls *core* | head -1`"'                            >> ${COPYOUT}
echo '      export CORE_NAME=`ls *core* | head -1`'                                   >> ${COPYOUT}
echo '      echo "echo $CORE_NAME"'                                                   >> ${COPYOUT}
echo '      echo $CORE_NAME'                                                          >> ${COPYOUT}
echo '      echo "ifdh cp $CORE_NAME ${DEST}/$CORE_NAME"'                             >> ${COPYOUT}
echo '      echo ifdh cp $CORE_NAME ${DEST}/$CORE_NAME'                               >> ${COPYOUT}
echo '      ifdh cp $CORE_NAME ${DEST}/$CORE_NAME'                                    >> ${COPYOUT}
echo '    else'                                                                       >> ${COPYOUT}
echo '      echo "No core file found."'                                               >> ${COPYOUT}
echo '    fi'                                                                         >> ${COPYOUT}
echo ' '                                                                              >> ${COPYOUT}
echo '    printf -v SECTION "%03d" ${PROCESS}'                                        >> ${COPYOUT}
echo '    ifdh cp '${TFILEOUT_NAME}'.root ${DEST}/'${TFILEOUT_NAME}'_${SECTION}.root' >> ${COPYOUT}
echo '    rm '${TFILEOUT_NAME}'.root'                                                 >> ${COPYOUT}
echo '}'                                                                              >> ${COPYOUT}
echo 'main $*'                                                                        >> ${COPYOUT}

if [ -e ${GRID_SCRIPT_PATH}/${COPYOUT} ]; then
   rm -f ${GRID_SCRIPT_PATH}/${COPYOUT}
fi
cat ${COPYOUT} > ${GRID_SCRIPT_PATH}/${COPYOUT}
rm ${COPYOUT}
chmod +x ${GRID_SCRIPT_PATH}/${COPYOUT}

echo
echo "COPYOUT: " ${GRID_SCRIPT_PATH}/${COPYOUT}

############################################
# Make source file to setup software       #
############################################
SETUP_NOVA=setup_nova_ups_${PROJECTNAME}.sh
echo '#! /usr/bin/env bash'                                              >  ${SETUP_NOVA}
echo ' '                                                                 >> ${SETUP_NOVA}
echo 'main(){'                                                           >> ${SETUP_NOVA}
echo '    echo ulimit -a'                                                >> ${SETUP_NOVA}
echo '    ulimit -a'                                                     >> ${SETUP_NOVA}
# echo '    echo ulimit -s 10240'                                          >> ${SETUP_NOVA}
# echo '    ulimit -s 10240'                                               >> ${SETUP_NOVA}
# echo '    echo ulimit -a'                                                >> ${SETUP_NOVA}
# echo '    ulimit -a'                                                     >> ${SETUP_NOVA}
echo '    export UPS_OVERRIDE="-H Linux64bit+2.6-2.12"'                  >> ${SETUP_NOVA}
echo '    source /cvmfs/nova.opensciencegrid.org/externals/setups'       >> ${SETUP_NOVA}
echo '    echo ls ${INPUT_TAR_FILE}'                                     >> ${SETUP_NOVA}
echo '    ls ${INPUT_TAR_FILE}'                                          >> ${SETUP_NOVA}
echo '    LOCAL_PRODUCTS=`basename '${LOCAL_PRODUCTS}'`'                 >> ${SETUP_NOVA}
echo '    echo ls -lh ${PWD}/${LOCAL_PRODUCTS}'                          >> ${SETUP_NOVA}
echo '    ls -lh ${PWD}/${LOCAL_PRODUCTS}'                               >> ${SETUP_NOVA}
echo '    export PRODUCTS=${PWD}/${LOCAL_PRODUCTS}/:${PRODUCTS}'         >> ${SETUP_NOVA}
echo '    echo PRODUCTS is set to ${PRODUCTS}'                           >> ${SETUP_NOVA}
echo '    echo ups flavor'                                               >> ${SETUP_NOVA}
echo '    ups flavor'                                                    >> ${SETUP_NOVA}
echo '    echo ups list -aK+ novasoft'                                   >> ${SETUP_NOVA}
echo '    ups list -aK+ novasoft'                                        >> ${SETUP_NOVA}
echo '    echo ups depend novasoft '${NOVASOFTVER}' -q '${QUALS}         >> ${SETUP_NOVA}
echo '    ups depend novasoft '${NOVASOFTVER}' -q '${QUALS}              >> ${SETUP_NOVA}
echo '    echo setup novasoft '${NOVASOFTVER}' -q '${QUALS}              >> ${SETUP_NOVA}
echo '    setup novasoft '${NOVASOFTVER}' -q '${QUALS}                   >> ${SETUP_NOVA}
echo ' '                                                                 >> ${SETUP_NOVA}
# echo '    # The fcl file is passed into the jobsub_submit command so add its'                 >> ${SETUP_NOVA}
# echo '    # location to the FHICL_FILE_PATH variable'                                         >> ${SETUP_NOVA}
# echo '    infcl=${NOVASOFT_DIR}/job/'${JOBFCL}                                                >> ${SETUP_NOVA}
# echo '    export FHICL_FILE_PATH=${CONDOR_DIR_INPUT}:${FHICL_FILE_PATH}'                      >> ${SETUP_NOVA}
# echo '    cat ${infcl} > '${FCLFILE}                                                          >> ${SETUP_NOVA}
# echo ' '                                                                                      >> ${SETUP_NOVA}
# echo '    echo The fcl configuration is '${JOBFCL}':'                    >> ${SETUP_NOVA}
# echo '    ART_DEBUG_CONFIG=1 art -c '${JOBFCL}                           >> ${SETUP_NOVA}
# echo ' '                                                                 >> ${SETUP_NOVA}
echo '    # set the copyOut script to be executable'                     >> ${SETUP_NOVA}
echo '    chmod gu+x ${CONDOR_DIR_INPUT}/'${COPYOUT}                     >> ${SETUP_NOVA}
echo '}'                                                                 >> ${SETUP_NOVA}
echo 'main $*'                                                           >> ${SETUP_NOVA}

if [ -e ${GRID_SCRIPT_PATH}/${SETUP_NOVA} ]; then
   rm -f ${GRID_SCRIPT_PATH}/${SETUP_NOVA}
fi
cat ${SETUP_NOVA} > ${GRID_SCRIPT_PATH}/${SETUP_NOVA}
rm ${SETUP_NOVA}
chmod +x ${GRID_SCRIPT_PATH}/${SETUP_NOVA}

echo
echo "SETUP_NOVA: " ${GRID_SCRIPT_PATH}/${SETUP_NOVA}

############################################
# Prestage the data set                    #
############################################
if [[ ${PRESTAGE_DATASET} == "yes" ]]; then
    echo
    echo "Prestaging dataset ${DATASET}"
    
    samweb prestage-dataset --defname=${DATASET} --parallel 4
fi

############################################
# The Magic Jobsub command                 #
############################################
SUBMIT_COMMAND="jobsub_submit \
    -N ${N_SECTIONS} \
    --OS=SL6 \
    -G nova \
    -e SAM_PROJECT_NAME \
    -e SAM_STATION \
    -e IFDH_BASE_URI \
    -e IFDH_DEBUG \
    -e EXPERIMENT \
    -e GRID_USER \
    --tar_file_name=dropbox:///pnfs/nova/scratch/users/${USER}/${TAR_LOCAL_PRODUCTS} \
    -f ${GRID_SCRIPT_PATH}/${SETUP_NOVA} \
    -f ${GRID_SCRIPT_PATH}/${COPYOUT} \
    --role=${ROLE} \
    --expected-lifetime=${LIFETIME} \
    --memory=${MEMORY} \
    file://${JOBSCRIPT} \
      --config ${JOBFCL} \
      --source '\$CONDOR_DIR_INPUT/'${SETUP_NOVA} \
      --limit ${N_FILES_PER_SECTION} \
      --export DEST=${OUTPUT_PATH} \
      --postscript '\$CONDOR_DIR_INPUT/'${COPYOUT} \
      -X nova -T ${TFILEOUT_NAME}.root -n ${N_EVENTS_PER_SECTION}"

echo
echo $SUBMIT_COMMAND

# echo NO SUBMISSION, JUST TESTING
$SUBMIT_COMMAND

#     --disk=${DISK} \
