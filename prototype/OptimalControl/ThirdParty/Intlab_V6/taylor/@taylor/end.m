function i = end(x,k,n)
%END          Overloaded functions end, specifies last index
%

% written  05/21/09     S.M. Rump
%

  global INTLAB_TAYLOR_END
  if INTLAB_TAYLOR_END             % reference by {}
    i = size(x.t,1)-1;
  else                             % reference by ()
    i = x.size(k);
  end
