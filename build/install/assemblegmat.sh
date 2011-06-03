#!/bin/sh

# This script assembles a complete executable copy of GMAT from original
# locations. See /build/install/PACKAGING-MANIFEST.txt for a description of
# where files are located.
#
# Usage: $0 [option] ...
# Options:
#   -b buildname  Assemble the named build instead of the latest available
#   -l|-m|[-w]    Assemble for Linux (-l), Mac (-m), or Windows (-w, default)
#   -t type       Assemble a particular type of distribution:
#                 full: everything included (default)

# Initializations
WINDOWS=true
MAC=false
LINUX=false
TYPE=full

usage() {
cat <<END
Usage: $0 [option] ...
Options:
  -b buildname  Assemble the named build instead of the latest available
  -l|-m|[-w]    Assemble for Linux (-l), Mac (-m), or Windows (-w, default)
  -t type       Assemble a particular type of distribution:
                full: everything included (default)
END
}

# File sources
sfrepo='https://gmat.svn.sourceforge.net/svnroot/gmat'
apppath="$sfrepo/trunk/application"

# Argument handling
while getopts b:lmt:w o
do
    case "$o" in
        b) buildname="$OPTARG";;
        l) LINUX=true; MAC=false; WINDOWS=false;;
        m) LINUX=false; MAC=true; WINDOWS=false;;
        t) TYPE="$OPTARG";;
        w) LINUX=false; MAC=false; WINDOWS=true;;
        ?) usage; exit 1;;
    esac
done

# bin, data, matlab
svn export --force $apppath gmat

# output (empty)
mkdir -p gmat/output

# Platform-dependent stuff
if $WINDOWS
then
    # File locations
    winbuildspath='//mesa-file/595/GMAT/Builds/windows/VS2010_build'
    
    # Find latest build
    if [ -z "$buildname" ]
    then
        buildname=`ls -1 $winbuildspath | tail -n 1`
    fi
    
    # bin (Windows)
    cp -prv \
        $winbuildspath/$buildname/GMAT.exe \
        $winbuildspath/$buildname/libGmatBase.dll \
        gmat/bin

    # bin (Windows)
    cp -prv \
        $winbuildspath/../vc_lib/wx2.8.12/*.dll \
        gmat/bin
    cp -prv \
        $winbuildspath/../vc_lib/*.dll \
        gmat/bin

    # plugins (Windows)
    mkdir -p gmat/plugins/proprietary
    cp -prv \
        $winbuildspath/$buildname/libFminconOptimizer.dll \
        $winbuildspath/$buildname/libGmatEstimation.dll \
        $winbuildspath/$buildname/libMatlabInterface.dll \
        gmat/plugins
    
    if [ `echo "$TYPE" | tr [:upper:] [:lower:]` == 'full' ]
    then
        cp -prv \
            $winbuildspath/$buildname/libCcsdsEphemerisFile.dll \
            $winbuildspath/$buildname/libDataFile.dll \
            gmat/plugins
        cp -prv \
            $winbuildspath/$buildname/libVF13Optimizer.dll \
            gmat/plugins/proprietary
    fi

    # Remove Windows hidden files
    find gmat -iname thumbs.db -delete
    
elif $LINUX
then
    echo 'Linux-specific files not implemented'
    
elif $MAC
then
    echo 'Mac-specific files not implemented'
fi