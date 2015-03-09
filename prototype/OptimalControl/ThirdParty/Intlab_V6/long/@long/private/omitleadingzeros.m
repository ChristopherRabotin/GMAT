function C = omitleadingzeros(C)
%OMITLEADINGZEROS  internal function for normalization
%

% written  12/30/98     S.M. Rump
% modified 11/15/04     S.M. Rump  exponent update for error
%

global INTLAB_LONG_BETA
global INTLAB_LONG_ERROR

  % zero mantissa
  indexzero = all( C.mantissa==0 , 2 );

  index = ~indexzero & ( C.mantissa(:,1)==0 );
  while any(index)
    C.mantissa(index,:) = [ C.mantissa(index,2:end) zeros(sum(index),1) ];
    C.exponent(index) = C.exponent(index) - 1;
    index = index & ( C.mantissa(:,1)==0 );
  end

  C.exponent(indexzero) = -inf;
