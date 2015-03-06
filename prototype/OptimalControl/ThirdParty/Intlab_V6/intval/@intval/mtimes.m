function c = mtimes(a,b,dummy)
%MTIMES       Implements  a * b  for intervals
%

% written  10/16/98     S.M. Rump
% modified 12/30/98     S.M. Rump  improved performance
% modified 09/02/00     S.M. Rump  rounding unchanged after use
% modified 04/04/04     S.M. Rump  set round to nearest for safety
%                                    major revision
%                                    remove check for 'double', sparse input
%                                    extra argument for non-interval output (internal use only)
%                                    improved performance for outer products
%                                    take care of Matlab sparse Inf/NaN bug
% modified 08/24/04     S.M. Rump  IR x IR for sharp multiplication 
% modified 04/06/05     S.M. Rump  rounding unchanged
% modified 11/20/05     S.M. Rump  fast check for rounding to nearest
% modified 02/11/06     S.M. Rump  SparseInfNanFlag removed
% modified 05/23/06     S.M. Rump  sparse Inf/NaN bug corrected in Version 7.2+
% modified 12/03/06     S.M. Rump  Sparse Bug global flag (thanks to Arnold)
% modified 12/05/06     S.M. Rump  Fast sharp multiplication if one factor contains no zero-IVs
% modified 05/09/09     S.M. Rump  thin intervals with equal NaNs
%

  global INTLAB_INTVAL_IVMULT

  [m n] = size(a); 
  [n1 n2] = size(b);

  if ( m*n==1 ) | ( n1*n2==1 )
    if nargin==2
      c = a .* b;
    else
      c = times(a,b,0);
    end
    return
  end

  if n~=n1
    error('matrices not compatible for multiplication')
  end

  e = 1e-30;
  if 1+e==1-e                           % fast check for rounding to nearest
    rndold = 0;
  else
    rndold = getround;
  end

  if ~isa(a,'intval')                     % a is double
    if ~isreal(a) | b.complex             % complex case
      if ~b.complex
        setround(1)
        b.mid = b.inf + 0.5*(b.sup-b.inf);
        b.rad = b.mid - b.inf;
      end
      c.complex = 1;                      % R * IC  or  C * IC
      c.inf = [];
      c.sup = [];
      if isreal(a) | ~b.complex           % one real factor
        setround(-1)                      % R * IC
        c1 = a * b.mid;
        setround(1)
        c2 = a * b.mid;
      else                                % C * IC
        setround(-1)
        c1 = real(a) * real(b.mid) + (-imag(a)) * imag(b.mid) + ...
             ( real(a) * imag(b.mid) + imag(a) * real(b.mid) ) * j;
        setround(1)
        c2 = real(a) * real(b.mid) + (-imag(a)) * imag(b.mid) + ...
             ( real(a) * imag(b.mid) + imag(a) * real(b.mid) ) * j;
      end
      c.mid = c1 + 0.5*(c2-c1);           % R * IC  or  C * IC
      if isequal(b.rad,0)
        c.rad = abs(c.mid-c1);
        if ~any(find(c.rad))              % take care of huge arrays
          c.rad = 0;
        end
      else
        c.rad = abs(c.mid-c1) + abs(a) * b.rad;
      end
    else                                  % real case  R * IR
      c.complex = 0;
      bthin = isequalwithequalnans(b.inf,b.sup);
      if bthin                            % R * R with directed rounding
        setround(-1)
        c.inf = a*b.inf;
        setround(1)
        c.sup = a*b.inf;
      else
        setround(1)                       % R * IR
        bmid = b.inf + 0.5*(b.sup-b.inf);
        brad = bmid - b.inf;
        crad = abs(a) * brad;
        c.inf = 0;                        % preserve order of definition of intval c
        c.sup = a * bmid + crad;
        setround(-1)
        c.inf = a * bmid - crad;
      end
      c.mid = [];
      c.rad = [];
    end
  elseif ~isa(b,'intval')                 % b is double
    if a.complex | ~isreal(b)             % complex case
      if ~a.complex
        setround(1)
        a.mid = a.inf + 0.5*(a.sup-a.inf);
        a.rad = a.mid - a.inf;
      end
      c.complex = 1;                      % IC * R  or IC * C
      c.inf = [];
      c.sup = [];
      if ~a.complex | isreal(b)           % one real factor
        setround(-1)                      % IC * R
        c1 = a.mid * b;
        setround(1)
        c2 = a.mid * b;
      else                                % IC * C
        setround(-1)
        c1 = real(a.mid) * real(b) + (-imag(a.mid)) * imag(b) + ...
             ( real(a.mid) * imag(b) + imag(a.mid) * real(b) ) * j;
        setround(1)
        c2 = real(a.mid) * real(b) + (-imag(a.mid)) * imag(b) + ...
             ( real(a.mid) * imag(b) + imag(a.mid) * real(b) ) * j;
      end
      c.mid = c1 + 0.5*(c2-c1);           % IC * R  or  IC * C
      if isequal(a.rad,0)
        c.rad = abs(c.mid-c1);
        if ~any(find(c.rad))              % take care of huge arrays
          c.rad = 0;
        end
      else
        c.rad = abs(c.mid-c1) + a.rad * abs(b);
      end
    else                                  % real case  IR * R
      c.complex = 0;
      athin = isequalwithequalnans(a.inf,a.sup);
      if athin                            % R * R with directed rounding
        setround(-1)
        c.inf = a.inf*b;
        setround(1)
        c.sup = a.inf*b;
      else
        setround(1)                       % IR * R
        amid = a.inf + 0.5*(a.sup-a.inf);
        arad = amid - a.inf;
        crad = arad * abs(b);
        c.inf = 0;                        % preserve order of definition of intval c
        c.sup = amid * b + crad;
        setround(-1)
        c.inf = amid * b - crad;
      end
      c.mid = [];
      c.rad = [];
    end
  else                                    % both a and b interval
    if a.complex | b.complex              % complex case
      if ~a.complex
        setround(1)
        a.mid = a.inf + 0.5*(a.sup-a.inf);
        a.rad = a.mid - a.inf;
      end
      if ~b.complex
        setround(1)
        b.mid = b.inf + 0.5*(b.sup-b.inf);
        b.rad = b.mid - b.inf;
      end
      c.complex = 1;
      c.inf = [];
      c.sup = [];
      if ~a.complex | ~b.complex          % one real factor
        setround(-1)
        c1 = a.mid * b.mid;
        setround(1)
        c2 = a.mid * b.mid;
      else                                % IC * IC
        setround(-1)
        c1 = real(a.mid) * real(b.mid) + (-imag(a.mid)) * imag(b.mid) + ...
             ( real(a.mid) * imag(b.mid) + imag(a.mid) * real(b.mid) ) * j;
        setround(1)
        c2 = real(a.mid) * real(b.mid) + (-imag(a.mid)) * imag(b.mid) + ...
             ( real(a.mid) * imag(b.mid) + imag(a.mid) * real(b.mid) ) * j;
      end
      c.mid = c1 + 0.5*(c2-c1);           % IC * IC
      if isequal(a.rad,0)
        if isequal(b.rad,0)
          c.rad = abs(c.mid-c1);
          if ~any(find(c.rad))            % take care of huge arrays
            c.rad = 0;
          end
        else
          c.rad = abs(c.mid-c1) + abs(a.mid) * b.rad;
        end
      elseif isequal(b.rad,0)
        c.rad = abs(c.mid-c1) + a.rad * abs(b.mid);
      else
        c.rad = abs(c.mid-c1) + ...
                  a.rad * ( abs(b.mid) + b.rad ) + abs(a.mid) * b.rad;
      end
    else                                  % real case,  IR * IR
      c.complex = 0;
      athin = isequalwithequalnans(a.inf,a.sup);
      bthin = isequalwithequalnans(b.inf,b.sup);
      if athin & bthin                    % R * R  with directed rounding
        setround(-1)
        c.inf = a.inf*b.inf;
        setround(1)
        c.sup = a.inf*b.inf;
      elseif athin
        setround(1)                       % R * IR
        bmid = b.inf + 0.5*(b.sup-b.inf);
        brad = bmid - b.inf;
        crad = abs(a.inf) * brad;
        c.inf = 0;                        % preserve order of definition of intval c
        c.sup = a.inf * bmid + crad;
        setround(-1)
        c.inf = a.inf * bmid - crad;
      elseif bthin
        setround(1)                       % IR * R
        amid = a.inf + 0.5*(a.sup-a.inf);
        arad = amid - a.inf;
        crad = arad * abs(b.inf);
        c.inf = 0;                        % preserve order of definition of intval c
        c.sup = amid * b.inf + crad;
        setround(-1)
        c.inf = amid * b.inf - crad;
      else                                % IR * IR
        if INTLAB_INTVAL_IVMULT | (n==1)  % interval mtimes interval by outer product
          index = find(a.inf<0);
          zero_a = any(a.sup(index)>0);   % factor a contains proper zero-intervals
          if zero_a
            index = find(b.inf<0);        % zero_b only defined if zero_a=1
            zero_b = any(b.sup(index)>0); % factor b contains proper zero-intervals
          end
          if ~zero_a                      % factor does not contain proper zero intervals
            indexapos = ( a.sup>0 );      % careful with sparse factor
            indexaneg = ( a.inf<0 );      % careful with sparse factor
            if any(indexaneg(:))          % there are non-positive intervals
              ainf = a.inf;
              ainf(indexapos) = 0;        % only non-positive entries
              asup = a.sup;
              asup(indexapos) = 0;        % only non-positive entries
              setround(-1)                % lower bound, factor is b.sup
              indexneg = ( b.sup<0 );
              indexpos = ( b.sup>0 );
              if any(indexneg(:))
                bsup = b.sup;
                bsup(indexpos) = 0;
                c.inf = asup*bsup;
              else
                if issparse(asup)
                  c.inf = sparse([],[],[],m,n2);
                else
                  c.inf = zeros(m,n2);
                end
              end
              if any(indexpos(:))
                bsup = b.sup;
                bsup(indexneg) = 0;
                c.inf = c.inf + ainf*bsup;
              end
              setround(1)                 % upper bound, factor is b.inf
              indexneg = ( b.inf<0 );
              indexpos = ( b.inf>0 );
              if any(indexneg(:))
                binf = b.inf;
                binf(indexpos) = 0;
                c.sup = ainf*binf;
              else
                if issparse(ainf)
                  c.sup = sparse([],[],[],m,n2);
                else
                  c.sup = zeros(m,n2);
                end
              end
              if any(indexpos(:))
                binf = b.inf;
                binf(indexneg) = 0;
                c.sup = c.sup + asup*binf;
              end
            else
              if issparse(a.inf)
                c.inf = sparse([],[],[],m,n2);
              else                
                c.inf = zeros(m,n2);
              end
              c.sup = c.inf;
            end
            if any(indexapos(:))          % there are non-positive intervals
              ainf = a.inf;
              ainf(indexaneg) = 0;        % only non-negative entries
              asup = a.sup;
              asup(indexaneg) = 0;        % only non-negative entries
              setround(-1)                % lower bound
              indexneg = ( b.inf<0 );
              indexpos = ( b.inf>0 );
              if any(indexneg(:))
                binf = b.inf;
                binf(indexpos) = 0;
                c.inf = c.inf + asup*binf;
              end
              if any(indexpos(:))
                binf = b.inf;
                binf(indexneg) = 0;
                c.inf = c.inf + ainf*binf;
              end
              setround(1)                 % upper bound
              indexneg = ( b.sup<0 );
              indexpos = ( b.sup>0 );
              if any(indexneg(:))
                bsup = b.sup;
                bsup(indexpos) = 0;
                c.sup = c.sup + ainf*bsup;
              end
              if any(indexpos(:))
                bsup = b.sup;
                bsup(indexneg) = 0;
                c.sup = c.sup + asup*bsup;
              end
            end
            if ~issparse(c)               % take care of NaN
              if a.complex
                index = isnan(a.mid) | isnan(a.rad);
              else
                index = isnan(a.inf) | isnan(a.sup);
              end
              if any(index(:))
                if c.complex
                  c.mid(any(index,2),:) = NaN;
                  c.rad(any(index,2),:) = NaN;
                else
                  c.inf(any(index,2),:) = NaN;
                  c.sup(any(index,2),:) = NaN;
                end
              end
              if b.complex
                index = isnan(b.mid) | isnan(b.rad);
              else
                index = isnan(b.inf) | isnan(b.sup);
              end
              if any(index(:))
                if c.complex
                  c.mid(:,any(index,1)) = NaN;
                  c.rad(:,any(index,1)) = NaN;
                else
                  c.inf(:,any(index,1)) = NaN;
                  c.sup(:,any(index,1)) = NaN;
                end
              end
            end
          elseif ~zero_b                  % factor does not contain proper zero intervals
            indexbpos = ( b.sup>0 );      % careful with sparse factor
            indexbneg = ( b.inf<0 );      % careful with sparse factor
            if any(indexbneg(:))          % there are non-positive intervals
              binf = b.inf;
              binf(indexbpos) = 0;        % only non-positive entries
              bsup = b.sup;
              bsup(indexbpos) = 0;        % only non-positive entries
              setround(-1)                % lower bound, factor is a.sup
              indexneg = ( a.sup<0 );
              indexpos = ( a.sup>0 );
              if any(indexneg(:))
                asup = a.sup;
                asup(indexpos) = 0;
                c.inf = asup*bsup;
              else
                if issparse(bsup)
                  c.inf = sparse([],[],[],m,n2);
                else
                  c.inf = zeros(m,n2);
                end
              end
              if any(indexpos(:))
                asup = a.sup;
                asup(indexneg) = 0;
                c.inf = c.inf + asup*binf;
              end
              setround(1)                 % upper bound, factor is a.inf
              indexneg = ( a.inf<0 );
              indexpos = ( a.inf>0 );
              if any(indexneg(:))
                ainf = a.inf;
                ainf(indexpos) = 0;
                c.sup = ainf*binf;
              else
                if issparse(binf)
                  c.sup = sparse([],[],[],m,n2);
                else
                  c.sup = zeros(m,n2);
                end
              end
              if any(indexpos(:))
                ainf = a.inf;
                ainf(indexneg) = 0;
                c.sup = c.sup + ainf*bsup;
              end
            else
              if issparse(a.inf)
                c.inf = sparse([],[],[],m,n2);
              else                
                c.inf = zeros(m,n2);
              end
              c.sup = c.inf;
            end
            if any(indexbpos(:))          % there are non-positive intervals
              binf = b.inf;
              binf(indexbneg) = 0;        % only non-negative entries
              bsup = b.sup;
              bsup(indexbneg) = 0;        % only non-negative entries
              setround(-1)                % lower bound, factor is a.inf
              indexneg = ( a.inf<0 );
              indexpos = ( a.inf>0 );
              if any(indexneg(:))
                ainf = a.inf;
                ainf(indexpos) = 0;
                c.inf = c.inf + ainf*bsup;
              end
              if any(indexpos(:))
                ainf = a.inf;
                ainf(indexneg) = 0;
                c.inf = c.inf + ainf*binf;
              end
              setround(1)                 % upper bound, factor is a.sup
              indexneg = ( a.sup<0 );
              indexpos = ( a.sup>0 );
              if any(indexneg(:))
                asup = a.sup;
                asup(indexpos) = 0;
                c.sup = c.sup + asup*binf;
              end
              if any(indexpos(:))
                asup = a.sup;
                asup(indexneg) = 0;
                c.sup = c.sup + asup*bsup;
              end
            end
            if ~issparse(c)               % take care of NaN
              if a.complex
                index = isnan(a.mid) | isnan(a.rad);
              else
                index = isnan(a.inf) | isnan(a.sup);
              end
              if any(index(:))
                if c.complex
                  c.mid(any(index,2),:) = NaN;
                  c.rad(any(index,2),:) = NaN;
                else
                  c.inf(any(index,2),:) = NaN;
                  c.sup(any(index,2),:) = NaN;
                end
              end
              if b.complex
                index = isnan(b.mid) | isnan(b.rad);
              else
                index = isnan(b.inf) | isnan(b.sup);
              end
              if any(index(:))
                if c.complex
                  c.mid(:,any(index,1)) = NaN;
                  c.rad(:,any(index,1)) = NaN;
                else
                  c.inf(:,any(index,1)) = NaN;
                  c.sup(:,any(index,1)) = NaN;
                end
              end
            end
          else                            % both factors contain proper zero intervals
            setround(-1)                  % multiplication with one loop
            colinf = a.inf(:,1); colsup = a.sup(:,1);
            rowinf = b.inf(1,:); rowsup = b.sup(1,:);
            cinf = min( colinf*rowinf , colinf*rowsup );
            cinf = min( cinf , colsup*rowinf );
            c.inf = min( cinf , colsup*rowsup );
            for k=2:n                       % make sure result sparse if a and b sparse (thanks to A. Rauh, Ulm)
              colinf = a.inf(:,k); colsup = a.sup(:,k);
              rowinf = b.inf(k,:); rowsup = b.sup(k,:);
              cinf = min( colinf*rowinf , colinf*rowsup );
              cinf = min( cinf , colsup*rowinf );
              c.inf = c.inf + min( cinf , colsup*rowsup );
            end
            setround(1)
            colinf = a.inf(:,1); colsup = a.sup(:,1);
            rowinf = b.inf(1,:); rowsup = b.sup(1,:);
            csup = max( colinf*rowinf , colinf*rowsup );
            csup = max( csup , colsup*rowinf );
            c.sup = max( csup , colsup*rowsup );
            for k=2:n                       % make sure result sparse if a and b sparse (thanks to A. Rauh, Ulm)
              colinf = a.inf(:,k); colsup = a.sup(:,k);
              rowinf = b.inf(k,:); rowsup = b.sup(k,:);
              csup = max( colinf*rowinf , colinf*rowsup );
              csup = max( csup , colsup*rowinf );
              c.sup = c.sup + max( csup , colsup*rowsup );
            end
          end
        else
          setround(1)                   % fast interval mtimes interval thru
          amid = a.inf + 0.5*(a.sup-a.inf);   % mid/rad arithmetic
          arad = amid - a.inf;
          bmid = b.inf + 0.5*(b.sup-b.inf);
          brad = bmid - b.inf;
          crad = arad * ( abs(bmid) + brad ) + abs(amid) * brad ;
          c.inf = 0;                    % preserve order of definition of intval c
          c.sup = amid*bmid + crad;
          setround(-1)
          c.inf = amid*bmid - crad;
        end
      end
      c.mid = [];
      c.rad = [];
    end
  end
  
  global INTLAB_SPARSE_BUG
  if INTLAB_SPARSE_BUG
    % take care of Matlab sparse NaN bug: sparse factor a
    if issparse(a)                            % simplified: factor inf always produces NaN
      index = any(isnan(b),1) | any(isinf(b),1);
      if any(any(index))
        if c.complex
          c.mid(:,index) = NaN;
          c.rad(:,index) = NaN;
        else
          c.inf(:,index) = NaN;
          c.sup(:,index) = NaN;
        end
      end
    end
    % take care of Matlab sparse NaN bug: sparse factor b
    if issparse(b)                            % simplified: factor inf always produces NaN
      index = any(isnan(a),2) | any(isinf(a),2);
      if any(any(index))
        if c.complex
          c.mid(index,:) = NaN;
          c.rad(index,:) = NaN;
        else
          c.inf(index,:) = NaN;
          c.sup(index,:) = NaN;
        end
      end
    end
  end
  
  % non-interval output for performance improvement for hessians
  if nargin==2
    c = class(c,'intval');
  end
  
  setround(rndold)
