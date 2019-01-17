#!/bin/bash

# export OMP_NUM_THREADS=8
export CALI_CONFIG_FILE=~/soft/src/Caliper/examples/configs/papi_cycles.conf

export CALI_REPORT_FILENAME=papi_test_main_anno.json
export CALI_PAPI_COUNTERS=PAPI_TOT_CYC
./mm_foo

#export CALI_REPORT_FILENAME=papi_tot_ins.json
#export CALI_PAPI_COUNTERS=PAPI_TOT_INS
#./mm

#export CALI_REPORT_FILENAME=papi_L1_tcm.json
#export CALI_PAPI_COUNTERS=PAPI_L1_TCM
#./mm

#export CALI_REPORT_FILENAME=papi_lst_ins.json
#export CALI_PAPI_COUNTERS=PAPI_LST_INS
#./mm





