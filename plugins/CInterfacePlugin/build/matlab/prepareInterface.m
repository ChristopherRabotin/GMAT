disp('This script builds the GMAT interface prototypes by calling ');
disp('loadlibrary. The prototype file is placed in');
disp('trunk/application/matlab/libCInterface.');

thisdir = fileparts(mfilename('fullpath'));
appdir = fullfile(thisdir, '..', '..', '..', '..', 'application');
libfile = fullfile(appdir, 'bin', 'libCInterface');
hfile = fullfile(thisdir, '..', '..', 'src', 'plugin', ...
    'CInterfaceFunctions');
incdir = fullfile(thisdir, '..', '..', 'src', 'include');
matlabdir = fullfile(appdir, 'matlab', 'libCInterface');

[notfound, warnings] = loadlibrary(libfile, hfile, ...
    'mfilename', 'interfacewrapper', 'includepath', incdir);

% interfacewrapper.m
movefile('interfacewrapper.m', matlabdir);

% thunk library
d = dir(['libCInterface_thunk_' computer() '*']);
for i = 1:length(d)
    movefile(d(i).name, matlabdir);
end
