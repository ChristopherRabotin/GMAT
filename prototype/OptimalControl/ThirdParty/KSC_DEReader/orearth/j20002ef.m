function r=j20002ef(jd,de,dp);

% J20002EF Compute DCM from J2000 to ECF of date
%     J20002EF(JED) calculates the DCM from the
%     J2000 coordinate system to the ECF coordinate
%     system at the Julian date JED.  J20002EF(JED,DE,DP)
%     uses the nutations DE and DP in obliquity and
%     longitude respectively.  If not supplied, the
%     nutations are computed using WOOLARD.

l=eme12eme2(jd,2451545); % l is DCM from EME of date to EME J2000

if nargin==1 % No nutations supplied
    load woolcon
    [me,de,dp]=woolard(jd,w,c);
else % Nutations supplied
    me=mobliq(jd);
end

n=eme2tod(me,de,dp); % n is DCM from EME to TOD
g=tod2ef(jd,de,dp); % g is DCM from TOD to EF
r=g*n*l';