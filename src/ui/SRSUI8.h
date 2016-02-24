/**************************************************************************
 * Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               *
 **************************************************************************/
#ifndef AMORE_SRSUI8_H
#define AMORE_SRSUI8_H

#include <map>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>


#include <VisualModule.h>

#include "TNtuple.h"
#include "TList.h"
#include <TROOT.h>
#include <TColor.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TRootEmbeddedCanvas.h>
#include <TGNumberEntry.h>
#include <TGTextView.h>
#include <RQ_OBJECT.h>
#include <TGTab.h>

#include "TObject.h"
#include <TStyle.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TColor.h>
#include <TCanvas.h>
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TList.h"
#include "TString.h"
#include "TSystem.h"
#include "TObjArray.h"
#include "TObjString.h"

#include "SRSMapping.h"
#include "SRSConfiguration.h"
#include "../common/SRSCommon.h" // Absolute path necessary to allow usage of ACLiC

#define MAXNUMBERCANVAS 50 

using namespace std;

namespace amore {

  namespace SRS {
    
    namespace ui {

/**
@author Kondo Gnanvo
*/

      class SRSUI8 : public amore::ui::VisualModule, public amore::SRS::common::SRSCommon { 

	RQ_OBJECT("SRSUI8");

      public:

	SRSUI8();
	~SRSUI8();

	virtual void Construct();
	virtual void Update();
	virtual void SubscribeMonitorObjects();
	virtual void Process();
	virtual void StartOfCycle();
	virtual void EndOfCycle();
	virtual void StartOfRun() {};
	virtual void EndOfRun() {};
	virtual void StartOfSession() {};
	virtual void EndOfSession() {};
	void SlotSelectedTab(Int_t id);

	virtual bool StartAuto();
	virtual bool InitAuto();

      protected:

	void Update1();
	void MySubscribe(); 

	void RootDisplaySetup(Int_t ncount) ;
	void ClearDisplayParameters() ;

	Bool_t SetDisplayParameters();

	TGTab* fTab;
	TRootEmbeddedCanvas* fEC[MAXNUMBERCANVAS] ;
        SRSConfiguration * fSRSConf;
	SRSMapping * fMapping ;

	TString fRunType, fCFGFile, fDisplayCfg;

	size_t fCycle;
	int fSelectedTab;

	map<TString, Int_t> fRowMap, fColMap, fLengthMap, fWidthMap;
        multimap<TString, TString> fHistoNameMap ;

	ClassDef(SRSUI8, 1);
      };
    };
  };
};

#endif
