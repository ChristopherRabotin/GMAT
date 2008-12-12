function RunEstimator_Exectute(Estimator,Propagator)

global ObjectStore

%==========================================================================
%==========================================================================
%---- Perform the last bits of initialization
%==========================================================================
%==========================================================================
Estimator  = RunEstimator_PreparetoSolve(Estimator);
Propagator = RunEstimator_PreparetoPropagate(Propagator,Estimator);

%==========================================================================
%==========================================================================
%---- Run the State Machine
%==========================================================================
%==========================================================================
EpochVec   = Propagator.Epoch + [0:300:3600]/86400;
for i = 1:size(EpochVec,2)

    %  Take a step
    NewEpoch   = EpochVec(i); 
    Propagator = Propagator_SteptoEpoch(Propagator,NewEpoch);
    
    %  Call the measurement models
    %  for i = 1:
    
end

ObjectStore.Objects{1}
ObjectStore.Objects{1}.STM
