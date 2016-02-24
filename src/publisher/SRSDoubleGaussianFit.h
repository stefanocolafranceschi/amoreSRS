#ifndef SRSDOUBLEGAUSSIANFIT_H
#define SRSDOUBLEGAUSSIANFIT_H
/*******************************************************************************
 *  AMORE FOR SRS - SRS                                                         *
 *  SRSDoubleGaussianFit                                                        *
 *  SRS Module Class                                                            *
 *  Author: Kondo GNANVO 28/05/2014                                             *
 *******************************************************************************/

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <map>
#include <vector>
#include <TMath.h>
#include <iostream>
#include <TObject.h>
#include "TCanvas.h"
#include "TH1F.h"
#include "TString.h"
#include "TSpectrum.h"
#include "TF1.h"

#endif

using namespace std;

class SRSDoubleGaussianFit : public TObject {
    
public:
    
    SRSDoubleGaussianFit() ;
    SRSDoubleGaussianFit(TH1F *htemp, int N_iter, float N_sigma_range, bool ShowFit) ;
    ~SRSDoubleGaussianFit();
    
    //  void DoubleGaussianFitLoop(TH1F *htemp, int N_iter, float N_sigma_range, bool ShowFit) ;
    
    Float_t GetRChi2() {return fRChi2;}
    Float_t GetMean() {return fMean;}
    Float_t GetSigma() {return fMeanError;}
    Float_t GetMeanError() {return fMeanError;}
    Float_t GetSigmaError() {return fSigmaError;}
    
private:
    
    Float_t fRChi2, fMean, fMeanError, fSigma, fSigmaError ;
    ClassDef(SRSDoubleGaussianFit,1)
};

#endif
