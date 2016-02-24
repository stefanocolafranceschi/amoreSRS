#ifndef __SRSOUTPUTROOT__
#define __SRSOUTPUTROOT__
/*******************************************************************************
 *  AMORE FOR SRS - SRS                                                         *
 *  SRSOutputROOT                                                                    *
 *  SRS Module Class                                                            *
 *  Author: Kondo GNANVO, Mike Staib 01/05/2011                                 *
 *******************************************************************************/
#if !defined(__CINT__) || defined(__MAKECINT__)

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "TFile.h"
#include "TTree.h"

#include "TObject.h"
#include "TString.h"

#include "SRSHit.h"
#include "SRSMapping.h"
#include "SRSConfiguration.h"
#include "SRSEventBuilder.h"
#include "SRSTrack.h"
#include "event.h"

#endif

using namespace std ;

class SRSOutputROOT : public TObject {
    
public:
    
    SRSOutputROOT(TString zeroSupCut, TString rootdatatype);
    //  SRSOutputROOT(const char * cfgname, TString zeroSupCut, TString rootdatatype);
    ~SRSOutputROOT();
    
    //  void InitRootFile(const char * cfgname) ;
    void InitRootFile() ;
    void WriteRootFile() ;
    void FillRootFile(SRSEventBuilder * eventbuilder, SRSTrack * track) ;
    
    void FillHitsTree(SRSEventBuilder * eventbuilder) ;
    void FillClustersTree(SRSEventBuilder * eventbuilder) ;
    void FillTrackingTree(SRSEventBuilder * eventbuilder, SRSTrack * track) ;
    
    void SetRunName(TString runname)     {fRunName = runname;}
    void SetRunType(TString runtype)     {fRunType = runtype;}
    void SetZeroSupCut(Int_t zeroSupCut) {fZeroSupCut = zeroSupCut;}
    
    /**
     void SetRunFileParameters(TString filePrefix, TString fileValue) {
     fRunFilePrefix = filePrefix ;
     fRunFileValue = fileValue ;
     }
     */
    
    void SetRunParameters(TString amoreAgentId, TString runtype, TString runname, TString filePrefix, TString fileValue) {
        fAmoreAgentID = amoreAgentId ;
        fRunType = runtype ;
        fRunName = runname ;
        fRunFilePrefix = filePrefix ;
        fRunFileValue = fileValue ;
    }
    
    
    void DeleteHitsTree() ;
    void DeleteClustersTree() ;
    void DeleteTrackingTree() ;
    
private:
    
    TFile * fFile ;
    TTree * fHitTree ;
    TTree * fClusterTree ;
    TTree * fTrackingTree ;
    
    TString fRunType, fRunName, fRunFilePrefix, fRunFileValue, fAmoreAgentID;
    TString fROOTDataType;
    
    Int_t m_evtID, m_chID, m_nclust, fZeroSupCut;            //
    
    Int_t * m_planeID;      // Plane Number
    Int_t * m_strip;        // Strip Number
    Int_t * m_detID;        // Detector Number
    Short_t * m_etaSector;  // Detector Number
    
    Int_t * m_hit_planeID;     // Plane Number
    Int_t * m_hit_detID;       // Detector Number
    Int_t * m_hit_timeBin;     // time sample of the strip with maximum ADCs
    Short_t * m_hit_etaSector; // Detector Number
    
    Float_t * m_clustPos;    // Cluster position
    Float_t * m_clustADCs;   // cluster charge
    
    Int_t   * m_clustSize;   // Cluster position
    Int_t * m_clustTimeBin; // time sample of the strip with maximum ADCs
    
    Float_t * m_REF1;
    Float_t * m_REF2;
    Float_t * m_REF3;
    Float_t * m_REF4;
    Float_t * m_SBS1;
    Float_t * m_UVAEIC;
    
    
    Short_t * m_adc0;     //ADC value for 1st time sample
    Short_t * m_adc1;     //ADC value for 2nd time sample
    Short_t * m_adc2;     //ADC value for 3rd time sample
    Short_t * m_adc3;     //ADC value for 4th time sample
    Short_t * m_adc4;     //ADC value for 5th time sample
    Short_t * m_adc5;     //ADC value for 6th time sample
    Short_t * m_adc6;     //ADC value for 7th time sample
    Short_t * m_adc7;     //ADC value for 8th time sample
    Short_t * m_adc8;     //ADC value for 9th time sample
    Short_t * m_adc9;     //ADC value for 10th time sample
    Short_t * m_adc10;     //ADC value for 11th time sample
    Short_t * m_adc11;     //ADC value for 12th time sample
    Short_t * m_adc12;     //ADC value for 13th time sample
    Short_t * m_adc13;     //ADC value for 14th time sample
    Short_t * m_adc14;     //ADC value for 15th time sample
    Short_t * m_adc15;     //ADC value for 16th time sample
    Short_t * m_adc16;     //ADC value for 17th time sample
    Short_t * m_adc17;     //ADC value for 18th time sample
    Short_t * m_adc18;     //ADC value for 19th time sample
    Short_t * m_adc19;     //ADC value for 20th time sample
    Short_t * m_adc20;     //ADC value for 21th time sample
    Short_t * m_adc21;     //ADC value for 22th time sample
    Short_t * m_adc22;     //ADC value for 23th time sample
    Short_t * m_adc23;     //ADC value for 24th time sample
    Short_t * m_adc24;     //ADC value for 25th time sample
    Short_t * m_adc25;     //ADC value for 26th time sample
    Short_t * m_adc26;     //ADC value for 27th time sample
    Short_t * m_adc27;     //ADC value for 28th time sample
    Short_t * m_adc28;     //ADC value for 29th time sample
    Short_t * m_adc29;     //ADC value for 30th time sample
    
    ClassDef(SRSOutputROOT,1)
};

#endif

