#!/bin/bash
# Author: 	Jfisher
# Project:	Gmat
# Title:	configure.sh
# Purpose:	Windows Script to configure and easily build gmat for end users
# Usage: 	-arch [x86 | x64] -mac

# Clear the screen
clear

# Intro Text
echo "........................................................"
echo "Starting GMAT Build"
echo "........................................................"
echo

# Set default variables
arch="x86"
mac=false

# ***********************************
# Input Args
# ***********************************
if [ "$1" = "-arch" ]
then
	arch=$2
fi

if [ "$3" = "-mac" ] 
then
	mac=true
fi

# ***********************************
# Make File Generation and Build
# ***********************************

# Change to build/os directory
if [ $mac == true ]
then
		cd ../build/macosx
else 
		cd ../build/linux
fi

# Generate unix makefiles
if [ "$arch" = "x86" ]
then
	cmake -G "Unix Makefiles" ../../src/
else
	cmake -G "Unix Makefiles" -D 64_BIT=true ../../src/
fi

# Make Gmat
make

# Change back to build directory
cd ../

echo ""
echo "*************************************"
echo "Gmat Build Finished Succesfully!"
echo "*************************************"
