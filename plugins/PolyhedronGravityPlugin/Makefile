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
#
# Flags that enable or disable specific plugins.  All standard ines are enabled
# by default.
BUILD_VF13 = 1
BUILD_DATAFIL = 1
BUILD_CCSDSEPHEM = 1
BUILD_EVENTLOCATOR = 0


all: 
	cd src; make all

rebuild: 
	cd src; make clean; make all

clean:
	cd src; make clean
	