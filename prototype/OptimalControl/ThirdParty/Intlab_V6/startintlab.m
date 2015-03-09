function startintlab
%STARTINTLAB  Initialization of paths, global variables etc.
%             Adapt this to your local needs
%

% written  10/16/98   S.M. Rump
% modified 11/05/98   allow blanks in path name, Max Jerrell, Flagstaff
% modified 10/23/99   S.M. Rump  change directory before intvalinit('init'),
%                                work directory
% modified 12/15/01   S.M. Rump  displaywidth added
% modified 12/08/02   S.M. Rump  reset directory to old dir (Matlab problems under unix)
% modified 12/18/02   S.M. Rump  Hessians added
% modified 04/04/04   S.M. Rump  working directory added, clear removed, changed to function
%                                  adapted to new functionalities
% modified 11/09/05   S.M. Rump  working dir and cd(INTLABPATH) removed, INTLABPATH computed
% modified 02/11/06   S.M. Rump  SparseInfNanFlag removed
% modified 06/15/06   S.M. Rump  directory name corrected (thanks to Jaap van de Griend)
% modified 05/09/07   S.M. Rump  path corrected (thanks to Bastian Ebeling)
% modified 09/10/07   S.M. Rump  INTLAB path
% modified 11/26/07   S.M. Rump  ordering of default settings
% modified 02/19/08   S.M. Rump  rename setround paths, avoid change of code
% modified 05/19/08   S.M. Rump  reorganization of setround
% modified 05/26/08   S.M. Rump  rehash added
% modified 05/09/09   S.M. Rump  directory AccSumDot added, warning for "format" supressed
% modified 05/25/09   S.M. Rump  directory taylor added
% modified 01/17/10   S.M. Rump  autoamd switched off
% modified 04/08/10   S.M. Rump  extra checkrounding omitted
%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Default setting:  please change these lines to your needs %%
%VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV

% Intlab directory

% Defines INTLABPATH to be the directory, in which this file "startintlab" is contained
  dir_startintlab = which('startintlab');
  INTLABPATH = dir_startintlab(1:end-13);

% If INTLAB is contained in another directory, please uncomment and adapt this line
% INTLABPATH = 'C:\INTLAB VERSIONS\INTLAB\';           % blanks allowed 


%AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%% add new paths
 
  wng = warning;          % supress warning that "format" is overloaded
  warning off
  addpath( [ INTLABPATH ]                       , ...
           [ INTLABPATH 'intval' ]              , ...
           [ INTLABPATH 'gradient' ]            , ...
           [ INTLABPATH 'hessian' ]             , ...
           [ INTLABPATH 'taylor' ]              , ...
           [ INTLABPATH 'slope' ]               , ...
           [ INTLABPATH 'polynom' ]             , ...
           [ INTLABPATH 'utility' ]             , ...
           [ INTLABPATH 'long' ]                , ...
           [ INTLABPATH 'accsumdot' ]           , ...
           [ INTLABPATH 'demos' ])
  path(path)			              % make sure paths are correct
											  % works in all Matlab versions
  warning(wng);

%%%%%%%%%% set INTLAB environment

  format compact
  format short
  try
    feature jit off                % avoid change of code
  end
  try
    feature accel off		    
  end


%%%%%%%%%% initialize sparse systems

  spparms('autoamd',0);            % switch off automatic band reduction
  spparms('autommd',0);            % ( switching on may slow down sparse
                                   %   computations significantly )


%%%%%%%%%% initialize interval toolbox (see "help intvalinit")

  intvalinit('Init',1,INTLABPATH)  % Initialize global constants


%%%%%%%%%% initialize gradient toolbox

  gradientinit


%%%%%%%%%% initialize Hessian toolbox

  hessianinit


%%%%%%%%%% initialize slope toolbox

  slopeinit


%%%%%%%%%% initialize long toolbox

  longinit('Init')


%%%%%%%%%% initialize polynom toolbox

  polynominit('Init')


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%  Default setting:  change these lines to your needs   %%%%%%%%%%%%%%
%VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV

  displaywidth(120);               % width of output

  intvalinit('Display_')
  intvalinit('RealStdFctsExcptnWarn')
  intvalinit('ImprovedResidual')
  intvalinit('LssFirstSecondStage')
  intvalinit('FastIVMult')
  intvalinit('RealComplexAssignAuto')
  intvalinit('ComplexInfSupAssignWarn')

  longinit('WithErrorTerm')

  polynominit('DisplayUPolySparse')
  polynominit('EvaluateUPolyHorner')
  polynominit('EvaluateMPolyPower')
  polynominit('AccessVariableWarn')

  slopeinit('SlopeSparseArrayDerivWarn')

  sparsegradient(50);     % Gradients stored sparse for fifty and more variables
  gradientinit('GradientSparseArrayDerivWarn')

  sparsehessian(10);      % Hessians stored sparse for ten and more variables
  hessianinit('HessianSparseArrayDerivWarn')

%AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  intvalinit('license')
  intlablogo([0:4:32 32:-4:24 24:4:32])
  pause(0.5)
  close
  
  
%%%%%%%%%% store current setting as default setting
  
  intvalinit('StoreDefaultSetting')
  
  
%%%%%%%%%% set working environment

setround(0)             % set rounding to nearest

% uncomment and adapt this statement if necessary
% cd('c:\rump\matlab\work')
  