#include "SRSTrack.h"
ClassImp(SRSTrack);

SRSTrack::SRSTrack(const char * cfgname, TString offsetDir, TString zeroSupCut, TString maxClustSize, TString minClustSize, TString maxClustMult, TString amoreAgentId) {
    fZeroSupCut = zeroSupCut.Atoi() ;
    fIsGoodEvent = kFALSE ;
    fIsGoodTrack = kFALSE ;
    
    fRunFilePrefix = "std" ;
    fRunFileValue = "0" ;
    fAmoreAgentID = amoreAgentId ;
    
    fAngleCutMinX = 0. ;
    fAngleCutMinY = 0. ;
    fAngleCutMaxX = 360. ;
    fAngleCutMaxY = 360. ;
    
    fNtupleSizeX = 100;
    fNtupleSizeY = 100;
    fNtupleSizeZ = 4000 ;
    fNtupleTitle = "noNtuple";
    
    fMinClusterSize = 0 ;
    fMaxClusterSize = 1000000 ;
    
    if(fZeroSupCut != 0) {
        fMaxClusterSize = maxClustSize.Atoi() ;
        fMinClusterSize = minClustSize.Atoi() ;
    }
    
    fMaxClusterMultiplicity = maxClustMult.Atoi() ;
    
    ReadCfg(cfgname);
    Int_t nbOfDetectors  = fDetectorList.size() ;
    Int_t nbOfTriggers   = fTriggerList.size() ;
    Int_t nbOfTrackers   = fTrackerList.size() ;
    printf("============  SRSTrack::SRSTrack()==> zeroSupCut = %d, minClustSize = %d,  maxClustSize = %d nbDetectors = %d, nbOfTriggers = %d, nbOfTrackers = %d \n",fZeroSupCut, fMinClusterSize, fMaxClusterSize, nbOfDetectors, nbOfTriggers, nbOfTrackers) ;
}

//=============================================================================================================
SRSTrack::~SRSTrack() {
    fDetXOffset.clear() ;
    fDetYOffset.clear() ;
    fDetZOffset.clear() ;
    
    fFitParameters.clear() ;
    
    fXRangeMaxResiduals.clear() ;
    fYRangeMaxResiduals.clear() ;
    fRRangeMaxResiduals.clear() ;
    fPHIRangeMaxResiduals.clear() ;
    
    fXRangeMinResiduals.clear() ;
    fYRangeMinResiduals.clear() ;
    fRRangeMinResiduals.clear() ;
    fPHIRangeMinResiduals.clear() ;
    
    fYNBinResiduals.clear() ;
    fXNBinResiduals.clear() ;
    fRNBinResiduals.clear() ;
    fPHINBinResiduals.clear() ;
    
    fTriggerList.clear() ;
    fTrackerList.clear() ;
    fDetectorList.clear() ;
    fDetXPlaneInputResolution.clear() ;
    fDetYPlaneInputResolution.clear() ;
    fDetPlaneRotationCorrection.clear() ;
    
    ClearSpacePoints(fTrackSpacePointMap) ;
    ClearSpacePoints(fFittedSpacePointMap) ;
    ClearSpacePoints(fRawDataSpacePointMap) ;
    ClearSpacePoints(fEICstripClusterRawDataYMap) ;
}

//=============================================================================================================
void SRSTrack::DeleteClustersInDetectorPlaneMap( map<TString, list <SRSCluster * > > & stringListMap) {
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
void SRSTrack::ClearSpacePoints( map<TString, vector<Float_t> > & spacePointMap) {
    map<TString, vector<Float_t> >::const_iterator map_itr ;
    for (map_itr = spacePointMap.begin(); map_itr != spacePointMap.end(); map_itr++) {
        vector<Float_t> hit = (*map_itr).second ;
        hit.clear() ;
    }
    spacePointMap.clear() ;
}

//============================================================================================
void SRSTrack::ReadCfg(const char * cfgname) {
    printf("\n============  SRSTrack::ReadCfg() ==> Loading SRS Tracking Configuration from: %s\n", cfgname);
    
    fTriggerList.clear() ;
    fTrackerList.clear() ;
    fDetectorList.clear() ;
    
    ClearSpacePoints(fTrackSpacePointMap) ;
    ClearSpacePoints(fRawDataSpacePointMap) ;
    ClearSpacePoints(fFittedSpacePointMap) ;
    
    ifstream file (cfgname, ifstream::in);
    Int_t iline = 0;
    TString line;
    
    while (line.ReadLine(file)) {
        iline++;
        line.Remove(TString::kLeading, ' ');   // strip leading spaces
        if (line.BeginsWith("#")) continue ;   // and skip comments
        
        TObjArray * tokens = line.Tokenize(","); // Create an array of the tokens separated by "," in the line; lines without "," are skipped
        TIter myiter(tokens);                    // iterator on the tokens array
        while (TObjString * st = (TObjString*) myiter.Next()) {  // inner loop (loop over the line);
            
            TString s = st->GetString().Remove(TString::kLeading, ' ' ); // remove leading spaces
            s.Remove(TString::kTrailing, ' ' );                          // remove trailing spaces
            
            if (!s.Contains("@")) continue ;
            
            if (s == "@RUNFILE") {
                fRunFilePrefix = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                fRunFileValue = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
            }
            
            if (s == "@TRACKANGLECUT") {
                Float_t angleCutMinX = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof() ;
                Float_t angleCutMaxX = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof() ;
                Float_t angleCutMinY = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof() ;
                Float_t angleCutMaxY = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof() ;
                
                fAngleCutMinX = (angleCutMinX * PI ) / 180 ;
                fAngleCutMaxX = (angleCutMaxX * PI ) / 180 ;
                fAngleCutMinY = (angleCutMinY * PI ) / 180 ;
                fAngleCutMaxY = (angleCutMaxY * PI ) / 180 ;
                printf("============  SRSTrack::ReadCfg()  %s ==> anglecut_minX = %f deg , anglecut_maxX = %f deg  anglecut_minY = %f deg , anglecut_maxY = %f deg \n", s.Data(), angleCutMinX, angleCutMaxX, angleCutMinY, angleCutMaxY);
                printf("============  SRSTrack::ReadCfg()  %s ==> anglecut_minX = %f rad , anglecut_maxX = %f rad  anglecut_minY = %f rad , anglecut_maxY = %f rad \n", s.Data(), fAngleCutMinX, fAngleCutMaxX, fAngleCutMinY, fAngleCutMaxY);
                continue;
            }
            
            if (s == "@EVENT3DDISPLAY") {
                fNtupleTitle = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                fNtupleSizeX = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof() ;
                fNtupleSizeY = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof() ;
                fNtupleSizeZ = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof() ;
                printf("============  SRSTrack::ReadCfg() %s ==>  fNtupleSizeX = %f mm, fNtupleSizeY = %f mm, fNtupleSizeZ = %f mm  \n", s.Data(), fNtupleSizeX, fNtupleSizeY, fNtupleSizeZ);
                continue;
            }
            
            if (s == "@DETECTORS") {
                TString name         = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                TString triggerType  = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                TString trackerType  = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                Float_t xOffset = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Float_t yOffset = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Float_t zOffset = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Float_t inputResolution = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Int_t  xnbin = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                Float_t xmin = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Float_t xmax = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Int_t  ynbin = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                Float_t ymin = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Float_t ymax = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                SetDetectorConfig(name, triggerType, trackerType, zOffset, inputResolution, inputResolution, xnbin, xmin, xmax, ynbin, ymin, ymax);
                continue;
            }
            
            if (s == "@TRACKING1") {
                TString name         = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                TString triggerType  = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                TString trackerType  = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                Float_t xOffset = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Float_t yOffset = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Float_t zOffset = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Float_t inputResolution = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Int_t  xnbin = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                Float_t xmin = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Float_t xmax = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Int_t  ynbin = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                Float_t ymin = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Float_t ymax = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                SetDetectorConfig(name, triggerType, trackerType,  zOffset, inputResolution, inputResolution, xnbin, xmin, xmax, ynbin, ymin, ymax);
                continue;
            }
            
            if (s == "@TRACKING2D") {
                TString name         = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                TString triggerType  = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                TString trackerType  = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                Float_t zOffset = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Float_t xinputResol = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Float_t yinputResol = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Int_t  xnbin = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                Float_t xmin = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Float_t xmax = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Int_t  ynbin = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                Float_t ymin = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Float_t ymax = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                SetDetectorConfig(name, triggerType, trackerType, zOffset, xinputResol, yinputResol, xnbin, xmin, xmax, ynbin, ymin, ymax);
                continue;
            }
            
            if (s == "@TRACKING1D") {
                TString name         = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                TString triggerType  = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                TString trackerType  = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
                Float_t zOffset = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Float_t inputResolution = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Int_t  xnbin = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
                Float_t xmin = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                Float_t xmax = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
                SetDetectorConfig(name, triggerType, trackerType, zOffset, inputResolution, inputResolution, xnbin, xmin, xmax, xnbin, xmin, xmax);
                continue;
            }
        }
        tokens->Delete();
    }
    printf("============  SRSTrack::ReadCfg() ==> Exit SRS Tracking Configuration =======================================================\n");
}

//-------------------------------------------------------------------------------------------------
void SRSTrack::LoadFTBFalignementParametersRootFile(TString offsetDir) {
    printf("\n============  SRSTrack::LoadFTBFalignementParametersRootFile()==>Enter %s \n",offsetDir.Data()) ;
    
    TString xyOffsetFilename = offsetDir  + "xyOffset_";
    //  if (fRunFilePrefix == "BeamPosition") xyOffsetFilename = offsetDir + "xyOffset_P"  + fRunFileValue + ".root";
    //  if (fRunFilePrefix == "HVScan")       xyOffsetFilename = offsetDir + "xyOffset_HV" + fRunFileValue + ".root";
    //  if (fRunFilePrefix.Contains("BeamPos")) xyOffsetFilename = offsetDir + "xyOffset_P"  + fRunFileValue + ".root";
    //  if (fRunFilePrefix.Contains("HVScan"))  xyOffsetFilename = offsetDir + "xyOffset_HV" + fRunFileValue + ".root";
    
    Int_t runId = fRunFileValue.Atoi() + fAmoreAgentID.Atoi() ;
    stringstream runIdStrStr ;
    runIdStrStr << runId;
    if (fRunFilePrefix.Contains("BeamPos")) xyOffsetFilename = offsetDir + "xyOffset_P"  + runIdStrStr.str() + ".root";
    if (fRunFilePrefix.Contains("HVScan"))  xyOffsetFilename = offsetDir + "xyOffset_HV" + runIdStrStr.str() + ".root";
    
    printf("  SRSTrack::LoadFTBFalignementParametersRootFile() ==> load FTBF x & y offsets parameters root file %s \n",xyOffsetFilename.Data()) ;
    
    //  Int_t fileNb = fRunFileValue.Atoi() ;
    Int_t fileNb = runId ;
    TFile * xyOffsetFile = new TFile(xyOffsetFilename,"read") ;
    if (!xyOffsetFile->IsOpen()){
        printf("  SRSTrack::LoadFTBFalignementParametersRootFile() **** ERROR ERROR *** Cannot open file %s \n", xyOffsetFilename.Data());
        map<TString, TString>::const_iterator det_itr ;
        for (det_itr = fDetectorList.begin(); det_itr != fDetectorList.end(); det_itr++) {
            TString detName = (*det_itr).first ;
            fDetXOffset[detName] = 0 ;
            fDetYOffset[detName] = 0 ;
        }
    }
    
    else {
        map<TString, TString>::const_iterator det_itr ;
        for (det_itr = fDetectorList.begin(); det_itr != fDetectorList.end(); det_itr++) {
            TString detName = (*det_itr).first ;
            fDetXOffset[detName] = 0 ;
            fDetYOffset[detName] = 0 ;
            
            if(detName == "TrkGEM0")  {
                fDetXOffset[detName] = ((TH1F *) xyOffsetFile->Get("offsetTrkGEM0X"))->GetFunction("fitFunction")->GetParameter("Mean") ;
                fDetYOffset[detName] = ((TH1F *) xyOffsetFile->Get("offsetTrkGEM0Y"))->GetFunction("fitFunction")->GetParameter("Mean") ;
                printf("  SRSTrack::LoadFTBFalignementParametersRootFile() ==> load FTBF: det = %s, offsetX = %f, offsetY = %f \n", detName.Data(), fDetXOffset[detName], fDetYOffset[detName]) ;
            }
            if(detName == "TrkGEM2")  {
                fDetXOffset[detName] = ((TH1F *) xyOffsetFile->Get("offsetTrkGEM2X"))->GetFunction("fitFunction")->GetParameter("Mean") ;
                fDetYOffset[detName] = ((TH1F *) xyOffsetFile->Get("offsetTrkGEM2Y"))->GetFunction("fitFunction")->GetParameter("Mean") ;
                printf("  SRSTrack::LoadFTBFalignementParametersRootFile() ==> load FTBF: det = %s, offsetX = %f, offsetY = %f \n", detName.Data(), fDetXOffset[detName], fDetYOffset[detName]) ;
            }
            
            if(detName == "SBSGEM2")  {
                fDetXOffset[detName] = ((TH1F *) xyOffsetFile->Get("offsetSBSGEM2X"))->GetFunction("fitFunction")->GetParameter("Mean") ;
                fDetYOffset[detName] = ((TH1F *) xyOffsetFile->Get("offsetSBSGEM2Y"))->GetFunction("fitFunction")->GetParameter("Mean") ;
                printf("  SRSTrack::LoadFTBFalignementParametersRootFile() ==> load FTBF: det = %s, offsetX = %f, offsetY = %f \n", detName.Data(), fDetXOffset[detName], fDetYOffset[detName]) ;
            }
            
            if(detName == "SBSGEM1")  {
                fDetXOffset[detName] = ((TH1F *) xyOffsetFile->Get("offsetSBSGEM1X"))->GetFunction("fitFunction")->GetParameter("Mean") ;
                fDetYOffset[detName] = ((TH1F *) xyOffsetFile->Get("offsetSBSGEM1Y"))->GetFunction("fitFunction")->GetParameter("Mean") ;
                printf("  SRSTrack::LoadFTBFalignementParametersRootFile() ==> load FTBF: det = %s, offsetX = %f, offsetY = %f \n", detName.Data(), fDetXOffset[detName], fDetYOffset[detName]) ;
            }
            
            if(detName == "EIC1")  {
                //	if ( (fileNb > 3) && (fileNb < 7) ) continue ;
                //	if ( (fileNb < 7) || (fileNb > 19) ) continue ;
                if ( ( (fileNb < 7) || (fileNb > 19) ) && (fRunFilePrefix.Contains("25GeVHadronBeam")) ) continue ;
                if ( ( (fileNb > 3) && (fileNb < 7) ) && (fRunFilePrefix.Contains("120GeVProtonBeam")) ) continue ;
                fDetXOffset[detName] = ((TH1F *) xyOffsetFile->Get("offsetEIC1TOP"))->GetFunction("fitFunction")->GetParameter("Mean") ;
                fDetYOffset[detName] = ((TH1F *) xyOffsetFile->Get("offsetEIC1BOT"))->GetFunction("fitFunction")->GetParameter("Mean") ;
                
                printf("  SRSTrack::LoadFTBFalignementParametersRootFile() ==> load FTBF: det = %s, offsetX = %f, offsetY = %f \n", detName.Data(), fDetXOffset[detName], fDetYOffset[detName]) ;
            }
        }
    }
    xyOffsetFile->Close() ;
    
    TString xyPlaneRotationFilename = offsetDir + "xyPlaneRotation_";
    //  if (fRunFilePrefix == "BeamPosition") xyPlaneRotationFilename = offsetDir + "xyPlaneRotation_P"  + fRunFileValue + ".root";
    //  if (fRunFilePrefix == "HVScan")       xyPlaneRotationFilename = offsetDir + "xyPlaneRotation_HV" + fRunFileValue + ".root";
    if (fRunFilePrefix.Contains("BeamPos")) xyPlaneRotationFilename = offsetDir + "xyPlaneRotation_P"  +  runIdStrStr.str() + ".root";
    if (fRunFilePrefix.Contains("HVScan"))  xyPlaneRotationFilename = offsetDir + "xyPlaneRotation_HV" +  runIdStrStr.str() + ".root";
    
    printf("  SRSTrack::LoadFTBFalignementParametersRootFile() ==> load FTBF x/y plane rotation parameters root file %s \n",xyPlaneRotationFilename.Data()) ;
    TFile * xyRotationPlaneFile = new TFile(xyPlaneRotationFilename,"read") ;
    if (!xyRotationPlaneFile->IsOpen()){
        printf("  SRSTrack::LoadFTBFalignementParametersRootFile() **** ERROR ERROR *** Cannot open file %s \n", xyPlaneRotationFilename.Data());
        map<TString, TString>::const_iterator det_itr ;
        for (det_itr = fDetectorList.begin(); det_itr != fDetectorList.end(); det_itr++) {
            TString detName = (*det_itr).first ;
            fDetPlaneRotationCorrection[detName] = 0 ;
        }
    }
    else {
        map<TString, TString>::const_iterator det_itr ;
        for (det_itr = fDetectorList.begin(); det_itr != fDetectorList.end(); det_itr++) {
            TString detName = (*det_itr).first ;
            fDetPlaneRotationCorrection[detName] = 0 ;
            
            if(detName == "TrkGEM0")  {
                fDetPlaneRotationCorrection[detName] = ((TH1F *) xyRotationPlaneFile->Get("rotationTrkGEM0"))->GetFunction("fitFunction")->GetParameter("Mean") ;
                printf("  SRSTrack::LoadFTBFalignementParametersRootFile() ==> load FTBF: det = %s, x/y plane rotation angle = %f\n", detName.Data(), fDetPlaneRotationCorrection[detName]) ;
            }
            if(detName == "TrkGEM2")  {
                fDetPlaneRotationCorrection[detName] = ((TH1F *) xyRotationPlaneFile->Get("rotationTrkGEM2"))->GetFunction("fitFunction")->GetParameter("Mean") ;
                printf("  SRSTrack::LoadFTBFalignementParametersRootFile() ==> load FTBF: det = %s, x/y plane rotation angle = %f\n", detName.Data(), fDetPlaneRotationCorrection[detName]) ;
            }
            
            if(detName == "SBSGEM2")  {
                fDetPlaneRotationCorrection[detName] = ((TH1F *) xyRotationPlaneFile->Get("rotationSBSGEM2"))->GetFunction("fitFunction")->GetParameter("Mean") ;
                printf("  SRSTrack::LoadFTBFalignementParametersRootFile() ==> load FTBF: det = %s, x/y plane rotation angle = %f\n", detName.Data(), fDetPlaneRotationCorrection[detName]) ;
            }
            
            if(detName == "SBSGEM1")  {
                fDetPlaneRotationCorrection[detName] = ((TH1F *) xyRotationPlaneFile->Get("rotationSBSGEM1"))->GetFunction("fitFunction")->GetParameter("Mean") ;
                printf("  SRSTrack::LoadFTBFalignementParametersRootFile() ==> load FTBF: det = %s, x/y plane rotation angle = %f\n", detName.Data(), fDetPlaneRotationCorrection[detName]) ;
            }
            
            if(detName == "EIC1")  {
                //	if ( (fileNb > 3) && (fileNb < 7) ) continue ;
                //	if ( (fileNb < 7) || (fileNb > 19) ) continue ;
                
                if ( ( (fileNb < 7) || (fileNb > 19) ) && (fRunFilePrefix.Contains("25GeVHadronBeam")) ) continue ;
                if ( ( (fileNb > 3) && (fileNb < 7) ) && (fRunFilePrefix.Contains("120GeVProtonBeam")) ) continue ;
                fDetPlaneRotationCorrection[detName] += ((TH1F *) xyRotationPlaneFile->Get("rotationEIC1"))->GetFunction("fitFunction")->GetParameter("Mean") ;
                printf("  SRSTrack::LoadFTBFalignementParametersRootFile() ==> load FTBF: det = %s, x/y plane rotation angle = %f\n", detName.Data(), fDetPlaneRotationCorrection[detName]) ;
            }
        }
    }
    xyRotationPlaneFile->Close() ;
    printf("============  SRSTrack::LoadFTBFalignementParametersRootFile()==> Exit \n\n") ;
}


//============================================================================================
void SRSTrack::SetDetectorConfig(TString detName, TString triggerType, TString trackerType, Float_t zOffset, Float_t xinputRes, Float_t yinputRes, Int_t xnbin, Float_t xmin, Float_t xmax, Int_t ynbin, Float_t ymin, Float_t ymax) {
    
    Int_t fileNb = fRunFileValue.Atoi() + fAmoreAgentID.Atoi() ;
    
    /**
     if(detName == "EIC1")  {
     //	if ( (fileNb > 3) && (fileNb < 7) {
     if ( (fileNb < 7) || (fileNb > 19) ) {
     //    if ( (fileNb < 7) && (fileNb > 19)) {
     trackerType = "noTracker" ;
     triggerType = "noTrigger";
     }
     }
     */
    
    
    if ( (detName == "EIC1") && (fRunFilePrefix.Contains("25GeVHadronBeam")) ) {
        if ( (fileNb > 3) && (fileNb < 7) ) {
            trackerType = "noTracker" ;
            triggerType = "noTrigger";
        }
    }
    
    if ( (detName == "EIC1") && (fRunFilePrefix.Contains("120GeVProtonBeam")) ) {
        if ( (fileNb < 7) && (fileNb > 19) ) {
            trackerType = "noTracker" ;
            triggerType = "noTrigger";
        }
    }
    
    printf("  SRSTrack::SetDetectorConfig() ==> %s,   %s,   %s,   %s,   %s,    %f,   %f,   %f\n", detName.Data(), fRunFilePrefix.Data(),  fRunFileValue.Data(), triggerType.Data(), trackerType.Data(),  zOffset, xinputRes, yinputRes) ;
    
    fDetZOffset[detName] = zOffset;
    fDetXPlaneInputResolution[detName] = xinputRes;
    fDetYPlaneInputResolution[detName] = yinputRes;
    fDetectorList[detName] = detName;
    
    if( (trackerType == "isTracker") || (trackerType == "IsTracker") ) fTrackerList[detName] = trackerType;
    if( (triggerType == "isTrigger") || (triggerType == "IsTrigger"))  fTriggerList[detName] = triggerType;
    
    fXNBinResiduals[detName]     = xnbin;
    fXRangeMinResiduals[detName] = xmin;
    fXRangeMaxResiduals[detName] = xmax ;
    
    fYNBinResiduals[detName]     = ynbin;
    fYRangeMinResiduals[detName] = ymin;
    fYRangeMaxResiduals[detName] = ymax ;
    
    fRNBinResiduals[detName]     = xnbin;
    fRRangeMinResiduals[detName] = xmin;
    fRRangeMaxResiduals[detName] = xmax ;
    
    fPHINBinResiduals[detName]     = ynbin;
    fPHIRangeMinResiduals[detName] = ymin;
    fPHIRangeMaxResiduals[detName] = ymax ;
}


//============================================================================================
Bool_t SRSTrack::IsTracker(TString detName) {
    //  Bool_t isTracker = kFALSE ;
    map < TString, TString > ::const_iterator it = fTrackerList.find(detName) ;
    return it != fTrackerList.end() ;
}

//============================================================================================
Bool_t SRSTrack::IsTrigger(TString detName) {
    map < TString, TString > ::const_iterator it = fTriggerList.find(detName) ;
    return it != fTriggerList.end() ;
}

//============================================================================================
void SRSTrack::BuildRawDataSpacePoints(SRSEventBuilder * eventbuilder) {
    ClearSpacePoints(fRawDataSpacePointMap) ;
    ClearSpacePoints(fEICstripClusterRawDataYMap) ;
    
    fIsGoodEvent = eventbuilder->IsAGoodEvent() ;
    if(fIsGoodEvent) {
        fIsGoodTrack = kTRUE ;
        SRSMapping * mapping = SRSMapping::GetInstance() ;
        map<TString, TString>::const_iterator det_itr ;
        for (det_itr = fDetectorList.begin(); det_itr != fDetectorList.end(); det_itr++) {
            TString detName = (*det_itr).first ;
            if (eventbuilder->IsAGoodEventInDetector(detName) == kFALSE) {
                continue ;
            }
            map < Int_t, vector <Float_t > > detectorEvent = eventbuilder->GetDetectorCluster(detName) ;
            Int_t clusterMult = detectorEvent.size() ;
            for (Int_t k = 0 ; k < clusterMult ; k++) {
                
                fRawDataSpacePointMap[detName].push_back(detectorEvent[k][0] + fDetXOffset[detName]);
                fRawDataSpacePointMap[detName].push_back(detectorEvent[k][1] + fDetYOffset[detName]);
                fRawDataSpacePointMap[detName].push_back(fDetZOffset[detName]) ;
                
                fEICstripClusterRawDataYMap[detName].push_back(detectorEvent[k][6]);
                fEICstripClusterRawDataYMap[detName].push_back(detectorEvent[k][7]);
                
                fEICstripClusterRawDataYMap[detName].push_back(fDetZOffset[detName]);
                detectorEvent[k].clear();
            }
            PlaneRotationCorrection(fDetPlaneRotationCorrection[detName], fRawDataSpacePointMap[detName]) ;
            detectorEvent.clear() ;
        }
    }
}

//============================================================================================
void SRSTrack::BuildTrack() {
    fIsGoodTrack = kFALSE ;
    //  printf(" --- SRSTrack::BuildTrack()\n") ;
    ClearSpacePoints(fTrackSpacePointMap) ;
    if(fIsGoodEvent) {
        fIsGoodTrack = kTRUE ;
        map<TString, TString>::const_iterator tracker_itr ;
        for (tracker_itr = fTrackerList.begin(); tracker_itr != fTrackerList.end(); tracker_itr++) {
            TString detName = (*tracker_itr).first ;
            TString detType = (*tracker_itr).second ;
            fTrackSpacePointMap[detName].clear() ;
            fTrackSpacePointMap[detName] = fRawDataSpacePointMap[detName] ;
        }
    }
}

//====================================================================================================================
void SRSTrack::DoTracking() {
    //  printf(" --- SRSTrack::DoTracking()\n") ;
    fIsGoodTrack = kFALSE ;
    if(fIsGoodEvent) {
        ClearSpacePoints(fFittedSpacePointMap) ;
        SRSTrackFit * trackFit = new SRSTrackFit(fTrackSpacePointMap, fRawDataSpacePointMap, fDetXPlaneInputResolution, fDetYPlaneInputResolution)  ;
        fFittedSpacePointMap = trackFit->GetTrackFittedData() ;
        
        fFitParameters = trackFit->GetFitParameters() ;
        Float_t angleX = fFitParameters["xDirection"] ;
        Float_t angleY = fFitParameters["yDirection"] ;
        angleX = fabs(angleX) ;
        angleY = fabs(angleY) ;
        if ( (angleX > fAngleCutMinX ) && (angleX < fAngleCutMaxX) && (angleY > fAngleCutMinY ) && (angleY < fAngleCutMaxY) ) {
            fIsGoodTrack = kTRUE ;
        }
    }
}

//====================================================================================================================
Bool_t  SRSTrack::IsAGoodTrack(SRSEventBuilder * eventbuilder) {
    fIsGoodTrack = kFALSE ;
    BuildRawDataSpacePoints(eventbuilder) ;
    Int_t nbOfTrackers = fTrackerList.size() ;
    if ((fZeroSupCut != 0) && (nbOfTrackers != 0)) {
        BuildTrack() ;
        DoTracking() ;
    }
    return fIsGoodTrack ;
}


//=========================================================================================================================//
const vector<Float_t> SRSTrack::directionVectorFrom2Points(const vector<Float_t> u, const vector<Float_t> v) {
    
    int sizeV =  v.size();
    int sizeU =  u.size();
    
    //  if(sizeU != 3)  printf("SRSTrack::directionVectorFrom2Points(): WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    //  if(sizeV != 3)  printf("SRSTrack::directionVectorFrom2Points(): WARNING ==> V point vector's size = %d,!= 3  \n",sizeV) ;
    vector<Float_t> dir ;
    dir.resize(3,0) ;
    
    if( (sizeU == 3) && (sizeV == 3) ) {
        vector<Float_t> sub ;
        sub.push_back(v[0] - u[0]) ;
        sub.push_back(v[1] - u[1]) ;
        sub.push_back(v[2] - u[2]) ;
        Float_t norm = normVec(sub) ;
        dir.push_back(sub[0]/norm) ;
        dir.push_back(sub[1]/norm) ;
        dir.push_back(sub[2]/norm) ;
        printf("SRSTrack::directionVectorFrom2Points(): sub[0]=%f, sub[1]=%f, sub[2]=%f, norm=%f \n",sub[0],sub[1],sub[2],norm) ;
    }
    return dir ;
}

//=========================================================================================================================//
Float_t SRSTrack::getAngleTo(const vector<Float_t> u, const vector<Float_t> v) {
    
    Float_t angle = 0.00 ;
    int sizeV =  v.size();
    int sizeU =  u.size();
    
    if( (sizeU == 3) && (sizeV == 3) ) {
        Float_t argument = dotVec(u,v)/(normVec(u)*normVec(v)) ;
        angle = acos(argument) ;
        if(angle != angle) angle = 0.0 ;
        //    printf("SRSTrack::getAngleTo()=> angle = %f \n",angle) ;
    }
    return angle ;
}

//=========================================================================================================================//
Float_t SRSTrack::getAngleAmplitude(const vector<Float_t> u, const vector<Float_t> v) {
    
    Float_t angle = 0.00 ;
    int sizeV =  v.size();
    int sizeU =  u.size();
    
    //  if(sizeU != 3)  printf("SRSTrack::getAngleTo(): WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    //  if(sizeV != 3)  printf("SRSTrack::getAngleTo(): WARNING ==> V point vector's size = %d,!= 3  \n",sizeV) ;
    
    if( (sizeU == 3) && (sizeV == 3) ) {
        Float_t argument = abs(dotVec(u,v))/(normVec(u)*normVec(v)) ;
        angle = acos(argument) ;
        if(angle != angle) angle = 0.0 ;
        //    printf("SRSTrack::getAngleTo()=> angle = %f \n",angle) ;
    }
    return angle ;
}

//=========================================================================================================================//
Float_t SRSTrack::normVec(const vector<Float_t> u) {
    int size =  u.size();
    if(size != 3) printf("SRSTrack::normVec(): WARNING ==> point vector's size = %d,!= 3  \n",size) ;
    return sqrt(dotVec(u,u)) ;
}

//=========================================================================================================================//
Float_t SRSTrack::dotVec(const vector<Float_t> u, const vector<Float_t> v) {
    int sizeV =  v.size();
    int sizeU =  u.size();
    if(sizeU != 3)  printf("SRSTrack::dotVec(): WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    if(sizeV != 3)  printf("SRSTrack::dotVec(): WARNING ==> V point vector's size = %d,!= 3  \n",sizeV) ;
    return v[0]*u[0] + v[1]*u[1] + v[2]*u[2] ;
}

//=========================================================================================================================//
const vector<Float_t> SRSTrack::subVec(const vector<Float_t> u, const vector<Float_t> v) {
    
    int sizeV =  v.size();
    int sizeU =  u.size();
    if(sizeU != 3)  printf("SRSTrack::subVec(): WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    if(sizeV != 3)  printf("SRSTrack::subVec(): WARNING ==> V point vector's size = %d,!= 3  \n",sizeV) ;
    
    vector<Float_t> sub ;
    sub.push_back(v[0] - u[0]) ;
    sub.push_back(v[1] - u[1]) ;
    sub.push_back(v[2] - u[2]) ;
    
    return sub ;
}

//=========================================================================================================================//
const vector<Float_t> SRSTrack::addVec(const vector<Float_t> u, const vector<Float_t> v) {
    
    int sizeV =  v.size();
    int sizeU =  u.size();
    if(sizeU != 3)  printf("SRSTrack::addVec(): WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    if(sizeV != 3)  printf("SRSTrack::addVec(): WARNING ==> V point vector's size = %d,!= 3  \n",sizeV) ;
    
    vector<Float_t> add ;
    add.push_back(u[0] + v[0]) ;
    add.push_back(u[1] + v[1]) ;
    add.push_back(u[2] + v[2]) ;
    
    return add ;
}

//=========================================================================================================================//
const vector<Float_t> SRSTrack::prodVec(Float_t a, const vector<Float_t> u) {
    
    int sizeU =  u.size();
    if(sizeU != 3)  printf("SRSTrack::prodVec(): WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    
    vector<Float_t> prod ;
    
    prod.push_back(a*u[0]) ;
    prod.push_back(a*u[1]) ;
    prod.push_back(a*u[2]) ;
    return prod ;
    
}

//=========================================================================================================================//
void SRSTrack::PlaneRotationCorrection(Float_t alpha, vector<Float_t> & u) {
    int sizeU =  u.size();
    if(sizeU != 3)  printf("SRSTrack::PlaneRotationCorrection: WARNING ==> U point vector's size = %d,!= 3  \n",sizeU) ;
    alpha = -1 * alpha ; 
    u[0] = u[0] * TMath::Cos(alpha) + u[1] * TMath::Sin(alpha) ;
    u[1] = u[1] * TMath::Cos(alpha) - u[0] * TMath::Sin(alpha) ;
    u[2] = u[2] ;
}
//=========================================================================================================================//
Float_t SRSTrack::projectedAngleXY(const vector<Float_t> u, TString xORy) {
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
Float_t SRSTrack::projectedAngleXZ(const vector<Float_t> u, TString xORz) {
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
const vector<Float_t> SRSTrack::getXandYKnowingZ(const vector<Float_t> w, const vector<Float_t> v, Float_t z0) {
    
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
Float_t SRSTrack::abs(Float_t x) {
    return ((x) >= 0 ? (x):(-x)) ; 
}

//=========================================================================================================================//
const vector<Float_t> SRSTrack::getDirection(const vector<Float_t> u) {
    
    int size =  u.size();
    if(size != 3)  printf("SRSTrack::getDirection(): WARNING ==> U point vector's size = %d,!= 3  \n",size) ;
    
    vector<Float_t> direction ;
    
    Float_t x = u[0]/normVec(u) ;
    Float_t y = u[1]/normVec(u) ;
    Float_t z = u[2]/normVec(u) ;
    
    direction.push_back(x) ;
    direction.push_back(y) ;
    direction.push_back(z) ;
    
    return direction ;
}
