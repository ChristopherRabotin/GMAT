classdef Trajectory < handle
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (SetAccess = 'public')
        
        %%  Bound constraints for cost function
        
        % Real.  The lower bound on the cost function
        costLowerBound
        % Real.  The upper bound on the cost function
        costUpperBound
        
        %%  User function names
        
        %  String.  The name of the user path function
        pathFunctionName        = '';
        %  String.  The name of the user point function
        pointFunctionName       = '';
        %  String.  The name of the user plot function
        plotFunctionName        = '';
        
        %%  Plotting parameters
        
        %  Bool.  Flag indicating whether or not to show user plot
        showPlot                = false();
        %  Int.  Number of func evals between plot updates
        plotUpdateRate          = 5;
        
        %%  Linkage configuration parameters
        
        %  Array of "pointers" to the phases in
        %  each linkage config.  To join p1,p2, and p3 together, as well
        %  link phase p2 to p4
        %  linkageConfig{1} = {p1,p2,p3}
        %  linkageConfig{2} = {p2,p4}
        linkageConfig
        % Array of phases to be included in the optimization problem
        phaseList
        
    end
    
    properties (SetAccess = 'private')
        
        %%  Decision vector data members
        
        %  Real array. the total desicion vector for the optimization
        %  problem containing chunks for all phases.
        decisionVector
        % Real array.  The lower bound vector for the decision vector
        decisionVecLowerBound
        % Real array.  The upper bound vector for the decision vector
        decisionVecUpperBound
        % Integer array.  The ith entry is the start of the decision vector
        % chunk for the ith phase
        decVecStartIdx
        % Integer array.  The ith entry is the index for the start of
        % the decision vector chunk for the ith phase
        decVecEndIdx
        %  Integer.  The number of elements in the trajectory decision
        %  vector
        totalnumDecisionParams
        %  Integer Array.  The ith entry is the number of decision
        %  parameters for the ith phase
        numPhaseDecisionParams
        
        %%  Sparsity pattern data members
        
        %  Sparse matrix.  sparsity pattern for the cost function
        sparsityCost
        %  Sparse matrix.  Sparsity pattern for the constraints
        sparsityConstraints
        %  Sparse matrix.  The sparsity pattern for the complete problem
        %  including cost and constraints.
        sparsityPattern
        
        %%  Linkage related data members
        
        %  Array of Linkages. An array of Linkage classes, 
        %  one for each linkage in the linkageConfig arrays. 
        linkageList
        %  Integer Array.  Number of links in each config.
        numLinkages
        
        %%  Key quantities regarding problem dimensions
        
        % Integer.  The number of phases in the problem
        numPhases
        % Integer array.  The ith element is the number of constraints
        % in the ith phase.
        numPhaseConstraints
        % Real. The cost function value
        costFunction
        %  Real Array.  The lower bound vector for all constraints in the
        %  problem (i.e. for all phases).
        allConLowerBound
        %  Real Array.  The upper bound vector for all constraints in the
        %  problem (i.e. for all phases).
        allConUpperBound
        %  Integer Array.  The ith element is the index indicates where
        %  the constraints for the ith phase start in the concatenated
        %  constraint vector
        conPhaseStartIdx
        % Integer.  The total number of constraints including all phases
        % and linkages
        totalnumConstraints
        
        %% Housekeeping parameters
        
        %  Bool.  Flag indicating whether or not optimization is under way
        isOptimizing       = false();
        %  Bool.  Flag indicating that optimization is complete
        isFinished         = false();
        %  Int.  Number of func evals since the last plot update
        plotUpdateCounter  = 1;
        %  Bool.  Flag to write out debug data
        displayDebugStatus = 0;
        
    end
    
    methods (Access = public)  
        
        function obj = Initialize(obj)
            %  Initialize the trajectory and all helper classes.
            obj.InitializePhases();
            obj.InitializeLinkages();
            obj.SetBounds();
            obj.SetInitialGuess();
            obj.SetSparsityPattern();
            obj.WriteSetupReport()
        end
        
        function obj = SetDecisionVector(obj,decVector)
            %  Sets decision vector by calling each phase and setting
            %  chunks.
            if length(decVector) == obj.totalnumDecisionParams
                obj.decisionVector = decVector;
            else
                errorMsg = ['Length of decisionVector must be ' ...
                    ' equal to totalnumDecisionParams'];
                errorLoc  = 'Trajectory:SetDecisionVector';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            %  Eventually need to redimensionalize here
            
            %  Now loop over phases and set on each phase
            for phaseIdx = 1:obj.numPhases
                obj.phaseList{phaseIdx}.SetDecisionVector(...
                    decVector(obj.decVecStartIdx(phaseIdx):...
                    obj.decVecEndIdx(phaseIdx)));
            end
            
            %  Handle plotting
            if obj.showPlot
                obj.PlotUserFunction();
            end
            
        end
        
        function [costFunction,constraintVec] = ...
                GetCostConstraintFunctions(obj)
            %  Compute cost and constraint functions
            constraintVec = GetContraintVector(obj);
            costFunction = GetCostFunction(obj);
        end
        
        function [z,F,xmul,Fmul] = Optimize(obj)
            %  Call the optimizer.  This is the Main() function and should not
            %  go here.  Eventually it will move to a new class.
            % SNOPT Required Globals
            global igrid iGfun jGvar traj
            obj.Initialize();
            
            %  Set the bounds on the Decision Variables
            zmax = obj.decisionVecUpperBound;
            zmin = obj.decisionVecLowerBound;
            
            % Set the bounds on the NLP constraints
            Fmin = [obj.costLowerBound;obj.allConLowerBound];
            Fmax = [obj.costUpperBound;obj.allConUpperBound];
            z0   = obj.decisionVector;
            
            %% =====  Initialize the optimizer and execute the problem
            % Set the derivative option
            snseti('Derivative Option',1);
            % Set the derivative verification level
            snseti('Verify Level',-1);
            % Set name of SNOPT print file
            snprint('snoptmain.out');
            % Print CPU times at bottom of SNOPT print file
            snseti('Timing level',3);
            % Echo SNOPT Output to MATLAB Command Window
            snscreen on;
            %
            %snseti('Scale Option', 2)
            % snset('Major optimality tolerance 1e-3')
            % snset('Major feasibility tolerance 1e-6')
            
            % Set initial guess on basis and Lagrange multipliers to zero
            zmul = zeros(size(z0));
            zstate = zmul;
            Fmul = zeros(size(Fmin));
            Fstate = Fmul;
            ObjAdd = 0;
            % Row 1 is the objective function row
            ObjRow = 1;
            % Assume for simplicity that all constraints
            % are purely nonlinear
            AA      = [];
            iAfun   = [];
            jAvar   = [];
            userfun = 'SNOPTFunctionWrapper';
            
            [iGfun,jGvar] = find(obj.sparsityPattern);
            obj.isOptimizing = true();
            obj.plotUpdateCounter = 1;
            
            %  Call SNOPT.
            [z,F,xmul,Fmul,info,xstate,Fstate,ns,...
                ninf,sinf,mincw,miniw,minrw]...
                = snsolve(z0,zmin,zmax,zmul,zstate,...
                Fmin,Fmax,Fmul,Fstate,...
                ObjAdd,ObjRow,AA,iAfun,jAvar,iGfun,jGvar,userfun);
            obj.isOptimizing = false();
            obj.isFinished   = true();
            obj.PlotUserFunction();
            obj.WriteSetupReport(1);
            
        end
    end
    
    %% Private methods
    methods (Access = private)
        
        %%  Initialize link helper classes and construct the linkages
        function InitializeLinkages(obj)        
            linkIdx = 0;
            for configIdx = 1:length(obj.linkageConfig)
                while linkIdx < length(obj.linkageConfig{configIdx})-1
                    linkIdx = linkIdx + 1;
                    obj.linkageList{linkIdx} = LinkageConstraint;
                    obj.linkageList{linkIdx}.Initialize(...
                        obj.phaseList{linkIdx},obj.phaseList{linkIdx+1});
                    obj.totalnumConstraints = obj.totalnumConstraints + ...
                        obj.linkageList{linkIdx}.numLinkageConstraints;
                end
            end
            obj.numLinkages = linkIdx;
        end
        
        %% Intialize the phase helper classes and phase related data
        function InitializePhases(obj)
            
            %  Get properties of each phase for later book-keeping
            obj.numPhases = length(obj.phaseList);
            if obj.numPhases <= 0
                errorMsg = 'Trajectory requires at least one phase.';
                errorLoc  = 'Phase:Initialize';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            %  Initialize phases and a few trajectory data members
            obj.numPhaseConstraints = 1;
            obj.totalnumDecisionParams = 0;
            obj.totalnumConstraints = 0;
            for phaseIdx = 1:obj.numPhases
                
                %  Configure user function names
                obj.phaseList{phaseIdx}.pathFunctionName = ...
                    obj.pathFunctionName;
                obj.phaseList{phaseIdx}.pointFunctionName = ...
                    obj.pointFunctionName;
                
                %  Intialize the current phase
                obj.phaseList{phaseIdx}.phaseNum = phaseIdx;
                obj.phaseList{phaseIdx}.Initialize();
                
                %  Extract decision vector and constraint properties
                obj.numPhaseConstraints(phaseIdx) = ...
                    obj.phaseList{phaseIdx}.numConstraints;
                obj.numPhaseDecisionParams(phaseIdx) = ...
                    obj.phaseList{phaseIdx}.numDecisionParams;
                obj.totalnumDecisionParams = ...
                    obj.totalnumDecisionParams + ...
                    obj.numPhaseDecisionParams(phaseIdx);
                obj.totalnumConstraints = obj.totalnumConstraints + ...
                    obj.phaseList{phaseIdx}.numConstraints;
                
            end
            obj.SetChunkIndeces();
        end
        
        %  Compute all constraints
        function [constraintVec] = GetContraintVector(obj)
            %  Loop over the phases and concatenate the constraint vectors
            constraintVec = [];
            for phaseIdx = 1:obj.numPhases
                constraintVec = [constraintVec ; ...
                    obj.phaseList{phaseIdx}.GetContraintVector()];
            end
            
            %  Now loop over the linkage constraints
            for linkIdx = 1:obj.numLinkages
                constraintVec = [constraintVec; ...
                    obj.linkageList{linkIdx}.GetLinkageConstraints()];
            end
        end
        
        %  Compute the cost function.
        function [costFunction] = GetCostFunction(obj)
            costFunction = 0;
            for phaseIdx = 1:obj.numPhases
                costFunction = costFunction + ...
                    obj.phaseList{phaseIdx}.GetCostFunction();
            end
        end
        
        %  Configure start and stop indeces for different chunks of the
        %  decision vector and constraint vector
        function obj = SetChunkIndeces(obj)
            
            %  Loop over phases and set start and end indeces for different
            %  chunks of the decision vector
            for phaseIdx = 1:obj.numPhases
                
                %  Indeces for start and end of decision vector chunks
                obj.decVecStartIdx(phaseIdx) = ...
                    obj.totalnumDecisionParams - ...
                    sum(obj.numPhaseDecisionParams(phaseIdx:end)) + 1;
                obj.decVecEndIdx(phaseIdx) = ...
                    obj.decVecStartIdx(phaseIdx) ...
                    + obj.numPhaseDecisionParams(phaseIdx) - 1;
                
                %  Function indeces
                obj.conPhaseStartIdx(phaseIdx) = ...
                    sum(obj.numPhaseConstraints(1:phaseIdx-1)) + 1;
                
            end
        end
        
        %  Initializes the sparse matrix for the constraint Jacobian
        function obj = InitializeSparsityPattern(obj)
            obj.sparsityPattern = sparse(obj.totalnumConstraints,...
                obj.totalnumDecisionParams);
        end
        
        %  Set bounds for decision vector and constraints
        function obj = SetBounds(obj)
            obj.SetConstraintBounds();
            obj.SetDecisionVectorBounds();
        end
        
        %  Sets upper and lower bounds on the complete constraint vector
        %  concatenating all phases.
        function obj = SetConstraintBounds(obj)
            
            %  Loop over all phases concatenating constraint vectors
            obj.allConLowerBound = [];
            obj.allConUpperBound = [];
            for phaseIdx = 1:obj.numPhases
                obj.allConLowerBound = [obj.allConLowerBound; ...
                    obj.phaseList{phaseIdx}.allConLowerBound];
                obj.allConUpperBound = [obj.allConUpperBound;...
                    obj.phaseList{phaseIdx}.allConUpperBound];
            end
            
            %  Now loop over the linkage constraints
            for linkIdx = 1:obj.numLinkages
                obj.allConLowerBound = [obj.allConLowerBound; ...
                    obj.linkageList{linkIdx}.lowerBound];
                obj.allConUpperBound = [obj.allConUpperBound;...
                    obj.linkageList{linkIdx}.upperBound];
            end
            
        end
        
        %  Sets upper and lower bounds on the complete decision vector
        %  concatenating all phases.
        function obj = SetDecisionVectorBounds(obj)
            % Loop over all phases
            obj.decisionVecLowerBound = ...
                zeros(obj.totalnumDecisionParams,1);
            obj.decisionVecUpperBound = ...
                zeros(obj.totalnumDecisionParams,1);
            lowIdx = 1;
            for phaseIdx = 1:obj.numPhases
                highIdx = lowIdx + ...
                    obj.numPhaseDecisionParams(phaseIdx)-1;
                obj.decisionVecLowerBound(lowIdx:highIdx) = ...
                    obj.phaseList{phaseIdx}.decisionVecLowerBound;
                obj.decisionVecUpperBound(lowIdx:highIdx) = ...
                    obj.phaseList{phaseIdx}.decisionVecUpperBound;
                lowIdx = highIdx + 1;
            end
        end
        
        %  Handles plotting of user function
        function PlotUserFunction(obj)
            
            %  Plot the function if at solution or at rate
            if ~isempty(obj.plotFunctionName)
                
                % Show the plot if criteria pass
                if (obj.showPlot && obj.isOptimizing && ...
                        obj.plotUpdateCounter == 1) ||...
                        (obj.showPlot && obj.isFinished);
                    feval(obj.plotFunctionName,obj);
                end
                
                %  Update rate counter, and reset if needed
                obj.plotUpdateCounter = obj.plotUpdateCounter + 1;
                if obj.plotUpdateCounter == obj.plotUpdateRate
                    obj.plotUpdateCounter = 1;
                end
            end
            
        end
        
        %  Loop over the phases and concatenate the complete decision
        %  vector
        function obj = SetInitialGuess(obj)
            % Loop over all phases
            obj.decisionVector = zeros(obj.totalnumDecisionParams,1);
            lowIdx = 1;
            for phaseIdx = 1:obj.numPhases
                highIdx=lowIdx+obj.numPhaseDecisionParams(phaseIdx)-1;
                obj.decisionVector(lowIdx:highIdx) = ...
                    obj.phaseList{phaseIdx}.DecVector.decisionVector;
                lowIdx = highIdx + 1;
            end
        end
        
        %  Determine the sparsity patter for the problem
        function obj = SetSparsityPattern(obj)
            
            %  Intialize the sparsity pattern sparse matrix
            obj.InitializeSparsityPattern();
            
            %  Intialize the sparse matrices
            method =  1;
            %  This is a generic algorithm, but it will NOT work in GMAT.
            %  method 1 is based on usin NaN.
            if method == 1
                iGfun = [];
                jGvar = [];
                initialGuess = obj.decisionVector;
                sparseCnt = 0;
                for decIdx = 1:obj.totalnumDecisionParams
                    tempVec         = obj.decisionVector;
                    tempVec(decIdx) = NaN;
                    obj.SetDecisionVector(tempVec);
                    conVec   = obj.GetContraintVector();
                    costVal  = obj.GetCostFunction();
                    totalVec = [costVal;conVec];
                    for funIdx = 1:obj.totalnumConstraints + 1
                        if isnan(totalVec(funIdx))
                            sparseCnt = sparseCnt + 1;
                            iGfun(sparseCnt,1) = [funIdx];
                            jGvar(sparseCnt,1) = [decIdx];
                        end
                    end
                end
                %  Set the initial guess back
                obj.SetDecisionVector(initialGuess);
            elseif method == 2
                iGfun = [];
                jGvar = [];
                initialGuess = obj.decisionVector;
                nomconVec    = obj.GetContraintVector();
                nomcostVal   = obj.GetCostFunction();
                sparseCnt    = 0;
                for decIdx = 1:obj.totalnumDecisionParams
                    tempVec         = obj.decisionVector;
                    % Evaluate at upper bounds
                    tempVec(decIdx) = obj.decisionVecUpperBound(decIdx);
                    obj.SetDecisionVector(tempVec);
                    highconVec   = obj.GetContraintVector();
                    highcostVal  = obj.GetCostFunction();
                    %  Evaluate at lower bounds
                    tempVec(decIdx) = obj.decisionVecLowerBound(decIdx);
                    obj.SetDecisionVector(tempVec);
                    lowconVec   = obj.GetContraintVector();
                    lowcostVal  = obj.GetCostFunction();
                    totalVec = [nomcostVal;nomconVec];
                    lowtotalVec = [highcostVal;lowconVec];
                    hightotalVec = [highcostVal;lowconVec];
                    for funIdx = 1:obj.totalnumConstraints + 1
                        if (lowtotalVec(funIdx) ~= totalVec(funIdx)) || ...
                                (hightotalVec(funIdx)~= totalVec(funIdx))
                            sparseCnt = sparseCnt + 1;
                            iGfun(sparseCnt,1) = [funIdx];
                            jGvar(sparseCnt,1) = [decIdx];
                        end
                    end
                end
                %  Set the initial guess back
                obj.SetDecisionVector(initialGuess);
                
            end
            obj.sparsityPattern = sparse(iGfun,jGvar,1);
            
        end
    end
end

