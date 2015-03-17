function x = flip(x)
%FLIP         Flip array along the first non-singleton 
%
%Call   y = flip(x), especially useful for vectors (distinction between fliplr and 
%  flipud not necessary).
%

% written  10/25/02     S.M. Rump
% modified 04/04/04     S.M. Rump  set round to nearest for safety
% modified 04/06/05     S.M. Rump  rounding unchanged
%

  s = size(x);
  i = find(s~=1);
  if ~isempty(i)
    x = flipdim(x,i(1));
  end
