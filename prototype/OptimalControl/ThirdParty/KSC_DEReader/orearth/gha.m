function g = gha(jd,deps,dpsi)

% GHA Calculate Greenwich Hour Angle
%     GHA(JD,DEPS,DPSI) returns the Greenwich
%     hour angle, i.e. the angle measured from
%     0 deg longitude to the true-of-date vernal
%     equinox (first point of Aries).  JD is the
%     julian date, deps is the differential obliquity
%     and dpsi is the differential precession.
%     If DEPS and DPSI are not supplied, they are
%     calculated using WOOLCON.

if nargin == 3
    t2ef = tod2ef(jd,deps,dpsi);
else
    t2ef = tod2ef(jd);
end

g = atan2(-t2ef(2,1),t2ef(1,1));
