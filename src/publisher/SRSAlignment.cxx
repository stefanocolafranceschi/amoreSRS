#include "SRSAlignment.h"
ClassImp(SRSAlignment);

SRSAlignment::SRSAlignment() {
    smallNumber = 0.00000001 ;
}

//=========================================================================================================================//
SRSAlignment::~SRSAlignment(){
    ClearTrackCoordinates() ;
}

//=========================================================================================================================//
map<Float_t, Float_t> SRSAlignment::GetTrackFitData(map <Float_t, Float_t> rawData) {
    map<Float_t, Float_t> toto ;
    /**
     SRSTrackFit * trackFit = new SRSTrackFit(rawData) ;
     map<Float_t, Float_t> fitData = trackFit->getFitData(rawData) ;
     fTrackOffset    = trackFit->getTrackOffset();
     fTrackDirection = trackFit->getTrackDirection();
     delete trackFit ;
     return fitData ;
     */
    return toto ;
}

//=========================================================================================================================//
Float_t SRSAlignment::GetAngleFromTracks(vector<Float_t> inX, vector<Float_t> inY, vector<Float_t> inZ, vector<Float_t> outX, vector<Float_t> outY, vector<Float_t> outZ) {
    
    Int_t inXsize = inX.size() ;
    Int_t inYsize = inY.size() ;
    Int_t inZsize = inZ.size() ;
    Int_t outXsize = outX.size() ;
    Int_t outYsize = outY.size() ;
    Int_t outZsize = outZ.size() ;
    
    if((inYsize != inXsize) || (inZsize != inXsize) || (outYsize != outXsize) || (outZsize != outXsize)) printf("Warning: SRSAlignment::GetAngleFromTracks===> Input vectors are not all the same dimension");
    
    // calculate the scattering angle
    vector<Float_t> u, v ;
    Int_t lastPoint = inYsize - 1 ;
    u.push_back(inX[lastPoint] - inX[0]);
    u.push_back(inY[lastPoint] - inY[0]);
    u.push_back(inZ[lastPoint] - inZ[0]);
    
    lastPoint = outYsize - 1 ;
    v.push_back(outX[lastPoint] - outX[0]);
    v.push_back(outY[lastPoint] - outY[0]);
    v.push_back(outZ[lastPoint] - outZ[0]);
    
    Float_t pocaAngle = getAngleTo(u,v) ;
    return pocaAngle;
}

//=========================================================================================================================//
void SRSAlignment::ClearTrackCoordinates() {
    fTrackX.clear() ;
    fTrackY.clear() ;
    fTrackZ.clear() ;
}

//=========================================================================================================================//
void SRSAlignment::TrackCoordinates() {
    ClearTrackCoordinates() ;
    map<TString, vector<Float_t> > track = fSRSTrack->GetTrackSpacePoints() ;
    map<TString, vector<Float_t> >::const_iterator detector_itr ;
    for (detector_itr = track.begin(); detector_itr != track.end(); ++detector_itr) {
        TString detectorName = (*detector_itr).first;
        if( fSRSTrack->GetDetectorType(detectorName) == "tracker") {
            vector<Float_t> detectorHit = (*detector_itr).second ;
            fTrackX.push_back(detectorHit[0]) ;
            fTrackY.push_back(detectorHit[1]) ;
            fTrackZ.push_back(detectorHit[2]) ;
        }
    }
}

//=========================================================================================================================//
Float_t SRSAlignment::normVec(const vector<Float_t> u) {
    int size =  u.size();
    if(size != 3) printf("SRSAlignment::normVec(): WARNING ==> point vector's size = %d,!= 3  \n",size) ;
    return sqrt(dotVec(u,u)) ;
}

//=========================================================================================================================//
Float_t SRSAlignment::dotVec(const vector<Float_t> u, const vector<Float_t> v) {
    int sizeV =  v.size();
    int sizeU =  u.size();
    if(sizeU != 3)  printf("SRSAlignment::dotVec(): WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    if(sizeV != 3)  printf("SRSAlignment::dotVec(): WARNING ==> V point vector's size = %d,!= 3  \n",sizeV) ;
    return v[0]*u[0] + v[1]*u[1] + v[2]*u[2] ;
}

//=========================================================================================================================//
const vector<Float_t> SRSAlignment::subVec(const vector<Float_t> u, const vector<Float_t> v) {
    
    int sizeV =  v.size();
    int sizeU =  u.size();
    if(sizeU != 3)  printf("SRSAlignment::subVec(): WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    if(sizeV != 3)  printf("SRSAlignment::subVec(): WARNING ==> V point vector's size = %d,!= 3  \n",sizeV) ;
    
    vector<Float_t> sub ;
    sub.push_back(v[0] - u[0]) ;
    sub.push_back(v[1] - u[1]) ;
    sub.push_back(v[2] - u[2]) ;
    
    return sub ;
}

//=========================================================================================================================//
const vector<Float_t> SRSAlignment::addVec(const vector<Float_t> u, const vector<Float_t> v) {
    
    int sizeV =  v.size();
    int sizeU =  u.size();
    if(sizeU != 3)  printf("SRSAlignment::addVec(): WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    if(sizeV != 3)  printf("SRSAlignment::addVec(): WARNING ==> V point vector's size = %d,!= 3  \n",sizeV) ;
    
    vector<Float_t> add ;
    add.push_back(u[0] + v[0]) ;
    add.push_back(u[1] + v[1]) ;
    add.push_back(u[2] + v[2]) ;
    
    return add ;
}

//=========================================================================================================================//
const vector<Float_t> SRSAlignment::prodVec(Float_t a, const vector<Float_t> u) {
    
    int sizeU =  u.size();
    if(sizeU != 3)  printf("SRSAlignment::prodVec(): WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    
    vector<Float_t> prod ;
    
    prod.push_back(a*u[0]) ;
    prod.push_back(a*u[1]) ;
    prod.push_back(a*u[2]) ;
    return prod ;
    
}
//=========================================================================================================================//
const vector<Float_t> SRSAlignment::directionVectorFrom2Points(const vector<Float_t> u, const vector<Float_t> v) {
    
    int sizeV =  v.size();
    int sizeU =  u.size();
    if(sizeU != 3)  printf("SRSAlignment::directionVectorFrom2Points(): WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    if(sizeV != 3)  printf("SRSAlignment::directionVectorFrom2Points(): WARNING ==> V point vector's size = %d,!= 3  \n",sizeV) ;
    
    vector<Float_t> sub ;
    
    sub.push_back(v[0] - u[0]) ;
    sub.push_back(v[1] - u[1]) ;
    sub.push_back(v[2] - u[2]) ;
    Float_t norm = normVec(sub) ;
    
    vector<Float_t> dir ;
    
    dir.push_back(sub[0]/norm) ;
    dir.push_back(sub[1]/norm) ;
    dir.push_back(sub[2]/norm) ;
    
    return dir ;
}

//=========================================================================================================================//
Float_t SRSAlignment::getAngleTo(const vector<Float_t> u, const vector<Float_t> v) {
    
    int sizeV =  v.size();
    int sizeU =  u.size();
    if(sizeU != 3)  printf("SRSAlignment::getAngleTo(): WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    if(sizeV != 3)  printf("SRSAlignment::getAngleTo(): WARNING ==> V point vector's size = %d,!= 3  \n",sizeV) ;
    
    Float_t argument = dotVec(u,v)/(normVec(u)*normVec(v)) ;
    Float_t angle = acos(argument) ;
    if(angle != angle) angle = 0 ;
    return angle ;
}

//=========================================================================================================================//
Float_t SRSAlignment::projectedAngleXY(const vector<Float_t> u, TString xORy) {
    vector<Float_t> v = getDirection(u) ;
    Float_t vz=v[2] ;
    Float_t vproj=v[0] ;
    if(xORy == "X") vproj=v[0] ;
    if(xORy == "Y") vproj=v[1] ;
    Float_t vplane = sqrt(vz*vz+vproj*vproj) ;
    Float_t projAngle = 0. ;
    if(vplane > 0.) {
        vz /= vplane ;
        projAngle = acos(-vz) ;
        if(vproj < 0.) projAngle *= -1. ;
    }
    return projAngle ;
}

//=========================================================================================================================//
Float_t SRSAlignment::projectedAngleXZ(const vector<Float_t> u, TString xORz) {
    vector<Float_t> v = getDirection(u) ;
    Float_t vy=v[1] ;
    Float_t vproj=v[0] ;
    
    if(xORz == "X") vproj=v[0] ;
    if(xORz == "Z") vproj=v[2] ;
    
    Float_t vplane = sqrt(vy*vy+vproj*vproj) ;
    Float_t projAngle = 0. ;
    if(vplane > 0.) {
        vy /= vplane ;
        projAngle = acos(-vy) ;
        if(vproj < 0.) projAngle *= -1. ;
    }
    return projAngle ;
}

//=========================================================================================================================//
const vector<Float_t> SRSAlignment::getXandYKnowingZ(const vector<Float_t> w, const vector<Float_t> v, Float_t z0) {
    
    vector<Float_t> d = directionVectorFrom2Points(v,w) ;
    vector<Float_t> p0  ;
    
    Float_t t0 = (w[2] - z0)/d[2] ;
    Float_t x0 = w[0] - d[0]*t0 ;
    Float_t y0 = w[1] - d[1]*t0 ;
    
    p0.push_back(x0) ;
    p0.push_back(y0) ;
    p0.push_back(z0) ;
    
    return p0 ;
}

//=========================================================================================================================//
const vector<Float_t> SRSAlignment::getDirection(const vector<Float_t> u) {
    
    int size =  u.size();
    if(size != 3)  printf("SRSAlignment::getDirection(): WARNING ==> U point vector's size = %d,!= 3  \n",size) ;
    
    vector<Float_t> direction ;
    
    Float_t x = u[0]/normVec(u) ;
    Float_t y = u[1]/normVec(u) ;
    Float_t z = u[2]/normVec(u) ;
    
    direction.push_back(x) ;
    direction.push_back(y) ;
    direction.push_back(z) ;
    
    return direction ;
}

//=========================================================================================================================//
Float_t SRSAlignment::abs(Float_t x) {
    return ((x) >= 0 ? (x):(-x)) ; 
}

