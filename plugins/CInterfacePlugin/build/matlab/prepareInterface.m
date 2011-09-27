disp('This script builds the GMAT interface prototypes by calling ');
disp('loadlibrary. The prototype file is placed in');
disp('trunk/application/matlab/libCInterface.');

thisdir = fileparts(mfilename('fullpath'));
appdir = fullfile(thisdir, '..', '..', '..', '..', 'application');
libdir = fullfile(appdir, 'bin', 'libCInterface');
hdir = fullfile(thisdir, '..', '..', 'src', 'plugin', ...
    'CInterfaceFunctions');
incdir = fullfile(thisdir, '..', '..', 'src', 'include');
matlabdir = fullfile(appdir, 'matlab', 'libCInterface');

[notfound, warnings] = loadlibrary(libdir, hdir, ...
    'mfilename','interfacewrapper', 'includepath', incdir);

movefile('interfacewrapper.m', matlabdir);
