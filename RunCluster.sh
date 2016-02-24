#!/bin/sh

#GENERAL SETTINGS
datapath=/data
amorepath=/Users/tetto/Documents/FIT/GEMs/SRS/Analysis/Amore/amoreSRS
results=results/run_cluster_$1
temp="temp.txt"
cycles=1

if [ "$#" -eq 3 ]; then
cp $amorepath/configFileDir/amore.cfg_cl $amorepath/configFileDir/amore.cfg
cp $amorepath/configFileDir/amore.cfg $amorepath/configFileDir/amore.cfg_cl
   for (( c=0; c<=$3; c++ ))
   do
      let events=$2/$3
      let startevent=$c*$events
      echo "Instance $c with $events starting at $startevent"

      #Amore config file changing start_event
      old="STARTEVENTNUMBER"
      new="STARTEVENTNUMBER  $startevent"
      sed "s/$old.*/$new/g" $amorepath/configFileDir/amore.cfg > $temp
      mv $temp $amorepath/configFileDir/amore.cfg

      #Amore config file changing root output
      old="RUNNAME"
      new="RUNNAME            $amorepath/$results-$startevent"
      sed "s|$old.*|$new|g" $amorepath/configFileDir/amore.cfg > $temp
      mv $temp $amorepath/configFileDir/amore.cfg


      #Amore start
      #amoreAgent -a SRS01 -s $datapath/fitgem$1.raw -e $events -c $cycles
   done

else
echo "Usage ./runZS RUN_NUMBER EVENTS INSTANCES"
fi


