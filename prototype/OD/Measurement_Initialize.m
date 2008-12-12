function Measurement = Measurement_Initialize(Measurement)

global ObjectStore

%==========================================================================
%==========================================================================
%---- Assign pointers for participants in each data type associated with
%     the measurement
%==========================================================================
%==========================================================================

for i = 1:size(Measurement.Measurements,1)
    
    %  Extact the ith data type
    currMeasurement = Measurement.Measurements{i};
    
    %  Determine measurement type and assign numeric Id
    if strcmp(currMeasurement{1},'Range')
        Measurement.Types(i) = 1;
    elseif strcmp(currMeasurement{1},'RangeRate')
        Measurement.Types(i) = 2;
    end
    
    %  Set pointers to participants.  These are locations of objects in
    %  ObjectStore.
    %  THIS IS A HARD CODED KLUDGE
    Measurement.Particpants{1} = [1 2];
    Measurement.Particpants{2} = [1 2];
       
end
