#!/bin/sh

# Build the GMAT NSIS installer.
#
# Requires makensis.exe in the user's path.

rm -fr gmat
if [ $? != 0 ]; then echo "Failed to delete gmat directory"; exit 1; fi

../assemblegmat.sh -d gmat -t public
if [ $? != 0 ]; then echo "../assemblegmat.sh failed to run"; exit 1; fi

./genfilelists.sh
if [ $? != 0 ]; then echo "genfilelists.sh failed to run"; exit 1; fi

makensis gmat.nsi
if [ $? != 0 ]; then echo "makensis failed"; exit 1; fi
