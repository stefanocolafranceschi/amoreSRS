#include "SRSAPVSignalFit.h"
ClassImp(SRSAPVSignalFit);

SRSAPVSignalFit::SRSAPVSignalFit(Int_t apvID, Int_t stripNo) {
    fapvID   = apvID;
    fStripNo = stripNo;
}

SRSAPVSignalFit::~SRSAPVSignalFit() {
}


Double_t SRSAPVSignalFit::GetAPVStripPulseHeight(vector<Double_t> apvTimingSignal) {
    
    Int_t nBins = apvTimingSignal.size() ;
    
    // Fill histo with the data to be fitted
    TH1F * histo = new TH1F("histo", "APV signal", nBins,1, nBins) ;
    for(int i=0; i < nBins;  i++) histo->SetBinContent(i+1, apvTimingSignal[i]) ;
    
    // create a TF1 with the range from 0 to nBins and 4 parameters
    TF1 *fitFcn = new TF1("fitFcn","((((x - [0]) / [1]) * (TMath::Exp(-1*((x - [0])/[1]) ))*[2] ) * ((TMath::Erf((x - [0])* 100 ) + 1) / 2))", 0, nBins) ;
    
    fitFcn->SetParName(0,"par1") ;
    fitFcn->SetParName(1,"par2") ;
    fitFcn->SetParName(2,"par3") ;
    fitFcn->SetParName(3,"par4") ;
    
    fitFcn->SetParameter(0,1.76) ;
    fitFcn->SetParameter(1,1.66) ;
    fitFcn->SetParameter(2,3243.1) ;
    fitFcn->SetParameter(3,0.0) ;
    
    // Fit the data
    histo->Fit("fitFcn","V+","ep") ;
    
    // Get the max of the fitted data (pulse height)
    Double_t max = fitFcn->GetMaximum() ;
    Double_t min = fitFcn->GetMinimum() ;
    
    fitFcn->SetNpx(100*nBins);
    fitFcn->SetLineWidth(2);
    fitFcn->SetLineColor(kBlue);
    
    histo->SetStats(0);
    histo->SetMarkerStyle(21);
    histo->SetMarkerSize(0.8);
    histo->SetMaximum(1.1*max) ; 
    histo->SetMinimum(1.1*min) ; 
    return max ; 
}





