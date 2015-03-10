function t2e=tod2ef(jd,deps,dpsi)

% TOD2EF Calculate DCM from TOD to Earth fixed system
%     TOD2EF(JD,DEPS,DPSI) calculates the direction cosine
%     matrix relating the true-of-date equinox/pole system to the
%     earth-fixed system.  If DEPS and DPSI are not supplied
%     they are calculated using WOOLARD.
%
%                   TOD                   Earth Fixed
%     X     True Vernal equinox   Equatorial plane at 0 deg longitude
%     Y     Completes RH triad    Completes RH triad
%     Z     North Pole            North Pole
%
%     JD is the Julian Date.  DEPS and DPSI are the 
%     differential obliquity and differential precession
%     of date in radians.

meps=mobliq(jd);

if nargin==1 % No nutations supplied
    load woolcon
    [meps,deps,dpsi]=woolard(jd,w,c);
else % Nutations supplied
    meps=mobliq(jd);
end    

dj5=jd-2451545;
djt=fix(dj5);
frac=dj5-djt;

if(dj5 > 0)
    if (frac < 0.5)
        days = djt - 0.5;
        dayprt = frac + 0.5;
    else
        days = djt + 0.5;
        dayprt = frac - 0.5;
    end
else
    if (abs(frac) <= 0.5)
        days = djt - 0.5;
        dayprt = frac + 0.5;
    else
        days = djt - 1.5;
        dayprt = frac + 1.5;
    end
end

tu = days/36525;

% gmst0 is Greenwich mean sidereal time for 0 hr UTI
% 1 mean solar day = 1.00273790934 mean sidereal days 

gmst0 = 24110.54841 + tu*(8640184.812866 + tu*(0.093104 - tu*0.0000062));
revs = gmst0/86400 + 1.00273790934*dayprt;

cycle = revs - fix(revs);

if (cycle < 0)
    cycle = cycle + 1;
end

gxm = 2*pi*cycle;
btg = dj5/36525;

gx = gxm + dpsi*cos(meps+deps);

t2e = rotmat(3,gx);