include ../Makefile.inc

CLEANFILES = *.$(ObjSuf) *.$(DllSuf) *Dict.* *~ core
noinst_HEADERS = SRSSubscriberLinkDef.h
pkginclude_HEADERS = SRSSubscriber.h
BUILT_SOURCES = SRSSubscriberDict.h SRSSubscriberDict.cxx

bin_PROGRAMS = libAmoreSRSSubscriber.$(DllSuf)
libAmoreSRSSubscriber_CPPFLAGS = $(CPPFLAGS) -I../common
libAmoreSRSSubscriber_CXXFLAGS = $(CXXFLAGS)
libAmoreSRSSubscriber_LIBS = $(LIBS) -lAmoreSRSCommon
libAmoreSRSSubscriber_LDFLAGS = $(LDFLAGS) -L../common -L$(AMORE_SITE)/lib -shared -Wl,-soname,$(bin_PROGRAMS) -o $(bin_PROGRAMS)
libAmoreSRSSubscriber_SOURCES = SRSSubscriberDict.cxx SRSSubscriber.cxx
libAmoreSRSSubscriber_OBJECTS = SRSSubscriberDict.$(ObjSuf) $(patsubst %.$(SrcSuf),%.$(ObjSuf),$(wildcard *.$(SrcSuf)))

# NEW
DESTDIR = ${AMORE_SITE}

%.$(ObjSuf):	%.$(SrcSuf) %.$(HdrSuf)
	@echo $<
	@$(CXX) $(INCLUDES) $(libAmoreSRSSubscriber_CPPFLAGS) $(libAmoreSRSSubscriber_CXXFLAGS) -c $<

default: all

all: $(bin_PROGRAMS)

$(bin_PROGRAMS): $(libAmoreSRSSubscriber_OBJECTS)
	@$(LD) $(libAmoreSRSSubscriber_LDFLAGS) $(libAmoreSRSSubscriber_LIBS) $^
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
	@rootcint -f $*Dict.$(SrcSuf) -c -p $(INCLUDES) $(libAmoreSRSSubscriber_CPPFLAGS) $(libAmoreSRSSubscriber_CXXFLAGS) $(pkginclude_HEADERS) $*LinkDef.h
