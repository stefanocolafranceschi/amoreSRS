
#include "SRSPedestal.h"
ClassImp (SRSPedestal);

SRSPedestal::SRSPedestal(Int_t nbOfAPVs, Int_t chMaskCut) :
fNbOfAPVs(nbOfAPVs), fNbOfChannels(NCH), fEventNb(-1), fChMaskCut(
                                                                  chMaskCut) {
    Init();
    fIsFirstEvent = kTRUE;
    fIsPedestalComputed = kFALSE;
    fIsMaskedChComputed = kFALSE;
}

//==================================================================================
SRSPedestal::SRSPedestal() {
    fIsFirstEvent = kTRUE;
    fIsPedestalComputed = kFALSE;
    fIsMaskedChComputed = kFALSE;
}

//==================================================================================
SRSPedestal::~SRSPedestal() {
    Clear();
    ClearMaps();
}

//==================================================================================
void SRSPedestal::Clear() {
    
    if (!fRMSDist.empty())
        fRMSDist.clear();
    
    if (!fNoises.empty())
        fNoises.clear();
    
    if (!fOffsets.empty())
        fOffsets.clear();
    
    if (!fMaskedChannels.empty())
        fMaskedChannels.clear();
    if (!fPedHistos.empty())
        fPedHistos.clear();
    if (!fPed2DHistos.empty())
        fPed2DHistos.clear();
}

//=========================================================================================================================
TString SRSPedestal::GetHistoName(Int_t apvKey, TString dataType,
                                  TString dataNb) {
    
    SRSMapping * mapping = SRSMapping::GetInstance();
    
    Int_t apvID = mapping->GetAPVIDFromAPVNo(apvKey);
    Int_t fecID = mapping->GetFECIDFromAPVID(apvID);
    Int_t adcCh = mapping->GetADCChannelFromAPVID(apvID);
    
    TString apvName = mapping->GetAPVFromID(apvID);
    stringstream out;
    
    out << apvID;
    TString apvIDStr = out.str();
    out.str("");
    
    out << fecID;
    TString fecIDStr = out.str();
    out.str("");
    
    out << adcCh;
    TString adcChStr = out.str();
    out.str("");
    
    out << apvKey;
    TString apvNoStr = out.str();
    out.str("");
    
    TString histoName = dataType + dataNb + "apvNo" + apvNoStr + apvName + "_Id"
    + apvIDStr + "_adcCh" + adcChStr + "_FecId" + fecIDStr;
    return histoName;
}

//==================================================================================
void SRSPedestal::ClearMaps() {
    fPedestalData.clear();
}

//==================================================================================
void SRSPedestal::Reset() {
    Clear();
}

//==================================================================================
void SRSPedestal::Init() {
    Clear();
    fIsFirstEvent = kTRUE;
    
    SRSMapping * mapping = SRSMapping::GetInstance();
    
    Int_t nbOfPlanes = mapping->GetNbOfDetectorPlane();
    
    for (Int_t apvKey = 0; apvKey < fNbOfAPVs; apvKey++) {
        fNoises.push_back(BookHistos(apvKey, "noise_", ""));
        fOffsets.push_back(BookHistos(apvKey, "offset_", ""));
        fMaskedChannels.push_back(BookHistos(apvKey, "maskedCh_", ""));
        fPed2DHistos.push_back(Book2DHistos(apvKey));
    }
    
    fRMSDist.push_back(new TH1F("allstripsAPVsPedestalRMSDist", "allstripsAPVsPedestalRMSDist", 300, 0, 30));
    fRMSDist.push_back(new TH1F("allXstripsAPVsPedestalRMSDist", "allXstripsAPVsPedestalRMSDist", 300, 0, 30));
    fRMSDist.push_back(new TH1F("allYstripsAPVsPedestalRMSDist","allYstripsAPVsPedestalRMSDist", 300, 0, 30));
    
    for (Int_t chNo = 0; chNo < NCH; chNo++) {
        for (Int_t apvKey = 0; apvKey < fNbOfAPVs; apvKey++) {
            stringstream out;
            out << chNo;
            TString chNoStr = out.str();
            fPedHistos.push_back(BookHistos(apvKey, "hped_", chNoStr));
        }
    }
    printf("  SRSPedestal::Init() ==> leaving Pedestal init\n");
}

//==================================================================================
TH1F * SRSPedestal::BookHistos(Int_t apvKey, TString dataType, TString dataNb) {
    stringstream out;
    out << apvKey;
    TString apvKeyStr = out.str();
    
    Float_t min = -0.5;
    Float_t max = 127.5;
    Int_t nbin = 128;
    
    TString histoName = GetHistoName(apvKey, dataType, dataNb);
    
    if (dataType.Contains("hped")) {
        min = -2048;
        max = 2048;
        nbin = 4097;
    }
    
    TH1F * h = new TH1F(histoName, histoName, nbin, min, max);
    //h->StatOverflows(true);
    return h;
}

//==================================================================================
TH2F * SRSPedestal::Book2DHistos(Int_t apvKey) {
    
    stringstream out;
    out << apvKey;
    TString apvKeyStr = out.str();
    
    Float_t min = -0.5;
    Float_t max = 127.5;
    Int_t nbin = 128;
    
    TString pedName = "ped2D_apvNo" + apvKeyStr;
    
    min = 0;
    max = 100;
    nbin = 101;
    
    TH2F * h = new TH2F(pedName, pedName, 128, 0, 127, nbin, min, max);
    //h->StatOverflows(true);
    return h;
}

//==================================================================================
void SRSPedestal::FillPedestalHistos(SRSFECPedestalDecoder * pedestalDecoder,
                                     SRSRawPedestal * rawped) {
    fEventNb++;
    SRSMapping * mapping = SRSMapping::GetInstance();
    
    TList * listOfAPVEvents = pedestalDecoder->GetFECEvents();
    TIter nextAPVEvent(listOfAPVEvents);
    
    while (SRSAPVEvent * apvEvent = (SRSAPVEvent *) nextAPVEvent()) {
        Int_t apvID = apvEvent->GetAPVID();
        Int_t apvKey = mapping->GetAPVNoFromID(apvID);
        
        apvEvent->SetRawPedestals(rawped->GetAPVNoises(apvID),
                                  rawped->GetAPVOffsets(apvID));
        apvEvent->ComputeMeanTimeBinPedestalData();
        
        Int_t chNo = 0;
        fPedestalData = apvEvent->GetPedestalData();
        vector<Float_t>::const_iterator fPedestalData_itr;
        for (fPedestalData_itr = fPedestalData.begin();
             fPedestalData_itr != fPedestalData.end(); ++fPedestalData_itr) {
            Float_t data = *fPedestalData_itr;
            fPedHistos[fNbOfChannels * apvKey + chNo]->Fill(data);
            Int_t stripNo = apvEvent->StripMapping(chNo);
            fPed2DHistos[apvKey]->Fill(stripNo, fEventNb, data);
            ++chNo;
        }
    }
    listOfAPVEvents->Delete();
}

//==================================================================================
void SRSPedestal::ComputePedestalData() {
    SRSMapping * mapping = SRSMapping::GetInstance();
    Int_t nbAPVs = mapping->GetNbOfAPVs();
    printf(
           "  SRSPedestal::ComputePedestal() ==> Compute the pedestals for %d APVs \n",
           nbAPVs);
    
    for (Int_t apvKey = 0; apvKey < nbAPVs; apvKey++) {
        //    printf("\n  SRSPedestal::ComputePedestal() ==> Compute the pedestals for APVs=%d \n",apvKey) ;
        
        for (Int_t chNo = 0; chNo < NCH; chNo++) {
            Float_t offset =
            fPedHistos[fNbOfChannels * apvKey + chNo]->GetMean();
            Float_t noise = fPedHistos[fNbOfChannels * apvKey + chNo]->GetRMS();
            fNoises[apvKey]->Fill(chNo, noise);
            fOffsets[apvKey]->Fill(chNo, offset);
            Int_t apvID = mapping->GetAPVIDFromAPVNo(apvKey);
            TString apvName = mapping->GetAPVFromID(apvID);
            fRMSDist[0]->Fill(noise);
            
            //      if(apvName.Contains("X")) fXorUstripsRMSDist->Fill(noise) ;
            //      if(apvName.Contains("Y")) fYorVstripsRMSDist->Fill(noise) ;
            
            if (apvName.Contains("X") || apvName.Contains("U")) {
                if (apvName.Contains("X")) {
                    fRMSDist[1]->SetName("allXstripsPedestalRMSDist");
                    fRMSDist[1]->SetTitle("allXstripsPedestalRMSDist");
                }
                if (apvName.Contains("U")) {
                    fRMSDist[1]->SetName("allUstripsPedestalRMSDist");
                    fRMSDist[1]->SetTitle("allUstripsPedestalRMSDist");
                }
                fRMSDist[1]->Fill(noise);
            }
            
            if (apvName.Contains("Y") || apvName.Contains("V")) {
                if (apvName.Contains("Y")) {
                    fRMSDist[2]->SetName("allYstripsPedestalRMSDist");
                    fRMSDist[2]->SetTitle("allYstripsPedestalRMSDist");
                }
                if (apvName.Contains("V")) {
                    fRMSDist[2]->SetName("allVstripsPedestalRMSDist");
                    fRMSDist[2]->SetTitle("allVstripsPedestalRMSDist");
                }
                fRMSDist[2]->Fill(noise);
            }
            
            //      printf("  SRSPedestal::ComputePedestal() ==> planeName=%s, chNo=%d, stripNoOnPlane=%d, noise=%f \n",  planeName.Data(), chNo, stripNoOnPlane, noise) ;
        }
        
        fNoises[apvKey]->Write();
        fOffsets[apvKey]->Write();
    }
    
    fRMSDist[0]->Write();
    fRMSDist[1]->Write();
    fRMSDist[2]->Write();
    
    fIsPedestalComputed = kTRUE;
}

//==================================================================================
void SRSPedestal::ComputeMaskedChannels() {
    
    Float_t meanNoise = fRMSDist[0]->GetMean();
    SRSMapping * mapping = SRSMapping::GetInstance();
    Int_t nbAPVs = mapping->GetNbOfAPVs();
    printf(
           "  SRSPedestal::ComputeMaskedChannels() ==> Compute the masked channels for %d APVs \n",
           nbAPVs);
    
    map<Int_t, vector<Float_t> > meanDetectorStripNoise;
    meanDetectorStripNoise.clear();
    
    map<TString, list<Int_t> > apvIDListFromDetNameMap =
    mapping->GetAPVIDListFromDetectorMap();
    map<TString, list<Int_t> >::const_iterator det_itr;
    for (det_itr = apvIDListFromDetNameMap.begin();
         det_itr != apvIDListFromDetNameMap.end(); ++det_itr) {
        TString detName = (*det_itr).first;
        list < Int_t > apvList = (*det_itr).second;
        list<Int_t>::const_iterator apv_itr;
        for (apv_itr = apvList.begin(); apv_itr != apvList.end(); ++apv_itr) {
            Int_t apvKey = mapping->GetAPVNoFromID(*apv_itr);
            for (Int_t chNo = 1; chNo <= NCH; chNo++) {
                meanDetectorStripNoise[apvKey].push_back(
                                                         fNoises[apvKey]->GetBinContent(chNo));
            }
        }
    }
    
    for (det_itr = apvIDListFromDetNameMap.begin();
         det_itr != apvIDListFromDetNameMap.end(); ++det_itr) {
        TString detName = (*det_itr).first;
        list < Int_t > apvList = (*det_itr).second;
        list<Int_t>::const_iterator apv_itr;
        
        for (apv_itr = apvList.begin(); apv_itr != apvList.end(); ++apv_itr) {
            Int_t apvKey = mapping->GetAPVNoFromID(*apv_itr);
            meanNoise = TMath::Mean(meanDetectorStripNoise[apvKey].begin(),
                                    meanDetectorStripNoise[apvKey].end());
            
            for (Int_t chNo = 0; chNo < NCH; chNo++) {
                Int_t binNumber = chNo + 1;
                Float_t noise = fNoises[apvKey]->GetBinContent(binNumber);
                fMaskedChannels[apvKey]->Fill(chNo, 0);
                Float_t cut = (Float_t)(fChMaskCut * meanNoise);
                
                if ((cut > 0) && (noise > cut)) {
                    fMaskedChannels[apvKey]->Fill(chNo, 1);
                    printf(
                           "  SRSPedestal::ComputeMaskedChannels(): detName = %s, apvNo = %d, noise = %f, meanNoise = %f, cut = %f\n",
                           detName.Data(), apvKey, noise, meanNoise, cut);
                }
            }
            fMaskedChannels[apvKey]->Write();
        }
        meanDetectorStripNoise.clear();
    }
    fIsMaskedChComputed = kTRUE;
}

//==================================================================================
Float_t SRSPedestal::GetOnlinePedestalMean(Int_t apvID, Int_t chNo) {
    SRSMapping * mapping = SRSMapping::GetInstance();
    Int_t apvKey = mapping->GetAPVNoFromID(apvID);
    return fPedHistos[fNbOfChannels * apvKey + chNo]->GetMean();
}

//==================================================================================
Float_t SRSPedestal::GetOnlinePedestalRMS(Int_t apvID, Int_t chNo) {
    SRSMapping * mapping = SRSMapping::GetInstance();
    Int_t apvKey = mapping->GetAPVNoFromID(apvID);
    return fPedHistos[fNbOfChannels * apvKey + chNo]->GetRMS();
}

//==================================================================================
void SRSPedestal::LoadPedestalData(const char * filename) {
    TFile f(filename);
    SRSMapping * mapping = SRSMapping::GetInstance();
    Int_t nbAPVs = mapping->GetNbOfAPVs();
    printf("  SRSPedestal::LoadPedestalData() ==> Compute the pedestals from %s for %d APVs \n", filename, nbAPVs);
    
    for (Int_t apvKey = 0; apvKey < nbAPVs; apvKey++) {
        stringstream out;
        out << apvKey;
        TString apvKeyStr = out.str();
        
        //The pedestals data
        TString noiseName = GetHistoName(apvKey, "noise_", "");
        TString offsetName = GetHistoName(apvKey, "offset_", "");
        TString maskedChName = GetHistoName(apvKey, "maskedCh_", "");
        
        TH1F * noiseHisto = (TH1F *) f.Get(noiseName);
        TH1F * offsetHisto = (TH1F *) f.Get(offsetName);
        TH1F * maskedChHisto = (TH1F *) f.Get(maskedChName);
        
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
            Float_t maskedCh = maskedChHisto->GetBinContent(binNumber);
            
            fNoises[apvKey]->Fill(chNo, noise);
            fOffsets[apvKey]->Fill(chNo, offset);
            fMaskedChannels[apvKey]->Fill(chNo, maskedCh);
        }
    }
    fIsMaskedChComputed = kTRUE;
    fIsPedestalComputed = kTRUE;
    f.Close();
}

//==================================================================================
Float_t SRSPedestal::GetMaskedChannelStatus(Int_t apvID, Int_t chNo) {
    SRSMapping * mapping = SRSMapping::GetInstance();
    if (!fIsPedestalComputed) {
        Warning("SRSPedestal",
                "GetMaskedChannelStatus:=> Pedestals & Noise not yet computed");
        return 0;
    }
    if (!fIsMaskedChComputed) {
        Warning("SRSPedestal",
                "GetMaskedChannelStatus:=> masked channel not yet computed");
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
    return fMaskedChannels[apvKey]->GetBinContent(binNumber);
}

//==================================================================================
Float_t SRSPedestal::GetNoise(Int_t apvID, Int_t chNo) {
    SRSMapping * mapping = SRSMapping::GetInstance();
    if (!fIsPedestalComputed) {
        Warning("SRSPedestal",
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
    return fNoises[apvKey]->GetBinContent(binNumber);
}

//==================================================================================
Float_t SRSPedestal::GetOffset(Int_t apvID, Int_t chNo) {
    SRSMapping * mapping = SRSMapping::GetInstance();
    if (!fIsPedestalComputed) {
        Warning("SRSPedestal",
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
    return fOffsets[apvKey]->GetBinContent(binNumber);
}

//==================================================================================
vector<Float_t> SRSPedestal::GetAPVNoises(Int_t apvID) {
    SRSMapping * mapping = SRSMapping::GetInstance();
    vector < Float_t > apvNoises;
    if (!fIsPedestalComputed) {
        Warning("SRSPedestal",
                "GetAPVNoise:=> Pedestals & Noise not yet computed");
        for (Int_t chNo = 1; chNo <= NCH; ++chNo)
            apvNoises.push_back(0);
    }
    Int_t apvKey = mapping->GetAPVNoFromID(apvID);
    for (Int_t chNo = 1; chNo <= NCH; ++chNo)
        apvNoises.push_back(fNoises[apvKey]->GetBinContent(chNo));
    //  printf("  SRSPedestal::GetAPVNoises() ==> Got the pedestals \n") ;
    return apvNoises;
}

//==================================================================================
vector<Float_t> SRSPedestal::GetAPVOffsets(Int_t apvID) {
    SRSMapping * mapping = SRSMapping::GetInstance();
    vector < Float_t > apvOffsets;
    if (!fIsPedestalComputed) {
        Warning("SRSPedestal",
                "GetAPVOffsets:=> Pedestals & Noises not yet computed");
        for (Int_t chNo = 1; chNo <= NCH; ++chNo)
            apvOffsets.push_back(0);
    }
    Int_t apvKey = mapping->GetAPVNoFromID(apvID);
    for (Int_t chNo = 1; chNo <= NCH; ++chNo) {
        apvOffsets.push_back(fOffsets[apvKey]->GetBinContent(chNo));
    }
    //  printf("  SRSPedestal::GetAPVOffsets() ==> Got the pedestals \n") ;
    return apvOffsets;
}

//==================================================================================
vector<Float_t> SRSPedestal::GetAPVMaskedChannels(Int_t apvID) {
    SRSMapping * mapping = SRSMapping::GetInstance();
    vector < Float_t > apvMaskedChannels;
    if (!fIsMaskedChComputed) {
        Warning("SRSPedestal",
                "GetAPVMaskedChannels:=> Pedestals & Noises not yet computed");
        for (Int_t chNo = 1; chNo <= NCH; ++chNo)
            apvMaskedChannels.push_back(0);
    }
    Int_t apvKey = mapping->GetAPVNoFromID(apvID);
    for (Int_t chNo = 1; chNo <= NCH; ++chNo)
        apvMaskedChannels.push_back(
                                    fMaskedChannels[apvKey]->GetBinContent(chNo));
    //  printf("  SRSPedestal::GetAPVMaskedChannels() ==> Got the pedestals \n") ;
    return apvMaskedChannels;
}

//==================================================================================
TH1F * SRSPedestal::GetPedHisto(Int_t apvID, Int_t chNo) {
    SRSMapping * mapping = SRSMapping::GetInstance();
    if (!fIsPedestalComputed) {
        Warning("GetPedestal", "Pedestals not yet computed");
        return 0;
    }
    Int_t apvKey = mapping->GetAPVNoFromID(apvID);
    return fPedHistos[fNbOfChannels * apvKey + chNo];
}

//==================================================================================
SRSPedestal * SRSPedestal::GetPedestalRootFile(const char * filename) {
    
    TFile * f = new TFile(filename, "read");
    SRSPedestal * pedestalToUse = (SRSPedestal *) f->Get("SRSPedestal");
    //  printf("SRSPedestal::GetPedestalRootFile() ==> load pedestal root file %s \n",filename) ;
    f->Close();
    return pedestalToUse;
}

//==================================================================================
void SRSPedestal::SavePedestalRunHistos() {
    
    ComputePedestalData();
    ComputeMaskedChannels();
    
    SRSMapping * mapping = SRSMapping::GetInstance();
    Int_t nbAPVs = mapping->GetNbOfAPVs();
    
    printf("SRSPedestal::SavePedestalRunHistos() ==> Get Mapping \n");
    
    TCanvas c("c1", "c1", 80, 80, 1200, 600);
    c.cd();
    
    TString distname = fRMSDist[0]->GetName();
    //  TString distName = fRunName + "pedRMSDist.png" ;
    TString distName = fRunName + "_" + distname + ".png";
    fRMSDist[0]->Draw("");
    fRMSDist[0]->SetYTitle("Frequency");
    fRMSDist[0]->SetXTitle("Pedestal RMS (ADC count)");
    gStyle->SetOptStat(1111);
    c.SaveAs(distName);
    
    distname = fRMSDist[1]->GetName();
    //  distName = fRunName + "pedYStripsRMSDist.png" ;
    distName = fRunName + "_" + distname + ".png";
    fRMSDist[1]->Draw("");
    fRMSDist[1]->SetYTitle("Frequency");
    fRMSDist[1]->SetXTitle("Y-Strips pedestal RMS (ADC count)");
    gStyle->SetOptStat(1111);
    c.SaveAs(distName);
    
    distname = fRMSDist[2]->GetName();
    //  distName = fRunName + "pedXStripsRMSDist.png" ;
    distName = fRunName + "_" + distname + ".png";
    fRMSDist[2]->Draw("");
    fRMSDist[2]->SetYTitle("Frequency");
    fRMSDist[2]->SetXTitle("X-Strips pedestal RMS (ADC count)");
    gStyle->SetOptStat(1111);
    c.SaveAs(distName);
    
    printf("SRSPedestal::SavePedestalRunHistos() ==> save ped dist plots \n");
    
    GetStyle();
    
    for (Int_t apvKey = 0; apvKey < nbAPVs; apvKey++) {
        
        TString histoname = fNoises[apvKey]->GetName();
        TString picturename = fRunName + "_ped_" + histoname + ".png";
        fNoises[apvKey]->Draw("");
        fNoises[apvKey]->UseCurrentStyle();
        fNoises[apvKey]->SetXTitle("APV Channel No");
        fNoises[apvKey]->SetYTitle("Pedestal Noise  (ADC counts)");
        c.SaveAs(picturename);
        /*
         histoname = fPed2DHistos[apvKey]->GetName() ;
         picturename = fRunName + "_ped_" +  histoname + ".png" ;
         fPed2DHistos[apvKey]->Draw("LEGO2") ;
         fPed2DHistos[apvKey]->SetXTitle("APV Channel No") ;
         fPed2DHistos[apvKey]->SetYTitle("Event No") ;
         fPed2DHistos[apvKey]->SetZTitle("ADC charge (A.U.)") ;
         c.SaveAs(picturename) ;
         
         histoname = fOffsets[apvKey]->GetName() ;
         picturename = fRunName + "_ped_" +  histoname + ".png" ;
         fOffsets[apvKey]->Draw("") ;
         fOffsets[apvKey]->UseCurrentStyle() ;
         fOffsets[apvKey]->SetXTitle("APV Channel No") ;
         fOffsets[apvKey]->SetYTitle("Pedestal Offset  (ADC counts)") ;
         c.SaveAs(picturename) ;
         
         histoname = fMaskedChannels[apvKey]->GetName() ;
         picturename = fRunName + "_ped_" +  histoname + ".png" ;
         fMaskedChannels[apvKey]->Draw("") ;
         fMaskedChannels[apvKey]->UseCurrentStyle() ;
         fMaskedChannels[apvKey]->SetXTitle("APV Channel No") ;
         fMaskedChannels[apvKey]->SetYTitle("maskChannels") ;
         c.SaveAs(picturename) ;
         */
    }
    
}

//==================================================================================
void SRSPedestal::GetStyle() {
    gStyle->SetOptStat(0);
    gStyle->SetCanvasColor(0);
    gStyle->SetCanvasBorderMode(0);
    
    gStyle->SetLabelFont(62, "xyz");
    gStyle->SetLabelSize(0.03, "xyz");
    gStyle->SetLabelColor(1, "xyz");
    gStyle->SetTitleBorderSize(0);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleSize(0.05, "xyz");
    gStyle->SetTitleOffset(1., "xy");
    gStyle->SetTitleOffset(1., "z");
    gStyle->SetPalette(1);
    
    const Int_t NRGBs = 5;
    const Int_t NCont = 32;
    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs] = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs] = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);
}
