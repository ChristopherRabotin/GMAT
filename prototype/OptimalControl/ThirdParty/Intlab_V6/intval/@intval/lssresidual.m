function res = lssresidual(A,x,b)
%LSSRESIDUAL    Improved inclusion of residual b-A*x or I-R*A
%Typical calls are
%
%   res = lssresidual(A,x,b)
%
%Input   A     nxn real or complex point matrix (dense or sparse)
%        x     some approximation to A\b
%        b     real or complex point or interval vector or nxk matrix
%Output  res   approximation of b-A*x
%
%or, for dense A, 
%
%   res = lssresidual(R,A,intval(speye(n)))
%
%Input   R     nxn real or complex point matrix
%        A     nxn real or complex point matrix
%Output  res   approximation of I-R*A
%
%Note that the fact that the last parameter is of type intval causes computation of an
%inclusion of the result rather than an approximation. Also note that only the last
%parameter is allowed to be of type intval, not one of the factors.
%
%This routine can be used in verifylss to improve accuracy of inclusion. Automatic use
%can be switched on and off, see intvalinit. Basically, the factors A and x are split
%into an upper and lower part and treated seperately. Fast way of splitting is taken from
%  T.J. Dekker: A floating-point technique for extending the available precision,
%    Numerische Mathematik 18:224-242, 1971.
%For timing and improvement of accuracy of inclusion see the table below.
%
%For randomly generated full matrices of dimension n and condition number 1e8, and 
%with b=a*(2*rand(n,1)-1) the following table lists the computing time t1 w/o and 
%t2 with improved residual in seconds on a 750 MHz Pentium III Laptop, as well as 
%the ratio t1/t2. Furthermore, the median of the radius of the inclusion is given 
%in r1 and r2 and the improvement r1/r2 in radius, respectively.
%
%     n     t1     t2      t2/t1     r1       r2     r1/r2      inclusion of b-A*x
%----------------------------------------------------------
%   200   0.006   0.033     5.9   3.1e-14  1.4e-19  2.2e+5
%   500   0.042   0.191     4.6   1.3e-13  6.0e-19  2.3e+5
%  1000   0.082   0.779     9.5   3.8e-13  2.0e-18  1.9e+5
%
%Similar numbers (in seconds) for a sparse matrix of dimensions 1000, 5000 and 10000 with 
%some 20 nonzero elements per row are as follows:
%
%     n     t1     t2      t2/t1     r1       r2     r1/r2      inclusionn of b-A*x
%----------------------------------------------------------
%  1000   0.013   0.118     9.1   1.2e-15  4.2e-21  2.8e+5
%  5000   0.078   0.648     8.3   1.2e-15  4.2e-21  2.8e+5
% 10000   0.158   1.324     8.4   1.2e-15  4.2e-21  2.8e+5
%
%Similar numbers for a random nxn matrix A, an approximate inverse R and timing for I-R*A:
%
%     n     t1     t2      t2/t1     r1       r2     r1/r2      inclusion of I-R*A
%----------------------------------------------------------
%   200   0.110   0.314     2.9   6.2e-15  4.5e-20  1.4e+5
%   500   1.492   4.391     2.9   5.0e-14  4.1e-19  1.2e+5
%  1000  10.745  32.497     3.0   9.7e-14  8.5e-19  1.1e+5
%

% written  04/02/02     S.M. Rump
% modified 04/04/04     S.M. Rump  set round to nearest for safety
% modified 04/06/05     S.M. Rump  rounding unchanged
% modified 11/20/05     S.M. Rump  fast check for rounding to nearest
% modified 05/30/07     S.M. Rump  typo
%

  e = 1e-30;
  if 1+e==1-e                           % fast check for rounding to nearest
    rndold = 0;
  else
    rndold = getround;
    setround(0)
  end

  if isa(A,'intval')                  % A interval
    error('Input A must be no interval')
  end
  
  if isa(x,'intval')                  % x interval
    error('Input x must be no interval')
  end
  
  if isreal(A)                        % A real
    if isreal(x)                    % x real
      if b.complex                % b complex
        error('Input A and x is real, so complex b makes no sense')
      else                        % A,x,b real
        res.complex = 0;
        [res.inf,res.sup] = lssresid(A,x,b.inf,b.sup);    
        res.mid = [];
        res.rad = [];
      end
    else                            % A real, x complex
      if b.complex
        setround(-1)                % convert intval b to inf/sup
        binf = b.mid - b.rad;
        setround(1)
        bsup = b.mid + b.rad;
      else
        binf = b.inf;
        bsup = b.sup;
      end
      [resrealinf,resrealsup] = lssresid(A,real(x),real(binf),real(bsup));
      [resimaginf,resimagsup] = lssresid(A,imag(x),imag(binf),imag(bsup));
      res.complex = 1;
      res.inf = [];
      res.sup = [];
      setround(1)
      res.mid = complex( resrealinf + 0.5*(resrealsup-resrealinf) , resimaginf + 0.5*(resimagsup-resimaginf) );
      res.rad = res.mid - complex( resrealinf , resimaginf );
    end
  else                                % A complex, no interval
    if isreal(x)                    % x real
      if b.complex
        setround(-1)                % convert intval b to inf/sup
        binf = b.mid - b.rad;
        setround(1)
        bsup = b.mid + b.rad;
      else
        binf = b.inf;
        bsup = b.sup;
      end
      [resrealinf,resrealsup] = lssresid(real(A),x,real(binf),real(bsup));
      [resimaginf,resimagsup] = lssresid(imag(A),x,imag(binf),imag(bsup));
      res.complex = 1;
      res.inf = [];
      res.sup = [];
      setround(1)
      res.mid = complex( resrealinf + 0.5*(resrealsup-resrealinf) , resimaginf + 0.5*(resimagsup-resimaginf) );
      res.rad = res.mid - complex( resrealinf , resimaginf );
    else                            % A and x complex
      if b.complex
        setround(-1)                % convert intval b to inf/sup
        binf = b.mid - b.rad;
        setround(1)
        bsup = b.mid + b.rad;
      else
        binf = b.inf;
        bsup = b.sup;
      end
      [resrealinf,resrealsup] = lssresid([real(A) imag(A)],[real(x);-imag(x)],real(binf),real(bsup));
      [resimaginf,resimagsup] = lssresid([real(A) imag(A)],[imag(x);real(x)],imag(binf),imag(bsup));
      res.complex = 1;
      res.inf = [];
      res.sup = [];
      setround(1)
      res.mid = complex( resrealinf + 0.5*(resrealsup-resrealinf) , resimaginf + 0.5*(resimagsup-resimaginf) );
      res.rad = mag(res.mid - complex( resrealinf , resimaginf ));
    end
  end
  
  res = class(res,'intval');
  
  setround(rndold)
  

function [resinf,ressup] = lssresid(A,x,binf,bsup)
%Inclusion of the residual b-A*x, splits A and x and multiplies A*x in parts

setround(0)
factor = 68719476737;                   % heuristically optimal splitting 2^36+1

C = factor*A;
Abig = C - A;
A1 = C - Abig;                          % small (upper) part from A
A2 = A - A1;                            % A = A1+A2 exact splitting

x = -x;
y = factor*x;
xbig = y - x;
x1 = y - xbig;                          % small (upper) part from -x
x2 = x - x1;                            % x = x1+x2 exact splitting

setround(-1)
resinf = (A1*x1+binf)+(A1*x2+A2*x);
setround(1)
ressup = (A1*x1+bsup)+(A1*x2+A2*x);
