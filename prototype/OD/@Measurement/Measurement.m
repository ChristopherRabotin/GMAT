classdef Measurement < handle
    
    % Set the public properties
    properties  (SetAccess = 'public')
        Measurements = {};
    end
    
    % Set the methods
    methods
        
        function Meas = AddMeasurement(Meas,varargin)
            numMeas   = size(Meas.Measurements,1)
            numFields = size(varargin)
            varargin
        end % employee
        
    end
end