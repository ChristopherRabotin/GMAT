function disp_(c)
%DISP_        Display of interval gradients in "_" notation
%
%   disp_(c)
%

% written  06/04/09     S.M. Rump
%

  global INTLAB_GRADIENT_NUMVAR

  loose = strcmp(get(0,'FormatSpacing'),'loose');

  numvar = size(c.dx,2);
  if numvar~=INTLAB_GRADIENT_NUMVAR
    warning('**** number of dependent variables and partial derivatives do not coincide')
  end

  sizecdx = size(c.x);
  if length(sizecdx)==2 & sizecdx(2)==1
    sizecdx = sizecdx(1);
  end

  if isa(c.x,'intval')
    if loose, disp(' '); end
    disp([ 'intval gradient value ' inputname(1) '.x = ' ])
    disp_(c.x,'',1)
    if loose, disp(' '); end

    if loose, disp(' '); end
    disp([ 'intval gradient derivative(s) ' inputname(1) '.dx = ' ])
    disp_( reshape( c.dx , [sizecdx INTLAB_GRADIENT_NUMVAR] ) , '' , 1 );
    if loose, disp(' '); end
  else
    display(c,inputname(1))
  end
  