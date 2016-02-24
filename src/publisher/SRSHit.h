#ifndef SRSHIT_H
#define SRSHIT_H
/*******************************************************************************
 *  AMORE FOR SRS - SRS                                                         *
 *  SRSHit                                                                      *
 *  SRS Module Class                                                            *
 *  Author: Kondo GNANVO 18/08/2010                                             *
 *******************************************************************************/

#if !defined(__CINT__) || defined(__MAKECINT__)

#include "SRSAPVSignalFit.h"

#include <map>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include "TList.h"
#include "TObject.h"
#include "TMath.h"
#include "TGraph.h"
#include "TVector.h"
#include <stdlib.h>
#endif

#define NCH 128
using namespace std;

class SRSHit : public TObject {
    
public:
    
    ~SRSHit() ;
    SRSHit() ;
    
    Bool_t IsSortable() const { return kTRUE; }
    
    void IsHitFlag(Bool_t hitOrNoise) { fIsHit = hitOrNoise ;}
    Bool_t IsHit() {return fIsHit ;}
    
    void Timing() ;
    
    //=== Sort hit according to the strip number
    Int_t Compare(const TObject *obj) const { return (fStripNo > ((SRSHit*)obj)->GetStripNo()) ? 1 : -1; }
    
    void ComputePosition() ;
    void AddTimeBinADCs(Float_t charges) {fTimeBinADCs.push_back(charges);}
    
    void SetTimeBinADCs(vector<Float_t> timebinCharges) {
        fTimeBinADCs.clear() ;
        fTimeBinADCs = timebinCharges ;
    }
    
    void SetPadDetectorMap(vector<Float_t> padDetectorMap) {
        fPadDetectorMap.clear() ;
        fPadDetectorMap = padDetectorMap ;
        fPadDetectorMap.resize(5) ;
    }
    
    void ClearTimeBinADCs() {fTimeBinADCs.clear() ;}
    
    vector<Float_t> GetTimeBinADCs() { return fTimeBinADCs ;}
    
    Int_t GetAPVID()            {return fapvID;}
    void  SetAPVID(Int_t apvID) {fapvID = apvID;}
    void SetHitADCs(Int_t sigmaLevel, Float_t charges, TString isHitMaxOrTotalADCs) ;
    Float_t GetHitADCs()   {return fHitADCs;}
    
    Int_t GetSignalPeakBinNumber()   {
        Timing() ;
        return fSignalPeakBinNumber;
    }
    
    TString GetHitMaxOrTotalADCs() { return fIsHitMaxOrTotalADCs ;}
    
    Int_t GetAPVIndexOnPlane()            {return fapvIndexOnPlane;}
    void  SetAPVIndexOnPlane(Int_t index) {fapvIndexOnPlane = index;}
    
    Int_t GetNbAPVsFromPlane()         {return fNbAPVsOnPlane;}
    void  SetNbAPVsFromPlane(Int_t nb) {fNbAPVsOnPlane = nb;}
    
    Int_t GetAPVOrientation()         {return fAPVOrientation;}
    void  SetAPVOrientation(Int_t nb) {fAPVOrientation = nb;}

    Int_t GetAPVstripmapping()         {return fAPVstripmapping;}
    void  SetAPVstripmapping(Int_t nb) {fAPVstripmapping = nb;}

    Float_t GetPlaneSize() {return fPlaneSize;}
    void SetPlaneSize(Float_t planesize) {fPlaneSize = planesize;}
    
    Float_t GetTrapezoidDetInnerRadius() {return fTrapezoidDetInnerRadius;}
    Float_t GetTrapezoidDetOuterRadius() {return fTrapezoidDetOuterRadius;}
    
    void SetTrapezoidDetRadius(Float_t innerRadius, Float_t outerRadius) {
        fTrapezoidDetInnerRadius = innerRadius ;
        fTrapezoidDetOuterRadius = outerRadius ;
    }
    
    
    TString GetPlane() {return fPlane;}
    void SetPlane(TString plane) {fPlane = plane;}
    
    TString GetDetector() {return fDetector;}
    void SetDetector(TString detector) {fDetector = detector;}
    
    TString GetReadoutBoard() {return fReadoutBoard;}
    void SetReadoutBoard(TString readoutBoard) {fReadoutBoard = readoutBoard;}
    
    TString GetDetectorType() {return fDetectorType;}
    void SetDetectorType(TString detectorType) {fDetectorType = detectorType;}
    
    void  SetStripNo(Int_t stripNo) ;
    Int_t GetStripNo()         {return fStripNo;}
    Int_t GetAbsoluteStripNo() {return fAbsoluteStripNo;}
    
    void  SetPadNo(Int_t padno)  {fPadNo = padno;}
    Int_t GetPadNo() {return fPadNo;}
    
    Float_t GetStripPosition() {
        ComputePosition() ;
        return fStripPosition;
    }
    
    vector<Float_t> GetPadPosition() {
        ComputePosition() ;
        return fPadPosition ;
    }
    
private:
    
    Bool_t fIsHit ;
    Int_t fapvID, fStripNo, fPadNo, fAbsoluteStripNo, fapvIndexOnPlane, fNbAPVsOnPlane, fAPVOrientation, fAPVstripmapping, fSignalPeakBinNumber;
    Float_t fHitADCs, fStripPosition, fPlaneSize, fTrapezoidDetInnerRadius, fTrapezoidDetOuterRadius;
    TString fPlane, fReadoutBoard, fDetectorType, fDetector, fIsHitMaxOrTotalADCs;
    
    vector<Float_t> fTimeBinADCs, fPadDetectorMap, fPadPosition  ;
    ClassDef(SRSHit,1) 
};

#endif
