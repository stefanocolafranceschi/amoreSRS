include ../Makefile.inc

CLEANFILES = *.$(ObjSuf) *.$(DllSuf) *Dict.* *~ core
noinst_HEADERS = SRSUILinkDef.h
pkginclude_HEADERS = SRSUI.h SRSUI2.h SRSUI3.h SRSUI.h SRSUI2.h SRSUI3.h SRSUI4.h SRSUI5.h SRSUI6.h SRSUI7.h SRSUI8.h SRSUI9.h
BUILT_SOURCES = SRSUIDict.h SRSUIDict.cxx 

bin_PROGRAMS = libAmoreSRSUI.$(DllSuf)
libAmoreSRSUI_CPPFLAGS = $(CPPFLAGS) -I../common
libAmoreSRSUI_CXXFLAGS = $(CXXFLAGS)
libAmoreSRSUI_LIBS = $(LIBS) -lAmoreSRSCommon
libAmoreSRSUI_LDFLAGS = $(LDFLAGS) -L../common -L$(AMORE_SITE)/lib -shared -Wl,-soname,$(bin_PROGRAMS) -o $(bin_PROGRAMS)
libAmoreSRSUI_SOURCES = SRSUIDict.cxx SRSUI.cxx SRSUI2.cxx  SRSUI3.cxx SRSUI4.cxx SRSUI5.cxx  SRSUI6.cxx SRSUI7.cxx SRSUI8.cxx  SRSUI9.cxx
libAmoreSRSUI_OBJECTS = SRSUIDict.$(ObjSuf) $(patsubst %.$(SrcSuf),%.$(ObjSuf),$(wildcard *.$(SrcSuf)))

# NEW
DESTDIR = ${AMORE_SITE}

%.$(ObjSuf):	%.$(SrcSuf) %.$(HdrSuf)
	@echo $<
	@$(CXX) $(INCLUDES) $(libAmoreSRSUI_CPPFLAGS) $(libAmoreSRSUI_CXXFLAGS) -c $<

default: all

all: $(bin_PROGRAMS)

$(bin_PROGRAMS): $(libAmoreSRSUI_OBJECTS)
	@$(LD) $(libAmoreSRSUI_LDFLAGS) $(libAmoreSRSUI_LIBS) $^
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
	@rootcint -f $*Dict.$(SrcSuf) -c -p $(INCLUDES) $(libAmoreSRSUI_CPPFLAGS) $(libAmoreSRSUI_CXXFLAGS) $(pkginclude_HEADERS) $*LinkDef.h
