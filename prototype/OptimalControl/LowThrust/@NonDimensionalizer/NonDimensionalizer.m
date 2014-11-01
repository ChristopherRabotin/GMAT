classdef NonDimensionalizer < handle
    %NONDIMENSIONLIZER: Base class for non-dimensionalization of decision
    %  vector and sparse Jacobian.
    
    properties (SetAccess = 'private')
        
        %%  The quantities to be scaled
        
        %  Integer: The number of decision variables
        numVars
        %  Integer: The number of constraints
        numCons
        
        %% Quantities used in scaling
        
        %  Real Array: Quantity that multiplies the decision variables
        decVecWeight
        %  Real Array: Quantity added to decision variables
        decVecShift
        %  Real:   Quantity that multiplies the cost Function
        costWeight
        %  Real Array: Quantity that muliplies the constraints
        conVecWeight
        %  Integer array:  Array of indeces for non-zero rows in Jacobian
        jacRowIdxVec
        %  Integer array:  Array of indeces for non-zero cols in Jacobian
        jacColIdxVec
        %  Integer:  number of entries in jacRowIdxVec and jacColIdxVec
        numRowsinSparsity
        
    end
    
    methods
        
        function Initialize(obj,numVars,numCons,...
                jacRowIdxVec,jacColIdxVec)
            %  Configure the non-dimensionalizer and prepare for use
            
            %  Set nubmer of variables, constraints and sparsity info.
            obj.numVars = numVars;
            obj.numCons = numCons;
            obj.jacRowIdxVec = jacRowIdxVec;
            obj.jacColIdxVec = jacColIdxVec;
            obj.numRowsinSparsity = length(jacColIdxVec);
            %  TODO:  Validate that jacRowIdxVec and jacColIdxVec are the
            %  same length.
            
            %  TODO:  Call the class specific methods to intialize
            %  data if necessary.
            
            % Initialize scaling parameters to ones and zeros.
            obj.decVecWeight = ones(obj.numVars);
            obj.decVecShift  = zeros(obj.numVars);
            obj.conVecWeight    = ones(obj.numRowsinSparsity);
            obj.costWeight   = 1;
        end
        
        function [decVec] =  ScaleDecisionVector(obj,decVec)
            %  Scale the decision vector
            for varIdx = 1:obj.numVars
                decVec(varIdx) = decVec(varIdx)*obj.decVecWeight(varIdx)+...
                    obj.decVecShift(varIdx);
            end
        end
        
        function [decVec] =  UnScaleDecisionVector(obj,decVec)
            %  Unscale the decision vector
            for varIdx = 1:obj.numVars
                decVec(varIdx) = (decVec(varIdx)-...
                    obj.decVecShift(varIdx))/obj.decVecWeight(varIdx);
            end
        end
        
        function [conVec] =  ScaleConstraintVector(obj,conVec)
            %  Scale the constraint vector
            for conIdx = 1:obj.numCons
                conVec(conIdx) = conVec(conIdx)*obj.conVecWeight(conIdx);
            end
        end
        
        function [conVec] =  UnScaleConstraintVector(obj,conVec)
            %  Unscale the constraint vector
            for conIdx = 1:obj.numCons
                conVec(conIdx) = conVec(conIdx)/obj.conVecWeight(conIdx);
            end
        end
        
        function [costFunc] =  ScaleCostFunction(obj,costFunc)
            %  Scale the cost function
            costFunc= costFunc*obj.costWeight;
        end
        
        function [costFunc] =  UnScaleCostFunction(obj,costFunc)
            %  Unscale the cost function
            costFunc= costFunc/obj.costWeight;
        end
        
        function [jacArray] = ScaleJacobian(obj,jacArray)
            %  Scale the Jacobian
            for arrIdx = 1:obj.numRowsinSparsity
                funIdx = obj.jacRowIdxVec(arrIdx);
                varIdx = obj.jacColIdxVec(arrIdx);
                jacArray(funIdx,varIdx) = ...
                    jacArray(funIdx,varIdx)*obj.conVecWeight(funIdx);
            end
        end
        
        function [jacArray] = UnScaleJacobian(obj,jacArray)
            %  Unscale the Jacobian
            for arrIdx = 1:obj.numRowsinSparsity
                funIdx = obj.jacRowIdxVec(arrIdx);
                varIdx = obj.jacColIdxVec(arrIdx);
                jacArray(funIdx,varIdx) = ...
                    jacArray(funIdx,varIdx)/obj.conVecWeight(funIdx);
            end
        end
        
        function SetDecVecScaling_Bounds(obj,decVecUpper,decVecLower)
            %  Compute the dec. vec. scaling parameters based on bounds
            for varIdx = 1:obj.numVars
                obj.decVecWeight(varIdx) = 1/(decVecUpper(varIdx) ...
                    -decVecLower(varIdx));
                obj.decVecShift(varIdx)  = 0.5 - decVecUpper(varIdx)*...
                    obj.decVecWeight(varIdx);
            end
        end
        
        function SetConstraintScaling_UserDefined(obj,conVecWeight)
            %  Set the scaling of the constraint functions directly
            
            %  Validate the input dimensions
            if length(conVecWeight) ~= obj.numCons
                errorMsg = ['Length of constraint weight vector'  ... 
                    ' must be equal to numCons'];
                errorLoc= ['NonDimensionalizer:' ...
                    'SetConstraintScaling_UserDefined'];
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
           
           % Set the weight array
           obj.conVecWeight = conVecWeight;
        end

        function SetCostScaling_UserDefined(obj,costWeight)
            %  Set the scaling of the cost functin directly
            
            %  Validate the input dimensions
            if size(costWeight,1) ~= 1 && size(costWeight,2) ~= 1
                errorMsg = 'Cost weight must be a 1x1 array';
                errorLoc= ['NonDimensionalizer:' ...
                    'SetCostScaling_UserDefined'];
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
           
           % Set the weight array
           obj.costWeight = costWeight;
        end
        
    end
    
end

