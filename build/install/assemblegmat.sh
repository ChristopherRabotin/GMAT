#!/bin/sh

# This script assembles a complete executable copy of GMAT from original
# locations. See /build/install/PACKAGING-MANIFEST.txt for a description of
# where files are located.
#
# Usage: assemblegmat.sh win|linux|mac
#
# Unresolved questions:
#   Are we supplying an empty plugins/proprietary folder?

# File sources
sfrepo='https://gmat.svn.sourceforge.net/svnroot/gmat'
apppath=$sfrepo/trunk/application
winbuildspath='//mesa-file/595/GMAT/Builds/windows'
winbuildname=2011-04-15

# bin, data, matlab
svn export --force $apppath gmat

if [ $1 == 'win' ]
then
    # bin (Windows)
    cp -prv \
        $winbuildspath/$winbuildname/GMAT.exe \
        $winbuildspath/$winbuildname/libCcsdsEphemerisFile.dll \
        $winbuildspath/$winbuildname/libDataFile.dll \
        $winbuildspath/$winbuildname/libGmatBase.dll \
        gmat/bin

    # bin (Windows)
    cp -prv \
        $winbuildspath/gcc_lib/wx2.8.11/* \
        gmat/bin
    cp -prv \
        $winbuildspath/gcc_lib/other/* \
        gmat/bin

    # plugins (Windows)
    mkdir -p gmat/plugins
    cp -prv \
        $winbuildspath/$winbuildname/libFminconOptimizer.dll \
        $winbuildspath/$winbuildname/libGmatEstimation.dll \
        $winbuildspath/$winbuildname/libMatlabInterface.dll \
        gmat/plugins
fi
