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
  -p pw         Use repository password pw (default: prompt) 
  -t type       Assemble a particular type of distribution:
                full: everything included (default)
                public: publicly-releasable version
                full-release: full excluding pre-release items
                public-release: public excluding pre-release items
  -u user       Use repository username user (default: prompt)
END
}

# File sources
sfrepo='svn://svn.code.sf.net/p/gmat/code'
jazzrepo='https://gs580s-jazz.ndc.nasa.gov/svn/GMAT' 
apppath="$sfrepo/trunk/application"

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
        u) user="$OPTARG";;
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
    exepath="$winbuildspath/$buildname/bin/GMAT.exe"
    if [ ! -e "$exepath" ]
    then
        echo "Invalid build: cannot find $exepath"
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

# remove debug directory
rm -rf "$dest/debug"

# Mars-GRAM 2005 data
if [ $TYPE = 'full' -o $TYPE = 'full-release' ]
then
    mgpath="$jazzrepo/trunk/code/MarsGRAMPlugin/data"
    if [ $user ]
    then
        ustring="--username $user"
    fi
    if [ $pw ]
    then
        pwstring="--password $pw"
    fi
    svn export $ustring $pwstring --force "$mgpath" "$dest/data/atmosphere"
fi

# libCInterface MATLAB files
cifacepath="$sfrepo/trunk/plugins/CInterfacePlugin"
svn export --force "$cifacepath/matlab" "$dest/matlab/libCInterface"

# Remove proprietary plugins if necessary
if [ $TYPE = 'public' -o $TYPE = 'public-release' ]
then
    rm -rf "$dest"/plugins/proprietary/*
    sed -i '/plugins\/proprietary/s/^\( *PLUGIN\) /#\1/g' \
        "$dest"/bin/gmat_startup_file.txt
    unix2dos "$dest"/bin/gmat_startup_file.txt
fi

