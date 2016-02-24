#ifndef SRSAPVSIGNALFIT_H
#define SRSAPVSIGNALFIT_H
/*******************************************************************************
 *  AMORE FOR SRS - SRS                                                         *
 *  SRSAPVSignalFit                                                             *
 *  SRS Module Class                                                            *
 *  Author: Kondo GNANVO 18/08/2010                                             *
 *******************************************************************************/

#if !defined(__CINT__) || defined(__MAKECINT__)

#include <map>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include "TH1.h"
#include "TF1.h"
#include "TMath.h"
#include "TList.h"
#include "TObject.h"
#endif

using namespace std;

class SRSAPVSignalFit : public TObject {
    
public:
    
    ~SRSAPVSignalFit() ;
    SRSAPVSignalFit(Int_t apvID, Int_t stripNo) ;
    
    void SetAPVID(Int_t apvID) {fapvID = apvID ;}
    void SetStripNo(Int_t stripNo) {fStripNo = stripNo ;}
    
    Double_t GetAPVStripPulseHeight(vector<Double_t> apvTimingSignal);
    
private:
    
    Int_t fapvID;   // APV channel ID
    Int_t fStripNo; // strip number
    
    ClassDef(SRSAPVSignalFit,1) 
};

#endif
