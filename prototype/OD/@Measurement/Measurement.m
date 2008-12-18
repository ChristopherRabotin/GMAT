classdef Measurement < handle

    % Set the public properties
    properties  (SetAccess = 'public')
        AddMeasurement = {};
    end
    
    % Set the private properties
    properties  (SetAccess = 'private')
        Types = [];
        Particpants = {};
    end

    % Set the methods
    methods

        %----- Initialization 
        function obj = Initialize(obj)
            
            for i = 1:size(obj.AddMeasurement,2)

                %  Extact the ith data type
                currMeasurement = obj.AddMeasurement{i};

                %  Determine measurement type and assign numeric Id
                if strcmp(currMeasurement{1},'Range')
                    obj.Types(i) = 1;
                elseif strcmp(currMeasurement{1},'RangeRate')
                    obj.Types(i) = 2;
                end

                %  Set pointers to participants.  These are locations of objects in
                %  ObjectStore.
                %  THIS IS A HARD CODED KLUDGE
                Measurement.Participants{1} = [1 2];
                Measurement.Participants{2} = [1 2];

            end

        end %----- function Intialize

    end % methods
    
end % classdef