#ifndef SRSTRACKFIT_H
#define SRSTRACKFIT_H
/*******************************************************************************
 *  AMORE FOR SRS - SRS                                                         *
 *  SRSTrackFit                                                                 *
 *  SRS Module Class                                                            *
 *  Author: Kondo GNANVO 01/05/2011                                             *
 *******************************************************************************/

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <map>
#include <vector>
#include <TMath.h>
#include <iostream>
#include <TObject.h>
#include "TCanvas.h"
#include "TH1F.h"
#include "TString.h"

#endif

#define PI 3.14159265359

using namespace std;

class SRSTrackFit : public TObject {
    
public:
    
    SRSTrackFit(map<TString, vector<Float_t> > trackerData, map<TString, vector<Float_t> > rawData, map<TString, Float_t > xinputRes, map<TString, Float_t > yinputRes );
    ~SRSTrackFit();
    
    void WeightedLeastSquareLinearFitTrack() ;
    
    map<TString, Float_t> GetFitParameters() { return fFitParameters ; }
    
    map<TString, vector<Float_t> > GetTrackerData()    { return fTrackerData ;}
    map<TString, vector<Float_t> > GetTrackFittedData() { return fTrackFittedData ;}
    
    Float_t GetSmallNumber()  { return smallNumber ;}
    void setSmallNumber(Float_t small_number) { smallNumber = small_number ;}
    
    Float_t convertRadianToDegree(Float_t angle_rad)  { return (Float_t) ((180*angle_rad) / PI);}
    Float_t convertDegreeToRadian(Float_t angle_deg)  { return (Float_t) ((PI*angle_deg) / 180);}
    
    Float_t abs(Float_t x) ;
    Float_t normVec(const vector<Float_t> u) ;
    Float_t dotVec(const vector<Float_t> u ,const vector<Float_t> v) ;
    Float_t getAngleTo(const vector<Float_t> u, const vector<Float_t> v) ;
    Float_t projectedAngleXY(const vector<Float_t> u, TString xORy) ;
    Float_t projectedAngleXZ(const vector<Float_t> u, TString xORz) ;
    
    const vector<Float_t> prodVec(Float_t a , const vector<Float_t> u) ;
    const vector<Float_t> addVec(const vector<Float_t> u ,const vector<Float_t> v) ;
    const vector<Float_t> subVec(const vector<Float_t> u ,const vector<Float_t> v) ;
    const vector<Float_t> getDirection(const vector<Float_t> u) ;
    const vector<Float_t> directionVectorFrom2Points(const vector<Float_t> u, const vector<Float_t> v) ;
    const vector<Float_t> getXandYKnowingZ(const vector<Float_t> w, const vector<Float_t> v, Float_t z0) ;
    
    void PlaneRotationAlongZaxis(Float_t alpha, vector<Float_t> & u) ;
    Float_t GetAngleBetweenTwoTracks(map<TString, vector<Float_t> > firstTrack, map<TString, vector<Float_t> > secondTrack) ;
    
    void ClearTracks();
    
private:
    
    Float_t smallNumber ;
    map<TString, vector<Float_t> > fTrackerData, fTrackFittedData, fRawData;
    map<TString, Float_t> fFitParameters, fDetYPlaneInputResolution, fDetXPlaneInputResolution;;
    ClassDef(SRSTrackFit,1)
};

#endif
