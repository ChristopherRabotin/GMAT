function mergemall(flist,fout)

% MERGEMALL Merge a number of binary ephemeris files into one file.
%    MERGEMALL(FLIST,FOUT) merges the binary ephemeris files in the
%    cell array FLIST into one binary ephemeris file FOUT.

for i=1:length(flist)-1
    if i==1
        tfile1=flist{1};
    else
        tfile1='TEMPFILE_EPHEMERIS';
    end
    
    binmerge(tfile1,flist{i+1},fout);
    copyfile(fout,'TEMPFILE_EPHEMERIS');
    delete(fout);
end

copyfile('TEMPFILE_EPHEMERIS',fout);
delete('TEMPFILE_EPHEMERIS');
