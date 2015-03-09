function initFlag = init_eph(fname)

% INIT_EPH Open ephemeris file and initialize constants
%    INIT_EPH(FNAME) opens the binary JPL planetary ephemeris
%    file FNAME and reads the header information from the file

eph_global

FID=fopen(fname,'r');

[ttlsize,cnamsize,sssize,iptsize] = eph_header_size;

TTL=char(fread(FID,ttlsize,[num2str(ttlsize(2)) '*uchar=>uchar']));
SST=fread(FID,sssize,'float64=>float64');
NCON=fread(FID,1,'int32=>float64');
CNAM=char(fread(FID,[NCON cnamsize],[num2str(cnamsize) '*uchar=>uchar']));
CVAL=fread(FID,NCON,'float64=>float64');
AU=fread(FID,1,'float64=>float64');
EMRAT=fread(FID,1,'float64=>float64');
IPT=fread(FID,iptsize,[num2str(iptsize(2)),'*int32=>float64']);
NUMDE=fread(FID,1,'int32=>float64');
NCOEF=fread(FID,1,'int32=>float64');
HEADSIZE=ftell(FID);

if FID ~= 0
    initFlag = 1;
end