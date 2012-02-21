#!/bin/sh

# This script assembles a complete Windows executable copy of GMAT after a
# build. It assumes that it is being run from the trunk/build/windows-VS2010
# directory.
#
# Usage: assemble.sh [option] ...
# Options:
#   -d dest       Place files in specified directory dest (default: ./gmat)
#   -j jazzpath   Path to a working copy of <Jazz>/trunk/code
#   -?            Print this usage message

# Initializations
cur=`dirname $0`
dest=./gmat
jazz=${cur}/../../../GmatPlugins

usage() {
cat <<END
Usage: $0 [option] ...
Options:
  -d dest       Place files in specified directory dest (default: ./gmat)
  -j jazzpath   Path to a working copy of <Jazz>/trunk/code
  -?            Print this usage message
END
}

# Argument handling
while getopts d:j: o
do
    case "$o" in
        d) dest="$OPTARG";;
        j) jazz="$OPTARG";;
        ?) usage; exit 1;;
    esac
done

# Create destination directory
if [ ! -e "$dest" ]
then
    mkdir -p "$dest"
fi

# Copy application files
cp -av ${cur}/../../application/* "$dest"

# Remove build files
find "$dest" -iname '*.exp' -delete
find "$dest" -iname '*.lib' -delete

# Copy support libraries
winbuildspath='//mesa-file/595/GMAT/Builds/windows/VS2010_build'
cp -av \
    "$winbuildspath"/../vc_lib/wx2.8.12/*.dll \
    "$dest"/bin
cp -av \
    "$winbuildspath"/../vc_lib/*.dll \
    "$dest"/bin

# Remove any Windows hidden files
find "$dest" -iname thumbs.db -delete

# Mars-GRAM 2005 data
mgpath="$jazz/MarsGRAMPlugin/data"
cp -av "$mgpath/"* "$dest/data"

# libCInterface MATLAB files
cifacepath=${cur}/../../plugins/CInterfacePlugin
cp -av "$cifacepath/matlab" "$dest/matlab/libCInterface"
