classdef Spacecraft < handle
    % SPACECRAFT: A spacecraft model.
    
    properties (Access = private)
        % OrbitState oject.  The orbital state.
        orbitState
        % AbsoluteDate object.  The orbit epoch.
        orbitEpoch
        % Int. The number of attached sensors
        numSensors = 0
        % Vector of sensor objects
        sensorList
        % Bool. Indicates if spacecraft has sensors
        hasSensors = false
    end
    
    methods
        
        function obj = Spacecraft(orbitEpoch,orbitState)
            % Class constructor
            obj.orbitState = orbitState;
            obj.orbitEpoch = orbitEpoch;
        end
        
        function orbitState = GetOrbitState(obj)
            % Returns the orbit state object
            orbitState = obj.orbitState;
        end
        
        function orbitEpoch = GetOrbitEpoch(obj)
            % Returns the orbit epoch object
            orbitEpoch = obj.orbitEpoch;
        end
        
        function jDate = GetJulianDate(obj)
            % Returns Julian date of spacecraft
            jDate = obj.orbitEpoch.GetJulianDate();
        end
        
        function cartState = GetCartesianState(obj)
            % Returns Cartesian state of spacecraft
            cartState = obj.orbitState.GetCartesianState();
        end
        
        function AddSensor(obj,sensorModel)
            % Adds a sensor to the spacecraft
            obj.numSensors = obj.numSensors + 1;
            obj.hasSensors = true;
            obj.sensorList{obj.numSensors} = sensorModel;
        end
        
        function sensorFOV = GetSensorFOV(obj,sensorNum)
            % Returns field of view given the sensor number
            if obj.hasSensors
                sensorFOV = obj.sensorList{sensorNum}.GetFieldOfView();
            else
                error('Spacecraft does not have any sensors')
            end
        end
        
        function hasSensors = GetHasSensors(obj)
            %  Returns bool indicating if spacecraft has sensors or not
            hasSensors = obj.hasSensors;
        end
    end
    
end

