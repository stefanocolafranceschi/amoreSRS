#ifndef __SRSHISTO__
#define __SRSHISTO__
/*******************************************************************************
 *  AMORE FOR SRS - SRS                                                         *
 *  SRSHistoManager                                                             *
 *  SRS Module Class                                                            *
 *  Author: Kondo GNANVO 18/08/2010                                             *
 *******************************************************************************/

#if !defined(__CINT__) || defined(__MAKECINT__)

#include <map>
#include <fstream>
#include <iostream>
#include <string>
#include "event.h"
#include <cmath>

#include "TROOT.h"
#include "TFitResult.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TDatime.h"

#include "TRandom.h"
#include "TStyle.h"
#include "TLatex.h"

#include "TH1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH1I.h"
#include "TH2I.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMarker.h"
#include "TString.h"
#include "TFile.h"
#include "TMath.h"
#include "TLine.h"
#include "TBranch.h"
#include "TTree.h"
#include "TPad.h"
#include "TChain.h"
#include "TObject.h"
#include "TString.h"
#include "TKey.h"

#include "TColor.h"
#include "TNtuple.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TPRegexp.h"
#include "TSpectrum.h"
#include "TASImage.h"

#include "SRSHit.h"
#include "SRSTrack.h"
#include "SRSMapping.h"
#include "SRSAPVEvent.h"
#include "SRSPedestal.h"
#include "SRSTrackFit.h"
#include "SRSAlignment.h"
#include "SRSRawPedestal.h"
#include "SRSAPVSignalFit.h"
#include "SRSEventBuilder.h"
#include "SRSFECEventDecoder.h"
#include "SRSDoubleGaussianFit.h"

#endif
#define NCH 128
#define PI 3.14159265359

using namespace std;

class SRSHistoManager : public TObject {
    
public:
    SRSHistoManager(const char * histoCfgname, Int_t zeroSupCut, Int_t minClustSize, Int_t maxClustSize, Int_t maxClustMult);
    //  SRSHistoManager(const char * histoCfgname, TString runtype, TString runname, Int_t zeroSupCut, Int_t minClustSize, Int_t maxClustSize, Int_t maxClustMult);
    SRSHistoManager(){;}
    ~SRSHistoManager();
    
    void BookRawDataHisto() ;
    void BookAPVGainHisto() ;
    void BookHitTimingHisto() ;
    void BookAPVLatencyHisto() ;
    
    void BookResidualsHisto(SRSTrack * track) ;
    void BookEvent3DDisplayNtuple(SRSTrack * track) ;
    void BookClusterPositionCorrectionHistos() ;
    
    void BookCMSHisto(TString histoName, TString title, TString type);
    void Book1DHisto(TString name, TString title, TString type, TString sigName);
    void Book1DHisto(TString name, TString title, TString type, TString sigName1, TString sigName2);
    void Book2DHisto(TString name, TString title, TString type, TString sigName);
    void Book2DHisto(TString name, TString title, TString type, TString sigName1, TString sigName2);
    
    void FillRawDataHistos(SRSEventBuilder * eventbuilder) ;
    void FillAPV25GainHistos(SRSEventBuilder * eventbuilder) ;
    void FillClusterPositionCorrectionHistos(SRSEventBuilder * eventbuilder) ;
    
    void TrackFit(SRSTrack * track) ;
    void ComputeOffset(SRSTrack * track) ;
    void Event3DDisplay(SRSTrack * track) ;
    void FillResidualHistos(SRSTrack * track) ;
    vector <Float_t> CartesianToCylindricalCoordinates(Float_t offsetx, Float_t offsety,  vector <Float_t> cartesianCoordinates ) ;
    Float_t PlaneRotationAngle(vector <Float_t> referencePoint,  vector <Float_t> rotationPoint) ;
    Float_t XYOffsets(vector <Float_t> referencePoint,  vector <Float_t> prodedDetector, TString xORy) ;
    
    void PhysicsRun(SRSTrack * track, SRSEventBuilder * eventbuilder, const eventHeaderStruct * eventHeader) ;
    
    void PedestalRun(SRSPedestal * ped) ;
    void RawPedestalRun(SRSRawPedestal * ped) ;
    
    void GaussFit(TH1F* h) ;
    //  void GaussFit(TH1F* h, Float_t start, Float_t end) ;
    //  void LandauFit(TH1F* h, Float_t MostProb, Float_t spread) ;
    void LandauFit(TH1F* h) ;
    void DoubleGaussFit(TH1F *h, Int_t N_iter, Float_t N_sigma_range, Bool_t ShowFit) ;
    
    void DumpList();
    void ResetHistos();
    void RefreshHistos();
    void DeleteHistos();
    void DeleteROOTObjs() ;
    
    void ClearMaps();
    void ClearMapOfHistos( map<TString,  TH1* > mapOfHistos) ;
    void ClearMapOfVectors( map<TString, vector <TString> > mapOfVectors) ;
    
    void DeleteListOfClusters(TList * listOfClusters) ;
    
    void GetStyle(Bool_t setStats) ;
    void ReadHistoCfgFile(const char * histoCfgname) ;
    void SetHistoTitle(TString histoName, Int_t goodEvents, Int_t trigger) ;
    void SetHistoTitle(TString histoName) ;
    
    /**
     void SetRunFileParameters(TString filePrefix, TString fileValue) {
     fRunFilePrefix = filePrefix ;
     fRunFileValue = fileValue ;
     }
     
     void SetRunParameters(TString runtype, TString runname) {
     fRunType = runtype ;
     fRunName = runname ;
     }
     */
    
    void SetRunParameters(TString amoreAgentId, TString runtype, TString runname, TString filePrefix, TString fileValue) ;
    /**
     void SetRunParameters(TString amoreAgentId, TString runtype, TString runname, TString filePrefix, TString fileValue) {
     fAmoreAgentID = amoreAgentId ;
     fRunType = runtype ;
     fRunName = runname ;
     fRunFilePrefix = filePrefix ;
     fRunFileValue = fileValue ;
     }
     */
    
    
    void SetBinning(TString type, TString detPlane1, TString detPlane2);
    
    void Reset1DHistBinning(TString type, Int_t nbin, Float_t min, Float_t max);
    void Reset2DHistBinning(TString type, Int_t nbin, Float_t min, Float_t max, Int_t nbin2, Float_t min2, Float_t max2) ;
    
    Int_t GetPedestalOffset(SRSAPVEvent * apvEv, SRSPedestal * ped) ;
    
    void SaveFTBF_Residuals(SRSTrack * track) {
        SaveFTBF_XYOffsetsROOT(track);
        SaveFTBF_PlaneRotationROOT(track);
        SaveFTBF_ResidualsROOT(track);
        SaveFTBF_ResidualsTXT(track);
    }
    
    void SaveFTBF_AnalysisTXT(SRSTrack * track) ;
    void SaveFTBF_ResidualsTXT(SRSTrack * track);
    void SaveFTBF_ResidualsROOT(SRSTrack * track) ;
    void SaveFTBF_PlaneRotationROOT(SRSTrack * track) ;
    void SaveFTBF_XYOffsetsROOT(SRSTrack * track) ;
    
    void SaveAllHistos();
    void SaveAPV25GainHistos();
    void SavePosCorrectionHistos();
    
    void SaveRunNTuple() ;
    void SaveRunHistos(TObject* obj, Bool_t setStats) ;
    void SaveNTuplePlots(TObject* obj, Bool_t setStats ) ;
    
    TString GetVarType(TString name) {return fVarType[name];}
    
    map<TString, TObject*> GetAllObjectsToBePublished()     { return fObjToDraw;}
    map<TString, TH1*>     GetResidualHistoToBePublished()  { return fResidualHistos;}
    map<TString, TH1*>     GetPosCorrectionHistosToBePublished() { return fPosCorrectionHistos ;}
    
    TNtuple* GetNtupleToBePublished()  {return fNtuple;}
    
    void DeleteHitsInDetectorPlaneMap(map<TString, list <SRSHit * > > & stringListMap) ;
    void DeleteClustersInDetectorPlaneMap(map<TString, list <SRSCluster * > > & stringListMap) ;
    
private:
    
    SRSMapping * fMapping ;
    
    TString fRunType, fRunName, fRunFilePrefix, fRunFileValue, fAmoreAgentID;
    Int_t fZeroSupCut ;
    
    vector<TH1*> fHist1dToReset;
    vector<TH2*> fHist2dToReset;
    
    TNtuple * fNtuple ;
    map<TString, TH1*>    fResidualHistos, fPosCorrectionHistos;
    map<TString, TObject*> fObjToDraw;
    
    map<TString, Int_t> fNbOfEvents ;
    map<TString, TString> fObjType, fVarType, fObjTitle;
    
    map<TString, TString> fDetector, fDetectorPlane1, fDetectorPlane2, fFitFunction, fFitParam1Str, fFitParam2Str, fDividedHistos ;
    
    map<TString, vector <TString> > fFittedHistos ;
    map<TString, vector <TString> > fHistoTypeToDetectorMap ;
    
    map<TString, Int_t> fAPVKeyStr;
    map<TString, Int_t> fAPVIDStr;
    map<Int_t, TString> fNameAPVIDMap;
    
    Int_t   fNBin;
    Float_t fRangeMin, fRangeMax;
    
    Int_t   fNBin2;
    Float_t fRangeMin2, fRangeMax2;
    
    Int_t   fNBinPlane;
    Float_t fRangeMinPlane, fRangeMaxPlane;
    
    Int_t   fNBinTime;
    Float_t fRangeMinTime, fRangeMaxTime;
    
    Int_t   fNBinSpectrum;
    Float_t fRangeMinSpectrum, fRangeMaxSpectrum ;
    
    Int_t   fNBinMap;
    Float_t fRangeMinMap, fRangeMaxMap ;
    
    Int_t   fNBinMap2;
    Float_t fRangeMinMap2, fRangeMaxMap2 ;
    
    Int_t   f1DHistNBin ;
    Float_t f1DHistMinRange, f1DHistMaxRange;
    
    Float_t fNtupleSizeX, fNtupleSizeY, fNtupleSizeZ ;
    Int_t   fHistosRatioCut, fClustSizeRange, fMinClustSize, fMaxClustSize, fMaxClustMult;
    Bool_t  fIsFirstEvent ;       
    
    unsigned long long fTriggerPattern;
    UInt_t  fEventType;
    
    Int_t fEventNumber ;
    Float_t fRChi2, fMean, fMeanError, fSigma, fSigmaError ; 
    
    vector<TString>  fDetPlaneNameList ; 
    ClassDef(SRSHistoManager, 1)
};


#endif
