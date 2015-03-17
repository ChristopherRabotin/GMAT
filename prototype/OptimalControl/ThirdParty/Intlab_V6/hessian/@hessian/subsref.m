function r = subsref(a,s)
%SUBSREF      Implements subscripted references for Hessians
%

% written  04/04/04     S.M. Rump
% modified 04/06/05     S.M. Rump  rounding unchanged
% modified 09/28/08     S.M. Rump  check for rounding to nearest improved
%

  e = 1e-30;
  if 1+e==1-e                           % fast check for rounding to nearest
    rndold = 0;
  else
    rndold = getround;
    setround(0)
  end

  global INTLAB_HESSIAN_NUMVAR
  N = INTLAB_HESSIAN_NUMVAR;

  while 1
    if ~isa(a,'hessian')                 % index reference a.x(i) etc.
      r = subsref(a,s(1));
    elseif strcmp(s(1).type,'()')        % index reference a(i)
      r.x = a.x(s(1).subs{:});
      index = reshape(1:prod(size(a.x)),size(a.x));
      index = index(s(1).subs{:});
      r.dx = a.dx(:,index(:));
      r.hx = a.hx(:,index(:));
      % avoid Matlab 6.5f bug: 
      % a = sparse([],[],[],1,1); a = reshape(a,1,1); abs(a)
      % produces  9.6721e-317  or similar number in underflow range
      if prod(size(r.x))==1
        r.x = full(r.x);
      end
      if prod(size(r.dx))==1
        r.dx = full(r.dx);
      end
      if prod(size(r.hx))==1
        r.hx = full(r.hx);
      end
      r = class(r,'hessian');
    elseif strcmp(s(1).type,'.')         % index reference a.x, a.dx or a.hx
      if strcmp(s(1).subs,'x')
        r = a.x;
      elseif strcmp(s(1).subs,'dx')
        sizeax = size(a.x);
        if ( length(sizeax)==2 ) & ( sizeax(2)==1 ) 
          sizeax = sizeax(1);            % row gradient for column vector a.x
        end
        if issparse(a.dx) & ( length(sizeax)>1 )
          global INTLAB_HESSIAN_DERIV_ERROR
          if INTLAB_HESSIAN_DERIV_ERROR==1
            error('access of .dx of higher dimensional sparse hessian, see hessianinit')
          elseif INTLAB_HESSIAN_DERIV_ERROR==0
            warning('hessian is stored 2-dimensional because Matlab does not allow multi-dimensional sparse arrays')
          end
        end
        r = reshape(transpose(a.dx),[sizeax N]);
      elseif strcmp(s(1).subs,'hx')
        if issparse(a.hx) & ( prod(size(a.x))>1 )
          global INTLAB_HESSIAN_DERIV_ERROR
          if INTLAB_HESSIAN_DERIV_ERROR==1
            error('access of .hx of non-scalar sparse hessian, see hessianinit')
          elseif INTLAB_HESSIAN_DERIV_ERROR==0
            warning('hessian is stored 2-dimensional because Matlab does not allow multi-dimensional sparse arrays')
          end
        end
        index = reshape(1:N*N,N,N)';
        r = a.hx + a.hx(index(:),:);     % Hessian is .hx + transpose(.hx)
        sizeax = size(a.x);
        if prod(sizeax)==1
          r = reshape(r,N,N);
        else
          r = reshape(r,[sizeax N N]);
        end
      elseif strcmp(s(1).subs,'mid')
        r = mid(a);
      else
        error('invalid subscript reference for hessian')
      end
    else
      error('invalid index reference for hessian')
    end
    if length(s)==1  
      if rndold
        setround(rndold)
      end
      return
    end
    s = s(2:end);
    a = r;
  end
