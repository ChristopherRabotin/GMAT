function u = subsasgn(u,s,b)
%SUBSASGN     Implements subscripted assignments for slopes
%
%  example  u(2,:) = b
%

% written  12/06/98     S.M. Rump
% modified 10/15/99     S.M. Rump  assignment []
% modified 09/28/01     S.M. Rump  matrices and multi-dimensional arrays
% modified 09/29/02     S.M. Rump  fix due to different behaviour of logical of Matlab 6.5
% modified 04/04/04     S.M. Rump  set round to nearest for safety
%                                    sparse input
% modified 04/06/05     S.M. Rump  rounding unchanged
%

  global INTLAB_SLOPE

  if length(s)>1
    error('multiple indexing for slope assignment not allowed')
  end

  if strcmp(s.type,'()')     % assignment u(i) = b

    uEmpty = isempty(u);     % assignment u(i) = b for empty u
    if isempty(b)
      % does not work in Matlab 5.3 for sparse r
      index = reshape(1:prod(u.size),u.size);
      index(s.subs{:}) = [];
      u.size = size(index);
      u.r = u.r(index,:);
      u.s = u.s(index,:);
      return
    end

    if ~isa(b,'slope')
      b = slope(b);
    end

    if ~uEmpty
      ur = u.r;
      us = u.s;
      value = ones(u.size);
      value_old = logical(value(:));
      value( s.subs{:} ) = 2;
      u.size = size(value);
      if issparse(u.s)
        u.r = intval(sparse([],[],[],prod(u.size),INTLAB_SLOPE.NUMVAR+1,0));
        u.s = intval(sparse([],[],[],prod(u.size),INTLAB_SLOPE.NUMVAR,0));
      else
        u.r = intval(zeros([prod(u.size) INTLAB_SLOPE.NUMVAR+1]));
        u.s = intval(zeros([prod(u.size) INTLAB_SLOPE.NUMVAR]));
      end
      u.r(value_old,:) = ur;
      u.r(value==2,:) = b.r;
      u.s(value_old,:) = us;
      u.s(value==2,:) = b.s;
    else
      index(s.subs{:}) = 1;
      u.size = size(index);
      if issparse(b.s)
        u.r = intval(sparse([],[],[],prod(size(index)),INTLAB_SLOPE.NUMVAR+1));;
        u.s = intval(sparse([],[],[],prod(size(index)),INTLAB_SLOPE.NUMVAR));;
      else
        u.r = intval(zeros(prod(size(index)),INTLAB_SLOPE.NUMVAR+1));
        u.s = intval(zeros(prod(size(index)),INTLAB_SLOPE.NUMVAR));
      end
      if prod(b.size)==1
        u.r(index==1,:) = b.r(ones(sum(index(:)),1),:);
        u.s(index==1,:) = b.s(ones(sum(index(:)),1),:);
      else
        u.r(index==1,:) = b.r;
        u.s(index==1,:) = b.s;
      end
      u = class(u,'slope');
    end

  else
    error('invalid index reference for slope')
  end
