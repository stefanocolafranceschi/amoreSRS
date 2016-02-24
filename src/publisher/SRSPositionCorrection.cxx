#include "SRSPositionCorrection.h"
ClassImp(SRSPositionCorrection);

//==================================================================================
SRSPositionCorrection::SRSPositionCorrection() {
    fIsEtaFunctionComputed = kFALSE;
}
//==================================================================================
SRSPositionCorrection::SRSPositionCorrection(TString runname, TString runtype) {
    fIsEtaFunctionComputed = kFALSE;
    fRunName = runname ;
    fRunType = runtype ;
    BookClusterPositionCorrection() ;
}

//==================================================================================
SRSPositionCorrection::~SRSPositionCorrection(){
    DeleteHitsInDetectorPlaneMap(fHitsInDetectorPlaneMap) ;
    DeleteClustersInDetectorPlaneMap(fClustersInDetectorPlaneMap) ;
    Clear() ;
}

//==================================================================================
void SRSPositionCorrection::Clear() {
    printf("SRSPositionCorrection::Clear():  ENTER \n") ;
    
    SRSMapping * mapping = SRSMapping::GetInstance() ;
    map <Int_t, TString >  detectorList = mapping->GetDetectorFromIDMap() ;
    map <Int_t, TString >::const_iterator det_itr ;
    for(det_itr = detectorList.begin(); det_itr != detectorList.end(); ++det_itr) {
        
        Int_t detID     = (*det_itr).first ;
        TString detName = (*det_itr).second ;
        
        list<TString> planeList = mapping->GetDetectorPlaneListFromDetector(detName) ;
        list<TString >::const_iterator plane_itr ;
        for(plane_itr = planeList.begin(); plane_itr != planeList.end(); ++plane_itr) {
            
            TString plane = *plane_itr ;
            Int_t planeID = (Int_t) mapping->GetPlaneIDorEtaSector(plane) ;
            Int_t histoID = (2 * detID) + planeID ;
            
            fEtaFunctionHistos[histoID]->Delete() ;
            fEta2PosHistos[histoID]->Delete() ;
            fEta3PosHistos[histoID]->Delete() ;
            fEta4PosHistos[histoID]->Delete() ;
            fEta5PosHistos[histoID]->Delete() ;
            fEta6PlusPosHistos[histoID]->Delete() ;
            
            fEta2NegHistos[histoID]->Delete() ;
            fEta3NegHistos[histoID]->Delete() ;
            fEta4NegHistos[histoID]->Delete() ;
            fEta5NegHistos[histoID]->Delete() ;
            fEta6PlusNegHistos[histoID]->Delete() ;
            
        }
        planeList.clear() ;
    }
    detectorList.clear() ;
    printf("SRSPositionCorrection::Clear():  After delete Histo one by one \n") ;
    
    if (fEtaFunctionHistos) delete [] fEtaFunctionHistos;
    if (fEta2PosHistos) delete [] fEta2PosHistos;
    if (fEta3PosHistos) delete [] fEta3PosHistos;
    if (fEta4PosHistos) delete [] fEta4PosHistos;
    if (fEta5PosHistos) delete [] fEta5PosHistos;
    if (fEta6PlusPosHistos) delete [] fEta6PlusPosHistos;
    
    if (fEta2NegHistos) delete [] fEta2NegHistos;
    if (fEta3NegHistos) delete [] fEta3NegHistos;
    if (fEta4NegHistos) delete [] fEta4NegHistos;
    if (fEta5NegHistos) delete [] fEta5NegHistos;
    if (fEta6PlusNegHistos) delete [] fEta6PlusNegHistos;
    
    printf("SRSPositionCorrection::Clear(): After delete array of PosHistos \n") ;
    
}

//====================================================================================================================
void SRSPositionCorrection::BookClusterPositionCorrection() {
    SRSMapping * mapping = SRSMapping::GetInstance() ;
    Int_t nbOfPlanes = mapping->GetNbOfDetectorPlane() ;
    fEtaFunctionHistos = new TH1F * [nbOfPlanes];
    fEta2PosHistos     = new TH1F * [nbOfPlanes];
    fEta3PosHistos     = new TH1F * [nbOfPlanes];
    fEta4PosHistos     = new TH1F * [nbOfPlanes];
    fEta5PosHistos     = new TH1F * [nbOfPlanes];
    fEta6PlusPosHistos = new TH1F * [nbOfPlanes];
    
    fEta2NegHistos     = new TH1F * [nbOfPlanes];
    fEta3NegHistos     = new TH1F * [nbOfPlanes];
    fEta4NegHistos     = new TH1F * [nbOfPlanes];
    fEta5NegHistos     = new TH1F * [nbOfPlanes];
    fEta6PlusNegHistos = new TH1F * [nbOfPlanes];
    
    
    map <Int_t, TString >  detectorList = mapping->GetDetectorFromIDMap() ;
    map <Int_t, TString >::const_iterator det_itr ;
    for(det_itr = detectorList.begin(); det_itr != detectorList.end(); ++det_itr) {
        
        Int_t detID     = (*det_itr).first ;
        TString detName = (*det_itr).second ;
        
        list<TString> planeList = mapping->GetDetectorPlaneListFromDetector(detName) ;
        list<TString >::const_iterator plane_itr ;
        for(plane_itr = planeList.begin(); plane_itr != planeList.end(); ++plane_itr) {
            
            TString plane = *plane_itr ;
            Int_t planeID = (Int_t) mapping->GetPlaneIDorEtaSector(plane) ;
            
            Int_t histoID = (2 * detID) + planeID ;
            TString histoName = "etaFunction" + plane ;
            TString histoTitle = "eta Function: " + plane ;
            fEtaFunctionHistos[histoID] = BookHisto (histoName, histoTitle) ;
            
            histoName = "eta2FunctionPos" + plane ;
            histoTitle = "eta2 Function Pos: " + plane ;
            fEta2PosHistos[histoID] = BookHisto (histoName, histoTitle) ;
            
            histoName = "eta3FunctionPos" + plane ;
            histoTitle = "eta3 Function Pos: " + plane ;
            fEta3PosHistos[histoID] = BookHisto (histoName, histoTitle) ;
            
            histoName = "eta4FunctionPos" + plane ;
            histoTitle = "eta4 Function Pos: " + plane ;
            fEta4PosHistos[histoID] = BookHisto (histoName, histoTitle) ;
            
            histoName = "eta5FunctionPos" + plane ;
            histoTitle = "eta5 Function Pos: " + plane ;
            fEta5PosHistos[histoID] = BookHisto (histoName, histoTitle) ;
            
            histoName = "eta6PlusFunctionPos" + plane ;
            histoTitle = "eta6 and more Function Pos: " + plane ;
            fEta6PlusPosHistos[histoID] = BookHisto (histoName, histoTitle) ;
            
            
            histoName = "eta2FunctionNeg" + plane ;
            histoTitle = "eta2 Function Neg: " + plane ;
            fEta2NegHistos[histoID] = BookHisto (histoName, histoTitle) ;
            
            histoName = "eta3FunctionNeg" + plane ;
            histoTitle = "eta3 Function Neg: " + plane ;
            fEta3NegHistos[histoID] = BookHisto (histoName, histoTitle) ;
            
            histoName = "eta4FunctionNeg" + plane ;
            histoTitle = "eta4 Function Neg: " + plane ;
            fEta4NegHistos[histoID] = BookHisto (histoName, histoTitle) ;
            
            histoName = "eta5FunctionNeg" + plane ;
            histoTitle = "eta5 Function Neg: " + plane ;
            fEta5NegHistos[histoID] = BookHisto (histoName, histoTitle) ;
            
            histoName = "eta6PlusFunctionNeg" + plane ;
            histoTitle = "eta6 and more Function Neg: " + plane ;
            fEta6PlusNegHistos[histoID] = BookHisto (histoName, histoTitle) ;
            
            
            fEtaFunctionHistos[histoID]->SetXTitle("(cluster Pos - Central Strip Pos) / pitch ") ;
            fEta2PosHistos[histoID]->SetXTitle("(cluster Pos - Central Strip Pos) / pitch ") ;
            fEta3PosHistos[histoID]->SetXTitle("(cluster Pos - Central Strip Pos) / pitch ") ;
            fEta4PosHistos[histoID]->SetXTitle("(cluster Pos - Central Strip Pos) / pitch ") ;
            fEta5PosHistos[histoID]->SetXTitle("(cluster Pos - Central Strip Pos) / pitch ") ;
            fEta6PlusPosHistos[histoID]->SetXTitle("(cluster Pos - Central Strip Pos) / pitch ") ;
            
            fEtaFunctionHistos[histoID]->SetYTitle("Counts") ;
            fEta2PosHistos[histoID]->SetYTitle("Counts") ;
            fEta3PosHistos[histoID]->SetYTitle("Counts") ;
            fEta4PosHistos[histoID]->SetYTitle("Counts") ;
            fEta5PosHistos[histoID]->SetYTitle("Counts") ;
            fEta6PlusPosHistos[histoID]->SetYTitle("Counts") ;
            
            fEta2NegHistos[histoID]->SetXTitle("(cluster Pos - Central Strip Pos) / pitch ") ;
            fEta3NegHistos[histoID]->SetXTitle("(cluster Pos - Central Strip Pos) / pitch ") ;
            fEta4NegHistos[histoID]->SetXTitle("(cluster Pos - Central Strip Pos) / pitch ") ;
            fEta5NegHistos[histoID]->SetXTitle("(cluster Pos - Central Strip Pos) / pitch ") ;
            fEta6PlusNegHistos[histoID]->SetXTitle("(cluster Pos - Central Strip Pos) / pitch ") ;
            
            fEta2NegHistos[histoID]->SetYTitle("Counts") ;
            fEta3NegHistos[histoID]->SetYTitle("Counts") ;
            fEta4NegHistos[histoID]->SetYTitle("Counts") ;
            fEta5NegHistos[histoID]->SetYTitle("Counts") ;
            fEta6PlusNegHistos[histoID]->SetYTitle("Counts") ;
            
            
        }
        planeList.clear() ;
    }
    detectorList.clear() ;
}

//==================================================================================
TH1F * SRSPositionCorrection::BookHisto(TString histoName, TString histoTitle) {
    Float_t min  = -0.50  ;
    Float_t max  = 0.50 ;
    
    if(histoName.Contains("Neg")) {
        min  = -0.50  ;
        max  = 0 ;
    }
    
    if(histoName.Contains("Pos")) {
        min  = 0  ;
        max  = 0.5 ;
    }
    
    Int_t nbin = 250;
    TH1F * h = new TH1F(histoName,histoTitle, nbin, min, max);
    return h;
}

//============================================================================================
static Bool_t CompareStripNo( TObject *obj1, TObject *obj2) {
    Bool_t compare ;
    if ( ( (SRSHit*) obj1 )->GetStripNo() < ( ( SRSHit*) obj2 )->GetStripNo() ) compare = kTRUE ;
    else compare = kFALSE ;
    return compare ;
}

//============================================================================================
void SRSPositionCorrection::ComputeClustersInDetectorPlane() {
    
    // printf("==SRSPositionCorrection::ComputeClustersInDetectorPlane() \n") ;
    
    map<TString, list <SRSHit*> >::const_iterator  listOfHits_itr ;
    for (listOfHits_itr = fHitsInDetectorPlaneMap.begin(); listOfHits_itr != fHitsInDetectorPlaneMap.end(); ++listOfHits_itr) {
        TString detPlane =  (*listOfHits_itr).first ;
        SRSMapping * mapping = SRSMapping::GetInstance() ;
        TString detector = mapping->GetDetectorFromPlane(detPlane) ;
        TString readoutBoard = mapping->GetReadoutBoardFromDetector(detector) ;
        
        list <SRSHit*> listOfHits = (*listOfHits_itr).second ;
        listOfHits.sort(CompareStripNo) ;
        Int_t listSize = listOfHits.size() ;
        
        if (listSize < 2) {
            continue ;
        }
        
        Int_t previousStrip = - 2 ;
        Int_t clusterNo = - 1 ;
        map<Int_t, SRSCluster *> clustersMap ;
        list <SRSHit *>::const_iterator hit_itr ;
        for (hit_itr = listOfHits.begin(); hit_itr != listOfHits.end(); hit_itr++) {
            SRSHit * hit =  * hit_itr ;
            
            Int_t currentStrip = hit->GetStripNo() ;
            Int_t apvIndexOnPlane = hit->GetAPVIndexOnPlane();
            
            if(readoutBoard != "PADPLANE") {
                if(currentStrip != (previousStrip + 1)) {
                    clusterNo++ ;
                }
            }
            else {
                clusterNo++ ;
            }
            if(!clustersMap[clusterNo]) {
                clustersMap[clusterNo] = new SRSCluster(2, 10, "totalCharges") ;
                clustersMap[clusterNo]->SetNbAPVsFromPlane(hit->GetNbAPVsFromPlane());
                clustersMap[clusterNo]->SetPlaneSize(hit->GetPlaneSize());
                clustersMap[clusterNo]->SetPlane(hit->GetPlane());
            }
            clustersMap[clusterNo]->AddHit(hit) ;
            previousStrip = currentStrip;
        }
        map<Int_t, SRSCluster *>::const_iterator  cluster_itr ;
        for (cluster_itr = clustersMap.begin(); cluster_itr != clustersMap.end(); cluster_itr++) {
            SRSCluster * cluster = ( * cluster_itr ).second ;
            
            if (!cluster->IsGoodCluster() ) {
                delete cluster ;
                continue ;
            }
            cluster->SetClusterPositionCorrection(kFALSE) ;
            cluster->ComputeClusterPositionWithoutCorrection() ;
            fClustersInDetectorPlaneMap[detPlane].push_back(cluster) ;
        }
        listOfHits.clear() ;
        clustersMap.clear() ;
    }
}

//==================================================================================
void SRSPositionCorrection::FillClusterPositionCorrection() {
    // printf("SRSPositionCorrection::FillClusterPositionCorrection(): \n") ;
    
    SRSMapping * mapping = SRSMapping::GetInstance() ;
    
    map < TString, list <SRSCluster * > >::const_iterator detPlane_itr ;
    for(detPlane_itr = fClustersInDetectorPlaneMap.begin(); detPlane_itr != fClustersInDetectorPlaneMap.end(); ++detPlane_itr) {
        TString detPlane = (*detPlane_itr).first ;
        TString detector =   mapping->GetDetectorFromPlane(detPlane) ;
        
        Int_t detID = mapping->GetDetectorIDFromDetector(detector) ;
        Int_t planeID = (Int_t) mapping->GetPlaneIDorEtaSector(detPlane) ;
        
        Int_t histoID = (2 * detID) + planeID ;
        TString histoName ="etaFunction" + detPlane ;
        
        list <SRSCluster * > listOfClusters = (*detPlane_itr).second ;
        Int_t clustMult = listOfClusters.size() ;
        
        if(clustMult == 0) {
            listOfClusters.clear() ;
            continue ;
        }
        
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
            if(detPlane.Contains("EIC")) pitch = 2* pitch ;
            
            Float_t eta = (clusterPosition - clusterCentralStrip) / pitch ;
            
            fEtaFunctionHistos[histoID]->Fill(eta) ;
            
            if (eta > 0) {
                if(clusterSize == 2) fEta2PosHistos[histoID]->Fill(eta) ;
                if(clusterSize == 3) fEta3PosHistos[histoID]->Fill(eta) ;
                if(clusterSize == 4) fEta4PosHistos[histoID]->Fill(eta) ;
                if(clusterSize == 5) fEta5PosHistos[histoID]->Fill(eta) ;
                if(clusterSize >= 6) fEta6PlusPosHistos[histoID]->Fill(eta) ;
            }
            
            
            else {
                if(clusterSize == 2) fEta2NegHistos[histoID]->Fill(eta) ;
                if(clusterSize == 3) fEta3NegHistos[histoID]->Fill(eta) ;
                if(clusterSize == 4) fEta4NegHistos[histoID]->Fill(eta) ;
                if(clusterSize == 5) fEta5NegHistos[histoID]->Fill(eta) ;
                if(clusterSize >= 6) fEta6PlusNegHistos[histoID]->Fill(eta) ;
            }
        }
        
        listOfClusters.clear() ;
    }
    DeleteHitsInDetectorPlaneMap(fHitsInDetectorPlaneMap) ;
    DeleteClustersInDetectorPlaneMap(fClustersInDetectorPlaneMap) ;
}

//==================================================================================
void SRSPositionCorrection::LoadClusterPositionCorrection(const char * filename) {
    TFile f(filename);
    printf("  SRSPositionCorrection::LoadClusterPositionCorrection() ==> from %s  \n",filename) ;
    SRSMapping * mapping = SRSMapping::GetInstance() ;
    map <Int_t, TString >  detectorList = mapping->GetDetectorFromIDMap() ;
    map <Int_t, TString >::const_iterator det_itr ;
    for(det_itr = detectorList.begin(); det_itr != detectorList.end(); ++det_itr) {
        
        Int_t detID     = (*det_itr).first ;
        TString detName = (*det_itr).second ;
        
        list<TString> planeList = mapping->GetDetectorPlaneListFromDetector(detName) ;
        list<TString >::const_iterator plane_itr ;
        for(plane_itr = planeList.begin(); plane_itr != planeList.end(); ++plane_itr) {
            
            TString plane = *plane_itr ;
            Int_t planeID = (Int_t) mapping->GetPlaneIDorEtaSector(plane) ;
            
            Int_t histoID = (2 * detID) + planeID ;
            TString histoName = "etaFunction" + plane ;
            TString histoTitle = "eta Function: " + plane ;
            TH1F * EtaFuncHisto  = (TH1F *) f.Get(histoName) ;
            
            
            for (Int_t chNo = 0; chNo < 1000; chNo++) {
                Int_t binNumber = chNo + 1 ; // This is an issue with ROOT Histo bin numbering with:
                // ==========================================//
                // bin = 0 is underflow bin                  //
                // bin = 1 is the first bin of the histogram //
                // bin = nbin is the last bin                //
                // bin = nbin + 1 is the overflow bin        //
                //===========================================//
                
                Float_t eta  = EtaFuncHisto->GetBinContent(binNumber) ;
                fEtaFunctionHistos[histoID]->Fill(chNo,eta) ;
            }
        }
        planeList.clear() ;
    }
    detectorList.clear() ;
    
    fIsEtaFunctionComputed = kTRUE;
    f.Close() ;
}

//==================================================================================
void SRSPositionCorrection::SaveClusterPositionCorrectionHistos() {
    
    GetStyle() ;
    GetStyle() ;
    SRSMapping * mapping = SRSMapping::GetInstance() ;
    map <Int_t, TString >  detectorList = mapping->GetDetectorFromIDMap() ;
    map <Int_t, TString >::const_iterator det_itr ;
    for(det_itr = detectorList.begin(); det_itr != detectorList.end(); ++det_itr) {
        
        Int_t detID     = (*det_itr).first ;
        TString detName = (*det_itr).second ;
        
        list<TString> planeList = mapping->GetDetectorPlaneListFromDetector(detName) ;
        list<TString >::const_iterator plane_itr ;
        for(plane_itr = planeList.begin(); plane_itr != planeList.end(); ++plane_itr) {
            
            TString plane = *plane_itr ;
            Int_t planeID = (Int_t) mapping->GetPlaneIDorEtaSector(plane) ;
            
            Int_t histoID = (2 * detID) + planeID ;
            
            PolynomialFit(fEtaFunctionHistos[histoID]) ;
            fEtaFunctionHistos[histoID]->Write() ;
            
            PolynomialFit(fEta2PosHistos[histoID]) ;
            fEta2PosHistos[histoID]->Write() ;
            
            PolynomialFit(fEta3PosHistos[histoID]) ;
            fEta3PosHistos[histoID]->Write() ;
            
            PolynomialFit(fEta4PosHistos[histoID]) ;
            fEta4PosHistos[histoID]->Write() ;
            
            PolynomialFit(fEta5PosHistos[histoID]) ;
            fEta5PosHistos[histoID]->Write() ;
            
            PolynomialFit(fEta6PlusPosHistos[histoID]) ;
            fEta6PlusPosHistos[histoID]->Write() ;
            
            
            
            PolynomialFit(fEta2NegHistos[histoID]) ;
            fEta2NegHistos[histoID]->Write() ;
            
            PolynomialFit(fEta3NegHistos[histoID]) ;
            fEta3NegHistos[histoID]->Write() ;
            
            PolynomialFit(fEta4NegHistos[histoID]) ;
            fEta4NegHistos[histoID]->Write() ;
            
            PolynomialFit(fEta5NegHistos[histoID]) ;
            fEta5NegHistos[histoID]->Write() ;
            
            PolynomialFit(fEta6PlusNegHistos[histoID]) ;
            fEta6PlusNegHistos[histoID]->Write() ;
            
            
            TCanvas c("c1", "c1", 10,10,1610,810) ;
            c.cd();
            TString histoname = fEtaFunctionHistos[histoID]->GetName() ;
            TString picturename = fRunName + histoname + ".png" ;
            
            fEtaFunctionHistos[histoID]->Draw("") ;
            fEta2PosHistos[histoID]->Draw("")  ;
            fEta3PosHistos[histoID]->Draw("")  ;
            fEta4PosHistos[histoID]->Draw("")  ;
            fEta5PosHistos[histoID]->Draw("")  ;
            fEta6PlusPosHistos[histoID]->Draw("")  ;
            
            fEtaFunctionHistos[histoID]->UseCurrentStyle() ;
            fEta2PosHistos[histoID]->UseCurrentStyle()  ;
            fEta3PosHistos[histoID]->UseCurrentStyle()  ;
            fEta4PosHistos[histoID]->UseCurrentStyle()  ;
            fEta5PosHistos[histoID]->UseCurrentStyle()  ;
            fEta6PlusPosHistos[histoID]->UseCurrentStyle() ;
            
            
            fEta2NegHistos[histoID]->Draw("")  ;
            fEta3NegHistos[histoID]->Draw("")  ;
            fEta4NegHistos[histoID]->Draw("")  ;
            fEta5NegHistos[histoID]->Draw("")  ;
            fEta6PlusNegHistos[histoID]->Draw("")  ;
            
            fEta2NegHistos[histoID]->UseCurrentStyle()  ;
            fEta3NegHistos[histoID]->UseCurrentStyle()  ;
            fEta4NegHistos[histoID]->UseCurrentStyle()  ;
            fEta5NegHistos[histoID]->UseCurrentStyle()  ;
            fEta6PlusNegHistos[histoID]->UseCurrentStyle() ;
            
            c.SaveAs(picturename) ;
        }
        planeList.clear() ;
    }
    detectorList.clear() ;
}

//==================================================================================
SRSPositionCorrection * SRSPositionCorrection::GetClusterPositionCorrectionRootFile(const char * filename) {
    TFile * f = new TFile(filename,"read") ;
    SRSPositionCorrection *  clusterPositionCorrection = (SRSPositionCorrection *) f->Get("SRSPositionCorrection") ;
    printf("SRSPositionCorrection::GetClusterPositionCorrectionRootFile() ==> to load cluster position correction root file %s \n",filename) ;
    f->Close();
    return  clusterPositionCorrection;
}

//==================================================================================
void SRSPositionCorrection::PolynomialFit( TH1F* h) {
    gROOT->Reset();
    gStyle->SetOptFit(1110) ;
    gStyle->SetStatFontSize(0.02) ;
    gStyle->SetHistFillColor(kBlue) ;
    Double_t par[10];
    
    TString fitFuncName = h->GetName() ;
    fitFuncName += "_FIT" ;
    
    TF1 * poly9 = new TF1(fitFuncName, "pol9", -0.5, 0.5);
    poly9->GetParameters(&par[0]);
    poly9->SetLineColor(kRed);
    poly9->SetLineWidth(2) ;
    h->Fit(poly9,"WWR+");
}


//==================================================================================
void SRSPositionCorrection::GetStyle() {
    gROOT->Reset();
    gStyle->SetOptStat(0);
    gStyle->SetCanvasColor(0) ;
    gStyle->SetCanvasBorderMode(0) ;
    
    gStyle->SetLabelFont(62,"xyz");
    gStyle->SetLabelSize(0.03,"xyz");
    gStyle->SetLabelColor(1,"xyz");
    gStyle->SetTitleBorderSize(0) ;
    gStyle->SetTitleFillColor(0) ;
    gStyle->SetTitleSize(0.05,"xyz");
    gStyle->SetTitleOffset(1.5,"xy");
    gStyle->SetTitleOffset(1.,"z");
    gStyle->SetPalette(1);
    
    const Int_t NRGBs = 5;
    const Int_t NCont = 32;
    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);
}

//============================================================================================
void SRSPositionCorrection::DeleteClustersInDetectorPlaneMap( map<TString, list <SRSCluster * > > & stringListMap) {
    map < TString, list <SRSCluster *> >::const_iterator itr ;
    for (itr = stringListMap.begin(); itr != stringListMap.end(); itr++) {
        list <SRSCluster *> listOfClusters = (*itr).second ;
        list <SRSCluster *>::const_iterator clust_itr ;
        for ( clust_itr = listOfClusters.begin(); clust_itr != listOfClusters.end(); clust_itr++) {
            delete (* clust_itr) ;
        }
        listOfClusters.clear() ;
    }
    stringListMap.clear() ;
}

//============================================================================================
void SRSPositionCorrection::DeleteHitsInDetectorPlaneMap( map<TString, list <SRSHit * > > & stringListMap) {
    map < TString, list <SRSHit *> >::const_iterator itr ;
    for (itr = stringListMap.begin(); itr != stringListMap.end(); itr++) {
        list <SRSHit *> listOfHits = (*itr).second ;
        list <SRSHit *>::const_iterator hit_itr ;
        for (hit_itr = listOfHits.begin(); hit_itr != listOfHits.end(); hit_itr++) {
            delete (* hit_itr) ;
        }
        listOfHits.clear() ;
    }
    stringListMap.clear() ;
} 
