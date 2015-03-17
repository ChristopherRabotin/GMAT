#!/bin/sh
# This script adapted from the wxWidgets Mac build directory
# Change the install names of the Mac wxWidgets libraries
# Arguments:
#  ${1}: wxWidgets library location, typically GMAT bin/libwx directory
#  ${2}: wxrc binary location, typically wxWidgets/bin directory
#  ${3}: new install name, typically "@rpath"
#  ${4}: old install name, typically wxWidgets/lib directory

# Find all dylibs in ${1} that are actual files (not symlinks)
libnames=`cd ${1} ; find *.dylib -type f`

# Collect all changes from old name ${4} to new name ${4}
changes=''
for dep in ${libnames} ; do
    changes="${changes} -change ${4}/${dep} ${3}/${dep}"
done

# Change install names
for i in ${libnames} ; do
    install_name_tool ${changes} -id ${3}/${i} ${1}/${i}
done

# wxrc is not installed with GMAT so ignore the second input argument
#install_name_tool ${changes} ${2}/wxrc-3.0
