function A = inf(A)
%INF          Infimum of long (only with error term)
%
%  C = inf(A)
%
%For A being a long scalar or column vector with error term (see long),
%  output C is the long lower bound
%

% written  11/06/99     S.M. Rump
% modified 04/04/04     S.M. Rump  set round to nearest for safety
% modified 09/20/04     S.M. Rump  E.sign fixed  
% modified 11/19/04     S.M. Rump  exponent update for error
% modified 04/06/05     S.M. Rump  rounding unchanged
% modified 07/25/05     S.M. Rump  result improved, thanks to 
%                                      Nozomu Matsuda and Nobito Yamamoto
%

  global INTLAB_LONG_ERROR

  if ~INTLAB_LONG_ERROR
    error('inf called for long without error term')
  end

  n = length(A.sign);
  E = long(A.error.mant);
  E.exponent = E.exponent + A.error.exp;

  A.error.mant = 0;
  A.error.exp = 0;

  A = A - E;
  index = ( A.error.mant~=0 );
  if any(index)
    %VVVV  A(index) = inf(A(index));
    s.type = '()'; s.subs = {index}; A = subsasgn(A,s,inf(subsref(A,s)));
    %AAAA  Matlab bug fix
  end
