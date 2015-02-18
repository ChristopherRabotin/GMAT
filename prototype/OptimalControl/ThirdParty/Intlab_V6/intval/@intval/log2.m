function y = log2(x)
%LOG2         Implements  log2(x)  for intervals (binary logarithm)
%
%   y = log2(x)
%
%interval standard function implementation
%

% written  11/08/07     S.M. Rump
%

  global INTLAB_INTVAL_STDFCTS_LOG2_

  % log(x) * ( 1/log(2) )
  y = log(x) * infsup(INTLAB_INTVAL_STDFCTS_LOG2_.INF , ...
                      INTLAB_INTVAL_STDFCTS_LOG2_.SUP );
  index = ( x==0 );           
  if any(index(:))
    %VVVV  y(index) = -inf;
    s.type = '()'; s.subs = {index}; y = subsasgn(y,s,-inf);
    %AAAA  Matlab bug fix    
  end
