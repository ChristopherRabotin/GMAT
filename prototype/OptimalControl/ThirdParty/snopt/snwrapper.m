 function [F,G] = snwrapper(x,userfun);
%function [F,G] = snwrapper(x,userfun);
%   Wrapper to allow variable length arguments in
%   user-supplied functions.

try
  [F,G] = feval(userfun,x);
catch
  F     = feval(userfun,x);
  G     = [];
end
