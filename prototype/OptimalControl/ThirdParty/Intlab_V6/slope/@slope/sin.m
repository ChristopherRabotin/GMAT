function u = sin(a)
%SIN          Slope sine  sin(a)
%

% written  12/06/98     S.M. Rump
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

  global INTLAB_SLOPE

  u = a;

  u.r = sin(a.r);
  indexc = 1:INTLAB_SLOPE.NUMVAR;
  indexr = 2:INTLAB_SLOPE.NUMVAR+1;
  Xxs = hull(a.r(:,indexc),a.r(:,indexr));
  u.s = cos(Xxs) .* a.s;
  
  if rndold
    setround(rndold)
  end
