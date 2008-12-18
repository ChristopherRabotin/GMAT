function [g,dgdx] = GSMeasurement_Evaluate(Measurement)

global ObjectStore


%==========================================================================
%==========================================================================
%---- Loop over the data types 
%==========================================================================
%==========================================================================

for i = 1:size(Measurement.Types,2)
   
    % Calculate the measurement and the partial
    if Measurement.Types(i) == 1
        
        Participant1 = ObjectStore.Objects{Measurement.Participants{i}(1)};
        Participant2 = ObjectStore.Objects{Measurement.Participants{i}(2)};
        [g,dgdx] = GroundStation_Range(Participant1,Participant2);
        
    end
    
end
