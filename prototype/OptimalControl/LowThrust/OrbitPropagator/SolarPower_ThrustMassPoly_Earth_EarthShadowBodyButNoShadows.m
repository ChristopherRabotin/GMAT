

%%  Define spacecraft orbit properties
Sat          = Spacecraft();
Sat.Epoch    = 21548.0299479207;
Sat.DryMass  = 850;
Sat.Attitude = 1;  % 1 for J2000, 2 for VNB
Sat.OrbitState = [-6159.48026910191
    -1086.08255858410
   -6471.533790730538
    4.712777171391656
    0.8309897693832682
   -4.624758002168765];

%%  Define the spacecraft power configuration
Sat.PowerModel         = 'Solar';  % Options: Solar, Nuclear.
Sat.InitialMaxPower    = 1.2124;  %  kW
Sat.PowerInitialEpoch  = 21547.00000039794;
Sat.PowerDecayRate     = 5.123 ;  % Percent per year decay rate
Sat.PowerMargin        = 4.998;
Sat.ModelShadows       = 1;
Sat.BusCoeff1          = 0.32;
Sat.BusCoeff2          = 0.0001;
Sat.BusCoeff3          = 0.0001;
Sat.SolarCoeff1        =  1.33;
Sat.SolarCoeff2        = -0.11;
Sat.SolarCoeff3        = -0.12;
Sat.SolarCoeff4        =  0.11;
Sat.SolarCoeff5        = -0.02;
Sat.debugMath          = false();

%%  Configure the Thrusters
aThruster                    = ElectricThruster();
aThruster.debugMath          = false();
%  ConstantThrustAndIsp, FixedEfficiency, ThrustMassPolynomial
aThruster.ThrustModel        = 'ThrustMassPolynomial';
aThruster.MaximumUsablePower = 7.4;
aThruster.MinimumUsablePower = 0.31;
aThruster.ThrustCoeff1       = 1.92E-06 ;
aThruster.ThrustCoeff2       =  54.05382 ;
aThruster.ThrustCoeff3       = -14.41789;
aThruster.ThrustCoeff4       =  2.96519 ;
aThruster.ThrustCoeff5       = -0.19082; %  milli - Newton
aThruster.MassFlowCoeff1     = 2.13781  ;
aThruster.MassFlowCoeff2     = 0.03211  ;
aThruster.MassFlowCoeff3     =  -0.09956;
aThruster.MassFlowCoeff4     = 0.05717 ;
aThruster.MassFlowCoeff5     = -0.004776; %  milligrams/sec
aThruster.FixedEfficiency    = 0.654321;
aThruster.GravitationalAccel = 9.82;
aThruster.Isp                = 3219.12314;
aThruster.ConstantThrust     = 0.01243;
aThruster.ThrustCoordSys     = 2;
aThruster.ThrustDirection1   = 1;
aThruster.ThrustDirection2   = 0;
aThruster.ThrustDirection3   = 0;
aThruster.DutyCycle          = .98;
aThruster.ThrustScaleFactor  = .87;

%% Configure the finite burn object and spacecraft
aFiniteBurn              = FiniteBurn();
aFiniteBurn.debugMath    = false();
aTank                    = FuelTank();
aTank.FuelMass           = 567.89;
aFiniteBurn.SetThrusters({aThruster});
Sat.SetThrusterSet(aFiniteBurn);
Sat.Tanks                = {aTank};

%%  Configure the force model
Force.UseSRP         = 0;
Force.UseRelativity  = 0;
Force.STM            = 0;
Force.Drag           = 'None';
Force.SRPModel       = 0;  %  1 for sphere, 2 for SPAD
Force.UseFiniteBurn  = 1;
Force.srpScaleFactor = 1;
Force.Debug          = 0;
Force.bodyMu         = 398600.4415;
Force.RefEpoch       = Sat.Epoch;
Force.debugMath      = false();