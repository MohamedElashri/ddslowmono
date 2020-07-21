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
MAKE_NEW_TARBALL="no"
PRESTAGE_DATASET="yes"

################################
# Parse command line arguments #
################################
AVAILABLE_TYPES="gen \
                 overlay"

case $1 in
    gen)
	# FHICL_DATASET='mfrank_fcl_test_2'
	FHICL_DATASET='mfrank_200211_FCL_MC_Beta1e-4to2.5e-4_and_Beta1e-2'
	# FHICL_DATASET='userdataset_mfrank_2020-02-04-11_58_02'
	# FHICL_DATASET='mfrank_200204_FCL_MC_Beta3.1e-4to7.9e-3'
	JOBNAME=Monopole_MC_Gen
	;;
    overlay)
	FHICL_DATASET='mfrank_200218_FCL_MC_Overlay_Beta1e-4to2.5e-4_and_Beta1e-2'
	JOBNAME=Monopole_MC_Overlay
	;;
    *)
	echo "usage:"
	echo "  ./mcgengrid.sh <type>"
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
LIFETIME=4h
N_FILES_PER_SECTION=1
N_SECTIONS=-1

# novasoft:
NOVASOFTVER='develop'

# S16-10-07 gen build (190610):
QUALS='e9:s28:grid:prof'
LOCAL_PRODUCTS=/nova/ana/users/mfrank/libs/mono_190610_S16-10-07/localProducts_nova_develop_e9_s28_prof
TAR_EXCLUDE_DIR='novasoft/develop/slf6.x86_64.e9.s28.prof'
SETUP_EXTRA_PRODUCTS='setup jobsub_client v1_3'

echo "Dataset parameters:"
echo "  FHICL_DATASET      =" $FHICL_DATASET
echo "  Time/Section       =" $LIFETIME
echo "  Files/Section      =" $N_FILES_PER_SECTION
echo "  Number of Sections =" $N_SECTIONS
echo "  Local Products     =" $LOCAL_PRODUCTS
echo "  novasoft version   =" $NOVASOFTVER
echo "  novasoft quals     =" $QUALS
echo
echo

###################################
# Static configuration parametes  #
###################################
GROUP=nova
USER=mfrank
ROLE="Analysis"
export GRID_USER=${USER}

OUTPUT_PATH=/pnfs/nova/scratch/users/${USER}/samout/
GRID_SCRIPT_PATH=/pnfs/nova/scratch/users/${USER}/grid_scripts

export DATE=$(date +%y%m%d_%H%M)

TAR_LABEL=mono_190610_S16-10-07_gen
TAR_LOCAL_PRODUCTS=$(basename ${LOCAL_PRODUCTS})_${TAR_LABEL}.tar.bz2

##############################################
# Setup local environment for job submission #
##############################################
source /cvmfs/fermilab.opensciencegrid.org/products/common/etc/setups.sh
export PRODUCTS=${PRODUCTS}:/cvmfs/nova.opensciencegrid.org/externals/:/cvmfs/nova.opensciencegrid.org/pidlibs/products
#export PRODUCTS=${LOCAL_PRODUCTS}:${PRODUCTS}
# Let's try using the S16-09-13 build to submit the job:
export PRODUCTS=/nova/ana/users/mfrank/libs/mono_180619_S16-09-13/localProducts_nova_develop_e9_s28_prof:${PRODUCTS}
setup novasoft $NOVASOFTVER -q $QUALS
$SETUP_EXTRA_PRODUCTS

echo
echo "Submission Environment Set Up"
echo "PRODUCTS = ${PRODUCTS}"
echo "novasoft product for submission:"
ups active | grep novasoft
echo

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

  tar --dereference --exclude=${TAR_EXCLUDE_DIR} -cjf ${TAR_LOCAL_PRODUCTS} ${localProdDir}
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
    # ifdh cp ${TAR_LOCAL_PRODUCTS} '/pnfs/nova/scratch/users/'${USER}'/'${TAR_LOCAL_PRODUCTS}
    rsync -avP ${TAR_LOCAL_PRODUCTS} '/pnfs/nova/scratch/users/'${USER}'/'${TAR_LOCAL_PRODUCTS}
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

############################################
# Setup the SAM project here               #
############################################
if [[ "x$PROJECTNAME" == "x" ]]; then
    PROJECTNAME="${USER}-${JOBNAME}-${DATE}"
    ifdh startProject "${PROJECTNAME}" "${SAM_STATION}" "${FHICL_DATASET}" "${USER}" "${SAM_STATION}"
    if [ $? -eq 0 ]; then
	export SAM_PROJECT_NAME="${PROJECTNAME}"
    fi
else
  export SAM_PROJECT_NAME="${PROJECTNAME}"
fi

export NUMPROJECTFILES=`samweb -e nova count-definition-files ${FHICL_DATASET}`
echo
echo "The dataset (${FHICL_DATASET}) contains ${NUMPROJECTFILES} files."

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
echo "FHICL_DATASET: " ${FHICL_DATASET}
echo "Project name is: ${SAM_PROJECT_NAME}"
echo "Station Monitor: http://samweb.fnal.gov:8480/station_monitor/nova/stations/nova/projects/${SAM_PROJECT_NAME}"
echo "${SAM_PROJECT_NAME}" >> $0.projectname.log

############################################
# Make copyOut.sh to copy completed files  #
############################################
COPYOUT=copyOut_${PROJECTNAME}.sh
echo '#! /usr/bin/env bash'                                  >  ${COPYOUT}
echo ' '                                                     >> ${COPYOUT}
echo 'main(){'                                               >> ${COPYOUT}
echo '    echo pwd'                                          >> ${COPYOUT}
echo '    pwd'                                               >> ${COPYOUT}
echo '    echo ls -lth'                                      >> ${COPYOUT}
echo '    ls -lth'                                           >> ${COPYOUT}
echo ' '                                                     >> ${COPYOUT}
echo '    if ls core.* 1> /dev/null 2>&1; then'              >> ${COPYOUT}
echo '      echo "export CORE_NAME=`ls *core* | head -1`"'   >> ${COPYOUT}
echo '      export CORE_NAME=`ls *core* | head -1`'          >> ${COPYOUT}
echo '      echo "echo $CORE_NAME"'                          >> ${COPYOUT}
echo '      echo $CORE_NAME'                                 >> ${COPYOUT}
echo '      echo "ifdh cp $CORE_NAME ${DEST}/$CORE_NAME"'    >> ${COPYOUT}
echo '      echo ifdh cp $CORE_NAME ${DEST}/$CORE_NAME'      >> ${COPYOUT}
echo '      ifdh cp $CORE_NAME ${DEST}/$CORE_NAME'           >> ${COPYOUT}
echo '    else'                                              >> ${COPYOUT}
echo '      echo "No core file found."'                      >> ${COPYOUT}
echo '    fi'                                                >> ${COPYOUT}
echo ' '                                                     >> ${COPYOUT}
echo '    for outfile in `ls -1 Monopole_MC*.root`; do'      >> ${COPYOUT}
echo '        echo ifdh cp $outfile ${DEST}/$outfile;'       >> ${COPYOUT}
echo '        ifdh cp $outfile ${DEST}/$outfile;'            >> ${COPYOUT}
echo '    done'                                              >> ${COPYOUT}
echo '}'                                                     >> ${COPYOUT}
echo 'main $*'                                               >> ${COPYOUT}

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
echo '#! /usr/bin/env bash'                                         >  ${SETUP_NOVA}
echo ' '                                                            >> ${SETUP_NOVA}
echo 'main(){'                                                      >> ${SETUP_NOVA}
echo '    echo ls -l /pnfs/nova/scratch/users/mfrank/'                                           >> ${SETUP_NOVA}
echo '    ls -l /pnfs/nova/scratch/users/mfrank/'                                           >> ${SETUP_NOVA}
echo '    echo ls -l /pnfs/nova/persistent/users/mfrank/'                                           >> ${SETUP_NOVA}
echo '    ls -l /pnfs/nova/persistent/users/mfrank/'                                           >> ${SETUP_NOVA}
echo '    echo ulimit -a'                                           >> ${SETUP_NOVA}
echo '    ulimit -a'                                                >> ${SETUP_NOVA}
echo '    export UPS_OVERRIDE="-H Linux64bit+2.6-2.12"'             >> ${SETUP_NOVA}
echo '    source /cvmfs/nova.opensciencegrid.org/externals/setups'  >> ${SETUP_NOVA}
echo '    echo ls ${INPUT_TAR_FILE}'                                >> ${SETUP_NOVA}
echo '    ls ${INPUT_TAR_FILE}'                                     >> ${SETUP_NOVA}
echo '    LOCAL_PRODUCTS=`basename '${LOCAL_PRODUCTS}'`'            >> ${SETUP_NOVA}
echo '    echo ls -lh ${PWD}/${LOCAL_PRODUCTS}'                     >> ${SETUP_NOVA}
echo '    ls -lh ${PWD}/${LOCAL_PRODUCTS}'                          >> ${SETUP_NOVA}
echo '    export PRODUCTS=${PWD}/${LOCAL_PRODUCTS}/:${PRODUCTS}'    >> ${SETUP_NOVA}
echo '    echo PRODUCTS is set to ${PRODUCTS}'                      >> ${SETUP_NOVA}
echo '    echo ups flavor'                                          >> ${SETUP_NOVA}
echo '    ups flavor'                                               >> ${SETUP_NOVA}
echo '    echo ups list -aK+ novasoft'                              >> ${SETUP_NOVA}
echo '    ups list -aK+ novasoft'                                   >> ${SETUP_NOVA}
echo '    echo ups depend novasoft '${NOVASOFTVER}' -q '${QUALS}    >> ${SETUP_NOVA}
echo '    ups depend novasoft '${NOVASOFTVER}' -q '${QUALS}         >> ${SETUP_NOVA}
echo '    echo setup novasoft '${NOVASOFTVER}' -q '${QUALS}         >> ${SETUP_NOVA}
echo '    setup novasoft '${NOVASOFTVER}' -q '${QUALS}              >> ${SETUP_NOVA}
echo ' '                                                            >> ${SETUP_NOVA}
echo '    # set the copyOut script to be executable'                >> ${SETUP_NOVA}
echo '    chmod gu+x ${CONDOR_DIR_INPUT}/'${COPYOUT}                >> ${SETUP_NOVA}
echo '}'                                                            >> ${SETUP_NOVA}
echo 'main $*'                                                      >> ${SETUP_NOVA}

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
    
    samweb prestage-dataset --defname=${FHICL_DATASET} --parallel 4
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
    -l \"+JobType=\"MC\"\"
    --role=${ROLE} \
    --expected-lifetime=${LIFETIME} \
    file://${JOBSCRIPT} \
      --getconfig \
      --source '\$CONDOR_DIR_INPUT/'${SETUP_NOVA} \
      --limit ${N_FILES_PER_SECTION} \
      --export DEST=${OUTPUT_PATH} \
      --postscript '\$CONDOR_DIR_INPUT/'${COPYOUT} \
      -X nova \
      --sam-file-type=importedSimulated \
      --sam-application-family=nova \
      --sam-data-tier=* \
      --sam-application-version=S16-10-07"

echo
echo $SUBMIT_COMMAND

#echo NO SUBMISSION, JUST TESTING
$SUBMIT_COMMAND
