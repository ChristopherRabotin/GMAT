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