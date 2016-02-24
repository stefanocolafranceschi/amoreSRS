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
#include "SRSUI.h"

ClassImp(amore::SRS::ui::SRSUI)

#include <iostream>
#include <sstream>
#include <TCanvas.h>
#include <TQObject.h>

namespace amore {
  namespace SRS {
    namespace ui {

      using amore::subscriber::Subscribe;
      using amore::subscriber::Unsubscribe;
      
      //===========================================================================================================
      SRSUI::SRSUI() {
	SetDisplayParameters() ;
      }

      //===========================================================================================================
      SRSUI::~SRSUI() {
        Unsubscribe() ;

        ClearDisplayParameters() ;

	for(Int_t i = 0; i < MAXNUMBERCANVAS; i++) {
	  delete fEC[i];
	}
	delete fTab ;
	delete fSRSConf ;
	delete fMapping ;
     }

      //===========================================================================================================
      void SRSUI::Construct() { // The custom GUI is constructed here. gRootFrame is the container of the custom widgets.
	printf("SRSUI::Construct() = > let Start to display with %s run\n", fRunType.Data()) ;

	//=== Class in which TGTab are created 
	fTab = new TGTab(amore::ui::gRootFrame);
	amore::ui::gRootFrame->AddFrame(fTab);
	TQObject::Connect(fTab, "Selected(Int_t)", "amore::SRS::ui::SRSUI", this, "SlotSelectedTab(Int_t)");
	TGCompositeFrame* tempFrame ;

	if(fRunType == "RAWDATA") {
	  Int_t icanvas = 0 ;
	  map<TString, Int_t>::const_iterator rowMap_it ;
	  for (rowMap_it = fRowMap.begin(); rowMap_it != fRowMap.end(); ++rowMap_it) {
	    TString canvasName = rowMap_it->first ;

	    if (canvasName == "RAWDATA_ALLAPVs") {
	      icanvas++ ;
	      Int_t nbOfAPVs = fMapping->GetNbOfAPVs() ;
	      Int_t nbOfCols = fColMap[canvasName] ;
	      Int_t nbOfRows = fRowMap[canvasName] ;
	      Int_t length   = fLengthMap[canvasName] ;
	      Int_t width    = fWidthMap[canvasName] ;
	      printf("SRSUI::Construct() = > canvas nbOfAPVs = %d, nbCols = %d, nbRows = %d length = %d width = %d\n",nbOfAPVs , nbOfCols, nbOfRows, length, width) ;

	      Int_t nbOfTabs = nbOfCols * nbOfRows ;
	      Int_t nbOfCanvas = 1 ;

	      if( (nbOfAPVs % nbOfTabs) == 0 ) nbOfCanvas = (Int_t) ( nbOfAPVs /  nbOfTabs);
	      else                             nbOfCanvas = ((Int_t) ( nbOfAPVs / nbOfTabs )) + 1 ;

	      for(Int_t i = 0 ; i < nbOfCanvas; i++) {
		std::stringstream stringStream;
		stringStream << i ;
		TString canvasId = stringStream.str();  
		TString canvasname = "gui1_rawdata" + canvasId ;
		tempFrame = fTab->AddTab(canvasname) ;	  
		fEC[i] = new TRootEmbeddedCanvas(canvasname, tempFrame, length, width) ;
		tempFrame->AddFrame(fEC[i]) ;
		fEC[i]->GetCanvas()->Divide(nbOfRows, nbOfCols) ;
		printf("SRSUI::Construct() = > canvas No %d, has %d cols and %d rows and %d length and %d width\n", i, nbOfCols, nbOfRows, length, width) ;
	      }
	    }

	    else if (canvasName == "RAWDATA_ALLPLANES") {
	      Int_t nbOfPlanes = fMapping->GetNbOfDetectorPlane() ;
	      Int_t nbOfCols   = fColMap[canvasName] ;
	      Int_t nbOfRows   = fRowMap[canvasName] ;
	      Int_t length     = fLengthMap[canvasName] ;
	      Int_t width      = fWidthMap[canvasName] ;
	      printf("SRSUI::Construct() = > canvas nbOfPlanes = %d, nbCols = %d, nbRows = %d length = %d width = %d\n",nbOfPlanes , nbOfCols, nbOfRows, length, width) ;

	      Int_t nbOfTabs = nbOfCols * nbOfRows ;
	      Int_t nbOfCanvas = 1 ;

	      if( (nbOfPlanes % nbOfTabs) == 0 ) nbOfCanvas = (Int_t) ( nbOfPlanes / nbOfTabs);
	      else                               nbOfCanvas = ((Int_t) ( nbOfPlanes / nbOfTabs )) + 1 ;

	      for(Int_t i = 0 ; i < nbOfCanvas; i++) {
		std::stringstream stringStream;
		stringStream << i ;
		TString canvasId = stringStream.str();  
		TString canvasname = "gui1_plane" + canvasId ;
	       	tempFrame = fTab->AddTab(canvasname) ;	  
		fEC[i] = new TRootEmbeddedCanvas(canvasname, tempFrame, length, width) ;
		tempFrame->AddFrame(fEC[i]) ;
		fEC[i]->GetCanvas()->Divide(nbOfRows, nbOfCols) ;
		printf("SRSUI::Construct() = > canvas No %d, has %d cols and %d rows and %d length and %d width\n", i, nbOfCols, nbOfRows, length, width) ;
	      }
	    }

	    else {
	      if (canvasName.Contains("RAWDATA_FEC") ) {
		TString canvas = canvasName ;
		Int_t fecID = (canvas.Remove(0,11)).Atoi();
		//		Int_t nbOfCols = fColMap[canvasName] ;
		//		Int_t nbOfRows = fRowMap[canvasName] ;
		Int_t nbOfCols = 4 ; 
		Int_t nbOfRows = 4 ;
		Int_t length   = fLengthMap[canvasName] ;
		Int_t width    = fWidthMap[canvasName] ;
		TString name = canvasName ;
		TString canvasname = "gui1_" + name.Remove(0,8);
		tempFrame = fTab->AddTab(canvasname) ;
		fEC[icanvas] = new TRootEmbeddedCanvas(canvasname, tempFrame, length, width) ;
		tempFrame->AddFrame(fEC[icanvas]) ;
		fEC[icanvas]->GetCanvas()->Divide(nbOfRows, nbOfCols) ;
		printf("SRSUI::Construct() = > canvas %s with canvasID %d, has %d cols and %d rows and %d length and %d width\n", canvasName.Data(), icanvas, nbOfCols, nbOfRows, length, width) ;
		icanvas++ ;
	      }
	    }
	  }
	}
	    
	else {
	  Int_t icanvas = 0 ;
	  //=== Loop over the data types
	  map<TString, Int_t>::const_iterator rowMap_it ;
	  for (rowMap_it = fRowMap.begin(); rowMap_it != fRowMap.end(); ++rowMap_it) {
	    TString canvasName = rowMap_it->first ;
	    if ( (canvasName == "RAWDATA_ALLAPVs") || (canvasName == "RAWDATA_ALLPLANES") || (canvasName.Contains("RAWDATA_FEC"))) continue ;
	    Int_t nbOfCols   = fColMap[canvasName] ;
	    Int_t nbOfRows   = fRowMap[canvasName] ;
	    Int_t length = fLengthMap[canvasName] ;
	    Int_t width  = fWidthMap[canvasName] ;
	    TString canvasname = "gui1_"+ canvasName ;
	    tempFrame = fTab->AddTab(canvasname) ;	  
	    fEC[icanvas] = new TRootEmbeddedCanvas(canvasname, tempFrame, length, width) ;
	    tempFrame->AddFrame(fEC[icanvas]) ;
	    fEC[icanvas]->GetCanvas()->Divide(nbOfRows, nbOfCols) ;
	    printf("SRSUI::Construct() = > canvas %s with canvasID %d, has %d cols and %d rows and %d length and %d width\n", canvasName.Data(), icanvas, nbOfCols, nbOfRows, length, width) ;
	    icanvas++  ;
	  }
	}

	amore::ui::gRootFrame->MapSubwindows() ;
	amore::ui::gRootFrame->Resize() ;
	amore::ui::gRootFrame->MapWindow() ; 

	SlotSelectedTab(0) ; // To select initial subscriptions/display

	printf("SRSUI::Construct() = > Exit Construct \n") ;
      }

      //===========================================================================================================
      void SRSUI::SubscribeMonitorObjects() { // This is now deprecated, not recognized by the framework
      }

      //===========================================================================================================
      void SRSUI::SlotSelectedTab(Int_t id) {
	switch(id) {
	case 0:
	  MySubscribe();
	  fSelectedTab = 0;
	  break;
	case 1:
          MySubscribe();
	  fSelectedTab = 1;
	  break;
	}
      }

      //===========================================================================================================
      void SRSUI::Update() { 
       	if (fSelectedTab == 0) Update1() ;
	if (fSelectedTab == 1) Update1() ;
      }

      //===========================================================================================================
      void SRSUI::MySubscribe() {
	Unsubscribe() ;

	if(fRunType == "RAWDATA") {
	  SRSMapping * fMapping = SRSMapping::GetInstance() ;
	  map<TString, Int_t>::const_iterator rowMap_it ;

	  for (rowMap_it = fRowMap.begin(); rowMap_it != fRowMap.end(); ++rowMap_it) {
	    TString canvasName = rowMap_it->first ;

            if (canvasName == "RAWDATA_ALLAPVs") {
	      map <Int_t, TString> listOfAPVs = fMapping->GetAPVFromIDMap() ;
	      map <Int_t, TString>::const_iterator apv_itr ;
	      for (apv_itr = listOfAPVs.begin(); apv_itr != listOfAPVs.end(); ++ apv_itr) {
		Int_t apvID = (*apv_itr).first;
		TString histoName = fMapping->GetAPVFromID(apvID) ;
		// printf("  SRSUI.cxx::MySubscribe() => histo=%s\n", histoName.Data()) ;
		std::stringstream stringStream;
		stringStream.str("\r") ;
		stringStream << "SRS01/" << histoName ;
		Subscribe(stringStream.str().c_str()) ;
		stringStream.str("\r") ;
	      }
	    }
	    else if (canvasName == "RAWDATA_ALLPLANES") {
	      map <TString, Float_t> listOfPlanes = fMapping->GetPlaneIDFromPlaneMap() ;
	      map <TString, Float_t>::const_iterator plane_itr ;
	      for (plane_itr = listOfPlanes.begin(); plane_itr != listOfPlanes.end(); ++ plane_itr) {
		TString histoName = (*plane_itr).first;
		printf("  SRSUI.cxx::MySubscribe() => histo=%s\n", histoName.Data()) ;
		std::stringstream stringStream;
		stringStream.str("\r") ;
		stringStream << "SRS01/" << histoName ;
		Subscribe(stringStream.str().c_str()) ;
		stringStream.str("\r") ;
	      }
	    }

	    else {
	      if (canvasName.Contains("RAWDATA_FEC")) {
		TString canvasname = canvasName ;
		Int_t fecID = (canvasname.Remove(0,11)).Atoi();
		list <Int_t> listOfAPVs = fMapping->GetAPVIDListFromFECID(fecID) ;
		list <Int_t>::const_iterator apv_itr ;
		for (apv_itr = listOfAPVs.begin(); apv_itr != listOfAPVs.end(); ++ apv_itr) {
		  Int_t apvID = *apv_itr ;
		  TString histoName = fMapping->GetAPVFromID(apvID) ;
		  /// printf("  SRSUI.cxx::MySubscribe() => histo=%s\n", histoName.Data()) ;
		  std::stringstream stringStream;
		  stringStream.str("\r") ;
		  stringStream << "SRS01/" << histoName ;
		  Subscribe(stringStream.str().c_str()) ;
		  stringStream.str("\r") ;
		}
	      }
	    }
	  }
	}

	else {
	  std::ostringstream stringStream;
	  pair<multimap<TString, TString>::iterator, multimap<TString, TString>::iterator> setOfHistos ;

	  //=== Loop over the data types      	      if( (fRunType != "PHYSICS") && (fHMan->GetVarType(obj->GetName()) != "RAWDATA")) continue ;

	  map<TString, Int_t>::const_iterator rowMap_it ;
	  for (rowMap_it = fRowMap.begin(); rowMap_it != fRowMap.end(); ++rowMap_it) {
	    TString canvasName = rowMap_it->first ;
	    if ( (canvasName == "RAWDATA_ALLAPVs") || (canvasName.Contains("RAWDATA_FEC"))) continue ;

	    //=== Get the set of histos associated to a given data type
	    setOfHistos = fHistoNameMap.equal_range(canvasName) ;
	    multimap<TString, TString>::iterator histoName_it ;

	    //==  Loop over the histos for a given data type
	    for (histoName_it = setOfHistos.first; histoName_it != setOfHistos.second; ++histoName_it) {

	      TString histoName = ((TString) histoName_it->second) ;
	      //	      printf("  SRSUI.cxx::MySubscribe() => histo=%s\n", histoName.Data()) ;
	      stringStream.str("\r") ;
	      stringStream << "SRS01/" << histoName ;
	      Subscribe(stringStream.str().c_str()) ;
	      stringStream.str("\r") ;
	    }
	  }
	}
      }

      //===========================================================================================================
      void SRSUI::Update1() { 

	RootDisplaySetup(32)  ;

	if(fRunType == "RAWDATA") {
	  gStyle->SetOptStat(0);

	  Int_t icanvas = 0 ;	    
	  map<TString, Int_t>::const_iterator rowMap_it ;
	  for (rowMap_it = fRowMap.begin(); rowMap_it != fRowMap.end(); ++rowMap_it) {
	    TString canvasName = rowMap_it->first ;

	    if (canvasName == "RAWDATA_ALLAPVs") {
	      //	    if (canvasName == "RAWDATA_ALLAPVs") {
	      Int_t nbOfAPVs = fMapping->GetNbOfAPVs() ;
	      Int_t nbOfCols = fColMap[canvasName] ;
	      Int_t nbOfRows = fRowMap[canvasName] ;
	      Int_t length   = fLengthMap[canvasName] ;
	      Int_t width    = fWidthMap[canvasName] ;
	      Int_t nbOfTabs = nbOfCols * nbOfRows ;

	      map <Int_t, TString> listOfAPVs = fMapping->GetAPVFromIDMap() ;
	      map <Int_t, TString>::const_iterator apv_itr ;
	      Int_t apv_number = 0 ;

	      for (apv_itr = listOfAPVs.begin(); apv_itr != listOfAPVs.end(); ++ apv_itr) {
		Int_t apvID = (*apv_itr).first;
		TString histoName = fMapping->GetAPVFromID(apvID) ;
		std::stringstream stringStream ;
		stringStream.str("\r") ;
		stringStream << "SRS01/" << histoName ;
		amore::core::MOH1F* myhisto = gSubscriber->At<amore::core::MOH1F>(stringStream.str().c_str()) ;
		Int_t currentCanvasNo = (Int_t) (apv_number / nbOfTabs) ;
		Int_t currentTabNo = (apv_number % nbOfTabs) + 1;
		fEC[currentCanvasNo]->GetCanvas()->cd(currentTabNo) ;
		if (myhisto) myhisto->Draw() ;
		fEC[currentCanvasNo]->GetCanvas()->Update() ;
		apv_number++ ;
	      }
	    }

	    else if (canvasName == "RAWDATA_ALLPLANES") {
	      //	      printf("  SRSUI.cxx::Update1() => canvas name = %s\n", canvasName.Data()) ;
	      Int_t nbOfPlanes = fMapping->GetNbOfDetectorPlane() ;
	      Int_t nbOfCols   = fColMap[canvasName] ;
	      Int_t nbOfRows   = fRowMap[canvasName] ;
	      Int_t length     = fLengthMap[canvasName] ;
	      Int_t width      = fWidthMap[canvasName] ;
	      Int_t nbOfTabs   = nbOfCols * nbOfRows ;

	      map <TString, Float_t> listOfPlanes = fMapping->GetPlaneIDFromPlaneMap() ;
	      map <TString, Float_t>::const_iterator plane_itr ;
	      Int_t plane_number = 0 ;

	      for (plane_itr = listOfPlanes.begin(); plane_itr != listOfPlanes.end(); ++ plane_itr) {
		TString histoName = (*plane_itr).first ;
		//		printf("  SRSUI.cxx::Update1() => histo name = %s\n",histoName.Data()) ;

		std::stringstream stringStream;
		stringStream.str("\r") ;
		stringStream << "SRS01/" << histoName ;
		amore::core::MOH1F* myhisto = gSubscriber->At<amore::core::MOH1F>(stringStream.str().c_str()) ;

		Int_t currentCanvasNo = (Int_t) (plane_number / nbOfTabs) ;
		Int_t currentTabNo = (plane_number % nbOfTabs) + 1;
		fEC[currentCanvasNo]->GetCanvas()->cd(currentTabNo) ;
		if (myhisto)  myhisto->Draw() ;
		fEC[currentCanvasNo]->GetCanvas()->Update() ;
		plane_number++ ;
		stringStream.str("\r") ;
	      }
	    }

	    else {

	      if (canvasName.Contains("RAWDATA_FEC")) {
		TString canvasname = canvasName ;

		Int_t fecID = (canvasname.Remove(0,11)).Atoi();
		Int_t nbOfCols = fColMap[canvasName] ;
		Int_t nbOfRows = fRowMap[canvasName] ;
		Int_t length   = fLengthMap[canvasName] ;
		Int_t width    = fWidthMap[canvasName] ;

		list <Int_t> listOfAPVs = fMapping->GetAPVIDListFromFECID(fecID) ;
		list <Int_t>::const_iterator apv_itr ;

		Int_t nbOfAPVs = listOfAPVs.size() ;
		Int_t nbOfTabs = nbOfCols * nbOfRows ;

		Int_t apv_number = 0 ;
		for (apv_itr = listOfAPVs.begin(); apv_itr != listOfAPVs.end(); ++ apv_itr) {

		  Int_t apvID = *apv_itr ;
		  TString histoName = fMapping->GetAPVFromID(apvID) ;
		  std::stringstream stringStream ;
		  stringStream.str("\r") ;
		  stringStream << "SRS01/" << histoName ;
		  amore::core::MOH1F* myhisto = gSubscriber->At<amore::core::MOH1F>(stringStream.str().c_str()) ;
		  Int_t adcChannelNo = fMapping->GetADCChannelFromAPVID(apvID) ;
		  Int_t currentTabNo = (adcChannelNo %  nbOfTabs) + 1 ;

		  fEC[icanvas]->GetCanvas()->cd(currentTabNo) ;
		  if (myhisto) myhisto->Draw() ;
		  fEC[icanvas]->GetCanvas()->Update() ;
		  apv_number++ ;
		}
		icanvas++ ;	
	      }
	    }
	  }
	}

	else { // NON RAW DATA DISPLAY
	  pair<multimap<TString, TString>::iterator, multimap<TString, TString>::iterator> setOfHistos ;

	  //=== Loop over the data types
	  Int_t canvas_id = 0 ;
	  map<TString, Int_t>::const_iterator rowMap_it ;
	  for (rowMap_it = fRowMap.begin(); rowMap_it != fRowMap.end(); ++rowMap_it) {
	    TString canvasName = rowMap_it->first ;
	    if ( (canvasName == "RAWDATA_ALLAPVs") || (canvasName.Contains("RAWDATA_FEC"))) continue ;

	    setOfHistos = fHistoNameMap.equal_range(canvasName) ;
	    multimap<TString, TString>::iterator histoName_it ;

	    Int_t itab = 1 ;
	    std::ostringstream stringStream ;
	    for (histoName_it = setOfHistos.first; histoName_it != setOfHistos.second; ++histoName_it) {
	      fEC[canvas_id]->GetCanvas()->cd(itab) ;
	      stringStream.str("\r") ;
	      TString histoName = ((TString) histoName_it->second) ;
	      stringStream << "SRS01/" << histoName ;

	      if (histoName.Contains("2D")) {
		amore::core::MOH2F* my2dhisto = gSubscriber->At<amore::core::MOH2F>(stringStream.str().c_str()) ;
		if (histoName.Contains("LogZ")) gPad->SetLogz(kTRUE)  ;
		if (my2dhisto) {
		  gStyle->SetOptStat(0);
		  if (histoName.Contains("LEGO"))      my2dhisto->Draw("lego2") ;
		  else if (histoName.Contains("SURF")) my2dhisto->Draw("surf2") ;
		  else if (histoName.Contains("COLZ")) my2dhisto->Draw("colz") ;
		  else if (histoName.Contains("CONT")) my2dhisto->Draw("cont4") ;
		  else 		                       my2dhisto->Draw() ;
		}
	      }

	      else if (histoName == "eventDisplayNtuple") {
		amore::core::MOTObj* myObject = gSubscriber->At<amore::core::MOTObj>(stringStream.str().c_str()) ;
		if (myObject) {
		  RootDisplaySetup(7) ; 
		  TNtuple * myNtuple = (TNtuple *) myObject->Object() ;
		  myNtuple->SetMarkerStyle(20) ;
		  myNtuple->SetMarkerSize(0.15) ;
		  myNtuple->Draw("y:x:z:parameter","","colz") ;
		  myNtuple->UseCurrentStyle() ;
		}
	      }
 
	      else {
	      amore::core::MOH1F* myhisto = gSubscriber->At<amore::core::MOH1F>(stringStream.str().c_str()) ;
	      if (myhisto) myhisto->Draw() ;
	      }

	      itab++ ;
	    }
	    fEC[canvas_id]->GetCanvas()->Update() ;
	    canvas_id++ ;
	  }
	}
      }

      //===========================================================================================================
      void SRSUI::ClearDisplayParameters() {
	fRowMap.clear();
	fColMap.clear();
	fWidthMap.clear();
	fLengthMap.clear();
	fHistoNameMap.clear();
      }

      //===========================================================================================================
      Bool_t SRSUI::SetDisplayParameters() {
	ClearDisplayParameters() ;

        fCFGFile = TString("$AMORESRS/configFileDir/amore.cfg") ;
        fSRSConf = new SRSConfiguration(fCFGFile.Data()) ;
        fRunType    = fSRSConf->GetRunType() ;

	fMapping = SRSMapping::GetInstance() ;
	fMapping->LoadDefaultMapping(fSRSConf->GetMappingFile()) ;

	const char * displayCfg = fSRSConf->GetDisplayCfgName() ;
        printf("SRSUI::SetDisplayParameters()->Loading Display from  %s \n", displayCfg) ;

	ifstream filestream (gSystem->ExpandPathName(displayCfg), ifstream::in) ;
        if(!filestream.is_open()) {
          printf("SRSUI::SetDisplayParameters() ==> CAN'T OPEN THE FILE  \n")  ;
          return kFALSE ;
        }

	TString line ;
	while (line.ReadLine(filestream)) {

	  line.Remove(TString::kLeading, ' ');     // strip leading spaces
	  if (line.BeginsWith("#")) continue;      // and skip comments
	  TObjArray * tokens = line.Tokenize(","); // Create an array of tokens separated by "," in the line; lines without "," are skipped
	  TIter myiter(tokens);                    // iterator on the tokens array

	  while (TObjString * st = (TObjString*) myiter.Next()) {
	    TString s = st->GetString().Remove(TString::kLeading, ' ' );
	    s.Remove(TString::kTrailing, ' ' );

	    if (s == "CANVAS") {
              TString canvasName = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' ) ;
	      Int_t col = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' ).Atoi();
	      Int_t row = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' ).Atoi();
	      Int_t length = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' ).Atoi();
	      Int_t width = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' ).Atoi();

	      fRowMap[canvasName] = row ;
	      fColMap[canvasName] = col ;
	      fLengthMap[canvasName] = length ;
	      fWidthMap[canvasName] = width ;
	      //	      printf("SRSUI::SetDisplayParameters() ==> Canvas=%s, nbOfCols=%d, nbOfRows=%d, canvasLength=%d, canvasWidth=%d \n",canvasName.Data(), col, row, length, width )  ;
	    }

	    else {
	      TString canvasName = s ; 
	      TString histoName  = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' ) ;
	      fHistoNameMap.insert(pair<TString, TString>(canvasName, histoName))  ;
	    }
	  }
	  tokens->Delete();
	}
	return kTRUE ;
      } 
            
      //===========================================================================================================
      void SRSUI::RootDisplaySetup(Int_t ncount) { 
	gROOT->Reset() ;
	gStyle->Reset() ;
	gStyle->SetCanvasColor(0) ;
	gStyle->SetCanvasBorderMode(0) ;

	gStyle->SetLabelFont(62,"xyz");
	gStyle->SetLabelSize(0.03,"xyz");
	gStyle->SetLabelColor(1,"xyz");
	gStyle->SetTitleSize(0.075,"xyz");
	gStyle->SetTitleOffset(4.5,"xyz");
	gStyle->SetPalette(1);

	const Int_t NRGBs = 5;
	const Int_t NCont = ncount ;

	Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
	Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
	Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
	Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
	TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
	gStyle->SetNumberContours(NCont);

      }

      //===========================================================================================================
      void SRSUI::Process() {
      }

      //===========================================================================================================
      void SRSUI::StartOfCycle() {
      }

      //===========================================================================================================
      void SRSUI::EndOfCycle() {
      }

      //===========================================================================================================
      bool SRSUI::StartAuto()  {
       // return true if the module should automatically Init and Start, false if we wait for
       // the user to click on the buttons. If we return true here, then InitAuto() isn't taken into account.
       return true;  // this is the default value
      }

      //===========================================================================================================
      bool SRSUI::InitAuto()  {
        // return true if the module should automatically Init, false if we wait for
        // the user to click on the button. If StartAuto returns true, then this method is not taken into account.
	return true; // this is the default value
      }
    }
  }
}
