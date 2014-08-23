function r = reshape(a,varargin)
%RESHAPE      Reshape for slope vectors/matrices
%
%   r = reshape(a,vector)  or  r = reshape(a,n1,n2,...)
%
% functionality as Matlab function reshape
%

% written  09/28/01     S.M. Rump
% modified 04/04/04     S.M. Rump  set round to nearest for safety
% modified 04/06/05     S.M. Rump  rounding unchanged
%

  r = a;
  r.size = zeros(1,length(varargin));
  for i=1:length(r.size)
    r.size(i) = varargin{i};
  end
  if ~isequal(prod(a.size),prod(r.size))
    error('to reshape the number of elements must not change')
  end
