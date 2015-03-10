function a = sparse(a)
%SPARSE       Convert gradient to sparse
%

% written  03/06/04     S.M. Rump
% modified 04/04/04     S.M. Rump  set round to nearest for safety
%                                    Matlab sparse bug
% modified 04/06/05     S.M. Rump  rounding unchanged
% modified 12/06/05     S.M. Rump  .x part also sparse
%

  % avoid Matlab 6.5f bug: 
  % a = sparse([],[],[],1,1); a = reshape(a,1,1); abs(a)
  % produces  9.6721e-317  or similar number in underflow range
  if prod(size(a.x))~=1
    a.x = sparse(a.x);
  end
  if prod(size(a.dx))~=1
    a.dx = sparse(a.dx);
  end
