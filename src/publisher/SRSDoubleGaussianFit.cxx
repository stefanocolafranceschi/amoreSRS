// Author: Kondo GNANVO 01/05/2011
#include "SRSDoubleGaussianFit.h"

//================================================================================================//
SRSDoubleGaussianFit::SRSDoubleGaussianFit() {
    //Set initial values...(in case fit fails)
    fRChi2      = -100 ;
    fMean       = -100 ;
    fMeanError  = -100 ;
    fSigma      = -100 ;
    fSigmaError = -100 ;
}

//================================================================================================//
SRSDoubleGaussianFit::~SRSDoubleGaussianFit() {
}

//================================================================================================//
//void SRSDoubleGaussianFit::DoubleGaussianFitLoop(TH1F *htemp, int N_iter, float N_sigma_range, bool ShowFit) {
SRSDoubleGaussianFit::SRSDoubleGaussianFit(TH1F *htemp, int N_iter, float N_sigma_range, bool ShowFit ) {
    //Set initial values...(in case fit fails)
    fRChi2      = -100 ;
    fMean       = -100 ;
    fMeanError  = -100 ;
    fSigma      = -100 ;
    fSigmaError = -100 ;
    
    //---------Basic Histo Peak Finding Parameters----------
    Int_t binMaxCnt   = htemp->GetMaximumBin() ;
    Int_t binMaxCnt_counts = (Int_t) htemp->GetBinContent(binMaxCnt);
    Double_t binMaxCnt_value  = (Double_t) htemp->GetXaxis()->GetBinCenter(binMaxCnt);
    
    TSpectrum * s = new TSpectrum(); //TSpectrum(1,1)->Argument: (Number of peaks to find, Distance to neighboring peak: "1"-->3sigma)
    
    Int_t NPeaks;
    Float_t * Peak;           //TSpectrum *s = new TSpectrum(); --> No warning message
    Float_t * PeakAmp;
    
    Float_t peak_pos = 0;
    Float_t peak_pos_amp = 0;  //Initial value, assuming positive going peaks
    Int_t   peak_pos_bin = 0;
    
    Int_t Nbins = 0 ;
    Int_t zero_value_bin = 0 ;
    Float_t low_limit = 0;
    Float_t high_limit = 0;
    Float_t peak_pos_count = 0;
    Float_t zero_bin_value = 0;
    Float_t max_bin_value = 0;
    
    //---------TSpectrum Peak Finding Parameters--------
    if (ShowFit) NPeaks = s->Search(htemp, 2, "goff", 0.5); //opens a canvas (one time in a loop), even with:  s->Search(htemp, 2, "nodraw", 0.9);
    Peak = s->GetPositionX();
    PeakAmp = s->GetPositionY();
    
    for ( Int_t i = 0; i < NPeaks; i++)     {
        if (peak_pos_amp < PeakAmp[i]) 	{
            peak_pos_amp = PeakAmp[i];   //TSpectrum finds peak counts
            peak_pos = Peak[i];          //TSpectrum finds pos. of peak in x-axis units
        }
    }
    
    peak_pos_bin = htemp->GetXaxis()->FindBin(peak_pos) ;
    peak_pos_count = htemp->GetBinContent(peak_pos_bin) ;
    zero_value_bin = htemp->GetXaxis()->FindBin(0.0) ;
    Nbins = htemp->GetSize() - 2 ;
    zero_bin_value =  htemp->GetXaxis()->GetBinCenter(0) ;
    max_bin_value =  htemp->GetXaxis()->GetBinCenter(Nbins) ;
    
    TF1 *func ;
    TF1 *func1 ;
    TF1 *func2 ;
    TF1 *func3 ;
    
    Float_t Chi2 ;
    Int_t NDF = 1 ;
    
    Float_t RChi2 = 1 ;
    Float_t peak  = 1;
    
    Float_t const1     = 1 ;
    Float_t mean1      = 1 ;
    Float_t mean_err1  = 1 ;
    Float_t sigma1     = 1 ;
    Float_t sigma_err1 = 1 ;
    
    Float_t const2     = 1 ;
    Float_t mean2      = 1 ;
    Float_t mean_err2  = 1 ;
    Float_t sigma2     = 1 ;
    Float_t sigma_err2 = 1 ;
    
    
    int TS = 0 ;
    func = new TF1("func", "gaus") ;
    
    //Make sure that TSpectrum peak is within histo range if not, use Par initial values from Basic Histo Peak Find
    if (peak_pos >= zero_bin_value  &&  peak_pos <= max_bin_value)  {
        TS=1 ;
        low_limit  = peak_pos - (0.1 * abs(max_bin_value-zero_bin_value));
        high_limit = peak_pos + (0.1 * abs(max_bin_value-zero_bin_value)) ;
        func->SetParameter(0, peak_pos_count) ;
        func->SetParameter(1, peak_pos) ;
    }
    
    else {
        low_limit = binMaxCnt_value - (0.1 * abs(max_bin_value-zero_bin_value));
        high_limit = binMaxCnt_value + (0.1 * abs(max_bin_value-zero_bin_value));
        func->SetParameter(0, binMaxCnt_counts);
        func->SetParameter(1, binMaxCnt_value);
    }
    
    //low_limit, high_limit); //  To Show fit: htemp->Fit("gaus"); //better fit?-> Fit("gaus", "MQ", "", "", "");
    htemp->Fit("gaus", "Q0", "", low_limit, high_limit) ;
    func  = htemp->GetFunction("gaus") ;
    Chi2 = func->GetChisquare() ;
    NDF = func->GetNDF() ;
    if (NDF != 0) RChi2 = Chi2/NDF ;
    const1 = func->GetParameter(0) ;
    mean1 = func->GetParameter(1) ;
    mean_err1 = func->GetParError(1) ;
    sigma1 = func->GetParameter(2) ;
    sigma_err1 = func->GetParError(2) ;
    
    for (Int_t i = 0; i< N_iter; i++) { //8 seems to work well, so let's keep it constant here.
        //   for (int i=0; i< 2; i++) {
        htemp->Fit("gaus", "Q0", "",(mean1 - (N_sigma_range*sigma1)), (mean1 + (N_sigma_range*sigma1) )) ; //don't show fit
        func  = htemp->GetFunction("gaus") ;
        Chi2 = func->GetChisquare() ;
        NDF = func->GetNDF() ;
        if (NDF != 0) RChi2 = Chi2/NDF ;
        const1 = func->GetParameter(0) ;
        mean1 = func->GetParameter(1) ;
        mean_err1 = func->GetParError(1) ;
        sigma1 = func->GetParameter(2) ;
        sigma_err1 = func->GetParError(2) ;
        cout << " sigma " << sigma1 << endl;
        sigma_err1 = func->GetParError(2);
    }
    
    //Amplitude
    peak =  func->GetParameter(0) ;
    
    //background height ~ bgd_h*gaus_amp
    Float_t bgd_h = 0.25;
    
    func1 = new TF1("func1", "gaus") ;
    func2 = new TF1("func2", "gaus") ;
    func3 = new TF1("func3", "func1 + func2", (mean1 - 3*sigma1), (mean1 + 3*sigma1) );
    
    //-----------Fit Parameter constraints:
    func3->SetParameters(const1, mean1, sigma1, const1/10, mean1, 4*sigma1); //Set Initial Valules
    //Max=5% of peak amp //************Set peak limit of background sigma*************
    func3->SetParLimits(3, 0, (0.05*peak_pos_amp) );
    
    //Don't show fit
    htemp->Fit("func3", "Q0");
    func  = htemp->GetFunction("func3") ;
    Chi2 = func3->GetChisquare() ;
    NDF = func->GetNDF() ;
    if (NDF != 0) RChi2 = Chi2/NDF ;
    const1 = func3->GetParameter(0);
    mean1 = func3->GetParameter(1);
    mean_err1 = func3->GetParError(1);
    sigma1 = func3->GetParameter(2);
    sigma_err1 = func3->GetParError(2);
    
    const2 = func3->GetParameter(3);
    mean2 = func3->GetParameter(4);
    mean_err2 = func3->GetParError(4);
    sigma2 = func3->GetParameter(5);
    sigma_err2 = func3->GetParError(5);
    func3->SetParNames("Primary Constant", "Primary Mean", "Primary Sigma", "Background Constant", "Background Mean", "Background Sigma");
    
    for (Int_t j=0; j<4; j++)  {
        func3->SetParameters(const1, mean1, sigma1, const2, mean2, sigma2) ;
        
        //----------------Show or don't show fit-----------------
        if (ShowFit) htemp->Fit("func3", "Q"); //*************Show Histo & Fit in quiet mode
        else htemp->Fit("func3", "Q0");        //*****************Don't show Histo & Fit in quiet mode
        //-------------------------------------------------------
        
        func3  = htemp->GetFunction("func3");;
        func3->SetLineColor(2);
        Chi2 = func3->GetChisquare();
        NDF = func3->GetNDF();
        if (NDF != 0) RChi2 = Chi2/NDF;
        const1 = func3->GetParameter(0);
        mean1 = func3->GetParameter(1);
        mean_err1 = func3->GetParError(1);
        sigma1 = func3->GetParameter(2);
        sigma_err1 = func3->GetParError(2);
        const2 = func3->GetParameter(3);
        mean2 = func3->GetParameter(4);
        mean_err2 = func3->GetParError(4);
        sigma2 = func3->GetParameter(5);
        sigma_err2 = func3->GetParError(5);
    }
    if (abs(const1 - peak) < abs(const2 - peak)) {
        fRChi2 = RChi2;
        fMean = mean1;
        fMeanError = mean_err1;
        fSigma = abs(sigma1);
        fSigmaError = sigma_err1;
    }
    else {
        fRChi2 = RChi2;
        fMean = mean2;
        fMeanError = mean_err2;
        fSigma = abs(sigma2);
        fSigmaError = sigma_err2;
    }
    
    delete s;
    delete func;
    delete func1;
    delete func2;
    delete func3; 
}
