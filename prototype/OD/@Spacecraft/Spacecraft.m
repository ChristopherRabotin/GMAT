classdef Spacecraft < handle

    %  Set the public data
    properties  (SetAccess = 'public')
        Epoch = 21545;
        X  = 7100;
        Y  = 0.0;
        Z  = 1300;
        VX = 0.1;
        VY = 7.35;
        VZ = 1.0;
        Cd = 2.0;
        Cr = 1.7;
        DragArea = 15;
        SRPArea  = 1.0;
        DryMass  = 850;
    end

    %  Set the public data
    properties  (SetAccess = 'protected')
        STM = eye(6,6);
    end

    %  Define the methods
    methods

        %  The constructor
        function obj = Spacecraft(obj)

        end

        %  Initialize
        function obj = Initialize(obj,Sandbox)
            obj.STM = eye(6);
        end

        %  GetState
        function x = GetState(Sat,Id)

            if Id == 1;
                x = [Sat.X Sat.Y Sat.Z Sat.VX Sat.VY Sat.VZ]';
            elseif Id == 2;
                x = reshape(Sat.STM,36,1);
            elseif Id == 3
                x = Sat.Cd;
            elseif Id == 4
                x = Sat.Cr;
            elseif Id == 5
                x = Sat.Epoch;
            end

        end

        function Sat = SetState(Sat,Id,x)

            if Id == 1;

                Sat.X  = x(1,1);
                Sat.Y  = x(2,1);
                Sat.Z  = x(3,1);
                Sat.VX = x(4,1);
                Sat.VY = x(5,1);
                Sat.VZ = x(6,1);

            elseif Id == 2;

                Sat.STM = reshape(x,6,6)';

            elseif Id == 3

                Sat.Cd = x(1,1);

            elseif Id == 4

                Sat.Cr = x(1,1);

            elseif Id == 4

                Sat.Epoch = x(1,1);

            end
            
        end

    end

end
