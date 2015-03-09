function r = hessianinit(v,see)
%HESSIANINIT  Initialization of hessian variable
%
%  x = hessianinit(v)
%
%The independent variable(s) x are identified and initialized with value(s) v.
%  v may be scalar for one independent variable, or vector (or matrix)
%  for several independent variables.
%
%The call
%
%  hessianinit
%
%without input parameters sets the number of independent variables to zero.
%
%Hessians are frequently sparsely populated. Therefore, one can choose to store
%first and second derivative dense or sparse, see "help sparsehessian". 
%Default exception handling for accessing .dx and .hx of hessian arrays:
%
%  'HessianSparseArrayDerivWarn'   Warning issued when accessing .dx or .hx of 
%                                    sparse gradient array
%  'HessianSparseArrayDerivErr'    Error issued when accessing .dx or .hx of 
%                                    sparse gradient array
%  'HessianSparseArrayDerivAuto'   No warning or error issued when accessing .dx 
%                                    or .hx of sparse gradient array
%  'HessianSparseArrayDeriv'       res = 'HessianSparseArrayDerivWarn'
%                                        'HessianSparseArrayDerivErr'
%                                        'HessianSparseArrayDerivAuto'
%
%The hessian of arrays is stored in the 'next dimension'. So y.hx is
%  3-dimensional for a column vector y, and y.dx is 3-dimensional for 
%  a gradient row vector y. 
%Since Matlab does not support multi-dimensional sparse arrays, y.dx 
%  and y.hx is stored as a sparse matrix for sparse y. Since there is 
%  a discrepency between the hessian of full and sparse y, a corresponding 
%  warning or error message is issued.
%
%As a simple example of hessians
%
%  u = hessianinit([ -3 ; 3+4i ])
%
%initializes the hessian package to have two independent variables. The
%variable u, a column vector, with values u.x(1) = -3 and u.x(2) = 3+4i 
%has first derivatives u(1).dx = [1 0] and u(2).dx = [0 1] and second
%derivatives u(1).hx = u(2).hx = zeros(2).
%If after that the statement
%
%  v = hessian( intval('3.14159_') )
%
%is executed, the value v.x is the interval with left bound 3.14158 and
%right bound 3.14160 (correctly rounded), the first derivative v.dx = [0 0]
%and second derivative v.hx = zeros(2) (v is treated like a constant). Similarly,
%
%  u(2) = -4711
%
%produces u.x = [ -3 ; -4711 ], u.dx = [ 1 0 ; 0 0 ] and does not change u.hx.
%
%Hessians frequently depend only on few variables. Therefore, if not specified 
%  otherwise, the first and second derivative are stored sparse for eight and
%  more unknowns (see above).
%To change only the display you may use "full(u)" or "sparse(u)". 
%
%For some examples, see demohessian.
%

% written  04/04/04     S.M. Rump
% modified 04/06/05     S.M. Rump  rounding unchanged
%

  if nargin==0
    global INTLAB_HESSIAN_NUMVAR
    INTLAB_HESSIAN_NUMVAR = 0;
    return
  end
  
  if ischar(v)
    if nargin==1
      see = 1;
    end
    
    switch lower(v)
      case 'hessiansparsearrayderiv'
        global INTLAB_HESSIAN_DERIV_ERROR
        switch INTLAB_HESSIAN_DERIV_ERROR
          case 1, r = 'HessianSparseArrayDerivErr';
          case 0, r = 'HessianSparseArrayDerivWarn';
          case -1, r = 'HessianSparseArrayDerivAuto';
        end
        return
        
      case 'hessiansparsearrayderivwarn'
        global INTLAB_HESSIAN_DERIV_ERROR
        INTLAB_HESSIAN_DERIV_ERROR = 0;
        if see
          disp('===> Warning issued when accessing .dx or .hx of sparse hessian array')
        end
        return
        
      case 'hessiansparsearrayderiverr'
        global INTLAB_HESSIAN_DERIV_ERROR
        INTLAB_HESSIAN_DERIV_ERROR = 1;
        if see
          disp('===> Error when accessing .dx or .hx of sparse hessian array')
        end
        return
        
      case 'hessiansparsearrayderivauto'
        global INTLAB_HESSIAN_DERIV_ERROR
        INTLAB_HESSIAN_DERIV_ERROR = -1;
        if see
          disp('===> Neither warning nor error when accessing .dx or .hx of sparse hessian array')
        end
        return
        
      otherwise
        error('hessianinit called with invalid argument')
        
    end
  end

  global INTLAB_HESSIAN_NUMVAR
  
  if ~( isa(v,'double') | isa(v,'intval') )
    error('invalid initialization of hessian')
  end
  INTLAB_HESSIAN_NUMVAR = prod(size(v));
  
  dummy.init = v;
  r = hessian( dummy , 'hessianinit' );
