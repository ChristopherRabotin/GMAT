function [meps,deleps,delpsi]=woolard(jd,w,c)

% WOOLARD Calculate mean obliquity, nutation in longitude and obliquity
%     [MEPS,DELEPS,DELPSI]=WOOLARD(JD,W,C) returns the mean 
%     obliquity MEPS, nutation in obliquity DELEPS, and nutation in
%     longitude DELPSI in radians.  The inputs are:
%
%          JD  = Julian Date
%          W   = Coefficients used in calculating DELEPS and DELPSI
%          C   = Coefficients used in calculating angular arguments
%
%     W and C are in woolard.mat

radsec=pi/648000; % 1 arcsec in radians

% Mean obliquity
meps=mobliq(jd);

% Convert JD from Julian date to time since J2000 in Julian cent.
btw=(jd-2451545)/36525;

dum=c(2,:).*btw-fix(c(2,:).*btw);
prtrev=1296000*dum;
term=(c(1,:)+prtrev+btw.*(c(3,:)+btw.*(c(4,:)+btw.*c(5,:))))*radsec;

arg=(term*w(:,1:5)')';
delpsi=sum((w(:,6)+btw.*w(:,7)).*sin(arg))/10000*radsec;
deleps=sum((w(:,8)+btw.*w(:,9)).*cos(arg))/10000*radsec;