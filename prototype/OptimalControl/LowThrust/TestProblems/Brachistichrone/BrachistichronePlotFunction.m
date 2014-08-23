function OrbitRaisingPlotFunction(traj)

stateArray = traj.phaseList{1}.DecVector.GetStateArray();
stateArray = stateArray.x;
controlArray = traj.phaseList{1}.DecVector.GetControlArray(); 
controlArray = controlArray.x;

x  = stateArray(:,1);
y  = stateArray(:,2);
plot(x,y); 
axis([0 2 -2 0]); grid on;
drawnow;
%pause
return

