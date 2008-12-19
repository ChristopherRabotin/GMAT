classdef Measurement < handle

    % Set the public properties
    properties  (SetAccess = 'public')
        AddMeasurement = {};
    end

    % Set the private properties
    properties  (SetAccess = 'private')
        Types = [];
        Participants = {};
    end

    % Set the methods
    methods

        %----- Initialization
        function obj = Initialize(obj,Sandbox)

            for i = 1:size(obj.AddMeasurement,2)

                %  Extact the ith data type
                currMeasurement = obj.AddMeasurement{i};

                %  Determine measurement type and assign numeric Id
                if strcmp(currMeasurement{1},'Range')
                    obj.Types(i) = 1;
                elseif strcmp(currMeasurement{1},'RangeRate')
                    obj.Types(i) = 2;
                end
                
                %  KLUDGE: ADD PARTICIPANTS TO CELL ARRAY
                obj.Participants{1} = {Sandbox.GetHandle(currMeasurement{2}) ...
                                     Sandbox.GetHandle(currMeasurement{3})};

            end

        end %----- function Intialize

        function [y,dydx] = Evaluate(Meas)

            global jd_ref
            
            for i = 1:size(Meas.Types,2)

                % Calculate the measurement and the partial
                if Meas.Types(i) == 1

                    %  THIS IS A KLUDGE
                    Station = Meas.Participants{1}{2};
                    Sat     = Meas.Participants{1}{1};

                    GS_EF = [Station.X Station.Y Station.Z]';

                    %==========================================================================
                    %==========================================================================
                    %---- Calculate Greenwich Meantime and rotation from fixed to inertial
                    %               THIS SHOULD BE MOVED SOMEWHERE ELSE!!!!
                    %==========================================================================
                    %==========================================================================
                    currJD   = Sat.Epoch + 2430000;
                    T_UT1    = (currJD - 2451545)/36525;
                    GMST     = 67310.54841 + (876600*3600 + 8640184.812866)*T_UT1+ ...
                        0.093104*T_UT1^2 - 6.2e-6*T_UT1^3;

                    while GMST < -86400
                        GMST = GMST + 86400;
                    end

                    while GMST > 86400
                        GMST = GMST - 86400;
                    end

                    GMST = GMST*pi/43200;
                    R_IF     = R3(GMST);

                    %==========================================================================
                    %==========================================================================
                    %---- Calculate the range vector, the range measurement and the partials
                    %==========================================================================
                    %==========================================================================
                    rangevec       = [Sat.X Sat.Y Sat.Z]' - R_IF'*GS_EF ;

                    y    = sqrt(rangevec'*rangevec);
                    dydx = rangevec'/y;

                end

            end
        end

    end % methods

end % classdef