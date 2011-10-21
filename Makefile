# $Id$
# Root level makefile 
#
# This file is really just a convenience.  In eclipse, it's useful to have a
# root level file that calls into the source tree.

BUILD_64BIT = 0

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
	cd ../../plugins/FminconOptimizerPlugin; make all; \
	cd ../../plugins/EstimationPlugin; make all; \
	cd ../../plugins/CInterfacePlugin; make all;
	
rebuild: 
	cd src; make -f MakeGmat.eclipse clean; make -f MakeGmat.eclipse all; \
	cd ../plugins/MatlabInterfacePlugin; make rebuild; \
	cd ../../plugins/FminconOptimizerPlugin; make rebuild; \
	cd ../../plugins/EstimationPlugin; make rebuild; \
	cd ../../plugins/CInterfacePlugin; make rebuild;

clean:
	cd src; make -f MakeGmat.eclipse clean; \
	cd ../plugins/MatlabInterfacePlugin; make clean; \
	cd ../../plugins/FminconOptimizerPlugin; make clean; \
	cd ../../plugins/EstimationPlugin; make rebuild; \
	cd ../../plugins/CInterfacePlugin; make rebuild;

else

all: 
	cd src; make -f MakeGmat.eclipse all BUILD_64BIT=$(BUILD_64BIT); \
	cd ../plugins/MatlabInterfacePlugin; make all BUILD_64BIT=$(BUILD_64BIT); \
	cd ../../plugins/EstimationPlugin; make all BUILD_64BIT=$(BUILD_64BIT); \
	cd ../../plugins/CInterfacePlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	
rebuild: 
	cd src; make -f MakeGmat.eclipse clean BUILD_64BIT=$(BUILD_64BIT); make -f MakeGmat.eclipse all BUILD_64BIT=$(BUILD_64BIT); \
	cd ../plugins/MatlabInterfacePlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT); \
	cd ../../plugins/EstimationPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT); \
	cd ../../plugins/CInterfacePlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	
clean:
	cd src; make -f MakeGmat.eclipse clean BUILD_64BIT=$(BUILD_64BIT); \
	cd ../plugins/MatlabInterfacePlugin; make clean BUILD_64BIT=$(BUILD_64BIT); \
	cd ../../plugins/EstimationPlugin; make clean BUILD_64BIT=$(BUILD_64BIT); \
	cd ../../plugins/CInterfacePlugin; make clean BUILD_64BIT=$(BUILD_64BIT);

endif

endif