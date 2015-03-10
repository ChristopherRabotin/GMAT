function r = gradientinit(v,see)
%GRADIENTINIT Initialization of gradient variable
%
%  x = gradientinit(v)
%
%The dependent variable(s) x are identified and initialized with value(s) v
%  v may be scalar for one independent variable, or vector (or matrix)
%  for several independent variables
%
%The call
%
%  gradientinit
%
%without input parameters sets the number of dependent variables to zero.
%Hessians are frequently sparsely populated. Therefore, one can choose to 
%store the derivative dense or sparse, see "help sparsegradient". 
%
%Default exception handling for accessing .dx of gradient arrays:
%
%  'GradientSparseArrayDerivWarn'  Warning issued when accessing .dx of 
%                                    sparse gradient array
%  'GradientSparseArrayDerivErr'   Error issued when accessing .dx of 
%                                    sparse gradient array
%  'GradientSparseArrayDerivAuto'  Neither warning nor error issued when 
%                                    accessing .dx of sparse gradient array
%  'GradientSparseArrayDeriv'      res = 'GradientSparseArrayDerivWarn'
%                                        'GradientSparseArrayDerivErr'
%                                        'GradientSparseArrayDerivAuto'
%
%The gradient of arrays is stored in the 'next dimension'. So y.dx is 
%  3-dimensional for a gradient row vector y. Since Matlab does not 
%  support multi-dimensional sparse arrays, y.dx is stored as a sparse
%  matrix for sparse y. Since there is a discrepency between the gradient
%  of full and sparse y, a corresponding warning or error message is issued.
%
%As a simple example of gradients
%
%  u = gradientinit([ -3 ; 3+4i ])
%
%initializes the gradient package to have two dependent variables. The
%variable u, a column vector, with values u.x(1) = -3 and u.x(2) = 3+4i 
%has gradients u(1).dx = [1 0] and u(2).dx = [0 1].
%If after that the statement
%
%  v = gradient( intval('3.14159_') )
%
%is executed, the value v.x is the interval with left bound 3.14158 and
%right bound 3.14160 (correctly rounded) and gradient value v.dx = [0 0]
%(v is treated like a constant). Similarly,
%
%  u(2) = -4711
%
%produces u.x = [ -3 ; -4711 ] and u.dx = [ 1 0 ; 0 0 ]
%
%A simple two-dimensional Newton iteration for the Rosenbrock function
%   f = inline(' [ 400*x(1)*(x(1)*x(1)-x(2)) + 2*(x(1)-1) ; 200*x(1)*(x(1)*x(1)-x(2)) ]')
%starting at x = [1.1;0.5] is
%  x = gradientinit([1.1;0.5]);  y = f(x); x = x - y.dx\y.x
%producing
%gradient value x.x = 
%    1.0000
%    0.9255
%gradient derivative(s) x.dx = 
%     1     0
%     0     1
%
%For other examples, see demogradient and intlablogo.
%

% written  10/16/98     S.M. Rump
% modified 11/30/98     S.M. Rump
% modified 09/16/00     S.M. Rump  typo
% modified 04/04/04     S.M. Rump  set round to nearest for safety
%                                    handling of derivatives of sparse arrays
%                                    sparsegradient added
% modified 04/06/05     S.M. Rump  rounding unchanged
%

  global INTLAB_GRADIENT_NUMVAR

  if nargin==0
    INTLAB_GRADIENT_NUMVAR = 0;
    return
  end
  
  if ischar(v)
    if nargin==1
      see = 1;
    end
    
    switch lower(v)
      case 'gradientsparsearrayderiv'
        global INTLAB_GRADIENT_DERIV_ERROR
        switch INTLAB_GRADIENT_DERIV_ERROR
          case 1, r = 'GradientSparseArrayDerivErr';
          case 0, r = 'GradientSparseArrayDerivWarn';
          case -1, r = 'GradientSparseArrayDerivAuto';
        end
        return
        
      case 'gradientsparsearrayderivwarn'
        global INTLAB_GRADIENT_DERIV_ERROR
        INTLAB_GRADIENT_DERIV_ERROR = 0;
        if see
          disp('===> Warning issued when accessing .dx of sparse 2-dimensional gradient')
        end
        return
        
      case 'gradientsparsearrayderiverr'
        global INTLAB_GRADIENT_DERIV_ERROR
        INTLAB_GRADIENT_DERIV_ERROR = 1;
        if see
          disp('===> Error when accessing .dx of sparse 2-dimensional gradient')
        end
        return
        
      case 'gradientsparsearrayderivauto'
        global INTLAB_GRADIENT_DERIV_ERROR
        INTLAB_GRADIENT_DERIV_ERROR = -1;
        if see
          disp('===> Neither warning nor error when accessing .dx of sparse 2-dimensional gradient')
        end
        return
        
      otherwise
        error('gradientinit called with invalid argument')
        
    end
  end

  if ~isa(v,'double') & ~isa(v,'intval')
    error('invalid initialization of dependent gradient variables')
  end
  INTLAB_GRADIENT_NUMVAR = prod(size(v));
  dummy.init = v;
  r = gradient( dummy , 'gradientinit' );
