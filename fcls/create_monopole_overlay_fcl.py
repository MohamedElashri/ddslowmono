from itertools import cycle as round_robin
from random import randint
import glob
from tqdm import tqdm 
# you can install tqdm by:python -m pip install --user tqdm
import sys
import subprocess

snews_files = glob.glob('/pnfs/nova/persistent/users/esong/snews/5ms/gain_100/*/*/*root')

monopole_files = glob.glob("/pnfs/nova/scratch/users/mfrank/samout/Monopole_MC_Beta*root")

print(snews_files[:1])
print(monopole_files[:1])
rr=round_robin(snews_files)
for idx, monopole_file in enumerate(tqdm(monopole_files)):
  job_fcl_path = "tmp_fcls/Monopole_MC_Overlay_{}.fcl".format(idx)
  with open(job_fcl_path,"w") as job_f:
    # 
    # I. Output File
    # - standard name (e.g. Monopole_MC_Beta5.01187234e-04_0.9dEdx_01.root)
    output_file = "Monopole_MC_Overlay_" + monopole_file.split("/")[-1][12:]
    # - with SAM hex code (e.g. aa600c32-e3a5-49a5-9eb2-231e074ad022-Monopole_MC_Beta1.00000000e-03_0.9dEdx_19.root)
    # output_file = "Monopole_MC_Overlay_" + monopole_file.split("/")[-1][49:]
    #
    # II. SNEWS File
    snews_file = rr.next()
    snews_file = "root://fndca1.fnal.gov:1094/pnfs/fnal.gov/usr/" + snews_file[6:]
    #
    # III. Monopole File
    monopole_file = "root://fndca1.fnal.gov:1094/pnfs/fnal.gov/usr/" + monopole_file[6:]
    #
    # IV. Write FHICL
    job_f.write('#include "prodFDMonopoleCosmicMerge.fcl"\n')
    job_f.write("source.fileNames:                      [\"{}\"]\n".format(monopole_file))
    job_f.write("outputs.out1.fileName:                 \"{}\" \n".format(output_file))
    job_f.write("physics.filters.singlemixer.fileNames: [\"{}\"]\n".format(snews_file))
    job_f.write("filters.singlemixer.seed:              [{}] \n".format(randint(0,20480)))

