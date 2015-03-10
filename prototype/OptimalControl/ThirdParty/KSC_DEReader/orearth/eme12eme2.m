function l=eme12eme2(jd1,jd2)

%  EME12EME2 Transform EME from one epoch to another.
%     EME12EME2(JD1,JD2) calculates the matrix that transforms vectors
%     from an EME system at epoch Julian date JD1 (arbitrary) to an
%     EME of date system, epoch Julian date JD2.  The epoch 0
%     (J2000) enters as an intermediate system of reference.
%
%     REFERENCE:  J.H. Lieske, Astron. Astrophys. 73, 282-284 (1979).

radsec=pi/648000;

dj3=jd1-2451545;
dj4=jd2-jd1;

btl=dj3/36525;
ltl=dj4/36525;

term=(2306.2181 + btl*(1.39656 - 0.000139*btl))*ltl;

gz = (term + ltl*ltl*((0.30188 - 0.000344*btl) ...
      + 0.017998*ltl))*radsec;
rz = (term + ltl*ltl*((1.09468 + 0.000066*btl) ...
    + 0.018203*ltl))*radsec;
th = (ltl*(2004.3109 + btl*(-0.85330 + btl*(-0.000217)) ...
   + ltl*(-0.42665 + btl*(-0.000217) + ltl*(-0.041833)))) ...
    *radsec;

l=eulerot([3 2 3],[-gz th -rz]);

%cgz=cos(gz);
%sgz=sin(gz);
%crz=cos(rz);
%srz=sin(rz);
%cth=cos(th);
%sth=sin(th);

%l=[crz*cth*cgz - srz*sgz    -crz*cth*sgz - srz*cgz    -crz*sth
%   srz*cth*cgz + crz*sgz    -srz*cth*sgz + crz*cgz    -srz*sth
%   sth*cgz                  -sth*sgz                   cth    ];