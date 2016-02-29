#ifndef __SRSFECEVENTDECODER__
#define __SRSFECEVENTDECODER__
/*******************************************************************************
*  AMORE FOR SRS - SRS                                                         *
*  SRSFECEventDecoder                                                          *
*  SRS Module Class                                                            *
*  Author: Kondo GNANVO 18/08/2010                                             *
*******************************************************************************/

#if !defined(__CINT__) || defined(__MAKECINT__)

#include <map>
#include <list>
#include <vector>
#include <iostream>

#include "TList.h"
#include "TObject.h"
#include "SRSMapping.h"
#include "SRSAPVEvent.h"
#include "SRSPedestal.h"
#include "SRSEventBuilder.h"
#include "SRSPositionCorrection.h"
#endif

using namespace std;

class SRSFECEventDecoder : public TObject {

public:

  SRSFECEventDecoder(Int_t nwords, UInt_t * buf,  SRSEventBuilder * eventBuilder);
  SRSFECEventDecoder(Int_t nwords, UInt_t * buf,  SRSPedestal * ped, SRSEventBuilder * eventBuilder, Int_t zeroSupCut);
  SRSFECEventDecoder(Int_t nwords, UInt_t * buf,  SRSPedestal * ped, SRSPositionCorrection * clusterPosCorr, Int_t zeroSupCut);

  SRSFECEventDecoder(){;}
  ~SRSFECEventDecoder();

  void BuildHits(vector<UInt_t> data32bits, Int_t fec_no, Int_t fec_channel, SRSPedestal * ped, SRSEventBuilder * eventBuilder, Int_t zeroSupCut) ;
  void BuildRawAPVEvents(vector<UInt_t> data32bits, Int_t fec_no, Int_t fec_channel, SRSEventBuilder * eventBuilder);
  void BuildHitForPositionCorrection(vector<UInt_t> data32bits, Int_t fec_id, Int_t adc_channel, SRSPedestal * ped, SRSPositionCorrection * clusterPosCorr, Int_t zeroSupCut) ;

  Int_t GetNWords() {return fNWords;}
  Int_t GetEventNumber() {return fEventNb;}
  Int_t GetPacketSize() {return fPacketSize;}

  map<Int_t, vector <Int_t> > GetActiveFecChannelsMap() {return fActiveFecChannelsMap ;}

 
private:

  UInt_t * fBuf;
  Int_t fNWords, fEventNb, fPacketSize;
  Bool_t fIsNewPacket ;

  vector <Int_t>  fActiveFecChannels ;
  map<Int_t, vector <Int_t> > fActiveFecChannelsMap ;

  ClassDef(SRSFECEventDecoder, 1)
};

#endif
