classdef Propagator < handle
    
    % Set the public properties
    properties  (SetAccess = 'public')
        CentralBody     = 'Earth';
        PointMasses     = {'Earth'};
        SRP             = 'Off';
        Type            = 'RungeKutta89';
        InitialStepSize = 60;
        Accuracy        = 1.0e-12;
        MinStep         = 0.001;
        MaxStep         = 2700;
        MaxStepAttempts = 50;
    end
    
    % Set the methods
    methods
        
%         function e = Initialize(name,dept)
%             e.Name = name;
%             e.Department = dept;
%         end % employee
        
    end
end