function HyperSensitivePlotFunction(traj)

stateArray = traj.phaseList{1}.DecVector.GetStateArray();
stateArray = stateArray.x;

x  = stateArray(:,1);
time =  traj.phaseList{1}.timeVector;
figure(8)
plot(time,x); grid on;
drawnow;
%pause
return

