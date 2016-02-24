#ifndef __SRSCONFIGURATION__
#define __SRSCONFIGURATION__
/*******************************************************************************
*  AMORE FOR SRS - SRS                                                         *
*  SRSConfiguration                                                            *
*  SRS Module Class                                                            *
*  Author: Kondo GNANVO 18/08/2010                                             *
*******************************************************************************/

#if !defined(__CINT__) || defined(__MAKECINT__)

#include <fstream>
#include <iostream>
#include "TObject.h"
#include "TH1F.h"
#include "TSystem.h"
#endif

// Class containing all configuration parameters for configuration from file(s)
using namespace std;

class SRSConfiguration : public TObject {

 public:
  SRSConfiguration();
  SRSConfiguration(const char * file);
  ~SRSConfiguration();

  void Init(const char * file = 0);
  bool FileExists(const char* name) const;

  const char * GetCycleWait() const {return gSystem->ExpandPathName(fCycleWait.Data());}
  void  SetCycleWait(const char * name) {fCycleWait = name;}

  const char * GetRunType() const {return gSystem->ExpandPathName(fRunType.Data());}
  void  SetRunType(const char * name) {fRunType = name;}

  const char * GetRunName() const {return gSystem->ExpandPathName(fRunName.Data());}
  void  SetRunName(const char * name) {fRunName = name;}

  const char * GetZeroSupCut() const {return gSystem->ExpandPathName(fZeroSupCut.Data());}
  void  SetZeroSupCut(const char * name) {fZeroSupCut = name;}

  const char * GetMaskedChannelCut() const {return gSystem->ExpandPathName(fMaskedChannelCut.Data());}
  void  SetMaskedChannelCut(const char * name) {fMaskedChannelCut = name;}
    
  const char * GetStartEventNumber() const {return gSystem->ExpandPathName(fStartEventNumber.Data());}
  void  SetStartEventNumber(const char * name) {fStartEventNumber = name;}
 
  const char * GetEventFrequencyNumber() const {return gSystem->ExpandPathName(fEventFrequencyNumber.Data());}
  void  SetEventFrequencyNumber(const char * name) {fEventFrequencyNumber = name;}
  
  const char * GetMinClusterSize() const {return gSystem->ExpandPathName(fMinClusterSize.Data());}
  void  SetMinClusterSize(const char * name) {fMinClusterSize = name;}

  const char * GetMaxClusterSize() const {return gSystem->ExpandPathName(fMaxClusterSize.Data());}
  void  SetMaxClusterSize(const char * name) {fMaxClusterSize = name;}

  const char * GetMaxClusterMultiplicity() const {return gSystem->ExpandPathName(fMaxClusterMultiplicity.Data());}
  void  SetMaxClusterMultiplicity(const char * name) {fMaxClusterMultiplicity = name;}

  const char * GetMappingFile() const {return gSystem->ExpandPathName(fMappingFile.Data());}
  void  SetMappingFile(const char * name) {fMappingFile = name;}

  const char * GetPadMappingFile() const {return gSystem->ExpandPathName(fPadMappingFile.Data());}
  void  SetPadMappingFile(const char * name) {fPadMappingFile = name;}

  const char * GetSavedMappingFile() const {return gSystem->ExpandPathName(fSavedMappingFile.Data());}
  void  SetSavedMappingFile(const char * name) {fSavedMappingFile = name;}

  const char * GetRunNbFile() const {return gSystem->ExpandPathName(fRunNbFile.Data());}
  void  SetRunNbFile(const char * name) {fRunNbFile = name;}

  const char * GetHistoCfgName() const {return gSystem->ExpandPathName(fHistosFile.Data());}
  void  SetHistoCfgName(const char * name) {fHistosFile = name;}
 
  const char * GetDisplayCfgName() const {return gSystem->ExpandPathName(fDisplayFile.Data());}
  void  SetDisplayCfgName(const char * name) {fDisplayFile = name;}

  const char * GetROOTDataType() const {return gSystem->ExpandPathName(fROOTDataType.Data());}
  void  SetROOTDataType(const char * name) {fROOTDataType = name;}

  const char * GetPedestalFile() const {return gSystem->ExpandPathName(fPedestalFile.Data());}
  void  SetPedestalFile(const char * name) {fPedestalFile = name;}

  const char * GetTrackingOffsetDir() const {return gSystem->ExpandPathName(fTrackingOffsetDir.Data());}
  void  SetTrackingOffsetDir(const char * name) {fTrackingOffsetDir = name;}

  const char * GetRawPedestalFile() const {return gSystem->ExpandPathName(fRawPedestalFile.Data());}
  void  SetRawPedestalFile(const char * name) {fRawPedestalFile = name;}

  const char * GetClusterPositionCorrectionFile() const {return gSystem->ExpandPathName(fPositionCorrectionFile.Data());}
  void  setClusterPositionCorrectionFile(const char * name) {fPositionCorrectionFile = name;}

  const char * GetClusterPositionCorrectionFlag() const {return gSystem->ExpandPathName(fPositionCorrectionFlag.Data());}
  void  setClusterPositionCorrectionFlag(const char * name) {fPositionCorrectionFlag = name;}

  const char * GetAPVGainCalibrationFile() const {return gSystem->ExpandPathName(fAPVGainCalibrationFile.Data());}
  void  SetAPVGainCalibrationFile(const char * name) {fAPVGainCalibrationFile = name;}

  const char * GetClusterMaxOrTotalADCs() const {return gSystem->ExpandPathName(fIsClusterMaxOrTotalADCs.Data());};
  void  SetClusterMaxOrTotalADCs(const char * name) {fIsClusterMaxOrTotalADCs  = name;}

  const char * GetHitMaxOrTotalADCs() const {return gSystem->ExpandPathName(fIsHitMaxOrTotalADCs.Data());};
  void  SetHitMaxOrTotalADCs(const char * name) {fIsHitMaxOrTotalADCs  = name;}

  Bool_t Load(const char * filename);
  void Save(const char * filename) const; 
  void Dump() const;

  SRSConfiguration & operator=(const SRSConfiguration &rhs);

 private:
  void BookArrays();
  void SetDefaults();

  TString fMappingFile, fPadMappingFile, fSavedMappingFile, fRunNbFile, fRunName, fRunType, fROOTDataType, fCycleWait, fZeroSupCut, fMaskedChannelCut, fHistosFile, fTrackingOffsetDir;
  TString fDisplayFile, fPositionCorrectionFile, fPositionCorrectionFlag, fPedestalFile, fRawPedestalFile, fAPVGainCalibrationFile;
  TString fMaxClusterSize, fMinClusterSize, fStartEventNumber, fMaxClusterMultiplicity, fIsHitMaxOrTotalADCs, fIsClusterMaxOrTotalADCs, fEventFrequencyNumber;

  ClassDef(SRSConfiguration, 1)
};

#endif
