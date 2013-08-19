#!/bin/sh

# This script assembles a complete Windows executable copy of GMAT after a
# build. It assumes that it is being run from the trunk/build/windows-VS2010
# directory.
#
# Usage: assemble.sh [option] ...
# Options:
#   -d dest       Place files in specified directory dest (default: ./gmat)
#   -j jazzpath   Path to a working copy of <Jazz>/trunk/code
#   -n netpath    Path to \\mesa-file\595\GMAT network mount
#   -?            Print this usage message
echo "Assembling latest complete version"

# Initializations
cur=`dirname $0`
dest=./gmat
jazz=${cur}/../../../GmatPlugins
netpath='//mesa-file/595/GMAT'

usage() {
cat <<END
Usage: $0 [option] ...
Options:
  -b binfiles   Path to daily binary files
  -d dest       Place files in specified directory dest (default: ./gmat)
  -j jazzpath   Path to a working copy of <Jazz>/trunk/code
  -n netpath    Path to \\mesa-file\595\GMAT network mount
  -?            Print this usage message
END
}

# Argument handling
while getopts b:d:j:n: o
do
    case "$o" in
        b) binfiles="$OPTARG";;
        d) dest="$OPTARG";;
        j) jazz="$OPTARG";;
        n) netpath="$OPTARG";;
        ?) usage; exit 1;;
    esac
done

echo "binfiles="$binfiles
echo "dest="$dest
echo "jazz="$jazz
echo "netpath="$netpath

# Create destination directory
if [ ! -e "$dest" ]
then
    mkdir -p "$dest"
fi

# Copy static application files
svn export --force ${cur}/../../application "$dest"

# Copy bin files
cp -av "$binfiles"/* "$dest"

# Copy support libraries
cp -av \
    "$netpath"/Builds/windows/vc_lib/wx2.8.12/*.dll \
    "$dest"/bin
cp -av \
    "$netpath"/Builds/windows/vc_lib/*.dll \
    "$dest"/bin

# Remove any Windows hidden files
find "$dest" -iname thumbs.db -delete

# Mars-GRAM 2005 data
mgpath="$jazz/MarsGRAMPlugin/data"
cp -av "$mgpath/"* "$dest/data/atmosphere"

# libCInterface MATLAB files
cifacepath=${cur}/../../plugins/CInterfacePlugin
cp -av "$cifacepath"/matlab/* "$dest/matlab/libCInterface"

echo "Finished assembling latest complete version"
