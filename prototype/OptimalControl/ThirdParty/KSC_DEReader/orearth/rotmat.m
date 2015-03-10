function r=rotmat(axis,ang)

% ROTMAT  Elementary transformation matrix
%   ROTMAT(AXIS,ANG) returns the transformation matrix representing
%   the rotation through ANG (radians) about AXIS (1,2,3).  If a *
%   coordinate system is formed by rotating the base coordinate system
%   through ANG about AXIS, then r*=ROTMAT(AXIS,ANG)*r.

ct=cos(ang);
st=sin(ang);

if axis==1

   r=[ 1   0   0
       0  ct  st
       0 -st  ct];

elseif axis==2

   r=[ct   0 -st
       0   1   0
      st   0  ct];

elseif axis==3

   r=[ ct st   0
      -st ct   0
       0   0   1];

else
   
   error('AXIS must be 1,2, or 3')

end
