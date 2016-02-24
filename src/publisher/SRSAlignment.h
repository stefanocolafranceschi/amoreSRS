#ifndef __SRSALIGNMENT__
#define __SRSALIGNMENT__
/*******************************************************************************
 *  AMORE FOR SRS - SRS                                                         *
 *  SRSAlignment                                                                *
 *  SRS Module Class                                                            *
 *  Author: Mike Staib 12/05/2011                                               *
 *******************************************************************************/
#if !defined(__CINT__) || defined(__MAKECINT__)

#include <iostream>
#include <vector>
#include <map>

#include "TObject.h"
#include "TCanvas.h"
#include "TH1F.h"

#include "SRSTrack.h"
#include "SRSTrackFit.h"
#endif

#define PI 3.14159265359

using namespace std ;

class SRSAlignment : public TObject {
    
public:
    
    SRSAlignment();
    ~SRSAlignment();
    
    void setSmallNumber(Float_t small_number) { smallNumber = small_number ;};
    
    Float_t fromRadianToDegree(Float_t angle_rad)  { return (Float_t) ((180*angle_rad) / PI);};
    Float_t fromDegreeToRadian(Float_t angle_deg)  { return (Float_t) ((PI*angle_deg) / 180);};
    
    Float_t getSmallNumber()  { return smallNumber ;};
    
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
    
    void TrackCoordinates();
    void ClearTrackCoordinates();
    
    map <Float_t, Float_t> GetTrackFitData(map<Float_t,Float_t> rawData);
    Float_t GetAngleFromTracks(vector<Float_t> inX, vector<Float_t> inY, vector<Float_t> inZ, vector<Float_t> outX, vector<Float_t> outY, vector<Float_t> outZ);
    
private:
    
    Float_t fTrackOffset, fTrackDirection ;
    Float_t smallNumber ;
    
    SRSTrack * fSRSTrack;
    vector<Float_t> fTrackX,  fTrackY, fTrackZ ;
    
    ClassDef(SRSAlignment,1)
};

#endif
