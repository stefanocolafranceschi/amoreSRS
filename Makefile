INCLUDES = $(all_includes)
METASOURCES = AUTO

SUBDIRS = src

timestamp=$(shell date +%Y%m%d)
snapname=amoreSRS$(timestamp)
snaptgz=$(snapname).tgz

RPMDIR=~/rpm
DETDIR=$(shell pwd)
RPMVERSION=1.3

default: all

all:
	@cd src; make;

install: all
	@cd src; make install;

uninstall:
	@cd src; make uninstall;

clean:
	@cd src; make clean;

dist:   clean
	@mkdir amoreSRS
	@cp -r src AUTHORS ChangeLog Makefile README rpm.spec rpmHowTo.txt TODO VERSION configFile.txt amoreSRS 
	@find . -name ".svn" | xargs rm -r
	@tar cfz $(snaptgz) amoreSRS;
	@rm -rf amoreSRS

prepare-rpm:
	@echo creating the directory structure
	@mkdir -p ${RPMDIR}/SOURCES ${RPMDIR}/SPECS ${RPMDIR}/BUILD ${RPMDIR}/RPMS ${RPMDIR}/RPMS/i386 ${RPMDIR}/SRPMS 
	@mkdir -p ${RPMDIR}/tmp
	@echo creating the tgz and copy it as well as the spec file to RPMDIR 
	@mkdir -p ${DETDIR}/../amoreSRS-${RPMVERSION}
	@cp -rf ${DETDIR}/* ${DETDIR}/../amoreSRS-${RPMVERSION}
	@tar -zcvf ${RPMDIR}/SOURCES/amoreSRS-${RPMVERSION}.tar.gz ${DETDIR}/../amoreSRS-${RPMVERSION}/
	#@cp rpm.spec ${RPMDIR}/SPECS/
	#@rm -rf ${DETDIR}/../amoreSRS-${RPMVERSION}
	@echo preparing rpm done !

rpm:
	@echo ${RPMDIR}
	rpmbuild --define "installDir $(AMORE_SITE)" --define "RPMVERSION ${RPMVERSION}" $(EXTRA_OPTION)  -bb ${RPMDIR}/SPECS/rpm.spec 

version: # substitution of the version number in the module publisher and in the VERSION file
	@rm VERSION
	@echo ${RPMVERSION} > VERSION
	@echo file VERSION updated !
	@cd src/publisher;\
	for file in *.cxx; do \
		echo Updating version number in file $$file; \
		sed 's/$$MAKEFILE_VERSION/${RPMVERSION}/' <$$file >temp.cxx;  \
		mv temp.cxx $$file; \
	done
