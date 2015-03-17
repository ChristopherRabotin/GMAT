% $Id$
% GMAT: General Mission Analysis Tool.
% 
%  Copyright (c) 2002-2011 United States Government as represented by the
%  Administrator of The National Aeronautics and Space Administration.
%  All Other Rights Reserved.
%
% Modified 11/6/2014 by Ravi Mathur (Emergent Space Technologies, Inc.)
% Adapted to create prototypes using installed GMAT CInterface instead of
% in-build CInterface.

disp('This script builds the GMAT interface prototype files and places them');
disp('in the appropriate GMAT folder for Matlab use.');

% Make sure GMAT is installed and in MATLAB's search path
if(exist('findgmatrootpath') ~= 2)
    error('GMAT:prepareInterface', ...
    ['GMAT not found in MATLAB path. Ensure you have a valid GMAT ' ...
    'installation and its appropriate paths added to your MATLAB path.']);
end

% Check that ROOT_PATH of startup file is a valid GMAT installation
root_path = findgmatrootpath;

% Path to folder where prototypes should be installed
destdir = fullfile(root_path, 'matlab', 'libCInterface');
disp('Matlab prototype files will be placed in:');
disp(destdir);

% Path to CInterface shared library
libname = 'libCInterface';
libfile = fullfile(root_path, 'bin', libname);

% Path to header file describing library functions
thisdir = fileparts(mfilename('fullpath'));
hfile = fullfile(thisdir, '..', '..', 'src', 'plugin', ...
    'CInterfaceFunctions');

% Path to folder with additional CInterface include files
incdir = fullfile(thisdir, '..', '..', 'src', 'include');

% Generate CInterface MATLAB prototypes
[notfound, warnings] = loadlibrary(libfile, hfile, ...
    'mfilename', 'interfacewrapper', 'includepath', incdir);
unloadlibrary(libname);

% Install prototype wrapper
disp('Moving file interfacewrapper.m');
movefile('interfacewrapper.m', destdir);

% Install thunk files
validexts = {'.dll', '.dylib', '.so'};
d = dir(['libCInterface_thunk_*']);
for i = 1:length(d)
    [~,~,ext] = fileparts(d(i).name);
    if(isempty(strmatch(ext, validexts, 'exact')))
        delete(d(i).name);
    else
        disp(['Moving file ' d(i).name]);
        movefile(d(i).name, destdir);
    end
end
