classdef ConicalSensor < handle
    %CONICALSENSOR a basic conical sensor model
    
    properties (Access = 'private')
        % Double.  Sensor field of view in radians
        fieldOfView
    end
    
    methods
        function obj = ConicalSensor(fieldOfView)
            % Class constructor.  
            % I/O.  fieldOfView in radians.
            obj.fieldOfView = fieldOfView;
        end
        function obj = SetFieldOfView(obj,fieldOfView)
            % Sets field of view in radians
           obj.fieldOfView = fieldOfView;
        end
        function fieldOfView = GetFieldOfView(obj)
            % Returns field of view in radians
           fieldOfView = obj.fieldOfView; 
        end
    end
    
end

