#ifndef __SRSAPVEVENT__
#define __SRSAPVEVENT__
/*******************************************************************************
 *  AMORE FOR SRS - SRS                                                         *
 *  SRSAPVEvent                                                                 *
 *  SRS Module Class                                                            *
 *  Author: Kondo GNANVO 18/08/2010                                             *
 *  Modified by Stefano Colafranceschi on 22/08/2015 to include ZS rawfiles     *
 *******************************************************************************/

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <map>
#include <list>
#include <vector>
#include <iostream>
#include "TList.h"
#include "TRandom.h"
#include "TObject.h"
#include "TMath.h"
#include "TObjArray.h"
#include "SRSHit.h"
#include "SRSMapping.h"
#endif

#define NCH 128

using namespace std;

class SRSAPVEvent : public TObject {
    
public:
    
    SRSAPVEvent(Int_t fec_no, Int_t fec_channel, Int_t apv_id, Int_t sigmaLevel, Int_t EventNumber, Int_t packet_size) ;
    ~SRSAPVEvent() ;
    
    void Add32BitsRawData(UInt_t rawdata32bits);
    void ComputeTimeBinCommonMode() ;
    
    void Set32BitsRawData(vector<UInt_t> rawdata32bits);
    vector<UInt_t> GetRawData32bits() {return fRawData32bits;}
    
    void ComputeRawData16bits();
    void ComputeRawData16bitsZS();
    
    vector<UInt_t> GetRawData16bits() {return fRawData16bits ;}
    
    
    multimap<Int_t, Float_t> GetTimeBinMap() {return fapvTimeBinDataMap ;}
    
    void ComputeMeanTimeBinPedestalData() ;
    vector<Float_t> GetPedestalData() {return fPedestalData ;}
    
    void ComputeMeanTimeBinRawPedestalData() ;
    vector<Float_t> GetRawPedestalData() {return fRawPedestalData ;}
    list <SRSHit * > ComputeListOfAPVHits() ;
    list <SRSHit * > ComputeListOfAPVHitsZS() ;
    
    
    Int_t APVchannelMapping(Int_t chNo) ;
    Int_t StandardMapping(Int_t) ;
    Int_t EICStripMapping(Int_t) ;
    Int_t CMSStripMapping(Int_t) ;
    Int_t CMSStripMapping1(Int_t) ;
    Int_t CMSStripMapping2(Int_t) ;
    Int_t CMSStripMapping3(Int_t) ;
    Int_t CMSStripMapping4(Int_t) ;
    Int_t HMSStripMapping(Int_t) ;
    Int_t NS2StripMapping(Int_t) ;
    Int_t ZigZagStripMapping(Int_t) ;
    Int_t MMStripMappingAPV1(Int_t) ;
    Int_t MMStripMappingAPV2(Int_t) ;
    Int_t MMStripMappingAPV3(Int_t) ;
    
    Int_t StripMapping(Int_t chNo) ;
    Int_t APVchannelCorrection(Int_t chNo) ;
    
    void SetZeroSupCut(Int_t sigmalevel)     {fZeroSupCut      = sigmalevel;}
    void SetAPVHeaderLevel(Int_t level)      {fAPVHeaderLevel  = level ;}
    void SetAPVIndexOnPlane(Int_t index)     {fAPVIndexOnPlane = index ;}
    
    void Print() ;
    void Clear() ;
    
    void SetFECNo(Int_t fecNo) {fFECNo = fecNo ;}
    Int_t GetFECNo()           {return fFECNo;}
    
    void SetAPVID(Int_t apvid) {fAPVID = apvid ;}
    Int_t GetAPVID()           {return fAPVID;}
    
    void SetAPVKey(Int_t apvkey) {fAPVKey = apvkey ;}
    Int_t GetAPVKey()            {return fAPVKey;}
    
    void SetAPV(TString apv) {fAPV = apv;}
    TString GetAPV()             {return fAPV;}
    
    void SetAPVGain(Float_t apvgain) {fAPVGain = apvgain ;}
    Float_t GetAPVGain()             {return fAPVGain;}
    
    void SetADCChannel(Int_t fecChannel) {fADCChannel = fecChannel ;}
    Int_t GetADCChannel()                {return fADCChannel;}
    
    void SetEventNumber(Int_t eventNb) {fEventNb = eventNb ;}
    Int_t GetEventNumber()             {return fEventNb;}
    
    Int_t GetAPVHeaderLevel()          {return fAPVHeaderLevel;}
    Int_t GetAPVIndexOnPlane()         {return fAPVIndexOnPlane;}
    
    void SetAPVOrientation(Int_t orient) {fAPVOrientation = orient;}
    Int_t GetAPVOrientation()            {return fAPVOrientation;}

    void SetAPVstripmapping(Int_t stripmapping) {fAPVstripmapping = stripmapping;}
    Int_t GetAPVstripmapping()            {return fAPVstripmapping;}

    void SetPacketSize(Int_t size)   {fPacketSize = size;}
    Int_t GetPacketSize()            {return fPacketSize;}
    
    void SetNbOfAPVsFromPlane(Int_t nb) {fNbOfAPVsFromPlane = nb;}
    Int_t GetNbOfAPVsFromPlane()        {return fNbOfAPVsFromPlane;}
    
    void SetPlaneSize(Float_t planesize) {fPlaneSize = planesize;}
    Float_t GetPlaneSize()               {return fPlaneSize;}
    
    void SetPlane(TString plane) {fPlane = plane;}
    TString GetPlane()           {return fPlane;}
    
    void SetPedSubFlag(Bool_t pedSub)         {fPedSubFlag = pedSub;}
    void SetCommonModeFlag(Bool_t commonmode) {fCommonModeFlag = commonmode;}
    
    void SetAllFlags(Bool_t pedSub, Bool_t commonmode) {
        SetPedSubFlag(pedSub) ;
        SetCommonModeFlag(commonmode) ;
    }
    
    void SetPedestals(vector<Float_t> noises, vector<Float_t> offsets, vector<Float_t> maskChs) {
        fMaskedChannels = maskChs;
        fPedestalNoises = noises ;
        fPedestalOffsets = offsets ;
    }
    
    void SetRawPedestals(vector<Float_t> noises, vector<Float_t> offsets) {
        fRawPedestalNoises = noises ;
        fRawPedestalOffsets = offsets ;
    }
    
    Float_t GetMeanAPVnoise() { return fMeanAPVnoise ;}
    
    
    void SetHitMaxOrTotalADCs(TString isHitMaxOrTotalADCs) {fIsHitMaxOrTotalADCs = isHitMaxOrTotalADCs ;}
    TString GetHitMaxOrTotalADCs() {return fIsHitMaxOrTotalADCs;}
    
private:
    
    Int_t fEventNb, fFECNo, fADCChannel, fAPVID, fAPVKey, fZeroSupCut, fNbCluster ;
    Int_t fAPVIndexOnPlane, fAPVOrientation, fAPVstripmapping, fNbOfAPVsFromPlane, fAPVHeaderLevel, fPacketSize;
    Float_t fPlaneSize, fEtaSectorPos, fAPVGain, fMeanAPVnoise;
    Bool_t fCommonModeFlag, fPedSubFlag, fIsCosmicRunFlag, fIsPedestalRunFlag, fIsRawPedestalRunFlag, fAPVGainFlag;
    TString fAPV, fPlane, fDetector, fDetectorType, fReadoutBoard, fIsHitMaxOrTotalADCs ;
    
    Float_t fTrapezoidDetLength, fTrapezoidDetOuterRadius, fTrapezoidDetInnerRadius;
    
    vector<UInt_t> fRawData16bits, fRawData32bits;
    multimap<Int_t, Float_t> fapvTimeBinDataMap ;
    vector<Float_t> fPedestalData, fRawPedestalData, fPedestalOffsets, fRawPedestalOffsets,fPedestalNoises, fRawPedestalNoises, fMaskedChannels;
    
    vector<Float_t>  fPadDetectorMap, fCommonModeOffsets, fCommonModeOffsets_odd,   fCommonModeOffsets_even ;
    
    map <Int_t, Int_t> fapvChToPadChMap ;
    
    ClassDef(SRSAPVEvent, 1)
};

#endif
