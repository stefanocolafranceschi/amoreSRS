#ifndef __SRSEVENTBUILDER__
#define __SRSEVENTBUILDER__
/*******************************************************************************
 *  AMORE FOR SRS - SRS                                                         *
 *  SRSEventBuilder                                                             *
 *  SRS Module Class                                                            *
 *  Author: Kondo GNANVO 18/08/2010                                             *
 *******************************************************************************/

#if !defined(__CINT__) || defined(__MAKECINT__)

#include <map>
#include <list>
#include <vector>
#include <iostream>

#include "TList.h"
#include "TObject.h"
#include "SRSMapping.h"
#include "SRSPedestal.h"
#include "SRSCluster.h"
#include "TFile.h"
#endif

#define PI 3.14159265359

using namespace std;
class SRSEventBuilder : public TObject {
    
public:
    
    SRSEventBuilder(Int_t triggerNb, TString maxClusterSize, TString minClusterSize, TString nbofsigma, TString runType, Bool_t isCluserPosCorrection ) ;
    
    
    ~SRSEventBuilder() ;
    
    template <typename M> void ClearVectorMap( M & amap ) ;
    
    void AddHitInDetectorPlane(SRSHit* hit) {fHitsInDetectorPlaneMap[hit->GetPlane()].push_back(hit) ;}
    
    void AddHit(SRSHit* hit ) {fListOfHits->Add(hit) ;}
    
    void AddMeanDetectorPlaneNoise(TString detPlaneName, Float_t meanAPVnoise) { fDetectorPlaneNoise[detPlaneName].push_back(meanAPVnoise);}
    
    void ComputeClustersInDetectorPlane() ;
    void AddAPVEvent(SRSAPVEvent * apvEvent) {fListOfAPVEvents->AddAt(apvEvent, apvEvent->GetAPVKey()); }
    TString GetRunType() {return fRunType;}
    
    TList * GetListOfHits() {return fListOfHits;}
    
    map < TString, list <SRSHit * > > GetHitsInDetectorPlane() {return fHitsInDetectorPlaneMap;}
    map < TString, list <SRSCluster * > >  GetClustersInDetectorPlane() {return fClustersInDetectorPlaneMap ;}
    
    void SetTriggerList(map<TString, TString> triggerList)  {fTriggerList = triggerList ;}
    Int_t GetTriggerCount() {return fTriggerCount;}
    
    Bool_t IsAGoodEvent()  ;
    Bool_t IsAGoodClusterEvent(TString detPlaneName)  ;
    Bool_t IsAGoodEventInDetector(TString detName) ;
    
    Bool_t IsCluserPosCorrection() {return fIsClusterPosCorrection ;}
    
    //  list <SRSCluster * > CrossTalkCorrection( list <SRSCluster * >  listOfClusters) ;
    
    SRSAPVEvent * GetAPVEventFromAPVKey(Int_t apvKey) {return (SRSAPVEvent*) (fListOfAPVEvents->At(apvKey));}
    
    void DeleteListOfAPVEvents(TList * listOfAPVEvents) ;
    void DeleteListOfHits(TList * listOfHits) ;
    void DeleteListOfClusters(TList * listOfClusters) ;
    
    void DeleteHitsInDetectorPlaneMap( map < TString, list <SRSHit * > >  & stringListMap) ;
    void DeleteClustersInDetectorPlaneMap( map < TString, list <SRSCluster * > > & stringListMap) ;
    Float_t GetDetectorPlaneNoise(TString planeName) { return TMath::Mean(fDetectorPlaneNoise[planeName].begin(), fDetectorPlaneNoise[planeName].end());}
    
    map < Int_t, vector <Float_t > > GetDetectorCluster(TString detector) ;
    
    void SetClusterPositionCorrection(Bool_t isClusterPosCorrection) {fIsClusterPosCorrection = isClusterPosCorrection;}
    void SetClusterPositionCorrectionRootFile(const char * filename) {fClusterPositionCorrectionRootFile = filename;}
    
    void SetHitMaxOrTotalADCs(TString isHitMaxOrTotalADCs) {fIsHitMaxOrTotalADCs = isHitMaxOrTotalADCs ;}
    TString GetHitMaxOrTotalADCs() {return fIsHitMaxOrTotalADCs;}
    
    void SetMaxClusterMultiplicity(TString maxClusterMult) {fMaxClusterMultiplicity = maxClusterMult.Atoi() ;}
    TString GetMaxClusterMultiplicity() {return fMaxClusterMultiplicity;}
    
    void SetClusterMaxOrTotalADCs(TString isClusterMaxOrTotalADCs) { fIsClusterMaxOrTotalADCs = isClusterMaxOrTotalADCs ;}
    TString GetClusterMaxOrTotalADCs() {return fIsClusterMaxOrTotalADCs;}
    
private:
    
    Int_t fTriggerCount, fMinClusterSize, fMaxClusterSize, fMaxClusterMultiplicity, fZeroSupCut ;
    
    TList *  fListOfAPVEvents, * fListOfHits;
    TString fIsClusterMaxOrTotalADCs, fIsHitMaxOrTotalADCs, fRunType ;
    Bool_t fIsGoodEvent, fIsGoodClusterEvent, fIsClusterPosCorrection ;
    
    map < TString, vector <Float_t > > fDetectorPlaneNoise ;
    map<TString, TString> fTriggerList ;
    map < TString, list <SRSHit * > >  fHitsInDetectorPlaneMap ;
    map < TString, list <SRSCluster * > >  fClustersInDetectorPlaneMap ;
    
    const char * fClusterPositionCorrectionRootFile ;
    
    ClassDef(SRSEventBuilder, 1)
};
#endif
