#ifndef AMORE_SRS_PUBLISHERSRSPUBLISHER_H
#define AMORE_SRS_PUBLISHERSRSPUBLISHER_H

/*******************************************************************************
 *  AMORE FOR SRS - SRS                                                         *
 *  SRSPublisher                                                                *
 *  SRS Module Class                                                            *
 *  Author: Kondo GNANVO 18/08/2010                                             *
 *  Modified by Stefano Colafranceschi on 22/08/2015 to include ZS rawfiles     *
 *******************************************************************************/
#include <map>
#include <list>
#include <fstream>
#include <iostream>

#include <MonitorObject.h>
#include <PublisherModule.h>

#include <Event.h>
#include <TDATEEventParser.h>

#include "TFile.h"

#include "SRSTrack.h"
#include "SRSCommon.h"
#include "SRSMapping.h"
#include "SRSPedestal.h"
#include "SRSAlignment.h"
#include "SRSOutputROOT.h"
#include "SRSRawPedestal.h"
#include "SRSHistoManager.h"
#include "SRSConfiguration.h"
#include "SRSFECEventDecoder.h"
#include "SRSFECPedestalDecoder.h"
#include "SRSPositionCorrection.h"

//class amore::core::Event;
using namespace std;

namespace amore {
    
    namespace SRS {
        
        namespace publisher {
            
            class SRSPublisher: public amore::publisher::PublisherModule, public amore::SRS::common::SRSCommon {
                
            public:
                
                SRSPublisher();
                ~SRSPublisher();
                
                virtual void BookMonitorObjects();
                virtual void EndOfCycle();
                virtual void StartOfCycle();
                virtual void MonitorEvent(amore::core::Event& event);
                virtual void EndOfRun(const amore::core::Run& run);
                virtual void StartOfRun(const amore::core::Run& run);
                virtual void EndOfSession(const amore::core::Session& session){};
                virtual void StartOfSession(const amore::core::Session& session){};
                
                virtual std::string Version(); // the version of the module
                
                void ResetMonitors(void) ;
                void ObjectsToBePublished(void) ;
                
                void LoadPedestalRootFile(const char * filename, Int_t nbOfAPVs) ;
                Int_t LoadPedestalRootFileIfNecessary(UInt_t * buffer) ;
                void LoadRawPedestalRootFile(const char * filename, Int_t nbOfAPVs) ;
                void LoadAPVGainCalibrationRootFile(const char * filename, Int_t nbOfAPVs) ;
                //     	void LoadClusterPositionCorrectionRootFile(const char * filename) ;
                
            private:
                
                Int_t fEvent;
                Int_t fEventType;
                Int_t fEventRunNb;
                Int_t fEquipmentSize;
                Int_t sigmaCut;
                
                TFile * fPedRootFile ;
                TFile * fRawPedRootFile ;
                TFile * fClusterPositionCorrectionRootFile ;
                
                //	TString fRunType, fCFGFile, fCycleWait, fPocaDataTextFile, fIsClusterMaxOrSumADCs, fStartEventNumber, fEventFrequencyNumber ;
                TString fRunType, fCFGFile, fCycleWait, fStartEventNumber, fEventFrequencyNumber, fAmoreAgentID;
                
                SRSTrack         * fTrack;
                SRSHistoManager  * fHMan;
                SRSConfiguration * fSRSConf;
                SRSAlignment     * fAlignment;
                SRSOutputROOT    * fROOT ;
                
                SRSPedestal      * fSavePedFromThisRun;
                SRSPedestal      * fLoadPedToUse;
                
                SRSRawPedestal   * fSaveRawPedFromThisRun;
                SRSRawPedestal   * fLoadRawPedToUse;
                
                SRSPositionCorrection  * fSavePosCorrFromThisRun; 
                SRSPositionCorrection  * fLoadPositionCorrectionToUse; 
                
                SRSMapping       * fMapping ;
                
                Int_t fMinNbOfEquipments ;
                Int_t fMaxNbOfEquipments ;
                
                Bool_t fIsClusterPosCorrection, fPlotEtaFunctionHistos;
                ;
                map <Int_t, Int_t > fApvNoFromApvIDMap ;
                map <Int_t, Float_t > fApvGainFromApvNoMap ;
                map <Int_t, TString > fApvNameFromIDMap ;
                TH1F ** fEtaFunctionHistos ;       
                
                ClassDef(SRSPublisher, 1); // SRS Module Base Class
            };
        };
    };
};

#endif
