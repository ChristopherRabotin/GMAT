classdef Spacecraft < handle
    % SPACECRAFT: A spacecraft model.
    
    properties
        % OrbitState oject.  The orbital state.
        orbitState
        % OrbitEpoch object.  The orbit epoch.
        orbitEpoch
    end
    
    methods
        function obj = Spacecraft(orbitEpoch,orbitState)
           obj.orbitState = orbitState;
           obj.orbitEpoch = orbitEpoch;
        end
    end
    
end

