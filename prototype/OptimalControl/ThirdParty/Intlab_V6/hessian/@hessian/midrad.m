function midrad(c)
%MIDRAD       Display of interval hessians in midrad notation
%
%   midrad(c)
%

% written  04/04/04     S.M. Rump
% modified 04/06/05     S.M. Rump  rounding unchanged
% modified 02/11/06     S.M. Rump  SparseInfNanFlag removed
% modified 06/04/09     S.M. Rump  Comment
%

  global INTLAB_HESSIAN_NUMVAR

  if nargin<2
    name = inputname(1);
    if isempty(name)                    % happens for display(hessianinit(random))
      name = 'ans';
    end
  end
  
  if isa(c.x,'intval')
    display_gen(c,name,'midrad')
  else
    display_gen(c,name,'display_')
  end

  