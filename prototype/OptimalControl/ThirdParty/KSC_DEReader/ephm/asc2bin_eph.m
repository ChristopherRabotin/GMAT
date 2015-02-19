function asc2bin_eph(hfile,efiles,bfiles)

% ASC2BIN_EPH Write binary file containing JPL planetary ephemeris
%     ASC2BIN_EPH(HFILE,EFILES,BFILES) reads ASCII JPL planetary
%     ephemeris data and writes the data in binary form to BFILES.
%     HFILE is the ASCII header file and EFILES is a cell array of
%     ephemeris coefficient files.  BFILES is a cell array of binary
%     output files.
%
%     ASCII ephemeris files are available at
%     http://ssd.jpl.nasa.gov/eph_info.html
%
%     WARNING:  This function executes at a painfully slow rate.  It
%     should, however, only need executing once for each ASCII
%     ephemeris file.  It is slow because the coefficients are read
%     and parsed using FGETL's rather than FSCANF.  This is necessary
%     because the ASCII files use 'D' rather than 'E' as the 
%     delimiter for exponents.  MATLAB does not recognize 'D' as such
%     a delimiter so each line must be read, 'E' substituded for 'D',
%     and a STR2NUM performed to get the coefficients.

% Set header parameter sizes

[ttlsize,cnamsize,sssize,iptsize] = eph_header_size;

% Read the ASCII header file

[ttl,ss,cnam,cval,ipt]=read_ephem_hdr(hfile);

% Assign values for numde, au, and emrat

numde=cval(find(strcmp('DENUM',cellstr(cnam))));
au=cval(find(strcmp('AU',cellstr(cnam))));
emrat=cval(find(strcmp('EMRAT',cellstr(cnam))));

for i=1:max(size(efiles))

    fid=fopen(bfiles{i},'w');

    % Write the header information to the binary output file

    fwrite(fid,ttl,[num2str(ttlsize(2)) '*uchar']);
    fwrite(fid,ss,'float64');
    fwrite(fid,size(cval,1),'int32');
    fwrite(fid,cnam,[num2str(cnamsize) '*uchar']);
    fwrite(fid,cval,'float64');
    fwrite(fid,au,'float64');
    fwrite(fid,emrat,'float64');
    fwrite(fid,ipt,'int32');
    fwrite(fid,numde,'int32');

    % For each coefficient file, read the data from the ASCII file
    % and write it to the binary output file

    eph=read_jpl_ephem(efiles{i}); % Read ASCII coefficient file

   
    ncoeff=size(eph,1);
    fwrite(fid,ncoeff,'int32'); % First time through, write number
                                % of coefficients per set
    fwrite(fid,eph,'float64');
    fseek(fid,prod(ttlsize),-1);
    ss(1)=eph(1,1);
    ss(2)=eph(2,size(eph,2));
    fwrite(fid,ss,'float64');
    fclose(fid);
end

%%% Subfunction READ_JPL_EPHEM

function eph=read_jpl_ephem(fn)

% Open file

disp(sprintf('Opening file %s ...',fn))
fid=fopen(fn,'r');

% Initialize arrays

eph=[];
l=0;

while 1
    [nc,ct]=fscanf(fid,'%e %e\n',2); % read group number and number 
                                     % coefficients
    if ct==0, break, end % end of file
    
    ne=ceil(nc(2)/3); % number of rows in this group
    nct=nc(2);
    ep=[];
    
    for i=1:ne
        s='';
        while strcmp(s,'')
            s=fgetl(fid);
            if ~ischar(s), break, end % end of file
            ep1=str2num(strrep(s,'D','E')); % <= ASCII ephemeris file uses D for
            ep=[ep;ep1(:)];                 %    exponent.  MATLAB chokes on this.
        end                                 %    Change 'D' to 'E'.
    end
    
    eph=[eph ep(1:nc(2))];
    
% Keep user informed of process
    
    l=l+1;
    if ~mod(l,10)
        disp(sprintf('%3i groups processed ...',l))
    end
    
end

% Tell user size of eph

disp(sprintf('Read and processed %i groups of %i coefficients from %s.', ...
    l,nct,fn))

% Close file

fclose(fid);

%%% Subfunction READ_EPHEM_HDR

function [ttl,ss,cnam,cval,ipt]=read_ephem_hdr(fn)

% READ_EHPEM_HDR Read ASCII JPL planetary ephemeris header file
%    [TTL,SS,CNAM,CVAL,IPT)= READ_EPHEM_HDR(FN) opens and reads
%    the ASCII header file FN.
%
%    OUTPUTS:   TTL     3-by-80 title string
%               SS      3-by-1 time span/interval vector
%               CNAM    n-by-8 constant name array
%               CVAL    n-by-1 constant value vector
%               IPT     3-by-13 coefficient pointer matrix

% Set sizes for TTL,CNAM,SS, and IPT

[ttlsize,cnamsize,sssize,iptsize]=eph_header_size;

% Open the ASCII header file

fid=fopen(fn,'r');

% Read the file line by line

while 1
    s=fgetl(fid);
    if ~ischar(s), break, end
    [s1,s2]=strtok(s);
    s1=lower(s1);
    s2=lower(s2);
    
    if ~isempty(s1)
        switch s1
        case 'ksize'
            ksize=str2num(strtok(s2));
        case 'group'
            s2=str2num(s2);
            switch s2
            case 1010 %title
                s=fgetl(fid); % blank line
                ttl=[];
                for i=1:3
                    ttl=[ttl;cellstr(fgetl(fid))]; %title
                end
                ttl=char(ttl);
                ttl=[ttl char(ones(size(ttl,1),ttlsize(2)-size(ttl,2))*' ')];
            case 1030 %timespan
                s=fgetl(fid); % blank line
                ss=fscanf(fid,'%g %g %g',3);
            case 1040 %header variable names
                s=fgetl(fid); % blank line
                nv=str2num(fgetl(fid));
                cnam=[];
                for i=1:ceil(nv/10)
                    cnam=[cnam;strread(fgetl(fid),'%s',10)];
                end
                cnam=char(cnam);
                cnam=[cnam char(ones(size(cnam,1),cnamsize-size(cnam,2))*' ')];
            case 1041 %header variable values
                s=fgetl(fid); % blank line
                nv=str2num(fgetl(fid));
                nl=ceil(nv/3);
                cval=[];
                for i=1:nl
                    cv=str2num(strrep(fgetl(fid),'D','E'));
                    cval=[cval;cv(:)];
                end
                cval=cval(1:nv);
            case 1050 %pointer values                
                s=fgetl(fid); % blank line
                ipt=fscanf(fid,'%i',fliplr(iptsize))';
            case 1070 %eof
            end
        end
    end
end