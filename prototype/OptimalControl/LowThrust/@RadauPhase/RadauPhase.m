classdef RadauPhase < Phase
    %UNTITLED5 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
        radauPoints
        radauWeights
        radauDiffMatrix
        numRadauPoints
        
    end
    
    methods
                
        %  Compute Radau points, weights, and differentiation matrix
        function obj = GetTranscriptionProperties(obj)
            
            %  Compute Radau points, weights, and differentiation matrix
            [obj.radauPoints,obj.radauWeights,obj.radauDiffMatrix]...
                = lgrPS(obj.meshIntervalFractions, ...
                obj.meshIntervalNumPoints);
            obj.numRadauPoints = length(obj.radauWeights);
            
            %  Compute the number of mesh points
            obj.numMeshPoints    = obj.numRadauPoints;
            obj.numStatePoints   = obj.numRadauPoints + 1;
            obj.numControlPoints = obj.numRadauPoints;
            obj.numTimeParams = 2;  % TODO: Hard coded for Radau!
            obj.numCollocationPoints = obj.numControlPoints;
            
        end
        
        %  Returns the time decision variables in [2x1] matrix form
        function  timeMat = GetMeshPoints(obj)
            timeMat = reshape(obj.DecVector.decisionVector...
                (obj.timeStartIdx:obj.timeEndIdx),...
                obj.numTimeParams,1);
        end
        
        %  Configure the time vector
        function ComputeTimeVector(obj)
            initialTime = obj.DecVector.GetFirstTime();
            obj.timeVector = (obj.DecVector.GetLastTime() - initialTime)*...
                (obj.radauPoints + 1)/2 + initialTime;
        end
        
    end
    
end

