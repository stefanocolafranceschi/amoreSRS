#include "SRSConfiguration.h"
ClassImp(SRSConfiguration);

SRSConfiguration::SRSConfiguration() {
  Init();
}
SRSConfiguration::SRSConfiguration(const char * file) {
  Init(file);
}
SRSConfiguration::~SRSConfiguration(){
}

bool SRSConfiguration::FileExists(const char* name) const {
ifstream f(gSystem->ExpandPathName(name));
	if (f.good()) {
		f.close();
		return true;
	} else {
		f.close();
		return false;
	}
}


//============================================================================================
SRSConfiguration & SRSConfiguration::operator=(const SRSConfiguration &rhs) {
  fCycleWait               = rhs.GetCycleWait() ;
  fRunType                 = rhs.GetRunType() ;
  fRunName                 = rhs.GetRunName() ;
  fZeroSupCut              = rhs.GetZeroSupCut() ;
  fROOTDataType            = rhs.GetROOTDataType();
  fRunNbFile               = rhs.GetRunNbFile();
  fMappingFile             = rhs.GetMappingFile();
  fPadMappingFile          = rhs.GetPadMappingFile();
  fSavedMappingFile        = rhs.GetSavedMappingFile();
  fHistosFile              = rhs.GetHistoCfgName();
  fDisplayFile             = rhs.GetDisplayCfgName();
  fPedestalFile            = rhs.GetPedestalFile() ;
  fTrackingOffsetDir       = rhs.GetTrackingOffsetDir();
  fPositionCorrectionFile  = rhs.GetClusterPositionCorrectionFile() ;
  fPositionCorrectionFlag  = rhs.GetClusterPositionCorrectionFlag() ;
  fMaskedChannelCut        = rhs.GetMaskedChannelCut() ;
  fStartEventNumber        = rhs.GetStartEventNumber() ;
  fEventFrequencyNumber    = rhs.GetEventFrequencyNumber() ;
  fRawPedestalFile         = rhs.GetRawPedestalFile() ;
  fMinClusterSize          = rhs.GetMinClusterSize() ;
  fMaxClusterSize          = rhs.GetMaxClusterSize() ;
  fMaxClusterMultiplicity  = rhs.GetMaxClusterMultiplicity() ;
  fIsHitMaxOrTotalADCs     = rhs.GetHitMaxOrTotalADCs() ;
  fIsClusterMaxOrTotalADCs = rhs.GetClusterMaxOrTotalADCs() ;
  fAPVGainCalibrationFile  = rhs.GetAPVGainCalibrationFile() ;
  return *this;
}

//============================================================================================
void SRSConfiguration::Init(const char * file) {
  if(!file) {
    Warning("Init", "conf file not specified. Setting defaults." );
    SetDefaults();
  }
  else {
    if(!Load(file)) {
      Warning("Init", "Cannot open conf file. Setting defaults." );
      SetDefaults();
    }
  }
}

//============================================================================================
void SRSConfiguration::SetDefaults() {
  fRunType                 = "RAWDATA" ;
  fRunName                 = "data" ;
  fCycleWait               = "1" ;
  fZeroSupCut              = "3" ;
  fMaskedChannelCut        = "20" ;
  fMaxClusterSize          = "20" ;
  fMinClusterSize          = "0" ;
  fMappingFile             = "../../configFileDir/mapping.cfg";
  fPadMappingFile          = "../../configFileDir/padMapping.cfg";
  fSavedMappingFile        = "../../results/savedMapping.cfg";
  fRunNbFile               = "../../configFileDir/runNb.cfg";
  fHistosFile              = "../../configFileDir/histogram.cfg";
  fDisplayFile             = "../../configFileDir/display.cfg" ;
  fPedestalFile            = "../../pedestalDir/pedestal.root";
  fRawPedestalFile         = "../../pedestalDir/rawpedestal.root";
  fPositionCorrectionFile  = "../../pedestalDir/clusterPositionCorrection.root";
  fPositionCorrectionFlag  = "NO" ;
  fTrackingOffsetDir      = "../../offsetDir/";
  fROOTDataType            = "HITS_ONLY" ;
  fAPVGainCalibrationFile  = "" ;
  fMaxClusterMultiplicity  = "1" ;
  fIsHitMaxOrTotalADCs     = "signalPeak" ;
  fIsClusterMaxOrTotalADCs = "TotalCharges" ;
  fStartEventNumber        = "0" ;
  fEventFrequencyNumber    = "1" ;
}

//============================================================================================
void SRSConfiguration::Save(const char * filename) const {
#ifdef DEBUG
  cout << "saving conf in " << gSystem->ExpandPathName(filename) << endl;
#endif
  ofstream file ( gSystem->ExpandPathName(filename));
  file << "RUNNAME "              << fRunName                << endl;
  file << "RUNTYPE "              << fRunType                << endl;
  file << "CYCLEWAIT "            << fCycleWait              << endl;
  file << "ZEROSUPCUT "           << fZeroSupCut             << endl;
  file << "CHMASKCUT "            << fMaskedChannelCut       << endl;
  file << "MINCLUSTSIZE "         << fMinClusterSize         << endl;
  file << "MAXCLUSTSIZE "         << fMaxClusterSize         << endl;
  file << "MAXCLUSTMULT "         << fMaxClusterMultiplicity << endl;
  file << "MAPFILE "              << fMappingFile            << endl;
  file << "PADMAPFILE "           << fPadMappingFile         << endl;
  file << "SAVEDMAPFILE "         << fSavedMappingFile       << endl;
  file << "RUNNBFILE "            << fRunNbFile              << endl;
  file << "HISTCFG "              << fHistosFile             << endl;
  file << "DISPCFG "              << fDisplayFile            << endl;
  file << "OFFSETDIR "            << fTrackingOffsetDir      << endl;
  file << "ROOTDATATYPE "         << fROOTDataType           << endl;
  file << "PEDFILE "              << fPedestalFile           << endl;
  file << "RAWPEDFILE "           << fRawPedestalFile        << endl;
  file << "CLUSTERPOSCORRFILE "   << fPositionCorrectionFile << endl;
  file << "CLUSTERPOSCORRFLAG "   << fPositionCorrectionFlag << endl;
  file << "STARTEVENTNUMBER "     << fStartEventNumber       << endl;
  file << "EVENTFREQUENCYNUMBER " << fEventFrequencyNumber   << endl;
  file << "APVGAINCALIB "         << fAPVGainCalibrationFile << endl;
  file << "HIT_ADCS "             << fIsHitMaxOrTotalADCs << endl;
  file << "CLUSTER_ADCS "         << fIsClusterMaxOrTotalADCs << endl;
  file.close();
}

//============================================================================================
Bool_t SRSConfiguration::Load(const char * filename) {
  printf("  SRSConfiguration::Load() ==> Loading cfg from %s\n", gSystem->ExpandPathName(filename)) ;
  //  ifstream file (gSystem->ExpandPathName(filename), ifstream::in);
  //  if(!file.is_open()) return kFALSE;

	ifstream file;
	if (FileExists(gSystem->ExpandPathName(filename))) {
		try {
			file.open(gSystem->ExpandPathName(filename), ifstream::in);
		} catch (ifstream::failure e) {
			std::cerr << gSystem->ExpandPathName(filename)
					<< ": File does not exist or cannot be opened!\n";
		} catch (...) {
			std::cerr << "Non-processed exception!\n";
		}

		if (!file.is_open()) {
			return kFALSE;
		}
	} else {
		return kFALSE;
	}

  TString line;
  while (line.ReadLine(file)) {

    // strip leading spaces and skip comments
    line.Remove(TString::kLeading, ' ');
    if(line.BeginsWith("#")) continue;        

    if(line.BeginsWith("RUNTYPE")) {
      char hfile[1000];
      sscanf(line.Data(), "RUNTYPE %s", hfile);
      fRunType = hfile;
    }
    if(line.BeginsWith("RUNNAME")) {
      char hfile[1000];
      sscanf(line.Data(), "RUNNAME %s", hfile);
      fRunName = hfile;
    }
    if(line.BeginsWith("ZEROSUPCUT")) {
      char hfile[10];
      sscanf(line.Data(), "ZEROSUPCUT %s", hfile);
      fZeroSupCut = hfile;
    }
    if(line.BeginsWith("MAXCLUSTSIZE")) {
      char hfile[10];
      sscanf(line.Data(), "MAXCLUSTSIZE %s", hfile);
      fMaxClusterSize = hfile;
    }
    if(line.BeginsWith("MINCLUSTSIZE")) {
      char hfile[10];
      sscanf(line.Data(), "MINCLUSTSIZE %s", hfile);
      fMinClusterSize = hfile;
    }
    if(line.BeginsWith("MAXCLUSTMULT")) {
      char hfile[100];
      sscanf(line.Data(), "MAXCLUSTMULT %s", hfile);
      fMaxClusterMultiplicity = hfile;
    }
    if(line.BeginsWith("CYCLEWAIT")) {
      char hfile[100];
      sscanf(line.Data(), "CYCLEWAIT %s", hfile);
      fCycleWait = hfile;
    }
    if(line.BeginsWith("STARTEVENTNUMBER")) {
      char hfile[100];
      sscanf(line.Data(), "STARTEVENTNUMBER %s", hfile);
      fStartEventNumber = hfile;
    }
    if(line.BeginsWith("EVENTFREQUENCYNUMBER")) {
      char hfile[1000];
      sscanf(line.Data(), "EVENTFREQUENCYNUMBER %s", hfile);
      fEventFrequencyNumber = hfile;
    }
    if(line.BeginsWith("HISTCFG")) {
      char hfile[1000];
      sscanf(line.Data(), "HISTCFG %s", hfile);
      fHistosFile = hfile;
    }
    if(line.BeginsWith("PEDFILE")) {  
      char pedfile[1000];  
      sscanf(line.Data(), "PEDFILE %s", pedfile);  
      fPedestalFile = pedfile; 
    }
    if(line.BeginsWith("RAWPEDFILE")) {  
      char rawpedfile[1000];  
      sscanf(line.Data(), "RAWPEDFILE %s", rawpedfile);  
      fRawPedestalFile = rawpedfile; 
    }
    if(line.BeginsWith("OFFSETDIR")) {  
      char offsetfile[1000];  
      sscanf(line.Data(), "OFFSETDIR %s", offsetfile);  
      fTrackingOffsetDir = offsetfile; 
    }
    if(line.BeginsWith("CLUSTERPOSCORRFILE")) {  
      char posCorrectfile[1000];  
      sscanf(line.Data(), "CLUSTERPOSCORRFILE %s", posCorrectfile);  
      fPositionCorrectionFile = posCorrectfile; 
    }
    if(line.BeginsWith("CLUSTERPOSCORRFLAG")) {  
      char posCorrectflag[10];  
      sscanf(line.Data(), "CLUSTERPOSCORRFLAG %s", posCorrectflag);  
      fPositionCorrectionFlag = posCorrectflag; 
    }
    if(line.BeginsWith("APVGAINCALIB")) {  
      char apvGainCalibrationFile[1000];  
      sscanf(line.Data(), "APVGAINCALIB %s", apvGainCalibrationFile);  
      fAPVGainCalibrationFile = apvGainCalibrationFile; 
    }
    if(line.BeginsWith("MAPFILE")) {
      char mapfile[1000];
      sscanf(line.Data(), "MAPFILE %s", mapfile);
      fMappingFile = mapfile;
    }
    if(line.BeginsWith("PADMAPFILE")) {
      char padmapfile[1000];
      sscanf(line.Data(), "PADMAPFILE %s", padmapfile);
      fPadMappingFile = padmapfile;
    }
    if(line.BeginsWith("SAVEDMAPFILE")) {
      char savedmapfile[1000];
      sscanf(line.Data(), "SAVEDMAPFILE %s", savedmapfile);
      fSavedMappingFile = savedmapfile;
    }
    if(line.BeginsWith("RUNNBFILE")) {
      char runNbfile[1000];
      sscanf(line.Data(), "RUNNBFILE %s", runNbfile);
      fRunNbFile = runNbfile;
    }
    if(line.BeginsWith("DISPCFG")) {
      char displayfile[1000];
      sscanf(line.Data(), "DISPCFG %s", displayfile);
      fDisplayFile = displayfile;
    }
    if(line.BeginsWith("CHMASKCUT")) {
      char maskchannels[1000];
      sscanf(line.Data(), "CHMASKCUT %s", maskchannels);
      fMaskedChannelCut = maskchannels;
    }
    if(line.BeginsWith("ROOTDATATYPE")) {
      char rootdatatype[1000];
      sscanf(line.Data(), "ROOTDATATYPE %s", rootdatatype);
      fROOTDataType = rootdatatype;
    }
    if(line.BeginsWith("CLUSTER_ADCS")) {
      char isClusterMaxOrTotalADCs[1000];
      sscanf(line.Data(), " CLUSTER_ADCS%s", isClusterMaxOrTotalADCs);
      fIsClusterMaxOrTotalADCs = isClusterMaxOrTotalADCs ;
    }
    if(line.BeginsWith("HIT_ADCS")) {
      char isHitMaxOrTotalADCs[1000];
      sscanf(line.Data(), " HIT_ADCS%s", isHitMaxOrTotalADCs);
      fIsHitMaxOrTotalADCs = isHitMaxOrTotalADCs ;
    }
  }
  Dump();
  return kTRUE;
}

void SRSConfiguration::Dump() const {
  printf("  SRSConfiguration::Load() ==> RUNTYPE               %s\n", fRunType.Data()) ;
  printf("  SRSConfiguration::Load() ==> RUNNAME               %s\n", fRunName.Data()) ;
  printf("  SRSConfiguration::Load() ==> CYCLEWAIT             %s\n", fCycleWait.Data()) ;
  printf("  SRSConfiguration::Load() ==> ZEROSUPCUT            %s\n", fZeroSupCut.Data()) ;
  printf("  SRSConfiguration::Load() ==> MINCLUSTSIZE          %s\n", fMinClusterSize.Data()) ;
  printf("  SRSConfiguration::Load() ==> MAXCLUSTSIZE          %s\n", fMaxClusterSize.Data()) ;
  printf("  SRSConfiguration::Load() ==> MAXCLUSTMULT          %s\n", fMaxClusterMultiplicity.Data()) ;
  printf("  SRSConfiguration::Load() ==> HISTCFG               %s\n", fHistosFile.Data()) ;
  printf("  SRSConfiguration::Load() ==> DISPCFG               %s\n", fDisplayFile.Data()) ;
  printf("  SRSConfiguration::Load() ==> MAPFILE               %s\n", fMappingFile.Data()) ;
  printf("  SRSConfiguration::Load() ==> PADMAPFILE            %s\n", fPadMappingFile.Data()) ;
  printf("  SRSConfiguration::Load() ==> SAVEDMAPFILE          %s\n", fSavedMappingFile.Data()) ;
  printf("  SRSConfiguration::Load() ==> PEDFILE               %s\n", fPedestalFile.Data()) ;
  printf("  SRSConfiguration::Load() ==> RAWPEDFILE            %s\n", fRawPedestalFile.Data()) ;
  printf("  SRSConfiguration::Load() ==> CLUSTERPOSCORRFILE    %s\n", fPositionCorrectionFile.Data()) ;
  printf("  SRSConfiguration::Load() ==> CLUSTERPOSCORRFLAG    %s\n", fPositionCorrectionFlag.Data()) ;
  printf("  SRSConfiguration::Load() ==> HIT_ADCS              %s\n", fIsHitMaxOrTotalADCs.Data()) ;
  printf("  SRSConfiguration::Load() ==> CLUSTER_ADCS          %s\n", fIsClusterMaxOrTotalADCs.Data()) ;
  printf("  SRSConfiguration::Load() ==> STARTEVENTNUMBER      %s\n", fStartEventNumber.Data()) ;
  printf("  SRSConfiguration::Load() ==> EVENTFREQUENCYNUMBER  %s\n", fEventFrequencyNumber.Data()) ;
  printf("  SRSConfiguration::Load() ==> APVGAINCALIB          %s\n", fAPVGainCalibrationFile.Data()) ;
  printf("  SRSConfiguration::Load() ==> ROOTDATATYPE          %s\n", fROOTDataType.Data()) ;
  printf("  SRSConfiguration::Load() ==> OFFSETDIR             %s\n", fTrackingOffsetDir.Data()) ;
}
