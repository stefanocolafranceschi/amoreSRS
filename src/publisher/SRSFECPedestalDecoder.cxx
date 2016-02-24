#include "SRSFECPedestalDecoder.h"

ClassImp(SRSFECPedestalDecoder);

SRSFECPedestalDecoder::SRSFECPedestalDecoder(Int_t nwords, UInt_t * buf) {
    
    SRSMapping * mapping = SRSMapping::GetInstance();
    map <Int_t, Int_t> apvNoFromApvIDMap = mapping->GetAPVNoFromIDMap();
    fBuf = buf;
    fNWords = nwords;
    fEventNb = -1 ;
    fIsNewPacket = kFALSE ;
    fPacketSize = 4000 ;
    
    fActiveFecChannelsMap.clear() ;
    map <Int_t, Int_t >::const_iterator fecChannel_itr ;
    for(fecChannel_itr = apvNoFromApvIDMap.begin(); fecChannel_itr != apvNoFromApvIDMap.end(); ++fecChannel_itr) {
        Int_t apvid = (* fecChannel_itr).first ;
        Int_t activeChannel = apvid & 0xF;
        Int_t fecId = (apvid >> 4 ) & 0xF;
        fActiveFecChannelsMap[fecId].push_back(activeChannel) ;
        //    printf("  SRSFECPedestalDecoder => List of  fecNo=%d, activeChannel = %d\n", fecId, activeChannel) ;
    }
    
    Int_t current_offset = 0, fecID = 0 ;
    fFECEvents = new TList ;
    //==========================================================================//
    // Needed as the key to link apvID (or fecChannel) to the apvEvent in the TList  //
    // Should be < to 15 (max 16 APV channel in the FEC)                        //
    //==========================================================================//
    
    Int_t fecChannel = 0, apvID = 0 ;
    UInt_t currentAPVPacketHdr;
    Int_t previousAPVPacketSize = 0 ;
    
    vector<UInt_t> data32BitsVector ;
    data32BitsVector.clear() ;
    
    //===============================================================================//
    // Dealing with the 7 Equipment header words. We just skip the first 2 words     //
    // and go straight to the 3rd word  where we extract the FEC no (Equip Id)       //
    //===============================================================================//
    current_offset += 2 ;
    UInt_t eqHeaderWord = fBuf[current_offset] ;
    fecID = eqHeaderWord & 0xff ;
    fActiveFecChannels.clear();
    fActiveFecChannels = fActiveFecChannelsMap[fecID] ;
    //  printf("  SRSFECPedestalDecoder => List of  fecNo = %d\n", fecID) ;
    
    //=== The next 4 words are Equip word, we dont care
    current_offset += 5 ;
    
    //================================================================================//
    // Start looking at the APV data word from here                                   //
    //================================================================================//
    while (current_offset < fNWords) {
        
        UInt_t rawdata = fBuf[current_offset] ;
        //   if (((rawdata >> 8) & 0xffffff) == 0x414443) printf("  SRSFECPedestalDecoder => dataWord=0x%x \n", rawdata) ;
        //    printf("  SRSFECPedestalDecoder => dataWord=0x%x \n", rawdata) ;
        //=============================================================================//
        // end of event ==> break: add the data from the last sample here              //
        //=============================================================================//
        
        if (rawdata == 0xfafafafa) {
            //===================================================================================================//
            // last word of the previous packet added for Filippo in DATE to count the eventNb x 16 UDP packets  //
            // We dont need it here, will just skip it We remove it from the vector of data                      //
            //===================================================================================================//
            if(!data32BitsVector.empty()) {
                BuildAPVEvent(data32BitsVector, fecID, fecChannel) ;
            }
            
            fecChannel = 0 ;
            data32BitsVector.clear() ;
            current_offset++ ;
            break ;
        }
        
        //==========================================================================================//
        // Word with the event number (trigger count) and the packet size information               //
        //==========================================================================================//
        /**
         if (fIsNewPacket) {
         previousAPVPacketSize = (rawdata & 0xffff) ;
         fEventNb = (rawdata & 0xffff) ;
         
         //printf("SRSFECPedestalDecoder::SRSFECPedestalDecoder()=> Sorin 2nd header word=0x%x, packet size=%d\n",rawdata, previousAPVPacketSize) ;
         fIsNewPacket = kFALSE ;
         current_offset++ ;
         continue ;
         }
         */
        
        //==========================================================================================//
        //                                     size of APV packet                                    //
        //==========================================================================================//
        if (fIsNewPacket) {
            //    if (((rawdata >> 8) & 0xffff) == 0xaabb) {
            fPacketSize = (rawdata & 0xffff) ;
            //      printf("SRSFECEventDecoder::SRSFECEventDecoder()=> Sorin 2nd header word=0x%x, packet size=%d\n",rawdata, fPacketSize) ;
            //      printf("SRSFECEventDecoder::SRSFECEventDecoder()=> Sorin 2nd header word=0x%x, packet size=%d\n",rawdata, previousAPVPacketSize) ;
            data32BitsVector.clear() ;
            fIsNewPacket = kFALSE ;
            current_offset++ ;
            continue ;
        }
        
        //=========================================================================================================//
        //         New packet (or frame) FEC channel data in the equipment                                         //
        //=========================================================================================================//
        if (((rawdata >> 8) & 0xffffff) == 0x414443) {
            
            Int_t datasize = data32BitsVector.size() ;
            //printf("  SRSFECPedestalDecoder => dataWord=0x%x, fecID=%d, apvID=%d, FEC Channel=%d, data size=%d \n",rawdata, fecID, apvID, fecChannel, datasize) ;
            
            data32BitsVector.pop_back() ;
            if(!data32BitsVector.empty()) {
                BuildAPVEvent(data32BitsVector, fecID, fecChannel) ;
            }
            
            currentAPVPacketHdr = rawdata  ;
            fecChannel = currentAPVPacketHdr & 0xff ;
            //      printf("  SRSFECPedestalDecoder => List of  fecNo=%d, adcChannel = %d\n", fecID, fecChannel) ;
            
            //=== REINITIALISE EVERYTHING
            if(fecChannel > 15) {
                printf("  SRSFECPedestalDecoder => ERROR => fecID=%d, apvID=%d, FEC Channel = %d > 15 \n", fecID, apvID, fecChannel) ;
                break ;
            }
            
            data32BitsVector.clear() ;
            fIsNewPacket = kTRUE ;
            current_offset++ ;
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
}

//====================================================================================================================
void SRSFECPedestalDecoder::BuildAPVEvent(vector<UInt_t> data32bits, Int_t fec_no, Int_t fec_channel) {
    Int_t apvID = (fec_no << 4) | fec_channel ;
    if (find(fActiveFecChannels.begin(), fActiveFecChannels.end(), fec_channel) != fActiveFecChannels.end() ) {
        Int_t sigmaCut = 0 ;
        SRSAPVEvent * apvEvent = new SRSAPVEvent(fec_no, fec_channel, apvID, sigmaCut, fEventNb, fPacketSize) ;
        vector <UInt_t >::const_iterator data_itr ;
        for(data_itr = data32bits.begin(); data_itr != data32bits.end(); ++data_itr) {
            UInt_t data = (* data_itr) ;
            apvEvent->Add32BitsRawData(data) ;
        }
        fFECEvents->Add(apvEvent) ;
    }
}

//=================================================================================================================================
SRSAPVEvent * SRSFECPedestalDecoder::GetAPVEvent(Int_t apvID) {
    SRSMapping * mapping = SRSMapping::GetInstance() ;
    return (SRSAPVEvent*) (fFECEvents->At(mapping->GetAPVNoFromID(apvID))) ;
}

//==================================================================================================================================
SRSFECPedestalDecoder::~SRSFECPedestalDecoder() {
    map<Int_t, vector <Int_t> >::const_iterator activeChannel_itr ;
    for(activeChannel_itr = fActiveFecChannelsMap.begin(); activeChannel_itr != fActiveFecChannelsMap.end(); ++activeChannel_itr) {
        vector <Int_t> activeCh = (* activeChannel_itr).second ;
        activeCh.clear() ;
    }
    fActiveFecChannelsMap.clear() ;
    TIter nextAPVEvent(fFECEvents) ;
    while(SRSAPVEvent * apvEvent = (SRSAPVEvent *) nextAPVEvent()) {
        delete apvEvent ;
    }
    fFECEvents->Delete() ;
    delete fFECEvents ;
}

