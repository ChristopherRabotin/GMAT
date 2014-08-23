function setting = intlabsetting(setting,see)
%SETTING      Current setting of INTLAB control variables
%
%For display of current setting, call
%
%   intlabsetting(see)
%
%Parameter see is optional; default is 1 for displaying results. For storing
%and restoring current setting use
%
%   currentsetting = intlabsetting  [or  currentsetting = intlabsetting(see)]
%   intlabsetting(currentsetting)   [or  intlabsetting(currentsetting,see)  ]
%
%For restoring the default as specified in startintlab use
%
%   intlabsetting('default')
%

% written  04/04/04     S.M. Rump
% modified 02/12/06     S.M. Rump  new settings added
% modified 09/10/07     S.M. Rump  approximate std fcts removed
% modified 10/23/07     S.M. Rump  new setting corrected
% modified 11/24/07     S.M. Rump  isstr replaced by ischar
% modified 10/18/08     S.M. Rump  out-of-range flag
%

  if ( nargin>=1 ) & ischar(setting)     % new setting
    if isequal(setting,'default')       % restore default setting
      global INTLAB_SETTING
      setting = INTLAB_SETTING;
    end
    if nargin==1
      see = 1;
    end
    for i=1:size(setting,1)
      j=findstr(')',setting(i,:));
      eval([ setting(i,1:j-1) ',0' setting(i,j:end) ';' ]);
    end
    if see
      intlabsetting
    end
    return
  end
  
  if nargin==0
    see = 1;            % display and possibly output of current setting
  else
    see = setting;
  end
  
  setting = [];
  digits = displaywidth; setting = strvcat( setting , ['displaywidth(' int2str(digits) ');'] );
  
  s = intvalinit('Display'); setting = strvcat( setting , ['intvalinit(''' s ''');'] );
  s = intvalinit('RealStdFctsExcptn'); setting = strvcat( setting , ['intvalinit(''' s ''');'] );
  s = intvalinit('Residual'); setting = strvcat( setting , ['intvalinit(''' s ''');'] );
  s = intvalinit('LssStages'); setting = strvcat( setting , ['intvalinit(''' s ''');'] );
  s = intvalinit('IVmult'); setting = strvcat( setting , ['intvalinit(''' s ''');'] );
  s = intvalinit('RealComplexAssign'); setting = strvcat( setting , ['intvalinit(''' s ''');'] );
  s = intvalinit('ComplexInfSupAssign'); setting = strvcat( setting , ['intvalinit(''' s ''');'] );
  
  s = gradientinit('GradientSparseArrayDeriv'); setting = strvcat( setting , ['gradientinit(''' s ''');'] );
  
  s = slopeinit('SlopeSparseArrayDeriv'); setting = strvcat( setting , ['slopeinit(''' s ''');'] );
  
  s = hessianinit('HessianSparseArrayDeriv'); setting = strvcat( setting , ['hessianinit(''' s ''');'] );
  
  s = polynominit('DisplayUPoly'); setting = strvcat( setting , ['polynominit(''' s ''');'] );
  s = polynominit('EvaluateUPoly'); setting = strvcat( setting , ['polynominit(''' s ''');'] );
  s = polynominit('EvaluateMPoly'); setting = strvcat( setting , ['polynominit(''' s ''');'] );
  s = polynominit('AccessVariable'); setting = strvcat( setting , ['polynominit(''' s ''');'] );
  
  s = longinit('ErrorTerm'); setting = strvcat( setting , ['longinit(''' s ''');'] );
  
  if see
    for i=1:size(setting,1)
      eval(setting(i,:))
    end
  end
  
  if nargout==0
    clear setting
  end
  