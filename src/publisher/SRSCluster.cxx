#include "SRSCluster.h"
ClassImp(SRSCluster);

//====================================================================================================================
SRSCluster::SRSCluster(Int_t minClusterSize, Int_t maxClusterSize, TString isMaximumOrTotalADCs) {
    fNbOfHits = 0;
    fstrip = 0;
    
    fClusterSumADCs = 0;
    fClusterPeakADCs = 0;
    
    fClusterTimeBinADC = 0 ;
    fClusterTimeBin = 0 ;
    fClusterPeakTimeBin = 0 ;
    
    fposition = 0;
    fclusterCentralStrip = 0;
    
    fMinClusterSize = minClusterSize;
    fMaxClusterSize = maxClusterSize;
    fIsClusterMaxOrSumADCs = isMaximumOrTotalADCs ;
    fPlaneSize = 512. ;
    fPlane     = "GEM1X" ;
    fNbAPVsOnPlane = 10 ;
    fArrayOfHits = new TObjArray(maxClusterSize);
    fIsGoodCluster = kTRUE ;
    fIsCluserPosCorrection = kFALSE ;
    //  printf("=======SRSCluster\n") ;
}

//====================================================================================================================
SRSCluster::~SRSCluster() {
    fArrayOfHits->Clear();
    delete fArrayOfHits;
    fClusterTimeBinADCs.clear() ;
}

//====================================================================================================================
Int_t SRSCluster::GetClusterTimeBin() {
    float q;
    TObjArray &temp = *fArrayOfHits;
    Int_t nbofhits =  GetNbOfHits() ;
    fClusterTimeBinADC = 0 ;
    
    for (int i = 0; i < nbofhits; i++) {
        vector< Float_t > timeBinADCs = ((SRSHit*)temp[i])->GetTimeBinADCs() ;
        Int_t nbOfTimeBins = timeBinADCs.size() ;
        fClusterTimeBinADCs.resize(nbOfTimeBins) ;
        for (int k = 0; k < nbOfTimeBins; k++) {
            fClusterTimeBinADCs[k] += timeBinADCs[k] ;
        }
        timeBinADCs.clear() ;
    }
    
    Timing() ;
    return fClusterTimeBin ;
}

//============================================================================================
void SRSCluster::Timing() {
    //  Bool_t timingStatus = kTRUE ;
    Int_t nBins = fClusterTimeBinADCs.size() ;
    TH1F * timeBinHist = new TH1F("timeBinHist", "timeBinHist", nBins, 0, (nBins-1) ) ;
    for (Int_t k = 0; k < nBins; k++) {
        timeBinHist->Fill(k,fClusterTimeBinADCs[k]) ;
    }
    fClusterTimeBin = timeBinHist->GetMaximumBin() ;
    delete timeBinHist ;
}



//====================================================================================================================
void SRSCluster::ClusterPositionPulseHeghtWeight() {  // Calculate the fposition and the total fClusterSumADCs
    float hitposition, q;
    TObjArray &temp = *fArrayOfHits;
    Int_t nbofhits =  GetNbOfHits() ;
    for (int i = 0; i < nbofhits; i++) {
        q  = ((SRSHit*)temp[i])->GetHitADCs() ;
        hitposition = ((SRSHit*)temp[i])->GetStripPosition() ;
        fClusterSumADCs += q ;
        fposition += q * hitposition ;
        
        if (q > fClusterPeakADCs) {
            fClusterPeakTimeBin = ((SRSHit*)temp[i])->GetSignalPeakBinNumber() ;
            fClusterPeakADCs = q ;
        }
    }
    fposition /= fClusterSumADCs;
    //  printf("   SRSCluster::ClusterPositionPulseHeghtWeight => clusterPosition = %f\n", fposition) ;
}

//====================================================================================================================
void SRSCluster::ClusterPositionHistoMean() {  // Calculate the fposition and the total fClusterSumADCs
    float hitposition, q;
    TObjArray &temp = *fArrayOfHits;
    Int_t nbofhits =  GetNbOfHits() ;
    Int_t indexLastHist = nbofhits - 1 ;
    Float_t pitch = fPlaneSize / (NCH * fNbAPVsOnPlane) ;
    Float_t min = ((SRSHit*)temp[0])->GetStripPosition() -  pitch;
    Float_t max = ((SRSHit*)temp[indexLastHist])->GetStripPosition() + pitch ;
    Int_t nbin = nbofhits + 2 ;
    TH1F * h = new TH1F("coordHisto", "coordinate" , nbin, min, max) ;
    for (int i = 0; i < nbofhits; i++) {
        q  = ((SRSHit*)temp[i])->GetHitADCs() ;
        hitposition = ((SRSHit*)temp[i])->GetStripPosition() ;
        h->Fill(hitposition , q) ;
        fClusterSumADCs += q ;
        if (q > fClusterPeakADCs) fClusterPeakADCs = q ;
    }
    fposition = h->GetMean() ;
    delete  h ;
    //  printf("   SRSCluster::ClusterPositionGausFitMean => clusterPosition = %f\n", fposition) ;
}

//====================================================================================================================
void SRSCluster::ClusterPositionGausFitMean() {  // Calculate the fposition and the total fClusterSumADCs
    float hitposition, q;
    TObjArray &temp = *fArrayOfHits;
    Int_t nbofhits =  GetNbOfHits() ;
    Int_t indexLastHist = nbofhits - 1 ;
    Float_t pitch = fPlaneSize / (NCH * fNbAPVsOnPlane) ;
    Float_t min = ((SRSHit*)temp[0])->GetStripPosition() -  pitch;
    Float_t max = ((SRSHit*)temp[indexLastHist])->GetStripPosition() + pitch ;
    Int_t nbin = nbofhits + 2 ;
    TH1F * h = new TH1F("coordHisto", "coordinate" , nbin, min, max) ;
    for (int i = 0; i < nbofhits; i++) {
        q  = ((SRSHit*)temp[i])->GetHitADCs() ;
        hitposition = ((SRSHit*)temp[i])->GetStripPosition() ;
        h->Fill(hitposition , q) ;
        fClusterSumADCs += q ;
        if (q > fClusterPeakADCs) fClusterPeakADCs = q ;
    }
    h->Fit("gaus", "Q", "", min, max) ;
    fposition = h->GetFunction("gaus")->GetParameter(1) ;
    delete  h ;
    //  printf("   SRSCluster::ClusterPositionGausFitMean => clusterPosition = %f\n", fposition) ;
}

//====================================================================================================================
void SRSCluster::ClusterCentralStrip() {
    float p, dp ;
    float dpmin = 99;
    TObjArray &temp = *fArrayOfHits;
    Int_t nbofhits =  GetNbOfHits() ;
    for (int i = 0; i < nbofhits; i++) {
        p  = ((SRSHit*)temp[i])->GetStripPosition();
        dp = fabs(fposition - p);
        if (dp <= dpmin) {
            fclusterCentralStrip = p;
            dpmin = dp;
        }
    }
}

//====================================================================================================================
Float_t SRSCluster::GetClusterADCs() {
    Float_t adcs = 0 ;
    if (fIsClusterMaxOrSumADCs == "maximumADCs") {
        adcs = fClusterPeakADCs ;
    }
    else {
        adcs = fClusterSumADCs ;
    }
    return adcs ;
}

//====================================================================================================================
void SRSCluster::Dump() {
    cout << "*** APV Cluster dump ***" << endl;
    TObject::Dump();
    cout << endl;
}

//====================================================================================================================
void SRSCluster::AddHit(SRSHit *h) {
    fArrayOfHits->AddLast(h);
}

//====================================================================================================================
void SRSCluster::ClearArrayOfHits() {
    fArrayOfHits->Clear();
}

//====================================================================================================================
Bool_t SRSCluster::IsGoodCluster() {
    fIsGoodCluster = kTRUE ;
    fNbOfHits = fArrayOfHits->GetEntries() ;
    if ( (fNbOfHits > fMaxClusterSize) || (fNbOfHits < fMinClusterSize) ) {
        ClearArrayOfHits() ;
        fIsGoodCluster  = kFALSE ;
        fNbOfHits = fArrayOfHits->GetEntries() ;
    }
    return fIsGoodCluster ;
}

//====================================================================================================================
int SRSCluster::Compare(const TObject *obj) const {
    int compare = (fClusterSumADCs < ((SRSCluster*)obj)->GetClusterADCs()) ? 1 : -1;
    return compare ;
}

//-------------------------------------------------------------------------------------------------
void SRSCluster::ComputeClusterPositionWithoutCorrection() {
    ClusterPositionPulseHeghtWeight() ;
    ClusterCentralStrip() ;
}

/**
 //-------------------------------------------------------------------------------------------------
 void SRSCluster::ComputeClusterPositionWithCorrection(const char * filename) {
 ComputeClusterPositionWithoutCorrection() ;
 
 if(fNbOfHits > 1)  {
 TFile * file =  new TFile(filename, "read") ;
 if (!file->IsOpen())   printf("SRSEventBuilder::LoadPositionCorrectionRootFile()==> ERROR Can not open file %s \n",filename) ;
 
 Float_t pitch    = fPlaneSize / (NCH * fNbAPVsOnPlane) ;
 if(fPlane.Contains("EIC")) pitch = 2 * pitch ;
 Float_t eta = (fposition - fclusterCentralStrip) / pitch ;
 
 TString histoName = "etaFunction" + fPlane ;
 
 if(eta > 0) {
 if(fNbOfHits == 2) histoName = "eta2FunctionPos" + fPlane ;
 if(fNbOfHits == 3) histoName = "eta3FunctionPos" + fPlane ;
 if(fNbOfHits == 4) histoName = "eta4FunctionPos" + fPlane ;
 if(fNbOfHits == 5) histoName = "eta5FunctionPos" + fPlane ;
 if(fNbOfHits > 5) histoName = "eta6PlusFunctionPos" + fPlane ;
 }
 
 if(eta < 0) {
 if(fNbOfHits == 2) histoName = "eta2FunctionNeg" + fPlane ;
 if(fNbOfHits == 3) histoName = "eta3FunctionNeg" + fPlane ;
 if(fNbOfHits == 4) histoName = "eta4FunctionNeg" + fPlane ;
 if(fNbOfHits == 5) histoName = "eta5FunctionNeg" + fPlane ;
 if(fNbOfHits > 5) histoName = "eta6PlusFunctionNeg" + fPlane ;
 }
 
 TH1F * posCorrectionHistos =  (TH1F *) file->Get(histoName) ;
 
 TString afterhistoname = posCorrectionHistos->GetName() ;
 TString fitName = afterhistoname+"_FIT" ;
 TF1 * correctionFunctionFit = (TF1 *) posCorrectionHistos->GetFunction(fitName) ;
 Float_t posCorrecFuncution = 0 ;
 if (eta > 0) posCorrecFuncution  = correctionFunctionFit->Integral(0, eta) / correctionFunctionFit->Integral(0,0.5) ;
 if (eta < 0) posCorrecFuncution  = correctionFunctionFit->Integral(eta, 0) / correctionFunctionFit->Integral(-0.5,0) ;
 delete correctionFunctionFit ;
 
 fposition = fclusterCentralStrip - (pitch * (0.5 - posCorrecFuncution)) ;
 delete posCorrectionHistos ;
 delete file ;
 }
 }
 */


//-------------------------------------------------------------------------------------------------
void SRSCluster::ComputeClusterPositionWithCorrection(const char * filename) {
    ComputeClusterPositionWithoutCorrection() ;
    
    if(fNbOfHits > 1)  {
        TFile * file =  new TFile(filename, "read") ;
        if (!file->IsOpen())   printf("SRSEventBuilder::LoadPositionCorrectionRootFile()==> ERROR Can not open file %s \n",filename) ;
        
        Float_t pitch    = fPlaneSize / (NCH * fNbAPVsOnPlane) ;
        if(fPlane.Contains("EIC")) pitch = 2 * pitch ;
        Float_t eta = (fposition - fclusterCentralStrip) / pitch ;
        
        TString histoNamePos = "etaFunction" + fPlane ;
        TString histoNameNeg = "etaFunction" + fPlane ;
        
        
        if(fNbOfHits == 2) {
            histoNamePos = "eta2FunctionPos" + fPlane ;
            histoNameNeg = "eta2FunctionNeg" + fPlane ;
        }
        
        if(fNbOfHits == 3) {
            histoNamePos = "eta3FunctionPos" + fPlane ;
            histoNameNeg = "eta3FunctionNeg" + fPlane ;
        }
        if(fNbOfHits == 4) {
            histoNamePos = "eta4FunctionPos" + fPlane ;
            histoNameNeg = "eta4FunctionNeg" + fPlane ;
        }
        if(fNbOfHits == 5) {
            histoNamePos = "eta5FunctionPos" + fPlane ;
            histoNameNeg = "eta5FunctionNeg" + fPlane ;
        }
        if(fNbOfHits > 5) {
            histoNamePos = "eta6PlusFunctionPos" + fPlane ;
            histoNameNeg = "eta6PlusFunctionNeg" + fPlane ;
        }
        
        TH1F * posCorrectionPosHistos =  (TH1F *) file->Get(histoNamePos) ;
        TH1F * posCorrectionNegHistos =  (TH1F *) file->Get(histoNameNeg) ;
        Float_t posCorrecFuncution = 0 ;
        
        Int_t firstBin = posCorrectionNegHistos->FindBin(-0.5) ;
        Int_t zeroBin  = posCorrectionPosHistos->FindBin(0) ;
        Int_t etaBin   = posCorrectionPosHistos->FindBin(eta) ;
        if (eta < 0) etaBin   = posCorrectionNegHistos->FindBin(eta) ;
        
        Float_t integralPos = posCorrectionPosHistos->Integral() ;
        Float_t integralNeg = posCorrectionPosHistos->Integral() ;
        Float_t integralSum = integralNeg + integralPos ;
        
        if (eta > 0) posCorrecFuncution = ( integralNeg + posCorrectionPosHistos->Integral(zeroBin,etaBin) )/ integralSum ;
        if (eta < 0) posCorrecFuncution = ( posCorrectionPosHistos->Integral(firstBin, etaBin) ) / integralSum ;
        
        /**
         TString afterhistonamePos = posCorrectionPosHistos->GetName() ;
         TString fitNamePos = afterhistonamePos + "_FIT" ;
         TF1 * correctionFunctionPosFit = (TF1 *) posCorrectionPosHistos->GetFunction(fitNamePos) ;
         
         TString afterhistonameNeg = posCorrectionNegHistos->GetName() ;
         TString fitNameNeg = afterhistonameNeg + "_FIT" ;
         TF1 * correctionFunctionNegFit = (TF1 *) posCorrectionNegHistos->GetFunction(fitNameNeg) ;
         
         Float_t integralPos = correctionFunctionPosFit->Integral(0,0.5)  ;
         Float_t integralNeg = correctionFunctionNegFit->Integral(-0.5,0) ;
         Float_t integralSum = integralNeg + integralPos ;
         
         if (eta > 0) posCorrecFuncution = ( integralNeg + correctionFunctionPosFit->Integral(0, eta) ) / integralSum ; 
         if (eta < 0) posCorrecFuncution  =( correctionFunctionNegFit->Integral(-0.5, eta) ) / integralSum ; 
         delete correctionFunctionPosFit ;
         delete correctionFunctionNegFit ;
         */
        
        fposition = fclusterCentralStrip - (pitch * (0.5 - posCorrecFuncution)) ; 
        delete posCorrectionPosHistos ;
        delete posCorrectionNegHistos ;
        delete file ; 
    }
}
