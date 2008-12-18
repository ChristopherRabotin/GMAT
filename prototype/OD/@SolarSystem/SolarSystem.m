
classdef SolarSystem < handle

    %  Set the public data
    properties  (SetAccess = 'public')

        SpeedofLight = 299792.458*1000;

        Mu = [22032.080486418
            324858.59882646
            398600.4415
            42828.314258067
            126712767.85780
            37940626.061137
            5794549.0070719
            6836534.0638793
            981.60088770700
            4902.8005821478
            132712440017.99];

        Mercury
        Venus
        Earth
        Mars
        Jupiter
        Saturn
        Uranus
        Neptune
        Pluto
        Luna
        Sol

    end

    %  Define the methods
    methods

        function SolSys = Initialize(SolSys)

            Earth.Radius =6378.1363;
            Earth.FlatteningCoefficient = 3.353642e-3;
            Earth.AngularVelocity = 7.2921158553*(10^(-5));
            Earth.J2 = 0.001082626683553;
            Earth.Mu = 398600.4415;

            SolSys.Earth = Earth;

        end

    end

end

