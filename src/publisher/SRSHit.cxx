#include "SRSHit.h"
ClassImp(SRSHit);

//============================================================================================
SRSHit::SRSHit() {
    fTimeBinADCs.clear();
    fPadDetectorMap.clear();
    fPadPosition.clear() ;
    fapvID   = 0 ;
    fHitADCs = 0 ;
    fStripNo = 0 ;
    fPadNo   = 0 ;
    fStripPosition = 0 ;
    fAbsoluteStripNo = 0 ;
    fPlaneSize = 512.0 ;
    fTrapezoidDetInnerRadius = 220.0 ;
    fTrapezoidDetOuterRadius = 430.0 ;
    fDetector = "GEM1" ;
    fPlane    = "GEM1X" ;
    fReadoutBoard = "CARTESIAN" ;
    fDetectorType = "CARTESIAN" ;
    fapvIndexOnPlane = 0 ;
    fNbAPVsOnPlane = 6 ;
    fAPVOrientation = 0 ;
    fAPVstripmapping = 0 ;
    fIsHit = kTRUE ;
    fSignalPeakBinNumber = 0 ;
    fIsHitMaxOrTotalADCs = "signalPeak" ;
}

//============================================================================================
void SRSHit::Timing() {
    Bool_t timingStatus = kTRUE ;
    Int_t nBins = fTimeBinADCs.size() ;
    TH1F * timeBinHist = new TH1F("timeBinHist", "timeBinHist", nBins, 0, (nBins-1) ) ;
    for (Int_t k = 0; k < nBins; k++) {
        timeBinHist->Fill(k,fTimeBinADCs[k]) ;
    }
    fSignalPeakBinNumber = timeBinHist->GetMaximumBin() ;
    delete timeBinHist ;
}

//============================================================================================
void SRSHit::SetHitADCs(Int_t zeroSupCut, Float_t charges, TString isHitMaxOrTotalADCs) {
    fIsHitMaxOrTotalADCs = isHitMaxOrTotalADCs ;
    
    if(zeroSupCut > 0 ) {
        Float_t totalADCs = 0, minADCs = 0, maxADCs = 0 ;
        Int_t nBins = fTimeBinADCs.size() ;
        for (Int_t k = 0; k < nBins; k++) {
            totalADCs += fTimeBinADCs[k] ;
            if(fTimeBinADCs[k] > maxADCs) maxADCs = fTimeBinADCs[k] ;
        }
        totalADCs /= nBins ;
        if (fIsHitMaxOrTotalADCs == "signalPeak") fHitADCs = maxADCs ;
        else                                      fHitADCs = totalADCs ;
    }
    
    else {
        fHitADCs = charges ;
    }
}


//============================================================================================
void SRSHit::ComputePosition() {
    Float_t pitch = fPlaneSize / (NCH * fNbAPVsOnPlane) ;
    
    if ((fReadoutBoard == "UV_ANGLE") &&  (fDetectorType == "EICPROTO1") ) {
        //    pitch = 2 * pitch ;
        //    fStripPosition = -0.5 * (fPlaneSize - pitch) + ((fPlaneSize / ( (NCH / 2 )* fNbAPVsOnPlane) ) * fStripNo) ;
        pitch = 2 * fTrapezoidDetOuterRadius  / (NCH * fNbAPVsOnPlane) ;
        fStripPosition = -0.5 * (fTrapezoidDetOuterRadius - pitch) + ((fTrapezoidDetOuterRadius / ( (NCH / 2 )* fNbAPVsOnPlane) ) * fStripNo) ;
    }
    
    else if (fReadoutBoard == "PADPLANE") {
        Float_t padSizeX = fPadDetectorMap[0] ;
        Float_t padSizeY = fPadDetectorMap[1] ;
        Int_t nbPadX = (Int_t) (fPadDetectorMap[2])  ;
        Int_t nbPadY = (Int_t) (fPadDetectorMap[3]) ;
        Int_t nbConn = (Int_t) (fPadDetectorMap[4]) ;
        Float_t posX = ( (fPadNo % nbPadX ) + 0.5 ) * padSizeX ;
        Float_t posY = ( (Int_t) (fPadNo / nbPadX ) + 0.5 ) * padSizeY ;
        fPadPosition.push_back(posX) ;
        fPadPosition.push_back(posY) ;
        fPadPosition.resize(2) ;
        fStripPosition = fStripNo ;
    }
    
    else {
        fStripPosition =  - 0.5 * (fPlaneSize - pitch) + ((fPlaneSize / (NCH * fNbAPVsOnPlane)) * fStripNo) ;
    }
}

//============================================================================================
void SRSHit::SetStripNo(Int_t stripNo) {
    fAbsoluteStripNo = stripNo ;
    
    if ((fReadoutBoard == "UV_ANGLE") &&  (fDetectorType == "EICPROTO1") ) {
        Int_t apvIndexOnPlaneEICBOT =  0 ;
        if (fapvIndexOnPlane < 4 ) apvIndexOnPlaneEICBOT = fapvIndexOnPlane + 8 ;
        else                       apvIndexOnPlaneEICBOT = fapvIndexOnPlane - 4;
        
        if ((fAPVOrientation == 0) && ((fPlane.Contains("TOP"))  && (fapvIndexOnPlane > 3)) || ((fPlane.Contains("BOT"))  && ( apvIndexOnPlaneEICBOT < 8))) {
            stripNo = 63 - stripNo ;
        }
        
        if (fPlane.Contains("TOP")) fStripNo = stripNo + (64 * (fapvIndexOnPlane % fNbAPVsOnPlane)) ;
        else                        fStripNo = stripNo + (64 * (apvIndexOnPlaneEICBOT % fNbAPVsOnPlane)) ;
    }
    
    else if ( (fReadoutBoard == "1DSTRIPS") || (fReadoutBoard == "CARTESIAN") || (fReadoutBoard == "CMSGEM") ){
        
        if(fAPVOrientation == 0) stripNo = 127 - stripNo ;
        
        if (fapvIndexOnPlane > fNbAPVsOnPlane) fStripNo = -1000000 ;
        else fStripNo = stripNo + (NCH * (fapvIndexOnPlane % fNbAPVsOnPlane)) ;
        //    printf("SRSHit::ComputePosition(): => stripNo=%d, fStripNo = %d, fapvIndexOnPlane=%d, \n",stripNo, fStripNo, fapvIndexOnPlane) ;
    }
    
    else if (fReadoutBoard == "PADPLANE") {
        fStripNo = stripNo ;
    }
    
    else {
        if(fAPVOrientation == 0) stripNo = 127 - stripNo ;
    }
}

//============================================================================================
SRSHit::~SRSHit(){
    fTimeBinADCs.clear() ;
    fPadDetectorMap.clear() ;
    fPadPosition.clear() ;
}
