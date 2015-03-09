function r = subsref(a,s)
%SUBSREF      Implements subscripted references for Taylor
%

% written  05/21/09     S.M. Rump
%

  e = 1e-30;
  if 1+e==1-e                           % fast check for rounding to nearest
    rndold = 0;
  else
    rndold = getround;
    setround(0)
  end

  global INTLAB_TAYLOR_ORDER

  while 1
    if ~isa(a,'taylor')                  % index reference a.x(i) etc.
      r = subsref(a,s(1));
    elseif strcmp(s(1).type,'()')        % index reference a(i)
      global INTLAB_TAYLOR_END
      INTLAB_TAYLOR_END = 0;             % reference by ()
      index = reshape(1:prod(a.size),a.size);
      index = index(s(1).subs{:});
      r.size = size(index);
      r.t = a.t(:,index(:));
      r = class(r,'taylor');
    elseif strcmp(s(1).type,'{}')        % Taylor coefficient reference a{i}
      global INTLAB_TAYLOR_END
      INTLAB_TAYLOR_END = 1;             % reference by {}
      index = ( 1:INTLAB_TAYLOR_ORDER+1 );
      index = index(s(1).subs{:}+1);
      r = a.t(index,:).';
    elseif strcmp(s(1).type,'.')         % index reference a.t
      if strcmp(s(1).subs,'t')
        r = a.t.';
      elseif strcmp(s(1).subs,'mid')
        r = mid(a);
      else
        error('invalid subscript reference for taylor')
      end
    else
      error('invalid index reference for taylor')
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
