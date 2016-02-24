// Author: Kondo GNANVO 18/08/2010
#include "SRSPublisher.h"

//#define DEBUG
ClassImp(amore::SRS::publisher::SRSPublisher)

namespace amore {
    namespace SRS {
        namespace publisher {
            using amore::publisher::Publish;
            
            //-------------------------------------------------------------------------------------------------
            SRSPublisher::SRSPublisher() {
                fEvent         = 0;
                fEquipmentSize = 0;
                fStartEventNumber = "0" ;
                fEventFrequencyNumber = "1" ;
                fMinNbOfEquipments = 1 ;
                fMaxNbOfEquipments = 8 ;
                fEventType     = 0;
                fEventRunNb    = -1;
                fRunType       = "RAWDATA";
                fApvNoFromApvIDMap.clear() ;
                fApvNameFromIDMap.clear() ;
                fApvGainFromApvNoMap.clear() ;
                fMapping = SRSMapping::GetInstance() ;
                fIsClusterPosCorrection = kFALSE ;
                fPlotEtaFunctionHistos= kFALSE ;
                sigmaCut = 0 ;
            }
            
            //-------------------------------------------------------------------------------------------------
            SRSPublisher::~SRSPublisher() {
            }
            
            //-------------------------------------------------------------------------------------------------
            void SRSPublisher::StartOfRun(const amore::core::Run & run) {
                ResetMonitors() ;
                printf("SRSPublisher::StartOfRun() ==> run type = %s; run number = %d\n",fRunType.Data(), run.RunNumber()) ;
            }
            
            //-------------------------------------------------------------------------------------------------
            void SRSPublisher::EndOfRun(const amore::core::Run& run) {
                printf("SRSPublisher::EndOfRun()==> Entering ========= \n") ;
                
                if (fRunType == "PEDESTAL")  {
                    fSavePedFromThisRun->SavePedestalRunHistos() ;
                    fSavePedFromThisRun->Write() ;
                    fPedRootFile->Close() ;
                    printf("SRSPublisher::EndOfRun()==> Leaving ========= \n");
                }
                
                if (fRunType == "RAWPEDESTAL")  {
                    fSaveRawPedFromThisRun->SaveRawPedestalRunHistos() ;
                    fSaveRawPedFromThisRun->Write() ;
                    fRawPedRootFile->Close() ;
                    printf("SRSPublisher::EndOfRun()==> Leaving ========= \n");
                }
                
                if (fRunType == "ETAFUNCTION")  {
                    fSavePosCorrFromThisRun->SaveClusterPositionCorrectionHistos() ;
                    fClusterPositionCorrectionRootFile->Close() ;
                    printf("SRSPublisher::EndOfRun()==> Leaving ========= \n");
                }
                
                if ( (fRunType == "PHYSICS") ||(fRunType == "RAWDATA") ){
                    fHMan->SaveAllHistos() ;
                    if (fRunType == "PHYSICS") {
                        fHMan->SaveFTBF_AnalysisTXT(fTrack) ;
                    }
                    if (fPlotEtaFunctionHistos) fHMan->SavePosCorrectionHistos() ;
                    printf("SRSPublisher::EndOfRun()==> Leaving ========= \n");
                }
                
                if ( fRunType == "APVGAIN" ){
                    fHMan->SaveAPV25GainHistos() ;
                    printf("SRSPublisher::EndOfRun()==> Leaving ========= \n");
                }
                
                if (fRunType == "TRACKING") {
                    fHMan->SaveFTBF_Residuals(fTrack) ;
                    printf("SRSPublisher::EndOfRun()==> Leaving ========= \n");
                }
                
                if (fRunType == "ROOTFILE") {
                    fROOT->WriteRootFile() ;
                    printf("SRSPublisher::EndOfRun()==> Leaving ========= \n");
                }
            }
            
            //-------------------------------------------------------------------------------------------------
            void SRSPublisher::StartOfCycle() {
                //	printf("===> StartOfcycle()\n");
            }
            
            //-------------------------------------------------------------------------------------------------
            void SRSPublisher::EndOfCycle() {
                //     	printf("===> endOfCycle()\n");
                if (fEvent >  fStartEventNumber.Atoi()) {
                    Int_t cycleWait = (fCycleWait.Atoi()) * 1000000 ;
                    usleep(cycleWait) ;
                }
            }
            
            //-------------------------------------------------------------------------------------------------
            void SRSPublisher::BookMonitorObjects() {
                printf("=========================================== SRSPublisher::BookMonitorObjects()==> LOAD CONFIG FILE  \n") ;
                //	printf("=========================================== SRSPublisher::BookMonitorObjects()==> LOAD CONFIG FILE FOR AGENT = %s \n",gPublisher->AgentName() ) ;
                
                //        fCFGFile = TString("$AMORESRS/configFileDir/amore.cfg") ;
                //	std::cout << "AgentName=" << gPublisher->GetAgentName() << std::endl;
                //	TString configFileNameAgent = "$AMORESRS/configFileDir/" + gPublisher->GetAgentName() + "amore.cfg";
                
                std::cout << "AgentName = " << gPublisher->AgentName() << std::endl;
                TString configFileNameAgent = "$AMORESRS/configFileDir/" + gPublisher->AgentName() + "amore.cfg";
                TString configFileName      = "$AMORESRS/configFileDir/amore.cfg";
                
                fAmoreAgentID = gPublisher->AgentName() ;
                fAmoreAgentID = ((TString) (gPublisher->AgentName())).Remove(0,3) ;
                printf(" SRSPublisher::BookMonitorObjects()==> amoreAgent Id = %s \n",fAmoreAgentID.Data()) ;
                
                if (fSRSConf->FileExists(configFileNameAgent.Data()))
                {
                    fCFGFile = configFileNameAgent;
                }
                else
                {
                    fCFGFile = configFileName;
                }
                cout << __PRETTY_FUNCTION__ << "==>LOAD CONFIG FILE to be used " << fCFGFile << " : " << endl;
                
                //=== Initialize the configuration file
                fSRSConf = new SRSConfiguration(fCFGFile.Data());
                
                //=== Initialize
                fCycleWait               = fSRSConf->GetCycleWait() ;
                fStartEventNumber        = fSRSConf->GetStartEventNumber();
                fEventFrequencyNumber    = fSRSConf->GetEventFrequencyNumber();
                
                if(fStartEventNumber.Atoi() < 1 ) {
                    printf(" SRSPublisher::BookMonitorObjects()==> before fStartEventNumber = %s \n",fStartEventNumber.Data()) ;
                    fStartEventNumber = "0" ;
                    printf(" SRSPublisher::BookMonitorObjects()==> after  fStartEventNumber = %s \n",fStartEventNumber.Data()) ;
                    
                }
                
                if(fEventFrequencyNumber.Atoi() < 1 ) {;
                    printf(" SRSPublisher::BookMonitorObjects()==> before fEventFrequency = %s \n",fEventFrequencyNumber.Data()) ;
                    fEventFrequencyNumber = "1" ;
                    printf(" SRSPublisher::BookMonitorObjects()==> after  fEventFrequency = %s \n",fEventFrequencyNumber.Data()) ;
                    
                }
                
                if( fSRSConf->GetRunType() == "") fRunType = "RAWDATA" ;
                else                              fRunType = fSRSConf->GetRunType() ;
                
                TString zeroSupCut        = fSRSConf->GetZeroSupCut() ;
                TString maskedChannelCut  = fSRSConf->GetMaskedChannelCut() ;
                TString minClusterSize    = fSRSConf->GetMinClusterSize() ;
                TString maxClusterSize    = fSRSConf->GetMaxClusterSize() ;
                TString maxClusterMult    = fSRSConf->GetMaxClusterMultiplicity() ;
                TString isCluserPosCorrec = fSRSConf->GetClusterPositionCorrectionFlag() ;
                
                if  (isCluserPosCorrec == "applyCorrections") {
                    fIsClusterPosCorrection = kTRUE ;
                    fPlotEtaFunctionHistos = kFALSE ;
                }
                
                else if  (isCluserPosCorrec == "plotEtaFuncWithCorrections") {
                    fIsClusterPosCorrection = kTRUE ;
                    fPlotEtaFunctionHistos = kTRUE;
                }
                
                else if  (isCluserPosCorrec == "plotEtaFuncWithoutCorrections") {
                    fIsClusterPosCorrection = kFALSE  ;
                    fPlotEtaFunctionHistos =  kTRUE;
                }
                
                else {
                    fIsClusterPosCorrection = kFALSE ;
                    fPlotEtaFunctionHistos = kFALSE ;
                }
                printf("================================================ SRSPublisher::BookMonitorObjects()==> LOAD CONFIG FILE: run type = %s \n\n",fRunType.Data()) ;
                
                printf("================================================ SRSPublisher::BookMonitorObjects()==> LOAD MAPPING started \n") ;
                //=== Get an instance of the mapping class object and load the mapping
                fMapping->LoadDefaultMapping(fSRSConf->GetMappingFile());
                fMapping->LoadAPVtoPadMapping(fSRSConf->GetPadMappingFile());
                fMapping->PrintMapping() ;
                fMapping->SaveMapping(fSRSConf->GetSavedMappingFile()) ;
                
                fApvNoFromApvIDMap = fMapping->GetAPVNoFromIDMap();
                fApvNameFromIDMap  = fMapping->GetAPVFromIDMap();
                fMaxNbOfEquipments = fMapping->GetNbOfFECs() ;
                printf("================================================ SRSPublisher::BookMonitorObjects()==> MAPPING LOADED \n\n") ;
                
                
                //=== RAW DATA RUN
                if (fRunType == "RAWDATA") {
                    //	  printf("SRSPublisher::BookMonitorObjects()==> Book raw data Histos\n") ;
                    fHMan = new SRSHistoManager(fSRSConf->GetHistoCfgName(), zeroSupCut.Atoi(), minClusterSize.Atoi(), maxClusterSize.Atoi(), maxClusterMult.Atoi()) ;
                    fHMan->SetRunParameters(fAmoreAgentID, fSRSConf->GetRunType(), fSRSConf->GetRunName(), "0", "0") ;
                    //	  fHMan->SetRunParameters(fAmoreAgentID, fSRSConf->GetRunType(), fSRSConf->GetRunName(), fTrack->GetRunFilePrefix(), fTrack->GetRunFileValue()) ;
                    fHMan->BookRawDataHisto() ;
                    printf("SRSPublisher::BookMonitorObjects()==> Book raw data Histos\n") ;
                }
                
                //=== For the raw pedestal runs
                if (fRunType == "RAWPEDESTAL") {
                    fRawPedRootFile = new TFile(fSRSConf->GetRawPedestalFile(), "recreate") ;
                    fSaveRawPedFromThisRun = new SRSRawPedestal(fMapping->GetNbOfAPVs());
                    fSaveRawPedFromThisRun->SetRunName(fSRSConf->GetRunName()) ;
                    fSaveRawPedFromThisRun->SetRunType(fSRSConf->GetRunType()) ;
                    printf("SRSPublisher::BookMonitorObjects()==> fSaveRawPedFromThisRun() initialised\n") ;
                }
                
                //=== For pedestal runs
                if (fRunType == "PEDESTAL") {
                    LoadRawPedestalRootFile(fSRSConf->GetRawPedestalFile(), fMapping->GetNbOfAPVs()) ;
                    printf("SRSPublisher::BookMonitorObjects()==> raw pedestal data loaded \n") ;
                    fPedRootFile = new TFile(fSRSConf->GetPedestalFile(), "recreate") ;
                    printf("SRSPublisher::BookMonitorObjects()==> pedestal file created \n") ;
                    fSavePedFromThisRun = new SRSPedestal(fMapping->GetNbOfAPVs(), maskedChannelCut.Atoi());
                    fSavePedFromThisRun->SetRunName(fSRSConf->GetRunName()) ;
                    fSavePedFromThisRun->SetRunType(fSRSConf->GetRunType()) ;
                    printf("SRSPublisher::BookMonitorObjects()==>  fSavePedFromThisRun() initialised \n") ;
                }
                
                // CLUSTER POSITION CORRECTION FUNCTION
                if (fRunType == "ETAFUNCTION") {
                    LoadPedestalRootFile(fSRSConf->GetPedestalFile(), fMapping->GetNbOfAPVs()) ;
                    printf("SRSPublisher::BookMonitorObjects()==> pedestal data loaded \n") ;
                    LoadAPVGainCalibrationRootFile(fSRSConf->GetAPVGainCalibrationFile(), fMapping->GetNbOfAPVs()) ;
                    fHMan = new SRSHistoManager(fSRSConf->GetHistoCfgName(), zeroSupCut.Atoi(), minClusterSize.Atoi(), maxClusterSize.Atoi(), maxClusterMult.Atoi()) ;
                    fClusterPositionCorrectionRootFile = new TFile(fSRSConf->GetClusterPositionCorrectionFile(), "recreate") ;
                    fSavePosCorrFromThisRun = new SRSPositionCorrection(fSRSConf->GetRunName(), fSRSConf->GetRunType()) ;
                    printf("SRSPublisher::BookMonitorObjects()==> fSavedPositionCorrectionRootFile() initialised\n") ;
                }
                
                //=== APV25 GAIN CALIBRATION
                if (fRunType == "APVGAIN") {
                    LoadPedestalRootFile(fSRSConf->GetPedestalFile(), fMapping->GetNbOfAPVs()) ;
                    printf("SRSPublisher::BookMonitorObjects()==> pedestal data loaded \n") ;
                    fTrack = new SRSTrack(fSRSConf->GetHistoCfgName(), fSRSConf->GetTrackingOffsetDir(), fSRSConf->GetZeroSupCut(), fSRSConf->GetMaxClusterSize(),  fSRSConf->GetMinClusterSize(), fSRSConf->GetMaxClusterMultiplicity(), fAmoreAgentID);
                    fHMan = new SRSHistoManager(fSRSConf->GetHistoCfgName(), zeroSupCut.Atoi(), minClusterSize.Atoi(), maxClusterSize.Atoi(), maxClusterMult.Atoi()) ;
                    fHMan->SetRunParameters(fAmoreAgentID, fSRSConf->GetRunType(), fSRSConf->GetRunName(), fTrack->GetRunFilePrefix(), fTrack->GetRunFileValue()) ;
                    fHMan->ReadHistoCfgFile(fSRSConf->GetHistoCfgName()) ;
                    fHMan->BookAPVGainHisto() ;
                    printf("SRSPublisher::BookMonitorObjects()==> book apv gain Histos\n") ;
                }
                
                //===PHYSICS  RUN
                if (fRunType == "PHYSICS") {
                    //LoadPedestalRootFile(fSRSConf->GetPedestalFile(), fMapping->GetNbOfAPVs()) ;
                    printf("SRSPublisher::BookMonitorObjects()==> pedestal data loaded \n") ;
                    LoadAPVGainCalibrationRootFile(fSRSConf->GetAPVGainCalibrationFile(), fMapping->GetNbOfAPVs()) ;
                    fTrack = new SRSTrack(fSRSConf->GetHistoCfgName(), fSRSConf->GetTrackingOffsetDir(), fSRSConf->GetZeroSupCut(), fSRSConf->GetMaxClusterSize(),  fSRSConf->GetMinClusterSize(), fSRSConf->GetMaxClusterMultiplicity(), fAmoreAgentID);
                    fHMan = new SRSHistoManager(fSRSConf->GetHistoCfgName(), zeroSupCut.Atoi(), minClusterSize.Atoi(), maxClusterSize.Atoi(), maxClusterMult.Atoi()) ;
                    fHMan->SetRunParameters(fAmoreAgentID, fSRSConf->GetRunType(), fSRSConf->GetRunName(), fTrack->GetRunFilePrefix(), fTrack->GetRunFileValue()) ;
                    fHMan->ReadHistoCfgFile(fSRSConf->GetHistoCfgName()) ;
                    if (fPlotEtaFunctionHistos) fHMan->BookClusterPositionCorrectionHistos() ;
                    printf("================================================ SRSPublisher::BookMonitorObjects()==> Tracking initialised \n\n") ;
                }
                
                //===TRACKING  RUN
                if (fRunType == "TRACKING") {
                    LoadPedestalRootFile(fSRSConf->GetPedestalFile(), fMapping->GetNbOfAPVs()) ;
                    printf("SRSPublisher::BookMonitorObjects()==> pedestal data loaded \n") ;
                    LoadAPVGainCalibrationRootFile(fSRSConf->GetAPVGainCalibrationFile(), fMapping->GetNbOfAPVs()) ;
                    fTrack = new SRSTrack(fSRSConf->GetHistoCfgName(), fSRSConf->GetTrackingOffsetDir(), fSRSConf->GetZeroSupCut(), fSRSConf->GetMaxClusterSize(),  fSRSConf->GetMinClusterSize(), fSRSConf->GetMaxClusterMultiplicity(), fAmoreAgentID);
                    fTrack->LoadFTBFalignementParametersRootFile(fSRSConf->GetTrackingOffsetDir()) ;
                    fHMan = new SRSHistoManager(fSRSConf->GetHistoCfgName(), zeroSupCut.Atoi(), minClusterSize.Atoi(), maxClusterSize.Atoi(), maxClusterMult.Atoi()) ;
                    fHMan->SetRunParameters(fAmoreAgentID, fSRSConf->GetRunType(), fSRSConf->GetRunName(), fTrack->GetRunFilePrefix(), fTrack->GetRunFileValue()) ;
                    fHMan->BookResidualsHisto(fTrack) ;
                    if(fHMan->GetNtupleToBePublished()) fHMan->BookEvent3DDisplayNtuple(fTrack) ;
                    printf("================================================ SRSPublisher::BookMonitorObjects()==> Tracking initialised \n\n") ;
                }
                
                //===ROOTFILE  RUN
                if (fRunType == "ROOTFILE" ) {
                    // Pedestal file is uploaded only if header is found to be non zero-suppressed
                    // (This is happening in the SRSFECDecoder and at the event==1)
                    //LoadPedestalRootFile(fSRSConf->GetPedestalFile(), fMapping->GetNbOfAPVs()) ;
                    //printf("SRSPublisher::BookMonitorObjects()==> pedestal data loaded \n") ;
                    TString rootData  = fSRSConf->GetROOTDataType() ;
                    LoadAPVGainCalibrationRootFile(fSRSConf->GetAPVGainCalibrationFile(), fMapping->GetNbOfAPVs()) ;
                    fTrack = new SRSTrack(fSRSConf->GetHistoCfgName(), fSRSConf->GetTrackingOffsetDir(), fSRSConf->GetZeroSupCut(), fSRSConf->GetMaxClusterSize(),  fSRSConf->GetMinClusterSize(), fSRSConf->GetMaxClusterMultiplicity(), fAmoreAgentID);
                    if(rootData == "TRACKING_ONLY") fTrack->LoadFTBFalignementParametersRootFile(fSRSConf->GetTrackingOffsetDir()) ;
                    fROOT = new SRSOutputROOT(fSRSConf->GetZeroSupCut(), fSRSConf->GetROOTDataType() ) ;
                    fROOT->SetRunParameters(fAmoreAgentID, fSRSConf->GetRunType(), fSRSConf->GetRunName(), fTrack->GetRunFilePrefix(), fTrack->GetRunFileValue()) ;
                    fROOT->InitRootFile() ;
                    printf("SRSPublisher::BookMonitorObjects()  ==> Get instance of SRSOutputROOT instance with \n")  ;
                }
                //=== Objects to be published
                ObjectsToBePublished() ;
            }
            
            //-------------------------------------------------------------------------------------------------
            void SRSPublisher::ResetMonitors(void) {
                if(fHMan) fHMan->ResetHistos() ;
            }
            
            //-------------------------------------------------------------------------------------------------
            void SRSPublisher::ObjectsToBePublished(void) {
                //=== Objects to be published
                if ( (fRunType == "PHYSICS") || (fRunType == "APVGAIN") || (fRunType == "RAWDATA") ) {
                    map<TString, TObject*> allObjectsToBePublished = fHMan->GetAllObjectsToBePublished() ;
                    Int_t size = allObjectsToBePublished.size() ;
                    if(size !=0) {
                        map<TString, TObject*>::const_iterator objectToBePublished_itr ;
                        for(objectToBePublished_itr = allObjectsToBePublished.begin(); objectToBePublished_itr != allObjectsToBePublished.end(); objectToBePublished_itr++) {
                            TNamed * obj = (TNamed*) (*objectToBePublished_itr).second ;
                            Publish(obj, obj->GetName());
                            if( (fRunType != "PHYSICS") && (fHMan->GetVarType(obj->GetName()) != "RAWDATA")) continue ;
                            if( (fRunType != "PHYSICS") && (fHMan->GetVarType(obj->GetName()) != "RAWDATAPLANE")) continue ;
                            
                            printf("SRSPublisher::ObjectsToBePublished()==> Publishing monitoring object %s\n", obj->GetName()) ;
                        }
                    }
                    else printf("SRSPublisher::ObjectsToBePublished()==> No object to be published\n") ;
                }
                
                if (fRunType == "TRACKING") {
                    map<TString, TH1*> toBePublished = fHMan->GetResidualHistoToBePublished() ;
                    Int_t size = toBePublished.size() ;
                    if(size !=0) {
                        map<TString, TH1*>::const_iterator toBePublished_itr ;
                        for(toBePublished_itr = toBePublished.begin(); toBePublished_itr != toBePublished.end(); toBePublished_itr++) {
                            TNamed * obj = (TNamed*) (*toBePublished_itr).second ;
                            Publish(obj, obj->GetName());
                            printf("SRSPublisher::ObjectsToBePublished()==> Publishing monitoring object %s\n", obj->GetName()) ;
                        }
                    }
                    if(fHMan->GetNtupleToBePublished()) {
                        TNamed * obj = (TNamed*) fHMan->GetNtupleToBePublished() ;
                        Publish(obj, obj->GetName()) ;
                        printf("SRSPublisher::ObjectsToBePublished()==> Publishing monitoring object %s\n", obj->GetName()) ;
                    }
                    else printf("SRSPublisher::ObjectsToBePublished()==> No object to be published\n") ;
                }
                
                if (fPlotEtaFunctionHistos) {
                    map<TString, TH1*> toBePublished = fHMan->GetPosCorrectionHistosToBePublished() ;
                    Int_t size = toBePublished.size() ;
                    if(size !=0) {
                        map<TString, TH1*>::const_iterator toBePublished_itr ;
                        for(toBePublished_itr = toBePublished.begin(); toBePublished_itr != toBePublished.end(); toBePublished_itr++) {
                            TNamed * obj = (TNamed*) (*toBePublished_itr).second ;
                            Publish(obj, obj->GetName());
                            printf("SRSPublisher::ObjectsToBePublished()==> Publishing monitoring object %s\n", obj->GetName()) ;
                        }
                    }
                }
            }
            
            //-------------------------------------------------------------------------------------------------
            void SRSPublisher::MonitorEvent(amore::core::Event& event) {
                
                if( (event.DATEEvent()->eventSize != 72) && (event.DATEEvent()->eventSize != 68) &&  (event.DATEEvent()->eventSize != 80) ) { // eventsize == 72 is an End of Run event
                    fEvent++ ;
                    
                    if (fEvent >  fStartEventNumber.Atoi() ) {
                        if ( (fEvent % fEventFrequencyNumber.Atoi()) == 0 ) {
                            
                            UInt_t * buffer = 0;
                            TDATEEventParser * parser = event.DATEEventParser();
                            parser->Parse();
                            
                            eventLdcIdType * firstLdcId = const_cast<eventLdcIdType*>(parser->FirstLDC());
                            eventLdcIdType * lastLdcId  = const_cast<eventLdcIdType*>(parser->LastLDC());
                            
                            SRSEventBuilder * eventBuilder = new SRSEventBuilder(fEvent, fSRSConf->GetMaxClusterSize(), fSRSConf->GetMinClusterSize(), fSRSConf->GetZeroSupCut(), fRunType, fIsClusterPosCorrection) ;
                            eventBuilder->SetHitMaxOrTotalADCs(fSRSConf->GetHitMaxOrTotalADCs());
                            eventBuilder->SetClusterMaxOrTotalADCs(fSRSConf->GetClusterMaxOrTotalADCs());
                            eventBuilder->SetMaxClusterMultiplicity(fSRSConf->GetMaxClusterMultiplicity());
                            
                            if (fRunType != "ETAFUNCTION") {
                                if (fIsClusterPosCorrection) {
                                    eventBuilder->SetClusterPositionCorrectionRootFile(fSRSConf->GetClusterPositionCorrectionFile()) ;
                                }
                            }
                            
                            for (eventLdcIdType * ldcId = firstLdcId; ldcId < lastLdcId; ldcId++) {
                                
                                const eventHeaderStruct * ldcHeader = parser->GetEventByLDCId(*ldcId);
                                
                                if (ldcHeader == 0) {
                                    std::cerr << "invalid LDC" << std::endl ;
                                    continue;
                                }
                                
                                const equipmentIdType * firstEqId = parser->FirstEquipment(*ldcId) ;
                                const equipmentIdType * lastEqId  = parser->LastEquipment(*ldcId) ;
                                
                                for (const equipmentIdType * eqId = firstEqId; eqId < lastEqId; eqId++) {
                                    const equipmentHeaderStruct * const equipmentHeader = parser->GetEquipmentById(*eqId) ;
                                    if (equipmentHeader) {
                                        buffer = (UInt_t*)equipmentHeader ;
                                        fEquipmentSize = equipmentHeader->equipmentSize ;
                                        
                                        if ((fRunType == "PEDESTAL") || (fRunType == "RAWPEDESTAL")) {
                                            SRSFECPedestalDecoder * pedestalDecoder = new SRSFECPedestalDecoder(fEquipmentSize, buffer) ;
                                            if (fRunType == "PEDESTAL")    fSavePedFromThisRun->FillPedestalHistos(pedestalDecoder, fLoadRawPedToUse) ;
                                            if (fRunType == "RAWPEDESTAL") fSaveRawPedFromThisRun->FillRawPedestalHistos(pedestalDecoder) ;
                                            delete pedestalDecoder ;
                                        }
                                        
                                        else {
                                            //Int_t sigmaCut = ((TString) fSRSConf->GetZeroSupCut()).Atoi() ;
                                            if (fRunType == "RAWDATA") {
                                                SRSFECEventDecoder * fecDecoder = new SRSFECEventDecoder(fEquipmentSize,buffer,eventBuilder) ;
                                                delete fecDecoder ;
                                            }
                                            else if (fRunType == "ETAFUNCTION") {
                                                SRSFECEventDecoder * fecDecoder = new SRSFECEventDecoder(fEquipmentSize,buffer,fLoadPedToUse,fSavePosCorrFromThisRun,sigmaCut) ;
                                                delete fecDecoder ;
                                            }
                                            else {
                                                
                                                // At the first event check if Pedestal file has to be uploaded
                                                // or thanks to APZ no ped file is required
                                                if (fEvent == 1) {
                                                    sigmaCut = LoadPedestalRootFileIfNecessary(buffer);
                                                }
                                                
                                                SRSFECEventDecoder * fecDecoder = new SRSFECEventDecoder(fEquipmentSize,buffer,fLoadPedToUse,eventBuilder,sigmaCut) ;
                                                delete fecDecoder ;
                                            }
                                            
                                        }
                                    }
                                }
                                
                                // RAWDATA RUN
                                if (fRunType == "RAWDATA") {
                                    fHMan->RefreshHistos() ;
                                    fHMan->FillRawDataHistos(eventBuilder) ;
                                }
                                
                                // APV GAIN RUN
                                if (fRunType == "APVGAIN") {
                                    eventBuilder->ComputeClustersInDetectorPlane() ;
                                    eventBuilder->SetTriggerList(fTrack->GetTriggerList()) ;
                                    fHMan->RefreshHistos() ;
                                    fHMan->FillAPV25GainHistos(eventBuilder) ;
                                }
                                
                                // PHYSICS RUN
                                if(fRunType == "PHYSICS")  {
                                    eventBuilder->ComputeClustersInDetectorPlane() ;
                                    eventBuilder->SetTriggerList(fTrack->GetTriggerList()) ;
                                    fHMan->RefreshHistos() ;
                                    fHMan->PhysicsRun(fTrack, eventBuilder, ldcHeader) ;
                                    if (fPlotEtaFunctionHistos) fHMan->FillClusterPositionCorrectionHistos(eventBuilder) ;
                                }
                                
                                // TRACKING
                                if (fRunType == "TRACKING") {
                                    eventBuilder->ComputeClustersInDetectorPlane() ;
                                    eventBuilder->SetTriggerList(fTrack->GetTriggerList()) ;
                                    fHMan->RefreshHistos() ;
                                    if (fTrack->IsAGoodTrack(eventBuilder)) {
                                        if((Int_t) (fHMan->GetResidualHistoToBePublished().size())) fHMan->FillResidualHistos(fTrack) ;
                                        if(fHMan->GetNtupleToBePublished()) fHMan->Event3DDisplay(fTrack) ;
                                    }
                                }
                                
                                // ROOT FILE RUN
                                if (fRunType == "ROOTFILE") {
                                    eventBuilder->ComputeClustersInDetectorPlane() ;
                                    eventBuilder->SetTriggerList(fTrack->GetTriggerList()) ;
                                    fROOT->FillRootFile(eventBuilder, fTrack) ;
                                }
                                
                                // CLUSTER POSITION CORRECTION FUNCTION
                                if (fRunType == "ETAFUNCTION") {
                                    fSavePosCorrFromThisRun->ComputeClustersInDetectorPlane() ;
                                    fSavePosCorrFromThisRun->FillClusterPositionCorrection() ;
                                }
                            }
                            // Delete the event decoder and event builder object class
                            delete eventBuilder ;
                        }
                    }
                }
            }
            
            //-------------------------------------------------------------------------------------------------
            string SRSPublisher::Version()  {
                return "$MAKEFILE_VERSION";
            }
            
            //-------------------------------------------------------------------------------------------------
            void SRSPublisher::LoadPedestalRootFile(const char * filename, Int_t nbOfAPVs) {
                printf("SRSPublisher::LoadPedestalRootFile() ==> load pedestal root file %s \n",filename) ;
                fLoadPedToUse = SRSPedestal::GetPedestalRootFile(filename) ;
                if(fLoadPedToUse) {
                    fLoadPedToUse->Init();
                    fLoadPedToUse->LoadPedestalData(filename) ;
                    printf("SRSPublisher::LoadPedestalRootFile() ==> Pedestal loaded \n")  ;
                }
                else {
                    printf("SRSPublisher::LoadPedestalRootFile() ==> cannot load pedestal root file %s \n",filename) ;
                }
            }
            
            Int_t SRSPublisher::LoadPedestalRootFileIfNecessary(UInt_t * buffer) {
                
                SRSMapping * mapping = SRSMapping::GetInstance();
                map <Int_t, Int_t> apvNoFromApvIDMap = mapping->GetAPVNoFromIDMap();
                
                Int_t size = apvNoFromApvIDMap.size() ;
                Int_t sigmaCut, executeonce = 0 ;

                Bool_t fIsNewPacket = kFALSE ;
                Int_t fPacketSize = 4000 ;
                
                vector <Int_t>  fActiveFecChannels ;
                map<Int_t, vector <Int_t> > fActiveFecChannelsMap ;
                
                
                fActiveFecChannelsMap.clear() ;
                map <Int_t, Int_t >::const_iterator adcChannel_itr ;
                for(adcChannel_itr = apvNoFromApvIDMap.begin(); adcChannel_itr != apvNoFromApvIDMap.end(); ++adcChannel_itr) {
                    Int_t apvid = (* adcChannel_itr).first ;
                    Int_t activeChannel = apvid & 0xF;
                    Int_t fecId = (apvid >> 4 ) & 0xF;
                    fActiveFecChannelsMap[fecId].push_back(activeChannel) ;
                    // printf("  SRSFECEventDecoder => List of  fecNo=%d, activeChannel = %d\n", fecId, activeChannel) ;
                }
                
                //==========================================================================//
                // Needed as the key to link apvID (or adcChannel) to the apvEvent in the TList  //
                // Should be < to 15 (max 16 APV channel in the FEC)                        //
                //==========================================================================//
                
                Int_t current_offset = 0, fecID = 0 ;
                Int_t adcChannel = 0, apvID = 0;
                UInt_t currentAPVPacketHdr;
                Int_t previousAPVPacketSize = 0 ;
                
                vector<UInt_t> data32BitsVector ;
                data32BitsVector.clear() ;
                
                //===============================================================================//
                // Dealing with the 7 Equipment header words. We just skip the first 2 words     //
                // and go straight to the 3rd word  where we extract the FEC no (Equip Id)       //
                //===============================================================================//
                current_offset += 2 ;
                UInt_t eqHeaderWord = buffer[current_offset] ;
                fecID = eqHeaderWord & 0xff ;
                fActiveFecChannels.clear();
                fActiveFecChannels = fActiveFecChannelsMap[fecID] ;
                
                //=== The next 4 words are Equip word, we dont care
                current_offset += 5 ;
                
                //================================================================================//
                // Start looking at the APV data word from here                                   //
                //================================================================================//
                while (current_offset < fEquipmentSize) {
                    
                    UInt_t rawdata = buffer[current_offset] ;
                    
                    if (rawdata == 0xfafafafa) {
                        //===================================================================================================//
                        // last word of the previous packet added for Filippo in DATE to count the eventNb x 16 UDP packets  //
                        // We dont need it here, will just skip it We remove it from the vector of data                      //
                        //===================================================================================================//
                        if(!data32BitsVector.empty()) {
                            apvID = (fecID << 4) | adcChannel ;
                            Int_t datasize = data32BitsVector.size() ;
                        }
                        
                        adcChannel = 0 ;
                        data32BitsVector.clear() ;
                        current_offset++ ;
                        break ;
                    }
                    
                    //==========================================================================================//
                    // Word with the event number (trigger count) and the packet size information               //
                    //                                     size of APV packet                                    //
                    //==========================================================================================//
                    if (fIsNewPacket) {
                        fPacketSize = (rawdata & 0xffff) ;
                        data32BitsVector.clear() ;
                        fIsNewPacket = kFALSE ;
                        current_offset++ ;
                        continue ;
                    }
                    
                    
                    //=========================================================================================================//
                    //         New packet (or frame) FEC channel data in the equipment                                         //
                    //=========================================================================================================//
                    if ( ( ( (rawdata >> 8) & 0xffffff) == 0x414443) && (executeonce == 0) ) {
                        executeonce = 1;
                        cout << endl << " The rawfile being processed is NOT zero suppressed! " << endl;
                        cout << " Please consider ZS for better performance! " << endl << endl;

                        data32BitsVector.pop_back() ;
                        if(!data32BitsVector.empty()) {
                            apvID = (fecID << 4) | adcChannel ;
                        }
                        
                        currentAPVPacketHdr = rawdata  ;
                        adcChannel = currentAPVPacketHdr & 0xff ;
                        
                        //=== REINITIALISE EVERYTHING
                        if(adcChannel > 15) {
                            printf("  SRSFECEventDecoder => ERROR #### fecID=%d, ADC Channel=%d, apvID=%d, \n",fecID, adcChannel, apvID) ;
                            break ;
                        }
                        
                        data32BitsVector.clear() ;
                        fIsNewPacket = kTRUE ;
                        
                        current_offset++ ;
                        LoadPedestalRootFile(fSRSConf->GetPedestalFile(), fMapping->GetNbOfAPVs()) ;
                        sigmaCut = ((TString) fSRSConf->GetZeroSupCut()).Atoi() ;
                        continue ;
                    }

                    if ( ( ( (rawdata >> 8) & 0xffffff) == 0x41505a) && (executeonce == 0) ) {
                        executeonce = 1;
                        cout << endl << " The rawfile being processed is Zero suppressed! " << endl << endl;
                        
                        data32BitsVector.pop_back() ;
                        if(!data32BitsVector.empty()) {
                            apvID = (fecID << 4) | adcChannel ;
                        }
                        
                        currentAPVPacketHdr = rawdata  ;
                        adcChannel = currentAPVPacketHdr & 0xff ;
                        
                        //=== REINITIALISE EVERYTHING
                        if(adcChannel > 15) {
                            printf("  SRSFECEventDecoder => ERROR #### fecID=%d, ADC Channel=%d, apvID=%d, \n",fecID, adcChannel, apvID) ;
                            break ;
                        }
                        
                        data32BitsVector.clear() ;
                        fIsNewPacket = kTRUE ;
                        
                        current_offset++ ;
                        sigmaCut=-999;
                        continue ;
                    }


                    //=========================================================================================================//
                    //         apv data in the packet (frame)                                                                 //
                    //========================================================================================================//
                    if (!fIsNewPacket) {
                        data32BitsVector.push_back(rawdata) ;
                        current_offset++ ;
                        continue ;
                    }
                }
                return sigmaCut;
            }
            
            void SRSPublisher::LoadRawPedestalRootFile(const char * filename, Int_t nbOfAPVs) {
                fLoadRawPedToUse = SRSRawPedestal::GetRawPedestalRootFile(filename);
                if(fLoadRawPedToUse) {
                    fLoadRawPedToUse->Init(nbOfAPVs);
                    fLoadRawPedToUse->LoadRawPedestalData(filename) ;
                    printf("SRSPublisher::LoadRawPedestalRootFile() ==> raw pedestal loaded \n")  ;
                }
                else {
                    printf("SRSPublisher::LoadPedestalRootFile() ==> cannot load raw pedestal root file %s \n",filename) ;
                }
            }
            
            //-------------------------------------------------------------------------------------------------
            void SRSPublisher::LoadAPVGainCalibrationRootFile(const char * filename, Int_t nbOfAPVs) {
                //	printf("SRSPublisher::LoadAPVGainCalibrationRootFile() ==> load  apv relative gain root file %s \n",filename) ;
                for(Int_t i = 0; i < nbOfAPVs; i++) fApvGainFromApvNoMap[i] = 1.0 ;
                
                /**
                 TFile * file = new TFile(filename,"read") ;
                 if (!file->IsOpen()){
                 //	  printf("SRSPublisher::LoadAPVGainCalibrationRootFile() **** ERROR ERROR *** Cannot open file %s", filename);
                 for(Int_t i = 0; i < nbOfAPVs; i++) fApvGainFromApvNoMap[i] = 1.0 ;
                 }
                 else {
                 TString allAPVGainHistoName = "apv25GainDir/allAPVGain" ;
                 TH1F * allAPVGainHisto   = (TH1F *) file->Get(allAPVGainHistoName) ;
                 if (!allAPVGainHisto) {
                 //	    printf("SRSPublisher::LoadAPVGainCalibrationRootFile() **** ERROR ERROR *** Cannot open apv Gain histo = %s  \n", allAPVGainHistoName.Data()) ;
                 for(Int_t i = 0; i < nbOfAPVs; i++) fApvGainFromApvNoMap[i] = 1.0 ;
                 }
                 else {
                 for(Int_t i = 0; i < nbOfAPVs; i++) {
                 Int_t binNumber = i + 1 ;
                 Float_t apvGain = allAPVGainHisto->GetBinContent(binNumber) ;
                 printf("SRSPublisher::LoadAPVGainCalibrationRootFile() ==> apvNo = %d, apvGain = %f  \n",i,apvGain) ;
                 fApvGainFromApvNoMap[i] = apvGain ;
                 }
                 }
                 }
                 file->Close() ;
                 */
            }
            
        }// SRSPublisher
    }
}
