%$Header: /GMAT/dev/cvs/supportfiles/matlab/gmat_keyword/CallGMAT.m,v 1.1 2007/08/22 19:15:16 shughes Exp $
function CallGMAT(cmd, varargin)

strarr = char(varargin{1});
strarr = strcat(strarr, '*');
m = size(strarr, 1);
str = [cmd '*'];

for i = 1:m
   str = strcat(str, strarr(i,:));
end

str = strrep(str, '*', ' ');
str = deblank(str);
str = [str ';'];
%disp(str);

%--------------------------------------
% call dde function to send string
%--------------------------------------
global gmatChannel;

Poke(gmatChannel, 'script', str);
