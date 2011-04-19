% Copyright (c) 2002 - 2011 United States Government as represented by the
% Administrator of the National Aeronautics and Space Administration.
% All Other Rights Reserved.
function String = SBInput2Str(Varargin)

if isempty(Varargin)
    String = ['[]'];
elseif isa(Varargin,'char')
   String =  [' ''' Varargin ''' '];  
elseif isa(Varargin,'double')
   String = num2str(Varargin,16);  
elseif isa(Varargin, 'cell')
   String = Cell2String(Varargin);  
end
