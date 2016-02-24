/**************************************************************************
 * Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               *
 **************************************************************************/
#ifndef AMORE_SRS_SUBSCRIBERSRSSUBSCRIBER_H
#define AMORE_SRS_SUBSCRIBERSRSSUBSCRIBER_H

#include <MonitorObject.h>
#include <SubscriberModule.h>
#include <SRSCommon.h>

namespace amore {

namespace SRS {

namespace subscriber {

/**
@author Filimon Roukoutakis
*/

class SRSSubscriber : public amore::subscriber::SubscriberModule, public amore::SRS::common::SRSCommon {

 public:
 
  SRSSubscriber();
  virtual ~SRSSubscriber();
  virtual void SubscribeMonitorObjects();
  virtual void Process();
  virtual void Update();
  virtual void StartOfCycle();
  virtual void EndOfCycle();
  virtual void StartOfRun();
  virtual void EndOfRun();
  virtual void StartOfSession();
  virtual void EndOfSession();
 
  ClassDef(SRSSubscriber, 1);

};

};

};

};

#endif
