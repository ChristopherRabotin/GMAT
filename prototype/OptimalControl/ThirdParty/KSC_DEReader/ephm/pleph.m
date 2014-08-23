function [r,v]=pleph(et,ntarg,ncent,kmflag)

% PLEPH  Read JPL planetary ephemeris and give body states
%    [R,V]=PLEPH(ET,NTARG,NCENT,KMFLAG) returns the position R
%    and velocity V of body NTARG relative to body NCENT at Julian
%    date ET.  The numbering convention for NTARG and NCENT is:
%
%     1 -> Mercury          8 -> Neptune
%     2 -> Venus            9 -> Pluto
%     3 -> Earth           10 -> Moon
%     4 -> Mars            11 -> Sun
%     5 -> Jupiter         12 -> Solar system barycenter
%     6 -> Saturn          13 -> Earth-Moon barycenter
%     7 -> Uranus          14 -> Nutations (longitude and obliquity)
%               15 -> Librations, if on file
%
%    If nutations or librations are desired, set NCENT = 0.
%
%    Units for position and velocity are AU and AU/day or, if KMFLAG
%    is nonzero, km and km/s.  Units for nutations and librations
%    are radians and radians/day.

% Declare global variables

eph_global

% Handle NTARG=NCENT

%if ntarg==ncent   % Degenerate case
%    r = zeros(3,1);
%    v = zeros(3,1);
%    return
%end

% Set up LIST for call to STATE_EPH

list=zeros(1,12);

% Check for nutation call

if ntarg==14 | ncent==14
    list(11)=2;
    [r,v]=state_eph(et,list,1,kmflag);
    r=r(1:2,12);
    v=v(1:2,12);
    return
end

%Check for libration call

if ntarg==15 | ncent==15
    list(12)=2;
    [r,v]=state_eph(et,list,1,kmflag);
    r=r(:,13);
    v=v(:,13);
    return
end

% Check for Earth call

for i=1:2
    k = ntarg;
    if i ==2, k = ncent; end
    if k <= 10, list(k)  = 2; end % Planets and Moon
    if k == 10, list(3)  = 2; end % If Moon, get EM-Bary
    if k ==  3, list(10) = 2; end % If Earth, get Moon
    if k == 13, list(3)  = 2; end % If EM-bary, get EM-Bary
end

% Make call to STATE_EPH

[pv,vv]=state_eph(et,list,1,kmflag);

if ntarg == 13 | ncent == 13 % EM-Bary
    pv(:,13) = pv(:,3);
    vv(:,13) = vv(:,3);
end

if ntarg*ncent == 30 & ntarg+ncent == 13 % Earth and Moon
    pv(:,3)=zeros(3,1);
    vv(:,3)=zeros(3,1);
else
    if list(3) == 2 % Get Earth from EM-Bary and Moon
        pv(:,3)=pv(:,3)-pv(:,10)/(1+EMRAT);
        vv(:,3)=vv(:,3)-vv(:,10)/(1+EMRAT);
    end
    if list(10) ==2 % Get Moon from EM-Bary and Geocentric Moon
        pv(:,10)=pv(:,3)+pv(:,10);
        vv(:,10)=vv(:,3)+vv(:,10);
    end
end

% Get NTARG relative to NCENT
r=pv(:,ntarg)-pv(:,ncent);
v=vv(:,ntarg)-vv(:,ncent);