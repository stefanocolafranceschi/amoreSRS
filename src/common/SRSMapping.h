// Mapping class is implemented as a singleton

#ifndef __SRSMAPPING__
#define __SRSMAPPING__
/*******************************************************************************
*  AMORE FOR SRS - SRS                                                         *
*  SRSMapping                                                                  *
*  SRS Module Class                                                            *
*  Author: Kondo GNANVO 18/08/2010                                             *
*******************************************************************************/

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <sstream>
#include <map>
#include <list>
#include <vector>
#include <fstream>
#include <iostream>
#include "TList.h"
#include "TObject.h"
#include "TString.h"
#include "TSystem.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TString.h"
#endif

using namespace std;

class SRSMapping : public TObject {

public:

  ~SRSMapping(){
    Clear() ;
    ClearMapOfList(fDetectorListFromDetectorTypeMap) ;
    ClearMapOfList(fDetectorListFromReadoutBoardMap) ;
    ClearMapOfList(fDetectorPlaneListFromDetectorMap) ;
    ClearMapOfList(fFECIDListFromDetectorPlaneMap) ;
    ClearMapOfList(fAPVIDListFromFECIDMap) ;
    ClearMapOfList(fAPVIDListFromDetectorPlaneMap) ;
    ClearMapOfList(fAPVIDListFromDetectorMap) ;
    ClearMapOfList(fAPVToPadChannelMap) ;
    ClearMapOfList(fPadDetectorMap) ;
    ClearMapOfList(f1DStripsPlaneMap) ;
    ClearMapOfList(fCartesianPlaneMap) ;
    ClearMapOfList(fCMSGEMDetectorMap) ;
    ClearMapOfList(fUVangleReadoutMap);
  }

  template <typename M> void ClearMapOfList( M & amap ) ;

  static SRSMapping * GetInstance() {
    if (instance == 0) instance = new SRSMapping() ;
    return instance;
  }

  void PrintMapping();
  void SaveMapping(const char * mappingCfgFilename);

  void LoadDefaultMapping(const char * mappingCfgFilename);
  void LoadAPVtoPadMapping(const char * mappingCfgFilename);

  void SetAPVMap(TString detPlane, Int_t fecId, Int_t adcCh, Int_t apvNo, Int_t apvOrient, Int_t apvIndex, Int_t apvHdrLevel, Int_t stripmapping);
  void SetAPVtoPadMapping(Int_t fecId, Int_t adcCh, Int_t padId, Int_t apvCh) ;

  void Set1DStripsReadoutMap(TString readoutBoard, TString detType, TString detName, Int_t detID, TString plane,  Float_t size, Int_t connectors, Int_t orient) ;
  void SetCartesianStripsReadoutMap(TString readoutBoard, TString detectorType, TString detector, Int_t detID, TString planeX,  Float_t sizeX, Int_t connectorsX, Int_t orientX, TString planeY,  Float_t sizeY, Int_t connectorsY, Int_t orientY) ;
  void SetUVStripsReadoutMap(TString readoutBoard, TString detType, TString detector, Int_t detID,  Float_t length,  Float_t outerR,  Float_t innerR, TString planeTop, Int_t conectTop, Int_t orientTop, TString planeBot, Int_t connectBot, Int_t orientBot) ;
  void SetPadsReadoutMap(TString readoutBoard, TString detType, TString detName, Int_t detID, TString padPlane, Float_t sizeX,  Float_t sizeY, Float_t nbOfPadX, Float_t nbOfPadY, Float_t nbOfConnectors) ;
  void SetCMSGEMReadoutMap(TString readoutBoard, TString detectorType,  TString detector, Int_t detID, TString EtaSector, Float_t etaSectorPos, Float_t etaSectorSize, Float_t nbOfSectorConnectors, Int_t apvOrientOnEtaSector) ;

  void Clear();

  map <Int_t, Int_t>  GetAPVNoFromIDMap()            {return fAPVNoFromIDMap ;}
  map <Int_t, Int_t>  GetAPVIDFromAPVNoMap()         {return fAPVIDFromAPVNoMap ;}
  map <Int_t, Int_t>  GetAPVGainFromIDMap()          {return fAPVGainFromIDMap ;}
  map <Int_t, Int_t>  GetAPVOrientationFromIDMap()   {return fAPVOrientationFromIDMap;}
  map <Int_t, Int_t>  GetAPVstripmappingFromIDMap()   {return fAPVstripmappingFromIDMap;}
  map <Int_t, Int_t>  GetAPVHeaderLevelFromIDMap()   {return fAPVHeaderLevelFromIDMap;}
  map <Int_t, Int_t>  GetAPVIndexOnPlaneFromIDMap()  {return fAPVIndexOnPlaneFromIDMap;}

  map <Int_t, TString>   GetAPVFromIDMap()           {return fAPVFromIDMap ;} 
  map <Int_t, TString>   GetDetectorFromIDMap()      {return fDetectorFromIDMap ;}

  map <TString, Float_t>   GetPlaneIDFromPlaneMap()      {return fPlaneIDFromPlaneMap ;}

  map<TString, vector<Float_t> > GetPadDetectorMap ()   {return fPadDetectorMap ;}
  map<TString, vector<Float_t> > GetCartesianPlaneMap() {return fCartesianPlaneMap ; }

  map <TString, list <Int_t> > GetAPVIDListFromDetectorMap()      {return fAPVIDListFromDetectorMap;}
  map <TString, list <Int_t> > GetAPVIDListFromDetectorPlaneMap() {return fAPVIDListFromDetectorPlaneMap;}

  list <Int_t> GetAPVIDListFromDetector(TString detName) {
    fAPVIDListFromDetectorMap[detName].unique() ;
    return fAPVIDListFromDetectorMap[detName];
  }

  list <Int_t> GetAPVIDListFromDetectorPlane(TString planeName) {
    fAPVIDListFromDetectorPlaneMap[planeName].unique() ;
    return fAPVIDListFromDetectorPlaneMap[planeName];
  }

  list <Int_t> GetFECIDListFromDetectorPlane(TString planeName) {
    fFECIDListFromDetectorPlaneMap[planeName].unique();
    return fFECIDListFromDetectorPlaneMap[planeName];
  }

  list <Int_t> GetAPVIDListFromFECID(Int_t fecId) {
    fAPVIDListFromFECIDMap[fecId].unique();
    return fAPVIDListFromFECIDMap[fecId];
  }

  list <TString> GetDetectorPlaneListFromDetector(TString detName) {
    fDetectorPlaneListFromDetectorMap[detName].unique();
    return  fDetectorPlaneListFromDetectorMap[detName] ;
  }

  Int_t GetFECIDFromAPVID(Int_t apvID)       {return (apvID >> 4 ) & 0xF;}
  Int_t GetADCChannelFromAPVID(Int_t apvID)  {return apvID & 0xF;}

  Int_t GetAPVNoFromID(Int_t apvID)          {return fAPVNoFromIDMap[apvID];}
  Int_t GetAPVIDFromAPVNo(Int_t apvID)       {return fAPVIDFromAPVNoMap[apvID];}
  Int_t GetAPVIndexOnPlane(Int_t apvID)      {return fAPVIndexOnPlaneFromIDMap[apvID];}
  Int_t GetAPVOrientation(Int_t apvID)       {return fAPVOrientationFromIDMap[apvID];}
  Int_t GetAPVstripmapping(Int_t apvID)       {return fAPVstripmappingFromIDMap[apvID];}
  Int_t GetAPVHeaderLevelFromID(Int_t apvID) {return fAPVHeaderLevelFromIDMap[apvID];}
  Int_t GetAPVIDFromName(TString apvName)    {return fAPVIDFromNameMap[apvName];}

  Int_t GetDetectorIDFromDetector(TString detName) {return fDetectorIDFromDetectorMap[detName];}
  TString GetDetectorFromID(Int_t detID)   {return fDetectorFromIDMap[detID];}

  TString GetDetectorTypeFromDetector(TString detectorName)   {return fDetectorTypeFromDetectorMap[detectorName];}
  TString GetReadoutBoardFromDetector(TString detectorName)   {return fReadoutBoardFromDetectorMap[detectorName];}
  TString GetDetectorFromPlane(TString planeName)             {return fDetectorFromPlaneMap[planeName];}

  vector< Float_t > GetPadDetectorMap(TString detector)     {return fPadDetectorMap[detector] ;}
  vector< Float_t > Get1DStripsReadoutMap(TString plane)    {return f1DStripsPlaneMap[plane] ;}
  vector< Float_t > GetCMSGEMReadoutMap(TString etaSector)  {return fCMSGEMDetectorMap[etaSector] ;}
  vector< Float_t > GetCartesianReadoutMap(TString plane)   {return fCartesianPlaneMap[plane] ;}
  vector< Float_t > GetUVangleReadoutMap(TString plane)     {return fUVangleReadoutMap[plane] ;}

  Float_t GetPlaneIDorEtaSector(TString planeName) {
    TString readoutType = GetReadoutBoardFromDetector(GetDetectorFromPlane(planeName))  ;
    Float_t planeIDorEtaSector = (fCartesianPlaneMap[planeName])[0] ;
    if(readoutType == "1DSTRIPS") planeIDorEtaSector = (f1DStripsPlaneMap[planeName])[0];
    if(readoutType == "CMSGEM")   planeIDorEtaSector = (fCMSGEMDetectorMap[planeName])[0];
    if(readoutType == "UV_ANGLE") planeIDorEtaSector = (fUVangleReadoutMap[planeName])[0] ;
    return planeIDorEtaSector ;
  }

  Float_t GetSizeOfPlane(TString planeName) {
    TString readoutType = GetReadoutBoardFromDetector(GetDetectorFromPlane(planeName))  ;
    Float_t planeSize   = (fCartesianPlaneMap[planeName])[1] ;
    if(readoutType == "1DSTRIPS") planeSize = (f1DStripsPlaneMap[planeName])[1] ;
    if(readoutType == "CMSGEM")   planeSize = (fCMSGEMDetectorMap[planeName])[1];
    return planeSize ;
  }

  Int_t GetNbOfAPVs(TString planeName)  {
    TString readoutType = GetReadoutBoardFromDetector(GetDetectorFromPlane(planeName))  ;
    TString detectorType = GetDetectorTypeFromDetector(GetDetectorFromPlane(planeName))  ;

    Int_t nbOfAPVs = (Int_t) (fCartesianPlaneMap[planeName])[2] ;
    if(readoutType == "1DSTRIPS") nbOfAPVs = (Int_t) (f1DStripsPlaneMap[planeName])[2];
    if(readoutType == "CMSGEM")   nbOfAPVs = (Int_t) (fCMSGEMDetectorMap[planeName])[2];
    if ((readoutType == "UV_ANGLE") &&  (detectorType == "EICPROTO1"))  nbOfAPVs =  (Int_t) (((fUVangleReadoutMap[planeName])[2]) /2 ); ;
    return nbOfAPVs ;
  } 

  Int_t GetPlaneOrientation(TString planeName)  {
    TString readoutType = GetReadoutBoardFromDetector(GetDetectorFromPlane(planeName))  ;
    Int_t orient = (Int_t) (fCartesianPlaneMap[planeName])[3] ;
    if(readoutType == "1DSTRIPS") orient = (Int_t) (f1DStripsPlaneMap[planeName])[3];
    if(readoutType == "CMSGEM")   orient = (Int_t) (fCMSGEMDetectorMap[planeName])[3];
    if(readoutType == "UV_ANGLE") orient = (Int_t) (fUVangleReadoutMap[planeName])[2];
    return orient ;
  } 
 
  TString GetDetectorPlaneFromAPVID(Int_t apvID) {return fDetectorPlaneFromAPVIDMap[apvID];}

  TString GetAPVFromID(Int_t apvID) {return fAPVFromIDMap[apvID];}
  TString GetAPV(TString detPlane, Int_t fecId, Int_t adcChannel, Int_t apvNo, Int_t apvIndex, Int_t apvID) ;

  vector<Int_t> GetPadChannelsMapping(Int_t apvID) {return fAPVToPadChannelMap[apvID]; }
  map < Int_t, vector<Int_t> > GetPadChannelsMapping() {return fAPVToPadChannelMap;}

  Int_t GetNbOfAPVs() {return fAPVNoFromIDMap.size();}
  Int_t GetNbOfFECs() {return fAPVIDListFromFECIDMap.size();}
  Int_t GetNbOfDetectorPlane() {return fDetectorFromPlaneMap.size();}
  Int_t GetNbOfDetectors() {return fReadoutBoardFromDetectorMap.size();}

  /**
  vector<Int_t> GetListOfAPVIDs(){
   vector<Int_t> list;
   map<Int_t, Int_t>::const_iterator itr;
   for (itr = fAPVNoFromIDMap.begin(); itr != fAPVNoFromIDMap.end(); itr++){
      list.push_back((*itr).first);
   }
   return list;
  }
  */

  /**
  vector<Int_t> GetListOfDetectors(){
   vector<Int_t> list;
   map<Int_t, Int_t>::const_iterator itr;
   for (itr = fDetectorFromIDMap.begin(); itr != fDetectorFromIDMap.end(); itr++){
      list.push_back((*itr).first);
   }
   return list;
  }
  */


  Bool_t IsAPVIDMapped(Int_t apvID) {
    map<Int_t, TString>::iterator itr ;
    itr = fAPVFromIDMap.find(apvID) ;
    if(itr != fAPVFromIDMap.end())
      return kTRUE ;
    else
      return kFALSE ;
  }

private:

  SRSMapping() {fNbOfAPVs = 0;}

  static SRSMapping * instance;
  Int_t fNbOfAPVs ;

  map<Int_t, Int_t>   fAPVHeaderLevelFromIDMap;
  map<Int_t, Int_t>   fAPVNoFromIDMap, fAPVIDFromAPVNoMap, fAPVIndexOnPlaneFromIDMap,fAPVOrientationFromIDMap, fAPVstripmappingFromIDMap;
  map<TString, Int_t> fNbOfAPVsFromDetectorMap ;          

  map<Int_t, Int_t>      fAPVGainFromIDMap ; 
  map<TString, Int_t>    fAPVIDFromNameMap ; 
  map<Int_t, TString>    fAPVFromIDMap ;  

  map<TString, Float_t>  fPlaneIDFromPlaneMap ;
  map<Int_t, TString>    fDetectorPlaneFromAPVIDMap;

  map<Int_t, TString>    fReadoutBoardFromIDMap ;  

  map<TString, TString>  fDetectorTypeFromDetectorMap ;
  map<TString, TString>  fReadoutBoardFromDetectorMap ;
  map<TString, Int_t>    fDetectorIDFromDetectorMap ;  
  map<Int_t, TString>    fDetectorFromIDMap ;   
  map<Int_t, TString>    fDetectorFromAPVIDMap; 
  map<TString, TString > fDetectorFromPlaneMap ;

  map<Int_t, list<Int_t> >   fAPVIDListFromFECIDMap;       

  map<TString, list<Int_t> > fFECIDListFromDetectorPlaneMap; 
  map<TString, list<Int_t> > fAPVIDListFromDetectorMap;     
  map<TString, list<Int_t> > fAPVIDListFromDetectorPlaneMap;

  map<TString, list<TString> > fDetectorListFromDetectorTypeMap ;
  map<TString, list<TString> > fDetectorListFromReadoutBoardMap;
  map<TString, list<TString> > fDetectorPlaneListFromDetectorMap ;

  map<Int_t, vector<Int_t> > fAPVToPadChannelMap; 

  map<TString, vector<Float_t> > fPadDetectorMap ;
  map<TString, vector<Float_t> > fUVangleReadoutMap;
  map<TString, vector<Float_t> > f1DStripsPlaneMap ;
  map<TString, vector<Float_t> > fCartesianPlaneMap ;
  map<TString, vector<Float_t> > fCMSGEMDetectorMap ;

  ClassDef(SRSMapping, 1)
};

#endif



