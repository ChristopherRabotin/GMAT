#!/bin/sh

# Generate the list of files for inclusion in gmat.nsi (both in the "GMAT" and
# "Uninstaller" sections).

# install files (not working for now)
# find gmat \
    # \( -type d -printf 'SetOutPath "$INSTDIR/%p"\n' \) , \
    # \( -type f -printf 'File "${SRCDIR}/%p"\n' \) \
    # | sed 's/\//\\/g' \
    # | sed 's/\\gmat//' \
    # > install.nsh


# delete files
find gmat -type f \
    | sed 's/\//\\/g' \
    | sed 's/^gmat/Delete "\$INSTDIR/' \
    | sed 's/$/"/' \
    | unix2dos \
    >> uninstall.nsh

# delete directories
find gmat -depth -type d \
    | sed 's/\//\\/g' \
    | sed 's/^gmat/RMDir "\$INSTDIR/' \
    | sed 's/$/"/' \
    | unix2dos \
    >> uninstall.nsh
