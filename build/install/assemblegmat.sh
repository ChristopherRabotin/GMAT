#!/bin/sh

# This script assembles a complete executable copy of GMAT from original
# locations. See /build/install/PACKAGING-MANIFEST.txt for a description of
# where files are located.
#
# Usage: $0 [option] ...
# Options:
#   -b buildname  Assemble the named build instead of the latest available
#   -d dest       Place files in specified directory dest (default: current
#                 directory)
#   -l|-m|[-w]    Assemble for Linux (-l), Mac (-m), or Windows (-w, default)
#   -t type       Assemble a particular type of distribution:
#                 full: everything included (default)
#
# Prerequisites:
#   svn
#   standard POSIX utilities

# Initializations
WINDOWS=true
MAC=false
LINUX=false
TYPE=full
dest=.

usage() {
cat <<END
Usage: $0 [option] ...
Options:
  -b buildname  Assemble the named build instead of the latest available
  -d dest       Place files in specified directory dest (default: current
                directory)
  -l|-m|[-w]    Assemble for Linux (-l), Mac (-m), or Windows (-w, default)
  -t type       Assemble a particular type of distribution:
                full: everything included (default)
END
}

# File sources
sfrepo='https://gmat.svn.sourceforge.net/svnroot/gmat'
apppath="$sfrepo/trunk/application"

# Argument handling
while getopts b:d:lmt:w o
do
    case "$o" in
        b) buildname="$OPTARG";;
        d) dest="$OPTARG";;
        l) LINUX=true; MAC=false; WINDOWS=false;;
        m) LINUX=false; MAC=true; WINDOWS=false;;
        t) TYPE="$OPTARG";;
        w) LINUX=false; MAC=false; WINDOWS=true;;
        ?) usage; exit 1;;
    esac
done

# Platform-dependent stuff
if $WINDOWS
then
    # File locations
    winbuildspath='//mesa-file/595/GMAT/Builds/windows/VS2010_build'
    
    # Find latest build
    if [ -z "$buildname" ]
    then
        buildname=`ls -1 "$winbuildspath" | \
            grep -E '[0-9]{4}-[0-9]{2}-[0-9]{2}' | \
            tail -n 1`
    fi

    # Validate build
    if [ ! -e "$winbuildspath"/"$buildname"/bin/GMAT.exe ]
    then
        echo 'Invalid build: cannot find GMAT.exe'
        exit 2;
    fi

    # Create destination directory
    if [ ! -e "$dest" ]
    then
        mkdir -p "$dest"
    fi
    
    # Copy build files
    cp -pRv "$winbuildspath"/"$buildname"/* "$dest"

    # Copy support libraries
    cp -pRv \
        "$winbuildspath"/../vc_lib/wx2.8.12/*.dll \
        "$dest"/bin
    cp -pRv \
        "$winbuildspath"/../vc_lib/*.dll \
        "$dest"/bin

    # Remove Windows hidden files
    find "$dest" -iname thumbs.db -delete
    
elif $LINUX
then
    echo 'Linux-specific files not implemented'
    
elif $MAC
then
    echo 'Mac-specific files not implemented'
fi

# bin, data, matlab
svn export --force "$apppath" "$dest"
