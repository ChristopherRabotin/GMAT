function meps=mobliq(jd)

% MOBLIQ Calculate mean obliquity
%     MOBLIQ(JD) returns the mean obliquity MEPS in radians at
%     Julian date JD.

% Convert jd to Julian centuries since 2000.0
btw=(jd-2451545)/36525;
radsec=pi/648000; % 1 arcsec in radians

% Mean obliquity
meps=(84381.448-btw.*(46.815+btw.*(0.00059-0.001813.*btw)))*radsec;
