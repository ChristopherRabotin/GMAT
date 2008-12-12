function Prop = Propagator_Initialize(Prop);

%     1 -> Mercury          8 -> Neptune
%     2 -> Venus            9 -> Pluto
%     3 -> Earth           10 -> Moon
%     4 -> Mars            11 -> Sun
%     5 -> Jupiter         12 -> Solar system barycenter
%     6 -> Saturn          13 -> Earth-Moon barycenter
%     7 -> Uranus          14 -> Nutations (longitude and obliquity)
%               15 -> Librations, if on file

%--------------------------------------------------------------------------
%  Set DE file index for central body
%--------------------------------------------------------------------------
if strcmp(Prop.FM.CentralBody,'Mercury')
    Prop.FM.CentralBodyIndex = 1;
end

if strcmp(Prop.FM.CentralBody,'Venus')
    Prop.FM.CentralBodyIndex = 2;
end

if strcmp(Prop.FM.CentralBody,'Earth')
    Prop.FM.CentralBodyIndex = 3;
end

if strcmp(Prop.FM.CentralBody,'Mars')
    Prop.FM.CentralBodyIndex = 4;
end

if strcmp(Prop.FM.CentralBody,'Jupiter')
    Prop.FM.CentralBodyIndex = 5;
end

if strcmp(Prop.FM.CentralBody,'Saturn')
    Prop.FM.CentralBodyIndex = 6;
end

if strcmp(Prop.FM.CentralBody,'Uranus')
    Prop.FM.CentralBodyIndex = 7;
end

if strcmp(Prop.FM.CentralBody,'Neptune')
    Prop.FM.CentralBodyIndex = 8;
end

if strcmp(Prop.FM.CentralBody,'Pluto')
    Prop.FM.CentralBodyIndex = 9;
end

if strcmp(Prop.FM.CentralBody,'Luna')
    Prop.FM.CentralBodyIndex = 10;
end

if strcmp(Prop.FM.CentralBody,'Sun')
    Prop.FM.CentralBodyIndex = 11;
end

%--------------------------------------------------------------------------
%  Set DE file indeces for point mass bodies
%--------------------------------------------------------------------------
j = 0;
for i = 1:size(Prop.FM.PointMasses,2);

    if strcmp(Prop.FM.PointMasses{i},'Mercury')
        j = j + 1;
        Prop.FM.PointMassIndeces{j} = 1;
    end

    if strcmp(Prop.FM.PointMasses{i},'Venus')
        j = j + 1;
        Prop.FM.PointMassIndeces{j} = 2;
    end

    if strcmp(Prop.FM.PointMasses{i},'Earth')
        j = j + 1;
        Prop.FM.PointMassIndeces{j} = 3;
    end

    if strcmp(Prop.FM.PointMasses{i},'Mars')
        j = j + 1;
        Prop.FM.PointMassIndeces{j} = 4;
    end

    if strcmp(Prop.FM.PointMasses{i},'Jupiter')
        j = j + 1;
        Prop.FM.PointMassIndeces{j} = 5;
    end

    if strcmp(Prop.FM.PointMasses{i},'Saturn')
        j = j + 1;
        Prop.FM.PointMassIndeces{j} = 6;
    end

    if strcmp(Prop.FM.PointMasses{i},'Uranus')
        j = j + 1;
        Prop.FM.PointMassIndeces{j} = 7;
    end

    if strcmp(Prop.FM.PointMasses{i},'Neptune')
        j = j + 1;
        Prop.FM.PointMassIndeces{j} = 8;
    end

    if strcmp(Prop.FM.PointMasses{i},'Pluto')
        j = j + 1;
        Prop.FM.PointMassIndeces{j} = 9;
    end

    if strcmp(Prop.FM.PointMasses{i},'Luna')
        j = j + 1;
        Prop.FM.PointMassIndeces{j} = 10;
    end

    if strcmp(Prop.FM.PointMasses{i},'Sun')
        j = j + 1;
        Prop.FM.PointMassIndeces{j} = 11;
    end

end
