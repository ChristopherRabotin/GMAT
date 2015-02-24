function rot=eulerot(ax,ang)

% EULEROT  Form direction cosine matrix from successive Euler rotations.
%   ROT = EULERROT(AX,ANG), returns the 3-by-3 direction cosine matrix from
%   successive rotations about axes AX through angles ANG.  AX and ANG are
%   n-by-1 or 1-by-n vectors and must be the same length.  Axis numbers are
%   defined such that 1 corresponds with x, 2 with y, and 3 with z and 
%   rotations are defined in a right-handed sense.  If r* is the coordinate
%   system resulting from the rotation about AX through ANG of r, then
%   r* = rot * r.

if (length(ax) ~= length(ang))
  error('AX and ANG must have the same length')
end

rot=eye(3);

for i=1:length(ax)
  rot=rotmat(ax(i),ang(i))*rot;
end
