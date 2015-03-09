function y = testfuntaylor(x)
%TESTFUNTAYLOR   A test function for the Taylor toolbox
%

% written  05/29/09     S.M. Rump
%

  if isintval(x)
    Pi = 4*atan(intval(1));
  else
    Pi = pi;
  end
  y = sin(Pi*x)-sin(x);
