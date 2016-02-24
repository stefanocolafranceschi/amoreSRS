// Author: Kondo GNANVO 01/05/2011
#include "SRSTrackFit.h"

//================================================================================================//
SRSTrackFit::SRSTrackFit(map<TString, vector<Float_t> > trackerData, map<TString, vector<Float_t> > rawData, map<TString, Float_t > xinputRes, map<TString, Float_t > yinputRes) {
    smallNumber = 0.00000001 ;
    ClearTracks() ;
    fTrackerData = trackerData ;
    fRawData = rawData ;
    fDetXPlaneInputResolution =  xinputRes;
    fDetYPlaneInputResolution =  yinputRes;
    
    //  LeastSquareLinearFitTrack() ;
    WeightedLeastSquareLinearFitTrack() ;
}

//================================================================================================//
SRSTrackFit::~SRSTrackFit() {
    ClearTracks();
}

/**
 //================================================================================================//
 void SRSTrackFit::LeastSquareLinearFitTrack() {
 
 Float_t sumx=0, sumy =0,sumz = 0 ;
 Float_t squareSumz = 0 ;
 Float_t crossSumxz = 0, crossSumyz =0 ;
 Int_t n = fTrackerData.size() ;
 
 map <TString, vector<Float_t> >::const_iterator tracker_itr ;
 for(tracker_itr = fTrackerData.begin(); tracker_itr != fTrackerData.end(); ++tracker_itr) {
 TString trackerName = (*tracker_itr).first ;
 vector <Float_t> point = (*tracker_itr).second ;
 Float_t x_coordinate = point[0] ;
 Float_t y_coordinate = point[1] ;
 Float_t z_coordinate = point[2] ;
 
 sumx += x_coordinate ;
 sumy += y_coordinate ;
 sumz += z_coordinate;
 squareSumz += z_coordinate * z_coordinate ;
 crossSumxz += x_coordinate * z_coordinate ;
 crossSumyz += y_coordinate * z_coordinate ;
 }
 
 fFitParameters["xOffset"]    = (sumz*crossSumxz - sumx*squareSumz) / (sumz*sumz - n*squareSumz) ;
 fFitParameters["yOffset"]    = (sumz*crossSumyz - sumy*squareSumz) / (sumz*sumz - n*squareSumz)  ;
 fFitParameters["xDirection"] = (sumz*sumx - n*crossSumxz) / (sumz*sumz - n*squareSumz) ;
 fFitParameters["yDirection"] = (sumz*sumy - n*crossSumyz) / (sumz*sumz - n*squareSumz);
 
 map <TString, vector<Float_t> >::const_iterator track_itr ;
 for(track_itr = fTrackerData.begin(); track_itr != fTrackerData.end(); ++track_itr) {
 TString trackerName = (*track_itr).first ;
 vector <Float_t> point = (*track_itr).second ;
 
 Int_t size =  fTrackFittedData[trackerName].size() ;
 if (size != 0)  fTrackFittedData[trackerName].clear() ;
 
 fTrackFittedData[trackerName].push_back(fFitParameters["xOffset"] +  fFitParameters["xDirection"] * point[2]) ;
 fTrackFittedData[trackerName].push_back(fFitParameters["yOffset"] +  fFitParameters["yDirection"] * point[2]) ;
 fTrackFittedData[trackerName].push_back(point[2]) ;
 }
 
 map <TString, vector<Float_t> >::const_iterator res_itr ;
 for(res_itr = fRawData.begin(); res_itr != fRawData.end(); ++res_itr) {
 TString  trackerName = (*res_itr).first ;
 vector <Float_t> point = (*res_itr).second ;
 
 Int_t size =  fTrackFittedData[trackerName].size() ;
 if (size != 0)  fTrackFittedData[trackerName].clear() ;
 
 fTrackFittedData[trackerName].push_back(fFitParameters["xOffset"] +  fFitParameters["xDirection"] * point[2]) ;
 fTrackFittedData[trackerName].push_back(fFitParameters["yOffset"] +  fFitParameters["yDirection"] * point[2]) ;
 fTrackFittedData[trackerName].push_back(point[2]) ;
 size = fTrackFittedData[trackerName].size() ;
 }
 }
 */

//================================================================================================//
void SRSTrackFit::WeightedLeastSquareLinearFitTrack() {
    
    Float_t sumx = 0, sumxz = 0, squareSumxz = 0, crossSumxz = 0, sumxRes = 0;;
    Float_t sumy = 0, sumyz = 0, squareSumyz = 0, crossSumyz = 0, sumyRes = 0;;
    Float_t xresol = 1, yresol = 1;
    
    Int_t n = fTrackerData.size() ;
    
    map <TString, vector<Float_t> >::const_iterator tracker_itr ;
    for(tracker_itr = fTrackerData.begin(); tracker_itr != fTrackerData.end(); ++tracker_itr) {
        
        TString trackerName = (*tracker_itr).first ;
        vector <Float_t> point = (*tracker_itr).second ;
        
        TString xplane = trackerName + "X" ;
        TString yplane = trackerName + "Y" ;
        
        Float_t x_coordinate = point[0] ;
        Float_t y_coordinate = point[1] ;
        Float_t z_coordinate = point[2] ;
        
        if (fDetXPlaneInputResolution[trackerName] > 0) xresol = fDetXPlaneInputResolution[trackerName] ;
        if (fDetYPlaneInputResolution[trackerName] > 0) yresol = fDetYPlaneInputResolution[trackerName] ;
        //    printf("SRSTrackFit::WeightedLeastSquareLinearFitTrack=> xresol = %f,  yresol = %f  \n",xresol, yresol) ;
        
        sumx += x_coordinate / (xresol * xresol );
        sumxz += z_coordinate / (xresol * xresol );
        squareSumxz += (z_coordinate * z_coordinate) / (xresol * xresol );
        crossSumxz += (x_coordinate * z_coordinate) / (xresol * xresol );
        sumxRes += 1 /  (xresol * xresol );
        
        sumy += y_coordinate / (yresol * yresol );
        sumyz += z_coordinate / (yresol * yresol );
        squareSumyz += (z_coordinate * z_coordinate) / (yresol * yresol );
        crossSumyz += (y_coordinate * z_coordinate) / (yresol * yresol );
        sumyRes += 1 /  (yresol * yresol );
    }
    
    fFitParameters["xOffset"]    = (sumxz*crossSumxz - sumx*squareSumxz) / (sumxz*sumxz - sumxRes*squareSumxz) ;
    fFitParameters["yOffset"]    = (sumyz*crossSumyz - sumy*squareSumyz) / (sumyz*sumyz - sumyRes*squareSumyz)  ;
    fFitParameters["xDirection"] = (sumxz*sumx - sumxRes*crossSumxz) / (sumxz*sumxz - sumxRes*squareSumxz);
    fFitParameters["yDirection"] = (sumyz*sumy - sumyRes*crossSumyz) / (sumyz*sumyz - sumyRes*squareSumyz);
    
    map <TString, vector<Float_t> >::const_iterator track_itr ;
    for(track_itr = fTrackerData.begin(); track_itr != fTrackerData.end(); ++track_itr) {
        TString trackerName = (*track_itr).first ;
        vector <Float_t> point = (*track_itr).second ;
        
        Int_t size =  fTrackFittedData[trackerName].size() ;
        if (size != 0)  fTrackFittedData[trackerName].clear() ;
        
        fTrackFittedData[trackerName].push_back(fFitParameters["xOffset"] +  fFitParameters["xDirection"] * point[2]) ;
        fTrackFittedData[trackerName].push_back(fFitParameters["yOffset"] +  fFitParameters["yDirection"] * point[2]) ;
        fTrackFittedData[trackerName].push_back(point[2]) ;
    }
    
    map <TString, vector<Float_t> >::const_iterator res_itr ;
    for(res_itr = fRawData.begin(); res_itr != fRawData.end(); ++res_itr) {
        TString  trackerName = (*res_itr).first ;
        vector <Float_t> point = (*res_itr).second ;
        
        Int_t size =  fTrackFittedData[trackerName].size() ;
        if (size != 0)  fTrackFittedData[trackerName].clear() ;
        
        fTrackFittedData[trackerName].push_back(fFitParameters["xOffset"] +  fFitParameters["xDirection"] * point[2]) ;
        fTrackFittedData[trackerName].push_back(fFitParameters["yOffset"] +  fFitParameters["yDirection"] * point[2]) ;
        fTrackFittedData[trackerName].push_back(point[2]) ;
        size = fTrackFittedData[trackerName].size() ;
    }
}

//================================================================================================//
void SRSTrackFit::ClearTracks() {
    fFitParameters.clear() ;
    fDetXPlaneInputResolution.clear() ;
    fDetYPlaneInputResolution.clear() ;
    
    map<TString, vector<Float_t> >::const_iterator rawTracker_itr ;
    for (rawTracker_itr = fTrackerData.begin(); rawTracker_itr != fTrackerData.end(); rawTracker_itr++) {
        vector<Float_t> point = (*rawTracker_itr).second ;
        point.clear() ;
    }
    fTrackerData.clear() ;
    
    map<TString, vector<Float_t> >::const_iterator fittedTracker_itr ;
    for (fittedTracker_itr = fTrackFittedData.begin(); fittedTracker_itr != fTrackFittedData.end(); fittedTracker_itr++) {
        vector<Float_t> point = (*fittedTracker_itr).second ;
        point.clear() ;
    }
    fTrackFittedData.clear() ;
    
    map<TString, vector<Float_t> >::const_iterator residual_itr ;
    for (residual_itr = fRawData.begin(); residual_itr != fRawData.end(); residual_itr++) {
        vector<Float_t> point = (*residual_itr).second ;
        point.clear() ;
    }
    fRawData.clear() ;
}

//=========================================================================================================================//
Float_t SRSTrackFit::GetAngleBetweenTwoTracks(map<TString, vector<Float_t> > firstTrack, map<TString, vector<Float_t> > secondTrack) {
    
    vector <Float_t> u, v ;
    vector<TString> firstTrackTrackerName, secondTrackTrackerName ;
    
    map<TString, vector<Float_t> >::const_iterator tracker_itr ;
    for (tracker_itr = firstTrack.begin(); tracker_itr != firstTrack.end(); tracker_itr++) {
        firstTrackTrackerName.push_back( (*tracker_itr).first ) ;
    }
    
    map<TString, vector<Float_t> >::const_iterator tracker2_itr ;
    for (tracker2_itr = secondTrack.begin(); tracker2_itr != secondTrack.end(); tracker2_itr++) {
        secondTrackTrackerName.push_back( (*tracker2_itr).first ) ;
    }
    
    int sizeFirst  = firstTrackTrackerName.size() ;
    int sizeSecond = secondTrackTrackerName.size() ;
    
    Int_t size_first  = sizeFirst - 1 ;
    Int_t size_second = sizeSecond - 1 ;
    
    vector <Float_t> firstTrackerFirstTrack  = firstTrack[firstTrackTrackerName[0]] ;
    vector <Float_t> lastTrackerFirstTrack   = firstTrack[firstTrackTrackerName[size_first]] ;
    vector <Float_t> firstTrackerSecondTrack = secondTrack[secondTrackTrackerName[0]] ;
    vector <Float_t> lastTrackerSecondTrack  = secondTrack[secondTrackTrackerName[size_second]] ;
    
    u.push_back(lastTrackerFirstTrack[0] - firstTrackerFirstTrack[0]);
    u.push_back(lastTrackerFirstTrack[1] - firstTrackerFirstTrack[1]);
    u.push_back(lastTrackerFirstTrack[2] - firstTrackerFirstTrack[2]);
    
    v.push_back(lastTrackerSecondTrack[0] - firstTrackerSecondTrack[0]);
    v.push_back(lastTrackerSecondTrack[1] - firstTrackerSecondTrack[1]);
    v.push_back(lastTrackerSecondTrack[2] - firstTrackerSecondTrack[2]);
    
    Float_t angleBetweenTwoTracks = getAngleTo(u,v) ;
    
    firstTrackerFirstTrack.clear() ;
    lastTrackerFirstTrack.clear() ;
    firstTrackerSecondTrack.clear() ;
    lastTrackerSecondTrack.clear() ;
    firstTrackTrackerName.clear() ;
    secondTrackTrackerName.clear() ;
    u.clear() ;
    v.clear() ;
    
    return angleBetweenTwoTracks;
}


//=========================================================================================================================//
Float_t SRSTrackFit::normVec(const vector<Float_t> u) {
    int size =  u.size();
    if(size != 3) printf("SRSTrackFit::normVec(): WARNING ==> point vector's size = %d,!= 3  \n",size) ;
    return sqrt(dotVec(u,u)) ;
}

//=========================================================================================================================//
Float_t SRSTrackFit::dotVec(const vector<Float_t> u, const vector<Float_t> v) {
    int sizeV =  v.size();
    int sizeU =  u.size();
    if(sizeU != 3)  printf("SRSTrackFit::dotVec(): WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    if(sizeV != 3)  printf("SRSTrackFit::dotVec(): WARNING ==> V point vector's size = %d,!= 3  \n",sizeV) ;
    return v[0]*u[0] + v[1]*u[1] + v[2]*u[2] ;
}

//=========================================================================================================================//
const vector<Float_t> SRSTrackFit::subVec(const vector<Float_t> u, const vector<Float_t> v) {
    
    int sizeV =  v.size();
    int sizeU =  u.size();
    if(sizeU != 3)  printf("SRSTrackFit::subVec(): WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    if(sizeV != 3)  printf("SRSTrackFit::subVec(): WARNING ==> V point vector's size = %d,!= 3  \n",sizeV) ;
    
    vector<Float_t> sub ;
    sub.push_back(v[0] - u[0]) ;
    sub.push_back(v[1] - u[1]) ;
    sub.push_back(v[2] - u[2]) ;
    
    return sub ;
}

//=========================================================================================================================//
const vector<Float_t> SRSTrackFit::addVec(const vector<Float_t> u, const vector<Float_t> v) {
    
    int sizeV =  v.size();
    int sizeU =  u.size();
    if(sizeU != 3)  printf("SRSTrackFit::addVec(): WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    if(sizeV != 3)  printf("SRSTrackFit::addVec(): WARNING ==> V point vector's size = %d,!= 3  \n",sizeV) ;
    
    vector<Float_t> add ;
    add.push_back(u[0] + v[0]) ;
    add.push_back(u[1] + v[1]) ;
    add.push_back(u[2] + v[2]) ;
    
    return add ;
}

//=========================================================================================================================//
const vector<Float_t> SRSTrackFit::prodVec(Float_t a, const vector<Float_t> u) {
    
    int sizeU =  u.size();
    if(sizeU != 3)  printf("SRSTrackFit::prodVec(): WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    
    vector<Float_t> prod ;
    
    prod.push_back(a*u[0]) ;
    prod.push_back(a*u[1]) ;
    prod.push_back(a*u[2]) ;
    return prod ;
    
}

//=========================================================================================================================//
void SRSTrackFit::PlaneRotationAlongZaxis(Float_t alpha, vector<Float_t> & u) {
    int sizeU =  u.size();
    if(sizeU != 3)  printf("SRSTrackFit::PlaneRotationAlongZaxis(): WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    u[0] = u[0] * TMath::Cos(alpha) + u[1] * TMath::Sin(alpha) ;
    u[1] = u[1] * TMath::Cos(alpha) - u[0] * TMath::Sin(alpha) ;
    u[2] = u[2] ;
}


//=========================================================================================================================//
const vector<Float_t> SRSTrackFit::getDirection(const vector<Float_t> u) {
    
    int size =  u.size();
    if(size != 3)  printf("SRSTrackFit::getDirection(): WARNING ==> U point vector's size = %d,!= 3  \n",size) ;
    
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
const vector<Float_t> SRSTrackFit::directionVectorFrom2Points(const vector<Float_t> u, const vector<Float_t> v) {
    
    int sizeV =  v.size();
    int sizeU =  u.size();
    if(sizeU != 3)  printf("SRSTrackFit::directionVectorFrom2Points(): WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    if(sizeV != 3)  printf("SRSTrackFit::directionVectorFrom2Points(): WARNING ==> V point vector's size = %d,!= 3  \n",sizeV) ;
    
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
Float_t SRSTrackFit::getAngleTo(const vector<Float_t> u, const vector<Float_t> v) {
    
    int sizeV =  v.size();
    int sizeU =  u.size();
    if(sizeU != 3)  printf("SRSTrackFit::getAngleTo(): WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    if(sizeV != 3)  printf("SRSTrackFit::getAngleTo(): WARNING ==> V point vector's size = %d,!= 3  \n",sizeV) ;
    
    Float_t argument = dotVec(u,v)/(normVec(u)*normVec(v)) ;
    Float_t angle = acos(argument) ;
    if(angle != angle) angle = 0 ;
    return angle ;
}

//=========================================================================================================================//
Float_t SRSTrackFit::projectedAngleXY(const vector<Float_t> u, TString xORy) {
    vector<Float_t> v = getDirection(u) ;
    Float_t vz=v[2] ;
    Float_t vproj=v[0] ;
    
    if((xORy == "X") || (xORy == "x")) vproj=v[0] ;
    if((xORy == "Y") || (xORy == "y")) vproj=v[1] ;
    
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
Float_t SRSTrackFit::projectedAngleXZ(const vector<Float_t> u, TString xORz) {
    vector<Float_t> v = getDirection(u) ;
    Float_t vy=v[1] ;
    Float_t vproj=v[0] ;
    
    if((xORz == "X") || (xORz == "x")) vproj=v[0] ;
    if((xORz == "Z") || (xORz == "z")) vproj=v[2] ;
    
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
const vector<Float_t> SRSTrackFit::getXandYKnowingZ(const vector<Float_t> w, const vector<Float_t> v, Float_t z0) {
    
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
Float_t SRSTrackFit::abs(Float_t x) {
    return ((x) >= 0 ? (x):(-x)) ; 
}
