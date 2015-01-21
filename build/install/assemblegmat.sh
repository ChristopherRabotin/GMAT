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
#   -p pw         Use repository password pw (default: prompt) 
#   -t type       Assemble a particular type of distribution:
#                 full: everything included (default)
#                 public: publicly-releasable version
#                 full-release: full excluding pre-release items
#                 public-release: public excluding pre-release items
#   -u user       Use repository username user (default: prompt)
#
# Prerequisites:
#   standard POSIX utilities

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
  -d dest       Place files in specified directory dest (default: gmat-<date>)
  -l|-m|[-w]    Assemble for Linux (-l), Mac (-m), or Windows (-w, default)
  -p pw         Use repository password pw (default: prompt) 
  -t type       Assemble a particular type of distribution:
                full: everything included (default)
                public: publicly-releasable version
                full-release: full excluding pre-release items
                public-release: public excluding pre-release items
  -u user       Use repository username user (default: prompt)
END
}

# Argument handling
while getopts b:d:lmp:t:u:w o
do
    case "$o" in
        b) buildname="$OPTARG";;
        d) dest="$OPTARG";;
        l) LINUX=true; MAC=false; WINDOWS=false;;
        m) LINUX=false; MAC=true; WINDOWS=false;;
        p) pw="$OPTARG";;
        t) TYPE=`echo "$OPTARG" | tr '[:upper:]' '[:lower:]'`;;
        u) user="${OPTARG}@";;
        w) LINUX=false; MAC=false; WINDOWS=true;;
        ?) usage; exit 1;;
    esac
done

# File sources
gmatrepo="ssh://${user}gs-mesajade.gsfc.nasa.gov/home/GMAT/git/gmat.git"
internalrepo="ssh://${user}gs-mesajade.gsfc.nasa.gov/home/GMAT/git/gmatinternal.git"

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
    exepath="$winbuildspath/$buildname/bin/GMAT.exe"
    if [ ! -e "$exepath" ]
    then
        echo "Invalid build: cannot find $exepath"
        exit 2;
    fi

    # Create destination directory
    if [ -z "$dest" ]
    then
        dest="gmat-$buildname"
    fi
    if [ ! -e "$dest" ]
    then
        mkdir -p "$dest"
    fi
    
    # Copy build files
    cp -pRv "$winbuildspath"/"$buildname"/* "$dest"

    # Copy support libraries
    cp -pRv \
        "$winbuildspath"/../vc_lib/wx3.0.2/*.dll \
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
git archive -v --format=tar --remote="$gmatrepo" HEAD:application | tar -x -C "$dest"

# remove debug directory
rm -rf "$dest/debug"

# Mars-GRAM 2005 data
if [ $TYPE = 'full' -o $TYPE = 'full-release' ]
then
    mgdatadir="$dest/data/atmosphere/MarsGRAM2005"
    mkdir -p "$mgdatadir"
    git archive -v --format=tar --remote="$internalrepo" HEAD:code/MarsGRAMPlugin/data/MarsGRAM2005 | tar -x -C "$mgdatadir"
fi

# libCInterface MATLAB files
git archive -v --format=tar --remote="$gmatrepo" HEAD:plugins/CInterfacePlugin/matlab | tar -x -C "$dest/matlab/libCInterface"

# Remove proprietary plugins if necessary
if [ $TYPE = 'public' -o $TYPE = 'public-release' ]
then
    rm -rf "$dest"/plugins/proprietary/*
    sed -i '/plugins\/proprietary/s/^\( *PLUGIN\) /#\1/g' \
        "$dest"/bin/gmat_startup_file.txt
    unix2dos "$dest"/bin/gmat_startup_file.txt
fi

# Remove git files
find "$dest" -iname .gitignore -delete
