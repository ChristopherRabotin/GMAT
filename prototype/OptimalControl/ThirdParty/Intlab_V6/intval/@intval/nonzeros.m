function x = nonzeros(a)
%NONZEROS     Implements  nonzeros(a)  for sparse interval matrix
%
%   x = nonzeros(a)
%
%Functionality as in Matlab.
%

% written  08/09/02     S.M. Rump 
% modified 04/04/04     S.M. Rump  set round to nearest for safety
% modified 04/06/05     S.M. Rump  rounding unchanged
% modified 08/25/07     S.M. Rump  huge indices for sparse matrices
% modified 02/18/08     S.M. Rump  no column vector output for ND arrays
% modified 10/18/08     S.M. Rump  huge arrays
% modified 05/31/09     S.M. Rump  multiple arrays
%

  if a.complex
    if isequal(a.rad,0)
      x = cintval(nonzeros(a.mid));
    else
      [m,n] = size(a.mid);          % n is product of remaining indices for multiple arrays
      a.mid = reshape(a.mid,m,n);   % make sure dimension fits with sparse(...)
      a.rad = reshape(a.rad,m,n);   % a.rad cannot be 0
      [I,J] = find(spones(a.mid)+spones(a.rad));    % correct index set
      xmid = complex( real(nonzeros(real(a.mid)+sparse(I,J,sqrt(-1),m,n))) , ...
                      real(nonzeros(imag(a.mid)+sparse(I,J,sqrt(-1),m,n))) );
      xrad = real( real(nonzeros(a.rad+sparse(I,J,sqrt(-1),m,n))) );
      x = intval(xmid,xrad,'midrad');
      if size(x.mid,1)==1
        x.mid = x.mid.';
        x.rad = x.rad';
      end
    end
  else
    [m,n] = size(a.inf);
    if prod(size(a.inf))<2^31       % not huge
      I = find(spones(a.inf)+spones(a.sup));
      x = intval(squeeze(a.inf(I)),squeeze(a.sup(I)),'infsup');
      sizexinf = size(x.inf);
      if ( length(sizexinf)<3) & ( size(x.inf,1)==1 )
        x.inf = x.inf';
        x.sup = x.sup';
      end
    else                            % huge array
      [I,J,asup] = find(a.sup);
      x = nonzeros(a.inf + sparse(I,J,complex(0,asup),m,n));
      x = intval(real(x),imag(x),'infsup');
    end
  end  
