################################################################################
# $Header: /GMAT/dev/cvs/matlab/gmat_mex/mex-file/gmatopts.sh,v 1.1 2005/12/22 19:14:08 jgurgan Exp $
#
# Purpose:     This is the option file for compiling and linking to create
#              mex-file.
#
# Author:      J. Gurganus
# Created:     2005/12/15
#
# NOTE:        This script has been copied and updated from 
#              $MATLAB/bin/gccopts.sh when you use mex -setup.   Also, added
#              wxWidgets flags for including directory and its linker/library 
#              for MacOS X only.
################################################################################
#
#
# gccopts.sh	Shell script for configuring MEX-file creation script,
#               mex.  These options were tested with gcc 3.2.3.
#
# usage:        Do not call this file directly; it is sourced by the
#               mex shell script.  Modify only if you don't like the
#               defaults after running mex.  No spaces are allowed
#               around the '=' in the variable assignment.
#
#               Note: only the gcc side of this script was tested.
#               The FORTRAN variables are lifted directly from
#               mexopts.sh; use that file for compiling FORTRAN
#               MEX-files.
#
# SELECTION_TAGs occur in template option files and are used by MATLAB
# tools, such as mex and mbuild, to determine the purpose of the contents
# of an option file. These tags are only interpreted when preceded by '#'
# and followed by ':'.
#
#SELECTION_TAG_MEX_OPT: Template Options file for building gcc MEX-files
#
# Copyright 1984-2004 The MathWorks, Inc.
# $Revision: 1.1 $  $Date: 2005/12/22 19:14:08 $
#----------------------------------------------------------------------------
#
    TMW_ROOT="$MATLAB"
    MFLAGS=''
    if [ "$ENTRYPOINT" = "mexLibrary" ]; then
        MLIBS="-L$TMW_ROOT/bin/$Arch -lmx -lmex -lmat -lmwservices -lut -lm"
    else  
        MLIBS="-L$TMW_ROOT/bin/$Arch -lmx -lmex -lmat -lm"
    fi

    case "$Arch" in
        Undetermined)
#----------------------------------------------------------------------------
# Change this line if you need to specify the location of the MATLAB
# root directory.  The script needs to know where to find utility
# routines so that it can determine the architecture; therefore, this
# assignment needs to be done while the architecture is still
# undetermined.
#----------------------------------------------------------------------------
            MATLAB="$MATLAB"
#
# Determine the location of the GCC libraries
#
	    GCC_LIBDIR=`gcc -v 2>&1 | awk '/.*Reading specs.*/ {print substr($4,0,length($4)-6)}'`
            ;;
        hpux)
#----------------------------------------------------------------------------
            echo "gcc is not supported on hpux"
#----------------------------------------------------------------------------
            ;;
        glnx86)
#----------------------------------------------------------------------------
            RPATH="-Wl,-rpath-link,$TMW_ROOT/bin/$Arch"
#           gcc -v
#           gcc version 3.2.3
            CC='gcc'
            CFLAGS='-fPIC -ansi -D_GNU_SOURCE -pthread -fexceptions -m32'
            CLIBS="$RPATH $MLIBS -lm -lstdc++"
            COPTIMFLAGS='-O -DNDEBUG'
            CDEBUGFLAGS='-g'
#           
#           g++ -v
#           gcc version 3.2.3
            CXX='g++'
            CXXFLAGS='-fPIC -ansi -D_GNU_SOURCE -pthread '
            CXXLIBS="$RPATH $MLIBS -lm"
            CXXOPTIMFLAGS='-O -DNDEBUG'
            CXXDEBUGFLAGS='-g'
#
#           g77 -fversion
#           GNU Fortran (GCC 3.2.3) 3.2.3 20030422 (release)
#           NOTE: g77 is not thread safe
            FC='g77'
            FFLAGS='-fPIC -fexceptions'
            FLIBS="$RPATH $MLIBS -lm -lstdc++"
            FOPTIMFLAGS='-O'
            FDEBUGFLAGS='-g'
#
            LD="$COMPILER"
            LDEXTENSION='.mexglx'
            LDFLAGS="-pthread -shared -m32 -Wl,--version-script,$TMW_ROOT/extern/lib/$Arch/$MAPFILE"
            LDOPTIMFLAGS='-O'
            LDDEBUGFLAGS='-g'
#
            POSTLINK_CMDS=':'
#----------------------------------------------------------------------------
            ;;
        glnxi64)
#----------------------------------------------------------------------------
echo "Error: Did not imbed 'options.sh' code"; exit 1 #imbed options.sh glnxi64 12
#----------------------------------------------------------------------------
            ;;
        glnxa64)
#----------------------------------------------------------------------------
            RPATH="-Wl,-rpath-link,$TMW_ROOT/bin/$Arch"
#           gcc -v
#           gcc version 3.2.3
            CC='gcc'
            CFLAGS='-fPIC -fno-omit-frame-pointer -ansi -D_GNU_SOURCE -pthread -fexceptions'
            CLIBS="$RPATH $MLIBS -lm -lstdc++"
            COPTIMFLAGS='-O -DNDEBUG'
            CDEBUGFLAGS='-g'
#           
#           g++ -v
#           gcc version 3.2.3
            CXX='g++'
            CXXFLAGS='-fPIC -fno-omit-frame-pointer -ansi -D_GNU_SOURCE -pthread '
            CXXLIBS="$RPATH $MLIBS -lm"
            CXXOPTIMFLAGS='-O -DNDEBUG'
            CXXDEBUGFLAGS='-g'
#
#           g77 -fversion
#           GNU Fortran (GCC 3.2.3) 3.2.3 20030422 (release)
#           NOTE: g77 is not thread safe
            FC='g77'
            FFLAGS='-fPIC -fno-omit-frame-pointer -fexceptions'
            FLIBS="$RPATH $MLIBS -lm -lstdc++"
            FOPTIMFLAGS='-O'
            FDEBUGFLAGS='-g'
#
            LD="$COMPILER"
            LDEXTENSION='.mexa64'
            LDFLAGS="-pthread -shared -Wl,--version-script,$TMW_ROOT/extern/lib/$Arch/$MAPFILE"
            LDOPTIMFLAGS='-O'
            LDDEBUGFLAGS='-g'
#
            POSTLINK_CMDS=':'
#----------------------------------------------------------------------------
            ;;
        sol2)
#----------------------------------------------------------------------------
            CC='gcc'
            GCC_LIBDIR=`$CC -v 2>&1 | sed -n '1s/[^\/]*\(.*\/lib\).*/\1/p'`
            CFLAGS='-fPIC -fexceptions'
            CLIBS="$MLIBS -lm"
            COPTIMFLAGS='-O -DNDEBUG'
            CDEBUGFLAGS='-g'  
            CXXDEBUGFLAGS='-g'
#
            CXX='g++'
            CXXFLAGS='-fPIC'
            CXXLIBS="$MLIBS -lm"
            CXXOPTIMFLAGS='-O -DNDEBUG'
#
            LD="$COMPILER"
            LDEXTENSION='.mexsol'
            LDFLAGS="-shared -Wl,-M,$TMW_ROOT/extern/lib/$Arch/$MAPFILE,-R,$GCC_LIBDIR"
            LDOPTIMFLAGS='-O'
            LDDEBUGFLAGS='-g'  
#
            POSTLINK_CMDS=':'
#----------------------------------------------------------------------------
            ;;
        mac)
#----------------------------------------------------------------------------
            CC='gcc-3.3'
            CFLAGS='-fno-common -no-cpp-precomp -fexceptions'
            CFLAGS='-fno-common -no-cpp-precomp -fexceptions'
            CLIBS="$MLIBS -lstdc++"
            COPTIMFLAGS='-O3 -DNDEBUG'
            CDEBUGFLAGS='-g'
#           
#           Get wxWidgets flags for compiler and link
            WX_CXXFLAGS=`$WX_HOME/wx-config --cxxflags` 
            WX_LIBS=`$WX_HOME/wx-config --libs` 

#
#           g++-3.3 -v
#           gcc version 3.3 20030304 (Apple Computer, Inc. build 1435)
            CXX=g++-3.3
            CXXFLAGS='-fno-common -no-cpp-precomp -fexceptions'
            CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS"
#            CXXLIBS="$MLIBS -lstdc++"
            CXXLIBS="$MLIBS -lstdc++ $WX_LIBS"
            CXXOPTIMFLAGS='-O3 -DNDEBUG'
            CXXDEBUGFLAGS='-g'
#
            LD="$CC"
            LDEXTENSION='.mexmac'
            LDFLAGS="-bundle -Wl,-flat_namespace -undefined suppress -Wl,-exported_symbols_list,$TMW_ROOT/extern/lib/$Arch/$MAPFILE"
            LDOPTIMFLAGS='-O'
            LDDEBUGFLAGS='-g'
#
            POSTLINK_CMDS=':'
#----------------------------------------------------------------------------
            ;;
    esac
#############################################################################
#
# Architecture independent lines:
#
#     Set and uncomment any lines which will apply to all architectures.
#
#----------------------------------------------------------------------------
#           CC="$CC"
#           CFLAGS="$CFLAGS"
#           COPTIMFLAGS="$COPTIMFLAGS"
#           CDEBUGFLAGS="$CDEBUGFLAGS"
#           CLIBS="$CLIBS"
#
#           LD="$LD"
#           LDFLAGS="$LDFLAGS"
#           LDOPTIMFLAGS="$LDOPTIMFLAGS"
#           LDDEBUGFLAGS="$LDDEBUGFLAGS"
#----------------------------------------------------------------------------
#############################################################################

