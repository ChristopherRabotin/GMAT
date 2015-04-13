#!/bin/bash
# Author:  Jfisher
# Project: Gmat
# Title:   configure.sh
# Purpose: This script allows developers to quickly and easily 
#	   configure the gmat development environment on Mac/Linux.
# Usage:   configure.sh [-p /path/to/gmat] [-w wx_version] [-help]
#          Default behavior detects GMAT path and builds wxWidgets 3.0.2
#           -p: (optional) path to top-level GMAT directory
#           -w: (optional) wxWidgets dotted version, e.g. 3.0.2
#           -help: Print usage
# Updates: Feb-Apr 2015: Ravi Mathur: Heavy updates for new CMake

# Set defaults
wx_build=true 
wx_version=3.0.2

# Get path to gmat (one folder above this script)
BASEDIR=$( dirname "${BASH_SOURCE[0]}" ) # Path to this script
BASEDIRFULL=$( cd "${BASEDIR}" && pwd ) # Go to script folder
gmat_path="$(dirname "${BASEDIRFULL}")" # Path to gmat folder

# ***********************************
# Input System
# ***********************************
while getopts pw:h opt; do
  case $opt in
    p) gmat_path="$OPTARG";;
    w) wx_version="$OPTARG";;
    h) echo Usage: configure.sh [-p /path/to/gmat] [-w wx_dotted_version] [-help]
      exit;;
  esac
done

clear
echo Launching from $gmat_path
echo Using wxWidgets $wx_version
  
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
	#echo $bin_path
	#echo $f2c_path
	#echo $cspice_path
	#echo $wxWidgets_path
	#echo $sofa_path
	#echo $tsplot_path

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
	if [ -d "$cspice_path" ]
	then
	  echo "CSPICE already configured"
	else
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
		  ftp ftp://naif.jpl.nasa.gov/pub/naif/toolkit/C/"$cspice_type"_32bit/packages/cspice.tar.Z
		  gzip -d cspice.tar.Z
		  tar xfv cspice.tar
		  mv cspice cspice32
		  rm cspice.tar
		  cd cspice32/src/cspice
		  export TKCOMPILEARCH="-m32"
		else
		  ftp ftp://naif.jpl.nasa.gov/pub/naif/toolkit/C/"$cspice_type"_64bit/packages/cspice.tar.Z
		  gzip -d cspice.tar.Z
		  tar xfv cspice.tar
		  mv cspice cspice64
		  rm cspice.tar
		  cd cspice64/src/cspice
		  export TKCOMPILEARCH="-m64"
		fi

		# Compile debug CSPICE with integer uiolen [GMT-5044]
		export TKCOMPILEOPTIONS="$TKCOMPILEARCH -c -ansi -g -fPIC -DNON_UNIX_STDIO -DUIOLEN_int"
		./mkprodct.csh
		mv ../../lib/cspice.a ../../lib/cspiced.a

		# Compile release CSPICE with integer uiolen [GMT-5044]
		export TKCOMPILEOPTIONS="$TKCOMPILEARCH -c -ansi -O2 -fPIC -DNON_UNIX_STDIO -DUIOLEN_int"
		./mkprodct.csh
	fi	

	# Create directories and download wxWidgets if needed
	if [ ! -d "$wxWidgets_path" ] && [ $wx_build == true ]
	then
		# Create Directories
		mkdir "$wxWidgets_path"
	
		# Change to wxWidgets directory
		cd "$wxWidgets_path"
	
		# Checkout wxWidgets source
		wx_version_download=`echo $wx_version | sed 's/\./_/g'`
		svn co http://svn.wxwidgets.org/svn/wx/wxWidgets/tags/WX_$wx_version_download/ wxWidgets-$wx_version

		# Make sure wxWidgets was downloaded
		if [ ! -d "wxWidgets-$wx_version" ]
		then
		  echo Error in wxWidgets-$wx_version download
		  wx_build=false
		fi
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
	  echo "wxWidgets already configured"
	else
	  echo "Building wxWidgets..."
	  mkdir -p "$wx_build_path"
	  cd "$wx_build_path"

	  if [ $mac == true ]
	  then
	    # Extra compile/link flags are required because OSX 10.9+ uses libc++ instead of libstdc++ by default.
	    macver=`sw_vers -productVersion`
	    macver_minor=`echo $macver | awk -F . '{print $2}'`
	    if [ $macver_minor -ge 9 ] # Check for OSX 10.9+
	    then
	      ../configure --enable-unicode --with-opengl --prefix="$wx_install_path" --with-osx_cocoa --with-macosx-version-min=10.8 CXXFLAGS="-stdlib=libc++ -std=c++11" OBJCXXFLAGS="-stdlib=libc++ -std=c++11" LDFLAGS=-stdlib=libc++
	    else
	      ../configure --enable-unicode --with-opengl --prefix="$wx_install_path" --with-osx_cocoa
	    fi
	    ncores=$(sysctl hw.ncpu | awk '{print $2}')
	  else
	    # Configure wxWidgets build
	    ../configure --enable-unicode --with-opengl --prefix="$wx_install_path"
	    ncores=$(nproc)
	  fi

	  # Compile, install, and clean wxWidgets
	  make -j$ncores
	  make install
	  cd ..; rm -Rf "$wx_build_path"
	fi
}

# Run Script Functions
download_depends
if [ $wx_build == true ]
then
  build_wxWidgets
fi

# ***********************************
# End of script
# ***********************************
echo Dependency Configuration Complete!
exit 1
