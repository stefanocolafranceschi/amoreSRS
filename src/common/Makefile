include ../Makefile.inc

CLEANFILES = *.$(ObjSuf) *.$(DllSuf) *Dict.* *~ core
noinst_HEADERS = SRSCommonLinkDef.h
pkginclude_HEADERS = Common.h SRSCommon.h SRSMapping.h SRSConfiguration.h 
BUILT_SOURCES = SRSCommonDict.h SRSCommonDict.cxx

bin_PROGRAMS = libAmoreSRSCommon.$(DllSuf)
libAmoreSRSCommon_CPPFLAGS = $(CPPFLAGS)
libAmoreSRSCommon_CXXFLAGS = $(CXXFLAGS)
libAmoreSRSCommon_LIBS = $(LIBS)
libAmoreSRSCommon_LDFLAGS = $(LDFLAGS) -shared -Wl,-soname,$(bin_PROGRAMS) -o $(bin_PROGRAMS)
libAmoreSRSCommon_SOURCES = SRSCommonDict.cxx Common.cxx SRSCommon.cxx  SRSMapping.cxx SRSConfiguration.cxx 
libAmoreSRSCommon_OBJECTS = SRSCommonDict.$(ObjSuf) $(patsubst %.$(SrcSuf),%.$(ObjSuf),$(wildcard *.$(SrcSuf)))

# NEW
DESTDIR = ${AMORE_SITE}

%.$(ObjSuf):	%.$(SrcSuf) %.$(HdrSuf)
	@echo $<
	@$(CXX) $(INCLUDES) $(libAmoreSRSCommon_CPPFLAGS) $(libAmoreSRSCommon_CXXFLAGS) -c $<

default: all

all: $(bin_PROGRAMS)

$(bin_PROGRAMS): $(libAmoreSRSCommon_OBJECTS)
	@$(LD) $(libAmoreSRSCommon_LDFLAGS) $(libAmoreSRSCommon_LIBS) $^
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
	@rootcint -f $*Dict.$(SrcSuf) -c -p $(INCLUDES) $(libAmoreSRSCommon_CPPFLAGS) $(libAmoreSRSCommon_CXXFLAGS) $(pkginclude_HEADERS) $*LinkDef.h


