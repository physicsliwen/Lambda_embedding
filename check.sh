#!/bin/sh

count=0

for i in `cat Lambda_exp.list`
do
   if [ ! -e output_exp/Lambda_exp.$count.la.picodst.root ] ; then
	echo $count
	#ls -l output.k0s3xx.final/k0s_3xx.$count.ks.picodst.root
   fi
   count=$(($count+1))
done
   
