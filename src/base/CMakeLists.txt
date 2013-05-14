# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT base library
# This file must be installed in the src/base directory
#
# Author: Joris T. Olympio
# Modified For Production By: Josh Fisher
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT base setup " ${VERSION}) 

if(WIN32)
	SET(BASETARGET  "libGmatBase")
else()
	SET(BASETARGET "GmatBase")
endif()

SET(TargetName ${BASETARGET})

# Add Preproccessor Definitions
ADD_DEFINITIONS("-D_DYNAMICLINK")
ADD_DEFINITIONS("-DGMAT_EXPORTS")
ADD_DEFINITIONS("-DEXP_STL")

# ====================================================================
# specific compilation flags
if (UNIX)
	MESSAGE("-- Using a shared base library")
	IF(APPLE) # mac
		SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -dylib -dynamiclib -two_levelnamespace -undefined dynamic_lookup")

		# snow leopard
		#SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -arch i386")
	endif()
endif (UNIX)


# ====================================================================
# SPICE specifics
if(64_BIT)
	SET(CSPICE_DIR "${CSPICE_DIR}/cspice64")
else()
	SET(CSPICE_DIR "${CSPICE_DIR}/cspice32")
endif()
	
INCLUDE_DIRECTORIES("${CSPICE_DIR}/include")
LINK_DIRECTORIES("${CSPICE_DIR}/lib")

if(WIN32)
	LINK_LIBRARIES("${CSPICE_DIR}/lib/cspice.lib")
else()
	LINK_LIBRARIES("${CSPICE_DIR}/lib/cspice.a")
endif()
	
ADD_DEFINITIONS("-D__USE_SPICE__")

SET(SPICE_SRCS
	 "attitude/SpiceAttitude.cpp"
	 "util/SpiceAttitudeKernelReader.cpp"
	 "util/SpiceInterface.cpp"
	 "util/SpiceOrbitKernelReader.cpp"
	 "util/SpiceKernelReader.cpp"
	 "util/SpiceKernelWriter.cpp"
	 "util/SpiceOrbitKernelWriter.cpp")

# Unix type systems need a change in the stack size
# Unix type systems include MAC, LINUX
MESSAGE("-- Executing... ulimit -s 61440")
execute_process(COMMAND ulimit -s 61440)

# ====================================================================
# F2C
MESSAGE("-- Use F2C version of MSISE.")
SET(MSISE_SRCS "solarsys/msise90_sub.c")


# ====================================================================
# include directories for header files
INCLUDE_DIRECTORIES("./include")
INCLUDE_DIRECTORIES("./foundation")
INCLUDE_DIRECTORIES("./hardware")
INCLUDE_DIRECTORIES("./executive")
INCLUDE_DIRECTORIES("./attitude")
INCLUDE_DIRECTORIES("./spacecraft")
INCLUDE_DIRECTORIES("./asset")
INCLUDE_DIRECTORIES("./forcemodel")
INCLUDE_DIRECTORIES("./forcemodel/harmonic")
INCLUDE_DIRECTORIES("./propagator")
INCLUDE_DIRECTORIES("./subscriber")
INCLUDE_DIRECTORIES("./command")
INCLUDE_DIRECTORIES("./solarsys")
INCLUDE_DIRECTORIES("./factory")
INCLUDE_DIRECTORIES("./stopcond")
INCLUDE_DIRECTORIES("./interpreter")
INCLUDE_DIRECTORIES("./util")
INCLUDE_DIRECTORIES("./interpolator")
INCLUDE_DIRECTORIES("./interface")
INCLUDE_DIRECTORIES("./parameter")
INCLUDE_DIRECTORIES("./configs")
INCLUDE_DIRECTORIES("./burn")
INCLUDE_DIRECTORIES("./solver")
INCLUDE_DIRECTORIES("./function")
INCLUDE_DIRECTORIES("./coordsystem")
INCLUDE_DIRECTORIES("./math")
INCLUDE_DIRECTORIES("./plugin")
INCLUDE_DIRECTORIES("./event")

# ====================================================================
# source files

# define the source files
SET(BASE_SRCS
    asset/AssetException.cpp
    asset/BodyFixedPoint.cpp
    asset/GroundstationInterface.cpp
    attitude/Attitude.cpp
    attitude/AttitudeException.cpp
    attitude/CSFixed.cpp
    attitude/Kinematic.cpp
    attitude/Spinner.cpp
    burn/BurnException.cpp
    burn/Burn.cpp
    burn/FiniteBurn.cpp
    burn/ImpulsiveBurn.cpp
    burn/InertialManeuverFrame.cpp
    burn/ManeuverFrameManager.cpp
    burn/ManeuverFrame.cpp
    burn/VnbManeuverFrame.cpp
    command/Achieve.cpp
    command/Assignment.cpp
    command/BeginFiniteBurn.cpp
    command/BeginFunction.cpp
    command/BeginMissionSequence.cpp
    command/BeginScript.cpp
    command/BranchCommand.cpp
    command/CallFunction.cpp
    command/CommandException.cpp
    command/GmatCommand.cpp
    command/CommandUtil.cpp
    command/ClearPlot.cpp
    command/ConditionalBranch.cpp
    command/Create.cpp
    command/Else.cpp
    command/EndFiniteBurn.cpp
    command/EndFor.cpp
    command/EndFunction.cpp
    command/EndIf.cpp
    command/EndOptimize.cpp
    command/EndScript.cpp
    command/EndTarget.cpp
    command/EndWhile.cpp
    command/For.cpp
    command/If.cpp
    command/ManageObject.cpp
    command/Maneuver.cpp
    command/MarkPoint.cpp
    command/Minimize.cpp
    command/NonlinearConstraint.cpp
    command/NoOp.cpp
    command/Optimize.cpp
    command/PenDown.cpp
    command/PenUp.cpp
    command/Propagate.cpp
    command/PropagationEnabledCommand.cpp
	command/PlotCommand.cpp
	command/SolverSequenceCommand.cpp
    command/Report.cpp
    command/RunSolver.cpp
    command/SaveMission.cpp
    command/SolverBranchCommand.cpp
    command/Stop.cpp
    command/Target.cpp
    command/Toggle.cpp
    command/Vary.cpp
    command/While.cpp
    configs/ConfigManagerException.cpp
    configs/ConfigManager.cpp
    coordsystem/AxisSystem.cpp
    coordsystem/BodyFixedAxes.cpp
    coordsystem/BodyInertialAxes.cpp
    coordsystem/CoordinateBase.cpp
    coordsystem/CoordinateConverter.cpp
    coordsystem/CoordinateSystemException.cpp
    coordsystem/CoordinateSystem.cpp
    coordsystem/DynamicAxes.cpp
    coordsystem/EquatorAxes.cpp
    coordsystem/GeocentricSolarEclipticAxes.cpp
    coordsystem/GeocentricSolarMagneticAxes.cpp
    coordsystem/InertialAxes.cpp
    coordsystem/ItrfCoefficientsFile.cpp
    coordsystem/LocalAlignedConstrainedAxes.cpp
    coordsystem/MeanOfDateAxes.cpp
    coordsystem/MJ2000EcAxes.cpp
    coordsystem/MJ2000EqAxes.cpp
    coordsystem/MODEcAxes.cpp
    coordsystem/MODEqAxes.cpp
    coordsystem/MOEEcAxes.cpp
    coordsystem/MOEEqAxes.cpp
    coordsystem/ObjectReferencedAxes.cpp
    coordsystem/TODEcAxes.cpp
    coordsystem/TODEqAxes.cpp
    coordsystem/TOEEcAxes.cpp
    coordsystem/TOEEqAxes.cpp
    coordsystem/TopocentricAxes.cpp
    coordsystem/TrueOfDateAxes.cpp
	coordsystem/ICRFAxes.cpp
	coordsystem/ICRFFile.cpp
	coordsystem/ITRFAxes.cpp
	coordsystem/IAUFile.cpp
	coordsystem/BodySpinSunAxes.cpp
	event/Brent.cpp
	event/EventException.cpp
	event/EventFunction.cpp
	event/EventLocator.cpp
	event/LocatedEvent.cpp
	event/LocatedEventTable.cpp
	event/RootFinder.cpp
    executive/Moderator.cpp
    executive/PlotInterface.cpp
    executive/PlotReceiver.cpp
    executive/PublisherException.cpp
    executive/Publisher.cpp
    executive/SandboxException.cpp
    executive/Sandbox.cpp
    factory/AtmosphereFactory.cpp
    factory/AttitudeFactory.cpp
    factory/AxisSystemFactory.cpp
    factory/BurnFactory.cpp
    factory/CalculatedPointFactory.cpp
    factory/CelestialBodyFactory.cpp
    factory/CommandFactory.cpp
    factory/CoordinateSystemFactory.cpp
    factory/FactoryException.cpp
    factory/FactoryManager.cpp
    factory/Factory.cpp
    factory/ODEModelFactory.cpp
    factory/HardwareFactory.cpp
    factory/InterfaceFactory.cpp
    factory/MathFactory.cpp
    factory/ParameterFactory.cpp
    factory/PhysicalModelFactory.cpp
    factory/PropagatorFactory.cpp
    factory/PropSetupFactory.cpp
    factory/SolarSystemFactory.cpp
    factory/SolverFactory.cpp
    factory/SpacecraftFactory.cpp
    factory/StopConditionFactory.cpp
    factory/SubscriberFactory.cpp
    forcemodel/DragForce.cpp
    forcemodel/FiniteThrust.cpp
    forcemodel/ODEModelException.cpp
    forcemodel/ODEModel.cpp
    forcemodel/GravityField.cpp
	forcemodel/GravityBase.cpp
    forcemodel/HarmonicField.cpp
    forcemodel/PhysicalModel.cpp
    forcemodel/PointMassForce.cpp
    forcemodel/SolarRadiationPressure.cpp
	forcemodel/EventModel.cpp
	forcemodel/RelativisticCorrection.cpp
	forcemodel/harmonic/Harmonic.cpp
	forcemodel/harmonic/HarmonicGravity.cpp
	forcemodel/harmonic/HarmonicGravityCof.cpp
	forcemodel/harmonic/HarmonicGravityGrv.cpp
    foundation/Covariance.cpp
    foundation/ElementWrapper.cpp
    foundation/GmatBaseException.cpp
    foundation/GmatBase.cpp
    foundation/GmatState.cpp
    foundation/ObjectInitializer.cpp
    foundation/SpacePoint.cpp
    foundation/StateManager.cpp
    foundation/TriggerManager.cpp
    function/Function.cpp
    function/FunctionException.cpp
    function/FunctionManager.cpp
    hardware/FuelTank.cpp
    hardware/HardwareException.cpp
    hardware/Hardware.cpp
    hardware/Thruster.cpp
    interface/InterfaceException.cpp
    interface/Interface.cpp
    interface/GmatInterface.cpp
    interpolator/InterpolatorException.cpp
    interpolator/Interpolator.cpp
    interpolator/BrentDekkerZero.cpp
    interpolator/CubicSplineInterpolator.cpp
    interpolator/LinearInterpolator.cpp
    interpolator/NotAKnotInterpolator.cpp
    interpolator/LagrangeInterpolator.cpp
    interpreter/InterpreterException.cpp
    interpreter/Interpreter.cpp
    interpreter/MathParser.cpp
    interpreter/MathTree.cpp
    interpreter/ScriptInterpreter.cpp
    interpreter/ScriptReadWriter.cpp
    interpreter/Validator.cpp
    math/Abs.cpp
    math/Acos.cpp
    math/Acosh.cpp
    math/Add.cpp
    math/Asin.cpp
    math/Asinh.cpp
    math/Atan.cpp
    math/Atan2.cpp
    math/Cos.cpp
    math/Cosh.cpp
    math/DegToRad.cpp
    math/Determinant.cpp
    math/Divide.cpp
    math/Exp.cpp
    math/FunctionRunner.cpp
    math/Log.cpp
    math/Log10.cpp
    math/Inverse.cpp
    math/MathElement.cpp
    math/MathException.cpp
    math/MathFunction.cpp
    math/MathNode.cpp
    math/Multiply.cpp
    math/Negate.cpp
    math/Norm.cpp
    math/Power.cpp
    math/RadToDeg.cpp
    math/Sin.cpp
    math/Sinh.cpp
    math/Sqrt.cpp
    math/Subtract.cpp
    math/Tan.cpp
    math/Tanh.cpp
    math/Transpose.cpp
    parameter/AngularParameters.cpp
    parameter/Array.cpp
    parameter/ArrayWrapper.cpp
    parameter/ArrayElementWrapper.cpp
    parameter/AttitudeData.cpp
    parameter/AttitudeParameters.cpp
    parameter/AttitudeReal.cpp
    parameter/BallisticMassParameters.cpp
    parameter/BallisticMassReal.cpp
    parameter/BooleanWrapper.cpp
    parameter/BplaneData.cpp
    parameter/BplaneParameters.cpp
    parameter/BplaneReal.cpp
    parameter/BurnData.cpp
    parameter/BurnParameters.cpp
    parameter/BurnReal.cpp
    parameter/CartesianParameters.cpp
    parameter/EnvData.cpp
    parameter/EnvParameters.cpp
    parameter/EnvReal.cpp
    parameter/EquinoctialParameters.cpp
    parameter/ExpressionParser.cpp
    parameter/HardwareReal.cpp
    parameter/HardwareParameters.cpp
    parameter/KeplerianParameters.cpp
    parameter/NumberWrapper.cpp
    parameter/ObjectWrapper.cpp
    parameter/ObjectPropertyWrapper.cpp
    parameter/OnOffWrapper.cpp
    parameter/OrbitalParameters.cpp
    parameter/OrbitData.cpp
    parameter/OrbitReal.cpp
    parameter/OrbitRmat33.cpp
    parameter/OrbitRmat66.cpp
    parameter/OrbitRvec6.cpp
    parameter/OrbitStmParameters.cpp
    parameter/ParameterDatabase.cpp
    parameter/ParameterInfo.cpp
    parameter/Parameter.cpp
    parameter/ParameterWrapper.cpp
    parameter/PlanetData.cpp
    parameter/PlanetParameters.cpp
    parameter/PlanetReal.cpp
    parameter/RealVar.cpp
    parameter/RefData.cpp
    parameter/Rmat33Var.cpp
    parameter/Rmat66Var.cpp
    parameter/Rvec6Var.cpp
    parameter/SpacecraftData.cpp
    parameter/SphericalParameters.cpp
    parameter/StringWrapper.cpp
    parameter/StringObjectWrapper.cpp
    parameter/StringVar.cpp
    parameter/TimeData.cpp
    parameter/TimeParameters.cpp
    parameter/TimeReal.cpp
    parameter/TimeString.cpp
    parameter/Variable.cpp
    parameter/VariableWrapper.cpp
	parameter/AttitudeString.cpp
    plugin/DynamicLibrary.cpp
    propagator/AdamsBashforthMoulton.cpp
    propagator/DormandElMikkawyPrince68.cpp
    propagator/Integrator.cpp
    propagator/PredictorCorrector.cpp
    propagator/PrinceDormand45.cpp
    propagator/PrinceDormand78.cpp
    propagator/PropagationStateManager.cpp
    propagator/Propagator.cpp
    propagator/PropSetup.cpp
    propagator/RungeKutta89.cpp
    propagator/RungeKuttaFehlberg56.cpp
    propagator/RungeKuttaNystrom.cpp
    propagator/RungeKutta.cpp
    solarsys/Asteroid.cpp
    solarsys/AtmosphereModel.cpp
    solarsys/Barycenter.cpp
    solarsys/CalculatedPoint.cpp
    solarsys/CelestialBody.cpp
    solarsys/Comet.cpp
    solarsys/DeFile.cpp
    solarsys/ExponentialAtmosphere.cpp
    solarsys/JacchiaRobertsAtmosphere.cpp
    solarsys/LibrationPoint.cpp
    solarsys/Moon.cpp
    solarsys/Msise90Atmosphere.cpp
    solarsys/PlanetaryEphemException.cpp
    solarsys/PlanetaryEphem.cpp
    solarsys/Planet.cpp
    solarsys/SlpFile.cpp
    solarsys/SolarFluxFileReader.cpp
    solarsys/SolarSystemException.cpp
    solarsys/SolarSystem.cpp
    solarsys/Star.cpp
    solver/Solver.cpp
    solver/DifferentialCorrector.cpp
    solver/Optimizer.cpp
    solver/InternalOptimizer.cpp
    solver/ExternalOptimizer.cpp
    solver/SteepestDescent.cpp
    solver/DerivativeModel.cpp
    solver/Gradient.cpp
    solver/Jacobian.cpp
    solver/LineSearch.cpp
    spacecraft/FormationInterface.cpp
    spacecraft/Spacecraft.cpp
    spacecraft/SpaceObject.cpp
    spacecraft/TextTrajectoryFile.cpp
    stopcond/StopCondition.cpp
    subscriber/EphemerisFile.cpp
    subscriber/MessageWindow.cpp
    subscriber/TextEphemFile.cpp
    subscriber/OrbitView.cpp
    subscriber/OwnedPlot.cpp
    subscriber/ReportFile.cpp
    subscriber/Subscriber.cpp
    subscriber/XyPlot.cpp
	subscriber/OrbitPlot.cpp
	subscriber/GroundTrackPlot.cpp
    util/A1Date.cpp
    util/A1Mjd.cpp
    util/AngleUtil.cpp
    util/AttitudeUtil.cpp
    util/BaseException.cpp
    util/BodyFixedStateConverter.cpp
	util/CalculationUtilities.cpp
	util/Code500EphemerisFile.cpp
    util/Date.cpp
    util/DateUtil.cpp
    util/ElapsedTime.cpp
    util/EopFile.cpp
    util/FileManager.cpp
    util/FileUtil.cpp
    util/GmatGlobal.cpp
    util/GregorianDate.cpp
    util/LatLonHgt.cpp
    util/LeapSecsFileReader.cpp
    util/Linear.cpp
    util/MemoryTracker.cpp
    util/MessageInterface.cpp
    util/MessageReceiver.cpp
    util/RealUtilities.cpp
    util/RgbColor.cpp
    util/Rmatrix33.cpp
    util/Rmatrix66.cpp
    util/Rmatrix.cpp
    util/Rvector3.cpp
    util/Rvector6.cpp
    util/Rvector.cpp
    util/StringTokenizer.cpp
    util/StringUtil.cpp
    util/TextParser.cpp
    util/TimeSystemConverter.cpp
    util/TimeTypes.cpp
    util/UtcDate.cpp
	util/StateConversionUtil.cpp
	util/GravityFileUtil.cpp
	util/Frozen.cpp
	util/OrbitDesignerTime.cpp
	util/SunSync.cpp
	util/RepeatSunSync.cpp
	util/RepeatGroundTrack.cpp
    solarsys/SimpleExponentialAtmosphere.cpp
	solarsys/SpecialCelestialPoint.cpp
    ${MSISE_SRCS}
    ${SPICE_SRCS}
)


# ====================================================================
# add the install targets
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../application/bin")

ADD_LIBRARY(${TargetName} SHARED ${BASE_SRCS})

# specify the version of the library. 
if(WIN32)
	SET_TARGET_PROPERTIES(${TargetName} PROPERTIES VERSION ${GMAT_VERSION} LINK_FLAGS "/NODEFAULTLIB:\"libcmt.lib\"")
else()
	SET_TARGET_PROPERTIES(${TargetName} PROPERTIES VERSION ${GMAT_VERSION} )
endif()
