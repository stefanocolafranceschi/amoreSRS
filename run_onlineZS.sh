#!/bin/sh
read datafolder</srsconfig/rawdatadir.txt
read originalrawfile</srsconfig/rawdatafile.txt
if [ "$#" -eq 1 ]; then 

  if [ -f $datafolder/$originalrawfile ]
    then
       amoreAgent -a SRS01 -s $datafolder/$originalrawfile -e $1 -c 1
       root /home/SRSUser/amoreSRS/results/temp_fitgem_dataTree.root
  else
    echo "Data-taking is not happening!"
  fi
else
   echo "Usage ./run_onlineZS EVENTS"
fi
