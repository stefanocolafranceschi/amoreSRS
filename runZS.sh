#!/bin/sh
if [ "$#" -eq 2 ]; then 
    amoreAgent -a SRS01 -s /data/fitgem$1.raw -e $2 -c 1
    mv /home/SRSUser/amoreSRS/results/temp_fitgem_dataTree.root /data/fitgem$1.root
else
   echo "Usage ./runZS RUN_NUMBER EVENTS"
fi
