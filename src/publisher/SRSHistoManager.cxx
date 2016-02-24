//Author: Kondo GNANVO 18/08/2010

#define NTIMEBINS 30
#define NDATA 4000
#include "SRSHistoManager.h"

ClassImp(SRSHistoManager);

//============================================================================================
SRSHistoManager::SRSHistoManager(const char * histoCfgname, Int_t zeroSupCut, Int_t minClustSize, Int_t maxClustSize, Int_t maxClustMult) {
    fMapping = SRSMapping::GetInstance() ;
    
    fRunName = "toto";
    fRunType = "RAWDATA" ;
    
    fRunFilePrefix = "std_" ;
    fRunFileValue = "0" ;
    fAmoreAgentID = "0" ;
    
    fZeroSupCut = zeroSupCut;
    fMaxClustMult   = maxClustMult + 1;
    fMinClustSize   = minClustSize - 1 ;
    fMaxClustSize   = maxClustSize + 1;
    fClustSizeRange = fMaxClustSize - fMinClustSize + 1 ;
    
    fHistosRatioCut = 1 ;
    f1DHistNBin     = 21;
    f1DHistMinRange = 0 ;
    f1DHistMaxRange = 20 ;
    
    fNBin     = 256;
    fRangeMin = -0.5;
    fRangeMax = 255.5 ;
    
    fNBin2     = 14;
    fRangeMin2 = -0.5;
    fRangeMax2 = 13.5;
    
    fNBinMap2    = 256;
    fNBinMap     = 256;
    fRangeMinMap = -0.5 ;
    fRangeMaxMap = 255.5 ;
    fRangeMinMap2 = -0.5 ;
    fRangeMaxMap2 = 255.5 ;
    
    fNBinPlane     = 256;
    fRangeMinPlane = -51.2;
    fRangeMaxPlane =  51.2;
    
    fNBinTime     = 30;
    fRangeMinTime = -0.5;
    fRangeMaxTime = 29.5;
    
    fNBinSpectrum     = 100;
    fRangeMinSpectrum = -0.5;
    fRangeMaxSpectrum = 3999.5;
    
    fIsFirstEvent = kTRUE;
    
    fNtupleSizeX = 100.0 ;
    fNtupleSizeY = 100.0 ;
    fNtupleSizeZ = 400.0 ;
    
    ResetHistos() ;
    RefreshHistos() ;
    
    fEventNumber = 0 ;
    
    //Set initial values...(in case fit fails)
    fRChi2      = -100 ;
    fMean       = -100 ;
    fMeanError  = -100 ;
    fSigma      = -100 ;
    fSigmaError = -100 ;
    
    //  if ((fRunType== "PHYSICS") || (fRunType== "APVGAIN"))  ReadHistoCfgFile(histoCfgname) ;
    printf("SRSHistoManager::SRSHistoManager()") ;
    
}

//============================================================================================
SRSHistoManager::~SRSHistoManager(){
    
    ClearMaps();
    ClearMapOfHistos(fResidualHistos) ;
    ClearMapOfHistos(fPosCorrectionHistos) ;
    
    ClearMapOfVectors(fFittedHistos) ;
    ClearMapOfVectors(fHistoTypeToDetectorMap) ;
    DeleteHistos() ;
    DeleteROOTObjs() ;
}

//============================================================================================
void SRSHistoManager::SetRunParameters(TString amoreAgentId, TString runtype, TString runname, TString filePrefix, TString fileValue) {
    printf("SRSHistoManager::SRSHistoManager() => runtype=%s, amoreAgentId=%s,filePrefix=%s, fileValue=%s\n",runtype.Data(), amoreAgentId.Data(),filePrefix.Data(), fileValue.Data()) ;
    fAmoreAgentID = amoreAgentId ;
    fRunType = runtype ;
    fRunName = runname ;
    fRunFilePrefix = filePrefix ;
    fRunFileValue = fileValue ;
    printf("SRSHistoManager::SRSHistoManager() => runtype=%s, amoreAgentId=%s,filePrefix=%s, fileValue=%s\n",runtype.Data(), amoreAgentId.Data(),filePrefix.Data(), fileValue.Data()) ;
    
}

//====================================================================================================================
void SRSHistoManager::ClearMaps() {
    fObjTitle.clear() ;
    fVarType.clear() ;
    fObjType.clear() ;
    fDetector.clear() ;
    fDetectorPlane1.clear() ;
    fDetectorPlane2.clear() ;
    fAPVIDStr.clear() ;
    fNbOfEvents.clear() ;
    fFitParam1Str.clear() ;
    fFitParam2Str.clear() ;
    fFitFunction.clear() ;
    fDividedHistos.clear() ;
    fNameAPVIDMap.clear() ;
}

//====================================================================================================================
void SRSHistoManager::ClearMapOfVectors( map<TString, vector <TString> > mapOfVectors) {
    map<TString, vector <TString> >::const_iterator itr;
    for(itr = mapOfVectors.begin(); itr != mapOfVectors.end(); ++itr) {
        vector <TString > vect =  (*itr).second ;
        vect.clear() ;
    }
    mapOfVectors.clear() ;
}
//====================================================================================================================
void SRSHistoManager::ClearMapOfHistos( map<TString,  TH1* > mapOfHistos) {
    map<TString, TH1*>::const_iterator hist_itr;
    for(hist_itr = mapOfHistos.begin(); hist_itr != mapOfHistos.end(); ++hist_itr){
        TString hist = (*hist_itr).first ;
        delete mapOfHistos[hist];
    }
    mapOfHistos.clear() ;
}
//====================================================================================================================
void SRSHistoManager::DeleteHistos() {
    while (!fHist1dToReset.empty()) {
        TH1 * h = fHist1dToReset.back();
        delete h;
        fHist1dToReset.pop_back();
    }
    while (!fHist2dToReset.empty()){
        TH2 * h = fHist2dToReset.back();
        delete h;
        fHist2dToReset.pop_back();
    }
}

//====================================================================================================================
void SRSHistoManager::DeleteROOTObjs() {
    map<TString, TObject*>::const_iterator itr ;
    for (itr = fObjToDraw.begin(); itr != fObjToDraw.end(); ++itr) {
        TNamed * obj = (TNamed*) (*itr).second ;
        delete obj ;
        printf("SRSHistoManager::(DeleteROOTObjs) == delete all ROOTs objects \n") ;
    }
}

//====================================================================================================================
void SRSHistoManager::ResetHistos() {
    map<TString,TObject*>::const_iterator itr;
    for (itr = fObjToDraw.begin(); itr != fObjToDraw.end(); ++itr){
        TString objName = (*itr).first;
        if (fObjType[objName] == "1D") {
            ((TH1*)fObjToDraw[objName])->Reset();
        }
        else if (fObjType[objName] == "2D") {
            ((TH2*)fObjToDraw[objName])->Reset();
        }
        else if (fObjType[objName] == "Ntuple") {
            ((TNtuple*)fObjToDraw[objName])->Reset();
        }
        else Error("SRSHistoManager","ResetHistos:=> Wrong histo name: %s", objName.Data()) ;
    }
    fIsFirstEvent = kTRUE;
}

//====================================================================================================================
void SRSHistoManager::RefreshHistos() {
    while (!fHist1dToReset.empty()) {
        TH1 * h = fHist1dToReset.back();
        h->Reset();
        fHist1dToReset.pop_back();
    }
    while (!fHist2dToReset.empty()){
        TH2 * h = fHist2dToReset.back();
        h->Reset();
        fHist2dToReset.pop_back();
    }
}

//============================================================================================
void SRSHistoManager::Reset2DHistBinning(TString type, Int_t nbin, Float_t min, Float_t max, Int_t nbin2, Float_t min2, Float_t max2) {
    //    printf("  SRSHistoManager::Reset2DHistBinning()=> type=%s, nbin=%d, min=%f, max=%f, nbin=%d, min2=%f, max2=%f \n",type.Data(), nbin, min, max, nbin2, min2, max2) ;
    if (type == "CLUSTCOR" )  {
        f1DHistNBin     = nbin ;
        f1DHistMinRange = min ;
        f1DHistMaxRange = max ;
        fNBinSpectrum     = nbin2 ;
        fRangeMinSpectrum = min2 ;
        fRangeMaxSpectrum = max2 ;
    }
    
    else if (type == "CHARGES_RATIODIST")  {
        fNBinMap = nbin ;
        fNBinMap2= nbin2 ;
    }
    
    else if ( (type == "HITMAP") || (type == "CORRELATION")){
        fNBinMap      = nbin ;
        fNBinMap2     = nbin2 ;
        fRangeMinMap  = min ;
        fRangeMaxMap  = max ;
        fRangeMinMap2 = min2 ;
        fRangeMaxMap2 = max2 ;
        //    printf("  SRSHistoManager::Reset2DHistBinning()=> type=%s,fNBin =%d, fRangeMin=%f, fRangeMax=%f, fNBin2=%d, fRangeMin2=%f, fRangeMax2=%f \n",type.Data(), fNBinMap, fRangeMinMap, fRangeMaxMap, fNBinMap2, fRangeMinMap2, fRangeMaxMap2) ;
    }
    else Error("SRSHistoManager", "ResetBinning:=> Unknown type %s", type.Data());
}

//============================================================================================
void SRSHistoManager::Reset1DHistBinning(TString type, Int_t nbin, Float_t min, Float_t max){
    //  printf("  SRSHistoManager::ResetBinning()=> type=%s\n",type.Data()) ;
    if ((type == "ADCDIST") || (type == "CHARGES_SH") || (type == "CHARGES_RATIO") || (type == "SPECTRUM") ) {
        fNBinSpectrum     = nbin ;
        fRangeMinSpectrum = min ;
        fRangeMaxSpectrum = max ;
    }
    
    else if ((type == "CLUSTERDIST")|| (type == "RELATIVE_OFFSET")) {
        fNBinMap      = nbin ;
        fRangeMinMap  = min ;
        fRangeMaxMap  =  max ;
    }
    
    else if ( (type == "PLANETIMEBIN") || (type == "CLUSTERTIMEBIN") ){
        fNBinTime     = nbin ;
        fRangeMinTime = min ;
        fRangeMaxTime = max ;
    }
    
    else if ((type == "TIMESLICE") || (type == "OCCUPANCY") || (type == "CHARGECROSSTALK") || (type == "CLUSTSTAT") || (type == "NOISERESO")) {
        f1DHistNBin     = nbin ;
        f1DHistMinRange = min ;
        f1DHistMaxRange = max ;
    }
    
    else Error("SRSHistoManager", "ResetBinning:=> Unknown type %s", type.Data());
}

//============================================================================================
void SRSHistoManager::SetBinning(TString type, TString detPlane1, TString detPlane2) {
    
    if ((type == "ADCDIST") || (type == "CHARGES_SH") || (type == "CHARGES_RATIO") || (type == "SPECTRUM")) {
        fNBin      = fNBinSpectrum ;
        fRangeMin  = fRangeMinSpectrum ;
        fRangeMax  = fRangeMaxSpectrum ;
        fNBin2     = fNBinSpectrum ;
        fRangeMin2 = fRangeMinSpectrum ;
        fRangeMax2 = fRangeMaxSpectrum ;
    }
    
    else if ((type == "HITDIST") || (type == "HITZEROSUP") || (type == "HITPEDOFFSET") || (type == "HITCOMMODE") || (type == "HITRAWDATA")) {
        fNBin      =  NCH * fMapping->GetNbOfAPVs(detPlane1)  ;
        fRangeMin  = -0.5 * fMapping->GetSizeOfPlane(detPlane1) ;
        fRangeMax  =  0.5 * fMapping->GetSizeOfPlane(detPlane1)   ;
        //    printf("  SRSHistoManager::SetBinning=> detPlane=%s, fNBin=%d, fRangeMin=%f, fRangeMax=%f \n", detPlane1.Data(), fNBin, fRangeMin, fRangeMax) ;
    }
    
    else if ( (type == "OLD_OFFSETX") || (type == "OLD_OFFSETY")) {
        fNBin     =  201 ;
        fRangeMin = -20 ;
        fRangeMax =  20 ;
    }
    else if (type == "PEDESTALS") {
        fNBin     = 128 ;
        fRangeMin = -0.5 ;
        fRangeMax = 127.5 ;
    }
    
    else if (type == "CHARGES_RATIODIST") {
        fNBin       =  fNBinMap ;
        fNBin2      =  fNBinMap ;
        fRangeMin  = -0.5 * fMapping->GetSizeOfPlane(detPlane1) ;
        fRangeMax  =  0.5 * fMapping->GetSizeOfPlane(detPlane1) ;
        fRangeMin2  = -0.5 * fMapping->GetSizeOfPlane(detPlane2) ;
        fRangeMax2  =  0.5 * fMapping->GetSizeOfPlane(detPlane2) ;
    }
    
    else if ( (type == "HITMAP") || (type == "CORRELATION") ) {
        if (fNBinMap == 99099)        fNBin = 256 ;
        else                          fNBin = fNBinMap ;
        
        if (fNBinMap2 == 99099)       fNBin2 = 256 ;
        else                          fNBin2 = fNBinMap2 ;
        
        if (fRangeMinMap == 99099.0)  fRangeMin = -0.5 * fMapping->GetSizeOfPlane(detPlane1) ;
        else                          fRangeMin = fRangeMinMap ;
        
        if (fRangeMinMap2 == 99099.0) fRangeMin2 = -0.5 * fMapping->GetSizeOfPlane(detPlane2) ;
        else                          fRangeMin2 = fRangeMinMap2 ;
        
        if (fRangeMaxMap == 99099.0)  fRangeMax = 0.5 * fMapping->GetSizeOfPlane(detPlane1) ;
        else                          fRangeMax = fRangeMaxMap ;
        if (fRangeMaxMap2 == 99099.0) fRangeMax2 = 0.5 * fMapping->GetSizeOfPlane(detPlane2) ;
        else                          fRangeMax2 = fRangeMaxMap2 ;
        //    printf("  SRSHistoManager::SetBinning => detPlane=%s, fNBin=%d, fRangeMin=%f, fRangeMax=%f, detPlane=%s, fNBin2=%d, fRangeMin2=%f, fRangeMax2=%f \n", detPlane1.Data(), fNBin, fRangeMin, fRangeMax, detPlane2.Data(), fNBin2, fRangeMin2, fRangeMax2) ;
    }
    
    
    else if (type == "CMSHITMAP") {
        fNBin = 8 ;
        fRangeMin = 0 ;
        fRangeMax = 1000 ;
        
        fNBin = 250 ;
        fRangeMin = 0 ;
        fRangeMax = 500 ;
        //    printf("  SRSHistoManager::SetBinning => detPlane=%s, fNBin=%d, fRangeMin=%f, fRangeMax=%f, detPlane=%s, fNBin2=%d, fRangeMin2=%f, fRangeMax2=%f \n", detPlane1.Data(), fNBin, fRangeMin, fRangeMax, detPlane2.Data(), fNBin2, fRangeMin2, fRangeMax2) ;
    }
    
    else if ((type == "CLUSTERDIST")|| (type == "RELATIVE_OFFSET")) {
        if (fNBinMap == 99099)        fNBin = NCH *  fMapping->GetNbOfAPVs(detPlane1) ;
        else                          fNBin = fNBinMap ;
        if (fRangeMaxMap == 99099.0)  fRangeMax = 0.5 * fMapping->GetSizeOfPlane(detPlane1) ;
        else                          fRangeMax = fRangeMaxMap ;
        if (fRangeMinMap == 99099.0)  fRangeMin = -0.5 * fMapping->GetSizeOfPlane(detPlane1) ;
        else                          fRangeMin = fRangeMinMap ;
        //    printf("  SRSHistoManager::SetBinning => detPlane=%s, fNBin=%d, fRangeMin=%f, fRangeMax=%f \n", detPlane1.Data(), fNBin, fRangeMin, fRangeMax) ;
    }
    
    else if (type == "HITCROSSTALK") {
        fNBin      = 128;
        fRangeMin  = -0.5 ;
        fRangeMax  = 127.5 ;
    }
    
    else if (type == "CLUSTSIZE") {
        fNBin     = fClustSizeRange;
        fRangeMin = fMinClustSize;
        fRangeMax = fMaxClustSize ;
    }
    
    else if (type == "CLUSTMULT") {
        fNBin     = fMaxClustMult + 1 ;
        fRangeMin = 0;
        fRangeMax = fMaxClustMult ;
    }
    
    else if ((type == "TIMESLICE") || (type == "OCCUPANCY") || (type == "CHARGECROSSTALK") || (type == "CLUSTSTAT") || (type == "NOISERESO")) {
        fNBin     = f1DHistNBin ;
        fRangeMin = f1DHistMinRange ;
        fRangeMax = f1DHistMaxRange ;
    }
    
    else if (type == "CLUSTCOR")  {
        fNBin2     = f1DHistNBin ;
        fRangeMin2 = f1DHistMinRange ;
        fRangeMax2 = f1DHistMaxRange ;
        
        fNBin     = 64 ;
        fRangeMin = -0.5 ;
        fRangeMax = 4095.5 ;
    }
    
    else if (type == "PLANETIMEBIN") {
        fNBin2     = fNBinTime  ;
        fRangeMin2 = fRangeMinTime ;
        fRangeMax2 = fRangeMaxTime ;
        
        fNBin     =  NCH  * fMapping->GetNbOfAPVs(detPlane1)  + 1 ;
        fRangeMin = -0.5 * fMapping->GetSizeOfPlane(detPlane1) ;
        fRangeMax =  0.5 * fMapping->GetSizeOfPlane(detPlane1) ;
    }
    
    else if (type == "CLUSTERTIMEBIN") {
        fNBin      = fNBinTime  ;
        fRangeMin  = fRangeMinTime ;
        fRangeMax  = fRangeMaxTime ;
    }
    else Error("   SRSHistoManager","SetBinning:=> Unknown type %s", type.Data());
}

//=========================================================================================================================
void SRSHistoManager::Book1DHisto(TString histoName, TString title, TString type, TString detPlane1, TString detPlane2){
    SetBinning(type, detPlane1, detPlane2) ;
    TH1F * h = new TH1F(histoName, title, fNBin, fRangeMin, fRangeMax);
    fObjToDraw[histoName]      = h;
    fDetectorPlane1[histoName] = detPlane1;
    fDetectorPlane2[histoName] = detPlane2;
    fObjType[histoName]        = "1D";
    fVarType[histoName]        = type;
    fObjTitle[histoName]       = title;
    fNbOfEvents[histoName]     = 0;
    fHistoTypeToDetectorMap[detPlane1].push_back(histoName) ;
    fHistoTypeToDetectorMap[detPlane2].push_back(histoName) ;
    printf("  SRSHistoManager::Book1DHisto() ==> histo = %s booked \n",histoName.Data()) ;
}

//=========================================================================================================
void SRSHistoManager::Book1DHisto(TString histoName, TString title, TString type, TString detPlane){
    SetBinning(type, detPlane, detPlane) ;
    TH1F * h = new TH1F(histoName, title, fNBin, fRangeMin, fRangeMax) ;
    fObjToDraw[histoName]      = h;
    fDetector[histoName]       = detPlane;
    fDetectorPlane1[histoName] = detPlane;
    fObjType[histoName]        = "1D";
    fVarType[histoName]        = type;
    fObjTitle[histoName]       = title;
    fNbOfEvents[histoName]     = 0;
    fHistoTypeToDetectorMap[detPlane].push_back(histoName) ;
    printf("  SRSHistoManager::Book1DHisto()=>histo=%s, plane1=%s, nBin=%d, rangeMin=%f, rangeMax=%f \n", histoName.Data(), detPlane.Data(), fNBin, fRangeMin, fRangeMax) ;
}

//====================================================================================================================
void SRSHistoManager::Book2DHisto(TString histoName, TString title, TString type, TString detector) {
    SetBinning(type, detector, detector) ;
    TH2F * h  = new TH2F(histoName, title, fNBin, fRangeMin, fRangeMax, fNBin2, fRangeMin2, fRangeMax2);
    fObjToDraw[histoName]  = h;
    fDetector[histoName]   = detector;
    fObjType[histoName]    = "2D";
    fVarType[histoName]    = type;
    fObjTitle[histoName]   = title;
    fNbOfEvents[histoName] = 0;
    fHistoTypeToDetectorMap[detector].push_back(histoName) ;
    printf("  SRSHistoManager::Book2DHisto()=> fNBin=%d, fRangeMin=%f, fRangeMax=%f, fNBin=%d, fRangeMin2=%f, fRangeMax2=%f \n",fNBin, fRangeMin, fRangeMax,fNBin2, fRangeMin2, fRangeMax2) ;
}

//====================================================================================================================
void SRSHistoManager::Book2DHisto(TString histoName, TString title, TString type, TString detPlane1, TString detPlane2) {
    SetBinning(type, detPlane1, detPlane2) ;
    TH2F * h  = new TH2F(histoName, title, fNBin, fRangeMin, fRangeMax, fNBin2, fRangeMin2, fRangeMax2) ;
    fObjToDraw[histoName]      = h;
    fDetectorPlane1[histoName] = detPlane1;
    fDetectorPlane2[histoName] = detPlane2;
    fObjType[histoName]        = "2D";
    fVarType[histoName]        = type;
    fObjTitle[histoName]       = title;
    fNbOfEvents[histoName]     = 0;
    fHistoTypeToDetectorMap[detPlane1].push_back(histoName) ;
    fHistoTypeToDetectorMap[detPlane2].push_back(histoName) ;
    printf("  SRSHistoManager::Book2DHisto()=> fNBinMap=%d, fRangeMinMap=%f, fRangeMaxMap=%f, fNBinMap2=%d, fRangeMinMap2=%f, fRangeMaxMap2=%f \n",fNBinMap, fRangeMinMap, fRangeMaxMap,fNBinMap2, fRangeMinMap2, fRangeMaxMap2) ;
}

//====================================================================================================================
void SRSHistoManager::BookCMSHisto(TString histoName, TString title, TString type) {
    Int_t nbOfAPVs = fMapping->GetNbOfAPVs() ;
    Int_t nbAPVBins = nbOfAPVs;
    Int_t nbBins = 128 * nbOfAPVs ;
    TH2F * h  = new TH2F(histoName, title, 8, 0, 1000, nbBins, -250 , 250);
    fObjToDraw[histoName]     = h;
    fObjType[histoName]       = "2D";
    fVarType[histoName]       = type;
    fObjTitle[histoName]      = title;
    fNbOfEvents[histoName]    = 0;
    printf("  SRSHistoManager::Book2DHisto() ==> histo = %s booked \n",histoName.Data()) ;
}


//=========================================================================================================================
void SRSHistoManager::BookHitTimingHisto(){
    if (fRunType == "HITTIMING") {
        for (Int_t i =0; i < 20; i++) {
            stringstream hitno ;
            hitno << i ;
            TString  hitNoStr = hitno.str() ;
            TString histoName = "timingHitNo_" + i;
            TString title = histoName ;
            TString type = "HITTIMING" ;
            TH1F * h = new TH1F(histoName, title, 30, 0, 29) ;
            fObjToDraw[histoName]  = h ;
            fObjType[histoName]    = "1D" ;
            fVarType[histoName]    = type ;
            fObjTitle[histoName]   = title ;
            fNbOfEvents[histoName] = 0 ;
            printf("  SRSHistoManager::BookHitTimingHisto() ==> histo = %s booked \n",histoName.Data() ) ;
        }
    }
}

//====================================================================================================================
void SRSHistoManager::PedestalRun( SRSPedestal * ped) {
    map<TString,TObject*>::const_iterator objToDraw_itr ;
    for (objToDraw_itr = fObjToDraw.begin(); objToDraw_itr != fObjToDraw.end(); ++objToDraw_itr) {
        TString histoName = (*objToDraw_itr).first;
        if (!fObjToDraw[histoName]) continue;
        fHist1dToReset.push_back((TH1F*)(fObjToDraw[histoName])) ;
        Int_t apvID = fAPVIDStr[histoName] ;
        if (fVarType[histoName] == "PEDESTALS") {
            for(Int_t chNo = 0; chNo < NCH; ++chNo) {
                Float_t rms  = ped->GetOnlinePedestalRMS(apvID, chNo) ;
                Float_t mean = ped->GetOnlinePedestalMean(apvID, chNo) ;
                int stripNo = chNo ;
                if (histoName.Contains("Mean")) ((TH1*) fObjToDraw[histoName])->Fill(stripNo, mean) ;
                if (histoName.Contains("RMS"))  ((TH1*) fObjToDraw[histoName])->Fill(stripNo, rms) ;
            }
        }
    }
}

//====================================================================================================================
void SRSHistoManager::RawPedestalRun(SRSRawPedestal * rawped) {
    //  printf("\nSRSHistoManager::SRSRawPedestalRun() \n") ;
    map<TString,TObject*>::const_iterator objToDraw_itr ;
    for (objToDraw_itr = fObjToDraw.begin(); objToDraw_itr != fObjToDraw.end(); ++objToDraw_itr) {
        TString histoName = (*objToDraw_itr).first;
        if (!fObjToDraw[histoName]) continue;
        fHist1dToReset.push_back((TH1F*)(fObjToDraw[histoName])) ;
        Int_t apvID = fAPVIDStr[histoName] ;
        if (fVarType[histoName] == "PEDESTALS") {
            for(Int_t chNo = 0; chNo < NCH; ++chNo) {
                Float_t rms  = rawped->GetOnlinePedestalRMS(apvID, chNo) ;
                Float_t mean = rawped->GetOnlinePedestalMean(apvID, chNo) ;
                int stripNo = chNo ;
                if (histoName.Contains("Mean")) ((TH1*) fObjToDraw[histoName])->Fill(stripNo, mean) ;
                if (histoName.Contains("RMS"))  ((TH1*) fObjToDraw[histoName])->Fill(stripNo, rms) ;
            }
        }
    }
}

//=========================================================================================================================
//
//     RAW DATA
//
//=========================================================================================================================
void SRSHistoManager::BookRawDataHisto() {
    printf("  SRSHistoManager::BookRawDataHisto() ==> ENTER \n") ;
    
    if (fRunType == "RAWDATA") {
        map <Int_t, TString> listOfAPVs = fMapping->GetAPVFromIDMap() ;
        map <Int_t, TString>::const_iterator apv_itr ;
        printf("  SRSHistoManager::BookRawDataHisto() ==> \n") ;
        for (apv_itr = listOfAPVs.begin(); apv_itr != listOfAPVs.end(); ++ apv_itr) {
            Int_t apvID = (*apv_itr).first;
            TString histoName = fMapping->GetAPVFromID(apvID) ;
            TString title = histoName ;
            TString type = "RAWDATA" ;
            TH1F * h = new TH1F(histoName, title, 2000, 0, 1999) ;
            fAPVIDStr[histoName] = apvID ;
            fObjToDraw[histoName]  = h ;
            fObjType[histoName]    = "1D" ;
            fVarType[histoName]    = type ;
            fObjTitle[histoName]   = title ;
            fNbOfEvents[histoName] = 0 ;
            printf("  SRSHistoManager::BookRawDataHisto() ==> histo = %s booked \n",histoName.Data() ) ;
        }
        
        map <TString, list <Int_t> > apvListFromPlaneMap = fMapping->GetAPVIDListFromDetectorPlaneMap() ;
        map <TString, list <Int_t> >::const_iterator plane_itr ;
        for (plane_itr = apvListFromPlaneMap.begin(); plane_itr != apvListFromPlaneMap.end(); ++ plane_itr) {
            TString histoName = (*plane_itr).first ;
            TString title =  histoName;
            TString type = "RAWDATAPLANE" ;
            TH1F * h = new TH1F(histoName, title, 2000, 0, 1999) ;
            fAPVIDStr[histoName]  =  0;
            fObjToDraw[histoName]  = h ;
            fObjType[histoName]    = "1D" ;
            fVarType[histoName]    = type ;
            fObjTitle[histoName]   = title ;
            fNbOfEvents[histoName] = 0 ;
            printf("  SRSHistoManager::BookRawDataHisto() ==> histo = %s booked \n",histoName.Data() ) ;
        }
        
    }
}
//=========================================================================================================================
void SRSHistoManager::FillRawDataHistos(SRSEventBuilder * eventbuilder) {
    
    Int_t triggerCount = eventbuilder->GetTriggerCount() ;
    Int_t rawdata_size = 4000 ;
    
    //  printf("  SRSHistoManager::FillRawDataHistos() ==> triggerCount = %d \n", triggerCount ) ;
    map<TString,TObject*>::const_iterator objToDraw_itr ;
    
    for (objToDraw_itr = fObjToDraw.begin(); objToDraw_itr != fObjToDraw.end(); ++objToDraw_itr) {
        TString histoName = (*objToDraw_itr).first;
        
        if (!fObjToDraw[histoName]) continue;
        //    printf("  SRSHistoManager::FillRawDataHistos() ==> vartype = %s\n", fVarType[histoName].Data() ) ;
        
        if (fVarType[histoName] == "RAWDATA") {
            fHist1dToReset.push_back((TH1F*)(fObjToDraw[histoName])) ;
            Int_t apvID = fAPVIDStr[histoName] ;
            
            if(!fMapping->IsAPVIDMapped(apvID)) {
                Warning("FillHistos","=> APVID=%d not connected, skip histo %s\n", apvID, histoName.Data()) ;
                continue ;
            }
            
            SRSAPVEvent * apvEvent = eventbuilder->GetAPVEventFromAPVKey(fMapping->GetAPVNoFromID(apvID)) ;
            apvEvent->ComputeRawData16bits() ;
            vector <UInt_t> rawdata16bitsVect = apvEvent->GetRawData16bits() ;
            rawdata_size = ((Int_t) (rawdata16bitsVect.size())) ;
            ((TH1*) fObjToDraw[histoName])->SetBins(rawdata_size, 0, rawdata_size) ;
            vector <UInt_t>::const_iterator  rawdata16_itr ;
            
            Int_t stripNo = 0 ;
            Int_t apvEventNb = apvEvent->GetEventNumber() ;
            stringstream out ;
            out << apvEventNb ;
            TString apvEventNbStr = out.str() ;
            TString newTitle = fObjTitle[histoName] ;
            
            ((TH1*) fObjToDraw[histoName])->SetTitle(newTitle) ;
            for(rawdata16_itr = rawdata16bitsVect.begin(); rawdata16_itr != rawdata16bitsVect.end(); rawdata16_itr++ ) {
                UInt_t rawdata16  =  * rawdata16_itr ;
                ((TH1*) fObjToDraw[histoName])->Fill(stripNo, rawdata16) ;
                //	printf("  SRSHistoManager::FillRawDataHistos() ==> apvID = %d, StripNo = %d, rawdata = %d\n", apvID, stripNo, rawdata16 ) ;
                stripNo++ ;
            }
        }
        
        else if (fVarType[histoName] == "RAWDATAPLANE") {
            fHist1dToReset.push_back((TH1F*)(fObjToDraw[histoName])) ;
            //      printf("  SRSHistoManager::FillRawDataHistos() ==> histoName = %s\n", histoName.Data() ) ;
            Int_t dataStripNb = 0;
            Int_t size = rawdata_size * (fMapping->GetNbOfAPVs(histoName)) ;
            ((TH1*) fObjToDraw[histoName])->SetBins(size, 0, size) ;
            
            list <Int_t> apvIDList = fMapping->GetAPVIDListFromDetectorPlane(histoName) ;
            list <Int_t>::const_iterator  apv_itr ;
            for (apv_itr = apvIDList.begin(); apv_itr != apvIDList.end(); ++ apv_itr) {
                Int_t apvID = * apv_itr ;
                SRSAPVEvent * apvEvent = eventbuilder->GetAPVEventFromAPVKey(fMapping->GetAPVNoFromID(apvID)) ;
                apvEvent->ComputeRawData16bits() ;
                vector <UInt_t> rawdata16bitsVect = apvEvent->GetRawData16bits() ;
                vector <UInt_t>::const_iterator  rawdata16_itr ;
                Int_t stripNo = 0 ;
                Int_t apvEventNb = apvEvent->GetEventNumber() ;
                stringstream out ;
                out << apvEventNb ;
                TString apvEventNbStr = out.str() ;
                TString newTitle = fObjTitle[histoName] ;
                ((TH1*) fObjToDraw[histoName])->SetTitle(newTitle) ;
                for(rawdata16_itr = rawdata16bitsVect.begin(); rawdata16_itr != rawdata16bitsVect.end(); rawdata16_itr++ ) {
                    UInt_t rawdata16  =  * rawdata16_itr ;
                    ((TH1*) fObjToDraw[histoName])->Fill(dataStripNb, rawdata16) ;
                    //	  printf("  SRSHistoManager::FillRawDataHistos() ==> histoName = %s, apvID = %d, StripNo = %d, rawdata = %d\n", histoName.Data(), apvID, stripNo, rawdata16 ) ;
                    dataStripNb++ ;
                }
                //	dataStripNb++ ;
            }
        }
        else {
            continue ;
        }
    }
}

//====================================================================================================================
//
//        APV GAIN
//
//====================================================================================================================
void SRSHistoManager::BookAPVGainHisto() {
    Int_t nbOfAPVs = fMapping->GetNbOfAPVs() ;
    Int_t nbAPVBins = nbOfAPVs;
    Int_t nbBins = 128 * nbOfAPVs ;
    Int_t binMax = nbBins - 1 ;
    
    TH1F * apvGainNormHistos = new TH1F("allAPVGainNorm", "all APV Gain Norm", 400, 0, 2048);
    TH1F * apvHitsHistos = new TH1F("allAPVGainHits", "all APV Gain Hits", nbOfAPVs, 0, nbAPVBins);
    TH1F * apvGainHistos = new TH1F("allAPVGain", "all APV Gain", nbOfAPVs, 0, nbAPVBins );
    
    fObjToDraw["allAPVGainHits"] = apvHitsHistos;
    fObjToDraw["allAPVGain"]     = apvGainHistos;
    fObjToDraw["allAPVGainNorm"] = apvGainNormHistos;
    
    fObjType["allAPVGainHits"] = "1D";
    fObjType["allAPVGain"] = "1D";
    fObjType["allAPVGainNorm"] = "1D";
    printf("  SRSHistoManager::BookAPVGainHisto ==> histos \n") ;
}

//====================================================================================================================
void SRSHistoManager::SaveAPV25GainHistos() {
    TString temp1 = "~/results/";
    temp1.Append(fRunType);
    temp1.Append("_");
    temp1.Append(fRunName);
    
    TString fileName = temp1.Append(".root");
    TFile *file = new TFile(fileName,"recreate");
    
    if (!file->IsOpen()){
        Error("SRSHistoManager","SaveAPV25GainHistos:=> Cannot open file %s", fileName.Data());
    }
    //=== Create Folder
    file->mkdir("apv25GainDir");
    
    ((TH1F*) fObjToDraw["allAPVGain"])->Divide((((TH1F*) fObjToDraw["allAPVGainHits"]))) ;
    ((TH1F*) fObjToDraw["allAPVGain"])->Scale( 1 / ((TH1F*) fObjToDraw["allAPVGainNorm"])->GetMean() );
    
    file->cd() ;
    gDirectory->cd("apv25GainDir") ;
    
    SaveRunHistos(fObjToDraw["allAPVGainHits"],kTRUE) ;
    SaveRunHistos(fObjToDraw["allAPVGain"], kTRUE) ;
    SaveRunHistos(fObjToDraw["allAPVGainNorm"], kTRUE) ;
    
    fObjToDraw["allAPVGainHits"]->Write() ;
    fObjToDraw["allAPVGain"]->Write() ;
}

//====================================================================================================================
void SRSHistoManager::FillAPV25GainHistos(SRSEventBuilder * eventbuilder) {
    //  printf("\nSRSHistoManager::FillAPV25GainHistos(): => entering the FillAPV25GainHistos \n") ;
    Int_t triggerCount   = eventbuilder->GetTriggerCount() ;
    
    map < TString, list <SRSHit * > >  hitsInDetectorPlaneMap ;
    hitsInDetectorPlaneMap = eventbuilder->GetHitsInDetectorPlane() ;
    
    // START FILLING HISTOGRAMS
    map<TString,TObject*>::const_iterator objToDraw_itr ;
    for (objToDraw_itr = fObjToDraw.begin(); objToDraw_itr != fObjToDraw.end(); ++objToDraw_itr) {
        TString histoName = (*objToDraw_itr).first;
        
        if (!fObjToDraw[histoName]) continue;
        fNbOfEvents[histoName] += 1 ;
        
        if (fVarType[histoName] == "HITDIST") {
            
            ((TH1*) fObjToDraw[histoName])->SetMarkerSize(0.5) ;
            ((TH1*) fObjToDraw[histoName])->SetMarkerStyle(21) ;
            
            list <SRSHit * >  listOfHits = hitsInDetectorPlaneMap[fDetectorPlane1[histoName].Data()] ;
            Int_t nbOfHits = listOfHits.size() ;
            if(nbOfHits == 0) {
                listOfHits.clear() ;
                continue ;
            }
            
            list <SRSHit * >::const_iterator hit_itr ;
            for(hit_itr = listOfHits.begin(); hit_itr != listOfHits.end(); ++hit_itr ) {
                
                SRSHit * hit = * hit_itr ;
                Int_t apvID = hit->GetAPVID() ;
                Int_t apvNo = fMapping->GetAPVNoFromID(apvID)  ;
                Int_t absoluteStripNb = hit->GetAbsoluteStripNo() ;
                Float_t charges = hit->GetHitADCs()  ;
                Int_t stripNb = (128 * apvNo) + absoluteStripNb ;
                
                ((TH1F*) fObjToDraw["allAPVGainHits"])->Fill(apvNo,1) ;
                ((TH1F*) fObjToDraw["allAPVGain"])->Fill(apvNo, charges) ;
                ((TH1F*) fObjToDraw["allAPVGainNorm"])->Fill(charges) ;
            }
            listOfHits.clear() ;
        }
    }
}

//====================================================================================================================
//
//        RESIDUALS
//
//====================================================================================================================
void SRSHistoManager::BookResidualsHisto(SRSTrack * track) {
    map <TString, TString > detectorList =  track->GetDetectorList() ;
    map <TString, TString >::const_iterator det_itr ;
    for(det_itr = detectorList.begin(); det_itr != detectorList.end(); ++det_itr) {
        TString detName = (*det_itr).first ;
        
        Int_t   xnbin = track->GetXNBinResiduals(detName) ;
        Float_t xmin  = track->GetXRangeMinResiduals(detName) ;
        Float_t xmax  = track->GetXRangeMaxResiduals(detName) ;
        Int_t   ynbin = track->GetYNBinResiduals(detName) ;
        Float_t ymin  = track->GetYRangeMinResiduals(detName) ;
        Float_t ymax  = track->GetYRangeMaxResiduals(detName) ;
        
        Int_t   rnbin = track->GetRNBinResiduals(detName) ;
        Float_t rmin  = track->GetRRangeMinResiduals(detName) ;
        Float_t rmax  = track->GetRRangeMaxResiduals(detName) ;
        Int_t   phinbin = track->GetPHINBinResiduals(detName) ;
        Float_t phimin  = track->GetPHIRangeMinResiduals(detName) ;
        Float_t phimax  = track->GetPHIRangeMaxResiduals(detName) ;
        
        TString planeX = (fMapping->GetDetectorPlaneListFromDetector(detName)).front() ;
        TString planeY = (fMapping->GetDetectorPlaneListFromDetector(detName)).back() ;
        
        TString xresName = "residuals" + planeX ;
        TString yresName = "residuals" + planeY ;
        TString xresTitle = planeX + " Residuals" ;
        TString yresTitle = planeY + " Residuals" ;
        
        if(detName == "EIC1") {
            xresName = "residuals" + detName + "X"  ;
            yresName = "residuals" + detName + "Y"  ;
            xresTitle = detName  + "X Residuals" ;
            yresTitle = detName  + "Y Residuals" ;
            
            TString topresName = "residuals" + planeX ;
            TString botresName = "residuals" + planeY ;
            TString topresTitle = planeX + " Residuals" ;
            TString botresTitle = planeY + " Residuals" ;
            
            TH1F * hTopY  = new TH1F(topresName, topresTitle, ynbin, ymin, ymax) ;
            TH1F * hBotY  = new TH1F(botresName, botresTitle, ynbin, ymin, ymax) ;
            
            hTopY->SetYTitle("Counts") ;
            hBotY->SetYTitle("Counts") ;
            fResidualHistos[topresName] = hTopY  ;
            fResidualHistos[botresName] = hBotY ;
            printf("  SRSHistoManager::BookResidualsHisto()=>residual histo Top strips = %s \n", topresName.Data()) ;
            printf("  SRSHistoManager::BookResidualsHisto()=>residual histo Bottom Strip = %s \n", botresName.Data()) ;
            
            TString rname    = "residuals" +  detName + "R" ;
            TString phiname  = "residuals" +  detName + "PHI" ;
            TString rtitle   = rname + " Residuals" ;
            TString phititle = phiname + " Residuals" ;
            TH1F * hR    = new TH1F(rname, rtitle, rnbin, rmin, rmax) ;
            TH1F * hPhi  = new TH1F(phiname, phititle, phinbin, phimin, phimax) ;
            
            hR->SetYTitle("Counts") ;
            hPhi->SetYTitle("Counts") ;
            fResidualHistos[rname] = hR  ;
            fResidualHistos[phiname] = hPhi ;
            printf("  SRSHistoManager::BookResidualsHisto()=>residual histo R = %s \n", rname.Data()) ;
            printf("  SRSHistoManager::BookResidualsHisto()=>residual histo phi = %s \n", phiname.Data()) ;
        }
        
        TH1F * hX  = new TH1F(xresName, xresTitle, xnbin, xmin, xmax) ;
        TH1F * hY  = new TH1F(yresName, yresTitle, ynbin, ymin, ymax) ;
        
        hX->SetYTitle("Counts") ;
        hY->SetYTitle("Counts") ;
        
        fResidualHistos[xresName] = hX ;
        fResidualHistos[yresName] = hY ;
        
        TString xyRotationName  = "rotation" + detName ;
        TString xyRotationTitle = "rotation" + detName ;
        TH1F * hXYRotation = new TH1F(xyRotationName, xyRotationTitle, 100, -1., 1.0) ;
        hXYRotation->SetYTitle("Counts") ;
        fResidualHistos[xyRotationName] = hXYRotation ;
        printf("  SRSHistoManager::BookResidualsHisto()=>residual histo = %s \n", xyRotationName.Data()) ;
        
        
        Int_t offsetNbBin = 512;
        Float_t xoffsetBinMin = -51.2;
        Float_t yoffsetBinMin = -51.2;
        Float_t xoffsetBinMax = 51.2;
        Float_t yoffsetBinMax = 51.2;
        
        if(detName.Contains("SBS")) {
            offsetNbBin = 3560;
            xoffsetBinMin = -356;
            yoffsetBinMin = -356;
            xoffsetBinMax = 356;
            yoffsetBinMax = 356;
        }
        if(detName.Contains("EIC")) {
            offsetNbBin = 2500;
            xoffsetBinMin = -500;
            xoffsetBinMax = 500;
            yoffsetBinMin = -225;
            yoffsetBinMax = 225;
        }
        
        TString xOffsetName  = "offset" + planeX ;
        TString xOffsetTitle = "offset" + planeX ;
        TH1F * hXOffset = new TH1F(xOffsetName, xOffsetName, offsetNbBin, xoffsetBinMin,  xoffsetBinMax) ;
        hXOffset->SetYTitle("Counts") ;
        fResidualHistos[xOffsetName] = hXOffset ;
        printf("  SRSHistoManager::BookResidualsHisto()=>residual histo = %s \n", xOffsetName.Data()) ;
        
        TString yOffsetName  = "offset" + planeY ;
        TString yOffsetTitle = "offset" + planeY ;
        TH1F * hYOffset = new TH1F(yOffsetName, yOffsetName, offsetNbBin, yoffsetBinMin,  yoffsetBinMax) ;
        hYOffset->SetYTitle("Counts") ;
        fResidualHistos[yOffsetName] = hYOffset ;
        printf("  SRSHistoManager::BookResidualsHisto()=>residual histo = %s \n", yOffsetName.Data()) ;
        
        if(!fResidualHistos["hXtrackAngle"]) {
            TH1F * hXtrackAngle = new TH1F("hXtrackAngle", "hXtrackAngle", 100, -0.01, 0.01) ;
            fResidualHistos["hXtrackAngle"] = hXtrackAngle ;
        }
        if(!fResidualHistos["hYtrackAngle"] ) {
            TH1F * hYtrackAngle = new TH1F("hYtrackAngle", "hYtrackAngle", 100, -0.01, 0.01) ;
            fResidualHistos["hYtrackAngle"] = hYtrackAngle ;
        }
        if(!fResidualHistos["hXtrackOffset"]) {
            TH1F * hXtrackOffset = new TH1F("hXtrackOffset", "hXtrackOffset", 100, -100, 100) ;
            fResidualHistos["hXtrackOffset"] = hXtrackOffset ;
        }
        if(!fResidualHistos["hYtrackOffset"]) {
            TH1F * hYtrackOffset = new TH1F("hYtrackOffset", "hYtrackOffset", 100, -100, 100) ;
            fResidualHistos["hYtrackOffset"] = hYtrackOffset ;
        }
        
    }
}

//====================================================================================================================
void SRSHistoManager::FillResidualHistos(SRSTrack * track) {
    //printf("  SRSHistoManager::FillResidualHistos() ==> ENTER IN \n") ;
    
    if(track->IsAGoodEvent()) {
        map <TString, TString >         detectorList     = track->GetDetectorList() ;
        map <TString, vector<Float_t> > fittedData       = track->GetFittedSpacePoints() ;
        map <TString, vector<Float_t> > rawData          = track->GetRawDataSpacePoints() ;
        map <TString, vector<Float_t> > eicClustRawDataY = track->GetEICstripClusterRawDataY() ;
        
        vector <Float_t> referencePoint =  rawData["TrkGEM1"];
        map <TString, TString>::const_iterator detector_itr ;
        for(detector_itr =  detectorList.begin(); detector_itr !=  detectorList.end(); ++detector_itr) {
            
            TString detector = (*detector_itr).first ;
            vector <Float_t> rawPoint =  rawData[detector] ;
            vector <Float_t> fittedPoint = fittedData[detector] ;
            
            Int_t size = rawPoint.size() ;
            Int_t sizeRef = referencePoint.size() ;
            if(size == 0) continue ;
            
            ((TH1F*) fResidualHistos["hXtrackAngle"]->Fill(track->GetFitParameters()["xDirection"]));
            ((TH1F*) fResidualHistos["hYtrackAngle"]->Fill(track->GetFitParameters()["yDirection"]));
            ((TH1F*) fResidualHistos["hXtrackOffset"]->Fill(track->GetFitParameters()["xOffset"]));
            ((TH1F*) fResidualHistos["hYtrackOffset"]->Fill(track->GetFitParameters()["yOffset"]));
            
            //============================================
            // X-Y Offset & Plane Rotation Angle
            //============================================
            if(sizeRef != 0) {
                // X-Offsets
                /**
                 if(detector == "TrkGEM1") {
                 printf("  SRSHistoManager detector = %s, sbs_x = %f\n", detector.Data(), rawPoint[0]) ;
                 }
                 */
                TString xOffsetName = "offset" + (fMapping->GetDetectorPlaneListFromDetector(detector)).front() ;
                ((TH1F*) fResidualHistos[xOffsetName]->Fill(XYOffsets(referencePoint, rawPoint, "X"))) ;
                // Y-Offsets
                TString yOffsetName = "offset" + (fMapping->GetDetectorPlaneListFromDetector(detector)).back() ;
                ((TH1F*) fResidualHistos[yOffsetName]->Fill(XYOffsets(referencePoint, rawPoint, "Y"))) ;
                // Plane Rotation Angle
                TString xyRotationName = "rotation" +  detector ;
                ((TH1F*) fResidualHistos[xyRotationName]->Fill(PlaneRotationAngle(referencePoint, rawPoint))) ;
            }
            
            //============================================
            // Cartesian coordinates
            //============================================
            
            /**
             TString planeX = "residuals" + (fMapping->GetDetectorPlaneListFromDetector(detector)).front() ;
             TString planeY = "residuals" + (fMapping->GetDetectorPlaneListFromDetector(detector)).back() ;
             
             Float_t residualX = fittedPoint[0] - rawPoint[0] ;
             Float_t residualY = fittedPoint[1] - rawPoint[1] ;
             
             ((TH1F*) fResidualHistos[planeX])->Fill(residualX) ;
             ((TH1F*) fResidualHistos[planeY])->Fill(residualY) ;
             ((TH1F*) fResidualHistos[planeX])->SetXTitle("Residuals on X-strips (mm)") ;
             ((TH1F*) fResidualHistos[planeY])->SetXTitle("Residuals on Y-strips (mm)") ;
             */
            
            if(detector == "EIC1") {
                
                //============================================
                // Top and Bottom strip residuals
                //============================================
                vector <Float_t> eicClusterY =  eicClustRawDataY[detector] ;
                TString TopStripY    = "residuals" + (fMapping->GetDetectorPlaneListFromDetector(detector)).front() ;
                TString BottomStripY = "residuals" + (fMapping->GetDetectorPlaneListFromDetector(detector)).back() ;
                Float_t eic_offsetX = track->GetDetXOffset()[detector] ;
                Float_t eic_offsetY = track->GetDetYOffset()[detector] ;
                Float_t alpha       =  track->GetDetPlaneRotationCorrection()[detector] ;
                Float_t eicFitX = fittedPoint[0] ;
                Float_t eicFitY = fittedPoint[1] ;
                //Correct the rotation offset
                eicFitX = eicFitX * TMath::Cos(alpha) + eicFitY * TMath::Sin(alpha) ;
                eicFitY = eicFitY * TMath::Cos(alpha) - eicFitX * TMath::Sin(alpha) ;
                //Correct the translation offsett
                eicFitX  -= eic_offsetX ;
                eicFitY  -= eic_offsetY ;
                
                Float_t trapezoidDetLength      = fMapping->GetUVangleReadoutMap(detector) [0] ;
                Float_t trapezoidDetInnerRadius = fMapping->GetUVangleReadoutMap(detector) [1] ;
                Float_t trapezoidDetOuterRadius = fMapping->GetUVangleReadoutMap(detector) [2] ;
                Float_t uvAngleCosineDirection = ( trapezoidDetOuterRadius - trapezoidDetInnerRadius ) / (2 * trapezoidDetLength) ;
                
                Float_t eicFitTopStripY = ( (0.5 * trapezoidDetLength - eicFitX) * uvAngleCosineDirection ) + eicFitY ;
                Float_t eicFitBotStripY = ( (eicFitX - 0.5 * trapezoidDetLength) * uvAngleCosineDirection ) + eicFitY ;
                
                Float_t topRes = eicFitTopStripY - eicClusterY[1] ;
                Float_t botRes = eicFitBotStripY - eicClusterY[0] ;
                
                ((TH1F*) fResidualHistos[TopStripY])->Fill(topRes) ;
                ((TH1F*) fResidualHistos[BottomStripY])->Fill(botRes) ;
                
                //	((TH1F*) fResidualHistos[TopStripY])->Fill(eicFitTopStripY - eicClusterY[1]) ;
                //	((TH1F*) fResidualHistos[BottomStripY])->Fill(eicFitBotStripY - eicClusterY[0]) ;
                
                //============================================
                // Conversion in Cylindrical coordinates
                //============================================
                TString rname   = "residuals" + detector + "R" ;
                TString phyname = "residuals" + detector + "PHI" ;
                vector <Float_t> fittedCylPoint = CartesianToCylindricalCoordinates(1523.865, 0,fittedPoint ) ;
                vector <Float_t> rawCylPoint = CartesianToCylindricalCoordinates(1523.865, 0, rawPoint) ;
                
                ((TH1F*) fResidualHistos[rname])->Fill(fittedCylPoint[0] - rawCylPoint[0]) ;
                //        if( (fabs(topRes) < 0.2) and  (fabs(botRes) < 0.2) ) ((TH1F*) fResidualHistos[rname])->Fill(fittedCylPoint[0] - rawCylPoint[0]) ;
                Float_t diffphy = 1000 * (fittedCylPoint[1] - rawCylPoint[1]) ;
                ((TH1F*) fResidualHistos[phyname])->Fill(diffphy) ;
                
                ((TH1F*) fResidualHistos[rname])->SetXTitle("Residuals for r (mm)") ;
                ((TH1F*) fResidualHistos[phyname])->SetXTitle("Residuals for phi (mrad)") ;
                
                
                //============================================
                // Cartesian coordinates
                //============================================
                TString planeX = "residuals" + detector + "X" ;
                TString planeY = "residuals" + detector + "Y";
                Float_t residualX = fittedPoint[0] - rawPoint[0] ;
                Float_t residualY = fittedPoint[1] - rawPoint[1] ;
                
                ((TH1F*) fResidualHistos[planeX])->Fill(residualX) ;
                //	if( (fabs(topRes) < 0.2) and  (fabs(botRes) < 0.2) ) ((TH1F*) fResidualHistos[planeX])->Fill(residualX) ;
                ((TH1F*) fResidualHistos[planeY])->Fill(residualY) ;
                ((TH1F*) fResidualHistos[planeX])->SetXTitle("Residuals on X-strips (mm)") ;
                ((TH1F*) fResidualHistos[planeY])->SetXTitle("Residuals on Y-strips (mm)") ;
                
            }
            
            else {
                
                TString planeX = "residuals" + (fMapping->GetDetectorPlaneListFromDetector(detector)).front() ;
                TString planeY = "residuals" + (fMapping->GetDetectorPlaneListFromDetector(detector)).back() ;
                
                Float_t residualX = fittedPoint[0] - rawPoint[0] ;
                Float_t residualY = fittedPoint[1] - rawPoint[1] ;
                
                ((TH1F*) fResidualHistos[planeX])->Fill(residualX) ;
                ((TH1F*) fResidualHistos[planeY])->Fill(residualY) ;
                ((TH1F*) fResidualHistos[planeX])->SetXTitle("Residuals on X-strips (mm)") ;
                ((TH1F*) fResidualHistos[planeY])->SetXTitle("Residuals on Y-strips (mm)") ;
            }
        }
        
        referencePoint.clear() ;
        detectorList.clear() ;
        
        map <TString, vector<Float_t> >::const_iterator raw_itr ;
        for(raw_itr =  rawData.begin(); raw_itr !=  rawData.end(); ++raw_itr) {
            vector<Float_t> rawdata = (*raw_itr).second ;
            rawdata.clear() ;
        }
        rawData.clear() ;
        
        map <TString, vector<Float_t> >::const_iterator fit_itr ;
        for(fit_itr = fittedData.begin(); fit_itr != fittedData.end(); ++fit_itr) {
            vector<Float_t> fittedTrack = (*fit_itr).second;
            fittedTrack.clear() ;
        }
        fittedData.clear() ;
        
        map <TString, vector<Float_t> >::const_iterator clustY_itr ;
        for(clustY_itr = eicClustRawDataY.begin(); clustY_itr != eicClustRawDataY.end(); ++clustY_itr) {
            vector<Float_t> eicClustRawDataY = (*clustY_itr).second;
            eicClustRawDataY.clear() ;
        }
        eicClustRawDataY.clear() ;
    }
}

//====================================================================================================================
vector <Float_t> SRSHistoManager::CartesianToCylindricalCoordinates(Float_t offsetx, Float_t offsety,  vector <Float_t> cartesianCoordinates ) {
    vector <Float_t> cylindricalCoordinates ;
    cylindricalCoordinates.clear() ;
    Float_t xnew = cartesianCoordinates[0] + offsetx ;
    Float_t ynew = cartesianCoordinates[1] + offsety ;
    Float_t r = sqrt( (xnew * xnew) + (ynew * ynew) ) ;
    Float_t phy = atan (ynew / xnew) ;
    cylindricalCoordinates.push_back(r) ;
    cylindricalCoordinates.push_back(phy) ;
    cylindricalCoordinates.push_back(cartesianCoordinates[2]) ;
    return cylindricalCoordinates ;
}

//====================================================================================================================
Float_t SRSHistoManager::PlaneRotationAngle(vector <Float_t> referencePoint,  vector <Float_t> rotationPoint) {
    Float_t num = (rotationPoint[0] * referencePoint[1]) - (referencePoint[0] * rotationPoint[1]) ;
    Float_t denom = (referencePoint[0] * referencePoint[0]) + referencePoint[1] * referencePoint[1] ;
    return asin (num / denom) ;
}

//====================================================================================================================
Float_t SRSHistoManager::XYOffsets(vector <Float_t> referencePoint,  vector <Float_t> prodedDetector, TString xORy) {
    Float_t offset = 0 ;
    if(xORy == "X") offset = referencePoint[0] - prodedDetector[0];
    if(xORy == "Y") offset = referencePoint[1] - prodedDetector[1];
    return offset ;
}

//====================================================================================================================
//
//       CLUSTER POSTION CORRECTION: ETA FUNCTION
//
//====================================================================================================================
void SRSHistoManager::BookClusterPositionCorrectionHistos() {
    SRSMapping * mapping = SRSMapping::GetInstance() ;
    
    map <Int_t, TString >  detectorList = fMapping->GetDetectorFromIDMap() ;
    map <Int_t, TString >::const_iterator det_itr ;
    for(det_itr = detectorList.begin(); det_itr != detectorList.end(); ++det_itr) {
        
        Int_t detID     = (*det_itr).first ;
        TString detName = (*det_itr).second ;
        
        list<TString> planeList = mapping->GetDetectorPlaneListFromDetector(detName) ;
        list<TString >::const_iterator plane_itr ;
        for(plane_itr = planeList.begin(); plane_itr != planeList.end(); ++plane_itr) {
            TString plane = *plane_itr ;
            Float_t planeSize = mapping->GetSizeOfPlane(plane) ;
            Int_t nbOfAPVsFromPlane = mapping->GetNbOfAPVs(plane) ;
            Float_t pitch =  planeSize / (NCH * nbOfAPVsFromPlane) ;
            
            TString etaFuncName = "positionCorrection" + plane ;
            TString etaFuncTitle = plane + ": Eta reponse funtion " ;
            TH1F * etaFunc  = new TH1F(etaFuncName, etaFuncTitle, 500,  -0.5*pitch, 0.5*pitch) ;
            fPosCorrectionHistos[etaFuncName] = etaFunc ;
            printf("  SRSHistoManager::BookPositionCorrectionFunction()=> histo = %s \n", etaFuncName.Data()) ;
        }
        planeList.clear() ;
    }
    detectorList.clear() ;
}

//====================================================================================================================
void SRSHistoManager::FillClusterPositionCorrectionHistos(SRSEventBuilder * eventbuilder) {
    //  printf("SRSHistoManager::FillClusterPositionCorrectionHistos(): \n") ;
    map < TString, list <SRSCluster * > >  clustersInDetectorPlaneMap ;
    clustersInDetectorPlaneMap = eventbuilder->GetClustersInDetectorPlane() ;
    map < TString, list <SRSCluster * > >::const_iterator detPlane_itr ;
    for(detPlane_itr = clustersInDetectorPlaneMap.begin(); detPlane_itr != clustersInDetectorPlaneMap.end(); ++detPlane_itr) {
        TString detPlane = (*detPlane_itr).first ;
        TString histoName ="positionCorrection" + detPlane ;
        
        list <SRSCluster * > listOfClusters = (*detPlane_itr).second ;
        Int_t clustMult = listOfClusters.size() ;
        if(clustMult == 0) 	continue ;
        list <SRSCluster * >::const_iterator cluster_itr ;
        for(cluster_itr = listOfClusters.begin(); cluster_itr != listOfClusters.end(); ++cluster_itr ) {
            
            SRSCluster * cluster = * cluster_itr ;
            Int_t   clusterSize         = cluster->GetNbOfHits() ;
            Float_t clusterADCcounts    = cluster->GetClusterADCs() ;
            Float_t clusterPosition     = cluster->GetClusterPosition() ;
            Float_t clusterCentralStrip = cluster->GetClusterCentralStrip() ;
            
            Int_t nbOfAPVsFromPlane = cluster->GetNbAPVsFromPlane() ;
            Float_t planeSize = cluster->GetPlaneSize() ;
            Float_t pitch =  planeSize / (NCH * nbOfAPVsFromPlane) ;
            if(detPlane.Contains("EIC")) pitch = 2 * pitch ;
            Float_t diffPos = (clusterPosition - clusterCentralStrip) ;
            if(clusterSize < 2 ) 	continue ;
            ((TH1F*) fPosCorrectionHistos[histoName])->Fill(diffPos) ;
            ((TH1F*) fPosCorrectionHistos[histoName])->SetXTitle("(clusterPosition - CentralStripPosition) ") ;
        }
        listOfClusters.clear() ;
    }
    DeleteClustersInDetectorPlaneMap(clustersInDetectorPlaneMap) ;
}


//====================================================================================================================
//
//       3D TRACKING EVENT DSPLAY
//
//====================================================================================================================
void SRSHistoManager::BookEvent3DDisplayNtuple(SRSTrack * track) {
    fNtuple = new TNtuple("eventDisplayNtuple", track->GetNtupleTitle(), "x:y:z:parameter") ;
    fNtupleSizeX  = track->GetNtupleSizeX() ;
    fNtupleSizeY  = track->GetNtupleSizeY() ;
    fNtupleSizeZ  = track->GetNtupleSizeZ() ;
    fNtuple->Fill(-fNtupleSizeX/2, -fNtupleSizeY/2, -1.0, 0.0) ;
    fNtuple->Fill(-fNtupleSizeX/2, -fNtupleSizeY/2, fNtupleSizeZ, 0.0) ;
    fNtuple->Fill(-fNtupleSizeX/2, fNtupleSizeY/2, -1.0, 0.0) ;
    fNtuple->Fill(-fNtupleSizeX/2, fNtupleSizeY/2, fNtupleSizeZ, 0.0) ;
    fNtuple->Fill(fNtupleSizeX/2, -fNtupleSizeY/2, -1.0, 0.0) ;
    fNtuple->Fill(fNtupleSizeX/2, -fNtupleSizeY/2, fNtupleSizeZ, 0.0) ;
    fNtuple->Fill(fNtupleSizeX/2, fNtupleSizeY/2,  0.0, 0.0) ;
    fNtuple->Fill(fNtupleSizeX/2, fNtupleSizeY/2, fNtupleSizeZ, 0.0) ;
    printf("  SRSHistoManager::BookNTuple() ==> ntuple = %s booked; fNtupleSizeX=%f, fNtupleSizeY=%f, fNtupleSizeZ=%f\n", track->GetNtupleName().Data(), fNtupleSizeX, fNtupleSizeY, fNtupleSizeZ) ;
}

//====================================================================================================================
void SRSHistoManager::Event3DDisplay(SRSTrack * track) {
    //  printf("  SRSHistoManager::EventDisplay() ==> ENTER IN \n") ;
    if(track->IsAGoodEvent()) {
        //  printf("  SRSHistoManager::EventDisplay() ==> ENTER IN \n") ;
        fEventNumber++ ;
        map <TString, vector<Float_t> > rawData = track->GetRawDataSpacePoints() ;
        map <TString, vector<Float_t> >::const_iterator spacePoint_itr ;
        Int_t color = 0 ;
        for(spacePoint_itr =  rawData.begin(); spacePoint_itr !=  rawData.end(); ++spacePoint_itr) {
            TString detName = (*spacePoint_itr).first ;
            if(detName.Contains("Trk")) color = 1 ;
            if(detName.Contains("SBS")) color = 3 ;
            if(detName.Contains("EIC")) color = 7 ;
            vector <Float_t> spacePoint = (*spacePoint_itr).second ;
            if ((fabs(spacePoint[0]) > fNtupleSizeX/2) || (fabs(spacePoint[1]) > fNtupleSizeY/2) || (fabs(spacePoint[2]) > fNtupleSizeZ)) continue;
            if (fEventNumber % 10 == 0)  fNtuple->Fill(spacePoint[0], spacePoint[1], spacePoint[2], color) ;
        }
        
        map <TString, vector<Float_t> >::const_iterator raw_itr ;
        for(raw_itr =  rawData.begin(); raw_itr !=  rawData.end(); ++raw_itr) {
            vector<Float_t> rawdata = (*raw_itr).second ;
            rawdata.clear() ;
        }
        rawData.clear() ;
    }
}

//=========================================================================================================================
//
//                                                          OFFSETS
//
//========================================================================================================================
void SRSHistoManager::ComputeOffset(SRSTrack * track) {
    
    map<TString,TObject*>::const_iterator objToDraw_itr ;
    for (objToDraw_itr = fObjToDraw.begin(); objToDraw_itr != fObjToDraw.end(); ++objToDraw_itr) {
        TString histoName = (*objToDraw_itr).first;
        if (!fObjToDraw[histoName]) continue;
        
        if ((fVarType[histoName] == "OLD_OFFSETX") || (fVarType[histoName] == "OLD_OFFSETY") ) {
            
            TString detectorName1 = fMapping->GetDetectorFromPlane(fDetectorPlane1[histoName]) ;
            TString detectorName2 = fMapping->GetDetectorFromPlane(fDetectorPlane2[histoName]) ;
            
            map<TString, vector<Float_t> > detectorSpacePoint = track->GetTrackSpacePoints() ;
            std::vector< Float_t > point1 = detectorSpacePoint[detectorName1] ;
            std::vector< Float_t > point2 = detectorSpacePoint[detectorName2] ;
            
            if ((fVarType[histoName] == "OLD_OFFSETX") || (fVarType[histoName] == "OLD_OFFSETY") ) {
                Int_t coordinate = 0 ;
                TString title = "delta x (mm)" ;
                
                if(fVarType[histoName] == "OLD_OFFSETX") {
                    coordinate = 0 ;
                    title = "delta x (mm)" ;
                }
                if(fVarType[histoName] == "OLD_OFFSETY") {
                    coordinate = 1 ;
                    title = "delta y (mm)" ;
                }
                
                stringstream out ;
                out << fNbOfEvents[histoName] ;
                TString  nbOfEventsStr = out.str() ;
                TString newTitle = fObjTitle[histoName] ;
                ((TH1F*) fObjToDraw[histoName])->SetTitle(newTitle) ;
                ((TH1F*) fObjToDraw[histoName])->SetXTitle(title) ;
                ((TH1F*) fObjToDraw[histoName])->SetYTitle("Counts") ;
                ((TH1F*) fObjToDraw[histoName])->SetTitleSize(0.05,"xy") ;
                ((TH1F*) fObjToDraw[histoName])->Fill(point1[coordinate] - point2[coordinate]);
            }
        }
    }
}

//============================================================================================
static Bool_t CompareHitADCs( TObject *obj1, TObject *obj2) {
    Bool_t compare ;
    if ( ( (SRSHit*) obj1 )->GetHitADCs() > ( ( SRSHit*) obj2 )->GetHitADCs()) compare = kTRUE ;
    else compare = kFALSE ;
    return compare ;
}

//============================================================================================
static Bool_t CompareClusterADCs( TObject *obj1, TObject *obj2) {
    Bool_t compare ;
    if ( ( (SRSCluster*) obj1 )->GetClusterADCs() > ( ( SRSCluster*) obj2 )->GetClusterADCs()) compare = kTRUE ;
    else compare = kFALSE ;
    return compare ;
}

//====================================================================================================================
void SRSHistoManager::SetHistoTitle(TString histoName,  Int_t goodEvents,  Int_t triggerCount) {
    //  printf("SRSHistoManager::SetHistoTitle(): => entering the SetHistoTitle \n") ;
    stringstream out, out2, out3 ;
    
    out2 <<  goodEvents;
    TString  goodEventstr = out2.str() ;
    
    out3 << triggerCount ;
    TString triggerCountStr = out3.str() ;
    
    TString newTitle = fObjTitle[histoName] +  " ("  + goodEventstr + " / " + triggerCountStr + ")" ;
    ((TH1F*) fObjToDraw[histoName])->SetTitle(newTitle) ;
    ((TH1F*) fObjToDraw[histoName])->SetTitleSize(0.05,"xy") ;
}

//====================================================================================================================
void SRSHistoManager::SetHistoTitle(TString histoName) {
    //  printf("SRSHistoManager::PhysicRun(): => entering the PhysicsRun \n") ;
    stringstream out, out2 ;
    out << fNbOfEvents[histoName] ;
    TString  nbOfEventsStr = out.str() ;
    
    TString newTitle = fObjTitle[histoName] ;
    ((TH1F*) fObjToDraw[histoName])->SetTitle(newTitle) ;
    ((TH1F*) fObjToDraw[histoName])->SetTitleSize(0.05,"xy") ;
}

//====================================================================================================================
void SRSHistoManager::PhysicsRun(SRSTrack * track, SRSEventBuilder * eventbuilder, const eventHeaderStruct * eventHeader) {
    //  printf("SRSHistoManager::PhysicRun(): => entering the PhysicsRun \n") ;
    
    Int_t eventType      = eventHeader->eventType ;
    Int_t triggerCount   = eventbuilder->GetTriggerCount() ;
    
    map < TString, list <SRSHit * > >  hitsInDetectorPlaneMap ;
    hitsInDetectorPlaneMap = eventbuilder->GetHitsInDetectorPlane() ;
    
    map < TString, list <SRSCluster * > >  clustersInDetectorPlaneMap ;
    clustersInDetectorPlaneMap = eventbuilder->GetClustersInDetectorPlane() ;
    
    // START FILLING HISTOGRAMS
    map<TString,TObject*>::const_iterator objToDraw_itr ;
    for (objToDraw_itr = fObjToDraw.begin(); objToDraw_itr != fObjToDraw.end(); ++objToDraw_itr) {
        
        TString histoName = (*objToDraw_itr).first;
        if (!fObjToDraw[histoName]) {
            continue ;
        }
        
        fNbOfEvents[histoName] += 1 ;
        
        // Histograms to be reset at each event
        if ((fVarType[histoName] == "HITZEROSUP") || (fVarType[histoName] == "HITPEDOFFSET") || (fVarType[histoName] == "HITRAWDATA") || (fVarType[histoName] == "HITCOMMODE") || (fVarType[histoName] == "CLUSTERTIMEBIN") ) {
            fHist1dToReset.push_back((TH1F*)(fObjToDraw[histoName])) ;
        }
        
        if (fVarType[histoName] == "PLANETIMEBIN") {
            fHist2dToReset.push_back((TH2F*)(fObjToDraw[histoName])) ;
        }
        
        //=========================================================================================================================//
        //                                                      ZERO SUPPRESSION DATA                                              //
        //=========================================================================================================================//
        if ( (fVarType[histoName] == "ADCDIST") ||  (fVarType[histoName] == "SPECTRUM") || (fVarType[histoName] == "HITZEROSUP") || (fVarType[histoName] == "PLANETIMEBIN") || (fVarType[histoName] == "CLUSTERDIST") || (fVarType[histoName] == "CLUSTSIZE") || (fVarType[histoName] == "CLUSTMULT") || (fVarType[histoName] == "HITCROSSTALK") || (fVarType[histoName] == "CHARGECROSSTALK") || (fVarType[histoName] == "OCCUPANCY") || (fVarType[histoName] == "NOISERESO")   || (fVarType[histoName] == "TIMESLICE") || (fVarType[histoName] == "CLUSTERTIMEBIN") ) {
            //     printf("   SRSHistoManager::PhysicRun(): => %s with plane1 %s for histo %s \n",fVarType[histoName].Data(), fDetectorPlane1[histoName].Data(), histoName.Data() ) ;
            
            if (fZeroSupCut == 0) {
                Warning("FillHistos","=> NO ZERO SUPPRESION CUT sigmaLevel = %d not set\n",fZeroSupCut) ;
                continue ;
            }
            
            TString detector    = fMapping->GetDetectorFromPlane(fDetectorPlane1[histoName].Data()) ;
            TString detectorType = fMapping->GetDetectorTypeFromDetector(detector) ;
            
            /**
             if(!(eventbuilder->IsAGoodEventInDetector(detector.Data()))) {
             if ( detectorType != "CMSGEM") {
             fNbOfEvents[histoName] -= 1 ;
             continue ;
             }
             }
             */
            
            if (!eventbuilder->IsAGoodEvent()) {
                fNbOfEvents[histoName] -= 1 ;
                continue ;
            }
            
            list <SRSCluster * > listOfClusters = clustersInDetectorPlaneMap[fDetectorPlane1[histoName].Data()] ;
            Int_t clustMult = listOfClusters.size() ;
            if(clustMult == 0) {
                fNbOfEvents[histoName] -= 1 ;
                listOfClusters.clear() ;
                continue ;
            }
            
            SetHistoTitle(histoName) ;
            if ( ((fVarType[histoName] == "HITCROSSTALK") || ((fVarType[histoName] == "CHARGECROSSTALK"))) && (clustMult == 2)) {
                
                listOfClusters.sort(CompareClusterADCs) ;
                SRSCluster * cluster1  = listOfClusters.front() ;
                SRSCluster * cluster2  = listOfClusters.back() ;
                Float_t diffStripNb    = fabs(cluster1->GetClusterCentralStrip() - cluster2->GetClusterCentralStrip()) ;
                Float_t ratioADCCounts = cluster1->GetClusterADCs() /  cluster2->GetClusterADCs() ;
                
                if(ratioADCCounts > 5) {
                    stringstream out ;
                    TString newTitle = fObjTitle[histoName] ;
                    ((TH1*) fObjToDraw[histoName])->SetTitle(newTitle) ;
                    ((TH1*) fObjToDraw[histoName])->SetTitleSize(0.05,"xy") ;
                    
                    if (fVarType[histoName] == "HITCROSSTALK" ) {
                        ((TH1F*) fObjToDraw[histoName])->SetYTitle("Counts") ;
                        ((TH1F*) fObjToDraw[histoName])->SetXTitle("Pos [cluster1] - Pos [cluster2] (strip number)") ;
                        ((TH1F*) fObjToDraw[histoName])->Fill(diffStripNb) ;
                    }
                    
                    if (fVarType[histoName] == "CHARGECROSSTALK" ) {
                        ((TH1F*) fObjToDraw[histoName])->SetYTitle("Counts") ;
                        ((TH1F*) fObjToDraw[histoName])->SetXTitle("(ADC count [cluster1]) / (ADC count [Cluster2]) ") ;
                        ((TH1F*) fObjToDraw[histoName])->Fill(ratioADCCounts) ;
                    }
                }
            }
            
            if (fVarType[histoName] == "CLUSTMULT") {
                ((TH1F*) fObjToDraw[histoName])->SetXTitle("Cluster Multiplicity") ;
                ((TH1F*) fObjToDraw[histoName])->SetYTitle("Counts") ;
                ((TH1F*) fObjToDraw[histoName])->Fill(clustMult) ;
            }
            
            if (fVarType[histoName] == "ADCDIST") {
                if(eventbuilder->GetClusterMaxOrTotalADCs() == "totalADCs") {
                    ((TH1F*) fObjToDraw[histoName])->SetXTitle("Cluster total charges (ADC counts)") ;
                }
                else {
                    ((TH1F*) fObjToDraw[histoName])->SetXTitle("Cluster peak height (ADC counts)") ;
                }
                ((TH1F*) fObjToDraw[histoName])->SetYTitle("Counts") ;
            }
            
            if (fVarType[histoName] == "OCCUPANCY") {
                ((TH1F*) fObjToDraw[histoName])->SetXTitle("Strip Occupancy") ;
                ((TH1F*) fObjToDraw[histoName])->SetYTitle("Counts") ;
            }
            
            if (fVarType[histoName] == "CLUSTSIZE") {
                ((TH1F*) fObjToDraw[histoName])->SetXTitle("Cluster size") ;
                ((TH1F*) fObjToDraw[histoName])->SetYTitle("Counts") ;
            }
            
            if ( ( fVarType[histoName] == "HITZEROSUP") || ((fVarType[histoName] == "CLUSTERDIST") ) ) {
                
                if ( (fVarType[histoName] == "CLUSTERDIST")  && ( (histoName.Contains("clusterADCDist")) || (histoName.Contains("clusterADCDist")) || (histoName.Contains("clusterAdcDist")) || (histoName.Contains("clusterAdcDist")) ) ) {
                    ((TH1F*) fObjToDraw[histoName])->SetYTitle("ADC Counts") ;
                }
                if ( (fVarType[histoName] == "CLUSTERDIST")  && (histoName.Contains("distanceToStrip")) ) {
                    ((TH1F*) fObjToDraw[histoName])->SetXTitle("diff cluster Position cluster central strip") ;
                    ((TH1F*) fObjToDraw[histoName])->SetYTitle("Counts") ;
                }
                if (fVarType[histoName] == "HITZEROSUP") ((TH1F*) fObjToDraw[histoName])->SetYTitle("ADC Counts") ;
                
                if(!histoName.Contains("StripNb")) {
                    ((TH1F*) fObjToDraw[histoName])->SetXTitle("Cluster position (mm)") ;
                }
                
                else {
                    Int_t nbBins = NCH * fMapping->GetNbOfAPVs(fDetectorPlane1[histoName]) ;
                    Int_t nmax   = NCH * fMapping->GetNbOfAPVs(fDetectorPlane1[histoName]) ;
                    TString detector = fMapping->GetDetectorFromPlane(fDetectorPlane1[histoName].Data()) ;
                    TString readoutBoard = fMapping->GetReadoutBoardFromDetector(detector) ;
                    TString detectorType = fMapping->GetDetectorTypeFromDetector(detector) ;
                    
                    if(readoutBoard =="PADPLANE") {
                        Int_t nbPadX = (Int_t) (fMapping->GetPadDetectorMap(detector) [2])  ;
                        Int_t nbPadY = (Int_t) (fMapping->GetPadDetectorMap(detector) [3]) ;
                        nbBins = nbPadX * nbPadY ;
                        nmax   = nbBins - 1  ;
                        ((TH1F*) fObjToDraw[histoName])->SetBins(nbBins,0, nmax) ;
                        ((TH1F*) fObjToDraw[histoName])->SetXTitle("hit position (strip number)") ;
                    }
                    
                    if ((readoutBoard == "UV_ANGLE") &&  (detectorType == "EICPROTO1") ) {
                        ((TH1F*) fObjToDraw[histoName])->SetBins(nbBins,0,nmax) ;
                        ((TH1F*) fObjToDraw[histoName])->SetXTitle("Cluster position (strip number)") ;
                    }
                }
            }
            
            if (fVarType[histoName] == "PLANETIMEBIN") {
                ((TH2F*) fObjToDraw[histoName])->SetYTitle("time bin number (25 ns)") ;
                ((TH2F*) fObjToDraw[histoName])->SetZTitle("ADC Counts") ;
                
                if(!histoName.Contains("StripNb")) {;
                    ((TH2F*) fObjToDraw[histoName])->SetXTitle("hit position (mm)") ;
                }
                else {
                    Int_t nbBins = NCH * fMapping->GetNbOfAPVs(fDetectorPlane1[histoName]) ;
                    Int_t nmax   = NCH * fMapping->GetNbOfAPVs(fDetectorPlane1[histoName]) ;
                    TString detector = fMapping->GetDetectorFromPlane(fDetectorPlane1[histoName].Data()) ;
                    TString readoutBoard = fMapping->GetReadoutBoardFromDetector(detector) ;
                    TString detectorType = fMapping->GetDetectorTypeFromDetector(detector) ;
                    ((TH2F*) fObjToDraw[histoName])->SetXTitle("hit position (stip number)") ;
                    ((TH2F*) fObjToDraw[histoName])->SetBins(nbBins,0,nmax,fNBinTime,fRangeMinTime,fRangeMaxTime) ;
                }
            }
            
            Int_t clusterNo = 0 ;
            Int_t stripOccupancy = 0 ;
            
            list <SRSCluster * >::const_iterator cluster_itr ;
            for(cluster_itr = listOfClusters.begin(); cluster_itr != listOfClusters.end(); ++cluster_itr ) {
                clusterNo++ ;
                SRSCluster * cluster     = * cluster_itr ;
                Int_t   clusterSize      = cluster->GetNbOfHits() ;
                Float_t clusterADCcounts = cluster->GetClusterADCs() ;
                Float_t clusterPosition  = cluster->GetClusterPosition() ;
                Float_t clusterCentralStrip  = cluster->GetClusterCentralStrip() ;
                stripOccupancy += clusterSize ;
                
                if ( (fVarType[histoName] ==  "TIMESLICE" ) && (histoName.Contains("cluster"))) {
                    Int_t timeSlice = cluster->GetClusterPeakTimeBin() ;
                    if (histoName.Contains("clusterPeakTimeBin")) timeSlice = cluster->GetClusterPeakTimeBin() ;
                    if (histoName.Contains("clusterTimeBin"))     timeSlice = cluster->GetClusterTimeBin() ;
                    
                    ((TH1F*) fObjToDraw[histoName])->SetXTitle("Time bin of the peak of highest strip in cluster") ;
                    ((TH1F*) fObjToDraw[histoName])->SetYTitle("Counts") ;
                    ((TH1F*) fObjToDraw[histoName])->Fill(timeSlice) ;
                }
                
                if (fVarType[histoName] == "ADCDIST") {
                    if(clusterNo > 1) continue ;
                    if (clusterADCcounts != 0) ((TH1F*) fObjToDraw[histoName])->Fill(cluster->GetClusterADCs()) ;
                    
                }
                
                if (fVarType[histoName] == "CLUSTSIZE") {
                    if(clusterNo > 1) continue ;
                    if (clusterADCcounts != 0) ((TH1F*) fObjToDraw[histoName])->Fill(clusterSize) ;
                }
                
                if (fVarType[histoName] == "CLUSTERDIST" ) {
                    if ( (histoName.Contains("clusterDist")) || (histoName.Contains("ClusterDist")) ) {
                        ((TH1F*) fObjToDraw[histoName])->Fill(clusterPosition) ;
                    }
                    
                    else if ( histoName.Contains("distanceToStrip") ) {
                        ((TH1F*) fObjToDraw[histoName])->Fill(clusterPosition - clusterCentralStrip) ;
                    }
                    
                    else if ( (histoName.Contains("clusterADCDist")) || (histoName.Contains("clusterADCDist")) || (histoName.Contains("clusterAdcDist")) || (histoName.Contains("clusterAdcDist"))) {
                        ((TH1F*) fObjToDraw[histoName])->Fill(clusterPosition, clusterADCcounts) ;
                    }
                    else {
                        ((TH1F*) fObjToDraw[histoName])->Fill(clusterPosition) ;
                    }
                }
                
                if (fVarType[histoName] == "CLUSTERTIMEBIN") {
                    SetHistoTitle(histoName, fNbOfEvents[histoName], triggerCount) ;
                    vector<Float_t > timebinCharges = cluster->GetClusterTimeBinADCs() ;
                    Int_t timebin = 0 ;
                    vector<Float_t >::const_iterator timebinCharge_itr  ;
                    ((TH1F*) fObjToDraw[histoName])->SetXTitle("Time sample (25ns) of the cluster") ;
                    ((TH1F*) fObjToDraw[histoName])->SetYTitle("sum ADCs of the cluster") ;
                    for (timebinCharge_itr = timebinCharges.begin(); timebinCharge_itr != timebinCharges.end(); ++timebinCharge_itr) {
                        Float_t charges = * timebinCharge_itr ;
                        if(charges < 200) charges = 0 ;
                        ((TH1F*) fObjToDraw[histoName])->Fill(timebin,charges) ;
                        timebin++ ;
                    }
                    timebinCharges.clear() ;
                }
                
                for(Int_t i = 0; i < clusterSize; i++ ) {
                    SRSHit * hit = cluster->GetHit(i) ;
                    Int_t stripNb = hit->GetStripNo() ;
                    Float_t hitADCcounts= hit->GetHitADCs()  ;
                    Float_t hitPos = hit->GetStripPosition() ;
                    Float_t centeredHitPos = hitPos - clusterPosition;
                    Float_t normalizedCharge = hitADCcounts / clusterADCcounts ;
                    
                    if ( fVarType[histoName] == "HITZEROSUP") {
                        if(!histoName.Contains("StripNb")) {
                            ((TH1F*) fObjToDraw[histoName])->Fill(hitPos,hitADCcounts) ;
                        }
                        else {
                            ((TH1F*) fObjToDraw[histoName])->Fill(stripNb,hitADCcounts) ;
                        }
                    }
                    
                    if (fVarType[histoName] == "PLANETIMEBIN") {
                        SetHistoTitle(histoName, fNbOfEvents[histoName], triggerCount) ;
                        vector<Float_t > timebinADCs = hit->GetTimeBinADCs() ;
                        Int_t timebin = 0 ;
                        vector<Float_t >::const_iterator timebinCharge_itr  ;
                        for (timebinCharge_itr = timebinADCs.begin(); timebinCharge_itr != timebinADCs.end(); ++timebinCharge_itr) {
                            Float_t charges = * timebinCharge_itr ;
                            if(!histoName.Contains("StripNb")) {
                                ((TH2F*) fObjToDraw[histoName])->Fill(hitPos,timebin,charges) ;
                                
                            }
                            else {
                                ((TH2F*) fObjToDraw[histoName])->Fill(stripNb,timebin,charges) ;
                            }
                            timebin++ ;
                        }
                        timebinADCs.clear() ;
                    }
                    
                }
            }
            
            if (fVarType[histoName] == "OCCUPANCY") ((TH1F*) fObjToDraw[histoName])->Fill(stripOccupancy) ;
            listOfClusters.clear() ;
        }
        
        if ( (fVarType[histoName] == "HITPEDOFFSET") || (fVarType[histoName] == "HITDIST") || (fVarType[histoName] == "TIMESLICE")  ) {
            SetHistoTitle(histoName) ;
            list <SRSHit * > listOfHits = hitsInDetectorPlaneMap[fDetectorPlane1[histoName].Data()] ;
            
            if ( (fVarType[histoName] == "HITPEDOFFSET") || (fVarType[histoName] == "HITDIST") ) {
                if (!histoName.Contains("StripNb")) {
                    ((TH1F*) fObjToDraw[histoName])->SetXTitle("position (mm)");
                }
                
                else {
                    TString detector = fMapping->GetDetectorFromPlane(fDetectorPlane1[histoName].Data()) ;
                    TString readoutBoard = fMapping->GetReadoutBoardFromDetector(detector) ;
                    TString detectorType = fMapping->GetDetectorTypeFromDetector(detector) ;
                    
                    if(readoutBoard =="PADPLANE") {
                        Int_t nbPadX = (Int_t) (fMapping->GetPadDetectorMap(detector) [2])  ;
                        Int_t nbPadY = (Int_t) (fMapping->GetPadDetectorMap(detector) [3]) ;
                        Int_t nbBins = nbPadX * nbPadY ;
                        Int_t  nmax  = nbBins - 1  ;
                        ((TH1F*) fObjToDraw[histoName])->SetBins(nbBins,0,nmax) ;
                        ((TH1F*) fObjToDraw[histoName])->SetXTitle("position (strip number)") ;
                    }
                    else {
                        Int_t nbBins = NCH * fMapping->GetNbOfAPVs(fDetectorPlane1[histoName]) ;
                        Int_t nmax  =  NCH * fMapping->GetNbOfAPVs(fDetectorPlane1[histoName]);
                        ((TH1F*) fObjToDraw[histoName])->SetBins(nbBins,0, nmax) ;
                        ((TH1F*) fObjToDraw[histoName])->SetXTitle("position (strip number)") ;
                    }
                }
            }
            
            list <SRSHit * >::const_iterator hit_itr ;
            for(hit_itr = listOfHits.begin(); hit_itr != listOfHits.end(); ++hit_itr ) {
                SRSHit * hit = * hit_itr ;
                
                Float_t stripNb      = hit->GetStripNo() ;
                Float_t hitPos       = hit->GetStripPosition() ;
                Float_t hitADCcounts = hit->GetHitADCs() ;
                
                if ( (fVarType[histoName] ==  "TIMESLICE" ) && (histoName.Contains("hitTiming"))) {
                    Int_t timeSlice  = hit->GetSignalPeakBinNumber() ;
                    ((TH1F*) fObjToDraw[histoName])->SetXTitle("Time bin signal's peak") ;
                    ((TH1F*) fObjToDraw[histoName])->SetYTitle("Counts") ;
                    ((TH1F*) fObjToDraw[histoName])->Fill(timeSlice) ;
                }
                
                if (fVarType[histoName] ==  "HITPEDOFFSET" ) {
                    ((TH1F*) fObjToDraw[histoName])->SetYTitle("ADC Counts") ;
                    if (!histoName.Contains("StripNb")) ((TH1F*) fObjToDraw[histoName])->Fill(hitPos,hitADCcounts) ;
                    else                                ((TH1F*) fObjToDraw[histoName])->Fill(stripNb,hitADCcounts) ;
                }
                
                if (fVarType[histoName] == "HITDIST") {
                    if ( (histoName.Contains("hitDist"))  || (histoName.Contains("hitdist")) || (histoName.Contains("HitDist")) ) {
                        ((TH1F*) fObjToDraw[histoName])->SetYTitle("Counts") ;
                        if(!histoName.Contains("StripNb")) ((TH1F*) fObjToDraw[histoName])->Fill(hitPos) ;
                        else 	                       ((TH1F*) fObjToDraw[histoName])->Fill(stripNb) ;
                    }
                    if ( (histoName.Contains("HitADCDist")) || (histoName.Contains("hitADCDist")) || (histoName.Contains("HitAdcDist")) || (histoName.Contains("hitAdcDist")) ) {
                        ((TH1F*) fObjToDraw[histoName])->SetYTitle("ADC Counts") ;
                        if(!histoName.Contains("StripNb")) ((TH1F*) fObjToDraw[histoName])->Fill(hitPos,hitADCcounts) ;
                        else 	                       ((TH1F*) fObjToDraw[histoName])->Fill(stripNb,hitADCcounts) ;
                    }
                }
            }
            listOfHits.clear() ;
        }
        
        //=========================================================================================================================//
        //                                                  HITMAP, CHARGE SHARING                                                 //
        //=========================================================================================================================//
        if ( (fVarType[histoName] == "CHARGES_SH") ||  (fVarType[histoName] == "CHARGES_RATIO") || (fVarType[histoName] == "CHARGES_RATIODIST") || (fVarType[histoName] == "CLUSTCOR") || (fVarType[histoName] == "HITMAP") || (fVarType[histoName] == "CMSHITMAP") ) {
            //       printf("   SRSHistoManager::PhysicRun(): => %s with plane1 %s and plane2 %s for histo %s \n",fVarType[histoName].Data(), fDetectorPlane1[histoName].Data(), fDetectorPlane2[histoName].Data(), histoName.Data() ) ;
            
            if (fZeroSupCut == 0) {
                Warning("FillHistos","=> NO ZERO SUPPRESION CUT sigmaLevel = %d not set\n",fZeroSupCut) ;
                continue ;
            }
            
            TString planeX = (fMapping->GetDetectorPlaneListFromDetector(fDetector[histoName])).front() ;
            TString planeY = (fMapping->GetDetectorPlaneListFromDetector(fDetector[histoName])).back() ;
            
            if(!(eventbuilder->IsAGoodEvent())) {
                fNbOfEvents[histoName] -= 1 ;
                continue ;
            }
            
            SetHistoTitle(histoName) ;
            map < Int_t, vector <Float_t > > detectorEvent = eventbuilder->GetDetectorCluster(fDetector[histoName]) ;
            Int_t clusterMult = detectorEvent.size() ;
            if (clusterMult ==  0) {
                fNbOfEvents[histoName] -= 1 ;
                detectorEvent.clear() ;
                continue ;
            }
            
            //*******************************************************************************
            // Make sense to use only ONE CLUSTER in a multi-cluster event for this analysis
            // The selection is on the cluster with maximum charge
            //*******************************************************************************
            Float_t pos1 = detectorEvent[0][0] ;
            Float_t pos2 = detectorEvent[0][1] ;
            
            Float_t adcCount1 = detectorEvent[0][2] ;
            Float_t adcCount2 = detectorEvent[0][3] ;
            Float_t totalCharge = adcCount1 + adcCount2 ;
            
            Float_t timing1 = detectorEvent[0][4] ;
            Float_t timing2 = detectorEvent[0][5] ;
            
            ((TH2F*) fObjToDraw[histoName])->SetXTitle(" x-strips (mm)") ;
            ((TH2F*) fObjToDraw[histoName])->SetYTitle(" y-strips (mm)") ;
            
            if(planeX != planeY) {
                ((TH2F*) fObjToDraw[histoName])->SetXTitle(" x-strips (mm)") ;
                ((TH2F*) fObjToDraw[histoName])->SetYTitle(" y-strips (mm)") ;
            }
            
            if ( fVarType[histoName] == "CHARGES_SH") {
                ((TH2F*) fObjToDraw[histoName])->SetXTitle("Cluster charge (ADC counts) in X") ;
                ((TH2F*) fObjToDraw[histoName])->SetYTitle("Cluster charge (ADC counts) in Y") ;
                
                if(planeX != planeY) {
                    if(eventbuilder->GetClusterMaxOrTotalADCs() == "totalADCs") {
                        ((TH2F*) fObjToDraw[histoName])->SetXTitle("Cluster ADCs (X-plane)") ;
                        ((TH2F*) fObjToDraw[histoName])->SetYTitle("Cluster ADCs (Y-plane)") ;
                    }
                    else {
                        ((TH2F*) fObjToDraw[histoName])->SetXTitle("Cluster peak ADC (X-plane)") ;
                        ((TH2F*) fObjToDraw[histoName])->SetYTitle("Cluster peak ADC (Y-plane)") ;
                    }
                }
                ((TH2F*) fObjToDraw[histoName])->SetZTitle("Events") ;
                ((TH2F*) fObjToDraw[histoName])->Fill(adcCount1, adcCount2) ;
            }
            
            if (fVarType[histoName] == "CHARGES_RATIO") {
                if(eventbuilder->GetClusterMaxOrTotalADCs() == "totalADCs") {
                    ((TH1F*) fObjToDraw[histoName])->SetXTitle("Cluster total charge ratio X/Y ") ;
                }
                else {
                    ((TH1F*) fObjToDraw[histoName])->SetXTitle("Cluster peak height ratio X/Y ") ;
                }
                ((TH1F*) fObjToDraw[histoName])->SetYTitle("Counts") ;
                ((TH1F*) fObjToDraw[histoName])->Fill(adcCount1 / adcCount2 ) ;
            }
            
            if (fVarType[histoName] == "CHARGES_RATIODIST") {
                ((TH2F*) fObjToDraw[histoName])->SetZTitle("total charge ratio X/Y") ;
                Float_t ratio = adcCount1 / adcCount2 ;
                ((TH2F*) fObjToDraw[histoName])->Fill(pos1, pos2, ratio) ;
            }
            
            if (fVarType[histoName] == "HITMAP") {
                TString readoutBoard = fMapping->GetReadoutBoardFromDetector(fDetector[histoName].Data()) ;
                TString detectorType = fMapping->GetDetectorTypeFromDetector(fDetector[histoName].Data()) ;
                
                if ( readoutBoard =="PADPLANE")  {
                    Float_t padSizeX = fMapping->GetPadDetectorMap(fDetector[histoName]) [0] ;
                    Float_t padSizeY = fMapping->GetPadDetectorMap(fDetector[histoName]) [1] ;
                    Int_t nbPadX = (Int_t) (fMapping->GetPadDetectorMap(fDetector[histoName]) [2]) ;
                    Int_t nbPadY = (Int_t) (fMapping->GetPadDetectorMap(fDetector[histoName]) [3]) ;
                    Float_t rangeX = nbPadX * padSizeX ;
                    Float_t rangeY = nbPadY * padSizeY ;
                    ((TH1F*) fObjToDraw[histoName])->SetBins(nbPadX, 0, rangeX, nbPadY, 0, rangeY) ;
                }
                
                if ( (histoName.Contains("Hit2D")) || (histoName.Contains("hit2D")) || (histoName.Contains("HIT2D")) ) {
                    ((TH2F*) fObjToDraw[histoName])-> SetZTitle("Counts") ;
                    ((TH2F*) fObjToDraw[histoName])->Fill(pos1, pos2) ;
                }
                
                if ( (histoName.Contains("adc2D")) || (histoName.Contains("ADC2D")) || (histoName.Contains("Adc2D")) ) {
                    ((TH2F*) fObjToDraw[histoName])->SetZTitle("ADC channels (Cluster)") ;
                    Float_t totalCharge = adcCount1 + adcCount2 ;
                    ((TH2F*) fObjToDraw[histoName])->Fill(pos1, pos2,totalCharge) ;
                }
                
                if ( (histoName.Contains("timing2D")) || (histoName.Contains("Timing2D")) || (histoName.Contains("timeBin2D")) ) {
                    ((TH2F*) fObjToDraw[histoName])->SetZTitle("Time bin of the peak of highest strip in cluster") ;
                    
                    if(histoName.Contains("X") ) {
                        ((TH2F*) fObjToDraw[histoName])->Fill(pos1, pos2, timing1) ;
                    }
                    
                    else if(histoName.Contains("Y") ) {
                        ((TH2F*) fObjToDraw[histoName])->Fill(pos1, pos2, timing2) ;
                    }
                    else {
                        Float_t timebin = (timing1 + timing2) / 2 ;
                        ((TH2F*) fObjToDraw[histoName])->Fill(pos1, pos2, timebin) ;
                        
                    }
                }
                if ( (histoName.Contains("chargeRatio2D")) || (histoName.Contains("ChargeRatio2D")) ) {
                    if(eventbuilder->GetClusterMaxOrTotalADCs() == "totalADCs") {
                        ((TH2F*) fObjToDraw[histoName])->SetZTitle("Cluster total ADCs ratio X/Y ") ;
                    }
                    else {
                        ((TH2F*) fObjToDraw[histoName])->SetZTitle("Cluster peak height ratio X/Y ") ;
                    }
                    Float_t ratio = adcCount1 / adcCount2 ;
                    ((TH2F*) fObjToDraw[histoName])->Fill(pos1, pos2, ratio) ;
                }
            }
            
            for (Int_t k = 0 ; k < clusterMult ; k++) {
                detectorEvent[k].clear() ;
            }
            detectorEvent.clear() ;
        }
        
        //=========================================================================================================================//
        //                                POSITION CORRELATION BETWEEN 2 DETECTORS                                                 //
        //=========================================================================================================================//
        if (fVarType[histoName] == "CORRELATION")  {
            //      printf("   SRSHistoManager::PhysicRun(): => %s with plane1 %s and plane2 %s for histo %s \n",fVarType[histoName].Data(), fDetectorPlane1[histoName].Data(), fDetectorPlane1[histoName].Data(), histoName.Data() ) ;
            
            if (fZeroSupCut == 0) {
                Warning("FillHistos","=> NO ZERO SUPPRESION CUT sigmaLevel = %d not set\n",fZeroSupCut) ;
                continue ;
            }
            TString detector1 = fMapping->GetDetectorFromPlane(fDetectorPlane1[histoName]) ;
            TString detector2 = fMapping->GetDetectorFromPlane(fDetectorPlane2[histoName]) ;
            if(!(eventbuilder->IsAGoodEvent())) {
                fNbOfEvents[histoName] -= 1 ;
                continue ;
            }
            
            SetHistoTitle(histoName, fNbOfEvents[histoName], triggerCount) ;
            map < Int_t, vector <Float_t > > detectorEvent1 = eventbuilder->GetDetectorCluster(detector1) ;
            map < Int_t, vector <Float_t > > detectorEvent2 = eventbuilder->GetDetectorCluster(detector2) ;
            Int_t clusterMult1 = detectorEvent1.size() ;
            Int_t clusterMult2 = detectorEvent2.size() ;
            if(clusterMult1 != clusterMult2) {
                fNbOfEvents[histoName] -= 1 ;
                detectorEvent1.clear() ;
                detectorEvent2.clear() ;
                continue ;
            }
            
            for (Int_t k = 0 ; k < clusterMult1 ; k++) {
                if( (histoName.Contains("timing")) || (histoName.Contains("Timing")) ) {
                    ((TH1F*) fObjToDraw[histoName])->SetXTitle("x-strips: Time bin signal's peak") ;
                    ((TH1F*) fObjToDraw[histoName])->SetYTitle("y-strips: Time bin signal's peak") ;
                    ((TH2F*) fObjToDraw[histoName])-> SetZTitle("Counts") ;
                    
                    Float_t x1t = detectorEvent1[k][4] ;
                    Float_t y1t = detectorEvent1[k][5] ;
                    Float_t x2t = detectorEvent2[k][4] ;
                    Float_t y2t = detectorEvent2[k][5] ;
                    
                    if( (fDetectorPlane1[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector1)).front() ) && (fDetectorPlane2[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector2)).front()) ) {
                        ((TH2F*) fObjToDraw[histoName])->Fill(x1t, x2t) ;
                    }
                    if( (fDetectorPlane1[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector1)).front() ) && (fDetectorPlane2[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector2)).back()) ) {
                        ((TH2F*) fObjToDraw[histoName])->Fill(x1t, y2t) ;
                    }
                    if( (fDetectorPlane1[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector1)).back() ) && (fDetectorPlane2[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector2)).front()) ) {
                        ((TH2F*) fObjToDraw[histoName])->Fill(y1t, x2t) ;
                    }
                    if( (fDetectorPlane1[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector1)).back() ) && (fDetectorPlane2[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector2)).back()) ) {
                        ((TH2F*) fObjToDraw[histoName])->Fill(y1t, y2t) ;
                    }
                    detectorEvent1[k].clear() ;
                    detectorEvent2[k].clear() ;
                }
                
                else {
                    Float_t x1 = detectorEvent1[k][0] ;
                    Float_t y1 = detectorEvent1[k][1] ;
                    Float_t x2 = detectorEvent2[k][0] ;
                    Float_t y2 = detectorEvent2[k][1] ;
                    
                    TString readoutBoard = fMapping->GetReadoutBoardFromDetector(fDetector[histoName].Data()) ;
                    TString detectorType = fMapping->GetDetectorTypeFromDetector(fDetector[histoName].Data()) ;
                    
                    if ( (readoutBoard == "UV_ANGLE") &&  (detectorType == "EICPROTO1") ) {
                        ((TH2F*) fObjToDraw[histoName])->SetBins(400, -500, 500, 200, -250, 250) ;
                    }
                    
                    ((TH2F*) fObjToDraw[histoName])-> SetZTitle("Counts") ;
                    
                    if( (fDetectorPlane1[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector1)).front() ) && (fDetectorPlane2[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector2)).front()) ) {
                        ((TH2F*) fObjToDraw[histoName])->Fill(x1, x2) ;
                    }
                    if( (fDetectorPlane1[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector1)).front() ) && (fDetectorPlane2[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector2)).back()) ) {
                        ((TH2F*) fObjToDraw[histoName])->Fill(x1, y2) ;
                    }
                    if( (fDetectorPlane1[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector1)).back() ) && (fDetectorPlane2[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector2)).front()) ) {
                        ((TH2F*) fObjToDraw[histoName])->Fill(y1, x2) ;
                    }
                    if( (fDetectorPlane1[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector1)).back() ) && (fDetectorPlane2[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector2)).back()) ) {
                        ((TH2F*) fObjToDraw[histoName])->Fill(y1, y2) ;
                    }
                    detectorEvent1[k].clear() ;
                    detectorEvent2[k].clear() ;
                }
            }
            detectorEvent1.clear() ;
            detectorEvent2.clear() ;
        }
        
        //=========================================================================================================================//
        //                                POSITION CORRELATION BETWEEN 2 DETECTORS                                                 //
        //=========================================================================================================================//
        if (fVarType[histoName] == "RELATIVE_OFFSET") {
            
            //      printf("   SRSHistoManager::PhysicRun(): => %s with plane1 %s and plane2 %s for histo %s \n",fVarType[histoName].Data(), fDetectorPlane1[histoName].Data(), fDetectorPlane1[histoName].Data(), histoName.Data() ) ;
            if (fZeroSupCut == 0) {
                Warning("FillHistos","=> NO ZERO SUPPRESION CUT sigmaLevel = %d not set\n",fZeroSupCut) ;
                continue ;
            }
            
            TString detector1 = fMapping->GetDetectorFromPlane(fDetectorPlane1[histoName]) ;
            TString detector2 = fMapping->GetDetectorFromPlane(fDetectorPlane2[histoName]) ;
            
            if(!(eventbuilder->IsAGoodEvent())) {
                fNbOfEvents[histoName] -= 1 ;
                continue ;
            }
            
            SetHistoTitle(histoName, fNbOfEvents[histoName], triggerCount) ;
            map < Int_t, vector <Float_t > > detectorEvent1 = eventbuilder->GetDetectorCluster(detector1) ;
            map < Int_t, vector <Float_t > > detectorEvent2 = eventbuilder->GetDetectorCluster(detector2) ;
            
            Int_t clusterMult1 = detectorEvent1.size() ;
            Int_t clusterMult2 = detectorEvent2.size() ;
            
            if(clusterMult1 != clusterMult2) {
                fNbOfEvents[histoName] -= 1 ;
                detectorEvent1.clear() ;
                detectorEvent2.clear() ;
                continue ;
            }
            
            for (Int_t k = 0 ; k < clusterMult1 ; k++) {
                if( (histoName.Contains("timing")) || (histoName.Contains("Timing")) ) {
                    ((TH1F*) fObjToDraw[histoName])->SetXTitle("x-strips: Time bin signal's peak") ;
                    ((TH1F*) fObjToDraw[histoName])->SetYTitle("y-strips: Time bin signal's peak") ;
                    ((TH1F*) fObjToDraw[histoName])-> SetZTitle("Counts") ;
                    
                    Float_t x1t = detectorEvent1[k][4] ;
                    Float_t y1t = detectorEvent1[k][5] ;
                    Float_t x2t = detectorEvent2[k][4] ;
                    Float_t y2t = detectorEvent2[k][5] ;
                    
                    if( (fDetectorPlane1[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector1)).front() ) && (fDetectorPlane2[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector2)).front()) ) {
                        ((TH1F*) fObjToDraw[histoName])->Fill(x1t-x2t) ;
                    }
                    if( (fDetectorPlane1[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector1)).front() ) && (fDetectorPlane2[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector2)).back()) ) {
                        ((TH1F*) fObjToDraw[histoName])->Fill(x1t-y2t) ;
                    }
                    if( (fDetectorPlane1[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector1)).back() ) && (fDetectorPlane2[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector2)).front()) ) {
                        ((TH1F*) fObjToDraw[histoName])->Fill(y1t-x2t) ;
                    }
                    if( (fDetectorPlane1[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector1)).back() ) && (fDetectorPlane2[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector2)).back()) ) {
                        ((TH1F*) fObjToDraw[histoName])->Fill(y1t-y2t) ;
                    }
                    detectorEvent1[k].clear() ;
                    detectorEvent2[k].clear() ;
                }
                
                else {
                    Float_t x1 = detectorEvent1[k][0] ;
                    Float_t y1 = detectorEvent1[k][1] ;
                    Float_t x2 = detectorEvent2[k][0] ;
                    Float_t y2 = detectorEvent2[k][1] ;
                    
                    TString readoutBoard = fMapping->GetReadoutBoardFromDetector(fDetector[histoName].Data()) ;
                    TString detectorType = fMapping->GetDetectorTypeFromDetector(fDetector[histoName].Data()) ;
                    if ( (readoutBoard == "UV_ANGLE") &&  (detectorType == "EICPROTO1") ) {
                        ((TH1F*) fObjToDraw[histoName])->SetBins(400, -500, 500, 200, -250, 250) ;
                    }
                    
                    ((TH1F*) fObjToDraw[histoName])-> SetZTitle("Counts") ;
                    
                    if( (fDetectorPlane1[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector1)).front() ) && (fDetectorPlane2[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector2)).front()) ) {
                        x1 = x1 + track->GetDetXOffset()[detector1] ;
                        x2 = x2 + track->GetDetXOffset()[detector2] ;
                        ((TH1F*) fObjToDraw[histoName])->Fill(x1-x2) ;
                    }
                    
                    if( (fDetectorPlane1[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector1)).front() ) && (fDetectorPlane2[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector2)).back()) ) {
                        x1 = x1 + track->GetDetXOffset()[detector1] ;
                        y2 = y2 + track->GetDetYOffset()[detector2] ;
                        ((TH1F*) fObjToDraw[histoName])->Fill(x1-y2) ;
                    }
                    
                    if( (fDetectorPlane1[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector1)).back() ) && (fDetectorPlane2[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector2)).front()) ) {
                        y1 = y1 + track->GetDetYOffset()[detector1] ;
                        x2 = x2 + track->GetDetXOffset()[detector2] ;
                        ((TH1F*) fObjToDraw[histoName])->Fill(y1-x2) ;
                    }
                    
                    if( (fDetectorPlane1[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector1)).back() ) && (fDetectorPlane2[histoName] == (fMapping->GetDetectorPlaneListFromDetector(detector2)).back()) ) {
                        y1 = y1 + track->GetDetYOffset()[detector1] ;
                        y2 = y2 + track->GetDetYOffset()[detector2] ;
                        ((TH1F*) fObjToDraw[histoName])->Fill(y1-y2) ;
                    }
                    detectorEvent1[k].clear() ;
                    detectorEvent2[k].clear() ;
                }
            }
            detectorEvent1.clear() ;
            detectorEvent2.clear() ;
        }
        
        //=========================================================================================================================//
        //                                          CMS GEM HITMAP, CHARGE SHARING                                                 //
        //=========================================================================================================================//
        if ( fVarType[histoName] == "CMSHITMAP") {
            if (fZeroSupCut == 0) {
                Warning("FillHistos","=> NO ZERO SUPPRESION CUT sigmaLevel = %d not set\n",fZeroSupCut) ;
                continue ;
            }
            
            SetHistoTitle(histoName, fNbOfEvents[histoName], triggerCount) ;
            map < TString, list <SRSCluster * > >::const_iterator detPlane_itr ;
            for(detPlane_itr = clustersInDetectorPlaneMap.begin(); detPlane_itr != clustersInDetectorPlaneMap.end(); ++detPlane_itr ) {
                TString detPlaneName = (* detPlane_itr).first ;
                Float_t clusterEtaPos =   62.5 ;
                
                TString detector = fMapping->GetDetectorFromPlane(detPlaneName) ;
                TString readoutBoard = fMapping->GetReadoutBoardFromDetector(detector) ;
                TString detectorType = fMapping->GetDetectorTypeFromDetector(detector) ;
                
                if ( (readoutBoard == "CMSGEM") &&  (detectorType == "CMSGEM") ) {
                    clusterEtaPos = fMapping->GetPlaneIDorEtaSector(detPlaneName) ;
                    list <SRSCluster * > listOfClusters = (* detPlane_itr).second ;
                    list <SRSCluster * >::const_iterator cluster_itr ;
                    for(cluster_itr = listOfClusters.begin(); cluster_itr != listOfClusters.end(); ++cluster_itr ) {
                        SRSCluster * cluster = * cluster_itr ;
                        
                        Float_t clusterXPos = clusterEtaPos ;
                        Float_t clusterYPos = cluster->GetClusterPosition() ;
                        Float_t clusterADCcounts = cluster->GetClusterADCs() ;
                        
                        ((TH2F*) fObjToDraw[histoName])->SetXTitle("Hit Cluster Position in X (mm)") ;
                        ((TH2F*) fObjToDraw[histoName])->SetYTitle("Hit Cluster Position in Y (mm)") ;
                        
                        if ( (histoName.Contains("adc2D")) || (histoName.Contains("ADC2D")) || (histoName.Contains("Adc2D")) ) {
                            ((TH2F*) fObjToDraw[histoName])->SetZTitle("mean ADC counts") ;
                            ((TH2F*) fObjToDraw[histoName])->Fill(clusterXPos,clusterYPos,clusterADCcounts) ;
                        }
                        else {
                            ((TH2F*) fObjToDraw[histoName])->SetZTitle("hit counts") ;
                            ((TH2F*) fObjToDraw[histoName])->Fill(clusterXPos,clusterYPos) ;
                        }
                    }
                    listOfClusters.clear() ;
                }
            }
        }
    }
    DeleteHitsInDetectorPlaneMap(hitsInDetectorPlaneMap) ;
    DeleteClustersInDetectorPlaneMap(clustersInDetectorPlaneMap) ;
}

//===============================================================================================================================
void SRSHistoManager::ReadHistoCfgFile(const char * histoCfgname){
    
    //printf("  SRSHistoManager::ReadHistoCfgFile() ==> Opening Histo Cfg file: %s\n", histoCfgname);
    ifstream file (histoCfgname, ifstream::in);
    
    Int_t iline = 0;
    TString detector1, detector2, plane1, plane2, fecNoStr, fecChannelStr, type, line, name, title, fitParam1Str, fitParam2Str, fitFunction ;
    TString histoNameGraph, SizeX, SizeY, SizeZ ;
    TString xOffset, yOffset, zOffset, invertCoordinate ;
    
    while (line.ReadLine(file)) {
        
        //=== reset flags
        Bool_t is1D = kFALSE, is2D = kFALSE, isRawData = kFALSE, isTimeBin = kFALSE, isFit = kFALSE, isCMSGEM = kFALSE;
        Bool_t isPlaneTimeBin = kFALSE, isSpectrum = kFALSE, isOneDetPlane = kFALSE, isTwoPlanes = kFALSE, isTwoDetectors = kFALSE, is2dDetector = kFALSE;
        
        iline++;
        Int_t column = 0;
        line.Remove(TString::kLeading, ' ');   // strip leading spaces
        if (line.BeginsWith("#")) continue ;   // and skip comments
        if (line.BeginsWith("@")) continue ;   // and skip comments
        
        //    printf("   SRSHistoManager::ReadHistoCfgFile() ==> Scanning the histo cfg file %s\n",line.Data()) ;
        TObjArray * tokens = line.Tokenize(","); // Create an array of the tokens separated by "," in the line; lines without "," are skipped
        TIter myiter(tokens);                    // iterator on the tokens array
        
        while (TObjString * st = (TObjString*) myiter.Next()) {  // inner loop (loop over the line)
            TString s = st->GetString().Remove(TString::kLeading, ' ' ); // remove leading spaces
            s.Remove(TString::kTrailing, ' ' );                          // remove trailing spaces
            
            //      printf("     SRSHistoManager::ReadHistoCfgFile() ==> Data ==> %s\n",s.Data()) ;
            if (column == 0) {
                
                if (s == "BINNING") { // Reset the binning of the histos
                    TString bintype = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    
                    if (bintype != "CLUSTERDIST") {
                        Int_t nbin      = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                        Float_t min     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                        Float_t max     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                        Reset1DHistBinning(bintype, nbin, min, max);
                    }
                    
                    else {
                        //	    printf("     SRSHistoManager::ReadHistoCfgFile() ==> binType ==> %s\n",bintype.Data()) ;
                        TString nbinStr = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                        TString minStr  = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                        TString maxStr  = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                        
                        Int_t nbin  = 99099;
                        Float_t min = 99099.0;
                        Float_t max = 99099.0;
                        
                        if(nbinStr != "INF") nbin = nbinStr.Atoi() ;
                        if(minStr  != "INF") min = minStr.Atof();
                        if(maxStr  != "INF") max = maxStr.Atof();
                        Reset1DHistBinning(bintype, nbin, min, max);
                    }
                    continue;
                }
                
                else if (s == "BINNING2D") { // Reset the binning of the histos
                    TString bintype  = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    TString nbinStr  = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    TString minStr   = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    TString maxStr   = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    TString nbin2Str = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    TString min2Str  = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    TString max2Str  = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    
                    Int_t nbin   = 99099;
                    Int_t nbin2  = 99099;
                    Float_t min  = 99099.0;
                    Float_t max  = 99099.0;
                    Float_t min2 = 99099.0;
                    Float_t max2 = 99099.0;
                    
                    if(nbinStr  != "INF") nbin  = nbinStr.Atoi() ;
                    if(nbin2Str != "INF") nbin2 = nbin2Str.Atoi() ;
                    if(minStr   != "INF") min   = minStr.Atof();
                    if(min2Str  != "INF") min2  = min2Str.Atof();
                    if(maxStr   != "INF") max   = maxStr.Atof();
                    if(max2Str  != "INF") max2  = max2Str.Atof();
                    Reset2DHistBinning(bintype, nbin, min, max, nbin2, min2, max2);
                    continue;
                }
                
                else if (s == "HITTIMING") {
                    BookHitTimingHisto() ;
                }
                
                else if ((s == "HITPEDOFFSET") || (s == "HITCOMMODE") ||  (s == "NOISERESO") || (s == "HITRAWDATA") || (s == "CLUSTSTAT") || (s == "HITDIST")  || (s == "CLUSTERDIST") || (s == "HITZEROSUP") || (s == "CLUSTSIZE") || (s == "CLUSTMULT") || (s == "ADCDIST") || (s == "TIMESLICE") || (s == "OCCUPANCY") || (s == "CLUSTERTIMEBIN") || (s == "HITCROSSTALK") || (s == "CHARGECROSSTALK") ||  (s == "SPECTRUM") ) {
                    is1D          = kTRUE ;
                    isOneDetPlane = kTRUE ;
                }
                
                else if ( (s == "HITMAP") || (s == "CHARGES_SH") || (s == "CHARGES_RATIODIST") ){
                    is2D         = kTRUE ;
                    is2dDetector = kTRUE ;
                }
                
                else if (s == "CORRELATION") {
                    is2D        = kTRUE ;
                    isTwoPlanes = kTRUE ;
                }
                
                else if ( s == "CMSHITMAP"){
                    isCMSGEM = kTRUE ;
                }
                
                else if ( (s == "OLD_OFFSETX") || (s == "OLD_OFFSETY") || (s == "RELATIVE_OFFSET") ) {
                    is1D        = kTRUE ;
                    isTwoPlanes = kTRUE ;
                }
                
                
                else if (s == "CHARGES_RATIO")  {
                    is1D          = kTRUE ;
                    is2dDetector = kTRUE ;
                }
                
                else if (s == "PLANETIMEBIN" )  {
                    is2D           = kTRUE ;
                    isPlaneTimeBin = kTRUE ;
                    isOneDetPlane  = kTRUE ;
                }
                
                else if (s == "DIVIDEDHISTOS") {
                    TString firstHisto  = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    TString secondHisto = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    fDividedHistos[firstHisto] = secondHisto ;
                    fHistosRatioCut   = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi() ;
                }
                
                else if (s == "FIT") {
                    TString histo = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    TString typeOfFit = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    TString firstParam = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    TString secondParam = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    fFittedHistos[histo].push_back(typeOfFit);
                    fFittedHistos[histo].push_back(firstParam);
                    fFittedHistos[histo].push_back(secondParam);
                }
                
                else {
                    Error("SRSHistoManager","ReadHistoCfgFile:=> Error in configuration line %s: unknown command", line.Data());
                    continue;
                }
                // Get the Type
                type = s;
            }
            
            //=== CONFIG FILE ERRORS
            if ( (isCMSGEM) && (column > 3)) {
                Error("SRSHistoManager","ReadHistoCfgFile:=> Error in configuration %s: histo line not the right length", line.Data());
                continue;
            }
            if ( (isOneDetPlane || isPlaneTimeBin) && (column > 4)) {
                Error("SRSHistoManager","ReadHistoCfgFile:=> Error in configuration %s: histo line not the right length", line.Data());
                continue;
            }
            
            if (isTwoPlanes && (column > 5)) {
                Error("SRSHistoManager","ReadHistoCfgFile:=> Error in configuration %s: histo line too long", line.Data()) ;
                continue ;
            }
            
            if (is2dDetector && (column > 4)) {
                Error("SRSHistoManager","ReadHistoCfgFile:=> Error in configuration %s: histo line too long", line.Data()) ;
                continue ;
            }
            
            if (isFit && (column > 6)) {
                Error("SRSHistoManager","ReadHistoCfgFile:=> Error in configuration %s: histo line too long", line.Data()) ;
                continue ;
            }
            
            switch (column) {
                case 1:
                    name = s;
                    break;
                    
                case 2:
                    title = s;
                    break;
                    
                case 3:
                    if (isTimeBin)           fecNoStr  = s;
                    else if (isFit)          fitParam1Str = s;
                    else if (is2dDetector)   detector1 = s;
                    else                     plane1 = s;
                    break;
                    
                case 4:
                    if (isTimeBin)           fecChannelStr = s;
                    else if (isFit)          fitParam2Str  = s;
                    else                     plane2 = s;
                    break;
                    
                case 5:
                    fitFunction = s;
                    break;
            }
            column++;
        } // End While : Inner loop (loop over the line)
        
        //=== CONFIG FILE ERRORS
        if ( (isCMSGEM) && (column != 3)) {
            Error("SRSHistoManager","ReadHistoCfgFile:=> Error in configuration %s: histo line not the right length", line.Data());
            continue;
        }
        if ( (isOneDetPlane || isPlaneTimeBin) && (column != 4)) {
            Error("SRSHistoManager","ReadHistoCfgFile:=> Error in configuration %s: histo line not the right length", line.Data());
            continue;
        }
        
        if ( (isTwoPlanes|| isTwoDetectors)  && (column != 5)) {
            Error("SRSHistoManager","ReadHistoCfgFile:=> Error in configuration %s: histo line too long", line.Data()) ;
            continue ;
        }
        
        if (is2dDetector && (column != 4)) {
            Error("SRSHistoManager","ReadHistoCfgFile:=> Error in configuration %s: histo line too long", line.Data()) ;
            continue ;
        }
        
        if (isFit&& (column != 6)) {
            Error("SRSHistoManager","ReadHistoCfgFile:=> Error in configuration %s: histo line too long", line.Data()) ;
            continue ;
        }
        tokens->Delete();
        
        //===================================================================================================
        // BOOKING THE HISTOGRAMS AND UPDATING HE RECORDS
        //******************************************************************
        if (isCMSGEM)               BookCMSHisto(name, title, type) ;
        if (is1D && isOneDetPlane)  Book1DHisto(name, title, type, plane1);
        if (is1D && isTwoPlanes)    Book1DHisto(name, title, type, plane1, plane2);
        if (is2D && isTwoPlanes)    Book2DHisto(name, title, type, plane1, plane2);
        if (is1D && is2dDetector)   Book1DHisto(name, title, type, detector1) ;
        if (is2D && is2dDetector)   Book2DHisto(name, title, type, detector1) ;
    }
}

//==================================================================================
void SRSHistoManager::SaveAllHistos() {
    //  printf("   SRSHistoManager::SaveAllHistos() => Entering ======== \n" ) ;
    TString fileName = fRunName + "_" + fRunFilePrefix + fRunFileValue + "_SRS" + fAmoreAgentID + ".root";
    
    TFile * file = new TFile(fileName,"recreate");
    if (!file->IsOpen()){
        Error("SRSHistoManager","SaveHistos:=> Cannot open file %s", fileName.Data());
    }
    
    map<TString, TString>::const_iterator ratio_itr ;
    for (ratio_itr = fDividedHistos.begin(); ratio_itr != fDividedHistos.end(); ++ratio_itr) {
        
        TString firsthistoname = (*ratio_itr).first ;
        TString secondhistoname = (*ratio_itr).second ;
        
        TH2F* histo1 =((TH2F*) fObjToDraw[firsthistoname]) ;
        TH2F* histo2 =((TH2F*) fObjToDraw[secondhistoname]) ;
        
        Int_t nbOfBinsX = histo1->GetNbinsX() ;
        Int_t nbOfBinsY = histo1->GetNbinsY() ;
        
        if (! histo1) {
            printf("   SRSHistoManager::SaveAllHistos() WARNING !!!! histo1 = %s  do not exist for fit \n", firsthistoname.Data()) ;
            printf("   SRSHistoManager::SaveAllHistos() WARNING !!!! PLEASE CHECK YOUR HISTO CONFIG FILE \n") ;
            continue ;
        }
        
        if (! histo2) {
            printf("   SRSHistoManager::SaveAllHistos() WARNING !!!! histo2 = %s  do not exist for fit \n", secondhistoname.Data()) ;
            printf("   SRSHistoManager::SaveAllHistos() WARNING !!!! PLEASE CHECK YOUR HISTO CONFIG FILE \n") ;
            continue ;
        }
        
        for (Int_t ii = 0; ii < nbOfBinsX; ii++) {
            for (Int_t jj = 0; jj < nbOfBinsY; jj++) {
                Int_t binx = ii + 1 ;
                Int_t biny = jj + 1 ;
                Int_t binContent = ((Int_t ) histo2->GetBinContent(binx,biny)) ;
                if (binContent < fHistosRatioCut ) histo1->SetBinContent(binx,biny,0) ;
            }
        }
        histo1->Divide(histo2) ;
    }
    
    map<TString, vector<TString> >::const_iterator fit_itr ;
    for (fit_itr = fFittedHistos.begin(); fit_itr != fFittedHistos.end(); ++fit_itr) {
        TString histoname = (*fit_itr).first ;
        
        TH1F* histo =((TH1F*) fObjToDraw[histoname]) ;
        if (! histo) {
            printf("   SRSHistoManager::SaveAllHistos() WARNING !!!! histo = %s  do not exist for fit \n", histoname.Data()) ;
            printf("   SRSHistoManager::SaveAllHistos() WARNING !!!! PLEASE CHECK YOUR HISTO CONFIG FILE \n") ;
            continue ;
        }
        
        vector<TString> fitParameter = (*fit_itr).second ;
        TString typeOfFit = fitParameter[0] ;
        Float_t start = fitParameter[1].Atof() ;
        Float_t end = fitParameter[2].Atof() ;
        printf("   SRSHistoManager::SaveAllHistos() typeOfFit  = %s, start = %f, end = %f \n", typeOfFit.Data(), start, end) ;
        
        if (histo->GetEffectiveEntries() < 256) {
            printf("   SRSHistoManager::SaveAllHistos(): Statistic of histo = %s is too low for the %s fit \n", histoname.Data(), typeOfFit.Data()) ;
            continue ;
        }
        
        if ((typeOfFit == "Gauss")  || (typeOfFit == "gauss") || (typeOfFit == "Gaussian")  || (typeOfFit == "gaussian")) {
            GaussFit (histo) ;
            //      GaussFit (histo, start, end) ;
        }
        if ((typeOfFit == "Landau") || (typeOfFit == "landau") ) {
            LandauFit (histo) ;
        }
        if ((typeOfFit == "DoubleGauss") || (typeOfFit == "doubleGauss") || (typeOfFit == "doublegauss")) {
            DoubleGaussFit(histo, 2, 3., kTRUE) ;
        }
    }
    
    map<TString, TObject*>::const_iterator itr;
    for (itr = fObjToDraw.begin(); itr != fObjToDraw.end(); ++itr) {
        TString name = (*itr).first;
        
        if (fVarType[name] == "RELATIVE_OFFSET") continue ;
        
        if (fObjToDraw[name]) {
            if ((fVarType[name] == "HITMAP") ||(fVarType[name] == "CMSHITMAP") || (fVarType[name] == "CHARGESMAP")) {
                if (! file->GetDirectory("HitMap") ) file->mkdir("HitMap") ;
                file->cd() ;
                gDirectory->cd("HitMap");
                //SaveRunHistos(fObjToDraw[name], kFALSE) ;
                fObjToDraw[name]->Write() ;
            }
            
            if (fVarType[name] == "RAWDATA")  {
                if (! file->GetDirectory("RawData") ) file->mkdir("RawData") ;
                file->cd() ;
                gDirectory->cd("RawData");
                //SaveRunHistos(fObjToDraw[name], kFALSE) ;
                fObjToDraw[name]->Write() ;
            }
            
            if ( (fVarType[name] == "HITCROSSTALK") || (fVarType[name] == "CHARGECROSSTALK") ) {
                if (! file->GetDirectory("Crostalk") ) file->mkdir("Crostalk") ;
                file->cd() ;
                gDirectory->cd("Crostalk");
                //SaveRunHistos(fObjToDraw[name], kFALSE) ;
                fObjToDraw[name]->Write() ;
            }
            
            if (fVarType[name] == "PEDESTALS") {
                if (! file->GetDirectory("Pedestals") ) file->mkdir("Pedestals") ;
                file->cd() ;
                gDirectory->cd("Pedestals");
                //SaveRunHistos(fObjToDraw[name], kTRUE) ;
                Info ("SRSHistoManager","SaveHistos:=> Histo %s saved", name.Data()) ;
                fObjToDraw[name]->Write() ;
            }
            
            if (fVarType[name] == "HITDIST") {
                if (! file->GetDirectory("HitDist") ) file->mkdir("HitDist") ;
                file->cd() ;
                gDirectory->cd("HitDist");
                //SaveRunHistos(fObjToDraw[name], kFALSE) ;
                fObjToDraw[name]->Write() ;
            }
            
            if (fVarType[name] == "CLUSTERDIST") {
                if (! file->GetDirectory("ClusterDist") ) file->mkdir("ClusterDist") ;
                file->cd() ;
                gDirectory->cd("ClusterDist");
                //SaveRunHistos(fObjToDraw[name], kFALSE) ;
                fObjToDraw[name]->Write() ;
            }
            
            if ( (fVarType[name] == "CLUSTSIZE") || (fVarType[name] == "CLUSTMULT") || (fVarType[name] == "OCCUPANCY")){
                if (! file->GetDirectory("Cluster") ) file->mkdir("Cluster") ;
                file->cd() ;
                gDirectory->cd("Cluster");
                //SaveRunHistos(fObjToDraw[name], kTRUE) ;
                fObjToDraw[name]->Write() ;
            }
            
            if (fVarType[name] == "TIMESLICE") {
                if (! file->GetDirectory("Timing") ) file->mkdir("Timing") ;
                file->cd() ;
                gDirectory->cd("Timing");
                //SaveRunHistos(fObjToDraw[name], kTRUE) ;
                fObjToDraw[name]->Write() ;
            }
            
            if (fVarType[name] == "CORRELATION") {
                if (! file->GetDirectory("Correlation") ) file->mkdir("Correlation") ;
                file->cd() ;
                gDirectory->cd("Correlation");
                //SaveRunHistos(fObjToDraw[name], kTRUE) ;
                fObjToDraw[name]->Write() ;
            }
            
            if ((fVarType[name] == "CHARGES_SH") || (fVarType[name] == "CHARGES_RATIO") ||  (fVarType[name] == "CHARGES_RATIODIST")) {
                if (! file->GetDirectory("ChargeSharing") ) file->mkdir("ChargeSharing") ;
                file->cd() ;
                gDirectory->cd("ChargeSharing");
                //SaveRunHistos(fObjToDraw[name], kFALSE) ;
                fObjToDraw[name]->Write() ;
            }
            
            if (fVarType[name] == "NOISERESO") {
                if (! file->GetDirectory("NoiseReso") ) file->mkdir("NoiseReso") ;
                file->cd() ;
                gDirectory->cd("NoiseReso");
                //SaveRunHistos(fObjToDraw[name], kTRUE) ;
                fObjToDraw[name]->Write() ;
            }
            
            if ( (fVarType[name] == "ADCDIST") || (fVarType[name] == "SPECTRUM")) {
                if (! file->GetDirectory("adcDist") ) file->mkdir("adcDist") ;
                file->cd() ;
                gDirectory->cd("adcDist");
                //SaveRunHistos(fObjToDraw[name], kFALSE) ;
                fObjToDraw[name]->Write() ;
            }
            
        }
    }
    file->Close();
}

//==================================================================================
void SRSHistoManager::SaveFTBF_PlaneRotationROOT(SRSTrack * track) {
    printf("   SRSHistoManager::SaveFTBF_PlaneRotationROOT()=> Entering ===== \n" ) ;
    
    TString planeRotationfileName = fRunName + "xyPlaneRotation.root" ;
    //  if (fRunFilePrefix == "BeamPosition") planeRotationfileName = fRunName + "xyPlaneRotation_P"  + fRunFileValue + ".root";
    //  if (fRunFilePrefix == "HVScan")       planeRotationfileName = fRunName + "xyPlaneRotation_HV" + fRunFileValue + ".root";
    
    Int_t runId = fRunFileValue.Atoi() + fAmoreAgentID.Atoi() ;
    stringstream runIdStrStr ;
    runIdStrStr << runId;
    TString runIdStr = runIdStrStr.str() ;
    
    if (fRunFilePrefix.Contains("BeamPos")) planeRotationfileName = fRunName + "xyPlaneRotation_P"  + runIdStr + ".root";
    if (fRunFilePrefix.Contains("HVScan"))  planeRotationfileName = fRunName + "xyPlaneRotation_HV" + runIdStr + ".root";
    
    
    TFile * planeRotationfile = new TFile(planeRotationfileName,"recreate") ;
    if (!planeRotationfile->IsOpen()){
        Error("SRSHistoManager","SaveFTBF_PlaneRotationROOT()=> Cannot open file %s", planeRotationfileName.Data());
    }
    
    //=== Create Folder
    map<TString, TH1*>::const_iterator itr;
    for (itr = fResidualHistos.begin(); itr != fResidualHistos.end(); ++itr) {
        TString name = (*itr).first ;
        if ((fResidualHistos[name])->GetEffectiveEntries() < 100 ) {
            printf("   SRSHistoManager::SaveFTBF_PlaneRotationROOT()=> Statistic of histo = %s is too low (<100) for the DoubleGaussFit \n", name.Data()) ;
            continue ;
        }
        GaussFit( (TH1F*) fResidualHistos[name]) ;
        //    DoubleGaussFit( (TH1F*) fResidualHistos[name], 2, 3, kTRUE) ;
        
        if (!name.Contains("rotation")) continue ;
        planeRotationfile->cd() ;
        fResidualHistos[name]->Write() ;
    }
    planeRotationfile->Close() ;
}



//==================================================================================
void SRSHistoManager::SaveFTBF_XYOffsetsROOT(SRSTrack * track) {
    printf("   SRSHistoManager::SaveFTBF_XYOffsetsROOT()=> Entering ======== \n" ) ;
    TString offsetfileName = fRunName + "xyOffset.root" ;
    
    Int_t runId = fRunFileValue.Atoi() + fAmoreAgentID.Atoi() ;
    stringstream runIdStrStr ;
    runIdStrStr << runId;
    TString runIdStr = runIdStrStr.str() ;
    
    if (fRunFilePrefix.Contains("BeamPos")) offsetfileName = fRunName + "xyOffset_P"  + runIdStr + ".root";
    if (fRunFilePrefix.Contains("HVScan"))  offsetfileName = fRunName + "xyOffset_HV" + runIdStr + ".root";
    
    printf("   SRSHistoManager::SaveFTBF_XYOffsetsROOT()=> file Prefix = %s, output data file = %s \n",fRunFilePrefix.Data(), offsetfileName.Data()) ;
    TFile * offsetfile = new TFile(offsetfileName,"recreate") ;
    if (!offsetfile->IsOpen()){
        Error("SRSHistoManager","SaveFTBF_XYOffsetsROOT()=> Cannot open file %s", offsetfileName.Data());
    }
    
    //=== Create Folder
    map<TString, TH1*>::const_iterator itr;
    for (itr = fResidualHistos.begin(); itr != fResidualHistos.end(); ++itr) {
        TString name = (*itr).first ;
        if (!name.Contains("offset")) continue ;
        if ((fResidualHistos[name])->GetEffectiveEntries() < 100 ) {
            printf("   SRSHistoManager::SaveFTBF_XYOffsetsROOT()=> Statistic of histo = %s is too low (<100) for the DoubleGaussFit \n", name.Data()) ;
            continue ;
        }
        GaussFit( (TH1F*) fResidualHistos[name]) ;
        //    DoubleGaussFit( (TH1F*) fResidualHistos[name], 2, 3, kTRUE) ;
        offsetfile->cd() ;
        fResidualHistos[name]->Write() ;
    }
    offsetfile->Close() ;
}

//==================================================================================
void SRSHistoManager::SaveFTBF_ResidualsROOT(SRSTrack * track) {
    printf("   SRSHistoManager::SaveFTBF_ResidualsROOT()=> Entering ===== \n" ) ;
    Int_t runId = fRunFileValue.Atoi() + fAmoreAgentID.Atoi() ;
    stringstream runIdStrStr ;
    runIdStrStr << runId;
    TString fileName = fRunName +  "_" + fRunFilePrefix + "_Run" + runIdStrStr.str() + "_Residuals.root" ;
    
    TFile * file = new TFile(fileName,"recreate");
    
    if (!file->IsOpen()){
        Error("SRSHistoManager","SaveFTBF_ResidualsROOT()=> Cannot open file %s", fileName.Data());
    }
    
    //=== Create Folder
    if (! file->GetDirectory("Tracking")) file->mkdir("Tracking") ;
    map<TString, TH1*>::const_iterator itr;
    for (itr = fResidualHistos.begin(); itr != fResidualHistos.end(); ++itr) {
        TString name = (*itr).first ;
        if ((fResidualHistos[name])->GetEffectiveEntries() < 100 ) {
            printf("   SRSHistoManager::SaveFTBF_ResidualsROOT()=> Statistic of histo = %s is too low (<100) for the DoubleGaussFit \n", name.Data()) ;
            continue ;
        }
        
        // GaussFit( (TH1F*) fResidualHistos[name]) ;
        //    DoubleGaussFit( (TH1F*) fResidualHistos[name], 8, 3, kTRUE) ;
        
        if(name.Contains("residuals")) DoubleGaussFit( (TH1F*) fResidualHistos[name], 8, 3, kTRUE) ;
        //    if(name.Contains("residuals")) DoubleGaussFit( (TH1F*) fResidualHistos[name], 8, 3, kTRUE) ;
        else                            GaussFit( (TH1F*) fResidualHistos[name]) ;
        
        file->cd() ;
        gDirectory->cd("Tracking");
        fResidualHistos[name]->Write() ;
    }
    file->Close() ;
}

//==================================================================================
void SRSHistoManager::SaveFTBF_ResidualsTXT(SRSTrack * track) {
    
    TString residualfilename = fRunName + fRunFilePrefix + "_Residuals.txt";
    
    
    Int_t runId = fRunFileValue.Atoi() + fAmoreAgentID.Atoi() ;
    
    const char *file = residualfilename.Data() ;
    FILE * f = fopen(file, "a+");
    printf("  SRSHistoManager::SaveFTBF_ResidualsTXT()=> residuals file [%s] created \n", file) ;
    
    TString rotationfilename = fRunName ;
    rotationfilename.Append("_Rotation.txt") ;
    const char *fileRot = rotationfilename.Data() ;
    FILE * f1 = fopen(fileRot, "a+");
    printf("  SRSHistoManager::SaveFTBF_ResidualsTXT()=> to rotation file [%s] created\n", fileRot) ;
    
    TString offsetfilename = fRunName ;
    offsetfilename.Append("_xyOffset.txt") ;
    const char *fileOffset = offsetfilename.Data() ;
    FILE * f2 = fopen(fileOffset, "a+");
    printf("  SRSHistoManager::SaveFTBF_ResidualsTXT()=> to offset file [%s] created\n", fileOffset) ;
    
    map <TString, TString > detectorList =  track->GetDetectorList() ;
    map <TString, TString >::const_iterator det_itr ;
    for(det_itr = detectorList.begin(); det_itr != detectorList.end(); ++det_itr) {
        TString detName = (*det_itr).first ;
        Int_t detId = 1 ;
        if(detName == "EIC1")     detId = 1 ;
        if(detName == "SBSGEM1")  detId = 2 ;
        if(detName == "SBSGEM2")  detId = 3 ;
        if(detName == "TrkGEM0")  detId = 4 ;
        if(detName == "TrkGEM1")  detId = 5 ;
        if(detName == "TrkGEM2")  detId = 6 ;
        
        map<TString, Float_t> xOffset = track->GetDetXOffset() ;
        map<TString, Float_t> yOffset = track->GetDetYOffset() ;
        map<TString, Float_t> zOffset = track->GetDetZOffset() ;
        map<TString, Float_t> rotationAlongZ = track->GetDetPlaneRotationCorrection() ;
        
        Float_t XOffset = xOffset[detName] ;
        Float_t YOffset = yOffset[detName] ;
        Float_t ZOffset = zOffset[detName] ;
        Float_t RotationAlongZ = rotationAlongZ[detName] ;
        
        TString planeX = (fMapping->GetDetectorPlaneListFromDetector(detName)).front() ;
        TString planeY = (fMapping->GetDetectorPlaneListFromDetector(detName)).back() ;
        
        TString xresName   = "residuals" + planeX ;
        TString yresName   = "residuals" + planeY ;
        TString topresName = "residuals" + planeX ;
        TString botresName = "residuals" + planeY ;
        TString rresName   = "residuals" + planeX ;
        TString phiresName = "residuals" + planeY ;
        
        if(detName == "EIC1") {
            xresName   = "residuals" + detName + "X"  ;
            yresName   = "residuals" + detName + "Y"  ;
            rresName   = "residuals" + detName + "R"  ;
            phiresName = "residuals" + detName + "PHI"  ;
        }
        
        Float_t x_residualRMS   = 0.0 ;
        Float_t y_residualRMS   = 0.0 ;
        Float_t top_residualRMS = 0.0 ;
        Float_t bot_residualRMS = 0.0 ;
        
        Float_t r_residualRMS = 0.0 ;   //  printf("   SRSHistoManager::DoubleGaussianFit(): RChi2  = %f, const2 = %f, mean2 = %f, meanError2 = %f, sigma2 = %f, sigma_error2 = %f\n", RChi2, const2, mean2, mean_err2, sigma2, sigma_err2) ;
        
        Float_t phi_residualRMS = 0.0 ;
        
        Float_t error_xRMS   = 0.0 ;
        Float_t error_yRMS   = 0.0 ;
        Float_t error_topRMS = 0.0 ;
        Float_t error_botRMS = 0.0 ;
        Float_t error_rRMS = 0.0 ;
        Float_t error_phiRMS = 0.0 ;
        
        Int_t nEntries = 0 ;
        
        TF1 * xFit   = ((TH1F*) fResidualHistos[xresName])->GetFunction("fitFunction") ;
        TF1 * yFit   = ((TH1F*) fResidualHistos[yresName])->GetFunction("fitFunction") ;
        TF1 * topFit = ((TH1F*) fResidualHistos[topresName])->GetFunction("fitFunction") ;
        TF1 * botFit = ((TH1F*) fResidualHistos[botresName])->GetFunction("fitFunction") ;
        TF1 * rFit   = ((TH1F*) fResidualHistos[rresName])->GetFunction("fitFunction") ;
        TF1 * phiFit = ((TH1F*) fResidualHistos[phiresName])->GetFunction("fitFunction") ;
        
        if(xFit) {
            x_residualRMS   = (Float_t) (xFit->GetParameter("Sigma")) ;
            Int_t nEntries = ((Int_t) ((TH1F*) fResidualHistos[xresName])->GetEntries()) ;
            error_xRMS = x_residualRMS / sqrt(nEntries) ;
        }
        if(yFit) {
            y_residualRMS   = (Float_t) (yFit->GetParameter("Sigma")) ;
            Int_t nEntries = ((Int_t) ((TH1F*) fResidualHistos[yresName])->GetEntries()) ;
            error_yRMS = y_residualRMS / sqrt(nEntries) ;
        }
        if(topFit) {
            top_residualRMS = (Float_t) (topFit->GetParameter("Sigma")) ;
            Int_t nEntries = ((Int_t) ((TH1F*) fResidualHistos[topresName])->GetEntries()) ;
            error_topRMS = top_residualRMS / sqrt(nEntries) ;
        }
        if(botFit) {
            bot_residualRMS = (Float_t) (botFit->GetParameter("Sigma")) ;
            Int_t nEntries = ((Int_t) ((TH1F*) fResidualHistos[botresName])->GetEntries()) ;
            error_botRMS = bot_residualRMS / sqrt(nEntries) ;
        }
        if(rFit) {
            r_residualRMS = (Float_t) (rFit->GetParameter("Sigma")) ;
            Int_t nEntries = ((Int_t) ((TH1F*) fResidualHistos[rresName])->GetEntries()) ;
            error_rRMS = r_residualRMS / sqrt(nEntries) ;
        }
        if(phiFit) {
            phi_residualRMS = (Float_t) (phiFit->GetParameter("Sigma")) ;
            Int_t nEntries = ((Int_t) ((TH1F*) fResidualHistos[phiresName])->GetEntries()) ;
            error_phiRMS = phi_residualRMS / sqrt(nEntries) ;
        }
        
        fprintf(f,"%d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f      %f    %f    %f    %f      %f    %f\n",detId, runId, XOffset, YOffset, ZOffset, RotationAlongZ,  x_residualRMS,  y_residualRMS, top_residualRMS, bot_residualRMS, r_residualRMS, phi_residualRMS, error_xRMS, error_yRMS, error_topRMS, error_botRMS, error_rRMS, error_phiRMS) ;
        //    printf("%d   %d   %f   %f   %f   %f   %f   %f  %f  %f  %f   %f  %f   %f\n",detId, fRunFileValue.Atoi(), XOffset, YOffset, ZOffset, RotationAlongZ,  x_residualRMS, y_residualRMS, r_residualRMS, phi_residualRMS, error_xRMS, error_yRMS, error_rRMS, error_phiRMS) ;
        printf("%d   %d   %f   %f   %f   %f   %f   %f  %f  %f  %f   %f  %f   %f\n",detId, runId, XOffset, YOffset, ZOffset, RotationAlongZ,  x_residualRMS, y_residualRMS, r_residualRMS, phi_residualRMS, error_xRMS, error_yRMS, error_rRMS, error_phiRMS) ;
        
        TString rotationName  = "rotation" + detName ;
        TF1 * rotFit =  ((TH1F*) fResidualHistos[rotationName])->GetFunction("fitFunction") ;
        Float_t rotationAngle = 0.0, mcs = 0.0, errorMean = 0.0 ;
        nEntries = ((Int_t) ((TH1F*) fResidualHistos[rotationName])->GetEntries()) ;
        if(rotFit) {
            rotationAngle = (Float_t) (rotFit->GetParameter("Mean")) ;
            mcs = (Float_t) (rotFit->GetParameter("Sigma")) ;
            errorMean = mcs / sqrt(nEntries) ;
        }
        fprintf(f1,"%d     %d    %f    %f    %f  %d\n", runId, detId, rotationAngle, mcs, errorMean, nEntries) ;
        //    printf("  SRSHistoManager::SaveFTBF_ResidualsTXT()=> SAVED to file [%s],\n", fileRot) ;
        
        TString xOffsetName  = "offset" + planeX ;
        TString yOffsetName  = "offset" + planeY ;
        
        Int_t xnEntries = 1, ynEntries = 1;
        Float_t xmean =0, xrms = 0, xerror = 0, ymean =0, yrms = 0, yerror = 0;
        
        TF1 * xoffsetFit =  ((TH1F*) fResidualHistos[xOffsetName])->GetFunction("fitFunction") ;
        xnEntries =  ((Int_t) ((TH1F*) fResidualHistos[xOffsetName])->GetEntries()) ;
        if(xoffsetFit) {
            xmean  = (Float_t) (xoffsetFit->GetParameter("Mean")) ;
            xrms   = (Float_t) (xoffsetFit->GetParameter("Sigma")) ;
            xerror = xrms / sqrt(xnEntries) ;
        }
        TF1 * yoffsetFit =  ((TH1F*) fResidualHistos[yOffsetName])->GetFunction("fitFunction") ;
        ynEntries =  ((Int_t) ((TH1F*) fResidualHistos[yOffsetName])->GetEntries()) ;
        if(yoffsetFit) {
            ymean  = (Float_t) (yoffsetFit->GetParameter("Mean")) ;
            yrms   = (Float_t) (yoffsetFit->GetParameter("Sigma")) ;
            yerror = yrms / sqrt(ynEntries) ;
        }
        fprintf(f2,"%d    %d   %d   %f   %f   %f   %d   %f   %f   %f\n",runId, detId, xnEntries, xmean, xrms, xerror, ynEntries, ymean, yrms, yerror) ;
        //    printf("  SRSHistoManager::SaveFTBF_ResidualsTXT()=> SAVED to file [%s],\n", fileOffset) ;
    }
    
    fclose(f);
    fclose(f1);
    fclose(f2);
}

//==================================================================================
void SRSHistoManager::SaveFTBF_AnalysisTXT(SRSTrack * track) {
    printf("   SRSHistoManager::SaveFTBF_AnalysisTXT() => Entering ===== \n" ) ;
    
    TString ftbfFileName = fRunName + fRunFilePrefix + "Characterisation.txt" ;
    Int_t runId = fRunFileValue.Atoi() + fAmoreAgentID.Atoi() ;
    
    const char *file =ftbfFileName.Data() ;
    printf("   SRSHistoManager::SaveFTBF_AnalysisTXT() => to file [%s], runId =%d \n", file, runId) ;
    FILE * ftbfFile = fopen(ftbfFileName, "a+");
    
    Int_t nbTriggers = 1, detId = 0;
    
    map <TString, TString > trackerList =  track->GetTrackerList() ;
    map <TString, TString >::const_iterator trk_itr ;
    for(trk_itr = trackerList.begin(); trk_itr != trackerList.end(); ++trk_itr) {
        
        Int_t  nbEntries = 0, nbXEntries = 0, nbYEntries = 0;
        Float_t chargeRatio = 0, adcDistX = 0, adcDistY = 0, clustSizeX = 0,  clustSizeY = 0, clustMultX = 0, clustMultY = 0, tbClusterX = 0, tbClusterY = 0;
        
        TString trkName = (*trk_itr).first ;
        if(trkName == "EIC1")     detId = 1 ;
        if(trkName == "SBSGEM1")  detId = 2 ;
        if(trkName == "SBSGEM2")  detId = 3 ;
        if(trkName == "TrkGEM0")  detId = 4 ;
        if(trkName == "TrkGEM1")  detId = 5 ;
        if(trkName == "TrkGEM2")  detId = 6 ;
        //    printf("   SRSHistoManager::SaveFTBF_AnalysisTXT() => is Tracker %s \n", trkName.Data()) ;
        
        std::vector < TString > listOHists = fHistoTypeToDetectorMap[trkName] ;
        std::vector < TString >::const_iterator hist_it ;
        for(hist_it = listOHists.begin(); hist_it != listOHists.end(); ++hist_it) {
            TString histoname = (*hist_it) ;
            if (fVarType[histoname] == "HITMAP") {
                nbEntries   = ((Int_t) ((TH1F*) fObjToDraw[histoname])->GetEntries());
                nbTriggers  = ((Int_t) ((TH1F*) fObjToDraw[histoname])->GetEntries()) ;
            }
            if (fVarType[histoname] == "CHARGES_RATIO") {
                TF1 * chargeRatioFit =  ((TH1F*) fObjToDraw[histoname])->GetFunction("fitFunction") ;
                if(chargeRatioFit) chargeRatio = chargeRatioFit->GetParameter("Mean") ;
                else               chargeRatio = ((TH1F*) fObjToDraw[histoname])->GetMean() ;
                delete chargeRatioFit ;
            }
        }
        
        TString planeX = (fMapping->GetDetectorPlaneListFromDetector(trkName)).front() ;
        std::vector < TString > listOHistsInX = fHistoTypeToDetectorMap[planeX] ;
        std::vector< TString >::const_iterator xhist_it ;
        for(xhist_it = listOHistsInX.begin(); xhist_it != listOHistsInX.end(); ++xhist_it) {
            TString histoname = (*xhist_it) ;
            if (fVarType[histoname] == "CLUSTMULT") nbXEntries = ((Int_t) ((TH1F*) fObjToDraw[histoname])->GetEntries());
            if (fVarType[histoname] == "CLUSTSIZE") clustSizeX = ((TH1F*) fObjToDraw[histoname])->GetMean();
            if (fVarType[histoname] == "CLUSTMULT") clustMultX = ((TH1F*) fObjToDraw[histoname])->GetMean();
            if (fVarType[histoname] == "ADCDIST")   adcDistX   = ((TH1F*) fObjToDraw[histoname])->GetMean();
            if (fVarType[histoname] == "TIMESLICE") tbClusterX    = ((TH1F*) fObjToDraw[histoname])->GetMean() ;
        }
        
        TString planeY = (fMapping->GetDetectorPlaneListFromDetector(trkName)).back() ;
        std::vector < TString > listOHistsInY = fHistoTypeToDetectorMap[planeY] ;
        std::vector< TString >::const_iterator yhist_it ;
        for(yhist_it = listOHistsInY.begin(); yhist_it != listOHistsInY.end(); ++yhist_it) {
            TString histoname = (*yhist_it) ;
            if (fVarType[histoname] == "CLUSTMULT") nbYEntries = ((Int_t) ((TH1F*) fObjToDraw[histoname])->GetEntries());
            if (fVarType[histoname] == "CLUSTSIZE") clustSizeY  = ((TH1F*) fObjToDraw[histoname])->GetMean() ;
            if (fVarType[histoname] == "CLUSTMULT") clustMultY  = ((TH1F*) fObjToDraw[histoname])->GetMean() ;
            if (fVarType[histoname] == "ADCDIST")   adcDistY    = ((TH1F*) fObjToDraw[histoname])->GetMean() ;
            if (fVarType[histoname] == "TIMESLICE") tbClusterY    = ((TH1F*) fObjToDraw[histoname])->GetMean() ;
        }
        fprintf(ftbfFile, "%d   %d   %d   %d   %d   %d   %f   %f   %f   %f    %f   %f   %f   %f  %f\n", detId, runId, nbTriggers, nbEntries, nbXEntries, nbYEntries, chargeRatio, adcDistX, adcDistY, clustSizeX, clustSizeY, clustMultX, clustMultY, tbClusterX, tbClusterY);
        printf("%d   %d   %d   %d   %d   %d   %f   %f   %f   %f    %f   %f   %f   %f  %f\n", detId, runId, nbTriggers, nbEntries, nbXEntries, nbYEntries, chargeRatio, adcDistX, adcDistY, clustSizeX, clustSizeY, clustMultX, clustMultY, tbClusterX, tbClusterY);
        listOHists.clear() ;
        listOHistsInX.clear() ;
        listOHistsInY.clear() ;  
    }
    
    map <TString, TString > detectorList =  track->GetDetectorList() ;
    map <TString, TString >::const_iterator det_itr ;
    for(det_itr = detectorList.begin(); det_itr != detectorList.end(); ++det_itr) {
        
        TString detName = (*det_itr).first ;
        if (track->IsTrigger(detName)) continue ;
        
        if(detName == "EIC1")     detId = 1 ; 
        if(detName == "SBSGEM1")  detId = 2 ; 
        if(detName == "SBSGEM2")  detId = 3 ; 
        if(detName == "TrkGEM0")  detId = 4 ; 
        if(detName == "TrkGEM1")  detId = 5 ; 
        if(detName == "TrkGEM2")  detId = 6 ; 
        
        Int_t  nbEntries = 0, nbXEntries = 0, nbYEntries = 0;
        Float_t chargeRatio = 0, adcDistX = 0, adcDistY = 0, clustSizeX = 0,  clustSizeY = 0, clustMultX = 0, clustMultY = 0, tbClusterX = 0, tbClusterY = 0;
        
        std::vector < TString > listOHistos = fHistoTypeToDetectorMap[detName] ;
        std::vector < TString >::const_iterator histo_itr ;
        
        for(histo_itr = listOHistos.begin(); histo_itr != listOHistos.end(); ++histo_itr) {
            TString histoname = (*histo_itr) ;
            
            if (fVarType[histoname] == "HITMAP") {
                nbEntries   = ((Int_t) ((TH1F*) fObjToDraw[histoname])->GetEntries());
            }
            if (fVarType[histoname] == "CHARGES_RATIO") {
                TF1 * chargeRatioFit =  ((TH1F*) fObjToDraw[histoname])->GetFunction("fitFunction") ;
                if(chargeRatioFit) chargeRatio = chargeRatioFit->GetParameter("Mean") ;
                else               chargeRatio = ((TH1F*) fObjToDraw[histoname])->GetMean() ;
                delete chargeRatioFit ;
            }
        }
        
        TString planeX = (fMapping->GetDetectorPlaneListFromDetector(detName)).front() ;
        std::vector < TString > listOHistosInX = fHistoTypeToDetectorMap[planeX] ;
        std::vector< TString >::const_iterator xhisto_itr ;
        for(xhisto_itr = listOHistosInX.begin(); xhisto_itr != listOHistosInX.end(); ++xhisto_itr) {
            TString histoname = (*xhisto_itr) ;
            if (fVarType[histoname] == "CLUSTMULT") nbXEntries = ((Int_t) ((TH1F*) fObjToDraw[histoname])->GetEntries());
            if (fVarType[histoname] == "CLUSTSIZE") clustSizeX = ((TH1F*) fObjToDraw[histoname])->GetMean();
            if (fVarType[histoname] == "CLUSTMULT") clustMultX = ((TH1F*) fObjToDraw[histoname])->GetMean();
            if (fVarType[histoname] == "TIMESLICE") tbClusterX    = ((TH1F*) fObjToDraw[histoname])->GetMean() ;
            if (fVarType[histoname] == "ADCDIST")   {
                TF1 * adcxFit =  ((TH1F*) fObjToDraw[histoname])->GetFunction("landau") ;
                if(adcxFit) adcDistX = adcxFit->GetParameter("MPV") ;
                else        adcDistX = ((TH1F*) fObjToDraw[histoname])->GetMean() ;
                delete adcxFit ;
            }
        }
        
        TString planeY = (fMapping->GetDetectorPlaneListFromDetector(detName)).back() ;
        std::vector < TString > listOHistosInY = fHistoTypeToDetectorMap[planeY] ;
        std::vector< TString >::const_iterator yhisto_itr ;
        for(yhisto_itr = listOHistosInY.begin(); yhisto_itr != listOHistosInY.end(); ++yhisto_itr) {
            TString histoname = (*yhisto_itr) ;
            if (fVarType[histoname] == "CLUSTMULT") nbYEntries = ((Int_t) ((TH1F*) fObjToDraw[histoname])->GetEntries());
            if (fVarType[histoname] == "CLUSTSIZE") clustSizeY  = ((TH1F*) fObjToDraw[histoname])->GetMean() ;
            if (fVarType[histoname] == "CLUSTMULT") clustMultY  = ((TH1F*) fObjToDraw[histoname])->GetMean() ;
            if (fVarType[histoname] == "TIMESLICE") tbClusterY    = ((TH1F*) fObjToDraw[histoname])->GetMean() ;
            if (fVarType[histoname] == "ADCDIST")   {
                TF1 * adcyFit =  ((TH1F*) fObjToDraw[histoname])->GetFunction("landau") ;
                if(adcyFit) adcDistY = adcyFit->GetParameter("MPV") ;
                else        adcDistY = ((TH1F*) fObjToDraw[histoname])->GetMean() ;
                delete adcyFit ;
            }
        }
        fprintf(ftbfFile, "%d   %d   %d   %d   %d   %d   %f   %f   %f   %f    %f   %f   %f   %f  %f\n", detId, runId, nbTriggers, nbEntries, nbXEntries, nbYEntries, chargeRatio, adcDistX, adcDistY, clustSizeX, clustSizeY, clustMultX, clustMultY, tbClusterX, tbClusterY);
        listOHistos.clear() ;
        listOHistosInX.clear() ;
        listOHistosInY.clear() ;  
    }
    fclose(ftbfFile);
}

//==================================================================================
void SRSHistoManager::SaveRunNTuple() {
    printf("   SRSHistoManager::SaveTrackingHistos() => Entering ======== \n" ) ;
    Int_t runId = fRunFileValue.Atoi() + fAmoreAgentID.Atoi() ;
    stringstream runIdStrStr ;
    runIdStrStr << runId;
    TString fileName = fRunName + fRunFilePrefix + "_Run" + runIdStrStr.str() + "_nTuple.root" ;
    
    TFile * file = new TFile(fileName,"recreate"); 
    if (!file->IsOpen()){
        Error("SRSHistoManager","SaveTrackingHistos:=> Cannot open file %s", fileName.Data());
    }  
    file->mkdir("plot3D");
    file->cd() ;
    gDirectory->cd("plot3D");
    SaveNTuplePlots(fNtuple,kTRUE) ;
    fNtuple->Write() ; 
    file->Close();
} 

//==================================================================================
void SRSHistoManager::SavePosCorrectionHistos() {
    printf("   SRSHistoManager::SaveClusterPositionCorrectionHistos() => Entering ======== \n" ) ;
    Int_t runId = fRunFileValue.Atoi() + fAmoreAgentID.Atoi() ;
    stringstream runIdStrStr ;
    runIdStrStr << runId;
    TString fileName = fRunName + fRunFilePrefix + "_Run" + runIdStrStr.str() + "_etaFunction.root" ;
    
    TFile * file = new TFile(fileName,"recreate"); 
    
    if (!file->IsOpen()){
        Error("SRSHistoManager","SaveClusterPositionCorrectionHistos():=> Cannot open file %s", fileName.Data());
    }
    
    //=== Create Folder
    file->mkdir("EtaFunction"); 
    map<TString, TH1*>::const_iterator itr; 
    for (itr = fPosCorrectionHistos.begin(); itr != fPosCorrectionHistos.end(); ++itr) {
        TString name = (*itr).first ;
        file->cd() ;
        gDirectory->cd("EtaFunction");
        SaveRunHistos(fPosCorrectionHistos[name], kTRUE) ;
        fPosCorrectionHistos[name]->Write() ;
    }
    file->Close();
}

//==================================================================================
void SRSHistoManager::SaveRunHistos(TObject* obj, Bool_t setStats ) {
    //  printf("   SRSHistoManager::SaveRunHistos() => Entering ======== \n" ) ;
    GetStyle(setStats) ;
    TString histoname = obj->GetName() ;
    TString picturename  = fRunName + "_" + histoname + ".png";
    
    TString plotstyle = "" ;
    if( (histoname.Contains("COLZ")) || (histoname.Contains("ColZ")) || (histoname.Contains("colZ"))) plotstyle = "colz" ;
    if( (histoname.Contains("LEGO")) || (histoname.Contains("lego")) || (histoname.Contains("lego"))) plotstyle = "lego2" ;
    if( (histoname.Contains("CONT")) || (histoname.Contains("Cont")) || (histoname.Contains("cont"))) plotstyle = "cont4" ;
    if( (histoname.Contains("SURF")) || (histoname.Contains("Surf")) || (histoname.Contains("surf"))) plotstyle = "surf2" ;
    
    TCanvas c("c1", "c1", 80,80,1200,1000)  ;
    if ( (histoname.Contains("EIC") || histoname.Contains("CMS") ) &&  (histoname.Contains("Hit2D") ||(histoname.Contains("Hit2D") || histoname.Contains("HIT2D")) ))  c.SetCanvasSize(2000,1000) ; 
    if ( (histoname.Contains("EIC") || histoname.Contains("CMS") ) &&  (histoname.Contains("ADC2D") ||(histoname.Contains("Adc2D") || histoname.Contains("adc2D")) ))  c.SetCanvasSize(2000,1000) ; 
    if ( (histoname.Contains("EIC") || histoname.Contains("CMS") ) &&  (histoname.Contains("Timing2D") ||(histoname.Contains("timeBin2D") || histoname.Contains("timing2D")) )) c.SetCanvasSize(2000,1000) ; 
    c.cd() ;
    obj->Draw(plotstyle) ;
    obj->UseCurrentStyle() ;
    
    if(histoname.Contains("LogZ")) gPad->SetLogz(kTRUE) ;
    c.SaveAs(picturename) ;
}

//==================================================================================
void SRSHistoManager::SaveNTuplePlots(TObject* obj, Bool_t setStats ) {
    
    //************remove gROOT->Reset() because they lead to segmentation faults in newer version of ROOT 5.34**
    //  gROOT->Reset();
    //**********************************************************************************************************
    gStyle->SetOptStat(0);
    gStyle->SetCanvasColor(0) ; 
    gStyle->SetCanvasBorderMode(0) ;
    
    gStyle->SetLabelFont(62,"xyz");
    gStyle->SetLabelSize(0.04,"xyz");
    gStyle->SetLabelColor(1,"xyz");
    gStyle->SetTitleBorderSize(0) ;
    gStyle->SetTitleFillColor(0) ;
    gStyle->SetTitleSize(0.04,"xyz");
    gStyle->SetTitleOffset(0.8,"xy");
    gStyle->SetTitleOffset(0.8,"z");
    gStyle->SetMarkerStyle(20);
    gStyle->SetMarkerSize(0.5);
    gStyle->SetPalette(0);
    
    const Int_t NRGBs = 5;
    const Int_t NCont = 8;
    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);
    
    TString histoname = obj->GetName() ;
    TString picturename  = fRunName + "_" + histoname + ".png";
    
    TCanvas c("c1", "c1", 80,80,3080,2080)  ;
    c.cd() ;
    ((TNtuple *) obj)->Draw("y:x:z:parameter","","colz") ;
    ((TNtuple *) obj)->UseCurrentStyle() ;
    c.SaveAs(picturename) ;
}


//==================================================================================
void SRSHistoManager::GetStyle(Bool_t setStats) { 
    
    gStyle->SetOptStat(0);
    if(setStats) {
        gStyle->SetOptStat(1100) ;
        gStyle->SetStatColor(0) ;
        gStyle->SetStatX(0.9) ;
        gStyle->SetStatY(0.9) ;
        gStyle->SetStatW(0.25) ;
        gStyle->SetStatH(0.25) ;
    }
    
    gStyle->SetCanvasColor(0) ;
    gStyle->SetCanvasBorderMode(0) ;
    gStyle->SetLabelFont(62,"xyz");
    gStyle->SetLabelSize(0.04,"xyz");
    gStyle->SetLabelColor(1,"xyz");
    gStyle->SetTitleBorderSize(0) ;
    gStyle->SetTitleFillColor(0) ;
    gStyle->SetTitleSize(0.045,"xyz");
    gStyle->SetTitleOffset(1.,"xyz");
    gStyle->SetPalette(1);
    gStyle->SetMarkerStyle(1);
    gStyle->SetMarkerSize(1);
    
    const Int_t NRGBs = 5;
    const Int_t NCont = 99;
    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);
}

//==================================================================================
void SRSHistoManager::DumpList() {
    map<TString,TObject*>::const_iterator itr;
    for (itr = fObjToDraw.begin(); itr != fObjToDraw.end(); ++itr){
        cout << itr->first <<" " << itr->second << endl;
    }
}


//==================================================================================
void SRSHistoManager::GaussFit( TH1F* h) {
    
    gStyle->SetOptFit(1110) ;
    gStyle->SetStatFontSize(0.02) ;
    gStyle->SetHistFillColor(kRed) ;
    
    Float_t start = h->GetMean() - 10*h->GetRMS() ;
    Float_t end   = h->GetMean() + 10*h->GetRMS() ;
    
    Double_t par[3];
    TF1 * gauss = new TF1("fitFunction","gaus", start, end);
    gauss->GetParameters(&par[0]);
    gauss->SetLineColor(kBlack);
    gauss->SetLineWidth(2) ;
    h->Fit(gauss,"R+");
}

//================================================================================================//
void SRSHistoManager::LandauFit( TH1F* h) { 
    
    gStyle->SetOptFit(1110) ;
    gStyle->SetStatFontSize(0.02) ;
    gStyle->SetHistFillColor(kGreen) ;
    
    Double_t MostProb =  h->GetMaximum() ;
    Double_t spread = h->GetRMS() ;
    
    Double_t from = MostProb - 0.2 * spread ;
    Double_t to   = MostProb + 3.0 * spread ;
    
    Double_t par[3];
    TF1 * landau = new TF1("fitFunction","landau",from,to);
    landau->SetParameters(1,MostProb,spread);
    landau->SetParNames("const","MPV","Lambda");
    landau->GetParameters(&par[0]);
    landau->SetRange(from, to);
    landau->SetLineColor(kBlack);
    landau->SetLineWidth(2) ;
    h->Fit(landau,"R+");
}

//================================================================================================//
void SRSHistoManager::DoubleGaussFit(TH1F *htemp, Int_t N_iter,  Float_t N_sigma_range, Bool_t ShowFit) { 
    
    gStyle->SetHistLineColor(kBlue) ;
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1100) ;
    
    //---------Basic Histo Peak Finding Parameters----------
    Int_t binMaxCnt   = htemp->GetMaximumBin() ;  
    Int_t binMaxCnt_counts = (Int_t) htemp->GetBinContent(binMaxCnt); 
    Double_t binMaxCnt_value  = (Double_t) htemp->GetXaxis()->GetBinCenter(binMaxCnt); 
    
    Int_t NPeaks ;
    Float_t * Peak;           
    Float_t * PeakAmp;   
    
    Float_t peak_pos = 0;
    Float_t peak_pos_amp = 0;  //Initial value, assuming positive going peaks
    Int_t   peak_pos_bin = 0;  
    
    Int_t Nbins = 0 ;
    Int_t zero_value_bin = 0 ;
    Float_t low_limit = 0;
    Float_t high_limit = 0;
    Float_t peak_pos_count = 0;
    Float_t zero_bin_value = 0;
    Float_t max_bin_value = 0;
    
    TSpectrum * s = new TSpectrum();
    //  printf("   SRSHistoManager::DoubleGaussianFit() After TSpectrum \n") ;
    
    //---------TSpectrum Peak Finding Parameters--------
    if (ShowFit) NPeaks = s->Search(htemp, 2, "goff", 0.5); //opens a canvas (one time in a loop), even with:  s->Search(htemp, 2, "nodraw", 0.9);
    Peak = s->GetPositionX();
    PeakAmp = s->GetPositionY();
    
    for ( Int_t i = 0; i < NPeaks; i++)     {
        if (peak_pos_amp < PeakAmp[i]) 	{
            peak_pos_amp = PeakAmp[i];   //TSpectrum finds peak counts
            peak_pos = Peak[i];          //TSpectrum finds pos. of peak in x-axis units
            printf("   SRSHistoManager::DoubleGaussianFit(): i = %d,  peak_pos = %f, peak_pos_amp = %f \n", i, peak_pos, peak_pos_amp  ) ;
        }
    }
    
    peak_pos_bin = htemp->GetXaxis()->FindBin(peak_pos) ;
    peak_pos_count = htemp->GetBinContent(peak_pos_bin) ;
    zero_value_bin = htemp->GetXaxis()->FindBin(0.0) ;
    Nbins = htemp->GetSize() - 2 ;
    zero_bin_value =  htemp->GetXaxis()->GetBinCenter(0) ;
    max_bin_value = htemp->GetXaxis()->GetBinCenter(Nbins) ;
    
    TF1 *func ;
    TF1 *func1 ;
    TF1 *func2 ;
    TF1 *func3 ;
    
    Float_t Chi2 ;
    Int_t NDF = 1 ;
    
    Float_t RChi2 = 1 ; 
    Float_t peak  = 1;  
    
    Float_t const1     = 1 ;
    Float_t mean1      = 1 ;
    Float_t mean_err1  = 1 ;
    Float_t sigma1     = 1 ;
    Float_t sigma_err1 = 1 ;
    
    Float_t const2     = 1 ;
    Float_t mean2      = 1 ;
    Float_t mean_err2  = 1 ;
    Float_t sigma2     = 1 ;
    Float_t sigma_err2 = 1 ;
    
    int TS = 0 ;
    func = new TF1("func", "gaus") ;
    
    //Make sure that TSpectrum peak is within histo range if not, use Par initial values from Basic Histo Peak Find
    if (peak_pos >= zero_bin_value  &&  peak_pos <= max_bin_value)  {
        TS=1 ;
        low_limit  = peak_pos - (0.1 * abs(max_bin_value-zero_bin_value));
        high_limit = peak_pos + (0.1 * abs(max_bin_value-zero_bin_value)) ;
        func->SetParameter(0, peak_pos_count) ;
        func->SetParameter(1, peak_pos) ;
    }
    
    else {
        low_limit = binMaxCnt_value - (0.1 * abs(max_bin_value-zero_bin_value)); 
        high_limit = binMaxCnt_value + (0.1 * abs(max_bin_value-zero_bin_value)); 
        func->SetParameter(0, binMaxCnt_counts);
        func->SetParameter(1, binMaxCnt_value);
    }
    
    //low_limit, high_limit); //  To Show fit: htemp->Fit("gaus"); //better fit?-> Fit("gaus", "MQ", "", "", ""); 
    htemp->Fit("gaus", "Q0", "", low_limit, high_limit) ;
    func  = htemp->GetFunction("gaus") ;
    Chi2 = func->GetChisquare() ;
    NDF = func->GetNDF() ;
    if (NDF != 0) RChi2 = Chi2/NDF ;
    const1 = func->GetParameter(0) ;
    mean1 = func->GetParameter(1) ;
    mean_err1 = func->GetParError(1) ;
    sigma1 = func->GetParameter(2) ;
    sigma_err1 = func->GetParError(2) ;
    
    printf("\n   SRSHistoManager::DoubleGaussianFit(): ===== BEFORE  FIRST LOOP \n") ;
    
    for (Int_t i = 0; i < N_iter; i++) { //8 seems to work well, so let's keep it constant here.
        //  for (int i=0; i< 8; i++) {
        htemp->Fit("gaus", "Q0", "",(mean1 - (N_sigma_range*sigma1)), (mean1 + (N_sigma_range*sigma1) )) ; //don't show fit
        func  = htemp->GetFunction("gaus") ;
        Chi2 = func->GetChisquare() ;
        NDF = func->GetNDF() ;
        
        if (NDF != 0) RChi2 = Chi2/NDF ;
        const1     = func->GetParameter(0) ;
        mean1      = func->GetParameter(1) ;
        mean_err1  = func->GetParError(1) ;
        sigma1     = func->GetParameter(2) ;  
        sigma_err1 = func->GetParError(2) ;
        //    printf("   SRSHistoManager::DoubleGaussianFit(): RChi2  = %f, const1 = %f, mean1 = %f, meanError1 = %f, sigma1 = %f, sigma_error1 = %f\n",RChi2, const1, mean1, mean_err1, sigma1, sigma_err1) ;
    }
    
    //Amplitude
    peak =  func->GetParameter(0) ; 
    
    //background height ~ bgd_h*gaus_amp
    Float_t bgd_h = 0.25;  
    
    func1 = new TF1("func1", "gaus") ;
    func2 = new TF1("func2", "gaus") ;
    //func3 = new TF1("fitFunction", "func1 + func2", (mean1 - N_sigma_range*sigma1), (mean1 + N_sigma_range*sigma1) );
    func3 = new TF1("fitFunction", "func1 + func2", (mean1 - 10*sigma1), (mean1 + 10*sigma1) );
    func3->SetLineColor(kRed) ;
    
    //-----------Fit Parameter constraints:
    func3->SetParameters(const1, mean1, sigma1, const1/3, mean1, 20*sigma1); //Set Initial Valules
    func3->SetParLimits(3, 0, (0.1*peak_pos_amp) ); 
    
    htemp->Fit("fitFunction", "Q0"); 
    func  = htemp->GetFunction("fitFunction") ;
    Chi2 = func3->GetChisquare() ;
    NDF = func->GetNDF() ;
    
    if (NDF != 0) RChi2 = Chi2/NDF ;
    const1     = func3->GetParameter(0);
    mean1      = func3->GetParameter(1);
    mean_err1  = func3->GetParError(1);
    sigma1     = func3->GetParameter(2);
    sigma_err1 = func3->GetParError(2);
    const2     = func3->GetParameter(3);
    mean2      = func3->GetParameter(4);
    mean_err2  = func3->GetParError(4);
    sigma2     = func3->GetParameter(5);
    sigma_err2 = func3->GetParError(5);
    
    func3->SetParNames("Const", "Mean", "Sigma", "Bkg Const", "Bkg Mean", "Bkg Sigma");
    
    for (Int_t j=0; j<4; j++)  {
        func3->SetParameters(const1, mean1, sigma1, const2, mean2, sigma2) ;
        
        //----------------Show or don't show fit----------------- 
        if (ShowFit) htemp->Fit("fitFunction", "R+"); 
        else htemp->Fit("fitFunction", "Q0");        
        //-------------------------------------------------------     
        
        func3 = htemp->GetFunction("fitFunction");
        func3->SetLineColor(kBlack);
        Chi2 = func3->GetChisquare();
        NDF = func3->GetNDF();
        if (NDF != 0) RChi2 = Chi2/NDF;
        
        const1     = func3->GetParameter(0);
        mean1      = func3->GetParameter(1);
        mean_err1  = func3->GetParError(1);
        sigma1     = func3->GetParameter(2);
        sigma_err1 = func3->GetParError(2);
        const2     = func3->GetParameter(3);
        mean2      = func3->GetParameter(4);
        mean_err2  = func3->GetParError(4);
        sigma2     = func3->GetParameter(5);
        sigma_err2 = func3->GetParError(5);
        //  printf("   SRSHistoManager::DoubleGaussianFit(): RChi2  = %f, const1 = %f, mean1 = %f, meanError1 = %f, sigma1 = %f, sigma_error1 = %f\n", RChi2, const1, mean1, mean_err1, sigma1, sigma_err1) ;
        //  printf("   SRSHistoManager::DoubleGaussianFit(): RChi2  = %f, const2 = %f, mean2 = %f, meanError2 = %f, sigma2 = %f, sigma_error2 = %f\n", RChi2, const2, mean2, mean_err2, sigma2, sigma_err2) ;
    }
    
    
    if (abs(const1 - peak) < abs(const2 - peak)) {
        fRChi2      = RChi2;
        fMean       = mean1;
        fMeanError  = mean_err1;
        fSigma      = abs(sigma1);
        fSigmaError = sigma_err1;
    }
    else {
        fRChi2      = RChi2;
        fMean       = mean2;
        fMeanError  = mean_err2;
        fSigma      = abs(sigma2);
        fSigmaError = sigma_err2;
    }
    
    delete s;
    delete func; 
    delete func1;
    delete func2;
    //  delete func3; 
    
}

//============================================================================================
void SRSHistoManager::DeleteListOfClusters(TList * listOfClusters) {
    listOfClusters->Clear();
    delete listOfClusters;
}

//============================================================================================
void SRSHistoManager::DeleteHitsInDetectorPlaneMap( map<TString, list <SRSHit * > > & stringListMap) {
    map < TString, list <SRSHit *> >::const_iterator itr ;
    for (itr = stringListMap.begin(); itr != stringListMap.end(); itr++) {
        list <SRSHit *> listOfHits = (*itr).second ;
        listOfHits.clear() ;
    }
    stringListMap.clear() ;
} 

//============================================================================================
void SRSHistoManager::DeleteClustersInDetectorPlaneMap( map<TString, list <SRSCluster * > > & stringListMap) {
    map < TString, list <SRSCluster *> >::const_iterator itr ;
    for (itr = stringListMap.begin(); itr != stringListMap.end(); itr++) {
        list <SRSCluster *> listOfClusters = (*itr).second ;
        listOfClusters.clear() ;
    }
    stringListMap.clear() ;
} 
