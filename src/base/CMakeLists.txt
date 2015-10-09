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

SET(TargetName GmatBase)

# Add preprocessor definitions
ADD_DEFINITIONS("-D_DYNAMICLINK")
ADD_DEFINITIONS("-DGMAT_EXPORTS")
ADD_DEFINITIONS("-DEXP_STL")

# ====================================================================
# SPICE specifics
# SPICE is linked because it contains the F2C implementation
LINK_LIBRARIES(optimized ${CSPICE_LIB} debug ${CSPICE_LIB_DEBUG})

if(GMAT_USE_SPICE)
  SET(SPICE_SRCS
    "attitude/SpiceAttitude.cpp"
    "util/SpiceAttitudeKernelReader.cpp"
    "util/SpiceInterface.cpp"
    "util/SpiceOrbitKernelReader.cpp"
    "util/SpiceKernelReader.cpp"
    "util/SpiceKernelWriter.cpp"
    "util/SpiceOrbitKernelWriter.cpp")
endif()

# ====================================================================
# F2C
MESSAGE("-- Use F2C version of MSISE.")
SET(MSISE_SRCS "solarsys/msise90_sub.c")

# ====================================================================
# list of directories containing source/header files
SET(BASE_DIRS
  asset
  attitude
  burn
  command
  configs
  coordsystem
  event
  executive
  factory
  forcemodel
  forcemodel/harmonic
  foundation
  function
  hardware
  include
  interface
  interpolator
  interpreter
  math
  parameter
  plugin
  propagator
  solarsys
  solver
  spacecraft
  stopcond
  subscriber
  util
  )

# ====================================================================
# Globally available list of GMAT Base include directories
# Use absolute path so other GMAT dependencies can refer to them
FOREACH(currdir ${BASE_DIRS})
  SET(GMAT_BASE_INCLUDE_DIRECTORIES
      ${GMAT_BASE_INCLUDE_DIRECTORIES}
	  "${CMAKE_CURRENT_SOURCE_DIR}/${currdir}"
	  CACHE INTERNAL "base includes"
	  )
ENDFOREACH(currdir)

# ====================================================================
# Include directories for header files
INCLUDE_DIRECTORIES(${GMAT_BASE_INCLUDE_DIRECTORIES})

# ====================================================================
# source files
SET(BASE_SRCS
    asset/AssetException.cpp
    asset/BodyFixedPoint.cpp
    asset/GroundstationInterface.cpp
    attitude/Attitude.cpp
    attitude/AttitudeException.cpp
    attitude/CCSDSAttitude.cpp
    attitude/CSFixed.cpp
    attitude/Kinematic.cpp
    attitude/NadirPointing.cpp
    attitude/PrecessingSpinner.cpp
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
    command/FindEvents.cpp
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
    event/EventException.cpp
    event/EventLocator.cpp
    event/LocatedEvent.cpp
    executive/ListenerManager.cpp
    executive/ListenerManagerInterface.cpp
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
    #forcemodel/EventModel.cpp
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
    foundation/IChangeListener.cpp
    foundation/ObjectInitializer.cpp
    foundation/SpacePoint.cpp
    foundation/StateManager.cpp
    foundation/TriggerManager.cpp
    function/Function.cpp
    function/FunctionException.cpp
    function/FunctionManager.cpp
    hardware/ChemicalTank.cpp
    hardware/ChemicalThruster.cpp
    hardware/ElectricTank.cpp
    hardware/ElectricThruster.cpp
    hardware/FuelTank.cpp
    hardware/HardwareException.cpp
    hardware/Hardware.cpp
    hardware/NuclearPowerSystem.cpp
    hardware/PowerSystem.cpp
    hardware/SolarPowerSystem.cpp
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
    parameter/AlternateEquinoctialParameters.cpp
    parameter/AngularParameters.cpp
    parameter/Array.cpp
    parameter/ArrayWrapper.cpp
    parameter/ArrayElementWrapper.cpp
    parameter/AttitudeData.cpp
    parameter/AttitudeParameters.cpp
    parameter/AttitudeReal.cpp
    parameter/AttitudeRvector.cpp
    parameter/BallisticMassParameters.cpp
    parameter/BallisticMassReal.cpp
    parameter/BooleanWrapper.cpp
    parameter/BplaneData.cpp
    parameter/BplaneParameters.cpp
    parameter/BplaneReal.cpp
    parameter/BrouwerMeanLongParameters.cpp
    parameter/BrouwerMeanShortParameters.cpp
    parameter/BurnData.cpp
    parameter/BurnParameters.cpp
    parameter/BurnReal.cpp
    parameter/CartesianParameters.cpp
    parameter/DelaunayParameters.cpp
    parameter/EnvData.cpp
    parameter/EnvReal.cpp
    parameter/EquinoctialParameters.cpp
    parameter/ExpressionParser.cpp
    parameter/HardwareReal.cpp
    parameter/HardwareParameters.cpp
    parameter/IncomingAsymptoteParameters.cpp
    parameter/KeplerianParameters.cpp
    parameter/ModEquinoctialParameters.cpp
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
    parameter/OutgoingAsymptoteParameters.cpp
    parameter/ParameterDatabase.cpp
    parameter/ParameterInfo.cpp
    parameter/Parameter.cpp
    parameter/ParameterWrapper.cpp
    parameter/PlanetodeticParameters.cpp
    parameter/PlanetData.cpp
    parameter/PlanetParameters.cpp
    parameter/PlanetReal.cpp
    parameter/RealVar.cpp
    parameter/RefData.cpp
    parameter/Rmat33Var.cpp
    parameter/Rmat66Var.cpp
    parameter/Rvec6Var.cpp
    parameter/RvectorVar.cpp
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
    solarsys/ShadowState.cpp
    solarsys/SlpFile.cpp
    solarsys/SolarFluxReader.cpp
    solarsys/SolarSystemException.cpp
    solarsys/SolarSystem.cpp
    solarsys/Star.cpp
    solver/Solver.cpp
    solver/DifferentialCorrector.cpp
    solver/Optimizer.cpp
    solver/InternalOptimizer.cpp
    solver/ISolverListener.cpp
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
    subscriber/EphemManager.cpp
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
    util/AttitudeConversionUtility.cpp
    util/AttitudeUtil.cpp
    util/BaseException.cpp
    util/BodyFixedStateConverter.cpp
    util/CalculationUtilities.cpp
    util/Code500EphemerisFile.cpp
    util/ColorDatabase.cpp
    util/CCSDSAEMEulerAngleSegment.cpp
    util/CCSDSAEMQuaternionSegment.cpp
    util/CCSDSAEMReader.cpp
    util/CCSDSAEMSegment.cpp
    util/CCSDSEMReader.cpp
    util/CCSDSEMSegment.cpp
    util/Date.cpp
    util/DateUtil.cpp
    util/ElapsedTime.cpp
    util/EopFile.cpp
    util/FileManager.cpp
    util/FileUtil.cpp
    util/GmatGlobal.cpp
    util/GregorianDate.cpp
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
    util/SPADFileReader.cpp
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
# Recursively find all include files, which will be added to IDE-based
# projects (VS, XCode, etc.)
FILE(GLOB_RECURSE BASE_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp)

# ====================================================================
# add the install targets

# Create GmatBase as a shared library
ADD_LIBRARY(${TargetName} SHARED ${BASE_SRCS} ${BASE_HEADERS})

# Library name should start with "lib"
# This is always true for Mac/Linux, but needs to be specified for Windows
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES PREFIX "lib")

# ====================================================================
# Add source/header files to IDE-based project source groups
# Macro defined in top-level CMakeLists.txt
_ADDSOURCEGROUPS("${BASE_DIRS}")

# specify the version of the library. 
if(WIN32)
  SET_TARGET_PROPERTIES(${TargetName} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:\"libcmt.lib\" /INCREMENTAL:NO")
endif()

# Create build products in bin directory
_SETOUTPUTDIRECTORY(${TargetName} bin)

# Override debug output directory
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${GMAT_BUILDOUTPUT_DEBUGDIR}
  LIBRARY_OUTPUT_DIRECTORY_DEBUG ${GMAT_BUILDOUTPUT_DEBUGDIR}
  )

# Specify where to install (make install or VS "INSTALL" project)
if(WIN32)
  INSTALL(TARGETS ${TargetName} RUNTIME DESTINATION bin)
else()
  if(APPLE AND GMAT_MAC_APPBUNDLE_PATH)
    SET(INSTALL_DIR ${GMAT_MAC_APPBUNDLE_PATH}/Frameworks)
  else()
    SET(INSTALL_DIR bin)
  endif()
  INSTALL(TARGETS ${TargetName} LIBRARY DESTINATION ${INSTALL_DIR})
endif()

# Set RPATH to find shared libraries in default locations on Mac/Linux
if(UNIX)
  if(APPLE)
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "@loader_path/;@loader_path/../plugins/;@loader_path/../plugins/proprietary/"
      )
  else()
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "\$ORIGIN/;\$ORIGIN/../plugins/;\$ORIGIN/../plugins/proprietary/"
      )
  endif()
endif()
