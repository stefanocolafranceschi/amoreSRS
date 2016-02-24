#ifndef __SRSPEDESTAL__
#define __SRSPEDESTAL__
/*******************************************************************************
 *  AMORE FOR SRS - SRS                                                         *
 *  SRSPedestal                                                                 *
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
#include <TColor.h>
#include <TCanvas.h>
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TList.h"
#include "TString.h"
#include "TSystem.h"
#include "TObjArray.h"
#include "TObjString.h"

#include "SRSRawPedestal.h"
#include "SRSFECPedestalDecoder.h"

#endif

using namespace std;

class SRSPedestal: public TObject {
    
public:
    
    SRSPedestal(Int_t nbOfAPVs, Int_t chMaskCut);
    
    SRSPedestal();
    ~SRSPedestal();
    
    void Init();
    void FillPedestalHistos(SRSFECPedestalDecoder* pedestalDecoder,
                            SRSRawPedestal * rawped);
    
    void Clear();
    void Reset();
    void GetStyle();
    void ClearMaps();
    
    void ComputePedestalData();
    void ComputeMaskedChannels();
    
    void SavePedestalRunHistos();
    void LoadPedestalData(const char * filename);
    
    Float_t GetNoise(Int_t apvID, Int_t channelId);
    Float_t GetOffset(Int_t apvID, Int_t channelId);
    Float_t GetMaskedChannelStatus(Int_t apvID, Int_t chNo);
    
    Float_t GetOnlinePedestalRMS(Int_t apvID, Int_t channelId);
    Float_t GetOnlinePedestalMean(Int_t apvID, Int_t channelId);
    
    vector<Float_t> GetAPVNoises(Int_t apvID);
    vector<Float_t> GetAPVOffsets(Int_t apvID);
    vector<Float_t> GetAPVMaskedChannels(Int_t apvID);
    
    TH1F * GetPedHisto(Int_t apvID, Int_t channelId);
    TH1F * BookHistos(Int_t apvKey, TString dataType, TString dataNb);
    TH2F * Book2DHistos(Int_t apvKey);
    
    TString GetHistoName(Int_t apvKey, TString dataType, TString dataNb);
    
    Bool_t IsPedestalComputed() {
        return fIsPedestalComputed;
    }
    Bool_t IsMaskedChannelComputed() {
        return fIsMaskedChComputed;
    }
    
    void SetRunName(TString runname) {
        fRunName = runname;
    }
    void SetRunType(TString runtype) {
        fRunType = runtype;
    }
    
    static SRSPedestal * GetPedestalRootFile(const char * filename);
    
private:
    Int_t fNbOfAPVs;
    Int_t fNbOfChannels;
    Int_t fEventNb, fChMaskCut;
    TString fRunName, fRunType;
    Bool_t fIsPedestalComputed, fIsMaskedChComputed, fIsAPVHeaderComputed,
    fIsCommonModeComputed, fIsFirstEvent;
    
    vector<TH1F*> fRMSDist;
    vector<TH1F*> fNoises;
    vector<TH1F*> fOffsets;
    vector<TH1F*> fMaskedChannels;
    vector<TH1F*> fPedHistos;
    vector<TH2F*> fPed2DHistos;
    
    vector<Float_t> fPedestalData;
    
    ClassDef(SRSPedestal,6)
};

#endif
