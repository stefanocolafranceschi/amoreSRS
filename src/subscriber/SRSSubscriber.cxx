/**************************************************************************
 * Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE DAQ Project.                                         *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/
#include "SRSSubscriber.h"

ClassImp(amore::SRS::subscriber::SRSSubscriber)

#include <sstream>
#include <iostream>

namespace amore {

namespace SRS {

namespace subscriber {

SRSSubscriber::SRSSubscriber()	{
}

SRSSubscriber::~SRSSubscriber()	{
}

void SRSSubscriber::SubscribeMonitorObjects() {
  std::ostringstream stringStream;
  for(size_t i=0; i<100; ++i) {
    stringStream << i;
    stringStream.str("\r");
  }
}

void SRSSubscriber::StartOfCycle() {
}

void SRSSubscriber::EndOfCycle() {
  Process();
  Update();
}

void SRSSubscriber::Process() {
  std::cout << "\nProcessing!";
  usleep(1000000);
}

void SRSSubscriber::Update() {
  std::cout << " Updating! ";
  usleep(1000000);
}

void SRSSubscriber::StartOfRun() {
}

void SRSSubscriber::EndOfRun() {
}

void SRSSubscriber::StartOfSession() {
}

void SRSSubscriber::EndOfSession() {
}

};

};

};
