function a = sec(a)
%SEC          Gradient secant sec(a)
%

% written  10/16/98     S.M. Rump
% modified 10/14/00     S.M. Rump  use Tony's trick
% modified 03/22/04     S.M. Rump  improved performance
% modified 04/04/04     S.M. Rump  set round to nearest for safety
%                                    accelaration for sparse input
% modified 04/06/05     S.M. Rump  rounding unchanged
% modified 09/28/08     S.M. Rump  check for rounding to nearest improved
% modified 10/08/08     S.M. Rump  improved sparse multiplication: not using intval data type
%

  e = 1e-30;
  if 1+e==1-e                           % fast check for rounding to nearest
    rndold = 0;
  else
    rndold = getround;
    setround(0)
  end

  global INTLAB_GRADIENT_NUMVAR
  N = INTLAB_GRADIENT_NUMVAR;

  ax = tan(a.x(:));
  a.x = sec(a.x);
  ax = a.x(:) .* ax;
  if issparse(a.dx)
    sizeax = size(a.dx,1);
    [ia,ja,sa] = find(a.dx);
    % take care of scalar a.x: cures Matlab V6.0 bug
    % a=7; i=[1 1]; x=a(i), b=sparse(a); y=b(i)  yields row vector x but column vector y
    ax = ax(ia);    
    if isa(a.x,'intval')
      adx = times(ax(:),sa(:),0);
      if adx.complex
        a.dx = intval( sparse(ia,ja,adx.mid,sizeax,N) , sparse(ia,ja,adx.rad,sizeax,N) , 'midrad' );
       else
        a.dx = intval( sparse(ia,ja,adx.inf,sizeax,N) , sparse(ia,ja,adx.sup,sizeax,N) , 'infsup' );
      end
    else
      a.dx = sparse(ia,ja,ax(:).*sa(:),sizeax,N);
    end
  else
    a.dx = a.dx .* ax(:,ones(1,N));
  end
  
  if rndold
    setround(rndold)
  end
