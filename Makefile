# $Id$
#
# GMAT: General Mission Analysis Tool.
# 
#  Copyright (c) 2002-2014 United States Government as represented by the
#  Administrator of The National Aeronautics and Space Administration.
#  All Other Rights Reserved.
#
# Root level makefile 
#
# This file is really just a convenience.  In eclipse, it's useful to have a
# root level file that calls into the source tree.

BUILD_64BIT = 1
BUILD_THREADS=1

ifeq ($(PLATFORM), )
PLATFORM = windows
endif

ifeq ($(PLATFORM), nomatlab)

all: 
	cd src; make -f MakeGmat.eclipse all; \
	cd ../plugins; make all; \

rebuild: 
	cd src; make -f MakeGmat.eclipse clean; make -f MakeGmat.eclipse all; \
	cd ../plugins; make clean all; \

clean:
	cd src; make -f MakeGmat.eclipse clean; \
	cd ../plugins; make clean; \

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