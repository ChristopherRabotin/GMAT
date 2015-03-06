function A = hankel(c,r)
%HANKEL       Implements  hankel(c,r)  for intervals
%
%   A = hankel(c,r)
%
% functionality as Matlab function toeplitz
%

% written  11/07/08     S.M. Rump
%

  if nargin==1
    if c.complex
      A = midrad( hankel(c.mid) , hankel(c.rad) );
    else
      A = infsup( hankel(c.inf) , hankel(c.sup) );
    end
  else
    c = intval(c);
    r = intval(r);
    if c.complex | r.complex
      A = midrad( hankel(c.mid,r.mid) , hankel(c.rad,r.rad) );
    else
      A = infsup( hankel(c.inf,r.inf) , hankel(c.sup,r.sup) );
    end
  end
