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

disp('This script builds the GMAT interface prototypes by calling');
disp('loadlibrary. The prototype files are placed in');
disp('GMAT/matlab/libCInterface/');

% Make sure GMAT is installed and in MATLAB's search path
if(exist('findgmatrootpath') ~= 2)
    error('GMAT:prepareInterface', ...
    ['GMAT not installed in MATLAB path. Ensure you have a valid GMAT ' ...
    'installation and its top-level path is added to your MATLAB path.']);
end

% Check that ROOT_PATH of startup file is a valid GMAT installation
root_path = findgmatrootpath;
if ~exist(fullfile(root_path, 'bin', 'gmat_startup_file.txt'))
    error('GMAT:prepareInterface', ...
        ['The ROOT_PATH variable in gmat_startup_file.txt does not '...
        'point to a valid GMAT installation.']);
end

thisdir = fileparts(mfilename('fullpath'));

% Path to CInterface shared library
libfile = fullfile(root_path, 'bin', 'libCInterface');

% Path to header file describing library functions
hfile = fullfile(thisdir, '..', '..', 'src', 'plugin', ...
    'CInterfaceFunctions');

% Path to folder with additional CInterface include files
incdir = fullfile(thisdir, '..', '..', 'src', 'include');

% Path to folder where prototypes should be installed
destdir = fullfile(root_path, 'matlab', 'libCInterface');

% Generate CInterface MATLAB prototypes
[notfound, warnings] = loadlibrary(libfile, hfile, ...
    'mfilename', 'interfacewrapper', 'includepath', incdir);

% Install prototype wrapper
movefile('interfacewrapper.m', destdir);

% Install thunk files
d = dir(['libCInterface_thunk_' computer() '*']);
for i = 1:length(d)
    movefile(d(i).name, destdir);
end
