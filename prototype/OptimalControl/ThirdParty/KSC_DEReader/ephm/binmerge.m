function binmerge(fin1,fin2,fout)

% BINMERGE Merge two binary JPL ephemeris files
%    BINMERGE(FIN1,FIN2,FOUT) reads ephemeris data from the binary
%    ephemeris files FIN1 and FIN2 and writes the combined data to
%    FOUT.  Header data and time consistency are checked.

% Open binary input and output files

f1=fopen(fin1,'r');
f2=fopen(fin2,'r');
fo=fopen(fout,'w');
disp(sprintf('%s %s',fin1,fin2))

% Read headers from input files

[ttl1,ss1,ncon1,cnam1,cval1,au1,emrat1,ipt1,numde1,ncoef1]=ephbinhr(f1);
[ttl2,ss2,ncon2,cnam2,cval2,au2,emrat2,ipt2,numde2,ncoef2]=ephbinhr(f2);

% Compare headers from input files

if ~strcmp(ttl1,ttl2), warning('Titles do not match.'), end
if ~strcmp(cnam1,cnam2), warning('CNAMs do not match.'), end
if ncon1 ~= ncon2, warning('NCONs do not match.'), end
if au1 ~= au2, warning('AUs do not match.'), end
if emrat1 ~= emrat2, warning('EMRATs do not match.'), end
if ipt1 ~= ipt2, error('IPTs do not match.'), end
if numde1 ~= numde2, error('NUMDEs do not match.'), end
if ncoef1 ~= ncoef2, error('NCOEFs do not match.'), end

% Check for time consistency:  Intervals should abut

if ss1(2) < ss2(1), warning('Time spans do not overlap or abut.'), end

overlap_flag=0;
if ss1(2) ~= ss2(1) % Time spans overlap:  truncate first file
    overlap_flag=1;
end

% Compose new time interval

ss=[ss1(1) ss2(2) ss1(3)]';

% If we got this far, everything's ok.  Write the combined ephemeris file

% Write header

fwrite(fo,ttl1,[num2str(size(ttl1,1)) '*uchar']);
fwrite(fo,ss,'float64');
fwrite(fo,size(cval1,1),'int32');
fwrite(fo,cnam1,[num2str(size(cnam1,1)) '*uchar']);
fwrite(fo,cval1,'float64');
fwrite(fo,au1,'float64');
fwrite(fo,emrat1,'float64');
fwrite(fo,ipt1,'int32');
fwrite(fo,numde1,'int32');
fwrite(fo,ncoef1,'int32');


% Write coefficients

% First file

ct=1;

while ct
    [e,ct]=fread(f1,ncoef1,'*float64');
    if isempty(e), break, end
    if overlap_flag & (e(2) > ss2(1)) % If at overlap point, skip to second file
        ct=0;
    else
        fwrite(fo,e,'float64');
    end
end

% Second file

ct=1;

while ct
    [e,ct]=fread(f2,ncoef1,'*float64');
    if ~isempty(e)
        fwrite(fo,e,'float64');
    end
end

fclose(f1);
fclose(f2);
fclose(fo);

% Subfunction EPHBINHR

function [ttl,ss,ncon,cnam,cval,au,emrat,ipt,numde,ncoef]=ephbinhr(fid)

[ttlsize,cnamsize,sssize,iptsize] = eph_header_size;

ttl=char(fread(fid,ttlsize,[num2str(ttlsize(2)) '*uchar=>uchar']));
ss=fread(fid,sssize,'float64=>float64');
ncon=fread(fid,1,'int32=>float64');
cnam=char(fread(fid,[ncon cnamsize],[num2str(cnamsize) '*uchar=>uchar']));
cval=fread(fid,ncon,'float64=>float64');
au=fread(fid,1,'float64=>float64');
emrat=fread(fid,1,'float64=>float64');
ipt=fread(fid,iptsize,[num2str(iptsize(2)),'*int32=>float64']);
numde=fread(fid,1,'int32=>float64');
ncoef=fread(fid,1,'int32=>float64');
