function t=chebyval(x,n)

% CHEBYVAL Evaluate Chebyshev polynomial
%     CHEBYVAL(X,N) returns the zeroth through the Nth Chebyshev
%     polynomial evaluated at X.  If X is scalar, CHEBYVAL returns
%     an N+1-by-1 vector whose elements are the zeroth through Nth
%     Chebyshev polynomials evaluated at X. If X is a p-by-1 or 
%     1-by-p vector, CHEBYVAL returns a p-by-N+1 or N+1-by-p matrix.
%     If X is a matrix, CHEBYVAL returns a matrix of size(X)-by-N+1.

c=size(x);
x=x(:); % Unwind x for easier evaluation
t=[ones(size(x)) x zeros(size(x,1),n-1)]; % pre-allocate t

for i=3:n+1
    t(:,i)=2*x.*t(:,i-1)-t(:,i-2); % Recurrence relation for Cheby. pol.
end

% Reshape t for output

if length(c)==2 & c(1)==1
    t=t';
elseif length(c)==2 & c(2)==1
else
    t=reshape(t,[c,n+1]);
end