#!/bin/bash
# Author:  Jfisher
# Project: Gmat
# Title:   configure.sh
# Purpose: This script allows developers to quickly and easily 
#	   configure the GMAT dependencies on Mac/Linux.
# Usage:   configure.sh [-w wx_version] [-help]
#          Default behavior detects GMAT path and builds wxWidgets 3.0.2
#           -w: (optional) wxWidgets dotted version, e.g. 3.0.2
#           -help: Print usage
# Updates: Feb-Apr 2015: Ravi Mathur: Heavy updates for new CMake

# Enable strict bash
set -uo pipefail

# Set defaults
wx_build=true 
wx_version=3.0.2
xerces_version=3.1.4
osx_min_version=10.10

# Get path to gmat (one folder above this script)
BASEDIR=$( dirname "${BASH_SOURCE[0]}" ) # Path to this script
BASEDIRFULL=$( cd "${BASEDIR}" && pwd ) # Go to script folder
gmat_path="$(dirname "${BASEDIRFULL}")" # Path to gmat folder
logs_path=$gmat_path/depends/logs

# ***********************************
# Input System
# ***********************************
while getopts w:h opt; do
  case $opt in
    w) wx_version="$OPTARG";;
    h) echo Usage: configure.sh [-w wx_dotted_version] [-help]
      exit;;
  esac
done

clear
echo Configuring GMAT dependencies at $gmat_path/depends
  
# Detect Mac/Linux and number of processing cores (for parallel compile)
if [ "$(uname)" = "Darwin" ]
then
  mac=true
  ncores=$(sysctl hw.ncpu | awk '{print $2}')
else
  mac=false
  ncores=$(nproc)
fi

# Detect architecture
if [[ "$(uname -m)" = *"64" ]]
then
  arch="x64"
else
  arch="x86"
fi

# Make sure curl exists
if ! type curl > /dev/null
then
  echo "This script requires curl to download dependencies. Please install curl first."
  exit
fi

# Make sure system libcurl is used (e.g. instead of Matlab's)
if [ $mac == true ]
then
  export DYLD_LIBRARY_PATH=/usr/lib${DYLD_LIBRARY_PATH:+:$DYLD_LIBRARY_PATH}
else
  # Account for both Red Hat and Ubuntu
  export LD_LIBRARY_PATH=/usr/lib64:/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
fi

# Variables for OS-specific flags
# These can be used as needed in the script
OSXFLAGS=""
LINUXFLAGS=""

# ***********************************
# Download Library Dependencies
# ***********************************
function download_depends() {

	# Set Variables
	bin_path=$gmat_path/depends/bin
	f2c_path=$gmat_path/depends/f2c
	cspice_path=$gmat_path/depends/cspice
	wxWidgets_path=$gmat_path/depends/wxWidgets
	xerces_path=$gmat_path/depends/xerces
	sofa_path=$gmat_path/depends/sofa
	tsplot_path=$gmat_path/depends/tsPlot
	pcre_path=$gmat_path/depends/pcre

	if [ $mac == true ]
	then
	  cspice_path=$cspice_path/macosx
	else
	  cspice_path=$cspice_path/linux
	fi

	# Create directories and download f2c if it does not already exist.
	# Disabled for now
	if [ ! -d "$f2c_path" ] && [ false == true ]
	then
		# Create Directories
		mkdir "$f2c_path"
	
		# Change to f2c directory
		cd "$f2c_path"
	
		# Download F2C
		wget -nH --cut-dirs=1 -r http://netlib.org/f2c/
	fi

	# Create directories and download cspice if it does not already exist.
	if [ ! -d "$cspice_path" ]
	then
		# Create Directories
		mkdir -p "$cspice_path"
	
		# Change to cspice directory
		cd "$cspice_path"

		# Determine download folder based on OS
        	if [ $mac == true ]
        	then
		  cspice_type=MacIntel_OSX_AppleC
        	else 
		  cspice_type=PC_Linux_GCC
        	fi
	
		# Download and extract Spice (32/64-bit)
		if [ "$arch" = "x86" ]
		then
		  echo "Downloading 32-bit CSPICE..."
		  curl http://naif.jpl.nasa.gov/pub/naif/toolkit/C/"$cspice_type"_32bit/packages/cspice.tar.Z > cspice.tar.Z
		  gzip -d cspice.tar.Z
		  tar -xf cspice.tar
		  mv cspice cspice32
		else
		  echo "Downloading 64-bit CSPICE..."
		  curl http://naif.jpl.nasa.gov/pub/naif/toolkit/C/"$cspice_type"_64bit/packages/cspice.tar.Z > cspice.tar.Z
		  gzip -d cspice.tar.Z
		  tar -xf cspice.tar
		  mv cspice cspice64
		fi
		rm cspice.tar # Cleanup
	fi	

	# Create directories and download wxWidgets if needed
	if [ ! -d "$wxWidgets_path/wxWidgets-$wx_version" ] && [ $wx_build == true ]
	then
		# Create Directories
		mkdir -p "$wxWidgets_path"
	
		# Change to wxWidgets directory
		cd "$wxWidgets_path"
	
		# Download wxWidgets source
		echo "Downloading wxWidgets $wx_version..."
		curl -Lk https://github.com/wxWidgets/wxWidgets/releases/download/v$wx_version/wxWidgets-$wx_version.tar.bz2 > wxWidgets.tar.bz2
		tar -xf wxWidgets.tar.bz2
		rm wxWidgets.tar.bz2

		# Make sure wxWidgets was downloaded
		if [ ! -d "wxWidgets-$wx_version" ]
		then
		  echo Error in wxWidgets-$wx_version download
		  wx_build=false
		fi
	fi

	# Download Xerces if it doesn't already exist
	if [ ! -d "$xerces_path" ]
	then
		# Create Directories
		mkdir -p "$xerces_path"
	
		# Change to Xerces directory
		cd "$xerces_path"

		# Download and extract Xerces
		echo "Downloading Xerces-C $xerces_version"
		curl http://archive.apache.org/dist/xerces/c/3/sources/xerces-c-$xerces_version.tar.gz > xerces.tar.gz
		gzip -d xerces.tar.gz
		tar -xf xerces.tar --strip-components 1
		rm xerces.tar
	fi


	# Create directories and download sofa if it does not already exist.
	# Disabled for now
	if [ ! -d "$sofa_path" ] && [ false == true ]
	then
		# Change to depends directory
		cd "$gmat_path/depends/"

		# Download and extract Sofa Source, finally remove archive
		wget http://www.iausofa.org/2012_0301_C/sofa_c_a.zip
		unzip sofa_c_a.zip
		rm sofa_c_a.zip	
	fi

	# Create directories and download tsplot if it does not already exist.
	# Disabled for now
	if [ ! -d "$tsplot_path" ] && [ false == true ]
	then
		# Create Directories
		mkdir "$tsplot_path"
	
		# Change to tsplot directory
		cd "$tsplot_path"
	
		# Checkout tsplot source.
		svn co svn://svn.code.sf.net/p/tsplot/code/trunk tsplot-latest
	fi

	# Create directories and download pcre if it does not already exist.
	# Disabled for now
	if [ ! -d "$pcre_path" ] && [ false == true ]
	then
		# Create Directories
		mkdir "$pcre_path"
	
		# Change to pcre directory
		cd "$pcre_path"
	
		# Checkout release pcre source.
		svn co svn://vcs.exim.org/pcre/code/tags/pcre-8.31 pcre-8.31
	fi
}

# Function to convert "x.y.z"-style version numbers to integers
# Each subnumber can have 3 digits maximum
# e.g. 2.4.10 -> 002004010
function version { echo "$@" | awk -F. '{ printf("%03d%03d%03d\n", $1,$2,$3); }'; }

function build_wxWidgets() {
	# Set build path based on version
	wx_path=$wxWidgets_path/wxWidgets-$wx_version

	# OS-specific vars
	if [ $mac == true ]
    	then
	  # Out-of-source wx build/install locations
	  wx_build_path=$wx_path/cocoa-build
	  wx_install_path=$wx_path/cocoa-install
    	else
	  # Out-of-source wx build/install locations
	  wx_build_path=$wx_path/gtk-build
	  wx_install_path=$wx_path/gtk-install
    	fi

	# Find a test file to see if wxWidgets has already been configured
	wx_test_file=`ls $wx_install_path/lib/libwx_baseu-* 2> /dev/null | head -n 1`

	# Build wxWidgets if the test file doesn't already exist
	# Note that according to 
	#   http://docs.wxwidgets.org/3.0/overview_debugging.html
	# debugging features "are always available by default", so
	# we don't build a separate debug version here.
	# IF a debug version is required in the future, then this
	# if/else block should be repeated with the --enable-debug flag
	# added to mac & linux versions of the wx ./configure command
	if [ -f "$wx_test_file" ]
	then
	  echo "wxWidgets $wx_version already configured"
	else
	  mkdir -p "$wx_build_path"
	  cd "$wx_build_path"

	  echo "Configuring wxWidgets $wx_version. This could take a while..."
	  if [ $mac == true ]
	  then
	    # wxWidgets 3.0.2 has a compile error due to an incorrect
	    # include file on OSX 10.10+. Apply patch to fix this.
	    # See [GMT-5384] and http://goharsha.com/blog/compiling-wxwidgets-3-0-2-mac-os-x-yosemite/
	    osx_ver=`sw_vers -productVersion`
	    if [ "$(version "$osx_ver")" -gt "$(version 10.10.0)" ] && [ "$(version "$wx_version")" -eq "$(version 3.0.2)" ]; then
	      sed -i.bk 's/WebKit.h/WebKitLegacy.h/' "$wx_path/src/osx/webview_webkit.mm"
	    fi

	    # wxWidgets needs these flags on OSX
	    OSXFLAGS="--with-osx_cocoa --with-macosx-version-min=$osx_min_version"
	  fi
	  ../configure $OSXFLAGS --enable-unicode --with-opengl --prefix="$wx_install_path" > "$logs_path/wxWidgets_configure.log" 2>&1

	  # Compile, install, and clean wxWidgets
	  make -j$ncores > "$logs_path/wxWidgets_build.log" 2>&1
	  if [ $? -eq 0 ]
	  then
	    make install > "$logs_path/wxWidgets_install.log" 2>&1
	    cd ..; rm -Rf "$wx_build_path"
	  else
	    echo "wxWidgets build failed. Fix errors and try again."
	    if [ $mac == false ]
	    then
	      # Report common OpenGL install bug on Linux
	      echo "On Linux, please make sure that /usr/lib/x86_64-linux-gnu/libGL.so (symbolic link) resolves to the actual OpenGL library."
	    fi
	    return
	  fi
	fi
}

function build_xerces() {
	# Out-of-source xerces build/install locations
	if [ $mac == true ]
    	then
	  xerces_build_path=$xerces_path/cocoa-build
	  xerces_install_path=$xerces_path/cocoa-install
    	else
	  xerces_build_path=$xerces_path/linux-build
	  xerces_install_path=$xerces_path/linux-install
    	fi

	# Find a test file to check if xerces has already been installed
	xerces_test_file=$xerces_install_path/lib/libxerces-c.a

	# Build Xerces if the test file doesn't already exist
	if [ -f "$xerces_test_file" ]
	then
	  echo "Xerces $xerces_version already configured"
	else
	  mkdir -p "$xerces_build_path"
	  cd "$xerces_build_path"

	  # For users who compile GMAT on multiple platforms side-by-side.
	  # Running Windows configure.bat causes Mac/Linux configure scripts
	  # to have missing permissions.
	  chmod u+x ../configure 
	  chmod u+x ../config/*

	  if [ $mac == true ]
	  then
	    # Xerces needs these flags on OSX
	    OSXFLAGS="-mmacosx-version-min=$osx_min_version"
	  fi
          COMMONXERCESFLAGS="--disable-shared --disable-netaccessor-curl --disable-transcoder-icu --disable-msgloader-icu"

	  echo "Configuring Xerces $xerces_version debug library. This could take a while..."
	  COMMONCFLAGS="-O0 -g -fPIC $OSXFLAGS"
	  ../configure $COMMONXERCESFLAGS CFLAGS="$COMMONCFLAGS" CXXFLAGS="$COMMONCFLAGS" --prefix="$xerces_install_path" > "$logs_path/xerces_configure_debug.log" 2>&1
	  make -j$ncores > "$logs_path/xerces_build_debug.log" 2>&1
	  if [ $? -eq 0 ]
	  then
	    make install > "$logs_path/xerces_install_debug.log" 2>&1
	    mv "$xerces_install_path/lib/libxerces-c.a" "$xerces_install_path/lib/libxerces-cd.a"
	    make clean > /dev/null 2>&1
	  else
	    echo "Xerces debug build failed. Fix errors and try again."
	    return
	  fi

	  echo "Configuring Xerces $xerces_version release library. This could take a while..."
	  COMMONCFLAGS="-O2 -fPIC $OSXFLAGS"
	  ../configure $COMMONXERCESFLAGS CFLAGS="$COMMONCFLAGS" CXXFLAGS="$COMMONCFLAGS" --prefix="$xerces_install_path" > "$logs_path/xerces_configure_release.log" 2>&1
	  make -j$ncores > "$logs_path/xerces_build_release.log" 2>&1
	  if [ $? -eq 0 ]
	  then
	    make install > "$logs_path/xerces_install_release.log" 2>&1
	    cd ..; rm -Rf "$xerces_build_path"
	  else
	    echo "Xerces release build failed. Fix errors and try again."
	    return
	  fi

	fi
}

function build_cspice() {
	# Setup compile variables
	if [ "$arch" = "x86" ]
	then
	  cspice_path=$cspice_path/cspice32
	  export TKCOMPILEARCH="-m32"
	else
	  cspice_path=$cspice_path/cspice64
	  export TKCOMPILEARCH="-m64"
	fi

	if [ $mac == true ]
	then
	  # CSPICE needs these flags on OSX
	  export OSXFLAGS="-mmacosx-version-min=$osx_min_version"
	fi

	# Find a test file to check if cspice has already been installed
	cspice_test_file=`ls $cspice_path/lib/cspiced.a 2> /dev/null | head -n 1`

	# Build CSPICE if the test file doesn't already exist
	if [ -f "$cspice_test_file" ]
	then
	  echo "CSPICE already configured"
	else
	  cd "$cspice_path/src/cspice"

	  # Compile debug CSPICE with integer uiolen [GMT-5044]
	  echo "Compiling CSPICE debug library. This could take a while..."
	  export TKCOMPILEOPTIONS="$TKCOMPILEARCH -c -ansi $OSXFLAGS -g -fPIC -DNON_UNIX_STDIO -DUIOLEN_int"
	  ./mkprodct.csh > "$logs_path/cspice_build_debug.log" 2>&1
	  if [ $? -eq 0 ]
	  then
	    mv ../../lib/cspice.a ../../lib/cspiced.a
          else
	    echo "CSPICE debug build failed. Fix errors and try again."
	    return
	  fi

	  # Compile release CSPICE with integer uiolen [GMT-5044]
	  echo "Compiling CSPICE release library. This could take a while..."
	  export TKCOMPILEOPTIONS="$TKCOMPILEARCH -c -ansi $OSXFLAGS -O2 -fPIC -DNON_UNIX_STDIO -DUIOLEN_int"
	  ./mkprodct.csh > "$logs_path/cspice_build_release.log" 2>&1
	  if [ $? -ne 0 ]
	  then
	    echo "CSPICE release build failed. Fix errors and try again."
	    return
	  fi
	fi
}

# Run Script Functions
mkdir -p "$logs_path"
download_depends
build_cspice
if [ $wx_build == true ]
then
  build_wxWidgets
fi
build_xerces

# ***********************************
# End of script
# ***********************************
echo Dependency Configuration Complete!
exit 1
