function A = toeplitz(c,r)
%TOEPLITZ     Implements  toeplitz(c,r)  for intervals
%
%   A = toeplitz(c,r)
%
% functionality as Matlab function toeplitz
%

% written  02/20/01     S.M. Rump
% modified 04/04/04     S.M. Rump  set round to nearest for safety
% modified 04/06/05     S.M. Rump  rounding unchanged
% modified 11/07/08     S.M. Rump  help
%

  if nargin==1
    if c.complex
      A = midrad( toeplitz(c.mid) , toeplitz(c.rad) );
    else
      A = infsup( toeplitz(c.inf) , toeplitz(c.sup) );
    end
  else
    c = intval(c);
    r = intval(r);
    if c.complex | r.complex
      A = midrad( toeplitz(c.mid,r.mid) , toeplitz(c.rad,r.rad) );
    else
      A = infsup( toeplitz(c.inf,r.inf) , toeplitz(c.sup,r.sup) );
    end
  end
