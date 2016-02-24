#ifndef __SRSPOSITIONCORRECTION__
#define __SRSPOSITIONCORRECTION__
/*******************************************************************************
 *  AMORE FOR SRS - SRS                                                         *
 *  SRSPositionCorrection                                                              *
 *  SRS Module Class                                                            *
 *  Author: Kondo GNANVO 06/06/2014                                             *
 *******************************************************************************/

#if !defined(__CINT__) || defined(__MAKECINT__)

#include <sstream>
#include <fstream>
#include <iostream>
#include "TObject.h"
#include <TStyle.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TColor.h>
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TList.h"
#include "TString.h"
#include "TSystem.h"
#include "TObjArray.h"
#include "TObjString.h"

#include <map>
#include <list>
#include <vector>
#include <iostream>


#include "SRSCluster.h"
#include "SRSMapping.h"

#endif

//#define NCH 128

using namespace std;
class SRSPositionCorrection : public TObject {
    
public:
    
    SRSPositionCorrection(TString runname, TString runtype) ;
    
    SRSPositionCorrection();
    ~SRSPositionCorrection();
    
    TH1F * BookHisto(TString histoName, TString histoTitle) ;
    void BookClusterPositionCorrection() ;
    
    void Clear() ;
    void GetStyle() ;
    
    void AddHitInDetectorPlane(SRSHit* hit) { fHitsInDetectorPlaneMap[hit->GetPlane()].push_back(hit) ; }
    void ComputeClustersInDetectorPlane() ;
    
    void FillClusterPositionCorrection() ;
    void SaveClusterPositionCorrectionHistos() ;
    
    void LoadClusterPositionCorrection(const char * filename) ;
    
    void DeleteHitsInDetectorPlaneMap( map < TString, list <SRSHit * > >  & stringListMap) ;
    void DeleteClustersInDetectorPlaneMap( map<TString, list <SRSCluster * > > & stringListMap) ;
    
    Bool_t IsEtaFunctionComputed(){ return fIsEtaFunctionComputed; }
    static SRSPositionCorrection * GetClusterPositionCorrectionRootFile(const char * filename);
    
    void SetRunName(TString runname) {fRunName = runname;}
    void SetRunType(TString runtype) {fRunType = runtype;}
    
    //  void PolynomialFit( TH1F* h, Float_t start, Float_t end) ;
    void PolynomialFit( TH1F* h) ;
    
private:
    
    TString fRunName, fRunType ;
    
    Bool_t fIsEtaFunctionComputed ;
    
    TH1F ** fEtaFunctionHistos;
    
    TH1F ** fEta2PosHistos;
    TH1F ** fEta3PosHistos;
    TH1F ** fEta4PosHistos;
    TH1F ** fEta5PosHistos;
    TH1F ** fEta6PlusPosHistos;
    
    TH1F ** fEta2NegHistos;
    TH1F ** fEta3NegHistos;
    TH1F ** fEta4NegHistos;
    TH1F ** fEta5NegHistos;
    TH1F ** fEta6PlusNegHistos;
    
    Int_t fTriggerCount, fMinClusterSize, fMaxClusterSize, fMaxClusterMultiplicity, fZeroSupCut ;
    
    map < TString, list <SRSHit * > >  fHitsInDetectorPlaneMap ;
    map < TString, list <SRSCluster * > >  fClustersInDetectorPlaneMap ;
    
    
    ClassDef(SRSPositionCorrection,7) 
};


#endif
