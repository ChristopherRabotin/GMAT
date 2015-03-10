function [flist,bdir]=multifile(bname)

a=dir;

bdir=pwd;
odir=pwd;

if isunix
    bdir=[bdir,'/'];
else
    bdir=[bdir,'\'];
end

[s,v]=listdlg('Name',bname,'ListString',char({a.name}));

if size(s(:),1)==1 & isdir(a(s).name)
    cd(bdir);
    cd(a(s).name)
    [flist,bdir]=multifile(bname)
else
    flist={a(s).name}';
end

cd(odir)