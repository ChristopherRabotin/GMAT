function a = sparse(a)
%SPARSE       Convert slope to sparse
%

% written  04/06/04     S.M. Rump
% modified 04/06/05     S.M. Rump  rounding unchanged
%

  a.r = sparse(a.r);
  a.s = sparse(a.s);
