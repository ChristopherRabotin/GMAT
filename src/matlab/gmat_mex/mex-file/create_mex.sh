#!/bin/sh
################################################################################
# $Header: /GMAT/dev/cvs/matlab/gmat_mex/mex-file/create_mex.sh,v 1.2 2006/09/08 15:29:30 wshoan Exp $
#
# Purpose:     Compiler SendGMAT.cpp to create mex-file.
#
# Author:      J. Gurganus
# Created:     2005/12/15
#
################################################################################
#
INC_FLAGS='-I../src -I. -I../../../gui/include -I../../../gui/include/bitmaps'
LIB_FLAGS='-L../lib -L../../../base/lib -lGmatGui'
#  -lMatlabClient'

mex -f gmatopts.sh  SendGMAT.cpp $INC_FLAGS $LIB_FLAGS
