#!/bin/sh

# Build the GMAT NSIS installer.
#
# Requires makensis.exe in the user's path.

rm -fr gmat

../assemblegmat.sh win 2011-04-18

./genfilelists.sh

makensis gmat.nsi