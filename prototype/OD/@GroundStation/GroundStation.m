classdef GroundStation < handle

    % Set the public properties
    properties  (SetAccess = 'public')
        
        GroundStationID  = 21;
        CentralBody      = 'Earth';
        CentralBodyShape = 'OblateSphere';
        StateType        = 'Cartesian';
        Sensors          = {}
        %----- Define the location
        X                = -4460.9879936
        Y                =  2682.3627968
        Z                = -3674.6265773

    end

    % Set the methods
    methods

        %-----  Initialize
        function obj = Initialize(obj)
             
        end  

    end
end