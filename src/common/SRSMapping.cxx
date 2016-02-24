#include "SRSMapping.h"

SRSMapping * SRSMapping::instance = 0 ;

//======================================================================================================================================
/**
 void SRSMapping::Set2DStripsDetectorMap(TString readoutBoard, TString detectorType, TString detector, Int_t detID, TString planeX,  Float_t sizeX, Int_t connectorsX, Int_t orientX, TString planeY,  Float_t sizeY, Int_t connectorsY, Int_t orientY) {
 
 printf("   SRSMapping::SetDetectorMap() => readout=%s, detType=%s, det=%s, detID=%d, planeX=%s, SizeX=%f, connectorsX=%d, orientationX=%d, planeY=%s, SizeY=%f, connectorsY=%d, orientationY=%d \n", readoutBoard.Data(), detectorType.Data(), detector.Data(), detID, planeX.Data(), sizeX, connectorsX, orientX, planeY.Data(), sizeY, connectorsY, orientY) ;
 
 fDetectorFromIDMap[detID]     = detector ;
 fReadoutBoardFromIDMap[detID] = readoutBoard ;
 
 fDetectorIDFromDetectorMap[detector] = detID ;
 fReadoutBoardFromDetectorMap[detector] = readoutBoard ;
 fDetectorTypeFromDetectorMap[detector] = detectorType ;
 
 fDetectorListFromDetectorTypeMap[detectorType].push_back(detector) ;
 fDetectorListFromReadoutBoardMap[readoutBoard].push_back(detector) ;
 
 fPlaneIDFromPlaneMap[planeX] = 0 ;
 fPlaneIDFromPlaneMap[planeY] = 1 ;
 
 fDetectorFromPlaneMap[planeX] = detector ;
 fDetectorFromPlaneMap[planeY] = detector ;
 
 fDetectorPlaneListFromDetectorMap[detector].push_back(planeX) ;
 fDetectorPlaneListFromDetectorMap[detector].push_back(planeY) ;
 
 fCartesianPlaneMap[planeX].push_back(0) ;
 fCartesianPlaneMap[planeX].push_back(sizeX) ;
 fCartesianPlaneMap[planeX].push_back(connectorsX) ;
 fCartesianPlaneMap[planeX].push_back(orientX) ;
 fCartesianPlaneMap[planeY].push_back(1) ;
 fCartesianPlaneMap[planeY].push_back(sizeY) ;
 fCartesianPlaneMap[planeY].push_back(connectorsY) ;
 fCartesianPlaneMap[planeY].push_back(orientY) ;
 }
 */

//======================================================================================================================================
void SRSMapping::SetCartesianStripsReadoutMap(TString readoutBoard, TString detectorType, TString detector, Int_t detID, TString planeX,  Float_t sizeX, Int_t connectorsX, Int_t orientX, TString planeY,  Float_t sizeY, Int_t connectorsY, Int_t orientY) {
    
    //printf("   SRSMapping::SetDetectorMap() => readout=%s, detType=%s, det=%s, detID=%d, planeX=%s, SizeX=%f, connectorsX=%d, orientationX=%d, planeY=%s, SizeY=%f, connectorsY=%d\n", readoutBoard.Data(), detectorType.Data(), detector.Data(), detID, planeX.Data(), sizeX, connectorsX, orientX, planeY.Data(), sizeY, connectorsY, orientY) ;
    
    fDetectorFromIDMap[detID]     = detector ;
    fReadoutBoardFromIDMap[detID] = readoutBoard ;
    
    fDetectorIDFromDetectorMap[detector] = detID ;
    fReadoutBoardFromDetectorMap[detector] = readoutBoard ;
    fDetectorTypeFromDetectorMap[detector] = detectorType ;
    
    fDetectorListFromDetectorTypeMap[detectorType].push_back(detector) ;
    fDetectorListFromReadoutBoardMap[readoutBoard].push_back(detector) ;
    
    fPlaneIDFromPlaneMap[planeX] = 0 ;
    fPlaneIDFromPlaneMap[planeY] = 1 ;
    
    fDetectorFromPlaneMap[planeX] = detector ;
    fDetectorFromPlaneMap[planeY] = detector ;
    
    fDetectorPlaneListFromDetectorMap[detector].push_back(planeX) ;
    fDetectorPlaneListFromDetectorMap[detector].push_back(planeY) ;
    
    fCartesianPlaneMap[planeX].push_back(0) ;
    fCartesianPlaneMap[planeX].push_back(sizeX) ;
    fCartesianPlaneMap[planeX].push_back(connectorsX) ;
    fCartesianPlaneMap[planeX].push_back(orientX) ;
    fCartesianPlaneMap[planeY].push_back(1) ;
    fCartesianPlaneMap[planeY].push_back(sizeY) ;
    fCartesianPlaneMap[planeY].push_back(connectorsY) ;
    fCartesianPlaneMap[planeY].push_back(orientY) ;
}

//======================================================================================================================================
void SRSMapping::SetUVStripsReadoutMap(TString readoutBoard, TString detectorType, TString detector, Int_t detID,  Float_t length,  Float_t innerR,  Float_t outerR, TString planeTop, Int_t conectTop, Int_t orientTop, TString planeBot, Int_t connectBot, Int_t orientBot) {
    
    //  printf("   SRSMapping::SetDetectorMap() => readout=%s, detType=%s, det=%s, detID=%d, planeX=%s, SizeX=%f, connectorsX=%d, orientationX=%d, planeY=%s, SizeY=%f, connectorsY=%d, orientationY=%d \n", readoutBoard.Data(), detectorType.Data(), detector.Data(), detID, planeX.Data(), sizeX, connectorsX, orientX, planeY.Data(), sizeY, connectorsY, orientY) ;
    
    fDetectorFromIDMap[detID]     = detector ;
    fReadoutBoardFromIDMap[detID] = readoutBoard ;
    
    fDetectorIDFromDetectorMap[detector] = detID ;
    fReadoutBoardFromDetectorMap[detector] = readoutBoard ;
    fDetectorTypeFromDetectorMap[detector] = detectorType ;
    
    fDetectorListFromDetectorTypeMap[detectorType].push_back(detector) ;
    fDetectorListFromReadoutBoardMap[readoutBoard].push_back(detector) ;
    
    fPlaneIDFromPlaneMap[planeTop] = 0 ;
    fPlaneIDFromPlaneMap[planeBot] = 1 ;
    
    fDetectorFromPlaneMap[planeTop] = detector ;
    fDetectorFromPlaneMap[planeBot] = detector ;
    
    fDetectorPlaneListFromDetectorMap[detector].push_back(planeTop) ;
    fDetectorPlaneListFromDetectorMap[detector].push_back(planeBot) ;
    
    fUVangleReadoutMap[detector].push_back(length) ;
    fUVangleReadoutMap[detector].push_back(innerR) ;
    fUVangleReadoutMap[detector].push_back(outerR) ;
    
    fUVangleReadoutMap[planeTop].push_back(0) ;
    fUVangleReadoutMap[planeTop].push_back(conectTop) ;
    fUVangleReadoutMap[planeTop].push_back(orientTop) ;
    fUVangleReadoutMap[planeBot].push_back(1) ;
    fUVangleReadoutMap[planeBot].push_back(connectBot) ;
    fUVangleReadoutMap[planeBot].push_back(orientBot) ;
    
}

//======================================================================================================================================
void SRSMapping::Set1DStripsReadoutMap(TString readoutBoard, TString detectorType, TString detector, Int_t detID, TString plane,  Float_t size, Int_t connectors, Int_t orient) {
    
    printf("   SRSMapping::SetDetectorMap() => readout=%s, detType=%s, det=%s, detID=%d, plane=%s, Size=%f, connectors=%d, orientation=%d \n", readoutBoard.Data(), detectorType.Data(), detector.Data(), detID, plane.Data(), size, connectors, orient) ;
    
    fDetectorFromIDMap[detID]     = detector ;
    fReadoutBoardFromIDMap[detID] = readoutBoard ;
    
    fDetectorIDFromDetectorMap[detector] = detID ;
    fReadoutBoardFromDetectorMap[detector] = readoutBoard ;
    fDetectorTypeFromDetectorMap[detector] = detectorType ;
    
    fDetectorListFromDetectorTypeMap[detectorType].push_back(detector) ;
    fDetectorListFromReadoutBoardMap[readoutBoard].push_back(detector) ;
    
    fPlaneIDFromPlaneMap[plane] = 0 ;
    fDetectorFromPlaneMap[plane] = detector ;
    fDetectorPlaneListFromDetectorMap[detector].push_back(plane) ;
    
    f1DStripsPlaneMap[plane].push_back(0) ;
    f1DStripsPlaneMap[plane].push_back(size) ;
    f1DStripsPlaneMap[plane].push_back(connectors) ;
    f1DStripsPlaneMap[plane].push_back(orient) ;
}

//======================================================================================================================================
void SRSMapping::SetCMSGEMReadoutMap(TString readoutBoard, TString detectorType,  TString detector, Int_t detID, TString EtaSector,  Float_t etaSectorPos, Float_t etaSectorSize, Float_t nbOfSectorConnectors, Int_t apvOrientOnEtaSector) {
    printf("   SRSMapping::SetDetectorMap() =>readout=%s, detType=%s, det=%s, detID=%d, EtaSector=%s, etaSectorSize=%f, nbOSectorfConnectors=%f, apvOrientOnEtaSector=%d\n", readoutBoard.Data(), detectorType.Data(), detector.Data(), detID, EtaSector.Data(), etaSectorSize, nbOfSectorConnectors, apvOrientOnEtaSector) ;
    
    fDetectorFromIDMap[detID]     = detector ;
    fReadoutBoardFromIDMap[detID] = readoutBoard ;
    
    fDetectorIDFromDetectorMap[detector] = detID ;
    fReadoutBoardFromDetectorMap[detector] = readoutBoard ;
    fDetectorTypeFromDetectorMap[detector] = detectorType ;
    fDetectorListFromDetectorTypeMap[detectorType].push_back(detector) ;
    fDetectorListFromReadoutBoardMap[readoutBoard].push_back(detector) ;
    
    fDetectorFromPlaneMap[EtaSector] = detector ;
    fDetectorPlaneListFromDetectorMap[detector].push_back(EtaSector) ;
    
    fCMSGEMDetectorMap[EtaSector].push_back(etaSectorPos) ;
    fCMSGEMDetectorMap[EtaSector].push_back(etaSectorSize) ;
    fCMSGEMDetectorMap[EtaSector].push_back(nbOfSectorConnectors) ;
    fCMSGEMDetectorMap[EtaSector].push_back(apvOrientOnEtaSector) ;
}

//======================================================================================================================================
void SRSMapping::SetPadsReadoutMap(TString readoutBoard, TString detectorType,  TString detector, Int_t detID, TString padPlane, Float_t padSizeX,  Float_t padSizeY, Float_t nbOfPadX, Float_t nbOfPadY, Float_t nbOfConnectors) {
    printf("   SRSMapping::SetDetectorMap() =>readout=%s, detType=%s, det=%s, detID=%d, padPlane=%s, nbOfPadX=%f, padSizeX=%f, nbOfPadY=%f, padSizeY=%f, nbOfConnectors=%f \n", readoutBoard.Data(), detectorType.Data(), detector.Data(), detID, padPlane.Data(), nbOfPadX, padSizeX, nbOfPadY, padSizeY, nbOfConnectors) ;
    
    fDetectorFromIDMap[detID]     = detector ;
    fReadoutBoardFromIDMap[detID] = readoutBoard ;
    
    fDetectorIDFromDetectorMap[detector] = detID ;
    fReadoutBoardFromDetectorMap[detector] = readoutBoard ;
    fDetectorTypeFromDetectorMap[detector] = detectorType ;
    fDetectorListFromDetectorTypeMap[detectorType].push_back(detector) ;
    fDetectorListFromReadoutBoardMap[readoutBoard].push_back(detector) ;
    
    fDetectorFromPlaneMap[padPlane] = detector ;
    fDetectorPlaneListFromDetectorMap[detector].push_back(padPlane) ;
    fPlaneIDFromPlaneMap[padPlane]  = 0 ;
    
    fPadDetectorMap[detector].push_back(padSizeX) ;
    fPadDetectorMap[detector].push_back(padSizeY) ;
    fPadDetectorMap[detector].push_back(nbOfPadX) ;
    fPadDetectorMap[detector].push_back(nbOfPadY) ;
    fPadDetectorMap[detector].push_back(nbOfConnectors) ;
}

//=========================================================================================================================
TString SRSMapping::GetAPV(TString detPlane, Int_t fecId, Int_t adcCh, Int_t apvNo, Int_t apvIndex, Int_t apvID) {
    stringstream out ;
    out << apvID ;
    TString apvIDStr = out.str();
    out.str("") ;
    out << fecId ;
    TString fecIDStr = out.str();
    out.str("") ;
    out <<  adcCh;
    TString adcChStr = out.str();
    out.str("") ;
    out <<  apvNo ;
    TString apvNoStr = out.str();
    out.str("") ;
    out <<  apvIndex ;
    TString apvIndexStr = out.str();
    out.str("") ;
    TString apvName = "apv" + apvNoStr + "_Id" + apvIDStr + "_" + detPlane + "_adcCh" + adcChStr + "_FecId" + fecIDStr ;
    return apvName ;
}

//======================================================================================================================================
void  SRSMapping::SetAPVMap(TString detPlane, Int_t fecId, Int_t adcCh, Int_t apvNo, Int_t apvOrient, Int_t apvIndex, Int_t apvHdr, Int_t stripmapping) {
    
    Int_t apvID = (fecId << 4) | adcCh ;
    
    TString apvName = GetAPV(detPlane, fecId, adcCh, apvNo, apvIndex, apvID) ;
    
    fAPVNoFromIDMap[apvID]           = apvNo ;
    fAPVIDFromAPVNoMap[apvNo]        = apvID ;
    fAPVFromIDMap[apvID]             = apvName ;
    fAPVHeaderLevelFromIDMap[apvID]  = apvHdr ;
    fAPVOrientationFromIDMap[apvID]  = apvOrient ;
    fAPVstripmappingFromIDMap[apvID]  = stripmapping ;
    
    fAPVIndexOnPlaneFromIDMap[apvID] = apvIndex ;
    
    fAPVIDFromNameMap[apvName]       = apvID ;
    fDetectorPlaneFromAPVIDMap[apvID] = detPlane ;
    
    fAPVIDListFromFECIDMap[fecId].push_back(apvID);
    fFECIDListFromDetectorPlaneMap[detPlane].push_back(fecId);
    fAPVIDListFromDetectorPlaneMap[detPlane].push_back(apvID);
    
    TString detector = GetDetectorFromPlane(detPlane) ;
    fAPVIDListFromDetectorMap[detector].push_back(apvID);
}

//======================================================================================================================================
void  SRSMapping::SetAPVtoPadMapping(Int_t fecId, Int_t adcCh, Int_t padId, Int_t apvCh) {
    Int_t apvID      = (fecId << 4) | adcCh ;
    Int_t apvChPadCh = (padId << 8) | apvCh ;
    fAPVToPadChannelMap[apvID].push_back(apvChPadCh) ;
}

//======================================================================================================================================
void SRSMapping::PrintMapping() {
    
    map<TString, list<TString> >::const_iterator det_itr ;
    for(det_itr = fDetectorPlaneListFromDetectorMap.begin(); det_itr != fDetectorPlaneListFromDetectorMap.end(); ++det_itr) {
        TString detector = det_itr->first ;
        printf("  ======================================================================================================================\n") ;
        printf("  SRSMapping::PrintMapping() ==> Detector = %s \n",detector.Data()) ;
        
        list<TString> detPlaneList = det_itr->second ;
        list<TString>::const_iterator plane_itr ;
        for(plane_itr = detPlaneList.begin(); plane_itr != detPlaneList.end(); ++plane_itr) {
            TString detPlane = * plane_itr ;
            list <Int_t> fecList = GetFECIDListFromDetectorPlane(detPlane) ;
            list<Int_t>::const_iterator fec_itr ;
            for(fec_itr = fecList.begin(); fec_itr != fecList.end(); ++fec_itr) {
                Int_t fecId = * fec_itr ;
                printf("  SRSMapping::PrintMapping()     Plane = %s,        FEC = %d \n", detPlane.Data(), fecId) ;
                
                list <Int_t> apvList = GetAPVIDListFromDetectorPlane(detPlane) ;
                list<Int_t>::const_iterator apv_itr ;
                for(apv_itr = apvList.begin(); apv_itr != apvList.end(); ++apv_itr) {
                    Int_t apvID       = * apv_itr ;
                    Int_t apvNo       = GetAPVNoFromID(apvID);
                    Int_t apvIndex    = GetAPVIndexOnPlane(apvID);
                    Int_t apvOrient   = GetAPVOrientation(apvID);
                    Int_t stripmapping = GetAPVstripmapping(apvID);
                    Int_t fecID       = GetFECIDFromAPVID(apvID);
                    Int_t adcCh       = GetADCChannelFromAPVID(apvID);
                    Int_t apvHdrLevel = GetAPVHeaderLevelFromID(apvID);
                    TString  apvName  = GetAPVFromID(apvID) ;
                    if(fecID == fecId) printf("  SRSMapping::PrintMapping() ==> adcCh=%d,  apvName=%s,  apvID=%d, apvNo=%d,  apvIndex=%d,  apvOrientation=%d,  apvHdr=%d \n", adcCh, apvName.Data(), apvID, apvNo, apvIndex, apvOrient, apvHdrLevel) ;
                }
            }
            printf("\n") ;
        }
    }
    printf("======================================================================================================================\n") ;
    printf("  SRSMapping::PrintMapping() ==> Mapping of %d detectors, %d planes, %d FECs, %d APVs\n", GetNbOfDetectors(), GetNbOfDetectorPlane(), GetNbOfFECs(), GetNbOfAPVs());
    printf("======================================================================================================================\n") ;
}

//======================================================================================================================================
void SRSMapping::SaveMapping(const char * file) {
    printf("  SRSMapping::SaveMapping() ==> Saving SRS Mapping to file [%s],\n", file) ;
    FILE * f = fopen(file, "w");
    
    fprintf(f,"#################################################################################################\n") ;
    fprintf(f,"         readoutType  Detector    Plane  DetNo   Plane   size (mm)  connectors  orientation\n");
    fprintf(f,"#################################################################################################\n") ;
    
    map<TString, list<TString> >::const_iterator det_itr ;
    for(det_itr = fDetectorPlaneListFromDetectorMap.begin(); det_itr != fDetectorPlaneListFromDetectorMap.end(); ++det_itr) {
        TString detector      = det_itr->first ;
        TString readoutBoard = GetReadoutBoardFromDetector(detector) ;
        TString detectorType = GetDetectorTypeFromDetector(detector) ;
        
        if ( (readoutBoard == "CARTESIAN") ||  (readoutBoard == "UV_ANGLE_OLD") ){
            list<TString> detPlaneList = det_itr->second ;
            TString planeX    = detPlaneList.front() ;
            vector <Float_t> cartesianPlaneX = GetCartesianReadoutMap(planeX) ;
            Float_t sizeX     = cartesianPlaneX[1] ;
            Int_t connectorsX = (Int_t) (cartesianPlaneX[2]) ;
            Int_t orientX     = (Int_t) (cartesianPlaneX[3]) ;
            
            TString planeY    = detPlaneList.back() ;
            vector <Float_t> cartesianPlaneY = GetCartesianReadoutMap(planeY) ;
            Float_t sizeY     = cartesianPlaneY[1] ;
            Int_t connectorsY = (Int_t) (cartesianPlaneY[2]) ;
            Int_t orientY     = (Int_t) (cartesianPlaneY[3]) ;
            fprintf(f,"DET,  %s,   %s,   %s,   %s,  %f,   %d,   %d,   %s,   %f,   %d,   %d \n", readoutBoard.Data(), detectorType.Data(), detector.Data(), planeX.Data(), sizeX, connectorsX, orientX, planeY.Data(), sizeY, connectorsY, orientY ) ;
        }
        else if (readoutBoard == "UV_ANGLE") {
            list<TString> detPlaneList = det_itr->second ;
            TString planeX    = detPlaneList.front() ;
            vector <Float_t> cartesianPlaneX = GetCartesianReadoutMap(planeX) ;
            Float_t sizeX     = cartesianPlaneX[1] ;
            Int_t connectorsX = (Int_t) (cartesianPlaneX[2]) ;
            Int_t orientX     = (Int_t) (cartesianPlaneX[3]) ;
            
            TString planeY    = detPlaneList.back() ;
            vector <Float_t> cartesianPlaneY = GetCartesianReadoutMap(planeY) ;
            Float_t sizeY     = cartesianPlaneY[1] ;
            Int_t connectorsY = (Int_t) (cartesianPlaneY[2]) ;
            Int_t orientY     = (Int_t) (cartesianPlaneY[3]) ;
            fprintf(f,"DET,  %s,   %s,   %s,   %s,  %f,   %d,   %d,   %s,   %f,   %d,   %d \n", readoutBoard.Data(), detectorType.Data(), detector.Data(), planeX.Data(), sizeX, connectorsX, orientX, planeY.Data(), sizeY, connectorsY, orientY ) ;
        }
        
        else if (readoutBoard == "PADPLANE") {
            list<TString> detPlaneList = det_itr->second ;
            TString padPlane = detPlaneList.back() ;
            Float_t padSizeX = GetPadDetectorMap(detector) [0];
            Float_t padSizeY = GetPadDetectorMap(detector) [1];
            Float_t nbOfPadX = GetPadDetectorMap(detector) [2];
            Float_t nbOfPadY = GetPadDetectorMap(detector) [3];
            Float_t nbOfCons = GetPadDetectorMap(detector) [4];
            fprintf(f,"DET,  %s,   %s,   %s,  %s,  %f,   %f,   %f,   %f,  %f\n", readoutBoard.Data(), detectorType.Data(), detector.Data(), padPlane.Data(), padSizeX, nbOfPadX,  padSizeY, nbOfPadY, nbOfCons) ;
        }
        
        else if (readoutBoard == "CMSGEM") {
            list<TString> detPlaneList = det_itr->second ;
        }
        
        else {
            printf("  SRSMapping::SaveMapping() ==> detector readout board type %s is not yet implemented ==> PLEASE MOVE ON \n", readoutBoard.Data()) ;
            continue ;
        }
    }
    
    fprintf(f,"###############################################################\n") ;
    fprintf(f,"#     fecId   adcCh   detPlane  apvOrient  apvIndex    apvHdr #\n");
    fprintf(f,"###############################################################\n") ;
    map<Int_t, TString>::const_iterator apv_itr;
    for(apv_itr = fAPVFromIDMap.begin(); apv_itr != fAPVFromIDMap.end(); ++apv_itr){
        Int_t apvID      = apv_itr->first;
        Int_t fecId      = GetFECIDFromAPVID(apvID);
        Int_t adcCh      = GetADCChannelFromAPVID(apvID);
        TString detPlane = GetDetectorPlaneFromAPVID(apvID) ;
        Int_t apvOrient  = GetAPVOrientation(apvID);
        Int_t stripmapping   = GetAPVstripmapping(apvID);
        Int_t apvIndex   = GetAPVIndexOnPlane(apvID);
        Int_t apvHdr     = GetAPVHeaderLevelFromID(apvID);
        fprintf(f,"APV,   %d,     %d,     %s,     %d,    %d,   %d\n", fecId, adcCh, detPlane.Data(), apvOrient, apvIndex, apvHdr);
    }
    fclose(f);
}

//============================================================================================
void SRSMapping::LoadDefaultMapping(const char * mappingCfgFilename) {
    
    Clear() ;
    printf("  SRSMapping::LoadDefaultMapping() ==> Loading Mapping from %s \n", mappingCfgFilename) ;
    Int_t apvNo = 0 ;
    Int_t detID = 0 ;
    
    ifstream filestream (mappingCfgFilename, ifstream::in);
    TString line;
    while (line.ReadLine(filestream)) {
        
        line.Remove(TString::kLeading, ' ');   // strip leading spaces
        if (line.BeginsWith("#")) continue ;   // and skip comments
        //printf("   SRSMapping::LoadDefaultMapping() ==> Scanning the mapping cfg file %s\n",line.Data()) ;
        
        //=== Create an array of the tokens separated by "," in the line;
        TObjArray * tokens = line.Tokenize(",");
        
        //=== iterator on the tokens array
        TIter myiter(tokens);
        while (TObjString * st = (TObjString*) myiter.Next()) {
            
            //== Remove leading and trailer spaces
            TString s = st->GetString().Remove(TString::kLeading, ' ' );
            s.Remove(TString::kTrailing, ' ' );
            
            //      printf("    SRSMapping::LoadDefaultMapping() ==> Data ==> %s\n",s.Data()) ;
            if(s == "DET") {
                TString readoutBoard = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                TString detectorType = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                TString detector     = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                
                if (readoutBoard == "CARTESIAN")  {
                    TString planeX           = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    Float_t sizeX            = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                    Int_t   nbOfConnectorsX  = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                    Int_t   orientationX     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                    
                    TString planeY           = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    Float_t sizeY            = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                    Int_t   nbOfConnectorsY  = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                    Int_t   orientationY     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                    SetCartesianStripsReadoutMap(readoutBoard, detectorType, detector, detID, planeX, sizeX, nbOfConnectorsX, orientationX, planeY, sizeY, nbOfConnectorsY, orientationY) ;
                }
                
                else if (readoutBoard == "1DSTRIPS")  {
                    TString plane           = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    Float_t size            = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                    Int_t   nbOfConnectors  = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                    Int_t   orientation     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                    Set1DStripsReadoutMap(readoutBoard, detectorType, detector, detID, plane, size, nbOfConnectors, orientation);
                }
                
                else if (readoutBoard == "UV_ANGLE_OLD") {
                    TString planeX           = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    Float_t sizeX            = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                    Int_t   nbOfConnectorsX  = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                    Int_t   orientationX     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                    
                    TString planeY           = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    Float_t sizeY            = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                    Int_t   nbOfConnectorsY  = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                    Int_t   orientationY     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                    SetCartesianStripsReadoutMap(readoutBoard, detectorType, detector, detID, planeX, sizeX, nbOfConnectorsX, orientationX, planeY, sizeY, nbOfConnectorsY, orientationY) ;
                }
                
                else if (readoutBoard == "UV_ANGLE") {
                    Float_t length           = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                    Float_t outerRadius      = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                    Float_t innerRadius      = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                    
                    TString planeTop           = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    Int_t   nbOfConnectorsTop  = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                    Int_t   orientationTop     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                    
                    TString planeBot           = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    Int_t   nbOfConnectorsBot  = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                    Int_t   orientationBot     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                    SetUVStripsReadoutMap(readoutBoard, detectorType, detector, detID, length, innerRadius, outerRadius, planeTop, nbOfConnectorsTop, orientationTop, planeBot, nbOfConnectorsBot, orientationBot) ;
                }
                
                else if (readoutBoard == "PADPLANE") {
                    TString padPlane     = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    Float_t padSizeX     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                    Float_t nbPadX       = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                    Float_t padSizeY     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                    Float_t nbPadY       = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                    Float_t nbConnectors = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                    SetPadsReadoutMap(readoutBoard, detectorType, detector, detID, padPlane, padSizeX, padSizeY, nbPadX, nbPadY, nbConnectors) ;
                }
                
                else if (readoutBoard == "CMSGEM") {
                    TString etaSector     = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                    Float_t etaSectorPos = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                    Float_t etaSectorSize = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                    Float_t nbConnectors        = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                    Int_t orientation     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                    SetCMSGEMReadoutMap(readoutBoard, detectorType, detector, detID, etaSector, etaSectorPos, etaSectorSize, nbConnectors, orientation) ;
                }
                
                else {
                    printf("XXXXXXX SRSMapping::LoadDefaultMapping()==> detector with this readout board type %s is not yet implemented ==> PLEASE MOVE ON XXXXXXXXXXX \n", readoutBoard.Data()) ;
                    continue ;
                }
                detID++ ;
            }
            
            if(s == "APV") {
                Int_t   fecId     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                Int_t   adcCh     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                TString detPlane  = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                Int_t   apvOrient = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                Int_t   apvIndex  = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                Int_t   apvheader = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                Int_t   stripmapping = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                
                if (detPlane == "NULL") continue ;

                SetAPVMap(detPlane, fecId, adcCh, apvNo, apvOrient, apvIndex, apvheader, stripmapping) ;
                apvNo++ ;
            }
        }
        tokens->Delete();
    }
    printf("\n  ======================================================================================================================\n") ;
}

//============================================================================================
void SRSMapping::LoadAPVtoPadMapping(const char * mappingCfgFilename) {
    //  Clear() ;
    printf("  SRSMapping::LoadAPVtoPadMapping() ==> Loading Mapping from %s \n", mappingCfgFilename) ;
    ifstream filestream (mappingCfgFilename, ifstream::in); 
    TString line;
    while (line.ReadLine(filestream)) {
        
        line.Remove(TString::kLeading, ' ');   // strip leading spaces
        if (line.BeginsWith("#")) continue ;   // and skip comments
        //    printf("   SRSMapping::LoadAPVtoPadMapping ==> Scanning the mapping cfg file %s\n",line.Data()) ;
        
        //=== Create an array of the tokens separated by "," in the line;
        TObjArray * tokens = line.Tokenize(","); 
        
        //=== iterator on the tokens array
        TIter myiter(tokens); 
        while (TObjString * st = (TObjString*) myiter.Next()) {
            
            //== Remove leading and trailer spaces
            TString s = st->GetString().Remove(TString::kLeading, ' ' );
            s.Remove(TString::kTrailing, ' ' );                         
            if(s == "PAD") {
                Int_t apvCh      = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                Int_t padId      = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )) .Atoi();;
                Int_t fecId      = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                Int_t adcCh      = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                SetAPVtoPadMapping(fecId, adcCh, padId, apvCh) ;
            }
        }
        tokens->Delete();
    }
    printf("\n  ======================================================================================================================\n") ;
}

//============================================================================================
void SRSMapping::Clear() {
    printf("  SRSMapping::Clear() ==> Clearing Previous Mapping \n") ;
    
    fAPVIDFromAPVNoMap.clear() ; 
    fAPVIDFromNameMap.clear() ; 
    fAPVIDListFromDetectorMap.clear() ; 
    fAPVIDListFromDetectorPlaneMap.clear() ; 
    fAPVNoFromIDMap.clear() ; 
    fAPVFromIDMap.clear() ; 
    fAPVHeaderLevelFromIDMap.clear() ;
    
    fPlaneIDFromPlaneMap.clear() ;
    fDetectorIDFromDetectorMap.clear() ; 
    fDetectorFromIDMap.clear() ; 
    fDetectorFromAPVIDMap.clear() ;  
    fDetectorFromPlaneMap.clear() ;
    
    fDetectorPlaneFromAPVIDMap.clear() ;
    
    fReadoutBoardFromIDMap.clear() ;
    fReadoutBoardFromDetectorMap.clear() ;
    
    fNbOfAPVsFromDetectorMap.clear() ;
    
    fAPVOrientationFromIDMap.clear() ;
    fAPVstripmappingFromIDMap.clear() ;
    
    fAPVIndexOnPlaneFromIDMap.clear() ;
    
    printf("  SRSMapping::Clear() ==> Previous Mapping cleared \n") ;
}

//============================================================================================
template <typename M> void ClearMapOfList( M & amap ) {
    for ( typename M::iterator it = amap.begin(); it != amap.end(); ++it ) {
        ((*it).second).clear();
    }
    amap.clear() ;
}

