#!/bin/sh

# This script assembles a complete executable copy of GMAT from original
# locations. See /build/install/PACKAGING-MANIFEST.txt for a description of
# where files are located.
#
# Usage: assemblegmat.sh win|linux|mac

# Initializations
WINDOWS=false
MAC=false
LINUX=false

usage() {
    echo "Usage:"
    echo "  $0 win <buildname>"
    echo "  $0 lin"
    echo "  $0 mac"
}

# File sources
sfrepo='https://gmat.svn.sourceforge.net/svnroot/gmat'
apppath=$sfrepo/trunk/application

# Platform selection
if [ "$1" == 'win' ]
then
    if [ -z "$2" ]
    then
        usage
        exit 1
    else
        WINDOWS=true
        winbuildname="$2"
    fi
elif [ "$1" == 'lin' ]
then
    LINUX=true
elif [ "$1" == 'mac' ]
then
    MAC=true
else
    usage
    exit 1
fi
    
# bin, data, matlab
svn export --force $apppath gmat

# output (empty)
mkdir -p gmat/output

# Platform-dependent stuff
if $WINDOWS
then
    # File locations
    winbuildspath='//mesa-file/595/GMAT/Builds/windows'
    
    # bin (Windows)
    cp -prv \
        $winbuildspath/$winbuildname/GMAT.exe \
        $winbuildspath/$winbuildname/libGmatBase.dll \
        gmat/bin

    # bin (Windows)
    cp -prv \
        $winbuildspath/gcc_lib/wx2.8.11/*.dll \
        gmat/bin
    cp -prv \
        $winbuildspath/gcc_lib/other/*.dll \
        gmat/bin

    # plugins (Windows)
    mkdir -p gmat/plugins/proprietary
    cp -prv \
        $winbuildspath/$winbuildname/libFminconOptimizer.dll \
        $winbuildspath/$winbuildname/libGmatEstimation.dll \
        $winbuildspath/$winbuildname/libMatlabInterface.dll \
        gmat/plugins

    # Remove Windows hidden files
    find gmat -iname thumbs.db -delete
    
elif $LINUX
then
    echo 'Linux-specific files not implemented'
    
elif $MAC
then
    echo 'Mac-specific files not implemented'
fi