#!/bin/tcsh
#setenv STAR_LEVEL SL11d
if ( -e $GROUP_DIR/group_env.csh ) then
        source $GROUP_DIR/group_env.csh
endif

#cd /star/u/lwen1990/ucla/v0_15GeV/embedding/Lambda
cd /global/homes/l/lwen1990/pwg/embedding/Lambda
#root4star -b -q reconstruct_mcv0.C\(\"xi_150.list\",$1,1\) 
#root4star -b -q reconstruct_mcv0.C\(\"xi_2xx.list\",$1,1\) 
#root4star -b -q reconstruct_mcv0.C\(\"k0s_3xx.list\",$1,1\) 
#root4star -b -q reconstruct_mcv0.C\(\"k0s_2xx.list\",$1,1\) 
#root4star -b -q reconstruct_mcv0.C\(\"la_15x.list\",$1,1\) 
#root4star -b -q reconstruct_mcv0.C\(\"ala_15x.list\",$1,1\) 
#root4star -b -q reconstruct_mcv0.C\(\"ala_27x.list\",$1,1\) 
#root4star -b -q reconstruct_mcv0.C\(\"ala_2xx.list\",$1,1\) 
#root4star -b -q reconstruct_mcv0.C\(\"la_2xx.list\",$1,1\) 
#root4star -b -q reconstruct_mcv0.C\(\"om_150.list\",$1,1\) 
#root4star -b -q reconstruct_mcv0.C\(\"ksexp_100_140.list\",$1,1\) 
#root4star -b -q reconstruct_mcv0.C\(\"ksfp_100_144.list\",$1,1\) 
#root4star -b -q reconstruct_mcv0.C\(\"alafp_101_141.list\",$1,1\) 
#root4star -b -q reconstruct_mcv0.C\(\"Lambda_test.list\",$1,1\)
root4star -b -q reconstruct_mcv0.C\(\"auau200GeV_run11_antila_flat.list\",$1,1\)
