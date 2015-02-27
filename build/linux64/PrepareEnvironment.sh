#!/bin/bash         

echo "Preparing environment to build GMAT"

echo "Please enter your wxwidgets build folder path"
read -p "(the path to the folder you used when running configure): " wxPath

cp BuildEnv.mk ../../src
cp RunGmatLauncher.sh ../../application/bin
cp RunGmatLauncher.sh ../../application/bin

echo "Checking for wxWidgets and CSPICE"
if [ ! -d "$wxPath"/lib ]; then
   echo "wxwidgets libraries not found; skipping the library copy"
fi

if [ -d "$wxPath"/lib ]; then
   echo "Copying wxwidgets libraries into place"
   cp -R "$wxPath"/lib ../../application/bin 
fi

if [ ! -d ../../../Gmat3rdParty/wxwidgets ]; then
   echo "wxWidgets not found; please install or copy it to ../../../Gmat3rdParty/wxwidgets"
fi

if [ ! -d ../../../Gmat3rdParty/cspice ]; then
   echo "CSPICE not found; please install it in ../../../Gmat3rdParty/cspice"
fi

echo "Preparing plugin build folders"
cp ../../plugins/CInterfacePlugin/build/linux64/*.mk ../../plugins/CInterfacePlugin/src
cp ../../plugins/DataInterfacePlugin/build/linux64/*.mk ../../plugins/DataInterfacePlugin/src
cp ../../plugins/EphemPropagatorPlugin/build/linux64/*.mk ../../plugins/EphemPropagatorPlugin/src
cp ../../plugins/EstimationPlugin/build/linux64/*.mk ../../plugins/EstimationPlugin/src
cp ../../plugins/EventLocatorPlugin/build/linux64/*.mk ../../plugins/EventLocatorPlugin/src
cp ../../plugins/ExtraPropagatorsPlugin/build/linux64/*.mk ../../plugins/ExtraPropagatorsPlugin/src
#cp ../../plugins/FminconOptimizerPlugin/build/linux64/*.mk ../../plugins/FminconOptimizerPlugin/src
cp ../../plugins/FormationPlugin/build/linux64/*.mk ../../plugins/FormationPlugin/src
cp ../../plugins/GeometricMeasurementPlugin/build/linux64/*.mk ../../plugins/GeometricMeasurementPlugin/src
cp ../../plugins/GmatFunctionPlugin/build/linux64/*.mk ../../plugins/GmatFunctionPlugin/src
cp ../../plugins/MatlabInterfacePlugin/build/linux64/*.mk ../../plugins/MatlabInterfacePlugin/src
cp ../../plugins/ProductionPropagatorPlugin/build/linux64/*.mk ../../plugins/ProductionPropagatorPlugin/src
cp ../../plugins/SaveCommandPlugin/build/linux64/*.mk ../../plugins/SaveCommandPlugin/src
cp ../../plugins/StationPlugin/build/linux64/*.mk ../../plugins/StationPlugin/src

echo ""
echo "Environment files in place.  Please edit the following files for your local folder settings:"
echo ""
echo "   Build files:"
echo "      src/BuildEnv.mk"
echo ""
echo "   Run time files:"
echo "      application/bin/RunGmatLauncher.sh"
echo ""
echo ""
echo "To build GMAT with MATLAB:"
echo "   Edit ../../plugins/MatlabInterfacePlugin/src/MatlabInterfaceEnv.mk to add your MATLAB path"
echo "   then build from the GMAT install folder using the command"
echo "   make PLATFORM=linux"
echo ""
echo "To build GMAT without MATLAB, from the GMAT install folder use the command"
echo "   make PLATFORM=nomatlab"
echo ""
echo "You can clean up from a build using"
echo "   make PLATFORM=<your options> clean"
echo ""
echo "or create a clean rebuild using"
echo "   make PLATFORM=<your options> clean all"
echo ""
echo ""
echo "Thank you for using GMAT on Linux!"
echo ""
