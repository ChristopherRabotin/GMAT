#!/bin/bash
# Author: 	Jfisher
# Project:	Gmat
# Title:	configure.sh
# Purpose:	This script allows developers to quickly and easily 
#		configure the gmat development environment on linux.
# Usage:        configure.sh [-p /path/to/gmat] [-w]
#               Default behavior detects OS and GMAT path, and builds wxWidgets
#               -p: specify path to top-level GMAT directory
#               -w: do NOT build wxWidgets (retained for compatibility)

# Clear the screen
clear

# Set defaults
use_latest=false # Use latest or predetermined releases of dependencies
build_wx=true 

BASEDIR=$( dirname "${BASH_SOURCE[0]}" )
BASEDIRFULL=$( cd "${BASEDIR}" && pwd )
gmat_path="$(dirname "${BASEDIRFULL}")"

# ***********************************
# Input System
# ***********************************
while getopts p:w opt; do
  case $opt in
    p) gmat_path="$OPTARG";;
    w) build_wx=false;;
  esac
done
  
# Detect Mac/Linux
if [ "$(uname)" = "Darwin" ]
then
  mac=true
else
  mac=false
fi

# Detect architecture
if [[ "$(uname -m)" = *"64" ]]
then
  arch="x64"
else
  arch="x86"
fi

# ***********************************
# Download Library Dependencies
# ***********************************
function download_depends() {

	# Set Variables
	bin_path=$gmat_path/depends/bin
	f2c_path=$gmat_path/depends/f2c
	cspice_path=$gmat_path/depends/cspice
	wxWidgets_path=$gmat_path/depends/wxWidgets
	sofa_path=$gmat_path/depends/sofa
	tsplot_path=$gmat_path/depends/tsPlot
	pcre_path=$gmat_path/depends/pcre

	if [ $mac == true ]
	then
	  cspice_path=$cspice_path/macosx
	else
	  cspice_path=$cspice_path/linux
	fi

	# Display the variables
	echo $bin_path
	echo $f2c_path
	echo $cspice_path
	echo $wxWidgets_path
	echo $sofa_path
	echo $tsplot_path

	# Create directories and download f2c if it does not already exist.
	if [ ! -d "$f2c_path" ]
	then
		# Create Directories
		mkdir "$f2c_path"
	
		# Change to f2c directory
		cd "$f2c_path"
	
		# Use wget to download software
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
		  wget ftp://naif.jpl.nasa.gov/pub/naif/toolkit//C/"$cspice_type"_32bit/packages/cspice.tar.Z
		  gzip -d cspice.tar.Z
		  tar xfv cspice.tar
		  mv cspice cspice32
		else
		  wget ftp://naif.jpl.nasa.gov/pub/naif/toolkit//C/"$cspice_type"_64bit/packages/cspice.tar.Z
		  gzip -d cspice.tar.Z
		  tar xfv cspice.tar
		  mv cspice cspice64
		fi

		# Remove downloaded archive
		rm cspice.tar
	fi	

	# Create directories and download wxwidgets if needed
	if [ ! -d "$wxWidgets_path" ] && [ build_wx == true ]
	then
		# Create Directories
		mkdir "$wxWidgets_path"
	
		# Change to wxwidgets directory
		cd "$wxWidgets_path"
	
		# Checkout wxWidget source.
		svn co http://svn.wxwidgets.org/svn/wx/wxWidgets/tags/WX_2_8_12/ wxWidgets-2.8.12
		
		# Copy modified configure scripts (This fixes the OpenGL location issue)
		cp "$bin_path/wx/configure" "$wxWidgets_path/wxWidgets-2.8.12/configure"
	
		if [ $use_latest == true ]
		then
			# Change to wxwidgets directory
			cd "$wxWidgets_path"
	
			# Checkout latest wxWidget source.
			svn co http://svn.wxwidgets.org/svn/wx/wxWidgets/trunk/ wxWidgets-latest
		
			# Copy modified configure scripts (This fixes the OpenGL location issue)
			cp "$bin_path/wx/configure" "$wxWidgets_path/wxWidgets-2.8.12/configure"
		
		fi
	fi

	# Create directories and download sofa if it does not already exist.
	if [ ! -d "$sofa_path" ]
	then
		# Change to depends directory
		cd "$gmat_path/depends/"
:w

		# Download and extract Sofa Source, finally remove archive
		wget http://www.iausofa.org/2012_0301_C/sofa_c_a.zip
		unzip sofa_c_a.zip
		rm sofa_c_a.zip	
	fi

	# Create directories and download tsplot if it does not already exist.
	if [ ! -d "$tsplot_path" ]
	then
		# Create Directories
		mkdir "$tsplot_path"
	
		# Change to tsplot directory
		cd "$tsplot_path"
	
		# Checkout tsplot source.
		svn co svn://svn.code.sf.net/p/tsplot/code/trunk tsplot-latest
	fi

	# Create directories and download pcre if it does not already exist.
	if [ ! -d "$pcre_path" ]
	then
		# Create Directories
		mkdir "$pcre_path"
	
		# Change to pcre directory
		cd "$pcre_path"
	
		# Checkout pcre source.
		svn co svn://vcs.exim.org/pcre/code/tags/pcre-8.31 pcre-8.31
	
		if [ $use_latest == true ]
		then
		
			# Change to pcre directory
			cd "$pcre_path"
	
			# Checkout latest pcre source.
			svn co svn://vcs.exim.org/pcre/code/trunk pcre-latest
		fi
	fi
}

function build_wxWidgets() {
	# Set build path based on version
	if [ $use_latest == true ]
	then
		wx_build_path=$gmat_path/depends/wxWidgets/latest
	else
		wx_build_path=$gmat_path/depends/wxWidgets/wxWidgets-2.8.12
	fi

	if [ $mac == true ]
    	then
        	# Check if dependencies have already been built
        	depend_path=$gmat_path/depends/wxWidgets/wxWidgets-2.8.12/lib/libwx_mac_core-2.8.0.dylib
    	else
        	# Check if dependencies have already been built
        	depend_path=$gmat_path/depends/wxWidgets/wxWidgets-2.8.12/lib/libwx_gtk2_core-2.8.so
    	fi

	cd "$wx_build_path"

	if [ ! -d "$depend_path" ]
	then
		if [ $mac == true ]
        	then
            		arch_flags="-arch i386"
            		./configure CFLAGS="$arch_flags" CXXFLAGS="$arch_flags" CPPFLAGS="$arch_flags" LDFLAGS="$arch_flags" OBJCFLAGS="$arch_flags" OBJCXXFLAGS="$arch_flags" --with-opengl --with-macosx-version-min=10.7 --with-macosx-sdk=/Developer/SDKs/MacOSX10.6.sdk
        	else
            		# Configure wxWidget build
            		./configure --with-opengl
        	fi

		# Compile wxWidget build
		make 
	
		# Change to contrib directory
		cd contrib
	
		# Compile wxWidget contrib
		make
	fi
}

# Run Script Functions
download_depends
if [ $build_wx == true ]
then
  build_wxWidgets
fi

# ***********************************
# End of script
# ***********************************
echo $gmat_path
echo Dependency Configuration Complete!
exit 1
