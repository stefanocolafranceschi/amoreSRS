#ifndef SRSCLUSTER_H
#define SRSCLUSTER_H
/*******************************************************************************
 *  AMORE FOR SRS - SRS                                                         *
 *  SRSCluster                                                                  *
 *  SRS Module Class                                                            *
 *  Author: Kondo GNANVO 18/08/2010                                             *
 *******************************************************************************/

#if !defined(__CINT__) || defined(__MAKECINT__)

#include <iostream>
#include "SRSHit.h"

#include <TMath.h>
#include <TObject.h>
#include <TObjArray.h>
#include "TFile.h"
#endif

using namespace std;

class SRSCluster : public TObject {
    
public:
    SRSCluster(Int_t minClusterSize, Int_t maxClusterSize, TString isMaximumOrTotalCharges) ;
    ~SRSCluster();
    
    Bool_t IsSortable() const {return kTRUE;}
    TObjArray* GetArrayOfHits() {return fArrayOfHits;}
    
    SRSHit * GetHit(Int_t i) {
        TObjArray &temp = * fArrayOfHits ;
        return (SRSHit *)temp[i];
    }
    
    void SetMinClusterSize(Int_t min) {fMinClusterSize = min ; }
    void SetMaxClusterSize(Int_t max) {fMaxClusterSize = max ; }
    
    void AddHit(SRSHit * h) ;
    void Timing() ;
    
    int Compare(const TObject *obj) const ;
    
    Int_t & GetNbOfHits() { return fNbOfHits ;  }
    
    TString GetPlane()     {return fPlane;}
    void SetPlane(TString planename) {fPlane = planename;}
    
    Int_t GetNbAPVsFromPlane() {return fNbAPVsOnPlane;}
    void SetNbAPVsFromPlane(Int_t nb)    {fNbAPVsOnPlane = nb;}
    
    Float_t GetPlaneSize()     {return fPlaneSize;}
    void SetPlaneSize(Float_t planesize) {fPlaneSize = planesize;}
    
    Float_t & GetClusterPosition()     {return fposition;}
    Float_t & GetClusterCentralStrip() {return fclusterCentralStrip;}
    
    Int_t GetClusterTimeBin()  ;
    Int_t GetClusterPeakTimeBin() {return fClusterPeakTimeBin ;}
    
    Float_t GetClusterADCs() ;
    void SetClusterADCs(Float_t adc) {fClusterSumADCs = adc;}
    
    void Dump() ;
    void ClearArrayOfHits();
    Bool_t IsGoodCluster() ;
    
    void ClusterCentralStrip();
    void ClusterPositionHistoMean() ;
    void ClusterPositionGausFitMean() ;
    void ClusterPositionPulseHeghtWeight() ;
    void GetClusterPositionCorrectionHisto() ;
    void GetClusterPositionAfterCorrection() ;
    vector< Float_t > GetClusterTimeBinADCs () {return fClusterTimeBinADCs; } ;
    
    void ComputeClusterPositionWithCorrection() ;
    void ComputeClusterPositionWithoutCorrection() ;
    
    void ComputeClusterPositionWithCorrection(const char * filename) ;
    void SetClusterPositionCorrection(Bool_t isCluserPosCorrection) { fIsCluserPosCorrection = isCluserPosCorrection;}
    
private:
    
    int         fNbOfHits;     // numbers of strips with hit
    TObjArray * fArrayOfHits;  // APV hits table
    Int_t fClusterPeakTimeBin, fClusterTimeBin;
    Float_t fClusterPeakADCs, fClusterTimeBinADC, fClusterSumADCs, fposition, fclusterCentralStrip, fstrip, fPlaneSize;
    Int_t fapvID, fStripNo, fAbsoluteStripNo, fapvIndexOnPlane, fNbAPVsOnPlane, fMinClusterSize, fMaxClusterSize;
    TString fIsClusterMaxOrSumADCs, fPlane;
    Bool_t fIsGoodCluster, fIsCluserPosCorrection ;
    
    vector< Float_t > fClusterTimeBinADCs ;
    
    ClassDef(SRSCluster,1) 
}; 
#endif
