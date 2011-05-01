# $Id$
# Root level makefile 
#
# This file is really just a convenience.  In eclipse, it's useful to have a
# root level file that calls into the source tree.

ifeq ($(PLATFORM), )
PLATFORM = windows
endif

ifeq ($(PLATFORM), nomatlab)

all: 
	cd src; make -f MakeGmat.eclipse all; \

rebuild: 
	cd src; make -f MakeGmat.eclipse clean; make -f MakeGmat.eclipse all; \

clean:
	cd src; make -f MakeGmat.eclipse clean; \

else

ifeq ($(PLATFORM), windows)

all: 
	cd src; make -f MakeGmat.eclipse all; \
	cd ../plugins/MatlabInterfacePlugin; make all; \
	cd ../../plugins/FminconOptimizerPlugin; make all;

rebuild: 
	cd src; make -f MakeGmat.eclipse clean; make -f MakeGmat.eclipse all; \
	cd ../plugins/MatlabInterfacePlugin; make rebuild; \
	cd ../../plugins/FminconOptimizerPlugin; make rebuild;

clean:
	cd src; make -f MakeGmat.eclipse clean; \
	cd ../plugins/MatlabInterfacePlugin; make clean; \
	cd ../../plugins/FminconOptimizerPlugin; make clean;

else

all: 
	cd src; make -f MakeGmat.eclipse all; \
	cd ../plugins/MatlabInterfacePlugin; make all; \

rebuild: 
	cd src; make -f MakeGmat.eclipse clean; make -f MakeGmat.eclipse all; \
	cd ../plugins/MatlabInterfacePlugin; make rebuild; \

clean:
	cd src; make -f MakeGmat.eclipse clean; \
	cd ../plugins/MatlabInterfacePlugin; make clean; \

endif

endif