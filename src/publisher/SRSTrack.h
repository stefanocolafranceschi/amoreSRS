#ifndef __SRSTRACK__
#define __SRSTRACK__
/*******************************************************************************
 *  AMORE FOR SRS - SRS                                                         *
 *  SRSTrack                                                                    *
 *  SRS Module Class                                                            *
 *  Author: Kondo GNANVO, Mike Staib 01/05/2011                                 *
 *******************************************************************************/
#if !defined(__CINT__) || defined(__MAKECINT__)

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "TObject.h"
#include "TString.h"
#include "SRSCluster.h"
#include "SRSMapping.h"
#include "SRSConfiguration.h"
#include "SRSEventBuilder.h"
#include "SRSTrackFit.h"

#include "event.h"
#include "TRandom2.h"

#endif
#define PI 3.14159265359

using namespace std ;

class SRSTrack : public TObject {
    
public:
    
    SRSTrack(const char * cfgname, TString offsetDir, TString zeroSupCut, TString maxClustSize, TString minClustSize, TString maxClustMult, TString amoreAgentId) ;
    ~SRSTrack();
    
    void DoTracking() ;
    void BuildTrack() ;
    void BuildRawDataSpacePoints(SRSEventBuilder * eventbuilder) ;
    
    Bool_t IsAGoodEvent() {return fIsGoodEvent ;}
    Bool_t IsAGoodTrack(SRSEventBuilder * eventbuilder) ;
    Bool_t IsTrigger(TString detName) ;
    Bool_t IsTracker(TString detName) ;
    
    void SetDetectorConfig(TString detName, TString triggerType, TString trackerType, Float_t zOffset, Float_t xinputRes, Float_t yinputRes, Int_t xnbin, Float_t xmin, Float_t xmax, Int_t ynbin, Float_t ymin, Float_t ymax) ;
    
    void ClearSpacePoints(map<TString, vector<Float_t> > & spacePointMap) ;
    void DeleteClustersInDetectorPlaneMap( map<TString, list <SRSCluster * > >  & stringListMap) ;
    void LoadFTBFalignementParametersRootFile(TString offsetDir);
    void ReadCfg(const char * cfgname);
    
    Int_t GetXNBinResiduals(TString detName)       { return fXNBinResiduals[detName]; }
    Float_t GetXRangeMaxResiduals(TString detName) { return fXRangeMaxResiduals[detName]; }
    Float_t GetXRangeMinResiduals(TString detName) { return fXRangeMinResiduals[detName]; }
    
    Int_t GetYNBinResiduals(TString detName)       { return fYNBinResiduals[detName]; }
    Float_t GetYRangeMaxResiduals(TString detName) { return fYRangeMaxResiduals[detName]; }
    Float_t GetYRangeMinResiduals(TString detName) { return fYRangeMinResiduals[detName]; }
    
    Int_t GetPHINBinResiduals(TString detName)       { return fPHINBinResiduals[detName]; }
    Float_t GetPHIRangeMaxResiduals(TString detName) { return fPHIRangeMaxResiduals[detName]; }
    Float_t GetPHIRangeMinResiduals(TString detName) { return fPHIRangeMinResiduals[detName]; }
    
    Int_t GetRNBinResiduals(TString detName)       { return fRNBinResiduals[detName]; }
    Float_t GetRRangeMaxResiduals(TString detName) { return fRRangeMaxResiduals[detName]; }
    Float_t GetRRangeMinResiduals(TString detName) { return fRRangeMinResiduals[detName]; }
    
    
    /**
     void SetRunFilePrefix(TString filePrefix) { fRunFilePrefix = filePrefix; }
     void SetRunFileValue(TString fileValue)   { fRunFileValue = fileValue; }
     */
    
    TString GetRunFilePrefix() { return fRunFilePrefix; }
    TString GetRunFileValue()  { return fRunFileValue; }
    
    TString GetNtupleName()  { return fNtupleName; }
    TString GetNtupleTitle() { return fNtupleTitle; }
    
    Float_t GetNtupleSizeX() { return fNtupleSizeX;}
    Float_t GetNtupleSizeY() { return fNtupleSizeY;}
    Float_t GetNtupleSizeZ() { return fNtupleSizeZ;}
    
    map<TString, Float_t> GetFitParameters() { return fFitParameters ; }
    
    map<TString, vector<Float_t> > GetTrackSpacePoints()        {return fTrackSpacePointMap;}
    map<TString, vector<Float_t> > GetFittedSpacePoints()       {return fFittedSpacePointMap;}
    map<TString, vector<Float_t> > GetRawDataSpacePoints()      {return fRawDataSpacePointMap;}
    map<TString, vector<Float_t> > GetEICstripClusterRawDataY() {return fEICstripClusterRawDataYMap;}
    
    map<TString, TString> GetTriggerList()  {return fTriggerList; }
    map<TString, TString> GetDetectorList() {return fDetectorList; }
    map<TString, TString> GetTrackerList()  {return fTrackerList; }
    
    map<TString, Int_t>   GetXNBinResiduals()     {return fXNBinResiduals; }
    map<TString, Float_t> GetXRangeMinResiduals() {return fXRangeMinResiduals; }
    map<TString, Float_t> GetXRangeMaxResiduals() {return fXRangeMaxResiduals; }
    
    map<TString, Int_t>   GetYNBinResiduals()     {return fYNBinResiduals; }
    map<TString, Float_t> GetYRangeMinResiduals() {return fYRangeMinResiduals; }
    map<TString, Float_t> GetYRangeMaxResiduals() {return fYRangeMaxResiduals; }
    
    map<TString, Int_t>   GetRNBinResiduals()     {return fRNBinResiduals; }
    map<TString, Float_t> GetRRangeMinResiduals() {return fRRangeMinResiduals; }
    map<TString, Float_t> GetRRangeMaxResiduals() {return fRRangeMaxResiduals; }
    
    map<TString, Int_t>   GetPHINBinResiduals()     {return fPHINBinResiduals; }
    map<TString, Float_t> GetPHIRangeMinResiduals() {return fPHIRangeMinResiduals; }
    map<TString, Float_t> GetPHIRangeMaxResiduals() {return fPHIRangeMaxResiduals; }
    
    map<TString, Float_t> GetDetXOffset() {return fDetXOffset; }
    map<TString, Float_t> GetDetYOffset() {return fDetYOffset; }
    map<TString, Float_t> GetDetZOffset() {return fDetZOffset; }
    map<TString, Float_t> GetDetPlaneRotationCorrection() {return fDetPlaneRotationCorrection; }
    
    map<TString, Float_t> GetDetXPlaneInputResolution() {return fDetXPlaneInputResolution; }
    map<TString, Float_t> GetDetYPlaneInputResolution() {return fDetYPlaneInputResolution; }
    
    void PlaneRotationCorrection(Float_t angle, vector<Float_t> & u) ;
    
    const vector<Float_t> subVec(const vector<Float_t> u, const vector<Float_t> v) ;
    const vector<Float_t> addVec(const vector<Float_t> u, const vector<Float_t> v) ;
    const vector<Float_t> prodVec(Float_t a, const vector<Float_t> u) ;
    const vector<Float_t> getDirection(const vector<Float_t> u) ;
    const vector<Float_t> getXandYKnowingZ(const vector<Float_t> w, const vector<Float_t> v, Float_t z0) ;
    const vector<Float_t> directionVectorFrom2Points(const vector<Float_t> u, const vector<Float_t> v) ;
    
    Float_t abs(Float_t x) ;
    Float_t dotVec(const vector<Float_t> u, const vector<Float_t> v) ;
    Float_t normVec(const vector<Float_t> u) ;
    Float_t getAngleTo(const vector<Float_t> u, const vector<Float_t> v) ;
    Float_t getAngleAmplitude(const vector<Float_t> u, const vector<Float_t> v) ;
    Float_t projectedAngleXY(const vector<Float_t> u, TString xORy) ;
    Float_t projectedAngleXZ(const vector<Float_t> u, TString xORz) ;
    
    TString GetDetectorType(TString detName) { return fDetectorList[detName]; }
    
private:
    
    Int_t fZeroSupCut, fMaxClusterSize, fMinClusterSize, fMaxClusterMultiplicity ;
    Float_t fNtupleSizeX, fNtupleSizeY, fNtupleSizeZ;
    TString fNtupleTitle, fNtupleName, fRunFilePrefix, fRunFileValue, fOffsetFilename, fAmoreAgentID;
    
    map<TString, TString> fTriggerList, fDetectorList, fTrackerList;
    map<TString, Int_t>   fXNBinResiduals, fYNBinResiduals,  fRNBinResiduals, fPHINBinResiduals;
    
    map<TString, Float_t> fDetXOffset, fDetYOffset, fDetZOffset, fDetXPlaneInputResolution,fDetYPlaneInputResolution, fDetPlaneRotationCorrection;
    
    map<TString, Float_t> fXRangeMinResiduals, fXRangeMaxResiduals, fYRangeMinResiduals, fYRangeMaxResiduals ;
    map<TString, Float_t> fRRangeMinResiduals, fRRangeMaxResiduals, fPHIRangeMinResiduals, fPHIRangeMaxResiduals ;
    
    map<TString, Float_t> fFitParameters ;
    
    map<TString,vector<Float_t> > fTrackSpacePointMap,  fFittedSpacePointMap, fRawDataSpacePointMap, fEICstripClusterRawDataYMap;
    
    Float_t fAngleCutMinX, fAngleCutMinY, fAngleCutMaxX, fAngleCutMaxY;
    Bool_t fIsGoodEvent,  fIsGoodTrack ;
    
    ClassDef(SRSTrack,1)
};

#endif

