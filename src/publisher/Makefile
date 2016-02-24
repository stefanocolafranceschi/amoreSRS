# K. GNANVO : sep 2010 SRS DAQ Commissioning
include ../Makefile.inc

CLEANFILES = *.$(ObjSuf) *.$(DllSuf) *Dict.* *~ core
noinst_HEADERS = SRSPublisherLinkDef.h
pkginclude_HEADERS = SRSAPVEvent.h SRSHit.h SRSAPVSignalFit.h SRSTrackFit.h SRSCluster.h SRSEventBuilder.h SRSFECEventDecoder.h SRSFECPedestalDecoder.h SRSHistoManager.h SRSPedestal.h SRSRawPedestal.h SRSPublisher.h SRSTrack.h SRSAlignment.h SRSOutputROOT.h SRSDoubleGaussianFit.h SRSPositionCorrection.h
BUILT_SOURCES = SRSPublisherDict.h SRSPublisherDict.cxx

bin_PROGRAMS = libAmoreSRSPublisher.$(DllSuf)
libAmoreSRSPublisher_CPPFLAGS = $(CPPFLAGS) -DDEBUG -I../common -I$(ALICE_ROOT)/include -I${ALICE_ROOT}/RAW
libAmoreSRSPublisher_CXXFLAGS = $(CXXFLAGS)
libAmoreSRSPublisher_LIBS = $(LIBS) -lAmoreSRSCommon -lVMC -lGeom -lEG -lProof
libAmoreSRSPublisher_LDFLAGS = $(LDFLAGS) -L../common -L$(AMORE_SITE)/lib -shared -Wl,-soname,$(bin_PROGRAMS) -o $(bin_PROGRAMS) -L$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)
libAmoreSRSPublisher_SOURCES = SRSCluster.cxx SRSAPVEvent.cxx SRSHit.cxx SRSTrackFit.cxx SRSAPVSignalFit.cxx SRSEventBuilder.cxx SRSFECEventDecoder.cxx SRSFECPedestalDecoder.cxx SRSHistoManager.cxx SRSPedestal.cxx SRSRawPedestal.cxx SRSPublisher.cxx SRSPublisherDict.cxx SRSTrack.cxx SRSAlignment.cxx SRSOutputROOT.cxx SRSDoubleGaussianFit.cxx SRSPositionCorrection.cxx
libAmoreSRSPublisher_OBJECTS = SRSPublisherDict.$(ObjSuf) $(patsubst %.$(SrcSuf),%.$(ObjSuf),$(wildcard *.$(SrcSuf)))
CINTFLAGS := $(CINTFLAGS)  $(DATECFLAGS)
DICT = libAmoreSRSPublisherDict

# NEW
DESTDIR = ${AMORE_SITE}

%.$(ObjSuf):	%.$(SrcSuf) %.$(HdrSuf)
	@echo $<
	@$(CXX) -g $(INCLUDES) $(libAmoreSRSPublisher_CPPFLAGS) $(libAmoreSRSPublisher_CXXFLAGS) -c $<

default: all

$(DICT).cxx: $(pkginclude_HEADERS) LinkDef.h
	@rootcint  -f $@ -c $(CINTFLAGS) $^

$(DICT).o: $(DICT).cxx
	@$(CXX) $(libAmoreSRSPublisher_CXXFLAGS) -c $<

all: $(bin_PROGRAMS)

$(bin_PROGRAMS): $(libAmoreSRSPublisher_OBJECTS)
	@echo $^
	@$(LD) $(libAmoreSRSPublisher_LDFLAGS) $(libAmoreSRSPublisher_LIBS) $^
#$(OutPutOpt)$@

install: all
# CHANGED
	@cp $(bin_PROGRAMS) ${DESTDIR}/lib; 

uninstall:
	@rm -rf ${AMORE_SITE}/lib/$(bin_PROGRAMS);

clean:
	@rm -rf $(CLEANFILES)

%Dict.$(SrcSuf) %Dict.$(HdrSuf): $(pkginclude_HEADERS)
	@echo Creating dictionary...
	@rootcint -f $*Dict.$(SrcSuf) -c -p $(INCLUDES) $(libAmoreSRSPublisher_CPPFLAGS) $(libAmoreSRSPublisher_CXXFLAGS) $(pkginclude_HEADERS) $*LinkDef.h
