function y = slope(a)
%SLOPE        Slope class constructor for interval input
%
%   y = slope(a)
%
%An explicit call of the constructor is only necessary to initialize
%  a constant to be of type slope. Otherwise, any operation with a
%  slope variable produces a result of type slope.
%

% written  12/06/98     S.M. Rump
% modified 04/04/04     S.M. Rump  set round to nearest for safety
% modified 04/06/05     S.M. Rump  rounding unchanged
%

  global INTLAB_SLOPE

  if INTLAB_SLOPE.NUMVAR==0
    error('Slope expansion not initialized')
  end

  dummy.init = a;
  y = slope(dummy,'slope');
