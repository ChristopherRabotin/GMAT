function e2t=eme2tod(meps,deps,dpsi)

% EME2TOD Compute DCM from EME of date to TOD
%    EME2TOD(MEPS,DEPS,DPSI) computes the dcm from the Earth
%    mean equator and equinox of date to the TOD equator and
%    equinox.  MEPS, DEPS, and DPSI are the mean obliquity,
%    differential obliquity and differential longitude, in
%    radians, respectively.

e2t = eulerot([1 3 1],[meps -dpsi -(meps+deps)]);

%cm=cos(meps);
%sm=sin(meps);
%ce=cos(meps+deps);
%se=sin(meps+deps);
%cp=cos(dpsi);
%sp=sin(dpsi);

%e2t = [cp         -sp*cm            -sp*sm
%       ce*sp      ce*cp*cm + se*sm  ce*cp*sm - se*cm
%       se*sp      se*cp*cm - ce*sm  se*cp*sm + ce*cm];