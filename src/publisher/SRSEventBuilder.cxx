#include "SRSEventBuilder.h"

ClassImp(SRSEventBuilder);

//============================================================================================
SRSEventBuilder::SRSEventBuilder(Int_t triggerNb, TString maxClusterSize, TString minClusterSize, TString zeroSupCut, TString runType, Bool_t isClusterPosCorrection ) {
    
    fTriggerCount = triggerNb ;
    
    //  printf("==SRSEventBuilder::SRSEventBuilder() triggerNb = %d \n", triggerNb) ;
    fZeroSupCut = zeroSupCut.Atoi() ;
    fMaxClusterSize = 100000 ;
    fMinClusterSize = 1 ;
    fIsClusterPosCorrection = isClusterPosCorrection ;
    
    if(fZeroSupCut != 0) {
        fMinClusterSize = minClusterSize.Atoi() ;
        fMaxClusterSize = maxClusterSize.Atoi() ;
    }
    
    fIsHitMaxOrTotalADCs = "signalPeak";
    fClusterPositionCorrectionRootFile = "" ;
    
    fIsGoodEvent = kFALSE ;
    fIsGoodClusterEvent = kFALSE ;
    
    fRunType = runType ;
    fListOfHits = new TList ;
    fListOfAPVEvents = new TList ;
    
    /**
     fTrapezoidDetLength    = 1000 ;
     fTrapezoidDetInnerRadius = 220 ;
     fTrapezoidDetOuterRadius = 430 ;
     */
    
    //  fEICstereoAngleDegree = (6.067 * PI ) / 180 ;
    //  fUVangleCosineDirection   = TMath::Tan(fTrapezoidDetUVangleDegree) ;
    //  fUVangleCosineDirection = (430 - 220) / (2 * fTrapezoidDetLength) ;
    //  fUVangleCosineDirection = ( fTrapezoidDetOuterRadius - fTrapezoidDetInnerRadius ) / (2 * fTrapezoidDetLength) ;
    //  fUVangle = TMath::ATan(fUVangleCosineDirection) ;
}

//============================================================================================
SRSEventBuilder::~SRSEventBuilder() {
    fTriggerList.clear() ;
    DeleteListOfHits(fListOfHits) ;
    DeleteListOfAPVEvents(fListOfAPVEvents) ;
    DeleteHitsInDetectorPlaneMap(fHitsInDetectorPlaneMap) ;
    DeleteClustersInDetectorPlaneMap(fClustersInDetectorPlaneMap) ;
}

//============================================================================================
template <typename M> void ClearVectorMap( M & amap ) {
    for ( typename M::iterator it = amap.begin(); it != amap.end(); ++it ) {
        ((*it).second).clear();
    }
    amap.clear() ;
}

//============================================================================================
void SRSEventBuilder::DeleteHitsInDetectorPlaneMap( map<TString, list <SRSHit * > > & stringListMap) {
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

//============================================================================================
void SRSEventBuilder::DeleteClustersInDetectorPlaneMap( map<TString, list <SRSCluster * > > & stringListMap) {
    map < TString, list <SRSCluster *> >::const_iterator itr ;
    for (itr = stringListMap.begin(); itr != stringListMap.end(); itr++) {
        list <SRSCluster *> listOfClusters = (*itr).second ;
        list <SRSCluster *>::const_iterator cluster_itr ;
        for (cluster_itr = listOfClusters.begin(); cluster_itr != listOfClusters.end(); cluster_itr++) {
            delete (* cluster_itr) ;
        }
        listOfClusters.clear() ;
    }
    stringListMap.clear() ;
}

//============================================================================================
void SRSEventBuilder::DeleteListOfAPVEvents(TList * listOfAPVEvents) {
    TIter nextAPVEvent(listOfAPVEvents) ;
    while( SRSAPVEvent * apvEvent = ( (SRSAPVEvent *) nextAPVEvent() ) ) delete apvEvent ;
    listOfAPVEvents->Clear();
    delete listOfAPVEvents;
}


//============================================================================================
void SRSEventBuilder::DeleteListOfClusters(TList * listOfClusters) {
    listOfClusters->Clear();
    delete listOfClusters;
}

//============================================================================================
void SRSEventBuilder::DeleteListOfHits(TList * listOfHits) {
    TIter nextHit(listOfHits) ;
    while( SRSHit * hit = ( (SRSHit *) nextHit() ) ) delete hit  ;
    listOfHits->Clear();
    delete listOfHits;
}

//============================================================================================
static Bool_t CompareStripNo( TObject *obj1, TObject *obj2) {
    Bool_t compare ;
    if ( ( (SRSHit*) obj1 )->GetStripNo() < ( ( SRSHit*) obj2 )->GetStripNo() ) compare = kTRUE ;
    else compare = kFALSE ;
    return compare ;
}

//============================================================================================
static Bool_t CompareHitADCs( TObject *obj1, TObject *obj2) {
    Bool_t compare ;
    if ( ( (SRSHit*) obj1 )->GetHitADCs() > ( ( SRSHit*) obj2 )->GetHitADCs()) compare = kTRUE ;
    else compare = kFALSE ;
    return compare ;
}

//============================================================================================
static Bool_t CompareClusterADCs( TObject *obj1, TObject *obj2) {
    Bool_t compare ;
    if ( ( (SRSCluster*) obj1 )->GetClusterADCs() > ( ( SRSCluster*) obj2 )->GetClusterADCs()) compare = kTRUE ;
    else compare = kFALSE ;
    return compare ;
}

//============================================================================================
void SRSEventBuilder::ComputeClustersInDetectorPlane() {
    //  printf("==SRSEventBuilder::ComputeClustersInDetectorPlane() \n") ;
    
    map<TString, list <SRSHit*> >::const_iterator  listOfHits_itr ;
    for (listOfHits_itr = fHitsInDetectorPlaneMap.begin(); listOfHits_itr != fHitsInDetectorPlaneMap.end(); ++listOfHits_itr) {
        TString detPlane =  (*listOfHits_itr).first ;
        SRSMapping * mapping = SRSMapping::GetInstance() ;
        TString detector = mapping->GetDetectorFromPlane(detPlane) ;
        TString readoutBoard = mapping->GetReadoutBoardFromDetector(detector) ;
        
        list <SRSHit*> listOfHits = (*listOfHits_itr).second ;
        listOfHits.sort(CompareStripNo) ;
        Int_t listSize = listOfHits.size() ;
        
        if (listSize < fMinClusterSize) {
            fIsGoodEvent = kFALSE ;
            continue ;
        }
        
        Int_t previousStrip = -2 ;
        Int_t clusterNo = -1 ;
        
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
                clustersMap[clusterNo] = new SRSCluster(fMinClusterSize, fMaxClusterSize, fIsClusterMaxOrTotalADCs) ;
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
            
            if (!cluster->IsGoodCluster()) {
                delete cluster ;
                continue ;
            }
            if (fIsClusterPosCorrection) {
                cluster->SetClusterPositionCorrection(fIsClusterPosCorrection) ;
                cluster->ComputeClusterPositionWithCorrection(fClusterPositionCorrectionRootFile) ;
            }
            else {
                cluster->SetClusterPositionCorrection(kFALSE) ;
                cluster->ComputeClusterPositionWithoutCorrection() ;
            }
            fClustersInDetectorPlaneMap[detPlane].push_back(cluster) ;
        }
        fClustersInDetectorPlaneMap[detPlane].sort(CompareClusterADCs) ;
        
        listOfHits.clear() ;
        clustersMap.clear() ;
    }
}

//============================================================================================
Bool_t  SRSEventBuilder::IsAGoodEventInDetector(TString detector) {
    Bool_t IsGoodEventInDetector = kFALSE ;
    SRSMapping * mapping = SRSMapping::GetInstance() ;
    TString readoutBoard = mapping->GetReadoutBoardFromDetector(detector) ;
    
    if( readoutBoard == "PADPLANE") {
        TString padPlane = (mapping->GetDetectorPlaneListFromDetector(detector)).front();
        list <SRSHit*> listOfHits = fHitsInDetectorPlaneMap[padPlane];
        Int_t size = listOfHits.size() ;
        if (size > 0) IsGoodEventInDetector = kTRUE ;
    }
    
    else if(readoutBoard == "CMSGEM") {
        TString plane = (mapping->GetDetectorPlaneListFromDetector(detector)).front();
        Int_t clusterMultiplicity = fClustersInDetectorPlaneMap[plane.Data()].size() ;
        if ( (clusterMultiplicity == 0) ||  (clusterMultiplicity > fMaxClusterMultiplicity) ) {
            fClustersInDetectorPlaneMap[plane.Data()].clear() ;
            IsGoodEventInDetector = kFALSE ;
        }
        else {
            IsGoodEventInDetector = kTRUE ;
        }
    }
    
    else if( readoutBoard == "1DSTRIPS") {
        TString plane = (mapping->GetDetectorPlaneListFromDetector(detector)).front();
        Int_t clusterMultiplicity = fClustersInDetectorPlaneMap[plane.Data()].size() ;
        if ( (clusterMultiplicity == 0) || (clusterMultiplicity > fMaxClusterMultiplicity) ) {
            fClustersInDetectorPlaneMap[plane.Data()].clear() ;
            IsGoodEventInDetector = kFALSE ;
        }
        else {
            IsGoodEventInDetector = kTRUE ;
        }
    }
    
    else {
        TString detPlaneX = (mapping->GetDetectorPlaneListFromDetector(detector)).front() ;
        TString detPlaneY = (mapping->GetDetectorPlaneListFromDetector(detector)).back() ;
        Int_t clusterMultiplicityX = fClustersInDetectorPlaneMap[detPlaneX.Data()].size() ;
        Int_t clusterMultiplicityY = fClustersInDetectorPlaneMap[detPlaneY.Data()].size() ;
        
        if ( (fTriggerList[detector] == "isTrigger") and ( (clusterMultiplicityX > 1) or (clusterMultiplicityY > 1)) ) {
            IsGoodEventInDetector = kFALSE ;
            fClustersInDetectorPlaneMap[detPlaneX.Data()].clear() ;
            fClustersInDetectorPlaneMap[detPlaneY.Data()].clear() ;
        }
        
        if ((clusterMultiplicityX == 0) or (clusterMultiplicityY == 0) or (clusterMultiplicityX > fMaxClusterMultiplicity) or (clusterMultiplicityY > fMaxClusterMultiplicity) ) {
            IsGoodEventInDetector = kFALSE ;
            fClustersInDetectorPlaneMap[detPlaneX.Data()].clear() ;
            fClustersInDetectorPlaneMap[detPlaneY.Data()].clear() ;
        }
        else {
            IsGoodEventInDetector = kTRUE ;
        }
    }
    return IsGoodEventInDetector ;
}

//============================================================================================
Bool_t  SRSEventBuilder::IsAGoodEvent() {
    fIsGoodEvent = kFALSE ;
    Int_t nbOfTriggeredDetectors = 0 ;
    
    SRSMapping * mapping = SRSMapping::GetInstance() ;
    Int_t nbOfDetectorsToBeTriggered = fTriggerList.size() ;
    
    if (nbOfDetectorsToBeTriggered == 0 ) {
        fIsGoodEvent = kTRUE ;
    }
    
    else {
        map<TString, TString>::const_iterator trigger_itr ;
        for (trigger_itr = fTriggerList.begin(); trigger_itr != fTriggerList.end(); trigger_itr++) {
            TString detector = (*trigger_itr).first ;
            if (!IsAGoodEventInDetector(detector) ) {
                continue ;
            }
            else {
                nbOfTriggeredDetectors++ ;
            }
        }
        if (nbOfTriggeredDetectors == nbOfDetectorsToBeTriggered) {
            fIsGoodEvent = kTRUE ;
        }
    }
    return fIsGoodEvent ;
}

//============================================================================================
Bool_t  SRSEventBuilder::IsAGoodClusterEvent(TString detPlane) {
    fIsGoodClusterEvent = kFALSE ;
    
    SRSMapping * mapping = SRSMapping::GetInstance() ;
    TString detector = mapping->GetDetectorFromPlane(detPlane) ;
    TString readoutBoard = mapping->GetReadoutBoardFromDetector(detector) ;
    
    if(readoutBoard == "PADPLANE") {
        list <SRSHit*> listOfHits = fHitsInDetectorPlaneMap[detPlane] ;
        Int_t size = listOfHits.size() ;
        if (size > 0) fIsGoodClusterEvent = kTRUE ;
        listOfHits.clear() ;
    }
    
    else {
        Int_t clusterMult = fClustersInDetectorPlaneMap[detPlane.Data()].size()  ;
        if ((clusterMult == 0) || (clusterMult > fMaxClusterMultiplicity) ) {
            fClustersInDetectorPlaneMap[detPlane.Data()].clear() ;
            fIsGoodClusterEvent = kFALSE ;
        }
        else {
            fIsGoodClusterEvent = kTRUE ;
        }
    }
    return fIsGoodClusterEvent ;
}
//============================================================================================

/**
 list <SRSCluster * >  SRSEventBuilder::CrossTalkCorrection( list <SRSCluster * >  listOfClusters) {
 
 Int_t clusterMultiplicity = listOfClusters.size() ;
 
 if(clusterMultiplicity == 2) {
 SRSCluster * cluster1 = listOfClusters.front() ;
 SRSCluster * cluster2 = listOfClusters.back() ;
 Float_t diffStripNb    = (fabs(cluster1->GetClusterCentralStrip() - cluster2->GetClusterCentralStrip())) ;
 Float_t ratioADCs = cluster1->GetClusterADCs() /  cluster2->GetClusterADCs() ;
 
 Float_t criteria32 = fabs(diffStripNb - 32) ;
 Float_t criteria88 = fabs(diffStripNb - 88) ;
 
 if ((( criteria32 <= 1)  || (criteria88 <=1)) && (ratioADCs > 1) ) {
 cluster1->SetClusterADCs(cluster1->GetClusterADCs() +  cluster2->GetClusterADCs() ) ;
 listOfClusters.pop_back() ;
 clusterMultiplicity = listOfClusters.size() ;
 }
 }
 return listOfClusters ;
 }
 */

//============================================================================================
map < Int_t, vector <Float_t > > SRSEventBuilder::GetDetectorCluster(TString detector) {
    
    SRSMapping * mapping = SRSMapping::GetInstance() ;
    
    TString detPlaneX = (mapping->GetDetectorPlaneListFromDetector(detector)).front() ;
    TString detPlaneY = (mapping->GetDetectorPlaneListFromDetector(detector)).back() ;
    TString readoutBoard = mapping->GetReadoutBoardFromDetector(detector) ;
    
    Int_t planeOrientationX = mapping->GetPlaneOrientation(detPlaneX) ;
    Int_t planeOrientationY = mapping->GetPlaneOrientation(detPlaneY) ;
    
    /**
     planeOrientationX = 1 ;
     planeOrientationY = 1;
     */
    
    
    map < Int_t, vector <Float_t > > detectorClusterMap ;
    list <SRSCluster * > listOfClustersX = fClustersInDetectorPlaneMap[detPlaneX.Data()] ;
    list <SRSCluster * > listOfClustersY = fClustersInDetectorPlaneMap[detPlaneY.Data()] ;
    
    Int_t clusterMultiplicityX = listOfClustersX.size() ;
    Int_t clusterMultiplicityY = listOfClustersY.size() ;
    
    Int_t clusterMult = clusterMultiplicityX ;
    if ( clusterMultiplicityY < clusterMult )    clusterMult = clusterMultiplicityY  ;
    if ( fMaxClusterMultiplicity < clusterMult ) clusterMult = fMaxClusterMultiplicity ;
    
    TList  * clusterListX, * clusterListY;
    clusterListX  = new TList ;
    clusterListY  = new TList ;
    
    list <SRSCluster * >::const_iterator clusterX_itr ;
    for(clusterX_itr = listOfClustersX.begin(); clusterX_itr != listOfClustersX.end(); ++clusterX_itr ) {
        clusterListX->Add(* clusterX_itr) ;
    }
    list <SRSCluster * >::const_iterator clusterY_itr ;
    for(clusterY_itr = listOfClustersY.begin(); clusterY_itr != listOfClustersY.end(); ++clusterY_itr ) {
        clusterListY->Add(* clusterY_itr) ;
    }
    
    for (Int_t k = 0 ; k < clusterMult ; k++) {
        Float_t clusterPos1   = ((SRSCluster *) clusterListX->At(k))->GetClusterPosition() ;
        Float_t clusterPos2   = ((SRSCluster *) clusterListY->At(k))->GetClusterPosition() ;
        
        Float_t x_coord   = ((SRSCluster *) clusterListX->At(k))->GetClusterPosition() ;
        Float_t y_coord   = ((SRSCluster *) clusterListY->At(k))->GetClusterPosition() ;
        
        clusterPos1 = planeOrientationX * clusterPos1 ;
        clusterPos2 = planeOrientationY * clusterPos2 ;
        
        Float_t xpos =  clusterPos1;
        Float_t ypos =  clusterPos2;
        
        Float_t adcCount1 = ((SRSCluster *) clusterListX->At(k))->GetClusterADCs() ;
        Float_t adcCount2 = ((SRSCluster *) clusterListY->At(k))->GetClusterADCs() ;
        
        Float_t timing1 = ((SRSCluster *) clusterListX->At(k))->GetClusterTimeBin() ;
        Float_t timing2 = ((SRSCluster *) clusterListY->At(k))->GetClusterTimeBin() ;
        
        if (readoutBoard == "UV_ANGLE") {
            Float_t trapezoidDetLength = mapping->GetUVangleReadoutMap(detector) [0] ;
            Float_t trapezoidDetInnerRadius =  mapping->GetUVangleReadoutMap(detector) [1] ;
            Float_t trapezoidDetOuterRadius =  mapping->GetUVangleReadoutMap(detector) [2] ;
            Float_t uvAngleCosineDirection = ( trapezoidDetOuterRadius - trapezoidDetInnerRadius ) / (2 * trapezoidDetLength) ;
            
            //      printf("==SRSEventBuilder::GetDetectorCluster(): trapezoidDetLength=%f, trapezoidDetInnerRadius=%f trapezoidDetOuterRadius=%f, uvAngleCosineDirection=%f\n", trapezoidDetLength, trapezoidDetInnerRadius, trapezoidDetOuterRadius, uvAngleCosineDirection) ;
            
            xpos = 0.5 * (trapezoidDetLength + ( (clusterPos1 - clusterPos2) / uvAngleCosineDirection ) );
            ypos = 0.5 * (clusterPos1 + clusterPos2) ;
        }
        
        if ( readoutBoard == "PADPLANE")  {
            SRSHit * hit = ((SRSCluster *) clusterListX->At(k))->GetHit(0) ;
            vector<Float_t> padPosition = hit->GetPadPosition() ;
            xpos = hit->GetPadPosition() [0];
            ypos = hit->GetPadPosition() [1];
            adcCount1 =  hit->GetHitADCs() ;
            adcCount2 =  hit->GetHitADCs() ;
        }
        
        if ( (adcCount1 == 0) || (adcCount2 == 0) ) {
            fClustersInDetectorPlaneMap[detPlaneX.Data()].clear() ;
            fClustersInDetectorPlaneMap[detPlaneY.Data()].clear() ;
            continue ;
        }
        detectorClusterMap[k].clear() ;
        detectorClusterMap[k].push_back(xpos) ;
        detectorClusterMap[k].push_back(ypos) ;
        detectorClusterMap[k].push_back(adcCount1) ;
        detectorClusterMap[k].push_back(adcCount2) ;
        detectorClusterMap[k].push_back(timing1) ;
        detectorClusterMap[k].push_back(timing2) ;
        detectorClusterMap[k].push_back(clusterPos1) ;
        detectorClusterMap[k].push_back(clusterPos2) ;
    }
    
    DeleteListOfClusters(clusterListX) ;
    DeleteListOfClusters(clusterListY) ;
    listOfClustersX.clear() ;
    listOfClustersY.clear() ;
    
    return detectorClusterMap ;
}

