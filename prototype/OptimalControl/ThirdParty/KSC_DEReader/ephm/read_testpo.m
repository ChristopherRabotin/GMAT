function testdat=read_testpo(fn)

% READ_TESTPO Read and parse contents of JPL ephemeris test data file
%    NOTE:  Some large values of libration may be stored incorrectly
%    due to the number of significant digits involved.

fid=fopen(fn,'r');
while 1
    l=fgetl(fid);
    if ~isstr(l), break, end
    if strcmp(l,'EOT'), break, end % Next line begins data
end

[testdat,ct]=fscanf(fid,'%*s %*s %f %f %f %f %f');
testdat=reshape(testdat,[5 ct/5])';
fclose(fid);