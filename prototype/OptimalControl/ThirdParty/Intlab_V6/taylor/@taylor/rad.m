function a = rad(a)
%RAD          Taylor radius
%
%  r = rad(a)
%

% written  05/21/09     S.M. Rump
%

  a.t = rad(a.t);
  if isequal(a.t,0)
    global INTLAB_TAYLOR_ORDER
    K = INTLAB_TAYLOR_ORDER+1;
    a.t = zeros(K,prod(a.size));
  end
