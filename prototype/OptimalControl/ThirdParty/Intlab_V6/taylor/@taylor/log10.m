function a = log10(a)
%LOG10        Taylor logarithm  log10(a)
%

% written  05/22/09     S.M. Rump
%

  a = log(a);
  if isa(a.t,'intval')
    global INTLAB_INTVAL_STDFCTS_LOG10_
    a.t = a.t .* infsup(INTLAB_INTVAL_STDFCTS_LOG10_.INF,INTLAB_INTVAL_STDFCTS_LOG10_.SUP);
  else
    a.t = a.t / log(10);
  end
  