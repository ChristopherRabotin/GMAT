function HyperSensitivePlotFunction(traj)


figure(8);clf;
lastTime = 0;
for phaseIdx = 1:traj.numPhases
    stateArray = traj.phaseList{phaseIdx}.DecVector.GetStateArray();
    if isa(stateArray,'gradient')
       stateArray = stateArray.x;
    end
    x    = [stateArray(:,1)];
    time = traj.phaseList{phaseIdx}.timeVector;
    plot(time,x); hold on;
end

drawnow;
%pause
