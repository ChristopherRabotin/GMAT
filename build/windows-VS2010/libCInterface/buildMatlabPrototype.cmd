rem Builds the prototype file for MATLAB's loadlibrary function.
rem Requires MATLAB in the path.

matlab -nosplash -minimize -r "addpath('..\..\..\plugins\CInterfacePlugin\build\matlab'); prepareInterface; quit"
