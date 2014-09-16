%% =====  Intializations
ClearAll
global igrid iGfun jGvar traj 

%% =====  Define the phase and trajectory properties

%  Create trajectory and its settings
traj                           = Trajectory();
traj.pathFunctionName          = 'HyperSensitivePathFunction';   
traj.plotFunctionName          = 'HyperSensitivePlotFunction'; 
traj.showPlot                  = true();
traj.costLowerBound            = 0;
traj.costUpperBound            = Inf;
traj.plotUpdateRate            = 3;

%  Set Time Properties
phase1                         = RadauPhase;
phase1.meshIntervalFractions   = linspace(-1,1,40+1).';
phase1.meshIntervalNumPoints   = 4*ones(40,1);
phase1.meshIntervalFractions   = [ -1 -0.5 0 0.5  1];                
phase1.meshIntervalNumPoints   = [20 10 10 20]';
phase1.initialEpoch            = 5;
phase1.finalEpoch              = 45;
phase1.initialTimeLowerBound   = 0;
phase1.initialTimeUpperBound   = 0;
phase1.finalTimeLowerBound     = 50;
phase1.finalTimeUpperBound     = 50;

%  Set state and control properties
phase1.numStates               = 1;
phase1.initialGuessMode        = 'LinearNoControl';
phase1.initialGuessState       = .75;
phase1.finalGuessState         = .33;
phase1.initialStateLowerBound  = 1;
phase1.initialStateUppperBound = 1;
phase1.finalStateLowerBound    = 1;
phase1.finalStateUpperBound    = 1;
phase1.stateLowerBound         = -50;
phase1.stateUpperBound         = 50;
phase1.numControls             = 1;
phase1.controlUpperBound       = 50;
phase1.controlLowerBound       = -50;

%% =====  Initialize the trajectory and phases and get ready for run

% Determine the sparsity pattern for the combined 
% cost function and constraint Jacobian
traj.phaseList = {phase1};

[z,F,xmul,Fmul] = traj.Optimize();
return

z = traj.decisionVector.x;
          
load('origSolution.mat')
if length(z) == length(zTruth)
   norm(z - zTruth)
end

%% =====  Plot/analyze the results
% Extract the state and control from the decision vector z.
% Remember that the state is approximated at the LGR points
% plus the final point, while the control is only approximated 
% at only the LGR points.
NLGR = traj.phaseList{1}.numRadauPoints;
tau = traj.phaseList{1}.radauPoints;
nstates = traj.phaseList{1}.numStates;
ncontrols = traj.phaseList{1}.numControls;
w   = traj.phaseList{1}.radauWeights;
D   = traj.phaseList{1}.radauDiffMatrix;

x = z(1:NLGR+1);
u = z((NLGR+1)+1:(NLGR+1)+NLGR);
t0 = z(end-1);
tf = z(end);
t = (tf-t0)*(tau+1)/2+t0;
tLGR = t(1:end-1);
%-----------------------------------------------------------------%
% Extract the Lagrange multipliers corresponding                  %
% the defect constraints.                                         %
%-----------------------------------------------------------------%
multipliersDefects = Fmul(2:nstates*NLGR+1);
multipliersDefects = reshape(multipliersDefects,NLGR,nstates);
%-----------------------------------------------------------------%
% Compute the costates at the LGR points via transformation       %
%-----------------------------------------------------------------%
costateLGR = inv(diag(w))*multipliersDefects;
%-----------------------------------------------------------------%
% Compute the costate at the tau=+1 via transformation            %
%-----------------------------------------------------------------%
costateF = D(:,end).'*multipliersDefects;                                                                                                   
%-----------------------------------------------------------------%                                                                         
% Now assemble the costates into a single matrix                  %                                                                         
%-----------------------------------------------------------------%
costate = [costateLGR; costateF];    
lamx    = costate;

figure(1);
subplot(2,2,1);
plot(x,lamx,'-o'); hold on;
xlabel('x');
ylabel('\lambda_x');
set(gca,'FontName','Times','FontSize',18);
grid on;

subplot(2,2,2);
plot(t,x,'-o'); hold on;
xlabel('time');
ylabel('x(t)');
set(gca,'FontName','Times','FontSize',18);
grid on;

subplot(2,2,3);
plot(t,lamx,'-o'); hold on;
xlabel('time');
ylabel('\lambda_x(t)');
set(gca,'FontName','Times','FontSize',18);
grid on;

subplot(2,2,4);
plot(tLGR,u,'-o'); hold on; grid on;
xlabel('t');
ylabel('u(t)');
set(gca,'FontName','Times','FontSize',18);


