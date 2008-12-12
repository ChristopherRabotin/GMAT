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
EpochVec   = Propagator.Epoch + [0:300:86400]/86400;
for i = 1:size(EpochVec,2)

    %  Step to next measurement epoch
    NewEpoch   = EpochVec(i); 
    Propagator = Propagator_SteptoEpoch(Propagator,NewEpoch);
    
    %  Call the measurement model
    [g,dgdx] = GSMeasurement_Evaluate(ObjectStore.Objects{3});
    Y(i,1) = g;
    Htilde(i,:) = dgdx;
    
end

ObjectStore.Objects{1}
ObjectStore.Objects{1}.STM

keyboard
