#ifndef __SRSRAWPEDESTAL__
#define __SRSRAWPEDESTAL__
/*******************************************************************************
 *  AMORE FOR SRS - SRS                                                         *
 *  SRSRawPedestal                                                              *
 *  SRS Module Class                                                            *
 *  Author: Kondo GNANVO 18/08/2010                                             *
 *******************************************************************************/
#define NCH 128
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

#include "SRSMapping.h"
#include "SRSFECPedestalDecoder.h"
//#include "SRSEventBuilder.h"

#endif



using namespace std;

/**
 @author Kondo Gnanvo
 */

class SRSRawPedestal : public TObject {
    
public:
    
    SRSRawPedestal(Int_t nbOfAPVs);
    SRSRawPedestal();
    ~SRSRawPedestal();
    
    void Init(Int_t nbOfAPVs);
    void FillRawPedestalHistos(SRSFECPedestalDecoder * pedestalDecoder);
    
    void Clear() ;
    void Reset() ;
    void GetStyle() ;
    void ClearMaps() ;
    
    void ComputeRawPedestalData() ;
    void SaveRawPedestalRunHistos() ;
    void LoadRawPedestalData(const char * filename) ;
    
    Float_t GetNoise(Int_t apvID, Int_t channelId);
    Float_t GetOffset(Int_t apvID, Int_t channelId);
    
    Float_t GetOnlinePedestalRMS(Int_t apvID, Int_t channelId) ;
    Float_t GetOnlinePedestalMean(Int_t apvID, Int_t channelId) ;
    
    vector<Float_t> GetAPVNoises(Int_t apvID) ;
    vector<Float_t> GetAPVOffsets(Int_t apvID) ;
    
    vector<Float_t> GetRawPedestalData() {
        return fRawPedestalData ;
    }
    
    TH1F * GetPedHisto(Int_t apvID, Int_t channelId);
    TH1F * BookHistos(Int_t apvKey, TString dataType, TString dataNb) ;
    TH2F * Book2DHistos(Int_t apvKey) ;
    
    Bool_t IsRawPedestalComputed(){ return fIsRawPedestalComputed;}
    static SRSRawPedestal * GetRawPedestalRootFile(const char * filename);
    
    void SetRunName(TString runname) {fRunName = runname;}
    void SetRunType(TString runtype) {fRunType = runtype;}
    
private:
    
    Int_t fNbOfAPVs;
    Int_t fNbOfChannels;
    Int_t fEventNb;
    TString fRunName, fRunType ;
    Bool_t fIsRawPedestalComputed, fIsFirstEvent ;
    
    vector<TH1F*> fRMSDist;
    
    vector<TH1F*> fNoises;
    vector<TH1F*> fOffsets;
    vector<TH2F*> fRawPed2DHistos;
    vector<TH1F*> fPedHistos;
    
    
    vector<Float_t> fRawPedestalData ;
    ClassDef(SRSRawPedestal,4) 
};


#endif
