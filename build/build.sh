#!/bin/bash
# Author: 	Jfisher
# Project:	Gmat
# Title:	configure.sh
# Purpose:	Shell script to configure and easily build gmat for end users
# Usage: 	build.sh [-arch (x86 | x64)]
#               Default behavior auto-detects architecture and OS

# Clear the screen
clear

# Intro Text
echo "........................................................"
echo "Starting GMAT Build"
echo "........................................................"
echo

# ***********************************
# Input Args
# ***********************************

# Auto-detect architecture if not input
if [ "$1" = "-arch" ]
then
  arch=$2
elif [[ "$(uname -m)" = *"64" ]]
then
  arch="x64"
else
  arch="x86"
fi

# Auto-detect Mac/Linux
if [ "$(uname)" = "Darwin" ] 
then
  mac=true
else
  mac=false
fi

# ***********************************
# Make File Generation and Build
# ***********************************

# Change to build/os directory
if [ "$mac" = true ]
then
  cd ../build/macosx
else 
  cd ../build/linux
fi

# Generate unix makefiles
if [ "$arch" = "x86" ]
then
  cmake -G "Unix Makefiles" -D 64_BIT=OFF ../../src/
else
  cmake -G "Unix Makefiles" -D 64_BIT=ON ../../src/
fi

# Make Gmat
make

# Change back to build directory
cd ../

echo ""
echo "*************************************"
echo "Gmat Build Finished. Check build output for status."
echo "*************************************"
