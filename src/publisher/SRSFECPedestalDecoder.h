#ifndef __SRSFECPEDESTALDECODER__
#define __SRSFECPEDESTALDECODER__
/*******************************************************************************
 *  AMORE FOR SRS - SRS                                                         *
 *  SRSFECPedestalDecoder                                                          *
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
#endif

using namespace std;

class SRSFECPedestalDecoder : public TObject {
    
public:
    
    SRSFECPedestalDecoder(Int_t nwords, UInt_t * buf);
    SRSFECPedestalDecoder(){;}
    ~SRSFECPedestalDecoder();
    
    void BuildAPVEvent(vector<UInt_t> data32bits, Int_t fec_no, Int_t fec_channel) ;
    
    Int_t GetNWords() {return fNWords;}
    Int_t GetEventNumber() {return fEventNb;}
    Int_t GetNbOfFECEvents() {return ((Int_t) fFECEvents->GetSize()); }
    Int_t GetPacketSize() {return fPacketSize;}
    
    
    TList * GetFECEvents() {return fFECEvents ;}
    SRSAPVEvent * GetAPVEvent(Int_t apvID) ;
    
    map<Int_t, vector <Int_t> > GetActiveFecChannelsMap() {return fActiveFecChannelsMap ;}
    
private:
    
    UInt_t * fBuf;
    Int_t fNWords, fEventNb, fPacketSize;
    Bool_t fIsNewPacket ;
    
    vector <Int_t>  fActiveFecChannels ;
    map<Int_t, vector <Int_t> > fActiveFecChannelsMap ;
    
    TList * fFECEvents;
    ClassDef(SRSFECPedestalDecoder, 1)
};

#endif
