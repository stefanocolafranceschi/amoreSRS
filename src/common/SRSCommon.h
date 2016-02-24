/**************************************************************************
 * Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               *
 **************************************************************************/
#ifndef AMORE_SRS_COMMONSRSCOMMON_H
#define AMORE_SRS_COMMONSRSCOMMON_H

#include "TSystem.h"
#include <MonitorObject.h>

namespace amore {

  namespace SRS {

    namespace common {

/**
@author Kondo Gnanvo
*/

      class SRSCommon {
      public:

	SRSCommon();
	~SRSCommon();

      protected:
	/**
        amore::core::MonitorObjectString *  moDisplayCfgFilename ;
        amore::core::MonitorObjectString *moString, *moDetPlanesDisplay, *moDetectorsDisplay;
        amore::core::MonitorObjectH1I    *moHit1D, *moHitDist, *moClusterSizeDist, *moPulseHeight1D ;
	amore::core::MonitorObjectH2I    *moHitMap ;
	amore::core::MonitorObjectGraph  *moChargesShGraph, *moClusterCorrGraph;
        */
      };
    };
  };
};

#endif
