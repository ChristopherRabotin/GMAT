# Standard settings recommended by GNU Coding Standards
SHELL = /bin/sh
.SUFFIXES:

# Customization
VERSION = R2013a

#
# Standard targets
#

all: installer-internal installer-public zip-internal zip-public

clean:
	rm -fr gmat-internal gmat-public *.exe *.zip uninstall.nsh

distclean: clean

mostlyclean: clean

maintainer-clean: clean

#
# Public targets
#

assemble: assemble-internal assemble-public

assemble-internal: gmat-internal/GMAT/bin/GMAT.exe

assemble-public: gmat-public/GMAT/bin/GMAT.exe

installer: installer-internal installer-public

installer-internal: gmat.nsi assemble-internal
	./genfilelists.sh gmat-internal/GMAT
	cmd /c "makensis /DVERSION=$(VERSION) $<"

installer-public: gmat.nsi assemble-public
	./genfilelists.sh gmat-public/GMAT
	cmd /c "makensis /DVERSION=$(VERSION) /DPUBLIC $<"

zip: zip-internal zip-public

zip-internal: gmat-internal-win-i586-$(VERSION).zip

zip-public: gmat-win-i586-$(VERSION).zip

#
# Internal targets
#

gmat-internal-win-i586-$(VERSION).zip: gmat-internal/GMAT/bin/GMAT.exe
	rm -f $@
	cd gmat-internal \
	    && zip -r9 ../$@ GMAT

gmat-win-i586-$(VERSION).zip: gmat-public/GMAT/bin/GMAT.exe
	rm -f $@
	cd gmat-public \
	    && zip -r9 ../$@ GMAT

gmat-internal/GMAT/bin/GMAT.exe:
	../assemblegmat.sh -d gmat-internal/GMAT -t full-release

gmat-public/GMAT/bin/GMAT.exe:
	../assemblegmat.sh -d gmat-public/GMAT -t public-release
