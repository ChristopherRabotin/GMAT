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
	cd ../plugins; make all; \

rebuild: 
	cd src; make -f MakeGmat.eclipse clean; make -f MakeGmat.eclipse all; \
	cd ../plugins; make rebuild; \


clean:
	cd src; make -f MakeGmat.eclipse clean; \
	cd ../plugins; make clean; \

else

all: 
	cd src; make -f MakeGmat.eclipse all BUILD_64BIT=$(BUILD_64BIT); \
	cd ../plugins; make all BUILD_64BIT=$(BUILD_64BIT); \

rebuild: 
	cd src; make -f MakeGmat.eclipse clean BUILD_64BIT=$(BUILD_64BIT); \
	make -f MakeGmat.eclipse all BUILD_64BIT=$(BUILD_64BIT); \
	cd ../plugins; make rebuild BUILD_64BIT=$(BUILD_64BIT); \

clean:
	cd src; make -f MakeGmat.eclipse clean BUILD_64BIT=$(BUILD_64BIT); \
	cd ../plugins; make clean BUILD_64BIT=$(BUILD_64BIT); \

endif

endif