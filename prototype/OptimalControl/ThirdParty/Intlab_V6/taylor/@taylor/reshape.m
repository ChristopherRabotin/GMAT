function a = reshape(a,varargin)
%RESHAPE      Reshape for Taylor vectors/matrices
%
%   r = reshape(a,vector)  or  r = reshape(a,n1,n2,...)
%
% functionality as Matlab function reshape
%

% written  05/21/09     S.M. Rump
%

  index = reshape(1:prod(a.size),a.size);
  index = reshape(index,varargin{:});
  a.size = size(index);
