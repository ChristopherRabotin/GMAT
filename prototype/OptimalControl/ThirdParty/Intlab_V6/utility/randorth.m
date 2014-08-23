function res=RandOrth(n);
%RANDORTH     Random orthogonal matrix
%
%   res = RandOrth(n);
%

% written   1/21/95     S.M. Rump
% modified 04/04/04     S.M. Rump  set round to nearest for safety
% modified 04/06/05     S.M. Rump  rounding unchanged
% modified 09/28/08     S.M. Rump  check for rounding to nearest improved
%

  e = 1e-30;
  if 1+e==1-e                           % fast check for rounding to nearest
    rndold = 0;
  else
    rndold = getround;
    setround(0)
  end

  res = orth(random(n));
  
  if rndold
    setround(rndold)
  end
