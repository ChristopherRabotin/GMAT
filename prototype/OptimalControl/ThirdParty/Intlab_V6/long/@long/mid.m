function C = mid(A)
%MID          Midpoint of long (only with error term)
%
%  C = mid(A)
%

% written  11/06/99     S.M. Rump
% modified 04/04/04     S.M. Rump  set round to nearest for safety
% modified 04/06/05     S.M. Rump  rounding unchanged
%

  global INTLAB_LONG_ERROR

  if ~INTLAB_LONG_ERROR
    error('mid called for long without error term')
  end

  C = A;
  C.error.mant = 0;
  C.error.exp = 0;
