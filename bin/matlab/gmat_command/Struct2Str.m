function String =  Struct2Str(UpperLevel,Struct)

global FullArray 
persistent ArrayCount

if isempty(FullArray)
    ArrayCount = 1;
end

if isempty(UpperLevel)
    UpperLevel = '';
end

Fnames = fieldnames(Struct);
m = size(Fnames,1);
for i = 1:m
    if (isa(getfield(Struct,Fnames{i}),'cell') |...
        isa(getfield(Struct,Fnames{i}),'double') |...
        isa(getfield(Struct,Fnames{i}),'char'));
       CurrentLevel = [ UpperLevel '.' Fnames{i} ];
       String = SBInput2Str(getfield(Struct,Fnames{i}));
       FullString = [CurrentLevel ' = ' String];
       FullArray{ArrayCount,1} = FullString;
       clear FullString
       ArrayCount = ArrayCount + 1;
   else
       CurrentLevel = [ UpperLevel '.' Fnames{i} ]; 
       Struct2Str(CurrentLevel,getfield(Struct,Fnames{i}));
   end
end

String = FullArray;