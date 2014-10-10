classdef RadauPhase < Phase
    %UNTITLED5 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        % Row vector that contains the number of control points for
        % each mesh interval
        meshIntervalNumPoints
        %  Row vector that the time fraction each mesh makes of the whole
        %  phase.  If meshFraction = [1 1 1], then there are three mesh
        %  intervals, and each one is one third of the phase duration
        meshIntervalFractions
        numStateStagePoints;
        numControlStagePoints;
        stepSize
    end
    
    properties (SetAccess = 'protected')
        
        %  Time parameters
        timeMat
        %  Mesh point and radau transcription details
        radauPoints
        radauWeights
        radauDiffMatrix
        numRadauPoints
        numStateEvalPoints
        totalNumPoints
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
            obj.numMeshPoints        = obj.numRadauPoints;
            obj.numStatePoints       = obj.numRadauPoints + 1;
            obj.numControlPoints     = obj.numRadauPoints;
            obj.numTimeParams        = 2;
            obj.numCollocationPoints = obj.numControlPoints;
            obj.numStateEvalPoints   = obj.numMeshPoints;
            obj.totalNumPoints       = obj.numStatePoints;
            obj.meshPoints           = obj.radauPoints/2 + 1/2;
            obj.SetStageProperties();
            
            %  Compute bookkeeping properties of the discretization
            obj.numStateParams   = obj.numStates*obj.numStatePoints;
            obj.numControlParams = obj.numControls*obj.numControlPoints;
            obj.numDecisionParams = obj.numStateParams + ...
                obj.numControlParams + obj.numStaticParams + ...
                obj.numTimeParams;
            
            obj.numDefectConstraints = obj.numStates*...
                obj.numCollocationPoints;
        end
        
        %  Configure the time vector
        function ComputeTimeVector(obj)
            obj.ComputeTimeParameters();
            obj.timeVector = (obj.finalTime - obj.initialTime)*...
                (obj.radauPoints + 1)/2 + obj.initialTime;
        end
        
        %  Compute the defect constraints
        function ComputeQuadratures(obj,rhsMatrix,costVec)
            %  Compute defect constraints
            if obj.PathFunction.hasDynFunctions
                obj.ComputeDefectConstraints(rhsMatrix)
            end
            %  Compute cost function
            if obj.PathFunction.hasCostFunction
               obj.ComputeCostFunctionIntegral(costVec)
            end
        end
                
        %  Compute the defect constraints
        function ComputeDefectConstraints(obj,rhsMatrix)
            if obj.PathFunction.hasDynFunctions
                stateMat = obj.DecVector.GetStateArray();
                defectConstraintsLHS = obj.radauDiffMatrix*stateMat;
                obj.ComputeTimeParameters();
                defectConstraints = defectConstraintsLHS - ...
                    (obj.finalTime -obj.initialTime)*0.5*rhsMatrix;
                defectConstraints = reshape(defectConstraints,...
                    obj.numDefectConstraints,1);
                obj.defectConstraintVec = defectConstraints;
            end
        end
        
        %  Compute the cost function
        function ComputeCostFunctionIntegral(obj,costVec)
            if obj.PathFunction.hasCostFunction
                obj.costFuncIntegral = costVec'*obj.radauWeights;
                obj.ComputeTimeParameters();
                obj.costFuncIntegral = ((obj.finalTime -...
                    obj.initialTime)/2)*obj.costFuncIntegral;
            end
        end
        
        %  Configure time parameters
        function ComputeTimeParameters(obj)
            obj.timeMat     = GetInitialFinalTime(obj);
            obj.finalTime   = obj.DecVector.GetLastTime();
            obj.initialTime = obj.DecVector.GetFirstTime();
        end
              
        %  Function to set stage properties for the phase type
        function SetStageProperties(obj)
            obj.numStateStagePoints   = 0;
            obj.numControlStagePoints = 0;
            obj.numStagePoints        = 0;
        end
    end
end
