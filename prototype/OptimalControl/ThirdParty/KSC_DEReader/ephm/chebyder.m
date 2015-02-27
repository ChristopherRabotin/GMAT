function tp=chebyder(x,t)

% CHEBYVAL Evaluate Chebyshev polynomial derivatives
%     CHEBYVAL(X,T) returns the Chebyshev polynomial derivatives
%     corresponding to the Chebyshev polynomials evaluated at X.
%     T is the matrix of Chebyshev polynomials evaluated at X
%     (see CHEBYVAL).

% Determine dimensions

c=size(t);
cc=size(x);
n=c(length(c))-1;
if length(c)==2
    n=max(c)-1;
end

% Unwind pc and x for easier evaluation

x=x(:);
t=t(:)';

tp=[zeros(size(x)) ones(size(x)) zeros(size(x,1),n-1)]; % pre-allocate tp

% Differentiate recurrence relation for Chebyshev polynomials

for i=3:n+1
    tp(:,i)=2*(t(:,i-1)+x.*tp(:,i-1))-tp(:,i-2);
end

% Reshape tp to be the same dimensions as t

if length(cc)==2 & cc(1)==1
    tp=tp';
elseif length(cc)==2 & cc(2)==1
else
    tp=reshape(tp,[cc,n+1]);
end