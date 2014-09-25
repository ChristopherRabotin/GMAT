classdef Phase < handle
    %PHASE Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (SetAccess = 'public')
        
        
        %%  Bounds during phase
        controlUpperBound
        controlLowerBound
        stateUpperBound
        stateLowerBound
        
        %%  Bounds at beginning and end of phase
        initialTimeUpperBound
        initialTimeLowerBound
        finalTimeUpperBound
        finalTimeLowerBound
        initialStateLowerBound
        initialStateUppperBound
        finalStateLowerBound
        finalStateUpperBound
        finalGuessState
        finalGuessEpoch
        
        %%  Initial Guess
        initialEpoch
        finalEpoch
        initialGuessState
        initialGuessEpoch
        initialGuessMode
        initialGuessControl
        
        %%  Phase properties
        numStates
        numControls
        
        decisionVector
        timeVector
        
        %% User functions
        pathFunctionName
        pointFunctionName
        
        %%  Bound vectors for different constraint types. These are
        %  assembled from user inputs.
        algConstraintLowerBound
        algConstraintUpperBound
        eventConstraintLowerBound
        eventConstraintUpperBound
        intConstraintUpperBound
        intConstraintLowerBound
        decisionVecLowerBound
        decisionVecUpperBound
        
        phaseNum
        
    end
    
    properties (SetAccess = 'protected')
        
        %  These do not go here
        meshPoints
        numMeshPoints
        numCollocationPoints
        
        %  Cost and constraint related parameters
        numStatePoints
        numControlPoints
        numStateParams
        numTimeParams
        numControlParams
        numStaticParams = 0;
        numDecisionParams
        defectConstraintVec
        algConstraintVec
        eventConstraintVec
        intConstraintVec
        costFuncIntegral
        costFuncPoint
        defectConLowerBound
        defectConUpperBound
        pathConLowerBound
        pathConUpperBound
        eventConLowerBound
        eventConUpperBound
        allConLowerBound
        allConUpperBound
        numConstraints
        numDefectConstraints
        numPathConstraints
        numEventConstraints
        
        %%  Bookkeeping parameters
        stateStartIdx
        controlStartIdx
        timeStartIdx
        stateEndIdx
        controlEndIdx
        timeEndIdx
        %  Constraint bookkeeping
        defectStartIdx
        pathStartIdx
        eventStartIdx
        defectEndIdx
        pathEndIdx
        eventEndIdx
        
        %%  Helper classes
        % For evaluating user functions
        PathFunction;% = UserPathFunction();
        % For evaluating user functions
        PointFunction;% = UserPointFunction();
        %  For managinging decision vector
        DecVector;% = DecisionVector();
        %probTranscription();  %  For transcription details
        
    end
    
    properties (SetAccess = 'private')
        numMeshIntervals
    end
    
    methods
        
        %  The constructor
        function obj = Phase()
            obj.DecVector     = DecisionVector();
            obj.PathFunction  = UserPathFunction();
            obj.PointFunction = UserPointFunction();
        end
        
        %  Initialize the phase
        function obj = Initialize(obj)
            
            %  Compute the properties for defined mesh properties
            ValidatePhaseConfig(obj);
            
            %  Get properties that are dependent upon the transcription
            %  being used.  These come from the leaf classes
            obj = GetTranscriptionProperties(obj);
            
            %  Compute bookkeeping properties of the discretization
            obj.numStateParams   = obj.numStates*obj.numStatePoints;
            obj.numControlParams = obj.numControls*obj.numControlPoints;
            obj.numDecisionParams = obj.numStateParams + ...
                obj.numControlParams + obj.numStaticParams + ...
                obj.numTimeParams;
            
            obj = SetStateChunkIndeces(obj);
            
            %  Initialize the decision vector helper class
            numIntegrals = 0;  % TODO:  should not be hard coded
            obj.DecVector.ConfigureDecisionVector(obj.numStates, ...
                obj.numControls,numIntegrals,obj.numStaticParams,...
                obj.numStatePoints,obj.numControlPoints)
            
            %  Compute and set the initial guess and configure time.  This
            %  will need to be handled on a transcription bases.
            obj = SetInitialGuess(obj);
            obj.ComputeTimeVector();
            
            %  Intialize the user functions
            obj.PathFunction.phaseNum = obj.phaseNum;
            obj.PointFunction.phaseNum = obj.phaseNum;
            obj.PathFunction.functionName  = obj.pathFunctionName;
            obj.PointFunction.functionName = obj.pointFunctionName;
            obj.PreparePathFunction(1);  % Evaluate at first point
            obj.PathFunction.Initialize();
            obj.ComputePathFunctions();
            obj.PreparePointFunction();
            obj.PointFunction.Initialize();
            
            %  Set chunk values and bounds.
            obj = SetConstraintChunkIndeces(obj);
            obj = SetConstraintBounds(obj);
            obj = SetDecisionVectorBounds(obj);
            
        end
        
        function obj = SetStateChunkIndeces(obj)
            %  Set start and end indeces of different state paramter types.
            %  These describe where different chunks of the decision vector
            %  begin and end.
            obj.stateStartIdx = 1;
            obj.stateEndIdx  = obj.stateStartIdx + obj.numStateParams -1;
            obj.controlStartIdx = obj.stateEndIdx + 1;
            obj.controlEndIdx = obj.controlStartIdx + ...
                obj.numControlParams -1;
            obj.timeStartIdx = obj.controlEndIdx + 1;
            obj.timeEndIdx = obj.timeStartIdx + obj.numTimeParams  - 1;
        end
        
        %  MERGE
        function obj = SetConstraintChunkIndeces(obj)
            %  Set start and end indeces of different constraint paramter types.
            %  These describe where different chunks of the contstraint vector
            %  begin and end.
            obj.numDefectConstraints = obj.numStates*...
                obj.numCollocationPoints;
            obj.numPathConstraints = obj.numMeshPoints*...
                obj.PathFunction.numAlgFunctions;
            obj.numEventConstraints = obj.PointFunction.numEventFunctions;
            obj.defectStartIdx = 1;
            obj.defectEndIdx  = obj.defectStartIdx + ...
                obj.numDefectConstraints -1;
            obj.pathStartIdx = obj.defectEndIdx + 1;
            obj.pathEndIdx = obj.pathStartIdx + obj.numPathConstraints -1;
            obj.eventStartIdx = obj.pathEndIdx + 1;
            obj.eventEndIdx = obj.eventStartIdx + ...
                obj.numEventConstraints -1;
            obj.numConstraints = obj.numDefectConstraints + ...
                obj.numPathConstraints + obj.numEventConstraints;
        end
        
        %  MERGE
        function obj = SetDecisionVectorBounds(obj)
            
            % numPoints = obj.numStatePoints;
            %  Dimension the vectors
            obj.decisionVecLowerBound = zeros(obj.numDecisionParams,1);
            obj.decisionVecUpperBound = zeros(obj.numDecisionParams,1);
            
            %  Fill in the state vector portion of the bounds
            for i = 1:obj.numStates
                obj.decisionVecLowerBound((i-1)*obj.numStatePoints+1) = ...
                    obj.initialStateLowerBound(i);
                obj.decisionVecLowerBound(((i-1)*obj.numStatePoints+2):...
                    (i*obj.numStatePoints-1)) = obj.stateLowerBound(i);
                obj.decisionVecLowerBound(i*obj.numStatePoints) = ...
                    obj.finalStateLowerBound(i);
                obj.decisionVecUpperBound((i-1)*obj.numStatePoints+1) = ...
                    obj.initialStateUppperBound(i);
                obj.decisionVecUpperBound(((i-1)*obj.numStatePoints+2):...
                    (i*obj.numStatePoints-1)) = obj.stateUpperBound(i);
                obj.decisionVecUpperBound(i*obj.numStatePoints) = ...
                    obj.finalStateUpperBound(i);
            end
            
            %  Fill in the control vector portion of the bounds
            cnt = obj.numStates*obj.numStatePoints+1;
            for i = 1:obj.numControls
                obj.decisionVecLowerBound(cnt:cnt+obj.numRadauPoints-1)=...
                    obj.controlLowerBound(i);
                obj.decisionVecUpperBound(cnt:cnt+obj.numRadauPoints-1)=...
                    obj.controlUpperBound(i);
                cnt = cnt+obj.numRadauPoints;
            end
            
            %  Fill in the static params portion of the bounds
            %  TODO: Implement static parameters
            
            %  Fill in the time portion of the bounds
            cnt = obj.numDecisionParams - 1;
            obj.decisionVecUpperBound(cnt:cnt+1) = ...
                [obj.initialTimeUpperBound obj.finalTimeUpperBound]';
            obj.decisionVecLowerBound(cnt:cnt+1) = ...
                [obj.initialTimeLowerBound obj.finalTimeLowerBound]';
            
        end
        
        %  MERGE
        function obj = SetConstraintBounds(obj)
            obj = SetDefectConstraintBounds(obj);
            obj = SetPathConstraintBounds(obj);
            obj = SetEventConstraintBounds(obj);
            
            dStartIdx = obj.defectStartIdx;
            dEndIdx = obj.defectEndIdx;
            pStartIdx = obj.pathStartIdx;
            pEndIdx = obj.pathEndIdx;
            eStartIdx = obj.eventStartIdx;
            eEndIdx = obj.eventEndIdx;
            obj.allConLowerBound(dStartIdx:dEndIdx,1) = ...
                obj.defectConLowerBound;
            obj.allConUpperBound(dStartIdx:dEndIdx,1) = ...
                obj.defectConUpperBound;
            obj.allConLowerBound(pStartIdx:pEndIdx,1) = ...
                obj.pathConLowerBound;
            obj.allConUpperBound(pStartIdx:pEndIdx,1) = ...
                obj.pathConUpperBound;
            obj.allConLowerBound(eStartIdx:eEndIdx,1) = ...
                obj.eventConLowerBound;
            obj.allConUpperBound(eStartIdx:eEndIdx,1) = ...
                obj.eventConUpperBound;
        end
        
        %  MERGE
        function obj = SetDefectConstraintBounds(obj)
            obj.defectConLowerBound = ...
                zeros(obj.numStates*obj.numRadauPoints,1);
            obj.defectConUpperBound = ...
                zeros(obj.numStates*obj.numRadauPoints,1);
        end
        
        %  MERGE
        function obj = SetPathConstraintBounds(obj)
            if obj.PathFunction.hasAlgFunctions
                numPathCon = obj.numRadauPoints*...
                    obj.PathFunction.numAlgFunctions;
                obj.pathConLowerBound = zeros(numPathCon,1);
                obj.pathConUpperBound = zeros(numPathCon,1);
                lowIdx = 1;
                for conIdx = 1:obj.numRadauPoints
                    highIdx = lowIdx + obj.PathFunction.numAlgFunctions-1;
                    obj.pathConLowerBound(lowIdx:highIdx,1) = ...
                        obj.algConstraintLowerBound;
                    obj.pathConUpperBound(lowIdx:highIdx,1) = ...
                        obj.algConstraintUpperBound;
                    lowIdx = lowIdx + obj.PathFunction.numAlgFunctions;
                end
            else
                obj.pathConLowerBound = [];
                obj.pathConUpperBound = [];
            end
        end
        
        %  Set bounds on event constraints
        function obj = SetEventConstraintBounds(obj)
            obj.eventConLowerBound = obj.eventConstraintLowerBound;
            obj.eventConUpperBound = obj.eventConstraintUpperBound;
        end
        
        %  Compute the initial guess
        function obj = SetInitialGuess(obj)
            
            switch obj.initialGuessMode
                
                case {'LinearNoControl','LinearUnityControl','LinearCoast'}
                    
                    % Supply an initial guess
                    xguess = zeros(obj.numStates*obj.numStatePoints,1);
                    uguess = zeros(obj.numControls*obj.numControlPoints,1);
                    for i=1:obj.numStates
                        xguess((i-1)*obj.numStatePoints+1:...
                            i*obj.numStatePoints)  = ...
                            linspace(obj.initialGuessState(i),...
                            obj.finalGuessState(i),obj.numStatePoints).';
                    end
                    if strcmp(obj.initialGuessMode,'LinearUnityControl')
                        controlVar = 1;
                    else
                        controlVar = 0;
                    end
                    if strcmp(obj.initialGuessMode,'LinearUnityControl') ...
                            || strcmp(obj.initialGuessMode,'LinearNoControl')
                        
                        
                        for i=1:obj.numControls
                            uguess((i-1)*obj.numControlPoints+1:...
                                i*obj.numControlPoints)  = ...
                                linspace(controlVar,controlVar,...
                                obj.numControlPoints).';
                        end
                    else
                        uguess = [];
                    end
                    obj.DecVector.SetStateVector(xguess);
                    obj.DecVector.SetControlVector(uguess);
                    obj.DecVector.SetTimeVector(...
                        [obj.initialEpoch; obj.finalEpoch]);
            end
        end
        
        %  Validate that the mesh properties are set acceptably
        function ValidateMeshProperties(obj)
            
            %  Check that sizes are consistent
            if size(obj.meshIntervalPoints,2) ~=  ...
                    size(obj.meshIntervalFractions,2)
                errorMsg = ['meshIntervalPoints must have the same' ...
                    'number of columns as meshFraction'];
                errorLoc  = 'Phase:SetMeshProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            %  Check that each are row vectors
            if size(obj.meshIntervalPoints,1) ~= 1
                errorMsg = 'meshIntervalPoints must be a row vector';
                errorLoc  = 'Phase:SetMeshProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            if size(obj.meshIntervalFractions,1) ~= 1
                errorMsg = 'meshFraction must be a row vector';
                errorLoc  = 'Phase:SetMeshProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
        end
        
        % Validate that the bounds have been set appropriately
        function ValidateBoundProperties(obj)
            
            if  obj.positionUpperBound <= obj.positionLowerBound
                errorMsg = ['positionUpperBound must be greater than' ...
                    'positionLowerBound'];
                errorLoc  = 'Phase:ValidateBoundProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            if  obj.velocityUpperBound <= obj.velocityLowerBound
                errorMsg = ['velocityUpperBound must be greater than' ...
                    'velocityLowerBound'];
                errorLoc  = 'Phase:ValidateBoundProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            if  obj.massUpperBound <= obj.massLowerBound
                errorMsg = ['massUpperBound must be greater than' ...
                    'massLowerBound'];
                errorLoc  = 'Phase:ValidateBoundProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
        end
        
        % Validate that the bounds have been set appropriately
        function ValidateStateProperties(obj)
            
            [rows,cols] = size(obj.initialEpoch);
            if rows ~= 1 && cols ~= 1
                errorMsg = 'initialEpoch must scalar real';
                errorLoc  = 'Phase:ValidateStateProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            [rows,cols] = size(obj.finalEpoch);
            if rows ~= 1 && cols ~= 1
                errorMsg = 'finalEpoch must scalar real';
                errorLoc  = 'Phase:ValidateStateProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            [rows,cols] = size(obj.initialGuessEpoch);
            if rows ~= 1 && cols ~= 1
                errorMsg = 'intialGuessEpoch must scalar real';
                errorLoc  = 'Phase:ValidateStateProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            [rows,cols] = size(obj.initialGuessState);
            if rows ~= 7 && cols ~= 1
                errorMsg = 'intialGuessState must be 7x1 vector';
                errorLoc  = 'Phase:ValidateStateProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
        end
        
        %  Sets decision vector provided by optimizer
        function obj = SetDecisionVector(obj,decVector)
            
            %  Set the decision vector on the trajectory.
            %  Maybe this is not needed, maybe we should just pass straight
            %  through to phases.  Not Sure.
            if length(decVector) == obj.numDecisionParams
                obj.DecVector.SetDecisionVector(decVector);
            else
                errorMsg = ['Length of decisionVector must be ' ...
                    ' equal to totalnumDecisionParams'];
                errorLoc  = 'Phase:SetDecisionVector';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            %  Update the time vector accordingly.
            obj.ComputeTimeVector()
            
        end
        
        %  Validates user configuration of the phase
        %  TODO:  Radau specific.
        function obj = ValidatePhaseConfig(obj)
            if length(obj.meshIntervalFractions) ~= ...
                    length(obj.meshIntervalNumPoints) + 1
                errorMsg = ['Length of meshIntervalNumPoints' ...
                    ' must be the one greater than length ' ....
                    ' of meshIntervalFractions' ];
                errorLoc  = 'Phase:ValidatePhaseConfig';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
        end
        
        function [contraintVec] = GetContraintVector(obj)
            % Call path and point functions and then assemble the
            % constraint vector
            obj.ComputePathFunctions();
            obj.ComputePointFunctions();
            if obj.PathFunction.hasDynFunctions
                contraintVec = ...
                    [obj.defectConstraintVec];
            end
            if obj.PathFunction.hasAlgFunctions
                contraintVec = [contraintVec; ...
                    obj.algConstraintVec];
            end
            if obj.PointFunction.hasEventFunctions
                contraintVec = [contraintVec; ...
                    obj.eventConstraintVec];
            end
        end
        
        %  Compute defect constraints
        function defectConstraints = ComputePathFunctions(obj)
            
            % stateMat = obj.DecVector.GetStateArray();
            algIdx    = 1;
            obj.costFuncIntegral = 0;
            
            %  Loop over the mesh points to compute functions at each point
            for pointIdx = 1:obj.numMeshPoints
                
                %  Prepare then evaluate the path function
                obj.PreparePathFunction(pointIdx);
                obj.PathFunction.Evaluate();
                
                %  Handle defect constraints
                if obj.PathFunction.hasDynFunctions
                    rhsMatrix(pointIdx,1:obj.numStates) = ...
                        obj.PathFunction.dynFunctions';
                end
                
                %  Handle cost function
                if obj.PathFunction.hasCostFunction
                    obj.costFuncIntegral = obj.costFuncIntegral + ...
                        obj.radauWeights(pointIdx)*...
                        obj.PathFunction.costFunction;
                end
                
                %  Handle algebraic constraints
                if obj.PathFunction.hasAlgFunctions
                    algArray(algIdx:algIdx+...
                        obj.PathFunction.numAlgFunctions-1,1) = ...
                        obj.PathFunction.algFunctions;
                    algIdx = algIdx + obj.PathFunction.numAlgFunctions;
                end
                
            end
            
            %  Set defect constraints
            obj.ComputeCostFunctionIntegral();
            obj.ComputeDefectConstraints(rhsMatrix);
            
            %  Set algebraic constraints
            if obj.PathFunction.hasAlgFunctions
                obj.algConstraintVec = algArray;
            end
            
        end
        
        %  Evaluate the user point functions
        function ComputePointFunctions(obj)
            %  Prepare and call the user function
            obj.PreparePointFunction();
            obj.PointFunction.Evaluate();
            %  Set event constraints if there are any
            if obj.PointFunction.hasEventFunctions
                obj.eventConstraintVec = obj.PointFunction.eventFunctions;
            end
            if obj.PointFunction.hasCostFunction
                obj.costFuncPoint =  obj.PointFunction.costFunction;
            end
            if obj.PointFunction.hasIntegralFunctions
                obj.intConstraintVec = obj.PointFunction.intFunctions;
            end
        end
        
        %  Compute defect constraints
        function costFunction = GetCostFunction(obj)
            %  Warning: Call to GetCostFunction must ALWAYS be preceded by
            %  call to GetContraintVector()
            costFunction = 0;
            if obj.PathFunction.hasCostFunction
                costFunction = costFunction + obj.costFuncIntegral;
            end
            if obj.PointFunction.hasCostFunction
                costFunction = costFunction + obj.costFuncPoint;
            end
        end
        
        %  Configure input structure for given mesh point
        function inputData = UpdateInputData(obj,pointIdx)
            
            %  This function extracts the state, control, time for state
            %  data from the decision vector at the mesh poin "pointIdx"
            inputData.stateVec =...
                obj.DecVector.GetStateAtMeshPoint(pointIdx);
            inputData.controlVec = ...
                obj.DecVector.GetControlAtMeshPoint(pointIdx);
            
        end
        
        %  Configure input structure for given mesh point
        function PreparePathFunction(obj,pointIdx)
            
            %  This function extracts the state, control, time for state
            %  data from the decision vector at the mesh point "pointIdx"
            obj.PathFunction.stateVec =...
                obj.DecVector.GetStateAtMeshPoint(pointIdx);
            obj.PathFunction.controlVec = ...
                obj.DecVector.GetControlAtMeshPoint(pointIdx);
            
        end
        
        %  Configure input structure for given mesh point
        function PreparePointFunction(obj)
            
            %  This function extracts the state, control, time for state
            %  data from the decision vector at the mesh point "pointIdx"
            obj.PointFunction.initialStateVec =...
                obj.DecVector.GetFirstStateVector();
            obj.PointFunction.finalStateVec = ...
                obj.DecVector.GetLastStateVector();
            obj.PointFunction.initialControlVec = ...
                obj.DecVector.GetLastControlVector();
            obj.PointFunction.intConstraintVec = obj.intConstraintVec;
            obj.PointFunction.initialTime = ...
                obj.DecVector.GetFirstTime();
            obj.PointFunction.finalTime = ...
                obj.DecVector.GetLastTime();
            
        end
        
        %  Evaluate constraint feasiblity
        function Status = CheckConstraintVariances(obj)
            
            %  Evaluate constraints initialize items
            constraintVec   = GetContraintVector(obj);
            if isa(constraintVec,'gradient')
                constraintVec = constraintVec.x;
            end
            numEventViolations  = 0;
            numdefectViolations = 0;
            numPathViolations   = 0;
            eventViolation  = false();
            defectViolation = false();
            pathViolation   = false();
            tol = 1e-5;  %  TODO.  Don't hard code this
            
            %  Check point (event) constraints for violation
            if obj.PointFunction.hasEventFunctions;
                for conIdx = obj.eventStartIdx:obj.eventEndIdx
                    if constraintVec(conIdx) <= ...
                            obj.allConLowerBound(conIdx) - tol|| ...
                            constraintVec(conIdx) >= ...
                            obj.allConUpperBound(conIdx) + tol;
                        eventViolation = true();
                        numEventViolations = numEventViolations + 1;
                    end
                end
            end
            
            %  Check point (event) constraints for violation
            if obj.PathFunction.hasDynFunctions;
                for conIdx = obj.defectStartIdx:obj.defectEndIdx
                    if constraintVec(conIdx) <= ...
                            obj.allConLowerBound(conIdx) - tol || ...
                            constraintVec(conIdx) >= ...
                            obj.allConUpperBound(conIdx) + tol;
                        defectViolation = true();
                        numPathViolations = numPathViolations + 1;
                    end
                end
            end
            
            %  Check path constraints for violation
            if obj.PathFunction.hasAlgFunctions;
                for conIdx = obj.pathStartIdx:obj.pathEndIdx
                    if constraintVec(conIdx) <= ...
                            obj.allConLowerBound(conIdx) - tol|| ...
                            constraintVec(conIdx) >= ...
                            obj.allConUpperBound(conIdx) + tol;
                        pathViolation = true();
                        numdefectViolations = numdefectViolations + 1;
                    end
                end
            end
            Status.numEventViolations  = numEventViolations;
            Status.numdefectViolations = numdefectViolations;
            Status.numPathViolations   = numPathViolations;
            Status.eventViolation      = eventViolation;
            Status.defectViolation     = defectViolation;
            Status.pathViolation       = pathViolation;
        end
        
    end
    
end

%         %  Compute sparsity pattern of defects contraints
%         function [sparsityPattern] = GetSparsityPattern_Defect_State(obj)
%             sparsityPattern = abs(sign(obj.radauDiffMatrix));
%         end
%
%         %  Compute sparsity pattern of defects contraints
%         function [sparsityPattern] = GetSparsityPattern_Defect_Control(obj)
%             sparsityPattern = eye(obj.numRadauPoints);
%         end
%
%         %  Compute sparsity pattern of defects contraints
%         function [sparsityPattern] = GetSparsityPattern_Defect_Time(obj)
%             sparsityPattern = ones(obj.numRadauPoints,2);
%         end