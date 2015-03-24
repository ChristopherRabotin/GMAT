#!/bin/sh
# Change the install names of the Mac wxWidgets libraries
# Arguments:
#  ${1}: wxWidgets library location, typically GMAT bin/libwx directory
#  ${2}: old install path, typically wxWidgets/lib directory
#  ${3}: new install path, typically "@rpath"
#  ${4+}: Additional GMAT binaries that use wxWidgets, e.g. Gui

# Find all wxWidgets dylibs
wxlibnames=`cd ${1} ; find *.dylib`

# Collect all changes from old name to new name
changes=''
for dep in ${wxlibnames} ; do
  changes="${changes} -change ${2}/${dep} ${3}/${dep}"
done

# Change install names of wxWidgets libraries
for i in ${wxlibnames} ; do
  install_name_tool ${changes} -id ${3}/${i} ${1}/${i}
done

# Change wxWidgets install names inside GMAT binaries
for i in ${@:4}; do
  install_name_tool ${changes} ${i}
done
