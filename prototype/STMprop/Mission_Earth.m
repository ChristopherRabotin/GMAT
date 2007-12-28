
%  This script tests propagation using Earth as the central body.  


% --------------------------------------
%          Define the Force Model
% --------------------------------------
ForceModel.CentralBody = 'Earth';
ForceModel.PrimaryBodies = {'Earth'};
ForceModel.PointMasses = {'Earth' 'Luna' 'Sun'};
ForceModel.SRP = 'On';
ForceModel.SolarRadiationPressure.Flux = 1367;
ForceModel.SolarRadiationPressure.Nominal_Sun = 149597870.691;

% --------------------------------------
%          Define the Spacecraft
% --------------------------------------
MMSRef.Epoch = '27 Jul 2000 20:48:50.014';
MMSRef.X = 7482.391175343022;
MMSRef.Y = -4121.720425267366;
MMSRef.Z = -1169.794220760123;
MMSRef.VX = 4.43353923085319;
MMSRef.VY = 8.266695838139212;
MMSRef.VZ = -1.56943034698562;

% --------------------------------------
%          Propagate the Spacecraft
% --------------------------------------
TOF     = 86400*.5;
[t, X, Phi] = Propagate(MMSRef,ForceModel,TOF);

% --------------------------------------
%          Output
% --------------------------------------
disp('  ')
disp('-------------------------------')
disp('----Propagation Complete-------')
disp('-------------------------------')
disp('  ')
n =  size(X,1);
disp(['               X-Comp.            Y-Comp.           Z- Comp.    ']);
disp(['Position   ' num2str( X(n,1:3),12) ]);
disp(['Velocity   ' num2str( X(n,4:6),12) ]);
disp('  ')








