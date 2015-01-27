#!/bin/bash
#cd /star/u/lwen1990/ucla/v0_15GeV/embedding/Lambda
echo $SGE_TASK_ID
cd /global/homes/l/lwen1990/pwg/embedding/Lambda
pwd
#for (( i=0 ; $i - 12; i++ )) # here 2 means filenumber less than 20
#do
#   echo $i
#   condor_run "./run2.csh $i" > /dev/null 2> /dev/null &
#   sleep 1
#done
./run2.csh $(($SGE_TASK_ID-1))
#./run2.csh $(($SGE_TASK_ID-1)) 
