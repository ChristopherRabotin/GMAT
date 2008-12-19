classdef BatchEstimator < handle

    % Set the public properties
    properties  (SetAccess = 'public')
        MaxIterations = 25;
        Measurements  = {};
        SolveFor      = {};
        Propagator
        ESV;
        SolutionEpoch
    end

    % Set the private properties
    properties  (SetAccess = 'private')
        numPropObjects = 0;
        ESM;
    end

    % Set the methods
    methods

        %-----  Constructor
        function obj = BatchEstimator(obj)
            obj.ESM.PropObjects  = {};
            obj.ESM.PropParamId  = {};
            obj.ESM.EstimObjects = {};
            obj.ESM.EstimParamId = {};
        end

        %-----  Initialize
        function obj = Initialize(obj,Sandbox)

            %  Get handle to propagator
            obj.Propagator = Sandbox.GetHandle(obj.Propagator);
            
            %  Get handle to measurements
            %for i = 1:size(obj.Measurements,2)
               obj.Measurements = Sandbox.GetHandle(obj.Measurements);
            %end

            %=============================================================
            %=============================================================
            %---- Determine what objects and properties will need to be
            %     estimated or proagpated and set their pointers
            %     handles and ids
            %=============================================================
            %=============================================================
            for i = 1:size(obj.SolveFor,2)

                %  Extract names of object and property
                solveFor           = obj.SolveFor{i};
                [objName,propName] = ParseSolveFor(solveFor);

                %  Get handle from SandBox for the current object
                solveObj = Sandbox.GetHandle(objName);
                propId   = GetParamId(propName);

                %  If object is a spacecraft not already in
                %  Prop Objects list, then add it, and add properties
                %  to prop PropParamId if they are not there already.
                if strcmp(class(solveObj),'Spacecraft')

                    objIndex = 0;
                    if obj.numPropObjects == 0
                        obj.numPropObjects = obj.numPropObjects + 1;
                        obj.ESM.PropObjects{obj.numPropObjects} = solveObj;
                        objIndex = obj.numPropObjects;
                    else

                        %  Check existing prop Objects
                        for i = 1:size(obj.ESM.PropObjects,2)

                            if ~isequal(obj.ESM.PropObjects{i},solveObj)
                                obj.numPropObjects = obj.numPropObjects + 1;
                                obj.ESM.PropObjects{obj.numPropObjects} = solveObj;
                            else
                                objIndex = obj.numPropObjects;
                            end

                        end

                    end % obj.numPropObjects == 0

                end % strcmp(class(solveObj),'Spacecraft')

                %  HARD CODE THE PARAMETER IDS FOR PROPAGATION!!!!
                %  HARD CODE THE ESTIMATOR OBJECTS AND IDS
                %
                obj.ESM.PropParamId{1} = [1 2];
                obj.ESM.EstimObjects   = {solveObj};
                obj.ESM.EstimParamId   = {1};
            end % i = 1:size(obj.SolveFor,2)

        end % obj = Initialize(obj,Sandbox)  

    end % methods

end % classdef