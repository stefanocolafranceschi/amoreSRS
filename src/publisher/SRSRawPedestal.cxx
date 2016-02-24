
#include "SRSRawPedestal.h"
ClassImp (SRSRawPedestal);

SRSRawPedestal::SRSRawPedestal(Int_t nbOfAPVs) :
fNbOfAPVs(nbOfAPVs), fNbOfChannels(NCH), fEventNb(-1) {
    Init(nbOfAPVs);
    fIsFirstEvent = kTRUE;
    fIsRawPedestalComputed = kFALSE;
}

//==================================================================================
SRSRawPedestal::SRSRawPedestal() {
    fIsFirstEvent = kTRUE;
    fIsRawPedestalComputed = kFALSE;
}

//==================================================================================
SRSRawPedestal::~SRSRawPedestal() {
    Clear();
    ClearMaps();
}

//==================================================================================
void SRSRawPedestal::Clear() {
    if (!fRMSDist.empty())
        fRMSDist.clear();
    
    if (!fNoises.empty())
        fNoises.clear();
    
    if (!fOffsets.empty())
        fOffsets.clear();
    
    if (!fRawPed2DHistos.empty())
        fRawPed2DHistos.clear();
    if (!fPedHistos.empty())
        fPedHistos.clear();
}

//==================================================================================
void SRSRawPedestal::Reset() {
    Clear();
}

//==================================================================================
void SRSRawPedestal::ClearMaps() {
    fRawPedestalData.clear();
}

//==================================================================================
void SRSRawPedestal::Init(Int_t nbOfAPVs) {
    Clear();
    fNbOfAPVs = nbOfAPVs;
    fIsFirstEvent = kTRUE;
    
    for (Int_t apvKey = 0; apvKey < fNbOfAPVs; apvKey++) {
        fNoises.push_back(BookHistos(apvKey, "rawNoise", "allstrips"));
        fOffsets.push_back(BookHistos(apvKey, "rawOffset", "allstrips"));
        fRawPed2DHistos.push_back(Book2DHistos(apvKey));
        
    }
    
    fRMSDist.push_back(new TH1F("allstripsAPVsRawPedestalRMSDist",
                                "allstripsAPVsRawPedestalRMSDist", 400, 0, 200));
    fRMSDist.push_back(new TH1F("allXstripsAPVsRawPedestalRMSDist",
                                "allXstripsAPVsRawPedestalRMSDist", 400, 0, 200));
    fRMSDist.push_back(new TH1F("allYstripsAPVsRawPedestalRMSDist",
                                "allYstripsAPVsRawPedestalRMSDist", 400, 0, 200));
    
    for (Int_t chNo = 0; chNo < NCH; chNo++) {
        for (Int_t apvKey = 0; apvKey < fNbOfAPVs; apvKey++) {
            stringstream out;
            out << chNo;
            TString chNoStr = out.str();
            fPedHistos.push_back(BookHistos(apvKey, "hped", chNoStr));
            
        }
    }
    
    printf("  SRSRawPedestal::Init() ==> leaving raw Pedestal init\n");
}

//==================================================================================
TH1F * SRSRawPedestal::BookHistos(Int_t apvKey, TString dataType,
                                  TString dataNb) {
    
    stringstream out;
    out << apvKey;
    TString apvKeyStr = out.str();
    
    Float_t min = -0.5;
    Float_t max = 127.5;
    Int_t nbin = 128;
    
    TString pedName = "apvNo_" + apvKeyStr + "_" + dataType + "_" + dataNb;
    
    if (dataType == "hped") {
        min = -2048;
        max = 2048;
        nbin = 4097;
    }
    
    TH1F * h = new TH1F(pedName, pedName, nbin, min, max);
    return h;
}

//==================================================================================
TH2F * SRSRawPedestal::Book2DHistos(Int_t apvKey) {
    
    stringstream out;
    out << apvKey;
    TString apvKeyStr = out.str();
    
    Float_t min = -0.5;
    Float_t max = 127.5;
    Int_t nbin = 128;
    
    TString pedName = "rawPed2D_apvNo" + apvKeyStr;
    
    min = 0;
    max = 100;
    nbin = 101;
    
    TH2F * h = new TH2F(pedName, pedName, 128, 0, 127, nbin, min, max);
    return h;
}

//==================================================================================
void SRSRawPedestal::FillRawPedestalHistos(
                                           SRSFECPedestalDecoder * pedestalDecoder) {
    fEventNb++;
    SRSMapping * mapping = SRSMapping::GetInstance();
    
    TList * listOfAPVEvents = pedestalDecoder->GetFECEvents();
    TIter nextAPVEvent(listOfAPVEvents);
    while (SRSAPVEvent * apvEvent = (SRSAPVEvent *) (nextAPVEvent())) {
        
        Int_t apvID = apvEvent->GetAPVID();
        Int_t apvKey = mapping->GetAPVNoFromID(apvID);
        
        //    printf("===>>>>>>>  SRSPedestal::FillRawPedestalHistos() ==> apvKey = %d,  apvID = %d \n",apvKey, apvID) ;
        apvEvent->ComputeMeanTimeBinRawPedestalData();
        
        Int_t chNo = 0;
        fRawPedestalData = apvEvent->GetRawPedestalData();
        
        vector<Float_t>::const_iterator rawData_itr;
        for (rawData_itr = fRawPedestalData.begin();
             rawData_itr != fRawPedestalData.end(); ++rawData_itr) {
            Float_t data = *rawData_itr;
            fPedHistos[fNbOfChannels * apvKey + chNo]->Fill(data);
            Int_t stripNo = apvEvent->StripMapping(chNo);
            fRawPed2DHistos[apvKey]->Fill(stripNo, fEventNb, data);
            ++chNo;
        }
        
    }
    
    listOfAPVEvents->Delete();
}

//==================================================================================
void SRSRawPedestal::ComputeRawPedestalData() {
    SRSMapping * mapping = SRSMapping::GetInstance();
    for (Int_t apvKey = 0; apvKey < fNbOfAPVs; apvKey++) {
        for (Int_t chNo = 0; chNo < NCH; chNo++) {
            Float_t offset =
            fPedHistos[fNbOfChannels * apvKey + chNo]->GetMean();
            Float_t noise = fPedHistos[fNbOfChannels * apvKey + chNo]->GetRMS();
            fNoises[apvKey]->Fill(chNo, noise);
            fOffsets[apvKey]->Fill(chNo, offset);
            Int_t apvID = mapping->GetAPVIDFromAPVNo(apvKey);
            TString apvName = mapping->GetAPVFromID(apvID);
            
            fRMSDist[0]->Fill(noise);
            if (apvName.Contains("X") || apvName.Contains("U"))
                fRMSDist[1]->Fill(noise);
            if (apvName.Contains("Y") || apvName.Contains("V"))
                fRMSDist[2]->Fill(noise);
        }
        
        fNoises[apvKey]->Write();
        fOffsets[apvKey]->Write();
    }
    
    fRMSDist[0]->Write();
    fRMSDist[1]->Write();
    fRMSDist[2]->Write();
    fIsRawPedestalComputed = kTRUE;
}

//==================================================================================
Float_t SRSRawPedestal::GetOnlinePedestalMean(Int_t apvID, Int_t chNo) {
    SRSMapping * mapping = SRSMapping::GetInstance();
    Int_t apvKey = mapping->GetAPVNoFromID(apvID);
    return fPedHistos[fNbOfChannels * apvKey + chNo]->GetMean();
    
}

//==================================================================================
Float_t SRSRawPedestal::GetOnlinePedestalRMS(Int_t apvID, Int_t chNo) {
    SRSMapping * mapping = SRSMapping::GetInstance();
    Int_t apvKey = mapping->GetAPVNoFromID(apvID);
    return fPedHistos[fNbOfChannels * apvKey + chNo]->GetRMS();
    
}

void SRSRawPedestal::LoadRawPedestalData(const char * filename) {
    TFile f(filename);
    printf(
           "  SRSRawPedestal::LoadRawPedestalData() ==> Compute the raw pedestals from %s for %d APVs \n",
           filename, fNbOfAPVs);
    
    for (Int_t apvKey = 0; apvKey < fNbOfAPVs; apvKey++) {
        stringstream out;
        out << apvKey;
        TString apvKeyStr = out.str();
        
        //The pedestals data
        TString noiseName = "apvNo_" + apvKeyStr + "_rawNoise_allstrips";
        TString offsetName = "apvNo_" + apvKeyStr + "_rawOffset_allstrips";
        
        TH1F * noiseHisto = (TH1F *) f.Get(noiseName);
        TH1F * offsetHisto = (TH1F *) f.Get(offsetName);
        
        for (Int_t chNo = 0; chNo < NCH; chNo++) {
            
            Int_t binNumber = chNo + 1; // This is an issue with ROOT Histo bin numbering with:
            // ==========================================//
            // bin = 0 is underflow bin                  //
            // bin = 1 is the first bin of the histogram //
            // bin = nbin is the last bin                //
            // bin = nbin + 1 is the overflow bin        //
            // ==========================================//
            
            Float_t noise = noiseHisto->GetBinContent(binNumber);
            Float_t offset = offsetHisto->GetBinContent(binNumber);
            fNoises[apvKey]->Fill(chNo, noise);
            fOffsets[apvKey]->Fill(chNo, offset);
            
        }
    }
    fIsRawPedestalComputed = kTRUE;
    f.Close();
}

//==================================================================================
Float_t SRSRawPedestal::GetNoise(Int_t apvID, Int_t chNo) {
    SRSMapping * mapping = SRSMapping::GetInstance();
    if (!fIsRawPedestalComputed) {
        Warning("SRSRawPedestal",
                "GetNoise:=> Pedestals & Noise not yet computed");
        return 0;
    }
    Int_t apvKey = mapping->GetAPVNoFromID(apvID);
    Int_t binNumber = chNo + 1; // This is an issue with ROOT Histo bin numbering with:
    // ==========================================//
    // bin = 0 is underflow bin                  //
    // bin = 1 is the first bin of the histogram //
    // bin = nbin is the last bin                //
    // bin = nbin + 1 is the overflow bin        //
    // ==========================================//
    Float_t noise = fNoises[apvKey]->GetBinContent(binNumber);
    return noise;
    
}

//==================================================================================
Float_t SRSRawPedestal::GetOffset(Int_t apvID, Int_t chNo) {
    SRSMapping * mapping = SRSMapping::GetInstance();
    if (!fIsRawPedestalComputed) {
        Warning("SRSRawPedestal",
                "GetPedestal:=> Pedestals & Noises not yet computed");
        return 0;
    }
    Int_t apvKey = mapping->GetAPVNoFromID(apvID);
    Int_t binNumber = chNo + 1; // This is an issue with ROOT Histo bin numbering with:
    // ==========================================//
    // bin = 0 is underflow bin                  //
    // bin = 1 is the first bin of the histogram //
    // bin = nbin is the last bin                //
    // bin = nbin + 1 is the overflow bin        //
    // ==========================================//
    Float_t offset = fOffsets[apvKey]->GetBinContent(binNumber);
    return offset;
}

//==================================================================================
vector<Float_t> SRSRawPedestal::GetAPVNoises(Int_t apvID) {
    SRSMapping * mapping = SRSMapping::GetInstance();
    vector < Float_t > apvNoises;
    if (!fIsRawPedestalComputed) {
        Warning(" ======= SRSRawPedestal",
                "GetNoise:=> Pedestals & Noise not yet computed");
        for (Int_t chNo = 1; chNo <= NCH; ++chNo)
            apvNoises.push_back(0);
    }
    Int_t apvKey = mapping->GetAPVNoFromID(apvID);
    for (Int_t chNo = 1; chNo <= NCH; ++chNo)
        apvNoises.push_back(fNoises[apvKey]->GetBinContent(chNo));
    return apvNoises;
}

//==================================================================================
vector<Float_t> SRSRawPedestal::GetAPVOffsets(Int_t apvID) {
    SRSMapping * mapping = SRSMapping::GetInstance();
    vector < Float_t > apvOffsets;
    if (!fIsRawPedestalComputed) {
        Warning(" ======= SRSRawPedestal",
                "GetPedestal:=> Pedestals & Noises not yet computed");
        for (Int_t chNo = 1; chNo <= NCH; ++chNo)
            apvOffsets.push_back(0);
    }
    Int_t apvKey = mapping->GetAPVNoFromID(apvID);
    for (Int_t chNo = 1; chNo <= NCH; ++chNo)
        apvOffsets.push_back(fOffsets[apvKey]->GetBinContent(chNo));
    return apvOffsets;
}

//==================================================================================
TH1F * SRSRawPedestal::GetPedHisto(Int_t apvID, Int_t chNo) {
    SRSMapping * mapping = SRSMapping::GetInstance();
    if (!fIsRawPedestalComputed) {
        Warning("GetPedestal", "Pedestals not yet computed");
        return 0;
    }
    Int_t apvKey = mapping->GetAPVNoFromID(apvID);
    return fPedHistos[fNbOfChannels * apvKey + chNo];
    
}

/**
 //==================================================================================
 SRSRawPedestal * SRSRawPedestal::GetRawPedestalRootFile(const char * filename){
 TFile f(filename);
 
 SRSRawPedestal * pedestalToUse = (SRSRawPedestal *) f.Get("SRSRawPedestal") ;
 printf("SRSRawPedestal::GetRawPedestalRootFile() ==> load raw pedestal root file %s \n",filename) ;
 f.Close();
 return pedestalToUse ;
 }
 */

//==================================================================================
SRSRawPedestal * SRSRawPedestal::GetRawPedestalRootFile(const char * filename) {
    TFile * f = new TFile(filename, "read");
    SRSRawPedestal * pedestalToUse = (SRSRawPedestal *) f->Get(
                                                               "SRSRawPedestal");
    printf(
           "SRSRawPedestal::GetRawPedestalRootFile() ==> load raw pedestal root file %s \n",
           filename);
    f->Close();
    return pedestalToUse;
}
//==================================================================================
void SRSRawPedestal::SaveRawPedestalRunHistos() {
    
    ComputeRawPedestalData();
    
    TCanvas c("c1", "c1", 80, 80, 1200, 800);
    c.cd();
    
    TString distname = fRMSDist[0]->GetName();
    TString distName = fRunName + "_rawPed.png";
    fRMSDist[0]->Draw("");
    fRMSDist[0]->SetYTitle("Frequency");
    fRMSDist[0]->SetXTitle("Pedestal RMS (ADC count)");
    gStyle->SetOptStat(1111);
    c.SaveAs(distName);
    
    distname = fRMSDist[0]->GetName();
    distName = fRunName + "_rawPedYStripsRMSDist.png";
    fRMSDist[1]->Draw("");
    fRMSDist[1]->SetYTitle("Frequency");
    fRMSDist[1]->SetXTitle("Y-Strips pedestal RMS (ADC count)");
    gStyle->SetOptStat(1111);
    c.SaveAs(distName);
    
    distname = fRMSDist[2]->GetName();
    distName = fRunName + "_rawPedXStripsRMSDist.png";
    fRMSDist[2]->Draw("");
    fRMSDist[2]->SetYTitle("Frequency");
    fRMSDist[2]->SetXTitle("X-Strips pedestal RMS (ADC count)");
    gStyle->SetOptStat(1111);
    c.SaveAs(distName);
    
    GetStyle();
    /*
     for (Int_t apvKey = 0; apvKey < fNbOfAPVs; apvKey++) {
     
     TString histoname = fRawPed2DHistos[apvKey]->GetName() ;
     TString picturename = fRunName + "_RawPed_" +  histoname + ".png" ;
     
     TCanvas c("c1", "c1", 80,80,1200,900)  ;
     
     fRawPed2DHistos[apvKey]->Draw("lego2") ;
     fRawPed2DHistos[apvKey]->SetXTitle("Strip No") ;
     fRawPed2DHistos[apvKey]->SetYTitle("Event No") ;
     fRawPed2DHistos[apvKey]->SetZTitle("ADC charges (A.U.)") ;
     fRawPed2DHistos[apvKey]->SetTitleSize(0.05,"xyz") ;
     fRawPed2DHistos[apvKey]->SetTitleOffset(1.5,"xy") ;
     fRawPed2DHistos[apvKey]->SetTitleOffset(1.,"z") ;
     c.SaveAs(picturename) ;
     
     histoname = fNoises[apvKey]->GetName() ;
     picturename = fRunName + "_RawPed_" +  histoname + ".png" ;
     fNoises[apvKey]->Draw("") ;
     fNoises[apvKey]->SetXTitle("APV Channel No") ;
     fNoises[apvKey]->SetYTitle("Pedestal Noise  (ADC count)") ;
     fNoises[apvKey]->UseCurrentStyle() ;
     c.SaveAs(picturename) ;
     
     histoname = fOffsets[apvKey]->GetName() ;
     picturename = fRunName + "_RawPed_" +  histoname + ".png" ;
     fOffsets[apvKey]->Draw("") ;
     fOffsets[apvKey]->SetXTitle("APV Channel No") ;
     fOffsets[apvKey]->SetYTitle("Pedestal Offset  (ADC count)") ;
     fOffsets[apvKey]->UseCurrentStyle() ;
     c.SaveAs(picturename) ;
     }
     */
    
}

//==================================================================================
void SRSRawPedestal::GetStyle() {
    gStyle->SetOptStat(0);
    gStyle->SetCanvasColor(0);
    gStyle->SetCanvasBorderMode(0);
    
    gStyle->SetLabelFont(62, "xyz");
    gStyle->SetLabelSize(0.03, "xyz");
    gStyle->SetLabelColor(1, "xyz");
    gStyle->SetTitleBorderSize(0);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleSize(0.05, "xyz");
    gStyle->SetTitleOffset(1.5, "xy");
    gStyle->SetTitleOffset(1., "z");
    gStyle->SetPalette(1);
    
    const Int_t NRGBs = 5;
    //  const Int_t NCont = 255;
    const Int_t NCont = 32;
    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs] = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs] = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);
}
