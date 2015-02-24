function display(c,name)
%DISPLAY      Command window display of Taylor
%

%Second parameter name for internal purposes
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

  loose = strcmp(get(0,'FormatSpacing'),'loose');

  if nargin<2
    name = inputname(1);
    if isempty(name)                    % happens for display(taylorinit(random))
      name = 'ans';
    end
  end

  numvar = size(c.t,1)-1;
  if numvar~=INTLAB_TAYLOR_ORDER
    warning('**** number of dependent variables and partial derivatives do not coincide')
  end

  if isa(c.t,'intval')
    strintval = 'intval ';
  else
    strintval = '';
  end
    
  % display result
  if loose, disp(' '); end
  disp([ strintval 'Taylor value ' name '.t = ' ])
  if loose, disp(' '); end
  if prod(c.size)==1
    display_(c.t,strintval,name)
  else
    if c.size(2)==1
      csize = c.size(1);
    else
      csize = c.size;
    end
    display_(reshape(c.t.',[csize size(c.t,1)]),strintval,name)
  end
  
  if loose, disp(' '); end
  
  if rndold
    setround(rndold)
  end

  
function display_(c,strintval,name)
% display dependent on strintval
  if isempty(strintval)
    % eval([name ' = c;']);       % make sure name is displayed
    % eval(['disp(' name ')'])
    disp(c)
  else
    display( c , name , 1 )
  end
