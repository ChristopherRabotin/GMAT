function HyperSensitivePlotFunctionMultiPhase(traj)

figure(8);clf;
for phaseIdx = 1:traj.numPhases
    stateArray = traj.phaseList{phaseIdx}.DecVector.GetStateArray();
    if isa(stateArray,'gradient')
       stateArray = stateArray.x;
    end
    x    = stateArray(:,1);
    time = traj.phaseList{phaseIdx}.timeVector;
    plot(time,x); hold on;
    plot(time(1),x(1),'go','MarkerSize',9,'LineWidth',2)
    plot(time(end),x(end),'rx','MarkerSize',7,'LineWidth',2)
end

drawnow;
%pause
