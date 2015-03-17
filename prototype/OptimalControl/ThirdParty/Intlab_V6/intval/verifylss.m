function X = verifylss(A,b,normal)
%VERIFYLSS    Verified solution of linear system
%
%   X = verifylss(A,b)
%
%Hybrid dense linear system solver. First stage based on Krawcyzk operator, see
%  R. Krawczyk: Newton-Algorithmen zur Bestimmung von Nullstellen mit
%    Fehlerschranken, Computing 4, 187-201, 1969.
%  R.E. Moore: A Test for Existence of Solutions for Non-Linear Systems,
%    SIAM J. Numer. Anal. 4, 611-615, 1977.
%with modifications for enclosing the error with respect to an approximate
%solution, an iteration scheme and epsilon-inflation as given in
%  S.M. Rump: Kleine Fehlerschranken bei Matrixproblemen, Ph.D. Thesis, 
%    University of Karlsruhe, 1980.
%  S.M. Rump: Verification methods for dense and sparse systems of equations, 
%    in : J. Herzberger (ed.), Topics in Validated Computations - Studies in 
%    Computational Mathematics, Elsevier, Amsterdam, 63-136, 1994.
%
%For sparse linear systems matrix is assumed to be s.p.d., otherwise normal
%equations are used. For symmetric matrix they can be forced to use by
%
%   X = verifylss(A,b,1);
%
%Note that this squares the condition number.
%
%If no success after 7 interval iterations, second stage starts according to
%  A. Neumaier: A simple derivation of the Hansen-Bliek-Rohn-Ning-Kearfott
%    enclosure for linear interval equations, Reliable Computing 5 (1999), 
%    131-136, and Erratum, Reliable Computing 6 (2000), 227.
%Additionally, the first stage based on Krawcyzk operator can be omitted to 
%  directly apply the second stage. To do this use intvalinit('LssSecondStage').
%  For details, see intvalinit. 
%The results of the second stage may be of better quality for extremely ill-conditioned
%  linear systems; normally the quality is similar. The second stage is a little
%  slower than the first stage (timing in seconds on an 800 MHz Laptop):
%
%  t1 = only first stage,  t2 = only second stage
%
%           A and b point data      A and b interval data
%     n     t1     t2     t2/t1      t1     t2     t2/t1
%  -------------------------------------------------------
%   100    0.04   0.05    1.33      0.04   0.05    1.31
%   200    0.17   0.25    1.49      0.23   0.31    1.35
%   500    2.1    3.2     1.52      2.8    3.9     1.40
%  1000   13.7   21.6     1.58     18.0   26.3     1.46
%
%
%Sparse system solver based on
%  S.M. Rump: Validated Solution of Large Linear Systems, Computing
%    Supplementum 9, 191-212, 1993.
%Current implementation only for s.p.d. matrices
%
%Improved residual improvement for point matrix is possible by
%  intvalinit('ImprovedResidual')
%For quadruple precision improvement of residual (for maximum accurate results) use
%  intvalinit('QuadrupleResidual')
%Because of interpretation overhead this is only for point dense matrix and for vector right
%  hand side. Extra computational costs and gain in accuracy is as follows (timing in 
%  seconds on an 800 MHz Laptop, point matrix and point right hand side, relative error
%  means maximum relative error of inclusion over all components and all test cases):
%
%  time[sec]      t1 = doubleresidual,  t2 = improvedresidual,  t3 = quadrupleresidual
%  max.rel.error  r1 = doubleresidual,  r2 = improvedresidual,  r3 = quadrupleresidual
%
%condition number of matrix 1e10:
%
%     n     t1     t2     t3     t2/t1     t3/t1      r1         r2         r3
%--------------------------------------------------------------------------------
%   100    0.03   0.03   0.07     1.20      2.63    1.6e-05    6.0e-11    2.2e-16
%   200    0.13   0.17   0.37     1.34      2.85    4.6e-05    1.5e-10    2.2e-16
%   500    1.7    2.2    4.0      1.25      2.28    1.4e-04    6.7e-10    2.2e-16
%
%condition number of matrix 1e15:
%
%     n     t1     t2     t3     t2/t1     t3/t1      r1         r2         r3
%--------------------------------------------------------------------------------
%   100    0.04   0.05   0.17     1.21      3.71    2.8e-00    1.3e-05    1.4e-15
%   200    0.27   0.30   0.45     1.11      1.66    2.8e-00    1.3e-05    1.4e-15
%   500    3.2    3.5    4.4      1.11      1.39    2.8e-00    1.3e-05    1.4e-15
%
%
%Over- and underdetermined system solver based on Section 3 in
%  S.M. Rump: Solving Algebraic Systems with High Accuracy, in "A New
%    Approach to Scientific Computation", eds. U. Kulisch and W. Miranker,
%    Academic Press, 51-120, 1983.
%

% written  10/16/98     S.M. Rump
% modified 11/06/98     second stage for dense systems and improvements for
%                         sparse systems, A. Neumaier, S.M. Rump
% modified 10/12/99     S.M. Rump  interval iteration stops if NaN occurred
% modified 09/02/00     S.M. Rump  rounding unchanged after use
% modified 04/02/02     S.M. Rump  improved residual added
% modified 06/26/02     S.M. Rump  output always of type intval, also for NaN
% modified 12/25/02     S.M. Rump  second stage offset corrected
% modified 11/29/03     S.M. Rump  second stage only, quadruple precision residual
% modified 04/04/04     S.M. Rump  set round to nearest for safety
% modified 04/06/05     S.M. Rump  rounding unchanged
% modified 11/20/05     S.M. Rump  fast check for rounding to nearest
% modified 02/09/06     S.M. Rump  error message prevented (thanks to Jiri Rohn)
% modified 09/20/07     S.M. Rump  intval answer if nan input (thanks to Jiri Rohn)
% modified 10/03/08     S.M. Rump  check for inf and NaN
% modified 02/18/09     S.M. Rump  change isempty to any
% modified 12/06/09     S.M. Rump  normal equations with paramater normal
% modified 02/16/10     S.M. Rump  minsvd initial approximation
% modified 03/01/10     S.M. Rump  isspd
% modified 03/16/10     S.M. Rump  symmetry/Hermitian ensured
%

  [m k] = size(A);
  [m1 n] = size(b);
  Aflag = any(isnan(A) | isinf(A));
  bflag = any(isnan(b) | isinf(b));
  if any(Aflag(:)) | any(bflag(:))
    X = intval(repmat(NaN,m1,n));
    return
  end
  if m~=m1
    error('linear system solver: inner matrix dimensions must agree')
  end
  
  e = 1e-30;
  if 1+e==1-e                           % fast check for rounding to nearest
    rndold = 0;
  else
    rndold = getround;
    setround(0)
  end

  if issparse(A)                 % A supposed to s.p.d. and square
    if m~=k
      error('sparse linear systems for non-square matrices not yet implemented')
    end   
    if nargin==2
      normal = 0;
    end
    if ~isequal(A,A') | normal
%       error('sparse linear systems for non-s.p.d. matrices not yet implemented')
      if isa(A,'intval')
        b = A'*b;
        A = A'*A;
      elseif ( ~isreal(A) ) | ( ~isreal(b) )
        b = A'*intval(b);
        A = A'*intval(A);
        if isreal(A)            % take care of 1x1 case
          A = cintval(A);
        end       
      else
        bpoint = ~isa(b,'intval');
        setround(-1)
        if bpoint
          Binf = A'*b;
        end
        Cinf = A'*A;
        setround(1)
        Csup = A'*A;
        if bpoint
          Bsup = A'*b;
        end
        setround(0)
        if bpoint
          b = intval(Binf,Bsup,'infsup');
        else
          b = A'*b;
        end
        A = intval(Cinf,Csup,'infsup');
      end
      A = tril(A)+tril(A,-1)';     % make sure A is symmetric/Hermitian
    end
    wng = warning;                 % get current warning mode
    warning off
    X = sparselss(A,b);
  else
    wng = warning;                 % get current warning mode
    warning off
    if m==k
      X = denselss(A,b);
    else
      if m>k      % least squares problem
        Y = denselss( [ A -eye(m) ; zeros(k) A' ] , [ b ; zeros(k,n) ] );
        if ~isnan(Y)
          X = Y(1:k,:);
        else
          X = intval(repmat(NaN,k,n));
        end
      else        % minimal norm solution
        Y = denselss( [ A' -eye(k) ; zeros(m) A ] , [ zeros(k,n) ; b ] );
        if ~isnan(Y)
          X = Y(m+1:m+k,:);
        else
          X = intval(repmat(NaN,k,n));
        end
      end
    end
  end
  
  if isnan(X)
    % disp('No inclusion achieved.')
  end
  warning(wng)                         % reset old rounding mode
    
  setround(rndold)



function X = sparselss(A,b)         % linear system solver for sparse matrices
  spparms('autommd',0);             % no additional minimum degree ordering
  n = dim(A);
  if isa(A,'intval')
    Aisintval = 1;
    Arad = rad(A);
  else
    Aisintval = 0;
  end
  A = mid(A);                       % save memory
   
  d = diag(A);                      % check diagonal positive
  m = min(d);
  if m<=0
    X = intval(repmat(NaN,size(b)));
    return
  end
  
  % Minimum degree reordering
  if exist('symamd','file')
    perm = symamd(A);
  else
    if n<2000                       % symmmd can be very slow
      perm = symmmd(A);
    end
  end
  if exist('perm','var')
    A = A(perm,perm);
    if Aisintval
      Arad = Arad(perm,perm);
    end
    b = b(perm,:);
    d = d(perm);
  end
  
  if max(d)/m > n                   % apply van der Sluis scaling
    D = spdiags( 2.^(-round(0.5*log2(d))) ,0,n,n );
    A = D*A*D;                      % exact since D_ii are powers of 2
    if Aisintval                    % constants below take care of underflow
      Arad = D*Arad*D;
    end
    b = D*b;
  end
  
  [s,xs] = singmin(A,mid(b));
% condA = cond(A)
% minAii=min(diag(A)), maxAii=max(diag(A))
  if s>0        % approximation for smallest singular value positive
    s = .8*s;
    if Aisintval      
      residual = mag(b-A*intval(xs));
      setround(1)
      residual = residual + Arad*abs(xs);
      residual = sqrt(sum(residual.^2));
      if size(b,2)~=1                       % matrix right hand side
        residual = repmat(residual,n,1);
      end
      setround(0)
      % approximation of norm(rad(A)) = rho(rad(A))
      x = ones(n,1);
      m = 1;
      M = 2;
      iter = 0;
      while ( abs((M-m)/(m+M))>.1 ) & ( iter<10 )
        iter = iter+1;
        y = Arad*x;
        x = y./x;
        M = max(x);
        m = min(x);
        scale = max(y);
        x = max( y/scale , 1e-12 );
      end
      % bound of norm(rad(A)) = rho(rad(A))
      setround(1)
      y = Arad*x;
      N = max(y./x);
      if s<=N
        X = intval(repmat(NaN,size(b)));
        setround(0)
        return
      end
    else
      N = 0;
      global INTLAB_INTVAL_RESIDUAL 
      if INTLAB_INTVAL_RESIDUAL==1      % improved residual calculation           
        residual = mag(lssresidual(A,xs,intval(b)));
      else                              % quadruple residual calculation not for sparse matrices
        residual = mag(b-A*intval(xs));
      end
      setround(1)
      residual = sqrt(sum(residual.^2));
      if size(b,2)~=1                       % matrix right hand side
        residual = repmat(residual,n,1);
      end
    end
    setround(-1)
    A = A - s*speye(n);
    setround(0)
    failed = 0;
    if isspd(A,0,0);                    % fast test
      minsvd = s;
    else                                % fast test failed, try sharper test
      [C,p] = chol(A);
      if p==0           % Cholesky decomposition of shifted matrix succeeded
        setround(-1)
        p1 = C'*C - A;
        setround(1)
        r = norm(p1,1);
        r = max( norm(C'*C - A,1) , r );
        minsvd = - (r - s);
      else              % Cholesky decomposition of shifted matrix failed
        failed = 1;
      end
    end
  else          % approximation for smallest singular value failed
    failed = 1;
  end
  if failed | (minsvd<0)
    X = intval(repmat(NaN,size(b)));
  else
    if ~Aisintval                       % point matrix
      setround(1)
      X = midrad( xs , residual/minsvd ) ;
    else                                % interval matrix
      if minsvd > N
        setround(1)
        denom = -(N-minsvd);
        X = midrad( xs , residual/denom ) ;    % A interval, lssresidual not used
      else
        X = intval(repmat(NaN,size(b)));
      end
    end
  end
  if exist('D','var')
    X = D*X;
  end
  if exist('perm','var')
    [dummy perm] = sort(perm);
    X = X(perm,:);
  end
  setround(0)
  
  
function X = denselss(A,b)        % linear system solver for dense matrices

  n = dim(A);
  midA = mid(A);
  midb = mid(b);
  
  % preconditioner: approximate inverse
  lastwarn('');                   % reset last warning
  R = inv( midA ) ;
  lastwarning = lastwarn;         % current last warning
  if isequal(lastwarning(1:min(18,length(lastwarning))),'Matrix is singular')
    X = intval(repmat(NaN,size(b)));
    return
  end
  
  % approximate solution with at least one iteration to ensure backward stability (Skeel)
  xs = R * midb ;
  
  global INTLAB_INTVAL_RESIDUAL
  improvedresidual = ( INTLAB_INTVAL_RESIDUAL==1 ) | ...
    ( ( INTLAB_INTVAL_RESIDUAL==2 ) & ( size(b,2)>1 ) );
  if improvedresidual    % improved residual calculation
    resnorm = inf;
    i = 0;
    while i<15
      i = i+1;
      resnormold = resnorm;
      res = R*lssresidual(midA,xs,midb);
      resnorm = norm(res,1);
      if resnorm<resnormold
        xs = xs + res;
      end
      if resnorm >= 1e-1*resnormold, break, end   % beware of zero residual
    end
  elseif INTLAB_INTVAL_RESIDUAL==2    % quadruple precision residual calculation
    resnorm = inf;
    i = 0;
    while i<15
      i = i+1;
      resnormold = resnorm;
      res = R*dot_(-1,midb,midA,xs);
      resnorm = norm(res,1);
      if resnorm<resnormold
        xs = xs - res;
      end
      if resnorm >= 1e-1*resnormold, break, end   % beware of zero residual
    end
  else
    xs = xs + R*(midb - midA*xs);
  end
  
  % interval iteration
  if isa(A,'intval')                  % A thick, no improved residual calculation
    ires = b - A*xs;
    Z = R * ires;
    RA = R*A;
  else                                % A thin, possibly improved residual calculation
    if improvedresidual
      ires = lssresidual(A,xs,intval(b));
    elseif INTLAB_INTVAL_RESIDUAL==2
      if isintval(b)
        if all(rad(b)==0)
          ires = dot_(1,b.inf,A,-xs,-2);
        else
          ires = lssresidual(A,xs,intval(b)); % b thick, quadruple residual does not make sense
        end
      else
        ires = dot_(1,b,A,-xs,-2);
      end
    else
      ires = b - A*intval(xs);
    end
    Z = R * ires;
    RA = R*intval(A);
  end
  
  global INTLAB_INTVAL_FIRST_SECOND_STAGE
  if INTLAB_INTVAL_FIRST_SECOND_STAGE     % start with first stage
    C = speye(n) - RA;
    Y = Z;
    E = 0.1*rad(Y)*hull(-1,1) + midrad(0,10*realmin);
    k = 0; kmax = 7; ready = 0; X = 0;
    while ( ~ready ) & ( k<kmax ) & ( ~any(isinf(X(:))) ) & ( ~any(isnan(Y(:))) )
      k = k+1;
      X = Y + E;
      Y = Z + C * X;
      ready = all(all(in0(Y,X)));
    end
    if ready               % success first stage
      X = xs + Y;  
      return
    end
  end
  
  % second stage starts
  b = R*ires;
  dRA = diag(RA);
  A = compmat(RA);
  B = inv(A);
  
  v = mag(B*ones(n,1));
  
  setround(-1)
  u = A*v;
  if all(min(u)>0),
    dAc =  diag(A);
    A = A*B - speye(n);
    setround(1)
    w = max(-A./(u*ones(1,n)));    % w_k = max_i{ -A(i,:)/u(i) }
    dlow = v.*w' - diag(B);
    dlow = max(0,-dlow);
    B = B + v*w;
    u = B*mag(b);
    d = diag(B);
    alpha = dAc + (-1)./d;
    k = size(b,2);
    if k==1
      beta = u./dlow - mag(b);
      X = xs + ( b + midrad(0,beta) ) ./ ( dRA + midrad(0,alpha) );
    else
      % d and dRA adapted for multiple r.h.s.
      v = ones(1,k);
      beta = u./(d*v) - mag(b);
      X = xs + ( b + midrad(0,beta) ) ./ ( ( dRA + midrad(0,alpha) ) * v );
    end
    index = any(isnan(X) | isinf(X) );
    index = any(index);
    if any(index(:))
      X = intval(repmat(NaN,size(b)));
    end
    return
  end;
  
  % second stage failed, compmat(RA) is numerically not an H-matrix
  X = intval(repmat(NaN,size(b)));
  

function [s,xs] = singmin(A,b)     
  % approximation of smallest singular value and approximate solution 
  % for positive definite A
  n = dim(A);
  [C,p] = chol(A);
  if p==0                       % Cholesky decomposition succeeded
    % approximation s of smallest eigenvalue
    y = C\(sum(abs(C),1)');     % initial approximation from normest
    s = n/(y'*y);
    k=0; notready=1;
    while notready | k<3
      k = k+1;
      sold = s;
      z = ( C'\y )*s;
      y = C\z;
      s = (z'*z)/(y'*y);
      notready = abs(s-sold)>.0001*abs(s+sold);
    end
    % approximate solution of linear system
    xs = C\(C'\b);
    % At least one residual iteration for backward stability (Skeel)
    global INTLAB_INTVAL_RESIDUAL
    if INTLAB_INTVAL_RESIDUAL==1      % more accurate residual calculation
      resnorm = inf;  
      while 1
        resnormold = resnorm;
        res = C\(C'\lssresidual(A,xs,b));
        resnorm = norm(res,1);
        if resnorm<resnormold
          xs = xs + res;
        end
        if resnorm >= 1e-2*resnormold, break, end   % beware of zero residual 
      end
    else                              % quadruple precision only for dense matrices
      xs = xs + C\(C'\(b-A*xs));
    end
  else                                % Cholesky decomposition failed
    s = 0;
    xs = 0;
  end
  