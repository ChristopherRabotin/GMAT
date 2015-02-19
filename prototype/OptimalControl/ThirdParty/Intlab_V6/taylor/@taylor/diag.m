function a = diag(a,k)
%DIAG         Implements  diag(a,k)  for Taylor
%
%   c = diag(a,k)
%
% functionality as Matlab function diag for matrices
%

% written  05/21/09     S.M. Rump
%

  if nargin==1
    k = 0;
  end
  
  if any(a.size==1)
    global INTLAB_TAYLOR_ORDER
    K = INTLAB_TAYLOR_ORDER+1;
    index = ( diag(1:prod(a.size),k)~= 0 );
    a.size = size(index);
    at = a.t;
    a.t = zeros(K,prod(size(index)));
    if isa(at,'intval')
      a.t = intval(a.t);
    end
    a.t(:,index) = at;
  else
    index = 1:prod(a.size);
    index = diag(reshape(index,a.size),k);
    a.size = [length(index) 1];
    a.t = a.t(:,index);
  end
