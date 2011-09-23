disp('This script builds the GMAT interface prototypes by calling ');
disp('loadlibrary.  The loadlibrary call will fail; this is expected.');
disp('The interfacewrapper should be built anyway.');

[notfound, warnings] = loadlibrary('../lib/libCInterface',...
    '../src/plugin/CInterfaceFunctions','mfilename','interfacewrapper',...
    'includepath','../src/include');
