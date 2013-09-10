//$Id$
//------------------------------------------------------------------------------
//                                  Spacecraft
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Joey Gurganus, Reworked by D. Conway
// Created: 2003/10/22
//
/**
 * Implements the Spacecraft base class.
 *    Spacecraft internal state is in EarthMJ2000Eq Cartesian.
 *    If state output is in Keplerian, the anomaly type is True Anomaly.
 *    Internal time is in A1ModJulian.
 *
 *    It converts to proper format using epochType, stateType, anomalyType
 *    before generating scripts from the internal data.
 */
//------------------------------------------------------------------------------

#include <sstream>
#include "Spacecraft.hpp"
#include "MessageInterface.hpp"
#include "SpaceObjectException.hpp"
#include "StateConversionUtil.hpp"
#include "StringUtil.hpp"
#include "TimeTypes.hpp"
#include "CSFixed.hpp"               // for default and command mode attitude creation
#include "Spinner.hpp"               // for command mode attitude creation
#include "PrecessingSpinner.hpp"
#include "NadirPointing.hpp"
#include "FileManager.hpp"           // for GetFullPathname()
#include "AngleUtil.hpp"             // for PutAngleInDegRange()
#ifdef __USE_SPICE__
#include "SpiceAttitude.hpp"         // for SpiceAttitude - to set object name and ID
#endif

// Do we want to write anomaly type?
//#define __WRITE_ANOMALY_TYPE__


//#define DEBUG_SPACECRAFT
//#define DEBUG_SPACECRAFT_SET
//#define DEBUG_SPACECRAFT_SET_ELEMENT
//#define DEBUG_LOOK_UP_LABEL
//#define DEBUG_SPACECRAFT_CS
//#define DEBUG_RENAME
//#define DEBUG_DATE_FORMAT
//#define DEBUG_STATE_INTERFACE
//#define DEBUG_SC_ATTITUDE
//#define DEBUG_OBJ_CLONE
//#define DEBUG_CLONES
//#define DEBUG_GET_REAL
//#define DEBUG_GET_STATE
//#define DEBUG_SC_PARAMETER_TEXT
//#define DEBUG_SC_REF_OBJECT
//#define DEBUG_SC_EPOCHSTR
//#define DEBUG_SC_SET_STRING
//#define DEBUG_WRITE_PARAMETERS
//#define DEBUG_OWNED_OBJECT_STRINGS
//#define DEBUG_SC_OWNED_OBJECT
//#define DEBUG_MASS_FLOW
//#define DEBUG_SPICE_KERNELS
//#define DEBUG_HARDWARE
//#define DEBUG_SC_INPUT_TYPES
//#define DEBUG_GEN_STRING


#ifdef DEBUG_SPACECRAFT
#include <iostream>
#include <sstream>
#endif

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

const int NO_MODEL = -1;

// Spacecraft parameter types
const Gmat::ParameterType
Spacecraft::PARAMETER_TYPE[SpacecraftParamCount - SpaceObjectParamCount] =
   {
      Gmat::STRING_TYPE,      // Epoch
      Gmat::REAL_TYPE,        // Element1
      Gmat::REAL_TYPE,        // Element2
      Gmat::REAL_TYPE,        // Element3
      Gmat::REAL_TYPE,        // Element4
      Gmat::REAL_TYPE,        // Element5
      Gmat::REAL_TYPE,        // Element6
      Gmat::STRING_TYPE,      // Element1Units
      Gmat::STRING_TYPE,      // Element2Units
      Gmat::STRING_TYPE,      // Element3Units
      Gmat::STRING_TYPE,      // Element4Units
      Gmat::STRING_TYPE,      // Element5Units
      Gmat::STRING_TYPE,      // Element6Units
      Gmat::ENUMERATION_TYPE, // StateType
      Gmat::ENUMERATION_TYPE, // DisplayStateType
      Gmat::ENUMERATION_TYPE, // AnomalyType
      Gmat::OBJECT_TYPE,      // CoordinateSystem
      Gmat::REAL_TYPE,        // DryMass
      Gmat::ENUMERATION_TYPE, // DateFormat
      Gmat::REAL_TYPE,        // Cd
      Gmat::REAL_TYPE,        // Cr
      Gmat::REAL_TYPE,        // DragArea
      Gmat::REAL_TYPE,        // SRPArea
      Gmat::OBJECTARRAY_TYPE, // Tanks
      Gmat::OBJECTARRAY_TYPE, // Thrusters
      Gmat::REAL_TYPE,        // TotalMass
      Gmat::STRING_TYPE,      // Id
      Gmat::OBJECT_TYPE,      // Attitude
      Gmat::RMATRIX_TYPE,     // OrbitSTM
      Gmat::RMATRIX_TYPE,     // OrbitAMatrix
      Gmat::REAL_TYPE,        // CartesianX
      Gmat::REAL_TYPE,        // CartesianY
      Gmat::REAL_TYPE,        // CartesianZ
      Gmat::REAL_TYPE,        // CartesianVX
      Gmat::REAL_TYPE,        // CartesianVY
      Gmat::REAL_TYPE,        // CartesianVZ
      Gmat::REAL_TYPE,        // Mass Flow
      Gmat::OBJECTARRAY_TYPE, // AddHardware
      Gmat::STRING_TYPE,      // Model File
      Gmat::REAL_TYPE,        // Model Offset X
      Gmat::REAL_TYPE,        // Model Offset Y
      Gmat::REAL_TYPE,        // Model Offset Z
      Gmat::REAL_TYPE,        // Model Rotation X
      Gmat::REAL_TYPE,        // Model Rotation Y
      Gmat::REAL_TYPE,        // Model Rotation Z
      Gmat::REAL_TYPE,        // Model Scale Factor
   };

const std::string
Spacecraft::PARAMETER_LABEL[SpacecraftParamCount - SpaceObjectParamCount] =
   {
      "Epoch",
      "Element1",
      "Element2",
      "Element3",
      "Element4",
      "Element5",
      "Element6",
      "Element1Units",
      "Element2Units",
      "Element3Units",
      "Element4Units",
      "Element5Units",
      "Element6Units",
      "StateType",
      "DisplayStateType",
      "AnomalyType",
      "CoordinateSystem",
      "DryMass",
      "DateFormat",
      "Cd",
      "Cr",
      "DragArea",
      "SRPArea",
      "Tanks",
      "Thrusters",
      "TotalMass",
      "Id",
      "Attitude",
      "OrbitSTM",
      "OrbitAMatrix",
      "CartesianX",
      "CartesianY",
      "CartesianZ",
      "CartesianVX",
      "CartesianVY",
      "CartesianVZ",
      "MassFlow",
      "AddHardware",
      "ModelFile",
      "ModelOffsetX",
      "ModelOffsetY",
      "ModelOffsetZ",
      "ModelRotationX",
      "ModelRotationY",
      "ModelRotationZ",
      "ModelScale",
};

const std::string Spacecraft::MULT_REP_STRINGS[EndMultipleReps - CART_X] =
{
   // Cartesian
   "X",
   "Y",
   "Z",
   "VX",
   "VY",
   "VZ",
   // Keplerian
   "SMA",
   "ECC",
   "INC",
   "RAAN",
   "AOP",
   "TA",
   "EA",
   "MA",
   "HA",
   // Modified Keplerian
   "RadPer",
   "RadApo",
   // Speherical AZFPA
   "RMAG",
   "RA",
   "DEC",
   "VMAG",
   "AZI",
   "FPA",
   // Spherical RADEC
   "RAV",
   "DECV",
   // Equinoctial
   "EquinoctialH",
   "EquinoctialK",
   "EquinoctialP",
   "EquinoctialQ",
   "MLONG",
   // ModifiedEquinoctial	 ; Modified by M.H.
   "SemiLatusRectum",
   "ModEquinoctialF",
   "ModEquinoctialG",
   "ModEquinoctialH",
   "ModEquinoctialK",
   "TLONG",
   // Delaunay				 ; Modified by M.H.
   "Delaunayl",
   "Delaunayg",
   "Delaunayh",
   "DelaunayL",
   "DelaunayG",
   "DelaunayH",
   // Planetodetic			; Modified by M.H.
   "PlanetodeticRMAG",
   "PlanetodeticLON",
   "PlanetodeticLAT",
   "PlanetodeticVMAG",
   "PlanetodeticAZI",
   "PlanetodeticHFPA",
};

const Integer  Spacecraft::ATTITUDE_ID_OFFSET  = 20000;
const Real     Spacecraft::UNSET_ELEMENT_VALUE = -999.999;

//-------------------------------------
// public methods
//-------------------------------------

//---------------------------------------------------------------------------
//  Spacecraft(const std::string &name)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <name> Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
Spacecraft::Spacecraft(const std::string &name, const std::string &typeStr) :
   SpaceObject          (Gmat::SPACECRAFT, typeStr, name),
   modelFile            (""),
   modelID              (NO_MODEL),
   dryMass              (850.0),
   coeffDrag            (2.2),
   dragArea             (15.0),
   srpArea              (1.0),
   reflectCoeff         (1.8),
   epochSystem          ("TAI"),
   epochFormat          ("ModJulian"),
   epochType            ("TAIModJulian"),  // Should be A1ModJulian?
   stateType            ("Cartesian"),
   displayStateType     ("Cartesian"),
   anomalyType          ("TA"),
   modelOffsetX         (0),
   modelOffsetY         (0),
   modelOffsetZ         (0),
   modelRotationX       (0),
   modelRotationY       (0),
   modelRotationZ       (0),
   modelScale           (1),
   solarSystem          (NULL),
   internalCoordSystem  (NULL),
   coordinateSystem     (NULL),
   coordSysName         ("EarthMJ2000Eq"),
   originMu             (0.0),
   coordSysSet          (false),
   epochSet             (false),
   spacecraftId         ("SatId"),
   attitudeModel        ("CoordinateSystemFixed"),
   attitude             (NULL),
   totalMass            (850.0),
   initialDisplay       (false),
   csSet                (false),
   isThrusterSettingMode(false),
   orbitSTM             (6,6),
   orbitAMatrix         (6,6),
   includeCartesianState(0)
{
   #ifdef DEBUG_SPACECRAFT
   MessageInterface::ShowMessage
      ("Spacecraft::Spacecraft() <%p>'%s' entered\n", this, name.c_str());
   #endif

   objectTypes.push_back(Gmat::SPACECRAFT);
   objectTypeNames.push_back("Spacecraft");
   ownedObjectCount = 0;
   blockCommandModeAssignment = false;

   std::stringstream ss("");
   ss << GmatTimeConstants::MJD_OF_J2000;
   scEpochStr = ss.str();

   Real a1mjd = UNSET_ELEMENT_VALUE;
   std::string outStr;
   Real taimjd = GmatTimeConstants::MJD_OF_J2000;

   // Internal epoch is in A1ModJulian, so convert
   TimeConverterUtil::Convert("TAIModJulian", taimjd, "",
                              "A1ModJulian", a1mjd, outStr);

   //state.SetEpoch(GmatTimeConstants::MJD_OF_J2000);
   state.SetEpoch(a1mjd);

   for (Integer ii = 0; ii < state.GetSize(); ii++)
      state[ii] = UNSET_ELEMENT_VALUE;

   defaultCartesian[0] = 7100.0;
   defaultCartesian[1] = 0.0;
   defaultCartesian[2] = 1300.0;
   defaultCartesian[3] = 0.0;
   defaultCartesian[4] = 7.35;
   defaultCartesian[5] = 1.0;

   stateElementLabel.push_back("X");
   stateElementLabel.push_back("Y");
   stateElementLabel.push_back("Z");
   stateElementLabel.push_back("VX");
   stateElementLabel.push_back("VY");
   stateElementLabel.push_back("VZ");

   stateElementUnits.push_back("km");
   stateElementUnits.push_back("km");
   stateElementUnits.push_back("km");
   stateElementUnits.push_back("km/s");
   stateElementUnits.push_back("km/s");
   stateElementUnits.push_back("km/s");

   representations.push_back("Cartesian");
   representations.push_back("Keplerian");
   representations.push_back("ModifiedKeplerian");
   representations.push_back("SphericalAZFPA");
   representations.push_back("SphericalRADEC");
   representations.push_back("Equinoctial");
   representations.push_back("ModifiedEquinoctial"); // Modified by M.H.
   representations.push_back("Delaunay");
   representations.push_back("Planetodetic");

   // initialize possible input state types to be any type
   for (unsigned int ii = 0; ii < representations.size(); ii++)
      possibleInputTypes.push_back(representations.at(ii));


   parameterCount = SpacecraftParamCount;

   // Create a default unnamed attitude
   attitude = new CSFixed("");
   attitude->SetEpoch(state.GetEpoch());
   attitude->SetOwningSpacecraft(this);
   ownedObjectCount++;

   #ifdef DEBUG_SC_OWNED_OBJECT
   MessageInterface::ShowMessage
      ("Spacecraft::Spacecraft() <%p>'%s' ownedObjectCount=%d\n",
       this, GetName().c_str(), ownedObjectCount);
   #endif

   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (attitude, "new attitude", "Spacecraft constructor()",
       "attitude = new CSFixed("")", this);
   #endif

   BuildElementLabelMap();

   // Initialize the STM to the identity matrix
   orbitSTM(0,0) = orbitSTM(1,1) = orbitSTM(2,2) =
   orbitSTM(3,3) = orbitSTM(4,4) = orbitSTM(5,5) = 1.0;

   orbitAMatrix(0,0) = orbitAMatrix(1,1) = orbitAMatrix(2,2) =
   orbitAMatrix(3,3) = orbitAMatrix(4,4) = orbitAMatrix(5,5) = 1.0;
   // Initialize the covariance matrix
   covariance.AddCovarianceElement("CartesianState", this);
   covariance.ConstructLHS();
   
   covariance(0,0) = covariance(1,1) = covariance(2,2) = 1.0e10;
   covariance(3,3) = covariance(4,4) = covariance(5,5) = 1.0e6;
   
   // Load default model file
   modelFile = FileManager::Instance()->GetFullPathname("SPACECRAFT_MODEL_FILE");
   modelScale = 3.0;
   modelID = NO_MODEL;
   
   #ifdef DEBUG_SPACECRAFT
   MessageInterface::ShowMessage
      ("Spacecraft::Spacecraft() <%p>'%s' exiting\n", this, name.c_str());
   #endif
}


//---------------------------------------------------------------------------
//  ~Spacecraft()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
Spacecraft::~Spacecraft()
{
   #ifdef DEBUG_SPACECRAFT
   MessageInterface::ShowMessage
      ("Spacecraft::~Spacecraft() <%p>'%s' entered, attitude=<%p>\n",
       this, GetName().c_str(), attitude);
   #endif

   // Delete the attached hardware (it was set as clones in the ObjectInitializer)
   // It is not anymore setting the clone (LOJ: 2009.07.24)
   //@see ObjectInitializer::BuildAssociations()
   DeleteOwnedObjects(true, true, true, true);

   #ifdef DEBUG_SPACECRAFT
   MessageInterface::ShowMessage
      ("Spacecraft::~Spacecraft() <%p>'%s' exiting\n", this, GetName().c_str());
   #endif
}


//---------------------------------------------------------------------------
//  Spacecraft(const Spacecraft &a)
//---------------------------------------------------------------------------
/**
 * Copy Constructor for base Spacecraft structures.
 *
 * @param <a> The original that is being copied.
 *
 * @notes We need to copy internal and display coordinate systems to work
 * properly in the mission sequence for object copy.
 */
//---------------------------------------------------------------------------
Spacecraft::Spacecraft(const Spacecraft &a) :
   SpaceObject          (a),
   modelFile            (a.modelFile),
   modelID              (a.modelID),
   scEpochStr           (a.scEpochStr),
   dryMass              (a.dryMass),
   coeffDrag            (a.coeffDrag),
   dragArea             (a.dragArea),
   srpArea              (a.srpArea),
   reflectCoeff         (a.reflectCoeff),
   epochSystem          (a.epochSystem),
   epochFormat          (a.epochFormat),
   epochType            (a.epochType),
   stateType            (a.stateType),
   displayStateType     (a.displayStateType),
   anomalyType          (a.anomalyType),
   modelOffsetX         (a.modelOffsetX),
   modelOffsetY         (a.modelOffsetY),
   modelOffsetZ         (a.modelOffsetZ),
   modelRotationX       (a.modelRotationX),
   modelRotationY       (a.modelRotationY),
   modelRotationZ       (a.modelRotationZ),
   modelScale           (a.modelScale),
   solarSystem          (a.solarSystem),           // need to copy
   internalCoordSystem  (a.internalCoordSystem),   // need to copy
   coordinateSystem     (a.coordinateSystem),      // need to copy
   coordSysName         (a.coordSysName),
   originMu             (a.originMu),
   defaultCartesian     (a.defaultCartesian),
   possibleInputTypes   (a.possibleInputTypes),
   coordSysSet          (a.coordSysSet),
   epochSet             (a.epochSet),
   coordSysMap          (a.coordSysMap),
   spacecraftId         (a.spacecraftId),
   attitudeModel        (a.attitudeModel),
   coordConverter       (a.coordConverter),
   totalMass            (a.totalMass),
   initialDisplay       (false),
   csSet                (a.csSet),
   isThrusterSettingMode(a.isThrusterSettingMode),
   orbitSTM             (a.orbitSTM),
   orbitAMatrix         (a.orbitAMatrix),
   includeCartesianState(a.includeCartesianState)
{
   #ifdef DEBUG_SPACECRAFT
   MessageInterface::ShowMessage
      ("Spacecraft::Spacecraft(copy) <%p>'%s' entered\n", this, GetName().c_str());
   #endif

   objectTypes.push_back(Gmat::SPACECRAFT);
   objectTypeNames.push_back("Spacecraft");
   parameterCount = a.parameterCount;
   ownedObjectCount = 0;

   state.SetEpoch(a.state.GetEpoch());
   state[0] = a.state[0];
   state[1] = a.state[1];
   state[2] = a.state[2];
   state[3] = a.state[3];
   state[4] = a.state[4];
   state[5] = a.state[5];
   trueAnomaly = a.trueAnomaly;

   stateElementLabel = a.stateElementLabel;
   stateElementUnits = a.stateElementUnits;
   representations   = a.representations;
   tankNames         = a.tankNames;
   thrusterNames     = a.thrusterNames;

   hardwareNames     = a.hardwareNames;
//   hardwareList      = a.hardwareList;

   // set cloned hardware
   CloneOwnedObjects(a.attitude, a.tanks, a.thrusters);

   BuildElementLabelMap();

   #ifdef DEBUG_SPACECRAFT
   MessageInterface::ShowMessage
      ("Spacecraft::Spacecraft(copy) <%p>'%s' exiting\n", this, GetName().c_str());
   #endif
}


//---------------------------------------------------------------------------
//  Spacecraft& operator=(const Spacecraft &a)
//---------------------------------------------------------------------------
/**
 * Assignment operator for Spacecraft structures.
 *
 * @note: Coordinate systems are not copied here.
 *
 * @param <a> The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
Spacecraft& Spacecraft::operator=(const Spacecraft &a)
{
   // Don't do anything if copying self
   if (&a == this)
      return *this;

   #ifdef DEBUG_SPACECRAFT
   MessageInterface::ShowMessage
      ("Spacecraft::Spacecraft(=) <%p>'%s' entered\n", this, GetName().c_str());
   #endif

   SpaceObject::operator=(a);

   ownedObjectCount     = a.ownedObjectCount;

   modelFile            = a.modelFile;
   modelID              = a.modelID;
   scEpochStr           = a.scEpochStr;
   dryMass              = a.dryMass;
   coeffDrag            = a.coeffDrag;
   dragArea             = a.dragArea;
   srpArea              = a.srpArea;
   reflectCoeff         = a.reflectCoeff;
   epochSystem          = a.epochSystem;
   epochFormat          = a.epochFormat;
   epochType            = a.epochType;
   stateType            = a.stateType;
   displayStateType     = a.displayStateType;
   anomalyType          = a.anomalyType;
   coordSysName         = a.coordSysName;
   originMu             = a.originMu;
   defaultCartesian     = a.defaultCartesian;
   possibleInputTypes   = a.possibleInputTypes;
   coordSysSet          = a.coordSysSet;
   epochSet             = a.epochSet;
   coordSysMap          = a.coordSysMap;
   spacecraftId         = a.spacecraftId;
   attitudeModel        = a.attitudeModel;
   solarSystem          = a.solarSystem;         // need to copy
   internalCoordSystem  = a.internalCoordSystem; // need to copy
   coordinateSystem     = a.coordinateSystem;    // need to copy
   coordConverter       = a.coordConverter;
   totalMass            = a.totalMass;
   initialDisplay       = false;
   csSet                = a.csSet;
   isThrusterSettingMode= a.isThrusterSettingMode;
   trueAnomaly          = a.trueAnomaly;
   modelOffsetX         = a.modelOffsetX;
   modelOffsetY         = a.modelOffsetY;
   modelOffsetZ         = a.modelOffsetZ;
   modelRotationX       = a.modelRotationX;
   modelRotationY       = a.modelRotationY;
   modelRotationZ       = a.modelRotationZ;
   modelScale           = a.modelScale;

   #ifdef DEBUG_SPACECRAFT
//   MessageInterface::ShowMessage
//      ("Anomaly has type %s, copied from %s\n", trueAnomaly.GetTypeString().c_str(),
//       a.trueAnomaly.GetTypeString().c_str());
   #endif

   state.SetEpoch(a.state.GetEpoch());
   state[0] = a.state[0];
   state[1] = a.state[1];
   state[2] = a.state[2];
   state[3] = a.state[3];
   state[4] = a.state[4];
   state[5] = a.state[5];

   stateElementLabel = a.stateElementLabel;
   stateElementUnits = a.stateElementUnits;
   representations   = a.representations;
   tankNames         = a.tankNames;
   thrusterNames     = a.thrusterNames;

   hardwareNames     = a.hardwareNames;
//   hardwareList      = a.hardwareList;

   // delete attached hardware, such as tanks and thrusters
   #ifdef DEBUG_SPACECRAFT
   MessageInterface::ShowMessage
      ("Spacecraft::Spacecraft(=) about to delete all owned objects\n");
      #endif
   DeleteOwnedObjects(true, true, true, true);

   // then cloned owned objects
   #ifdef DEBUG_SPACECRAFT
   MessageInterface::ShowMessage
      ("Spacecraft::Spacecraft(=) about to clone all owned objects\n");
      #endif
   CloneOwnedObjects(a.attitude, a.tanks, a.thrusters);

   BuildElementLabelMap();

   orbitSTM = a.orbitSTM;
   orbitAMatrix = a.orbitAMatrix;
//   orbitSpiceKernelNames = a.orbitSpiceKernelNames;
   includeCartesianState = a.includeCartesianState;


   #ifdef DEBUG_SPACECRAFT
   MessageInterface::ShowMessage
      ("Spacecraft::Spacecraft(=) <%p>'%s' exiting\n", this, GetName().c_str());
   #endif

   return *this;
}


//---------------------------------------------------------------------------
// virtual void SetSolarSystem(SolarSystem *ss)
//---------------------------------------------------------------------------
void Spacecraft::SetSolarSystem(SolarSystem *ss)
{
   #ifdef DEBUG_SET_SOLAR_SYSTEM
   MessageInterface::ShowMessage
      ("Spacecraft::SetSolarSystem() this=<%p>'%s' entered, ss=<%p>\n", this,
       GetName().c_str(), ss);
   #endif
   solarSystem = ss;
}


//---------------------------------------------------------------------------
// void SetInternalCoordSystem(CoordinateSystem *cs)
//---------------------------------------------------------------------------
void Spacecraft::SetInternalCoordSystem(CoordinateSystem *cs)
{
   #ifdef DEBUG_SPACECRAFT_CS
   MessageInterface::ShowMessage
      ("Spacecraft::SetInternalCoordSystem() this=<%p> '%s', setting %s <%p>\n",
       this, GetName().c_str(), cs->GetName().c_str(), cs);
   #endif

   if (internalCoordSystem != cs)
   {
      internalCoordSystem = cs;
      if (coordinateSystem == NULL)
         coordinateSystem = cs;
   }
}


//---------------------------------------------------------------------------
// CoordinateSystem* GetInternalCoordSystem()
//---------------------------------------------------------------------------
CoordinateSystem* Spacecraft::GetInternalCoordSystem()
{
   return internalCoordSystem;
}


//---------------------------------------------------------------------------
//  void SetState(const Rvector6 &cartState)
//---------------------------------------------------------------------------
/**
 * Set the elements to Cartesian states.
 *
 * @param <cartState> cartesian state
 *
 */
//---------------------------------------------------------------------------
void Spacecraft::SetState(const Rvector6 &cartState)
{
   #ifdef DEBUG_SPACECRAFT_SET
      MessageInterface::ShowMessage("Spacecraft::SetState(Rvector6)\n");
      MessageInterface::ShowMessage(
      "Spacecraft::SetState(Rvector6) cartesianState=%s\n",
       cartState.ToString().c_str());
   #endif

   SetState(cartState[0], cartState[1], cartState[2],
            cartState[3], cartState[4], cartState[5]);
}


//---------------------------------------------------------------------------
//  void SetState(const std::string elementType, Real *instate)
//---------------------------------------------------------------------------
/**
 * Set the elements to Cartesian states.
 *
 * @param <elementType>  Element Type
 * @param <instate>      element states
 *
 */
//---------------------------------------------------------------------------
void Spacecraft::SetState(const std::string &elementType, Real *instate)
{
   #ifdef DEBUG_SPACECRAFT_SET
      MessageInterface::ShowMessage(
         "Spacecraft::SetState() elementType = %s, instate =\n"
         "   %.9lf, %.9lf, %.9lf, %.14lf, %.14lf, %.14lf\n",
         elementType.c_str(), instate[0], instate[1], instate[2], instate[3],
         instate[4], instate[5]);
   #endif

   Rvector6 newState;

   newState.Set(instate[0],instate[1],instate[2],
                instate[3],instate[4],instate[5]);

   if (elementType != "Cartesian")
   {
      stateType = "Cartesian";
      newState = StateConversionUtil::Convert(instate, elementType,
         stateType, originMu, originFlattening, originEqRadius, anomalyType);
   }

   SetState(newState.Get(0),newState.Get(1),newState.Get(2),
            newState.Get(3),newState.Get(4),newState.Get(5));
}


//------------------------------------------------------------------------------
//  void SetState(const Real s1, const Real s2, const Real s3,
//                const Real s4, const Real s5, const Real s6)
//------------------------------------------------------------------------------
/**
 * Set the elements of a Cartesian state.
 *
 * @param <s1>  First element
 * @param <s2>  Second element
 * @param <s3>  Third element
 * @param <s4>  Fourth element
 * @param <s5>  Fifth element
 * @param <s6>  Sixth element
 */
//------------------------------------------------------------------------------
void Spacecraft::SetState(const Real s1, const Real s2, const Real s3,
                          const Real s4, const Real s5, const Real s6)
{
    state[0] = s1;
    state[1] = s2;
    state[2] = s3;
    state[3] = s4;
    state[4] = s5;
    state[5] = s6;
}




//------------------------------------------------------------------------------
//  GmatState& GetState()
//------------------------------------------------------------------------------
/**
 * "Unhide" the SpaceObject method.
 *
 * @return the core GmatState.
 */
//------------------------------------------------------------------------------
GmatState& Spacecraft::GetState()
{
   #ifdef DEBUG_GET_STATE
   Rvector6 stateTmp;
   stateTmp.Set(SpaceObject::GetState().GetState());
   MessageInterface::ShowMessage
      ("Spacecraft::GetState() '%s' returning\n   %s\n", GetName().c_str(),
       stateTmp.ToString().c_str());
   #endif
   // need to return default Cartesian State if entire state has not been set??
   return SpaceObject::GetState();
}

//------------------------------------------------------------------------------
//  Rvector6 GetState(std::string &rep)
//------------------------------------------------------------------------------
/**
 * Get the Cartesian state converted to the input state type.
 *
 * @return Cartesian state converted to specified state type
 */
//------------------------------------------------------------------------------
Rvector6 Spacecraft::GetState(std::string rep)
{
   #ifdef DEBUG_STATE_INTERFACE
      MessageInterface::ShowMessage("Getting state in representation %s",
         rep.c_str());
   #endif
   rvState = GetStateInRepresentation(rep);
   return rvState;
}


//------------------------------------------------------------------------------
//  Rvector6 GetState(std::string &rep)
//------------------------------------------------------------------------------
/**
 * Get the Cartesian state converted to the input state type.
 *
 * @return Cartesian state converted to specified state type
 */
//------------------------------------------------------------------------------
Rvector6 Spacecraft::GetState(Integer rep)
{
   rvState = GetStateInRepresentation(rep);
   return rvState;
}


//------------------------------------------------------------------------------
//  Rvector6 GetCartesianState()   [OBSOLETE]
//------------------------------------------------------------------------------
/**
 * Get the Cartesian state.
 *
 * @return  Cartesian state
 */
//------------------------------------------------------------------------------
Rvector6 Spacecraft::GetCartesianState()
{
   MessageInterface::ShowMessage("GetCartesianState() is obsolete; "
      "use GetState(\"Cartesian\") or GetState(%d) instead.\n", CARTESIAN_ID);
   return GetState("Cartesian");
}

//------------------------------------------------------------------------------
//  Rvector6 GetKeplerianState()   [OBSOLETE]
//------------------------------------------------------------------------------
/**
 * Get the state converted to Keplerian.
 *
 * @return  Keplerian state
 */
//------------------------------------------------------------------------------
Rvector6 Spacecraft::GetKeplerianState()
{
   MessageInterface::ShowMessage("GetKeplerianState() is obsolete; "
      "use GetState(\"Keplerian\") or GetState(%d) instead.\n", KEPLERIAN_ID);
   return GetState("Keplerian");
}

//------------------------------------------------------------------------------
//  Rvector6 GetModifiedKeplerianState()   [OBSOLETE]
//------------------------------------------------------------------------------
/**
 * Get the state converted to Modified Keplerian.
 *
 * @return  Keplerian state
 */
//------------------------------------------------------------------------------
Rvector6 Spacecraft::GetModifiedKeplerianState()
{
   MessageInterface::ShowMessage("GetModifiedKeplerianState() is obsolete; "
      "use GetState(\"ModifiedKeplerian\") or GetState(%d) instead.\n",
      MODIFIED_KEPLERIAN_ID);
   return GetState("ModifiedKeplerian");
}


//------------------------------------------------------------------------------
// Real GetAnomaly() const
//------------------------------------------------------------------------------
Real Spacecraft::GetAnomaly() const
{
   return trueAnomaly;
}

//------------------------------------------------------------------------------
// std::string GetAnomalyType() const
//------------------------------------------------------------------------------
std::string Spacecraft::GetAnomalyType() const
{
   return anomalyType;
}


//------------------------------------------------------------------------------
// bool HasAttitude()
//------------------------------------------------------------------------------
bool Spacecraft::HasAttitude()
{
   return true;
}

//------------------------------------------------------------------------------
// const Rmatrix33& GetAttitude(Real a1mjdTime) const
//------------------------------------------------------------------------------
const Rmatrix33& Spacecraft::GetAttitude(Real a1mjdTime)
{
   #ifdef DEBUG_SC_ATTITUDE
      MessageInterface::ShowMessage("Entering SC::GetAttitude ...\n");
   #endif
   if (attitude) return attitude->GetCosineMatrix(a1mjdTime);
   else
   {
      std::string errmsg =
         "Error attempting to retrieve Attitude Matrix for spacecraft \"";
      errmsg += instanceName + "\", for which no attitude has been set.\n";
      throw SpaceObjectException(errmsg);
   }
}


//------------------------------------------------------------------------------
// const Rvector3& GetAngularVelocity(Real a1mjdTime) const
//------------------------------------------------------------------------------
const Rvector3&  Spacecraft::GetAngularVelocity(Real a1mjdTime) const
{
   if (attitude) return attitude->GetAngularVelocity(a1mjdTime);
   else
   {
      std::string errmsg =
         "Error attempting to retrieve Angular Velocity for spacecraft \"";
      errmsg += instanceName + "\", for which no attitude has been set.\n";
      throw SpaceObjectException(errmsg);
   }
}

//------------------------------------------------------------------------------
// const UnsignedIntArray& GetEulerAngleSequence() const
//------------------------------------------------------------------------------
const UnsignedIntArray& Spacecraft::GetEulerAngleSequence() const
{
   if (attitude)
      return attitude->GetUnsignedIntArrayParameter("EulerSequenceArray");
   else
   {
      std::string errmsg =
         "Error attempting to retrieve Euler Angle Sequence for spacecraft \"";
      errmsg += instanceName + "\", for which no attitude has been set.\n";
      throw SpaceObjectException(errmsg);
   }
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Spacecraft.
 *
 * @return clone of the Spacecraft.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Spacecraft::Clone() const
{
   Spacecraft *clone = new Spacecraft(*this);

   #ifdef DEBUG_SPACECRAFT
      MessageInterface::ShowMessage
         ("Spacecraft::Clone() cloned <%p>'%s' to <%p>'%s'\n", this,
         instanceName.c_str(), clone, clone->GetName().c_str());
   #endif
   #ifdef DEBUG_OBJ_CLONE
      Spacecraft *tmp = (const_cast<Spacecraft*>(this));
      GmatState vec6Orig = tmp->GetState();
      GmatState vec6     = clone->GetState();
      MessageInterface::ShowMessage("ORIGINAL spacecraft has state of %12.10f %12.10f %12.10f %12.10f %12.10f %12.10f\n",
            vec6Orig[0], vec6Orig[1], vec6Orig[2], vec6Orig[3], vec6Orig[4], vec6Orig[5]);
      MessageInterface::ShowMessage("Cloned spacecraft has state of %12.10f %12.10f %12.10f %12.10f %12.10f %12.10f\n",
            vec6[0], vec6[1], vec6[2], vec6[3], vec6[4], vec6[5]);
   #endif

   return (clone);
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void Spacecraft::Copy(const GmatBase* orig)
{
   operator=(*((Spacecraft *)(orig)));
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool Spacecraft::RenameRefObject(const Gmat::ObjectType type,
                                 const std::string &oldName,
                                 const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Spacecraft::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type != Gmat::HARDWARE && type != Gmat::COORDINATE_SYSTEM)
      return true;
   
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (coordSysName == oldName)
         coordSysName = newName;
   }
   
   // hardware
   if (type == Gmat::HARDWARE)
   {
      for (UnsignedInt i=0; i<hardwareNames.size(); i++)
      {
         if (hardwareNames[i] == oldName)
         {
            hardwareNames[i] = newName;
            break;
         }
      }
      
      for (UnsignedInt i=0; i<thrusterNames.size(); i++)
      {
         if (thrusterNames[i] == oldName)
         {
            thrusterNames[i] = newName;
            break;
         }
      }
      
      for (UnsignedInt i=0; i<tankNames.size(); i++)
      {
         if (tankNames[i] == oldName)
         {
            tankNames[i] = newName;
            break;
         }
      }
   }
   
   if (attitude)
      attitude->RenameRefObject(type, oldName, newName);

   return true;
}


//---------------------------------------------------------------------------
//  const std::string GetAttributeCommentLine(Integer index))
//---------------------------------------------------------------------------
const std::string Spacecraft::GetAttributeCommentLine(Integer index)
{
   #ifdef DEBUG_ATTRIB_COMMENT
   MessageInterface::ShowMessage
      ("Spacecraft::GetAttributeCommentLine() entered, index=%d\n", index);
   #endif
   
   std::string comment;
   std::string text = GetParameterText(index);

   #ifdef DEBUG_ATTRIB_COMMENT
   MessageInterface::ShowMessage("===> text='%s'\n", text.c_str());
   #endif
   
   // Return attribute comment for multiple state reps here
   if (((index >= CART_X) && (index < EndMultipleReps)) || (index == ATTITUDE))
   {
      if (attribCommentLineMap.find(text) != attribCommentLineMap.end())
         comment = attribCommentLineMap[text];

      #ifdef DEBUG_ATTRIB_COMMENT
      MessageInterface::ShowMessage
         ("===> Returning '%s' for '%s'\n", comment.c_str(), text.c_str());
      #endif
      return comment;
   }
   else if (index >= ATTITUDE_ID_OFFSET)
   {
      if (attitude)
         comment = attitude->GetAttributeCommentLine(index - ATTITUDE_ID_OFFSET);
      
      #ifdef DEBUG_ATTRIB_COMMENT
      MessageInterface::ShowMessage
         ("===> Returning '%s' for Attitude\n", comment.c_str());
      #endif
      return comment;
   }
   else
      return SpaceObject::GetAttributeCommentLine(index);
}

//---------------------------------------------------------------------------
//  void SetAttributeCommentLine(Integer index, const std::string &comment)
//---------------------------------------------------------------------------
void Spacecraft::SetAttributeCommentLine(Integer index,
                                         const std::string &comment)
{
   #ifdef DEBUG_ATTRIB_COMMENT
   MessageInterface::ShowMessage
      ("===> Spacecraft::SetAttributeCommentLine() entered, index=%d, comment='%s'\n",
       index, comment.c_str());
   #endif
   std::string text = GetParameterText(index);

   #ifdef DEBUG_ATTRIB_COMMENT
   MessageInterface::ShowMessage("===> text='%s'\n", text.c_str());
   #endif
   
   // Save attribute comment for multiple state reps here
   if (((index >= CART_X) && (index < EndMultipleReps)) || (index == ATTITUDE))
   {
      attribCommentLineMap[text] = comment;
      #ifdef DEBUG_ATTRIB_COMMENT
      MessageInterface::ShowMessage("===> '%s' set to attribCommentLineMap\n", text.c_str());
      #endif
   }
   else if (index >= ATTITUDE_ID_OFFSET)
   {
      if (attitude)
         attitude->SetAttributeCommentLine(index - ATTITUDE_ID_OFFSET, comment);
   }
   else
      SpaceObject::SetAttributeCommentLine(index, comment);
}

//---------------------------------------------------------------------------
//  const std::string GetInlineAttributeComment(Integer index)
//---------------------------------------------------------------------------
const std::string Spacecraft::GetInlineAttributeComment(Integer index)
{
   #ifdef DEBUG_ATTRIB_COMMENT
   MessageInterface::ShowMessage
      ("Spacecraft::GetInlineAttributeComment() entered, index=%d\n", index);
   #endif
   
   std::string comment;
   std::string text = GetParameterText(index);
   #ifdef DEBUG_ATTRIB_COMMENT
   MessageInterface::ShowMessage("===> text='%s'\n", text.c_str());
   #endif
   // Return inline attribute comment for multiple state reps here
   if (((index >= CART_X) && (index < EndMultipleReps)) || (index == ATTITUDE))
   {
      if (inlineAttribCommentMap.find(text) != inlineAttribCommentMap.end())
         comment = inlineAttribCommentMap[text];
      #ifdef DEBUG_ATTRIB_COMMENT
      MessageInterface::ShowMessage
         ("Spacecraft::GetInlineAttributeComment() returning '%s' for '%s'\n",
          comment.c_str(), text.c_str());
      #endif
      return comment;
   }
   else if (index >= ATTITUDE_ID_OFFSET)
   {
      if (attitude)
         comment = attitude->GetInlineAttributeComment(index - ATTITUDE_ID_OFFSET);
      #ifdef DEBUG_ATTRIB_COMMENT
      MessageInterface::ShowMessage
         ("Spacecraft::GetInlineAttributeComment() returning '%s' for Attitude\n",
          comment.c_str());
      #endif
      return comment;
   }
   else
      return SpaceObject::GetInlineAttributeComment(index);
}

//---------------------------------------------------------------------------
//  void SetInlineAttributeComment(Integer index, const std::string &comment)
//---------------------------------------------------------------------------
void Spacecraft::SetInlineAttributeComment(Integer index,
                                           const std::string &comment)
{
   #ifdef DEBUG_ATTRIB_COMMENT
   MessageInterface::ShowMessage
      ("Spacecraft::SetInlineAttributeComment() entered, index=%d, comment='%s'\n",
       index, comment.c_str());
   #endif
   std::string text = GetParameterText(index);
   
   #ifdef DEBUG_ATTRIB_COMMENT
   MessageInterface::ShowMessage("   parameter text='%s'\n", text.c_str());
   #endif
   
   // Save inline attribute comment for multiple state reps here
   if (((index >= CART_X) && (index < EndMultipleReps)) || (index == ATTITUDE))
   {
      inlineAttribCommentMap[text] = comment;
      
      #ifdef DEBUG_ATTRIB_COMMENT
      MessageInterface::ShowMessage
         ("   '%s' set to inlineAttribCommentMap\n", text.c_str());
      #endif
   }
   else if (index >= ATTITUDE_ID_OFFSET)
   {
      #ifdef DEBUG_ATTRIB_COMMENT
      MessageInterface::ShowMessage
         ("   Trying to set comment to attitude<%p>\n", attitude);
      #endif
      if (attitude)
         attitude->SetInlineAttributeComment(index - ATTITUDE_ID_OFFSET, comment);
   }
   else
      SpaceObject::SetInlineAttributeComment(index, comment);
}

//------------------------------------------------------------------------------
// std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * This method returns the name of the referenced objects.
 *
 * @return name of the reference object of the requested type.
 */
//------------------------------------------------------------------------------
std::string Spacecraft::GetRefObjectName(const Gmat::ObjectType type) const
{
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      return coordSysName;
   }
   if (type == Gmat::ATTITUDE)   return "";   // Attitude objects don't have names
   return SpaceObject::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool Spacecraft::HasRefObjectTypeArray()
{
   return true;
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of reference object types used by this class.
 *
 * @return the list of object types.
 *
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& Spacecraft::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::COORDINATE_SYSTEM);
   refObjectTypes.push_back(Gmat::HARDWARE);
   if (attitude)
   {
      ObjectTypeArray attRefObjTypes = attitude->GetRefObjectTypeArray();
      for (unsigned int ii = 0; ii < attRefObjTypes.size(); ii++)
         if (find(refObjectTypes.begin(), refObjectTypes.end(), attRefObjTypes.at(ii)) == refObjectTypes.end())
            refObjectTypes.push_back(attRefObjTypes.at(ii));
   }
   // Now Attitude is local object it will be created all the time
   //refObjectTypes.push_back(Gmat::ATTITUDE);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * This method returns an array with the names of the referenced objects.
 *
 * @return a vector with the names of objects of the requested type.
 */
//------------------------------------------------------------------------------
const StringArray&
Spacecraft::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   #ifdef DEBUG_SC_REF_OBJECT
   MessageInterface::ShowMessage
      ("Spacecraft::GetRefObjectNameArray() <%p>'%s' entered, type='%s'\n",
       this, GetName().c_str(), GmatBase::GetObjectTypeString(type).c_str());
   #endif
   static StringArray fullList;  // Maintain scope if the full list is requested
   fullList.clear();

   // If type is UNKNOWN_OBJECT, add only coordinate system and attitude.
   // Other objects are handled separately in the ObjectInitializer
   if (type == Gmat::UNKNOWN_OBJECT)
   {
      // Put in the SpaceObject origin
      fullList.push_back(originName);

      // Add Spacecraft CS name
      fullList.push_back(coordSysName);

      // Add Tank names
      fullList.insert(fullList.end(), tankNames.begin(), tankNames.end());

      // Add Thruster names and it's ref. object names
      for (ObjectArray::iterator i = thrusters.begin(); i < thrusters.end(); ++i)
      {
         // Add Thruster name
         if ((*i)->GetName() != "")
            fullList.push_back((*i)->GetName());

         // Add Thruster's ref. object name
         StringArray refObjNames = (*i)->GetRefObjectNameArray(type);
         for (StringArray::iterator j = refObjNames.begin(); j != refObjNames.end(); ++j)
         {
            if (find(fullList.begin(), fullList.end(), (*j)) == fullList.end())
               fullList.push_back(*j);
         }
      }

      // Add other hardware names and it's ref. object names
      fullList.insert(fullList.end(), hardwareNames.begin(), hardwareNames.end());

      // Add Attitude's ref. object names
//      std::string attRefObjName = attitude->GetRefObjectName(type);
//      if (find(fullList.begin(), fullList.end(), attRefObjName) == fullList.end())
//         fullList.push_back(attRefObjName);
      // Add Attitude's ref. object name
      StringArray attRefObjNames = attitude->GetRefObjectNameArray(type);
      for (StringArray::iterator j = attRefObjNames.begin(); j != attRefObjNames.end(); ++j)
      {
         if (find(fullList.begin(), fullList.end(), (*j)) == fullList.end())
            fullList.push_back(*j);
      }

      #ifdef DEBUG_SC_REF_OBJECT
      MessageInterface::ShowMessage
         ("Spacecraft::GetRefObjectNameArray() ALL, thrusters.size()=%d, "
          "fullList.size()=%d, returning\n", thrusters.size(), fullList.size());
      for (UnsignedInt i=0; i<fullList.size(); i++)
         MessageInterface::ShowMessage("   '%s'\n", fullList[i].c_str());
      #endif
      return fullList;
   }
   else
   {
      #ifdef DEBUG_SC_REF_OBJECT
      MessageInterface::ShowMessage
         ("Spacecraft::GetRefObjectNameArray() NOT UNNOWN_OBJECT, type='%s' (%d)\n",
          GmatBase::GetObjectTypeString(type).c_str(), type);
      #endif
      if (type == Gmat::ATTITUDE)
      {
         #ifdef DEBUG_SC_REF_OBJECT
            MessageInterface::ShowMessage
            ("Spacecraft::GetRefObjectNameArray() Asking attitude for its Attitude name????\n");
         #endif
         fullList.push_back(attitude->GetRefObjectName(type)); // makes no sense
         return fullList;
      }
      if (type == Gmat::CELESTIAL_BODY)
      {
         // Add Attitude's ref. object names
         #ifdef DEBUG_SC_REF_OBJECT
            MessageInterface::ShowMessage
            ("Spacecraft::GetRefObjectNameArray() Asking attitude for its CelestialBody name\n");
         #endif
//         std::string attRefObjName = attitude->GetRefObjectName(type);
//         #ifdef DEBUG_SC_REF_OBJECT
//            MessageInterface::ShowMessage
//            ("Spacecraft::GetRefObjectNameArray() Adding attitude celestial body %s to the list\n",
//                  attRefObjName.c_str());
//         #endif
//         if (find(fullList.begin(), fullList.end(), attRefObjName) == fullList.end())
//            fullList.push_back(attRefObjName);
         // Add Attitude's ref. object name
         StringArray attRefObjNames = attitude->GetRefObjectNameArray(type);
         for (StringArray::iterator j = attRefObjNames.begin(); j != attRefObjNames.end(); ++j)
         {
            if (find(fullList.begin(), fullList.end(), (*j)) == fullList.end())
            {
               #ifdef DEBUG_SC_REF_OBJECT
                  MessageInterface::ShowMessage
                  ("Spacecraft::GetRefObjectNameArray() Adding attitude celestial body %s to the list\n",
                        (*j).c_str());
               #endif
               fullList.push_back(*j);
            }
         }
         return fullList;
      }

      if (type == Gmat::FUEL_TANK)
         return tankNames;
      if (type == Gmat::THRUSTER)
         return thrusterNames;

      if (type == Gmat::HARDWARE)
      {
         fullList = tankNames;
         fullList.insert(fullList.end(), thrusterNames.begin(), thrusterNames.end());
         fullList.insert(fullList.end(), hardwareNames.begin(), hardwareNames.end());
         return fullList;
      }

      if (type == Gmat::COORDINATE_SYSTEM)
      {
         // Add Spacecraft's CoordinateSystem name
         fullList.push_back(coordSysName);

         // Add Thruster's CoordinateSystem name
         for (ObjectArray::iterator i = thrusters.begin(); i < thrusters.end(); ++i)
         {
            StringArray refObjNames = (*i)->GetRefObjectNameArray(type);
            for (StringArray::iterator j = refObjNames.begin(); j != refObjNames.end(); ++j)
            {
               if (find(fullList.begin(), fullList.end(), (*j)) == fullList.end())
                  fullList.push_back(*j);
            }
         }

         // Add Attitude's CoordinateSystem name
         std::string attRefObjName = attitude->GetRefObjectName(type);

         if (find(fullList.begin(), fullList.end(), attRefObjName) == fullList.end())
            fullList.push_back(attRefObjName);

         #ifdef DEBUG_SC_REF_OBJECT
         MessageInterface::ShowMessage
            ("Spacecraft::GetRefObjectNameArray() CS, thrusters.size()=%d, "
             "fullList.size()=%d, returning\n", thrusters.size(), fullList.size());
         for (UnsignedInt i=0; i<fullList.size(); i++)
            MessageInterface::ShowMessage("   '%s'\n", fullList[i].c_str());
         #endif

         return fullList;
      }
   }

   return SpaceObject::GetRefObjectNameArray(type);
}


// DJC: Not sure if we need this yet...
//------------------------------------------------------------------------------
// bool SetRefObjectName(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
bool Spacecraft::SetRefObjectName(const Gmat::ObjectType type, const std::string &name)
{
   #ifdef DEBUG_SC_REF_OBJECT
   MessageInterface::ShowMessage
      ("Spacecraft::SetRefObjectName() this=<%p>'%s' entered, type=%d, name='%s'\n",
       this, GetName().c_str(), type, name.c_str());
   #endif

   if (type == Gmat::COORDINATE_SYSTEM)
   {
      #ifdef DEBUG_SPACECRAFT_CS
      MessageInterface::ShowMessage
         ("Spacecraft::SetRefObjectName() About to change CoordSysName "
          "'%s' to '%s'\n", coordSysName.c_str(), name.c_str());
      #endif
//      if (coordSysSet && !csSet && (coordSysName != name))
//      {
//         std::string warnmsg = "*** WARNING *** You have set the coordinate system for Spacecraft ";
//         warnmsg += instanceName + " more than once in assignment mode (i.e. before the BeginMissionSequence command).  ";
//         warnmsg += "This may have unintended consequences and you should perform these ";
//         warnmsg += "operations in command mode (i.e. after the BeginMissionSequence command).\n";
//         MessageInterface::PopupMessage(Gmat::WARNING_,warnmsg);
//      }
//      coordSysSet  = true;
      coordSysName = name;
      return true;
   }

   return SpaceObject::SetRefObjectName(type, name);
}


//---------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name)
//---------------------------------------------------------------------------
/**
 * Returns the reference object pointer.
 *
 * @param type type of the reference object.
 * @param name name of the reference object.
 *
 * @return reference object pointer.
 */
//---------------------------------------------------------------------------
GmatBase* Spacecraft::GetRefObject(const Gmat::ObjectType type,
                                   const std::string &name)
{
   #ifdef DEBUG_GET_REF_OBJECT
   MessageInterface::ShowMessage
      ("Spacecraft::GetRefObject() <%p>'%s' entered, type=%d, name='%s'\n"
       "tanks.size()=%d, thrusters.size()=%d\n", this, GetName().c_str(), type,
       name.c_str(), tanks.size(), thrusters.size());
   #endif

   // This switch statement intentionally drops through without breaks, so that
   // the search in the tank and thruster name lists only need to be coded once.
   switch (type)
   {
      case Gmat::COORDINATE_SYSTEM:
         return coordinateSystem;

      case Gmat::ATTITUDE:
         #ifdef DEBUG_SC_ATTITUDE
         MessageInterface::ShowMessage(
         "In SC::GetRefObject - returning Attitude pointer <%p>\n", attitude);
         #endif
         return attitude;

      case Gmat::HARDWARE:
          for (ObjectArray::iterator i = hardwareList.begin();
               i < hardwareList.end(); ++i)
          {
             if ((*i)->GetName() == name)
                return *i;
          }

      case Gmat::FUEL_TANK:
         for (ObjectArray::iterator i = tanks.begin();
              i < tanks.end(); ++i)
         {
            if ((*i)->GetName() == name)
               return *i;
         }

      case Gmat::THRUSTER:
         for (ObjectArray::iterator i = thrusters.begin();
              i < thrusters.end(); ++i)
         {
            if ((*i)->GetName() == name)
            {
               #ifdef DEBUG_GET_REF_OBJECT
               MessageInterface::ShowMessage
                  ("Spacecraft::GetRefObject() Found Thruster named '%s', so "
                   "returning <%p>\n", name.c_str(), (*i));
               #endif
               return *i;
            }
         }

         // Other Hardware cases go here...

         return NULL;      // Hardware requested, but not in the hardware lists

      default:
         break;
   }

   return SpaceObject::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                   const std::string &name)
//------------------------------------------------------------------------------
bool Spacecraft::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                              const std::string &name)
{
   #ifdef DEBUG_SC_REF_OBJECT
   MessageInterface::ShowMessage
      ("Entering SC::SetRefObject <%p>'%s', obj=<%p><%s>'%s'\n", this, GetName().c_str(),
       obj, obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL");
   #endif

   if (obj == NULL)
      return false;

   std::string objType = obj->GetTypeName();
   std::string objName = obj->GetName();

   if (objName == originName)
   {
      if (obj->IsOfType(Gmat::SPACE_POINT))
      {
         origin   = (SpacePoint*)obj;
         if (origin->IsOfType("CelestialBody"))
         {
            originMu         = ((CelestialBody*) origin)->GetGravitationalConstant();
            originFlattening = ((CelestialBody*) origin)->GetFlattening();
            originEqRadius   = ((CelestialBody*) origin)->GetEquatorialRadius();
         }
      }
   }

   // now work on hardware
   if (type == Gmat::HARDWARE || type == Gmat::FUEL_TANK || type == Gmat::THRUSTER)
   {
      #ifdef DEBUG_SC_REF_OBJECT
      MessageInterface::ShowMessage
         ("Spacecraft::SetRefObject() tanks.size()=%d, thrusters.size()=%d\n",
          tanks.size(), thrusters.size());
      #endif

      // set fueltank
      if (objType == "FuelTank")
      {
         bool retval = SetHardware(obj, tankNames, tanks);
         if (retval)
         {
            if (isInitialized)
            {
               // Update tank/thruster settings; don't throw if misconfigured
               try
               {
                  AttachTanksToThrusters();
               }
               catch (BaseException &)
               {
               }
            }
         }
         return retval;
      }

      // set thruster
      if (objType == "Thruster")
      {
         bool retval = SetHardware(obj, thrusterNames, thrusters);
         if (retval)
         {
            if (isInitialized)
            {
               // Update tank/thruster settings; don't throw if misconfigured
               try
               {
                  AttachTanksToThrusters();
               }
               catch (BaseException &)
               {
               }
            }
         }
         return retval;
      }

      // set on hardware
      if (obj->GetType() == Gmat::HARDWARE)
      {
         return SetHardware(obj, hardwareNames, hardwareList);
      }

      return false;
   }
   else if (type == Gmat::COORDINATE_SYSTEM)
   {
      CoordinateSystem *cs = (CoordinateSystem*)obj;

      #ifdef DEBUG_SPACECRAFT_CS
      MessageInterface::ShowMessage
         ("Spacecraft::SetRefObject() '%s', coordinateSystem=%s<%p>, cs=%s<%p>\n",
          instanceName.c_str(), coordinateSystem->GetName().c_str(), coordinateSystem,
          cs->GetName().c_str(), cs);
      #endif

      // Assign CoordinateSystem to map, so that spacecraft can set
      // CoordinateSystem pointer to cloned thruster in SetHardware()(LOJ: 2009.08.25)
      coordSysMap[objName] = cs;
      #ifdef DEBUG_SC_REF_OBJECT
      MessageInterface::ShowMessage
         ("Spacecraft::SetRefObject() Assigned <%p>'%s' to coordSysMap, "
          "coordSysMap.size()=%d, isThrusterSettingMode=%d\n", obj, objName.c_str(),
          coordSysMap.size(), isThrusterSettingMode);
      #endif

      // first, try setting it on the attitude (owned object)   .......
      if (attitude)
      {
         try
         {
            #ifdef DEBUG_SC_ATTITUDE
            MessageInterface::ShowMessage
               ("   Setting <%p><%s>'%s' to attitude <%p>\n", obj,
                objType.c_str(), objName.c_str(), attitude);
            #endif
            // Pass objName as name since name can be blank.
            // Attitude::SetRefObject() checks names before setting
            attitude->SetRefObject(obj, type, objName);
         }
         catch (BaseException &)
         {
            #ifdef DEBUG_SC_ATTITUDE
            MessageInterface::ShowMessage(
               "------ error setting ref object %s on attitude\n",
               name.c_str());
            #endif
         }
      }

      // Set Thruster's CoordinateSystem
      for (ObjectArray::iterator i = thrusters.begin(); i != thrusters.end(); ++i)
      {
         GmatBase *thr = *i;
         std::string thrCsName = thr->GetRefObjectName(Gmat::COORDINATE_SYSTEM);

         if (thrCsName == name)
         {
            #ifdef DEBUG_SC_REF_OBJECT
            MessageInterface::ShowMessage
               ("   Setting CoordinateSystem <%p>'%s' to thruster<%p>'%s'\n",
                cs, name.c_str(), thr, thr->GetName().c_str());
            #endif
            thr->SetRefObject(cs, Gmat::COORDINATE_SYSTEM, thrCsName);
         }
      }

      // If thruster setting mode, we are done.
      if (isThrusterSettingMode)
         return true;

      // If CS name is not the spacecraft CS name, we are done.
      if (objName != coordSysName)
         return true;

      if (cs->UsesSpacecraft(instanceName))
      {
         std::string errmsg = "Cannot set coordinate system \"";
         errmsg += cs->GetName() + "\" on spacecraft \"";
         errmsg += instanceName + "\".  Coordinate system contains circular reference to the spacecraft.\n";
         throw SpaceObjectException(errmsg);
      }

      // Otherwise, convert initial state to new CS
//      if (coordinateSystem == cs)
//      {
//         #ifdef DEBUG_SPACECRAFT_CS
//         MessageInterface::ShowMessage
//            ("   Input coordinateSystem is the same as current one, so ignoring\n");
//         #endif
//      }
//      else
//      {
         #ifdef DEBUG_SPACECRAFT_CS
         MessageInterface::ShowMessage
            ("   About to convert to new CS '%s'\n", coordSysName.c_str());
         #endif

         // saved the old CS and added try/catch block to set to old CS
         // in case of exception thrown (loj: 2008.10.23)
         CoordinateSystem *oldCS = coordinateSystem;
         coordinateSystem = cs;

         originName = coordinateSystem->GetOriginName();
         origin     = coordinateSystem->GetOrigin();
         if (origin->IsOfType("CelestialBody"))
         {
            originMu         = ((CelestialBody*) origin)->GetGravitationalConstant();
            originFlattening = ((CelestialBody*) origin)->GetFlattening();
            originEqRadius   = ((CelestialBody*) origin)->GetEquatorialRadius();
         }

         try
         {
            #ifdef DEBUG_SPACECRAFT_CS
            MessageInterface::ShowMessage
               ("Spacecraft::SetRefObject() calling TakeAction - ApplyCoordinateSystem\n");
            #endif
            TakeAction("ApplyCoordinateSystem");

            #ifdef DEBUG_SPACECRAFT_CS
            MessageInterface::ShowMessage
               ("Spacecraft::SetRefObject() coordinateSystem applied ----------\n");
            Rvector6 vec6(state.GetState());
            MessageInterface::ShowMessage("   %s\n", vec6.ToString().c_str());
            #endif
         }
         catch (BaseException &e)
         {
            #ifdef DEBUG_SPACECRAFT_CS
            MessageInterface::ShowMessage
               ("Exception thrown: '%s', so setting back to old CS\n", e.GetFullMessage().c_str());
            #endif
            coordinateSystem = oldCS;
            throw;
         }
//      }

      return true;
   }
   else if (type == Gmat::ATTITUDE)
   {
      if ((attitude != NULL) && (attitude != (Attitude*) obj))
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (attitude, "attitude", "Spacecraft::SetRefObject()",
             "deleting attitude of " + GetName(), this);
         #endif
         delete attitude;
         ownedObjectCount--;
         #ifdef DEBUG_SC_OWNED_OBJECT
         MessageInterface::ShowMessage
            ("Spacecraft::SetRefObject() <%p>'%s' ownedObjectCount=%d\n",
             this, GetName().c_str(), ownedObjectCount);
         #endif
      }
      attitude = (Attitude*) obj;
      attitudeModel = attitude->GetAttitudeModelName();
      ownedObjectCount++;
      // set epoch ...
      #ifdef DEBUG_SC_ATTITUDE
         MessageInterface::ShowMessage("Setting attitude object <%p> on spacecraft %s <%p>\n",
         attitude, instanceName.c_str(), this);
         MessageInterface::ShowMessage(
         "Setting epoch on attitude object for spacecraft %s\n",
         instanceName.c_str());
      #endif
      attitude->SetEpoch(state.GetEpoch());
      attitude->SetOwningSpacecraft(this);
      #ifdef __USE_SPICE__
         if (attitude->IsOfType("SpiceAttitude"))
            ((SpiceAttitude*) attitude)->SetObjectID(instanceName, naifId, naifIdRefFrame);
      #endif
      return true;
   }
   else
   {
      // first, try setting it on the attitude (owned object)   .......
      if (attitude)
      {
         try
         {
            #ifdef DEBUG_SC_ATTITUDE
            MessageInterface::ShowMessage
               ("   Setting <%p><%s>'%s' to attitude <%p>\n", obj,
                objType.c_str(), objName.c_str(), attitude);
            #endif
            // Pass objName as name since name can be blank.
            // Attitude::SetRefObject() checks names before setting
            attitude->SetRefObject(obj, type, objName);
         }
         catch (BaseException &)
         {
            #ifdef DEBUG_SC_ATTITUDE
            MessageInterface::ShowMessage(
               "------ error setting ref object %s on attitude\n",
               name.c_str());
            #endif
         }
      }
   }


   #ifdef DEBUG_SC_REF_OBJECT
   MessageInterface::ShowMessage
      ("Exiting SC::SetRefObject, Calling SpaceObject::SetRefObject()\n");
   #endif

   return SpaceObject::SetRefObject(obj, type, name);
}


//---------------------------------------------------------------------------
//  ObjectArray& GetRefObjectArray(const Gmat::ObjectType type)
//---------------------------------------------------------------------------
/**
 * Obtains an array of GmatBase pointers by type.
 *
 * @param type The type of objects requested
 *
 * @return Reference to the array.
 */
//---------------------------------------------------------------------------
ObjectArray& Spacecraft::GetRefObjectArray(const Gmat::ObjectType type)
{
   if (type == Gmat::HARDWARE)
      return hardwareList;
   if (type == Gmat::FUEL_TANK)
      return tanks;
   if (type == Gmat::THRUSTER)
      return thrusters;
   return SpaceObject::GetRefObjectArray(type);
}


//---------------------------------------------------------------------------
//  ObjectArray& GetRefObjectArray(const Gmat::ObjectType type)
//---------------------------------------------------------------------------
/**
 * Obtains an array of GmatBase pointers based on a string (e.g. the typename).
 *
 * @param typeString The string used to find the objects requested.
 *
 * @return Reference to the array.
 */
//---------------------------------------------------------------------------
ObjectArray& Spacecraft::GetRefObjectArray(const std::string& typeString)
{
   if (typeString == "Hardware")
      return hardwareList;
   if ((typeString == "FuelTank") || (typeString == "Tanks"))
      return tanks;
   if ((typeString == "Thruster") || (typeString == "Thrusters"))
      return thrusters;
   return SpaceObject::GetRefObjectArray(typeString);
}


//---------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//---------------------------------------------------------------------------
/**
 * Retrieve the ID for the parameter given its description.
 *
 * @param <str> Description for the parameter.
 *
 * @return the parameter ID, or -1 if there is no associated ID.
 */
//---------------------------------------------------------------------------
Integer Spacecraft::GetParameterID(const std::string &str) const
{
   #ifdef DEBUG_PARM_PERFORMANCE
      MessageInterface::ShowMessage("Spacecraft::GetParameterID(%s)\n", str.c_str());
   #endif
   #ifdef DEBUG_GET_REAL
   MessageInterface::ShowMessage("In SC::GetParameterID, str = %s\n", str.c_str());
   #endif

   try
   {
      // handle AddHardware parameter:
      if (str == "AddHardware")
         return ADD_HARDWARE;

      // first check the multiple reps
      Integer sz = EndMultipleReps - CART_X;
      for (Integer ii = 0; ii < sz; ii++)
         if (str == MULT_REP_STRINGS[ii])
         {
            #ifdef DEBUG_GET_REAL
            MessageInterface::ShowMessage(
            "In SC::GetParameterID, multiple reps found!! - str = %s and id = %d\n ",
            str.c_str(), (ii + CART_X));
            #endif
            return ii + CART_X;
         }

      Integer retval = -1;
      if (str == "Element1" || str == "X" || str == "SMA" || str == "RadPer" ||
          str == "RMAG" || str == "Delaunayl" || str == "PlanetodeticRMAG")
         retval =  ELEMENT1_ID;

      else if (str == "Element2" || str == "Y" || str == "ECC" || str == "RadApo" ||
               str == "RA" || str == "PEY" || str == "EquinoctialH" || str == "ModEquinoctialF" ||
               str == "Delaunayg" || str == "PlanetodeticLON")
         retval =  ELEMENT2_ID;

      else if (str == "Element3" || str == "Z" || str == "INC" || str == "DEC" ||
               str == "PEX" || str == "EquinoctialK" || str == "ModEquinoctialG" ||
               str == "Delaunayh" || str == "PlanetodeticLAT")
         retval =  ELEMENT3_ID;

      else if (str == "Element4" || str == "VX" || str == "RAAN" || str == "VMAG" ||
               str == "PNY" || str == "EquinoctialP" || str == "ModEquinoctialH" ||
               str == "DelaunayL" || str == "PlanetodeticVMAG")
         retval =  ELEMENT4_ID;

      else if (str == "Element5" || str == "VY" || str == "AOP" || str == "AZI" ||
               str == "RAV" || str == "PNX" || str == "EquinoctialQ" ||
               str == "ModEquinoctialH" || str == "DelaunayL" || str == "PlanetodeticVMAG")
         retval =  ELEMENT5_ID;

      else if (str == "Element6" || str == "VZ" || str == "TA" || str == "MA" ||
               str == "EA" || str == "HA" || str == "FPA" || str == "DECV" || str == "MLONG" ||
               str == "DelaunayH" || str == "PlanetodeticHFPA")
         retval =  ELEMENT6_ID;
      
      #ifdef DEBUG_GET_REAL
      MessageInterface::ShowMessage(
      "In SC::GetParameterID, after checking for elements, id = %d\n ",
      retval);
      #endif
      if (retval != -1) return retval;

      for (Integer i = SpaceObjectParamCount; i < SpacecraftParamCount; ++i)
      {
         if (str == PARAMETER_LABEL[i - SpaceObjectParamCount])
         {
            #ifdef DEBUG_GET_REAL
            MessageInterface::ShowMessage(
            "In SC::GetParameterID, getting id %d for str = %s\n ",
            i, str.c_str());
            #endif
            return i;
         }
      }
      if (str == "STM")
         return ORBIT_STM;

      if (str == "AMatrix")
         return ORBIT_A_MATRIX;


      if ((str == "CartesianState") || (str == "CartesianX")) return CARTESIAN_X;
      if (str == "CartesianY" )  return CARTESIAN_Y;
      if (str == "CartesianZ" )  return CARTESIAN_Z;
      if (str == "CartesianVX")  return CARTESIAN_VX;
      if (str == "CartesianVY")  return CARTESIAN_VY;
      if (str == "CartesianVZ")  return CARTESIAN_VZ;

      return SpaceObject::GetParameterID(str);
   }
   catch (BaseException&)
   {
      // continue - could be an attitude parameter
      if (attitude)
      {
            Integer attId = attitude->GetParameterID(str);
            #ifdef DEBUG_SC_ATTITUDE
            MessageInterface::ShowMessage(
               "------ Now calling attitude to get id for label %s\n",
               str.c_str());
            MessageInterface::ShowMessage(" ------ and the id = %d\n", attId);
            MessageInterface::ShowMessage(" ------ and the id with offset  = %d\n",
                                          attId + ATTITUDE_ID_OFFSET);
            #endif
            return attId + ATTITUDE_ID_OFFSET;

      }
      // Add other owned objects here

      // Rethrow the exception
      else
         throw;
   }

   return -1;
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false if not,
 */
//---------------------------------------------------------------------------
bool Spacecraft::IsParameterReadOnly(const Integer id) const
{
   if (id >= ATTITUDE_ID_OFFSET)
   {
      if (attitude)
         return attitude->IsParameterReadOnly(id - ATTITUDE_ID_OFFSET);
   }
   // We are currently not allowing users to set anomaly other than the
   // True Anomaly ****** to be modified in the future ******
   if ((id == ELEMENT6_ID) &&
       ((stateElementLabel[5] == "MA") || (stateElementLabel[5] == "EA") ||
        (stateElementLabel[5] == "HA")))
   {
      return true;
   }
   if ((id >= ELEMENT1UNIT_ID) && (id <= ELEMENT6UNIT_ID))
   {
      return true;
   }

   if ((id >= CARTESIAN_X) && (id <= CARTESIAN_VZ))
   {
      return true;
   }

   if (id == TOTAL_MASS_ID)
   {
      return true;
   }

   // Hide SpaceObject epoch so spacecraft can treat it as a string
   if (id == EPOCH_PARAM)
   {
      return true;
   }

   if (id == ORBIT_STM)
   {
      return true;
   }

   if (id == ORBIT_A_MATRIX)
   {
      return true;
   }

   if (id == MASS_FLOW)
   {
      return true;
   }

   if ((id > MODEL_FILE) && (id < MODEL_MAX))
   {
      if (modelFile == "")
         return true;
      else
         return false;
   }

   // NAIF ID is not read-only for spacecraft
   if (id == NAIF_ID)  return false;

   // NAIF ID for the spacecraft reference frame is not read-only for spacecraft
   if (id == NAIF_ID_REFERENCE_FRAME)  return false;

   // if (id == STATE_TYPE) return true;   when deprecated stuff goes away

   return SpaceObject::IsParameterReadOnly(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not.
 */
//---------------------------------------------------------------------------
bool Spacecraft::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Tests to see if an object property can be set in Command mode
 *
 * @param id The ID of the object property
 *
 * @return true if the property can be set in command mode, false if not.
 */
//------------------------------------------------------------------------------
bool Spacecraft::IsParameterCommandModeSettable(const Integer id) const
{
   if (id == MODEL_FILE)
      return false;

   if ((id == ORBIT_SPICE_KERNEL_NAME)    ||
       (id == ATTITUDE_SPICE_KERNEL_NAME) ||
       (id == SC_CLOCK_SPICE_KERNEL_NAME) ||
       (id == FRAME_SPICE_KERNEL_NAME))
      return false;

   return true;
}


//------------------------------------------------------------------------------
// bool ParameterAffectsDynamics(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Determines if a parameter update affects propagation, and therefore forces a
 * reload of parameters used in propagation
 *
 * @param id The ID of the parameter
 *
 * @return true if the parameter affects propagation, false otherwise
 */
//------------------------------------------------------------------------------
bool Spacecraft::ParameterAffectsDynamics(const Integer id) const
{
   if (id == MASS_FLOW)
      return true;

   //if (includeCartesianState > 0)
   if (isManeuvering)
   {
      if (id == CARTESIAN_X)
         return true;
      if (id == CARTESIAN_Y)
         return true;
      if (id == CARTESIAN_Z)
         return true;
      if (id == CARTESIAN_VX)
         return true;
      if (id == CARTESIAN_VY)
         return true;
      if (id == CARTESIAN_VZ)
         return true;
   }

   if (id == SRP_AREA_ID)
      return true;

   if (id == DRAG_AREA_ID)
      return true;

   return SpaceObject::ParameterAffectsDynamics(id);
}

//------------------------------------------------------------------------------
// bool ParameterDvInitializesNonzero(const Integer id) const
//------------------------------------------------------------------------------
bool Spacecraft::ParameterDvInitializesNonzero(const Integer id,
      const Integer r, const Integer c) const
{
   if (id == ORBIT_STM)
   {
      if (r == c-3)
         return true;
      return false;
   }

   if (id == ORBIT_A_MATRIX)
   {
      if (r == c-3)
         return true;
      return false;
   }

   return SpaceObject::ParameterDvInitializesNonzero(id);
}

//------------------------------------------------------------------------------
// Real ParameterDvInitialValue(const Integer id, const Integer r,
//                              const Integer c) const
//------------------------------------------------------------------------------
Real Spacecraft::ParameterDvInitialValue(const Integer id, const Integer r,
      const Integer c) const
{
   if (r == c-3)
      return 1.0;
   return 0.0;
}

//---------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//---------------------------------------------------------------------------
/*
 * @see GmatBase
 */
//---------------------------------------------------------------------------
std::string Spacecraft::GetParameterText(const Integer id) const
{
   #ifdef DEBUG_SC_PARAMETER_TEXT
   MessageInterface::ShowMessage("SC::GetParameterText - called with id = %d\n",
   id);
   #endif

   if ((id >= CART_X) && (id < EndMultipleReps))
   {
      #ifdef DEBUG_SC_PARAMETER_TEXT
      MessageInterface::ShowMessage("SC::GetParameterText - returning text = %s\n",
      (MULT_REP_STRINGS[id-CART_X]).c_str());
      #endif
      return MULT_REP_STRINGS[id - CART_X];
   }
   // Handle the dynamic labels for the elements first
   if (id == ELEMENT1_ID || id == ELEMENT2_ID || id == ELEMENT3_ID
       || id == ELEMENT4_ID || id == ELEMENT5_ID || id == ELEMENT6_ID)
      return stateElementLabel[id - ELEMENT1_ID];

   if ((id >= SpaceObjectParamCount) && (id < SpacecraftParamCount))
      return PARAMETER_LABEL[id - SpaceObjectParamCount];

   if (id >= ATTITUDE_ID_OFFSET)
   {
      if (attitude)
         return attitude->GetParameterText(id - ATTITUDE_ID_OFFSET);
   }

   #ifdef DEBUG_SC_PARAMETER_TEXT
   MessageInterface::ShowMessage(
   "SC::GetParameterText - calling through to base class .....\n");
   #endif
   return SpaceObject::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the type of a parameter.
 *
 * @param <id> Integer ID of the parameter.
 *
 * @return The type of the parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Spacecraft::GetParameterType(const Integer id) const
{
   if ((id >= CART_X) && (id < EndMultipleReps))
      return Gmat::REAL_TYPE;
   if ((id >= SpaceObjectParamCount) && (id < SpacecraftParamCount))
      return PARAMETER_TYPE[id - SpaceObjectParamCount];
   if (id >= ATTITUDE_ID_OFFSET)
      if (attitude)
      {
         #ifdef DEBUG_SC_ATTITUDE
            MessageInterface::ShowMessage
               ("Calling attitude to get parameter type ( for %d) - it is %d (%s)\n",
               id, attitude->GetParameterType(id - ATTITUDE_ID_OFFSET),
               (GmatBase::PARAM_TYPE_STRING[(Integer)(attitude->GetParameterType(id - ATTITUDE_ID_OFFSET))]).c_str());
         #endif
         return attitude->GetParameterType(id - ATTITUDE_ID_OFFSET);
      }

    return SpaceObject::GetParameterType(id);
}

//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the text description for the type of a parameter.
 *
 * @param <id> Integer ID of the parameter.
 *
 * @return The text description of the type of the parameter.
 */
//------------------------------------------------------------------------------
std::string Spacecraft::GetParameterTypeString(const Integer id) const
{
//    return SpaceObject::PARAM_TYPE_STRING[GetParameterType(id)];  // why isn't this right?  WCS
    return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
//---------------------------------------------------------------------------
Real Spacecraft::GetRealParameter(const Integer id) const
{
   #ifdef DEBUG_GET_REAL
      MessageInterface::ShowMessage(
      "In SC::GetReal, asking for parameter %d, whose string is \"%s\"\n",
      id, (GetParameterText(id)).c_str());
      //for (Integer i=0; i<6;i++)
      //   MessageInterface::ShowMessage("   state(%d) = %.12f\n",
      //   i, state[i]);
      //MessageInterface::ShowMessage("    and stateType = %s\n",
      //   stateType.c_str());
   #endif

   if ((id >= ELEMENT1_ID && id <= ELEMENT6_ID) ||
      (id >= CART_X      && id < EndMultipleReps))
   {
      #ifdef DEBUG_GET_REAL
         MessageInterface::ShowMessage(
         "In SC::GetReal, calling GetElement ....... \n");
      #endif
      return (const_cast<Spacecraft*>(this))->GetElement(GetParameterText(id));
   }

   if (id == DRY_MASS_ID)   return dryMass;
   if (id == CD_ID)         return coeffDrag;
   if (id == CR_ID)         return reflectCoeff;
   if (id == DRAG_AREA_ID)  return dragArea;
   if (id == SRP_AREA_ID)   return srpArea;
   if (id == TOTAL_MASS_ID) return UpdateTotalMass();

   if (id == CARTESIAN_X )  return state[0];
   if (id == CARTESIAN_Y )  return state[1];
   if (id == CARTESIAN_Z )  return state[2];
   if (id == CARTESIAN_VX)  return state[3];
   if (id == CARTESIAN_VY)  return state[4];
   if (id == CARTESIAN_VZ)  return state[5];

   if (id == MASS_FLOW)     return UpdateTotalMass();

   if (id >= ATTITUDE_ID_OFFSET)
      if (attitude)
      {
         #ifdef DEBUG_SC_ATTITUDE
         MessageInterface::ShowMessage(
            "------ Now calling attitude to get real parameter for id =  %d\n",
            id);
         #endif
         return attitude->GetRealParameter(id - ATTITUDE_ID_OFFSET);
      }

   if (id == MODEL_OFFSET_X)     return modelOffsetX;
   if (id == MODEL_OFFSET_Y)     return modelOffsetY;
   if (id == MODEL_OFFSET_Z)     return modelOffsetZ;
   if (id == MODEL_ROTATION_X)   return modelRotationX;
   if (id == MODEL_ROTATION_Y)   return modelRotationY;
   if (id == MODEL_ROTATION_Z)   return modelRotationZ;
   if (id == MODEL_SCALE)        return modelScale;

   return SpaceObject::GetRealParameter(id);
}

//---------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param <label> The label of the parameter.
 *
 * @return The parameter's value.
 */
//---------------------------------------------------------------------------
Real Spacecraft::GetRealParameter(const std::string &label) const
{
   // Performance!
    if (label == "A1Epoch")
       return state.GetEpoch();

    // First check with anomaly
    /* OLD CODE
    if (label == "TA" || label == "MA" || label == "EA" || label == "HA")
    {
       //return trueAnomaly.GetValue();
       return trueAnomaly.GetValue(label);
    }
    */

    return GetRealParameter(GetParameterID(label));
}

//---------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//---------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not Real.
 */
//---------------------------------------------------------------------------
Real Spacecraft::SetRealParameter(const Integer id, const Real value)
{
   #ifdef DEBUG_SPACECRAFT_SET
   MessageInterface::ShowMessage("In SC::SetRealParameter (%s), id = %d (%s) and value = %.12f\n",
   instanceName.c_str(), id, GetParameterText(id).c_str(), value);
   #endif
   if (id >= CART_X && id < EndMultipleReps)
   {
      std::string idString = MULT_REP_STRINGS[id - CART_X];
      return SetRealParameter(idString,value);
   }
   if (id == ELEMENT1_ID) return SetRealParameter(stateElementLabel[0],value);
   if (id == ELEMENT2_ID) return SetRealParameter(stateElementLabel[1],value);
   if (id == ELEMENT3_ID) return SetRealParameter(stateElementLabel[2],value);
   if (id == ELEMENT4_ID) return SetRealParameter(stateElementLabel[3],value);
   if (id == ELEMENT5_ID) return SetRealParameter(stateElementLabel[4],value);
   if (id == ELEMENT6_ID) return SetRealParameter(stateElementLabel[5],value);

   if (id == DRY_MASS_ID)
   {
      parmsChanged = true;
      return SetRealParameter("DryMass", value);
   }

   if (id == CD_ID)
   {
      parmsChanged = true;
      return SetRealParameter("Cd",value);
   }
   if (id == CR_ID)
   {
      parmsChanged = true;
      return SetRealParameter("Cr",value);
   }
   if (id == DRAG_AREA_ID)
   {
      parmsChanged = true;
      return SetRealParameter("DragArea",value);
   }
   if (id == SRP_AREA_ID)
   {
      parmsChanged = true;
      return SetRealParameter("SRPArea",value);
   }

   // We should not allow users to set this one -- it's a calculated parameter
   if (id == TOTAL_MASS_ID) return SetRealParameter("TotalMass",value);

   if (id >= ATTITUDE_ID_OFFSET)
      if (attitude)
      {
         // We have to set the epoch on the attitude each time we set a
         // state element
         bool attOK = false;
         //LOJ: To fix compile warning from MSVC++
         //'Real' : forcing value to bool 'true' or 'false' (performance warning)
         //attOK = attitude->SetRealParameter(id - ATTITUDE_ID_OFFSET,value);
         attOK = (attitude->SetRealParameter(id - ATTITUDE_ID_OFFSET,value) == value);
         if (attOK)  attitude->SetEpoch(state.GetEpoch());
         //return attOK; //LOJ: To return real value
         return value;
      }

   if (id == CARTESIAN_X )
   {
      state[0] = value;
      return state[0];
   }

   if (id == CARTESIAN_Y )
   {
      state[1] = value;
      return state[1];
   }

   if (id == CARTESIAN_Z )
   {
      state[2] = value;
      return state[2];
   }

   if (id == CARTESIAN_VX)
   {
      state[3] = value;
      return state[3];
   }

   if (id == CARTESIAN_VY)
   {
      state[4] = value;
      return state[4];
   }

   if (id == CARTESIAN_VZ)
   {
      state[5] = value;
      return state[5];
   }


   if (id == MASS_FLOW)
   {
      return ApplyTotalMass(value);
   }
   
   if (id == MODEL_OFFSET_X || id == MODEL_OFFSET_Y || id == MODEL_OFFSET_Z)
   {
      Real newVal = value;
      if (value < -3.5 || value > 3.5)
      {
         newVal = value < -3.5 ? -3.5 : value > 3.5 ? 3.5 : value;
         std::string valueStr = GmatStringUtil::ToStringNoZeros(value);
         std::string newValStr = GmatStringUtil::ToStringNoZeros(newVal);
         MessageInterface::ShowMessage
            ("*** WARNING *** The value of %s for field \"%s\" on object \"%s\" "
             "is out of bounds so it is set to nearest boundary of %s.  "
             "The allowed values are: [-3.5 <= Real <= 3.5].\n", valueStr.c_str(),
             GetParameterText(id).c_str(), instanceName.c_str(), newValStr.c_str());
      }
      
      if (id == MODEL_OFFSET_X)
         modelOffsetX = newVal;
      else if (id == MODEL_OFFSET_Y)
         modelOffsetY = newVal;
      else if (id == MODEL_OFFSET_Z)
         modelOffsetZ = newVal;
      
      return newVal;
   }
   
   if (id == MODEL_ROTATION_X || id == MODEL_ROTATION_Y || id == MODEL_ROTATION_Z)
   {
      Real newVal = value;
      if (value < -180.0 || value > 180.0)
      {
         // Put angle in -180 and +180 range
         newVal = AngleUtil::PutAngleInDegRange(value, -180.0, 180.0);
         std::string valueStr = GmatStringUtil::ToStringNoZeros(value);
         std::string newValStr = GmatStringUtil::ToStringNoZeros(newVal);
         MessageInterface::ShowMessage
            ("*** WARNING *** The value of %s for field \"%s\" on object \"%s\" "
             "is set to %s to fit the range of -180 and 180.\n", valueStr.c_str(),
             GetParameterText(id).c_str(), instanceName.c_str(), newValStr.c_str());
      }
      
      if (id == MODEL_ROTATION_X)
         modelRotationX = newVal;
      else if (id == MODEL_ROTATION_Y)
         modelRotationY = newVal;
      else if (id == MODEL_ROTATION_Z)
         modelRotationZ = newVal;
      
      return newVal;
   }
   
   if (id == MODEL_SCALE)
   {
      Real newVal = value;
      if (value < 0.001 || value > 1000)
      {
         newVal = value < 0.001 ? 0.001 : value > 1000.0 ? 1000.0 : value;
         std::string valueStr = GmatStringUtil::ToStringNoZeros(value);
         std::string newValStr = GmatStringUtil::ToStringNoZeros(newVal);
         MessageInterface::ShowMessage
            ("*** WARNING *** The value of %s for field \"%s\" on object \"%s\" "
             "is out of bounds so it is set to nearest boundary of %s.  "
             "The allowed values are: [0.001 <= Real <= 1000].\n", valueStr.c_str(),
             GetParameterText(id).c_str(), instanceName.c_str(), newValStr.c_str());
      }
      
      modelScale = newVal;
      return newVal;
   }


   return SpaceObject::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param <label> The label of the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real Spacecraft::SetRealParameter(const std::string &label, const Real value)
{
   #ifdef DEBUG_SPACECRAFT_SET
   MessageInterface::ShowMessage
      ("In SC::SetRealParameter(label)(%s), label = %s and value = %.12f\n",
       instanceName.c_str(), label.c_str(), value);
   #endif
   // first (really) see if it's a parameter for an owned object (i.e. attitude)
   if (GetParameterID(label) >= ATTITUDE_ID_OFFSET)
      if (attitude)
      {
         // We have to set the epoch on the attitude each time we set a
         // state element
         bool attOK = false;
         //LOJ: To fix compile warning from MSVC++
         //'Real' : forcing value to bool 'true' or 'false' (performance warning)
         //attOK = attitude->SetRealParameter(label, value);
         attOK = (attitude->SetRealParameter(label, value) == value);
         if (attOK)  attitude->SetEpoch(state.GetEpoch());
         //return attOK; //LOJ: To return real value
         return value;
      }

   // We are currently not allowing users to set anomaly other than the True
   // Anomaly ****** to be modified in the future ******
   if ((label == "MA") || (label == "EA") || (label == "HA"))
      throw SpaceObjectException
         ("ERROR - setting of anomaly of type other than True Anomaly not "
          "currently allowed.");

   // First try to set as a state element
   if (SetElement(label, value))
      return value;

   if (label == "A1Epoch")
   {
      state.SetEpoch(value);
      if (epochSet && !csSet)
      {
         std::string warnmsg = "*** WARNING *** You have set the epoch for Spacecraft ";
         warnmsg += instanceName + " more than once in assignment mode (i.e. before the BeginMissionSequence command).  ";
         warnmsg += "This may have unintended consequences and you should perform these ";
         warnmsg += "operations in command mode (i.e. after the BeginMissionSequence command).\n";
         MessageInterface::PopupMessage(Gmat::WARNING_,warnmsg);
      }
      epochSet  = true;
      return value;
   }

   if (label == "DryMass")
   {
      if (value >= 0.0)
         dryMass = value;
      else
      {
         SpaceObjectException soe("");
         soe.SetDetails(errorMessageFormat.c_str(),
                        GmatStringUtil::ToString(value, 16).c_str(),
                        "DryMass", "Real Number >= 0.0");
         throw soe;
      }
      parmsChanged = true;
      return dryMass;
   }

   if (label == "Cd")
   {
      if (value >= 0.0)
         coeffDrag = value;
      else
      {
         SpaceObjectException soe("");
         soe.SetDetails(errorMessageFormat.c_str(),
                        GmatStringUtil::ToString(value, 16).c_str(),
                        "Cd", "Real Number >= 0.0");
         throw soe;
      }
      parmsChanged = true;
      return coeffDrag;
   }
   if (label == "DragArea")
   {
      if (value >= 0.0)
         dragArea = value;
      else
      {
         SpaceObjectException soe("");
         soe.SetDetails(errorMessageFormat.c_str(),
                        GmatStringUtil::ToString(value, 16).c_str(),
                        "DragArea", "Real Number >= 0.0");
         throw soe;
      }
      parmsChanged = true;
      return dragArea;
   }
   if (label == "SRPArea")
   {
      if (value >= 0.0)
         srpArea = value;
      else
      {
         SpaceObjectException soe("");
         soe.SetDetails(errorMessageFormat.c_str(),
                        GmatStringUtil::ToString(value, 16).c_str(),
                        "SRPArea", "Real Number >= 0.0");
         throw soe;
      }
      parmsChanged = true;
      return srpArea;
   }
   if (label == "Cr")
   {
      if ((value >= 0.0) && (value <= 2.0))
         reflectCoeff = value;
      else
      {
         SpaceObjectException soe("");
         soe.SetDetails(errorMessageFormat.c_str(),
                        GmatStringUtil::ToString(value, 16).c_str(),
                        "Cr", "0.0 <= Real Number <= 2.0");
         throw soe;
      }
      parmsChanged = true;
      return reflectCoeff;
   }

   if (label == "TotalMass")// return totalMass;    // Don't change the total mass
      throw SpaceObjectException("The parameter \"TotalMass\" is a calculated "
            "parameter and cannot be set on the spacecraft " + instanceName);

   return SpaceObject::SetRealParameter(label, value);
}

//------------------------------------------------------------------------------
//  const Rvector& GetRvectorParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * This method gets the Rvector parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  the Rvector parameter (Angles are returned in degrees).
 *
 */
//------------------------------------------------------------------------------
const Rvector& Spacecraft::GetRvectorParameter(const Integer id) const
{
   if (id >= ATTITUDE_ID_OFFSET)
      if (attitude)
         return attitude->GetRvectorParameter(id - ATTITUDE_ID_OFFSET);
   
   return SpaceObject::GetRvectorParameter(id);
}

//------------------------------------------------------------------------------
//  const Rvector& GetRvectorParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * This method gets the Rvector parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  the Rvector parameter.
 *
 */
//------------------------------------------------------------------------------
const Rvector& Spacecraft::GetRvectorParameter(const std::string &label) const
{
   return GetRvectorParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  const Rvector& SetRvectorParameter(const Integer id, const Rvector& value)
//------------------------------------------------------------------------------
/**
 * This method sets the Rvector parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> Rvector value for the requested parameter (angles are
 *                expected in degrees).
 *
 * @return  Rvector parameter value.
 *
 */
//------------------------------------------------------------------------------
const Rvector& Spacecraft::SetRvectorParameter(const Integer id,
                                               const Rvector &value)
{
   if (id >= ATTITUDE_ID_OFFSET)
      if (attitude)
         return attitude->SetRvectorParameter(id - ATTITUDE_ID_OFFSET, value);
   
   return SpaceObject::SetRvectorParameter(id, value);
}

//------------------------------------------------------------------------------
//  const Rvector& SetRvectorParameter(const std::string &label, 
//                                     const Rvector& value)
//------------------------------------------------------------------------------
/**
 * This method sets the Rvector parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 * @param <value> Rvector value for the requested parameter.
 *
 * @return  Rvector parameter value.
 *
 */
//------------------------------------------------------------------------------
const Rvector& Spacecraft::SetRvectorParameter(const std::string &label,
                                               const Rvector &value)
{
   return SetRvectorParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
//------------------------------------------------------------------------------
std::string Spacecraft::GetStringParameter(const Integer id) const
{
    if (id == SC_EPOCH_ID)
    {
       return (const_cast<Spacecraft*>(this))->GetEpochString();
//       return scEpochStr;
    }

    if (id == DATE_FORMAT_ID)
       return epochType;

    if (id == STATE_TYPE_ID)
    {
       MessageInterface::ShowMessage( "\"StateType\" is deprecated as the "
          "string specifying the state type for display, and will be "
          "removed from a future build; please use \"DisplayStateType\" "
          "instead.\n" );
       return displayStateType;
       //return stateType;
    }

    if (id == DISPLAY_STATE_TYPE_ID)
    {
       return displayStateType;
    }

    if (id == ANOMALY_ID)
       return anomalyType;

    if (id == COORD_SYS_ID)
       return coordSysName;

    if ((id >= ELEMENT1UNIT_ID) && (id <= ELEMENT6UNIT_ID))
       return stateElementUnits[id - ELEMENT1UNIT_ID];

    if (id == SPACECRAFT_ID)
       return spacecraftId;

    if (id >= ATTITUDE_ID_OFFSET)
       if (attitude)
       {
         #ifdef DEBUG_SC_ATTITUDE
         MessageInterface::ShowMessage(
            "------ Now calling attitude to get string parameter for id =  %d\n",
            id);
         #endif
          return attitude->GetStringParameter(id - ATTITUDE_ID_OFFSET);
       }

    if (id == MODEL_FILE)
       return modelFile;

    return SpaceObject::GetStringParameter(id);
}

//---------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <label> The label for the parameter.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
//---------------------------------------------------------------------------
std::string Spacecraft::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}
//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method retrieves a string parameter from a StringArray
 *
 * @param id The ID of the parameter
 *
 * @return The parameter
 */
//------------------------------------------------------------------------------
std::string Spacecraft::GetStringParameter(const Integer id,
                                           const Integer index) const
{
   switch (id)
   {
      case ADD_HARDWARE:
         {
            if ((0 <= index)&(index < (Integer)(hardwareNames.size())))
               return hardwareNames[index];
            else
               return "";
         }

      default:
         break;
   }
   return SpaceObject::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string & label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method retrieves a string parameter from a StringArray
 *
 * @param label The string label for the parameter
 *
 * @return The parameter
 */
//------------------------------------------------------------------------------
std::string Spacecraft::GetStringParameter(const std::string & label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Accesses lists of tank and thruster (and, eventually, other hardware) names,
 * and other StringArray parameters.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//---------------------------------------------------------------------------
const StringArray& Spacecraft::GetStringArrayParameter(const Integer id) const
{
   if (id == ADD_HARDWARE)
      return hardwareNames;
   if (id == FUEL_TANK_ID)
      return tankNames;
   if (id == THRUSTER_ID)
      return thrusterNames;
   if (id >= ATTITUDE_ID_OFFSET)
      if (attitude)
      {
         #ifdef DEBUG_SC_ATTITUDE
         MessageInterface::ShowMessage(
            "------ Now calling attitude to SET string parameter for id =  %d\n",
            id);
         #endif
         return attitude->GetStringArrayParameter(id - ATTITUDE_ID_OFFSET);
      }
   return SpaceObject::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accesses lists of tank and thruster (and, eventually, other hardware) names,
 * and other StringArray parameters.
 *
 * @param label The script string for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& Spacecraft::GetStringArrayParameter(
      const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

//---------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored, false if not.
 */
//---------------------------------------------------------------------------
bool Spacecraft::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_SC_SET_STRING
      MessageInterface::ShowMessage
         ("Spacecraft::SetStringParameter() string parameter %d (%s) to %s\n",
         id, GetParameterText(id).c_str(), value.c_str());
   #endif

   if (id == ATTITUDE)
   {
      #ifdef DEBUG_SC_ATTITUDE
         MessageInterface::ShowMessage("Attitude model type is currently %s\n", attitudeModel.c_str());
         MessageInterface::ShowMessage("Now setting Spacecraft %s attitude model type to \"%s\"\n",
               instanceName.c_str(), value.c_str());
      #endif
      // strip quotes off of the string here - particularly in Command Mode, when the
      // Assignment command treats it as a string
      std::string newAttType = GmatStringUtil::RemoveOuterString(value, "'", "'");
      #ifdef DEBUG_SC_ATTITUDE
         MessageInterface::ShowMessage("Attitude model type after stripping quotes is: %s\n",
               newAttType.c_str());
      #endif
      Attitude *newAtt;  // NOTE - we need a more generic way to do this since users may add
                         // attitude models via plugins!!!
      if (attitudeModel != newAttType)
      {
         if (newAttType == "Spinner")
            newAtt = new Spinner();
         else if (newAttType == "CoordinateSystemFixed")
            newAtt = new CSFixed();
         else if (newAttType == "PrecessingSpinner")
            newAtt = new PrecessingSpinner();
         else if (newAttType == "NadirPointing")
            newAtt = new NadirPointing();
         #ifdef __USE_SPICE__
         else if (newAttType == "SpiceAttitude")
            newAtt = new SpiceAttitude();
         #endif
         else
         {
            std::string errmsg = "Cannot create Attitude object of unknown attitude type \"";
            errmsg += newAttType + "\"\n";
            throw SpaceObjectException(errmsg);
         }
         #ifdef DEBUG_SC_ATTITUDE
            MessageInterface::ShowMessage("Created an attitude of type %s\n",
                  (newAtt->GetTypeName()).c_str());
         #endif
         if (!newAtt)
         {
            std::string errmsg = "Unable to create an Attitude of type ";
            errmsg += value + "\n";
            throw SpaceObjectException(errmsg);
         }
         // Get reference objects from old attitude object
         std::string oldAttCSName      = "";
         std::string oldAttBodyName    = "";
         GmatBase    *oldAttCS         = NULL;
         GmatBase    *oldAttBody       = NULL;
         bool        oldAttFound       = false;
         if (attitude != NULL)
         {
            #ifdef DEBUG_SC_ATTITUDE
               MessageInterface::ShowMessage("   new attitude is of type %s\n",
                     newAtt->GetAttitudeModelName().c_str());
            #endif
            // Get reference coordinate system from old attitude object
            oldAttCSName    = attitude->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
            oldAttCS        = attitude->GetRefObject(Gmat::COORDINATE_SYSTEM, oldAttCSName);
            //oldAttBodyName  = attitude->GetRefObjectName(Gmat::CELESTIAL_BODY);
            //oldAttBody      = attitude->GetRefObject(Gmat::CELESTIAL_BODY, oldAttBodyName);
            oldAttFound = true;
            delete attitude;
            ownedObjectCount--;
         }
         #ifdef __USE_SPICE__
         // If it's a SPICE attitude, set the object data
         // @todo we may need to add some kind of AddData method to Attitude
         // so that plugin attitudes may implement it for setup
         if (newAtt->IsOfType("SpiceAttitude"))
         {
            SpiceAttitude *spiceAtt = (SpiceAttitude*) newAtt;
            spiceAtt->SetObjectID(instanceName, naifId, naifIdRefFrame);
            for (Integer ii = 0; ii < (Integer) attitudeSpiceKernelNames.size(); ii++)
               spiceAtt->SetStringParameter("AttitudeKernelName", attitudeSpiceKernelNames[ii], ii);
            for (Integer ii = 0; ii < (Integer) scClockSpiceKernelNames.size(); ii++)
               spiceAtt->SetStringParameter("SCClockKernelName", scClockSpiceKernelNames[ii], ii);
            for (Integer ii = 0; ii < (Integer) frameSpiceKernelNames.size(); ii++)
               spiceAtt->SetStringParameter("FrameKernelName", frameSpiceKernelNames[ii], ii);
            }
         #endif
         newAtt->SetEpoch(state.GetEpoch()); // correct? do we want to do this?  for Spinner?
         newAtt->NeedsReinitialization();
         newAtt->SetOwningSpacecraft(this);
         if (oldAttFound)
         {
            newAtt->SetRefObjectName(Gmat::COORDINATE_SYSTEM, oldAttCSName);
            newAtt->SetRefObject(oldAttCS, Gmat::COORDINATE_SYSTEM, oldAttCSName);
            //newAtt->SetRefObjectName(Gmat::CELESTIAL_BODY, oldAttBodyName);
            //newAtt->SetRefObject(oldAttCS, Gmat::CELESTIAL_BODY, oldAttBodyName);
         }
         attitude = newAtt;
         ownedObjectCount++;
         attitudeModel = value;
      }
      return true;
   }
   // this is also handled in SpacePoint - we catch it here to tailor the warning message
   if (id == J2000_BODY_NAME)
   {
      static bool writeIgnoredMessage = true;
      if (writeIgnoredMessage)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** \"J2000BodyName\" on Spacecraft is ignored and will be "
             "removed from a future build\n");
         writeIgnoredMessage = false;
      }

      return true;
   }

   if (id == ADD_HARDWARE)
   {
      // Only add the hardware if it is not in the list already
      if (find(hardwareNames.begin(), hardwareNames.end(), value) ==
          hardwareNames.end())
      {
         hardwareNames.push_back(value);
      }
      return true;
   }

   if (id >= ATTITUDE_ID_OFFSET)
      if (attitude)
      {
         #ifdef DEBUG_SC_ATTITUDE
         MessageInterface::ShowMessage(
            "------ Now calling attitude to SET string parameter for id =  %d"
            " and value = %s\n", id, value.c_str());
         #endif
         return attitude->SetStringParameter(id - ATTITUDE_ID_OFFSET, value);
      }

   if ((id < SpaceObjectParamCount) || (id >= SpacecraftParamCount))
   {
      #ifdef DEBUG_SC_SET_STRING
         MessageInterface::ShowMessage
            ("Spacecraft::SetStringParameter() calling SpaceObject::GetPId with id = %d\n", id);
      #endif
      return SpaceObject::SetStringParameter(id, value);
   }

   if (id == SC_EPOCH_ID)
   {
      // Set the epoch value in the state
      SetEpoch(value);
   }
   else if (id == DATE_FORMAT_ID)
   {
      SetDateFormat(value);
   }
   else if ((id == STATE_TYPE_ID) || (id == DISPLAY_STATE_TYPE_ID))
   {
      if (id == STATE_TYPE_ID)
          MessageInterface::ShowMessage( "\"StateType\" is deprecated as the "
          "string specifying the state type for display, and will be "
          "removed from a future build; please use \"DisplayStateType\" "
          "instead.\n" );

      // Check for invalid input then return unknown value from GmatBase
      if (value != "Cartesian" && value != "Keplerian" &&
          value != "ModifiedKeplerian" && value != "SphericalAZFPA" &&
          value != "SphericalRADEC" && value != "Equinoctial" &&
          value != "ModifiedEquinoctial" && value != "Delaunay" && value != "Planetodetic") // Modified by M.H.
      {
         throw SpaceObjectException("Unknown state element representation: " +
            value);
      }
      #ifdef DEBUG_SC_SET_STRING
      MessageInterface::ShowMessage("SC::SetString - setting display state type to %s\n",
      value.c_str());
      #endif

      if ((value == "Keplerian") || (value == "ModifiedKeplerian"))
      {
         // Load trueAnomaly with the state data
         Rvector6 kep = GetStateInRepresentation("Keplerian");
         trueAnomaly = kep[5];
      }

      //stateType = value;
      displayStateType = value;
      UpdateElementLabels();
   }
   else if (id == ANOMALY_ID)
   {
      // Check for invalid input then return unknown value from GmatBase
      if (!StateConversionUtil::IsValidAnomalyType(value))
      {
         return GmatBase::SetStringParameter(id, value);
      }
      #ifdef DEBUG_SC_SET_STRING
//          MessageInterface::ShowMessage("\nSpacecraft::SetStringParamter()..."
//             "\n   Before change, Anomaly info -> a: %f, e: %f, %s: %f\n",
//             trueAnomaly.GetSMA(),trueAnomaly.GetECC(),trueAnomaly.GetTypeString().c_str(),
//             trueAnomaly.GetValue());
      #endif

      //trueAnomaly.SetType(value);
      anomalyType = value;
      UpdateElementLabels();

      #ifdef DEBUG_SC_SET_STRING
//          MessageInterface::ShowMessage(
//             "\n   After change, Anomaly info -> a: %lf, e: %lf, %s: %lf\n",
//             trueAnomaly.GetSMA(), trueAnomaly.GetECC(), trueAnomaly.GetTypeString().c_str(),
//             trueAnomaly.GetValue());
      #endif
      if ((stateType == "Keplerian") ||
          (stateType == "ModifiedKeplerian"))
         rvState[5] = trueAnomaly;   // WCS 2012.07.19 rvState will only have a value now if GetState has been called - ??
   }
   else if (id == COORD_SYS_ID)
   {
      #ifdef DEBUG_SPACECRAFT_CS
      MessageInterface::ShowMessage
         ("Spacecraft::SetStringParameter() About to change CoordSysName "
          "'%s' to '%s'\n", coordSysName.c_str(), value.c_str());
      #endif
      if (coordSysSet && !csSet && (coordSysName != value))
      {
         std::string warnmsg = "*** WARNING *** You have set the coordinate system for Spacecraft ";
         warnmsg += instanceName + " more than once in assignment mode (i.e. before the BeginMissionSequence command).  ";
         warnmsg += "This may have unintended consequences and you should perform these ";
         warnmsg += "operations in command mode (i.e. after the BeginMissionSequence command).\n";
         MessageInterface::PopupMessage(Gmat::WARNING_,warnmsg);
      }
      parmsChanged = true;
      coordSysName = value;
      coordSysSet  = true;
   }
   else if (id == SPACECRAFT_ID)
   {
      spacecraftId = value;
   }
   else if (id == FUEL_TANK_ID)
   {
      // Only add the tank if it is not in the list already
      if (find(tankNames.begin(), tankNames.end(), value) == tankNames.end())
      {
          tankNames.push_back(value);
      }
   }
   else if (id == THRUSTER_ID)
   {
      // Only add the thruster if it is not in the list already
      if (find(thrusterNames.begin(), thrusterNames.end(), value) ==
          thrusterNames.end())
      {
         thrusterNames.push_back(value);
      }
   }
   else if (id == MODEL_FILE)
   {
        modelFile = value;
   }

   #ifdef DEBUG_SC_SET_STRING
   MessageInterface::ShowMessage
      ("Spacecraft::SetStringParameter() returning true\n");
   #endif

   return true;
}

//---------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <label> The label for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored, false if not.
 */
//---------------------------------------------------------------------------
bool Spacecraft::SetStringParameter(const std::string &label,
                                    const std::string &value)
{
   #ifdef DEBUG_SPACECRAFT_SET
       MessageInterface::ShowMessage
          ("\nSpacecraft::SetStringParameter(\"%s\", \"%s\") enters\n",
           label.c_str(), value.c_str() );
       Integer id = GetParameterID(label);
       MessageInterface::ShowMessage
          ("GetParameterText: %s\n", GetParameterText(id).c_str());
       MessageInterface::ShowMessage
          ("Spacecraft::SetStringParameter exits sooner\n\n");
   #endif

   return SetStringParameter(GetParameterID(label),value);
}


//---------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value,
//                          const Integer index)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
bool Spacecraft::SetStringParameter(const Integer id, const std::string &value,
                                    const Integer index)
{
   #ifdef DEBUG_SC_SET_STRING
      MessageInterface::ShowMessage("In SC::SetStringParameter, id = %d, value = %s, index = %d\n",
            id, value.c_str(), index);
   #endif
   if (index < 0)
   {
      SpaceObjectException ex;
      ex.SetDetails("The index %d is out-of-range for field \"%s\"", index,
                    GetParameterText(id).c_str());
      throw ex;
   }
   // check for owned object IDs first
   if (id >= ATTITUDE_ID_OFFSET)
   {
      if (attitude)
      {
         #ifdef DEBUG_SC_ATTITUDE
         MessageInterface::ShowMessage(
            "------ Now calling attitude to SET string parameter for id =  %d"
            ", index = %d,  and value = %s\n", id, index, value.c_str());
         #endif
         return attitude->SetStringParameter(id - ATTITUDE_ID_OFFSET, value, index);
      }
   }

   switch (id)
   {
   case ADD_HARDWARE:
      {
         if (index < (Integer)hardwareNames.size())
            hardwareNames[index] = value;
         else
            // Only add the hardware if it is not in the list already
            if (find(hardwareNames.begin(), hardwareNames.end(), value) == hardwareNames.end())
               hardwareNames.push_back(value);

         return true;
      }
   case FUEL_TANK_ID:
      {
         if (index < (Integer)tankNames.size())
            tankNames[index] = value;
         else
            // Only add the tank if it is not in the list already
            if (find(tankNames.begin(), tankNames.end(), value) == tankNames.end())
               tankNames.push_back(value);

         return true;
      }
   case THRUSTER_ID:
      {
         if (index < (Integer)thrusterNames.size())
            thrusterNames[index] = value;
         else
            // Only add the tank if it is not in the list already
            if (find(thrusterNames.begin(), thrusterNames.end(), value) ==
                thrusterNames.end())
               thrusterNames.push_back(value);

         return true;
      }


   default:
      return SpaceObject::SetStringParameter(id, value, index);
   }
}


//---------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value,
//                          const Integer index)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
bool Spacecraft::SetStringParameter(const std::string &label,
                                    const std::string &value,
                                    const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

// todo: Comment these methods
//---------------------------------------------------------------------------
// const Rmatrix& GetRmatrixParameter(const Integer id) const
//---------------------------------------------------------------------------
const Rmatrix& Spacecraft::GetRmatrixParameter(const Integer id) const
{
   if (id == ORBIT_STM)
      return orbitSTM;

   if (id == ORBIT_A_MATRIX)
      return orbitAMatrix;

//   if (id == ORBIT_COVARIANCE)
//      return covariance;

   return SpaceObject::GetRmatrixParameter(id);
}

//---------------------------------------------------------------------------
// const Rmatrix& SetRmatrixParameter(const Integer id, const Rmatrix &value)
//---------------------------------------------------------------------------
const Rmatrix& Spacecraft::SetRmatrixParameter(const Integer id,
                                         const Rmatrix &value)
{
   if (id == ORBIT_STM)
   {
      orbitSTM = value;
      return orbitSTM;
   }

   if (id == ORBIT_A_MATRIX)
   {
      orbitAMatrix = value;
      return orbitAMatrix;
   }

//   if (id == ORBIT_COVARIANCE)
//   {
//      covariance = value;
//      return covariance;
//   }

   return SpaceObject::SetRmatrixParameter(id, value);
}

//---------------------------------------------------------------------------
// const Rmatrix& GetRmatrixParameter(const std::string &label) const
//---------------------------------------------------------------------------
const Rmatrix& Spacecraft::GetRmatrixParameter(const std::string &label) const
{
   return GetRmatrixParameter(GetParameterID(label));
}

//---------------------------------------------------------------------------
// const Rmatrix& SetRmatrixParameter(const std::string &label,
//                                    const Rmatrix &value)
//---------------------------------------------------------------------------
const Rmatrix& Spacecraft::SetRmatrixParameter(const std::string &label,
                                               const Rmatrix &value)
{
   return SetRmatrixParameter(GetParameterID(label), value);
}

//---------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer row,
//                       const Integer col) const
//---------------------------------------------------------------------------
Real Spacecraft::GetRealParameter(const Integer id, const Integer row,
                                  const Integer col) const
{
   if (id == ORBIT_STM)
      return orbitSTM(row, col);

   if (id == ORBIT_A_MATRIX)
      return orbitAMatrix(row, col);

   return SpaceObject::GetRealParameter(id, row, col);
}

//---------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label, const Integer row,
//                       const Integer col) const
//---------------------------------------------------------------------------
Real Spacecraft::GetRealParameter(const std::string &label,
                                  const Integer row,
                                  const Integer col) const
{
   return GetRealParameter(GetParameterID(label), row, col);
}

//---------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value,
//                       const Integer row, const Integer col)
//---------------------------------------------------------------------------
Real Spacecraft::SetRealParameter(const Integer id, const Real value,
                                  const Integer row, const Integer col)
{
   if (id == ORBIT_STM)
   {
      if ((row < 0) || (row >= orbitSTM.GetNumRows()))
         throw SpaceObjectException("SetRealParameter: row requested for orbitSTM is out-of-range\n");
      if ((col < 0) || (col >= orbitSTM.GetNumColumns()))
         throw SpaceObjectException("SetRealParameter: col requested for orbitSTM is out-of-range\n");
      orbitSTM(row, col) = value;
      return orbitSTM(row, col);
   }

   if (id == ORBIT_A_MATRIX)
   {
      if ((row < 0) || (row >= orbitAMatrix.GetNumRows()))
         throw SpaceObjectException("SetRealParameter: row requested for orbitAMatrix is out-of-range\n");
      if ((col < 0) || (col >= orbitAMatrix.GetNumColumns()))
         throw SpaceObjectException("SetRealParameter: col requested for orbitAMatrix is out-of-range\n");
      orbitAMatrix(row, col) = value;
      return orbitAMatrix(row, col);
   }

   return SpaceObject::SetRealParameter(id, value, row, col);
}

//---------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value, const Integer row,
//                       const Integer col)
//---------------------------------------------------------------------------
Real Spacecraft::SetRealParameter(const std::string &label,
                                  const Real value, const Integer row,
                                  const Integer col)
{
   return SetRealParameter(GetParameterID(label), value, row, col);
}

//---------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value, Integer index)
//---------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new parameter value.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return the parameter value at the end of this call, or
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real Spacecraft::SetRealParameter(const Integer id, const Real value,
                                  const Integer index)
{
   try
   {
      if (id >= ATTITUDE_ID_OFFSET)
      {
         if (attitude)
         {
            #ifdef DEBUG_SC_ATTITUDE
            MessageInterface::ShowMessage(
               "------ Now calling attitude to SET real parameter for id =  %d"
               ", index = %d,  and value = %12.10f\n", id, index, value);
            #endif
            bool attOK = false;
            //LOJ: To fix compile warning from MSVC++
            //'Real' : forcing value to bool 'true' or 'false' (performance warning)
            //attOK = attitude->SetRealParameter(id - ATTITUDE_ID_OFFSET, value, index);
            attOK = (attitude->SetRealParameter(id - ATTITUDE_ID_OFFSET, value, index) == value);
            if (attOK)  attitude->SetEpoch(state.GetEpoch());
            //return attOK; //LOJ: To return real value
            return value;
         }
      }
   }
   catch (BaseException &)
   {
      return SpaceObject::SetRealParameter(id, value, index);
   }
   return SpaceObject::SetRealParameter(id, value, index);
}

//---------------------------------------------------------------------------
//  bool TakeAction(const std::string &action, const std::string &actionData)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
bool Spacecraft::TakeAction(const std::string &action,
                            const std::string &actionData)
{
   #ifdef DEBUG_SC_REF_OBJECT
   MessageInterface::ShowMessage
      ("Entering SC<%p>'%s'::TakeAction with action = '%s', and actionData = "
       "'%s'\n", this, GetName().c_str(), action.c_str(), actionData.c_str());
   #endif
   if (action == "SetupHardware")
   {
      AttachTanksToThrusters();

      #ifdef DEBUG_SC_REF_OBJECT
      MessageInterface::ShowMessage("Leaviong SC::TakeAction with true\n");
      #endif
      return true;
   }

   if (action == "RequireCartesianStateDynamics")
   {
      ++includeCartesianState;
      return true;
   }

   if (action == "ReleaseCartesianStateDynamics")
   {
      --includeCartesianState;
      if (includeCartesianState < 0)
         includeCartesianState = 0;
      return true;
   }

   if ((action == "RemoveHardware") || (action == "RemoveTank") ||
       (action == "RemoveThruster"))
   {
      bool removeTank = true, removeThruster = true, removeAll = false;
      if (action == "RemoveTank")
         removeThruster = false;
      if (action == "RemoveThruster")
         removeTank = false;
      if (actionData == "")
         removeAll = true;

      if (removeThruster)
      {
         if (removeAll)
         {
            DeleteOwnedObjects(false, false, true, false);
            thrusters.clear();
            thrusterNames.clear();
         }
         else
         {
            for (StringArray::iterator i = thrusterNames.begin();
                 i != thrusterNames.end(); ++i)
               if (*i == actionData)
                  thrusterNames.erase(i);
            for (ObjectArray::iterator i = thrusters.begin();
                 i != thrusters.end(); ++i)
               if ((*i)->GetName() == actionData)
               {
                  GmatBase *thr = (*i);
                  thrusters.erase(i);
                  #ifdef DEBUG_MEMORY
                  MemoryTracker::Instance()->Remove
                     (*i, (*i)->GetName(), "Spacecraft::TakeAction()",
                      "deleting cloned Thruster", this);
                  #endif
                  delete thr;
               }
         }
      }

      if (removeTank)
      {
         if (removeAll)
         {
            DeleteOwnedObjects(false, true, true, false);
            tanks.clear();
            tankNames.clear();
         }
         else
         {
            for (StringArray::iterator i = tankNames.begin();
                 i != tankNames.end(); ++i)
               if (*i == actionData)
                  tankNames.erase(i);
            for (ObjectArray::iterator i = tanks.begin();
                 i != tanks.end(); ++i)
               if ((*i)->GetName() == actionData)
               {
                  GmatBase *tnk = (*i);
                  tanks.erase(i);
                  #ifdef DEBUG_MEMORY
                  MemoryTracker::Instance()->Remove
                     (*i, (*i)->GetName(), "Spacecraft::TakeAction()",
                      "deleting cloned Tanks", this);
                  #endif
                  delete tnk;
               }
         }
      }

      #ifdef DEBUG_SC_REF_OBJECT
      MessageInterface::ShowMessage("Leaviong SC::TakeAction with true\n");
      #endif
      return true;
   }

   if (action == "ApplyCoordinateSystem")
   {
      if (!coordinateSystem)
      {
         throw SpaceObjectException(
            "\nError:  Spacecraft has empty coordinate system\n");
      }

      if (csSet == false)
      {
         try
         {
            // We haven't done any state conversions on inputs up to this point (primarily because we
            // don't have a mu to use for conversions until the coordinate system is set).
            // first convert the default cartesian state to the input state type OR if all of the elements have been set,
            // use those values
            // First, check to see if the stateType requires a coordinate system with a Celestial Body
            // origin and if so, if the coordinate system meets that criterion
            bool needsCBOrigin = StateConversionUtil::RequiresCelestialBodyOrigin(stateType);
            if (needsCBOrigin && coordinateSystem && !coordinateSystem->HasCelestialBodyOrigin())
            {
               std::string errmsg = "The Spacecraft \"";
               errmsg += instanceName + "\" failed to set the orbit state because the state type is \"";
               errmsg += stateType + "\" and coordinate system \"";
               errmsg += coordinateSystem->GetName() + "\" does not have a celestial body at the origin.\n";
               throw SpaceObjectException(errmsg);
            }
            Rvector6 convertedState;
           if (NumStateElementsSet() == state.GetSize())
           {
               #ifdef DEBUG_SPACECRAFT_CS
                  MessageInterface::ShowMessage("Spacecraft::TakeAction() all elements are set ------------\n");
               #endif
               for (Integer ii = 0; ii < state.GetSize(); ii++)
                  convertedState[ii] = state[ii];
               #ifdef DEBUG_SPACECRAFT_CS
                  MessageInterface::ShowMessage("    state input in %s is: %s\n", stateType.c_str(), (convertedState.ToString()).c_str());
               #endif
            }
            else
            {
               #ifdef DEBUG_SPACECRAFT_CS
                  MessageInterface::ShowMessage("Spacecraft::TakeAction() Converting cartesian default to state type %s\n", stateType.c_str());
                  MessageInterface::ShowMessage("    coordinateSystem = %s\n", coordinateSystem->GetName().c_str());
                  MessageInterface::ShowMessage("    originMu = %12.10f\n", originMu);
                  MessageInterface::ShowMessage("    ->default cartesian is: %s\n", (defaultCartesian.ToString()).c_str());
                  MessageInterface::ShowMessage("    and current input state is: %12.10f   %12.10f      %12.10f   %12.10f   %12.10f   %12.10f\n",
                        state[0], state[1], state[2], state[3], state[4], state[5]);
               #endif
               convertedState = GetStateInRepresentation(stateType, true); // will use defaultCartesian for the state here
//               convertedState = StateConversionUtil::Convert(originMu, defaultCartesian, "Cartesian", stateType);
               #ifdef DEBUG_SPACECRAFT_CS
                  MessageInterface::ShowMessage("    state converted to %s is: %s\n", stateType.c_str(), (convertedState.ToString()).c_str());
               #endif
               // then assign the input values to the state
               for (Integer ii = 0; ii < state.GetSize(); ii++)
                  if (state[ii] != UNSET_ELEMENT_VALUE) convertedState[ii] = state[ii];
               #ifdef DEBUG_SPACECRAFT_CS
                  MessageInterface::ShowMessage("    state converted to %s with input values is: %s\n", stateType.c_str(), (convertedState.ToString()).c_str());
                  MessageInterface::ShowMessage
                     ("Spacecraft::TakeAction() Calling SetStateFromRepresentation(%s, cartesianstate), "
                      "since CS was not set()\n", stateType.c_str());
               #endif
            }
            SetStateFromRepresentation(stateType, convertedState);
         }
         catch (BaseException &be)
         {
//            std::string errmsg = "Error applying coordinate system due to errors in spacecraft state. ";
//            errmsg += be.GetFullMessage() + "\n";
            throw;
         }

         #ifdef DEBUG_SPACECRAFT_CS
         MessageInterface::ShowMessage
            ("Spacecraft::TakeAction() setting csSet to true ........\n");
         #endif
         csSet = true;
      }

      #ifdef DEBUG_SPACECRAFT_CS
      MessageInterface::ShowMessage("Spacecraft::TakeAction() returning true\n");
      #endif
      return true;
   }

   // 6/12/06 - arg: reset scEpochStr to epoch from prop state
   if (action == "UpdateEpoch")
   {
      Real currEpoch = state.GetEpoch();

      if (epochSystem != "")
      {
         if (epochSystem != "A1")
            currEpoch = TimeConverterUtil::Convert(currEpoch,
                          TimeConverterUtil::A1,
                          TimeConverterUtil::GetTimeTypeID(epochSystem),
                          GmatTimeConstants::JD_JAN_5_1941);
      }

      if (epochFormat != "")
      {
         if (epochFormat == "Gregorian")
            scEpochStr = TimeConverterUtil::ConvertMjdToGregorian(currEpoch);
         else
         {
            std::stringstream timestream;
            timestream.precision(GetTimePrecision());
            timestream << currEpoch;
            scEpochStr = timestream.str();
         }
      }
      #ifdef DEBUG_SC_EPOCHSTR
      MessageInterface::ShowMessage("In TakeAction, epochSystem = %s\n",
      epochSystem.c_str());
      MessageInterface::ShowMessage("In TakeAction, epochFormat = %s\n",
      epochFormat.c_str());
      MessageInterface::ShowMessage("In TakeAction, scEpochStr being set to %s\n",
      scEpochStr.c_str());
      #endif

      return true;
   }

   if (action == "ThrusterSettingMode")
   {
      if (actionData == "On")
         isThrusterSettingMode = true;
      else
         isThrusterSettingMode = false;

      return true;
   }

   if (action == "ResetSTM")
   {
      orbitSTM(0,0) = orbitSTM(1,1) = orbitSTM(2,2) =
      orbitSTM(3,3) = orbitSTM(4,4) = orbitSTM(5,5) = 1.0;

      orbitSTM(0,1)=orbitSTM(0,2)=orbitSTM(0,3)=orbitSTM(0,4)=orbitSTM(0,5)=
      orbitSTM(1,0)=orbitSTM(1,2)=orbitSTM(1,3)=orbitSTM(1,4)=orbitSTM(1,5)=
      orbitSTM(2,0)=orbitSTM(2,1)=orbitSTM(2,3)=orbitSTM(2,4)=orbitSTM(2,5)=
      orbitSTM(3,0)=orbitSTM(3,1)=orbitSTM(3,2)=orbitSTM(3,4)=orbitSTM(3,5)=
      orbitSTM(4,0)=orbitSTM(4,1)=orbitSTM(4,2)=orbitSTM(4,3)=orbitSTM(4,5)=
      orbitSTM(5,0)=orbitSTM(5,1)=orbitSTM(5,2)=orbitSTM(5,3)=orbitSTM(5,4)
            = 0.0;
   }

   if (action == "ResetAMatrix")
   {
      orbitAMatrix(0,0) = orbitAMatrix(1,1) = orbitAMatrix(2,2) =
      orbitAMatrix(3,3) = orbitAMatrix(4,4) = orbitAMatrix(5,5) = 1.0;

      orbitAMatrix(0,1) = orbitAMatrix(0,2) = orbitAMatrix(0,3) =
      orbitAMatrix(0,4) = orbitAMatrix(0,5) = orbitAMatrix(1,0) =
      orbitAMatrix(1,2) = orbitAMatrix(1,3) = orbitAMatrix(1,4) =
      orbitAMatrix(1,5) = orbitAMatrix(2,0) = orbitAMatrix(2,1) =
      orbitAMatrix(2,3) = orbitAMatrix(2,4) = orbitAMatrix(2,5) =
      orbitAMatrix(3,0) = orbitAMatrix(3,1) = orbitAMatrix(3,2) =
      orbitAMatrix(3,4) = orbitAMatrix(3,5) = orbitAMatrix(4,0) =
      orbitAMatrix(4,1) = orbitAMatrix(4,2) = orbitAMatrix(4,3) =
      orbitAMatrix(4,5) = orbitAMatrix(5,0) = orbitAMatrix(5,1) =
      orbitAMatrix(5,2) = orbitAMatrix(5,3) = orbitAMatrix(5,4) = 0.0;
   }

   return SpaceObject::TakeAction(action, actionData);
}


//---------------------------------------------------------------------------
// bool IsOwnedObject(Integer id) const;
//---------------------------------------------------------------------------
bool Spacecraft::IsOwnedObject(Integer id) const
{
   if (id == ATTITUDE)
      return true;
   else
      return false;
}


//---------------------------------------------------------------------------
// GmatBase* GetOwnedObject(Integer whichOne)
//---------------------------------------------------------------------------
GmatBase* Spacecraft::GetOwnedObject(Integer whichOne)
{
   // only one owned object at the moment
   if (attitude)
      return attitude;

   return NULL;
}

//---------------------------------------------------------------------------
// Gmat::ObjectType GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves object type of parameter of given id.
 *
 * @param <id> ID for the parameter.
 *
 * @return parameter ObjectType
 */
//---------------------------------------------------------------------------
Gmat::ObjectType Spacecraft::GetPropertyObjectType(const Integer id) const
{
   if (id >= ATTITUDE_ID_OFFSET)
   {
      if (attitude)
      {
         #ifdef DEBUG_SC_ATTITUDE
         MessageInterface::ShowMessage(
            "------ Now calling attitude to Get property object type for id =  %d\n", id);
         #endif
         return attitude->GetPropertyObjectType(id - ATTITUDE_ID_OFFSET);
      }
   }
   switch (id)
   {
   case COORD_SYS_ID:
      return Gmat::COORDINATE_SYSTEM;
   case FUEL_TANK_ID:
      return Gmat::FUEL_TANK;
   case THRUSTER_ID:
      return Gmat::THRUSTER;
   default:
      return SpaceObject::GetPropertyObjectType(id);
   }
}

//---------------------------------------------------------------------------
// bool CanAssignStringToObjectProperty(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Returns flag indicating whether or not a string can be assigned to the
 * specified object property.
 *
 * @param <id> ID for the property.
 *
 * @return true, if a string can be assigned; false otherwise
 */
//---------------------------------------------------------------------------
bool Spacecraft::CanAssignStringToObjectProperty(const Integer id) const
{
   if (id >= ATTITUDE_ID_OFFSET)
   {
      if (attitude)
      {
         #ifdef DEBUG_SC_ATTITUDE
         MessageInterface::ShowMessage(
            "------ Now calling attitude to check string assignment for id =  %d\n", id);
         #endif
         return attitude->CanAssignStringToObjectProperty(id - ATTITUDE_ID_OFFSET);
      }
   }

//   if ((id == COORD_SYS_ID) || (id == FUEL_TANK_ID) || (id == THRUSTER_ID))
   if (id == COORD_SYS_ID)
      return false;


   return SpaceObject::CanAssignStringToObjectProperty(id);

}


//------------------------------------------------------------------------------
// bool Validate()
//------------------------------------------------------------------------------
/**
 * Checks to be sure that initial spacecraft settings are OK
 *
 * @return true if the settings are correct, throws if not.
 */
//------------------------------------------------------------------------------
bool Spacecraft::Validate()
{
   // right now, we only have the attitude to Validate
   if (attitude)  attitude->Validate();

   return SpaceObject::Validate();
}


//---------------------------------------------------------------------------
//  bool Initialize()
//---------------------------------------------------------------------------
/**
 * Initialize the default values of spacecraft information.
 *
 * @return always success unless the coordinate system is empty
 */
//---------------------------------------------------------------------------
bool Spacecraft::Initialize()
{
   #ifdef DEBUG_SPACECRAFT_CS
   MessageInterface::ShowMessage
      ("Spacecraft::Initialize() entered ---------- this=<%p> '%s'\n   "
       "internalCoordSystem=<%p> '%s', coordinateSystem=<%p> '%s'\n", this,
       GetName().c_str(), internalCoordSystem,
       internalCoordSystem ? internalCoordSystem->GetName().c_str() : "NULL",
       coordinateSystem, coordinateSystem ? coordinateSystem->GetName().c_str() : "NULL");
   MessageInterface::ShowMessage
      ("   stateType=%s, state=\n   %.9f, %.9f, %.9f, %.14f, %.14f, %f.14\n",
       stateType.c_str(), state[0], state[1], state[2], state[3],
       state[4], state[5]);
   #endif

   bool retval = false;

   if (SpaceObject::Initialize())
   {
      // Set the mu if CelestialBody is there through coordinate system's origin;
      // Otherwise, discontinue process and send the error message
      if (!coordinateSystem)
      {
         throw SpaceObjectException("Spacecraft has empty coordinate system");
      }
      if (!attitude)
      {
         #ifdef DEBUG_SC_ATTITUDE
         MessageInterface::ShowMessage("Spacecraft %s has no defined attitude object.\n",
                        instanceName.c_str());
         #endif
         throw SpaceObjectException("Spacecraft has no attitude set.");
      }
      else
      {
         #ifdef DEBUG_SC_ATTITUDE
            MessageInterface::ShowMessage(
            "Initializing attitude object for spacecraft %s\n",
            instanceName.c_str());
         #endif

   #ifdef __USE_SPICE__
         if (attitude->IsOfType("SpiceAttitude"))
         {
            #ifdef DEBUG_SPICE_KERNELS
            MessageInterface::ShowMessage("About to set %d CK kernels on spiceAttitude\n",
                  attitudeSpiceKernelNames.size());
            MessageInterface::ShowMessage("About to set %d SCLK kernels on spiceAttitude\n",
                  scClockSpiceKernelNames.size());
            MessageInterface::ShowMessage("About to set %d FK kernels on spiceAttitude\n",
                  frameSpiceKernelNames.size());
            #endif
            SpiceAttitude *spiceAttitude = (SpiceAttitude*) attitude;
            spiceAttitude->SetObjectID(instanceName, naifId, naifIdRefFrame);
            for (Integer ii = 0; ii < (Integer) attitudeSpiceKernelNames.size(); ii++)
               spiceAttitude->SetStringParameter("AttitudeKernelName", attitudeSpiceKernelNames[ii], ii);
            for (Integer ii = 0; ii < (Integer) scClockSpiceKernelNames.size(); ii++)
               spiceAttitude->SetStringParameter("SCClockKernelName", scClockSpiceKernelNames[ii], ii);
            for (Integer ii = 0; ii < (Integer) frameSpiceKernelNames.size(); ii++)
               spiceAttitude->SetStringParameter("FrameKernelName", frameSpiceKernelNames[ii], ii);
         }
   #endif
         attitude->Initialize();
         #ifdef DEBUG_SC_ATTITUDE
            MessageInterface::ShowMessage(
            "***Finished initializing attitude object for spacecraft %s\n",
            instanceName.c_str());
         #endif
      }

      #ifdef DEBUG_HARDWARE
         MessageInterface::ShowMessage("Hardware list names:\n");
         for (UnsignedInt i = 0; i < hardwareNames.size(); ++i)
         {
            MessageInterface::ShowMessage("   %s\n", hardwareNames[i].c_str());
         }

         MessageInterface::ShowMessage("Hardware list objects:\n");
         for (UnsignedInt i = 0; i < hardwareList.size(); ++i)
         {
            MessageInterface::ShowMessage("   %s\n", hardwareList[i]->GetName().c_str());
         }
      #endif

      // Set the hardware interconnections
      for (ObjectArray::iterator i=hardwareList.begin(); i!=hardwareList.end(); ++i)
      {
         if ((*i)->IsOfType(Gmat::HARDWARE))
         {
            Hardware *current = (Hardware*)(*i);

            // Get the hardware reference list
            StringArray refs = current->GetRefObjectNameArray(Gmat::UNKNOWN_OBJECT);
            for (UnsignedInt j = 0; j < refs.size(); ++j)
            {
               #ifdef DEBUG_HARDWARE
                  MessageInterface::ShowMessage("Connecting up %s for %s\n",
                        refs[j].c_str(), current->GetName().c_str());
               #endif

               for (UnsignedInt k = 0; k < hardwareList.size(); ++k)
               {
                  if (hardwareList[k]->GetName() == refs[j])
                     current->SetRefObject(hardwareList[k],
                           hardwareList[k]->GetType(), hardwareList[k]->GetName());
               }
            }
         }
      }


      // Verify all Spacecraft's referenced objects:
      if (VerifyAddHardware() == false)            // verify added hardware
              return false;

      #ifdef DEBUG_SPACECRAFT_CS
         MessageInterface::ShowMessage("Spacecraft::Initialize() exiting ----------\n");
      #endif

      for (UnsignedInt i = 0; i < tanks.size(); ++i)
      {
         // MessageInterface::ShowMessage("%d:\n%s\n", i,
         //       tanks[i]->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
         tanks[i]->Initialize();
      }

      for (UnsignedInt i = 0; i < thrusters.size(); ++i)
      {
         // MessageInterface::ShowMessage("%d:\n%s\n", i,
         //       thrusters[i]->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
         thrusters[i]->Initialize();
      }

      isInitialized = true;
      retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// std::string GetEpochString()
//------------------------------------------------------------------------------
std::string Spacecraft::GetEpochString()
{
   Real outMjd = -999.999;
   std::string outStr;

   TimeConverterUtil::Convert("A1ModJulian", GetEpoch(), "",
                              epochType, outMjd, outStr);

   return outStr;
}


//------------------------------------------------------------------------------
// void SetDateFormat(const std::string &dateType)
//------------------------------------------------------------------------------
/**
 * Sets the output date format of epoch.
 *
 * @param <dateType> date type given.
 */
//------------------------------------------------------------------------------
void Spacecraft::SetDateFormat(const std::string &dateType)
{
   #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage("Spacecraft::SetDateFormat() "
         "Setting date format to %s; initial epoch is %s\n",
         dateType.c_str(), scEpochStr.c_str());
   #endif

   if (TimeConverterUtil::IsValidTimeSystem(dateType))
   {
      epochType = dateType;
      scEpochStr = GetEpochString();
   }
   else
   {
      char msg[512];
      std::string timeRepList;
      StringArray validReps = TimeConverterUtil::GetValidTimeRepresentations();
      for (UnsignedInt i = 0; i < validReps.size(); ++i)
      {
         if (i != 0)
            timeRepList += ", ";
         timeRepList += validReps[i];
      }

      // The valid format list here should be retrieved from TimeconverterUtil,
      // once that code is refactored
      std::sprintf(msg, errorMessageFormat.c_str(), dateType.c_str(),
            PARAMETER_LABEL[DATE_FORMAT_ID - SpaceObjectParamCount].c_str(),
            timeRepList.c_str());

      throw SpaceObjectException(msg);
   }
}


//------------------------------------------------------------------------------
//  void SetEpoch(std::string ep)
//------------------------------------------------------------------------------
/**
 * Set the epoch.
 *
 * @param <ep> The new epoch.
 */
//------------------------------------------------------------------------------
void Spacecraft::SetEpoch(const std::string &ep)
{
   #ifdef DEBUG_DATE_FORMAT
   MessageInterface::ShowMessage
      ("Spacecraft::SetEpoch() Setting epoch  for spacecraft %s to %s\n",
       instanceName.c_str(), ep.c_str());
   #endif

   std::string timeSystem;
   std::string timeFormat;
   TimeConverterUtil::GetTimeSystemAndFormat(epochType, timeSystem, timeFormat);
   if (timeFormat == "ModJulian") // numeric - save and output without quotes
      scEpochStr = GmatStringUtil::RemoveEnclosingString(ep, "'");
   else // "Gregorian" - not numeric - save and output with quotes
   {
      if (!GmatStringUtil::IsEnclosedWith(ep, "'"))
         scEpochStr = GmatStringUtil::AddEnclosingString(ep, "'");
      else
         scEpochStr = ep;
   }


   Real fromMjd = -999.999;
   Real outMjd = -999.999;
   std::string outStr;

   #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage
         ("Spacecraft::SetEpoch() Converting from %s to A1ModJulian\n", epochType.c_str());
   #endif

   // remove enclosing quotes for the conversion
   std::string epNoQuote = GmatStringUtil::RemoveEnclosingString(ep, "'");
   TimeConverterUtil::Convert(epochType, fromMjd, epNoQuote, "A1ModJulian", outMjd,
                              outStr);
   #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage
         ("Spacecraft::SetEpoch() Done converting from %s to A1ModJulian\n", epochType.c_str());
   #endif

   if (outMjd != -999.999)
   {
      RecomputeStateAtEpoch(outMjd);
      state.SetEpoch(outMjd);
      if (epochSet && !csSet)
      {
         std::string warnmsg = "*** WARNING *** You have set the epoch for Spacecraft ";
         warnmsg += instanceName + " more than once in assignment mode (i.e. before the BeginMissionSequence command).  ";
         warnmsg += "This may have unintended consequences and you should perform these ";
         warnmsg += "operations in command mode (i.e. after the BeginMissionSequence command).\n";
         MessageInterface::PopupMessage(Gmat::WARNING_,warnmsg);
      }
      epochSet  = true;
      if (attitude) attitude->SetEpoch(outMjd);
   }
   else
   {
      #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage("Spacecraft::SetEpoch() oops!  outMjd = -999.999!!\n");
      #endif
   }

}


//------------------------------------------------------------------------------
// void SetEpoch(const std::string &type, const std::string &ep, Real a1mjd)
//------------------------------------------------------------------------------
/*
 * Sets output epoch type, system, format, and epoch.  No conversion is done here.
 *
 * @param  type   epoch type to be used for output (TAIModJulian, TTGregorian, etc)
 * @param  epoch  epoch string
 * @param  a1mjd  epoch in TAIModJulian format (A1MJD?   WCS)
 */
//------------------------------------------------------------------------------
void Spacecraft::SetEpoch(const std::string &type, const std::string &ep, Real a1mjd)
{
   #ifdef DEBUG_SC_EPOCHSTR
   MessageInterface::ShowMessage("In SC::SetEpoch, type = %s, ep = %s, a1mjd = %.12f\n",
   type.c_str(), ep.c_str(), a1mjd);
   #endif
   TimeConverterUtil::GetTimeSystemAndFormat(type, epochSystem, epochFormat);
   epochType = type;
   scEpochStr = ep;
   RecomputeStateAtEpoch(a1mjd);
   state.SetEpoch(a1mjd);
   if (epochSet && !csSet)
   {
      std::string warnmsg = "*** WARNING *** You have set the epoch for Spacecraft ";
      warnmsg += instanceName + " more than once in assignment mode (i.e. before the BeginMissionSequence command).  ";
      warnmsg += "This may have unintended consequences and you should perform these ";
      warnmsg += "operations in command mode (i.e. after the BeginMissionSequence command).\n";
      MessageInterface::PopupMessage(Gmat::WARNING_,warnmsg);
   }
   epochSet  = true;
   if (attitude) attitude->SetEpoch(a1mjd);
   #ifdef DEBUG_SC_EPOCHSTR
   MessageInterface::ShowMessage("and in SC::SetEpoch, epochSystem = %s, epochFormat = %s\n",
   epochSystem.c_str(), epochFormat.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void  SetState(const std::string &type, const Rvector6 &cartState)
//------------------------------------------------------------------------------
/*
 * Sets output state type and state in cartesian representation. Just set to
 * internal cartesian state. No conversion is done here.
 *
 * @param  type       state type to be used for output
 * @param  cartState  cartesian state to set as internal state
 */
//------------------------------------------------------------------------------
void Spacecraft::SetState(const std::string &type, const Rvector6 &cartState)
{
   #ifdef DEBUG_SPACECRAFT_SET
   MessageInterface::ShowMessage
      ("Spacecraft::SetState() type=%s, cartState=\n   %s\n", type.c_str(),
       cartState.ToString().c_str());
   MessageInterface::ShowMessage("About to set displayStateType to %s\n", type.c_str());
   #endif

   //stateType = type;
   displayStateType = type;
   SetState(cartState[0], cartState[1], cartState[2],
            cartState[3], cartState[4], cartState[5]);
   UpdateElementLabels();
}


//------------------------------------------------------------------------------
// void Spacecraft::SetAnomaly(const std::string &type, Real ta)
//------------------------------------------------------------------------------
/*
 * Sets anomaly type and input true anomaly value
 *
 * @param  type  Anomaly type ("TA", "MA", "EA, "HA" or full name like "True Anomaly")
 * @param  ta    True anomaly to set as internal true anomaly
 */
//------------------------------------------------------------------------------
void Spacecraft::SetAnomaly(const std::string &type, Real ta)
{
   trueAnomaly = ta;
   anomalyType = StateConversionUtil::GetAnomalyShortText(type);
   // wcs 2007.05.18 - don't assume - only set the label if it's appropriate
   if (displayStateType == "Keplerian" || displayStateType == "ModifiedKeplerian")
      stateElementLabel[5] = anomalyType;

   #ifdef DEBUG_SPACECRAFT_SET
   MessageInterface::ShowMessage
      ("Spacecraft::SetAnomaly() anomalyType=%s, value=%f\n", anomalyType.c_str(),
       trueAnomaly);
   MessageInterface::ShowMessage
      ("Spacecraft::SetAnomaly() stateElementLabel[5] = %s\n",
      stateElementLabel[5].c_str());
   #endif
}

// todo: comment methods
//------------------------------------------------------------------------------
// Integer Spacecraft::GetPropItemID(const std::string &whichItem)
//------------------------------------------------------------------------------
Integer Spacecraft::GetPropItemID(const std::string &whichItem)
{
   if (whichItem == "CartesianState")
      return Gmat::CARTESIAN_STATE;
   if (whichItem == "STM")
      return Gmat::ORBIT_STATE_TRANSITION_MATRIX;
   if (whichItem == "AMatrix")
      return Gmat::ORBIT_A_MATRIX;

   return SpaceObject::GetPropItemID(whichItem);
}

//------------------------------------------------------------------------------
// Integer SetPropItem(const std::string &propItem)
//------------------------------------------------------------------------------
Integer Spacecraft::SetPropItem(const std::string &propItem)
{
   if (propItem == "CartesianState")
      return Gmat::CARTESIAN_STATE;
   if (propItem == "STM")
      return Gmat::ORBIT_STATE_TRANSITION_MATRIX;
   if (propItem == "AMatrix")
      return Gmat::ORBIT_A_MATRIX;
   if (propItem == "MassFlow")
      if (tanks.size() > 0)
         return Gmat::MASS_FLOW;

   return SpaceObject::SetPropItem(propItem);
}


//------------------------------------------------------------------------------
// StringArray GetDefaultPropItems()
//------------------------------------------------------------------------------
StringArray Spacecraft::GetDefaultPropItems()
{
   StringArray defaults = SpaceObject::GetDefaultPropItems();
   defaults.push_back("CartesianState");
   return defaults;
}


//------------------------------------------------------------------------------
// Real* GetPropItem(const Integer item)
//------------------------------------------------------------------------------
Real* Spacecraft::GetPropItem(const Integer item)
{
   Real* retval = NULL;
   switch (item)
   {
      case Gmat::CARTESIAN_STATE:
         retval = state.GetState();
         break;

      case Gmat::ORBIT_STATE_TRANSITION_MATRIX:
//         retval = stm;
         break;

      case Gmat::ORBIT_A_MATRIX:
         break;

      case Gmat::MASS_FLOW:
         // todo: Access tanks for mass information to handle mass flow
         break;

      // All other values call up the class hierarchy
      default:
         retval = SpaceObject::GetPropItem(item);
   }

   return retval;
}

//------------------------------------------------------------------------------
// Integer GetPropItemSize(const Integer item)
//------------------------------------------------------------------------------
Integer Spacecraft::GetPropItemSize(const Integer item)
{
   Integer retval = -1;
   switch (item)
   {
      case Gmat::CARTESIAN_STATE:
         retval = state.GetSize();
         break;

      case Gmat::ORBIT_STATE_TRANSITION_MATRIX:
         retval = 36;
         break;

      case Gmat::ORBIT_A_MATRIX:
         retval = 36;
         break;
      case Gmat::MASS_FLOW:
         // todo: Access tanks for mass information to handle mass flow

         // For now, only allow one tank
         retval = 1;
         break;

      // All other values call up the hierarchy
      default:
         retval = SpaceObject::GetPropItemSize(item);
   }

   return retval;
}

//------------------------------------------------------------------------------
// bool PropItemNeedsFinalUpdate(const Integer item)
//------------------------------------------------------------------------------
bool Spacecraft::PropItemNeedsFinalUpdate(const Integer item)
{
   switch (item)
   {
      case Gmat::ORBIT_STATE_TRANSITION_MATRIX:
      case Gmat::ORBIT_A_MATRIX:
         return true;

      case Gmat::CARTESIAN_STATE:
      case Gmat::MASS_FLOW:
         return false;

      // All other values call up the hierarchy
      default:
         ;        // Intentional drop through
   }

   return SpaceObject::PropItemNeedsFinalUpdate(item);
}


//------------------------------------------------------------------------------
// bool IsEstimationParameterValid(const Integer item)
//------------------------------------------------------------------------------
bool Spacecraft::IsEstimationParameterValid(const Integer item)
{
   bool retval = false;

   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;

   switch (id)
   {
      case Gmat::CARTESIAN_STATE:
         retval = true;
         break;

      case Gmat::MASS_FLOW:
         // todo: Access tanks for mass information to handle mass flow
         break;

      // All other values call up the hierarchy
      default:
         retval = SpaceObject::IsEstimationParameterValid(item);
   }

   return retval;
}

//------------------------------------------------------------------------------
// Integer GetEstimationParameterSize(const Integer item)
//------------------------------------------------------------------------------
Integer Spacecraft::GetEstimationParameterSize(const Integer item)
{
   Integer retval = 1;


   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Spacecraft::GetEstimationParameterSize(%d)"
            " called; parameter ID is %d\n", item, id);
   #endif


   switch (id)
   {
      case CARTESIAN_X:
         retval = 6;
         break;

      case Gmat::MASS_FLOW:
         // todo: Access tanks for mass information to handle mass flow
         break;

      // All other values call up the hierarchy
      default:
         retval = SpaceObject::GetEstimationParameterSize(item);
   }

   return retval;
}

//------------------------------------------------------------------------------
// Real* GetEstimationParameterValue(const Integer item)
//------------------------------------------------------------------------------
Real* Spacecraft::GetEstimationParameterValue(const Integer item)
{
   Real* retval = NULL;

   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;

   switch (id)
   {
      case CARTESIAN_X:
         retval = state.GetState();
         break;

//      case Gmat::MASS_FLOW:
//         // todo: Access tanks for mass information to handle mass flow
//         break;

      // All other values call up the class heirarchy
      default:
         retval = SpaceObject::GetEstimationParameterValue(item);
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool HasLocalClones()
//------------------------------------------------------------------------------
/**
 * Method to check for the presence of local clones
 *
 * @return true if there are local clones, false if not
 */
//------------------------------------------------------------------------------
bool Spacecraft::HasLocalClones()
{
   return true;
}


//------------------------------------------------------------------------------
// void UpdateClonedObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Updates cloned objects that are copies of the object passed in
 *
 * @param obj The object with setting updates
 */
//------------------------------------------------------------------------------
void Spacecraft::UpdateClonedObject(GmatBase *obj)
{
   #ifdef DEBUG_CLONES
      MessageInterface::ShowMessage("***Spacecraft clone update for %s on %s\n",
            obj->GetName().c_str(), instanceName.c_str());
   #endif

   if (obj->IsOfType(Gmat::HARDWARE))
   {
      Gmat::ObjectType objType = obj->GetType();

      // updates for fueltank
      if (objType == Gmat::FUEL_TANK)
      {
         for (UnsignedInt i = 0; i < tanks.size(); ++i)
         {
            if (obj->GetName() == tanks[i]->GetName())
            {
               ((FuelTank*)tanks[i])->operator=(*((FuelTank*)obj));
               // Update init flag
               if (isInitialized)
                  isInitialized = tanks[i]->IsInitialized();
            }
         }
      }

      // updates for thruster
      if (objType == Gmat::THRUSTER)
      {
         {
            for (UnsignedInt i = 0; i < thrusters.size(); ++i)
               if (obj->GetName() == thrusters[i]->GetName())
               {
                  // Buffer the isFiring flag for the thruster
                  bool active = thrusters[i]->GetBooleanParameter("IsFiring");

                  ((Thruster*)thrusters[i])->operator=(*((Thruster*)obj));
                  // Update init flag
                  if (isInitialized)
                  {
                     isInitialized = thrusters[i]->IsInitialized();
                  }

                  thrusters[i]->SetBooleanParameter("IsFiring", active);
               }
         }
      }

      // updates for other hardware
      if (obj->GetType() == Gmat::HARDWARE)
      {
         // Needs to be watched to be sure hardware uses operator= correctly
         for (UnsignedInt i = 0; i < hardwareList.size(); ++i)
            if (obj->GetName() == hardwareList[i]->GetName())
            {
               ((Hardware*)hardwareList[i])->operator =(*((Hardware*)obj));
               // Update init flag
               if (isInitialized)
                  isInitialized = hardwareList[i]->IsInitialized();
            }
      }
   }

   if (obj->IsOfType(Gmat::ATTITUDE))
   {
      // Get reference coordinate system from old attitude object
      std::string oldAttCSName    = "";
      std::string oldAttBodyName  = "";
      GmatBase    *oldAttCS       = NULL;
      GmatBase    *oldAttBody     = NULL;
      bool        oldAttFound     = false;
      if (attitude != NULL)
      {
         // Get reference coordinate system from old attitude object
         oldAttCSName    = attitude->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
         oldAttCS        = attitude->GetRefObject(Gmat::COORDINATE_SYSTEM, oldAttCSName);
         //oldAttBodyName  = attitude->GetRefObjectName(Gmat::CELESTIAL_BODY);
         //oldAttBody      = attitude->GetRefObject(Gmat::CELESTIAL_BODY, oldAttBodyName);
         oldAttFound = true;
         delete attitude;
         --ownedObjectCount;
      }
      attitude = (Attitude*)(obj->Clone());
      ++ownedObjectCount;
      attitude->SetEpoch(state.GetEpoch());
      attitude->SetOwningSpacecraft(this);

      #ifdef DEBUG_CLONES
      MessageInterface::ShowMessage
         ("   oldAttFound=%d, oldAttCS=<%p>, oldAttBody=<%p>\n", oldAttFound, oldAttCS, oldAttBody);
      #endif
      
      if (oldAttFound)
      {
         attitude->SetRefObjectName(Gmat::COORDINATE_SYSTEM, oldAttCSName);
         attitude->SetRefObject(oldAttCS, Gmat::COORDINATE_SYSTEM, oldAttCSName);
         //attitude->SetRefObjectName(Gmat::CELESTIAL_BODY, oldAttBodyName);
         //attitude->SetRefObject(oldAttBody, Gmat::CELESTIAL_BODY, oldAttBodyName);
      }
      isInitialized = false;
   }
}

//------------------------------------------------------------------------------
// void UpdateClonedObjectParameter(GmatBase *obj, Integer updatedParameterId)
//------------------------------------------------------------------------------
/**
 * This method changes a single parameter on an owned clone
 *
 * @param obj The master object holding the new parameter value
 * @param updatedParameterId The ID of the updated parameter
 */
//------------------------------------------------------------------------------
void Spacecraft::UpdateClonedObjectParameter(GmatBase *obj,
      Integer updatedParameterId)
{
   #ifdef DEBUG_CLONES
      MessageInterface::ShowMessage("Spacecraft::UpdateClonedObject(%s) "
            "called\n", obj->GetName().c_str());
   #endif

   GmatBase *theClone = NULL;

   if (obj->IsOfType(Gmat::HARDWARE))
   {
      Gmat::ObjectType objType = obj->GetType();

      // updates for fueltank
      if (objType == Gmat::FUEL_TANK)
         for (UnsignedInt i = 0; i < tanks.size(); ++i)
            if (obj->GetName() == tanks[i]->GetName())
               theClone = tanks[i];

      // updates for thruster
      if (objType == Gmat::THRUSTER)
         for (UnsignedInt i = 0; i < thrusters.size(); ++i)
            if (obj->GetName() == thrusters[i]->GetName())
               theClone = thrusters[i];

      // updates for other hardware
      if (objType == Gmat::HARDWARE)
         for (UnsignedInt i = 0; i < hardwareList.size(); ++i)
            if (obj->GetName() == hardwareList[i]->GetName())
               theClone = hardwareList[i];
   }

   if (theClone != NULL)
      theClone->CopyParameter(*obj, updatedParameterId);

   if (obj->IsOfType(Gmat::ATTITUDE))
      UpdateClonedObject(obj);
}


//-------------------------------------
// protected methods
//-------------------------------------


//------------------------------------------------------------------------------
//  void UpdateTotalMass()
//------------------------------------------------------------------------------
/**
 * Updates the total mass by adding all hardware masses to the dry mass.
 */
//------------------------------------------------------------------------------
Real Spacecraft::UpdateTotalMass()
{
   #ifdef DEBUG_UPDATE_TOTAL_MASS
   MessageInterface::ShowMessage
      ("Spacecraft::UpdateTotalMass() <%p>'%s' entered, dryMass=%f\n",
       this, GetName().c_str(), dryMass);
   #endif

   totalMass = dryMass;
   for (ObjectArray::iterator i = tanks.begin(); i < tanks.end(); ++i)
   {
      totalMass += (*i)->GetRealParameter("FuelMass");
   }

   #ifdef DEBUG_UPDATE_TOTAL_MASS
   MessageInterface::ShowMessage
      ("Spacecraft::UpdateTotalMass() <%p>'%s' returning %f\n", this,
       GetName().c_str(), totalMass);
   #endif

   return totalMass;
}


//------------------------------------------------------------------------------
//  Real UpdateTotalMass() const
//------------------------------------------------------------------------------
/**
 * Calculates the total mass by adding all hardware masses to the dry mass.
 *
 * This method is const (so const methods can obtain the value), and therefore
 * does not update the internal data member.
 *
 * @return The mass of the spacecraft plus the mass of the fuel in the tanks.
 */
//------------------------------------------------------------------------------
Real Spacecraft::UpdateTotalMass() const
{
   #ifdef DEBUG_UPDATE_TOTAL_MASS
   MessageInterface::ShowMessage
      ("Spacecraft::UpdateTotalMass() const <%p>'%s' entered, dryMass=%f, "
       "attached tank count = %d\n", this, GetName().c_str(), dryMass,
       tanks.size());
   #endif

   Real tmass = dryMass;
   for (ObjectArray::const_iterator i = tanks.begin(); i < tanks.end(); ++i)
   {
      tmass += (*i)->GetRealParameter("FuelMass");
   }

   #ifdef DEBUG_UPDATE_TOTAL_MASS
   MessageInterface::ShowMessage
      ("Spacecraft::UpdateTotalMass() const <%p>'%s' returning %f\n", this,
       GetName().c_str(), tmass);
   #endif

   return tmass;
}


//------------------------------------------------------------------------------
// bool ApplyTotalMass(Real newMass)
//------------------------------------------------------------------------------
/**
 * Adjusts the mass in the fuel tanks, based on the active thrusters, to a new
 * value
 *
 * @param newMass The new total mass
 *
 * @return true if the mass was adjusted to the new value
 *
 * @note This method applies a new total mass after propagating through a
 * maneuver.  This aspect of the design will need to change once multiple tanks
 * are used in maneuvers so that the proportional draw on the tanks is modeled
 * correctly.
 */
//------------------------------------------------------------------------------
bool Spacecraft::ApplyTotalMass(Real newMass)
{
   bool retval = true;
   Real massChange = newMass - UpdateTotalMass();

   #ifdef DEBUG_MASS_FLOW
      MessageInterface::ShowMessage("Mass change = %.12le; depeting ", massChange);
   #endif

   // Find the active thruster(s)
   ObjectArray active;
   RealArray   flowrate;
   Real        totalFlow = 0.0, rate;
   for (ObjectArray::iterator i = thrusters.begin(); i != thrusters.end(); ++i)
   {
      if ((*i)->GetBooleanParameter("IsFiring"))
      {
         active.push_back(*i);
         rate = ((Thruster*)(*i))->CalculateMassFlow();
         flowrate.push_back(rate);
         totalFlow += rate;
      }
   }

   // Divide the mass flow evenly between the tanks on each active thruster
   Real numberFiring = active.size();
   if ((numberFiring <= 0) && (massChange != 0.0))
   {
      std::stringstream errmsg;
      errmsg.precision(15);
      errmsg << "Mass update " << massChange
             << " requested for " << instanceName
             << " but there are no active thrusters";
      throw SpaceObjectException(errmsg.str());
   }

   Real dm;  // = massChange / numberFiring;
   for (UnsignedInt i = 0; i < active.size(); ++i)
   {
      // Change the mass in each attached tank
      ObjectArray usedTanks = active[i]->GetRefObjectArray(Gmat::HARDWARE);
      dm = massChange * flowrate[i] / totalFlow;

      #ifdef DEBUG_MASS_FLOW
         MessageInterface::ShowMessage("%.12le from %s = [ ", dm, active[i]->GetName().c_str());
      #endif

      Real dmt = dm / usedTanks.size();
      for (ObjectArray::iterator j = usedTanks.begin();
            j != usedTanks.end(); ++j)
      {
         #ifdef DEBUG_MASS_FLOW
            MessageInterface::ShowMessage(" %.12le ", dmt);
         #endif
         (*j)->SetRealParameter("FuelMass",
               (*j)->GetRealParameter("FuelMass") + dmt);
      }
      #ifdef DEBUG_MASS_FLOW
               MessageInterface::ShowMessage(" ] ");
      #endif
   }
   #ifdef DEBUG_MASS_FLOW
      MessageInterface::ShowMessage("\n");
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// void DeleteOwnedObjects(bool deleteAttitude, bool deleteTanks, bool deleteThrusters)
//------------------------------------------------------------------------------
/*
 * Deletes owned objects, such as attitude, tanks, and thrusters
 */
//------------------------------------------------------------------------------
void Spacecraft::DeleteOwnedObjects(bool deleteAttitude, bool deleteTanks,
                                    bool deleteThrusters, bool otherHardware)
{
   #ifdef DEBUG_DELETE_OWNED_OBJ
   MessageInterface::ShowMessage
      ("Spacecraft::DeleteOwnedObjects() <%p>'%s' entered, deleteAttitude=%d, "
       "deleteTanks=%d, deleteThrusters=%d, otherHardware=%d\n", this,
       GetName().c_str(), deleteAttitude, deleteTanks, deleteThrusters,
       otherHardware);
   #endif

   // delete attitude
   if (deleteAttitude)
   {
      if (attitude)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (attitude, "attitude", "Spacecraft::DeleteOwnedObjects()",
             "deleting attitude of " + GetName(), this);
         #endif
         delete attitude;
         attitude = NULL;
         ownedObjectCount--;
         #ifdef DEBUG_SC_OWNED_OBJECT
         MessageInterface::ShowMessage
            ("Spacecraft::DeleteOwnedObjects() <%p>'%s' ownedObjectCount=%d\n",
             this, GetName().c_str(), ownedObjectCount);
         #endif
      }
   }

   // delete tanks
   if (deleteTanks)
   {
      for (ObjectArray::iterator i = tanks.begin(); i < tanks.end(); ++i)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (*i, (*i)->GetName(), "Spacecraft::DeleteOwnedObjects()",
             "deleting cloned Tank", this);
         #endif
         delete *i;
      }
      tanks.clear();
   }

   // delete thrusters
   if (deleteThrusters)
   {
      for (ObjectArray::iterator i = thrusters.begin(); i < thrusters.end(); ++i)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (*i, (*i)->GetName(), "Spacecraft::DeleteOwnedObjects()",
             "deleting cloned Thruster", this);
         #endif
         delete *i;
      }
      thrusters.clear();
   }

   // Delete other hardware
   if (otherHardware)
   {
      for (ObjectArray::iterator i = hardwareList.begin();
            i < hardwareList.end(); ++i)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (*i, (*i)->GetName(), "Spacecraft::DeleteOwnedObjects()",
             "deleting cloned Hardware", this);
         #endif
         delete *i;
      }
      hardwareList.clear();
   }

   #ifdef DEBUG_DELETE_OWNED_OBJ
   MessageInterface::ShowMessage
      ("Spacecraft::DeleteOwnedObjects() <%p>'%s' leaving, tanks.size()=%d, "
       "thrusters.size()=%d, hardwareLise.size()=%d\n", this, GetName().c_str(),
       tanks.size(), thrusters.size(), hardwareList.size());
   #endif
}


//------------------------------------------------------------------------------
// void CloneOwnedObjects(Attitude *att, const ObjectArray &tnks, const ObjectArray &thrs)
//------------------------------------------------------------------------------
/*
 * Clones input tanks and thrusters set as attached hardware.
 */
//------------------------------------------------------------------------------
void Spacecraft::CloneOwnedObjects(Attitude *att, const ObjectArray &tnks,
                                   const ObjectArray &thrs)
{
   #ifdef DEBUG_OBJ_CLONE
   MessageInterface::ShowMessage
      ("Spacecraft::CloneOwnedObjects() <%p>'%s' entered, att=<%p>, tank count = %d,"
       " thruster count = %d\n", this, GetName().c_str(), att, tnks.size(), thrs.size());
   #endif


   // clone the attitude
   if (att)
   {
      #ifdef DEBUG_SC_OWNED_OBJECT
      MessageInterface::ShowMessage
         ("Spacecraft::CloneOwnedObjects() ---> cloning attitude <%p> of type %s\n",
          att, att->GetTypeName().c_str());
      #endif
      attitude = NULL;
      attitude = (Attitude*) att->Clone();
      #ifdef DEBUG_SC_OWNED_OBJECT
      MessageInterface::ShowMessage
         ("Spacecraft::CloneOwnedObjects() ------> new CLONED attitude <%p> is of type %s\n",
          attitude, attitude->GetTypeName().c_str());
      #endif
      attitude->SetEpoch(state.GetEpoch());
      attitude->SetOwningSpacecraft(this);
      ownedObjectCount++;
      #ifdef DEBUG_SC_OWNED_OBJECT
      MessageInterface::ShowMessage
         ("Spacecraft::CloneOwnedObjects() <%p>'%s' ownedObjectCount=%d\n",
          this, GetName().c_str(), ownedObjectCount);
      #endif
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (attitude, "cloned attitude", "Spacecraft::CloneOwnedObjects()",
          "attitude = (Attitude*) att->Clone()", this);
      #endif
   }

   // handle tanks
   if (tnks.size() > 0)
   {
      for (UnsignedInt i=0; i<tnks.size(); i++)
      {
         // clone the tanks here
         GmatBase *clonedTank = (tnks[i])->Clone();
         tanks.push_back(clonedTank);
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (clonedTank, clonedTank->GetName(), "Spacecraft::CloneOwnedObjects()",
             "clonedTank = (tnks[i])->Clone()", this);
         #endif
      }
   }

   // handle thrusters
   if (thrs.size() > 0)
   {
      for (UnsignedInt i=0; i<thrs.size(); i++)
      {
         // clone the thrusters here
         GmatBase *clonedObj = (thrs[i])->Clone();
         thrusters.push_back(clonedObj);
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (clonedObj, clonedObj->GetName(), "Spacecraft::CloneOwnedObjects()",
             "clonedObj = (thrs[i])->Clone()", this);
         #endif

         #ifdef DEBUG_OBJ_CLONE
         MessageInterface::ShowMessage
            ("Spacecraft::CloneOwnedObjects() Setting ref objects to "
             "thruster<%p>'%s'\n", clonedObj, clonedObj->GetName().c_str());
         #endif

         // Set ref. objects to cloned Thruster
         clonedObj->SetSolarSystem(solarSystem);
         clonedObj->SetRefObject(this, Gmat::SPACECRAFT, GetName());

         // Set Thruster's CoordinateSystem
         std::string thrCsName = clonedObj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
         if (coordSysMap.find(thrCsName) != coordSysMap.end())
            clonedObj->SetRefObject(coordSysMap[thrCsName], Gmat::COORDINATE_SYSTEM, thrCsName);
      }
   }

   if (tnks.size() > 0 && thrs.size() > 0)
   {
      #ifdef DEBUG_OBJ_CLONE
      MessageInterface::ShowMessage
         ("Spacecraft::CloneOwnedObjects() calling AttachTanksToThrusters()\n");
      #endif
      AttachTanksToThrusters();
   }
}


//--------------------------------------------------------------------
// void AttachTanksToThrusters()
//--------------------------------------------------------------------
void Spacecraft::AttachTanksToThrusters()
{
   #ifdef DEBUG_SC_REF_OBJECT
   MessageInterface::ShowMessage
      ("Spacecraft::AttachTanksToThrusters() this=<%p>'%s' entered\n",
       this, GetName().c_str());
   #endif

   // Attach tanks to thrusters
   StringArray tankNommes;
   GmatBase *tank;

   #ifdef DEBUG_SC_REF_OBJECT
   MessageInterface::ShowMessage
      ("   tanks.size()=%d, thrusters.size()=%d\n", tanks.size(), thrusters.size());
   #endif

   for (ObjectArray::iterator i = thrusters.begin(); i < thrusters.end(); ++i)
   {
      tankNommes = (*i)->GetStringArrayParameter("Tank");

      #ifdef DEBUG_SC_REF_OBJECT
      MessageInterface::ShowMessage
         ("   go through %d tank(s) in the thruster <%p>'%s'\n", tankNommes.size(),
          (*i), (*i)->GetName().c_str());
      #endif

      for (StringArray::iterator j = tankNommes.begin();
           j < tankNommes.end(); ++j)
      {
         // Look up the tank in the hardware list
         #ifdef DEBUG_SC_REF_OBJECT
         MessageInterface::ShowMessage
            ("   Checking to see if '%s' is in the spacecraft tank list\n",
             (*j).c_str());
         #endif
         tank = NULL;
         for (ObjectArray::iterator k = tanks.begin(); k < tanks.end(); ++k)
            if ((*k)->GetName() == *j)
            {
               tank = *k;
               break;
            }

         if (tank)
         {
            #ifdef DEBUG_SC_REF_OBJECT
            MessageInterface::ShowMessage
               ("   Setting the tank <%p>'%s' to the thruster <%p>'%s'\n",
                tank, tank->GetName().c_str(), (*i), (*i)->GetName().c_str());
            #endif
            (*i)->SetRefObject(tank, tank->GetType(), tank->GetName());
         }
         else
            throw SpaceObjectException
               ("Cannot find tank \"" + (*j) + "\" in spacecraft \"" +
                instanceName + "\"\n");
      }
   }

   #ifdef DEBUG_SC_REF_OBJECT
   MessageInterface::ShowMessage
      ("Spacecraft::AttachTanksToThrusters() exiting\n");
   #endif
}


//---------------------------------------------------------------------------
// bool SetHardware(GmatBase *obj, StringArray &hwNames, ObjectArray &hwArray)
//---------------------------------------------------------------------------
bool Spacecraft::SetHardware(GmatBase *obj, StringArray &hwNames,
                             ObjectArray &hwArray)
{
   std::string objType = obj->GetTypeName();
   std::string objName = obj->GetName();

   /// This is a bit kludgy, but preserves the firing state for thrusters
   bool isFiring = false;

   #ifdef DEBUG_SC_REF_OBJECT
   MessageInterface::ShowMessage
      ("Spacecraft::SetHardware() <%p>'%s' entered, obj=<%p>'%s'\n", this,
       GetName().c_str(), obj, objType.c_str(), objName.c_str());
   MessageInterface::ShowMessage
      ("   There are %d hw name(s) and %d hw pointer(s)\n", hwNames.size(), hwArray.size());
   for (UnsignedInt i=0; i<hwNames.size(); i++)
      MessageInterface::ShowMessage("   hwNames[%d] = '%s'\n", i, hwNames[i].c_str());
   for (UnsignedInt i=0; i<hwArray.size(); i++)
      MessageInterface::ShowMessage("   hwArray[%d] = <%p>\n", i, hwArray[i]);
   #endif

   /// @todo The find test here will always reach the end() iterator because
   /// the hardware was cloned, so it ought to be reworked

   // if not adding the same hardware
   if (find(hwArray.begin(), hwArray.end(), obj) == hwArray.end())
   {
      #ifdef DEBUG_SC_REF_OBJECT
      MessageInterface::ShowMessage
         ("   Now checking for the hardware name '%s'\n", objName.c_str());
      #endif

      // if hardware name found
      if (find(hwNames.begin(), hwNames.end(), objName) != hwNames.end())
      {
         #ifdef DEBUG_SC_REF_OBJECT
         MessageInterface::ShowMessage
            ("      The hardware name '%s' found\n", objName.c_str());
         #endif

         for (ObjectArray::iterator i = hwArray.begin(); i != hwArray.end(); ++i)
         {
            #ifdef DEBUG_SC_REF_OBJECT
            MessageInterface::ShowMessage
               ("      Now Checking if '%s' == '%s'\n", (*i)->GetName().c_str(), objName.c_str());
            #endif

            // check if same name found, delete it first since it was cloned
            if ((*i)->GetName() == objName)
            {
               // delete the old one
               GmatBase *old = (*i);
               if (old->IsOfType(Gmat::THRUSTER))
                  isFiring = old->GetBooleanParameter("IsFiring");
               hwArray.erase(i);
               #ifdef DEBUG_MEMORY
               MemoryTracker::Instance()->Remove
                  (old, old->GetName(), "Spacecraft::SetHardware()",
                   "deleting old cloned " + objType, this);
               #endif
               delete old;
               old = NULL;
               break;
            }
         }

         // clone and push the hardware to the list
         GmatBase *clonedObj = obj->Clone();
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (clonedObj, clonedObj->GetName(), "Spacecraft::SetHardware()",
             "GmatBase *cloneObj = obj->Clone()", this);
         #endif

         hwArray.push_back(clonedObj);

         #ifdef DEBUG_SC_REF_OBJECT
         MessageInterface::ShowMessage
            ("      Added cloned hardware <%p><%s>'%s' to list, "
             "hwArray.size()=%d\n", clonedObj, clonedObj->GetTypeName().c_str(),
             clonedObj->GetName().c_str(), hwArray.size());
         #endif

         if (clonedObj->IsOfType("Thruster"))
         {
            #ifdef DEBUG_SC_REF_OBJECT
            MessageInterface::ShowMessage
               ("   Setting ref objects to thruster<%p>'%s'\n",
                clonedObj, clonedObj->GetName().c_str());
            #endif

            // Set SolarSystem and Spacecraft
            clonedObj->SetSolarSystem(solarSystem);
            clonedObj->SetRefObject(this, Gmat::SPACECRAFT, GetName());
            clonedObj->SetBooleanParameter("IsFiring", isFiring);
            // Set CoordinateSystem
            std::string csName;
            csName = clonedObj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
            if (csName != "" && coordSysMap.find(csName) != coordSysMap.end())
            {
               #ifdef DEBUG_SC_REF_OBJECT
               MessageInterface::ShowMessage
                  ("   Setting the CoordinateSystem <%p>'%s' to cloned thruster\n",
                  coordSysMap[csName], csName.c_str() );
               #endif
               clonedObj->SetRefObject(coordSysMap[csName], Gmat::COORDINATE_SYSTEM, csName);
            }
         }
      }
      else
      {
         #ifdef DEBUG_SC_REF_OBJECT
         MessageInterface::ShowMessage
            ("   The name '%s' not found in %s list\n", objName.c_str(),
             objType.c_str());
         #endif
      }
   }
   else
   {
      #ifdef DEBUG_SC_REF_OBJECT
      MessageInterface::ShowMessage
         ("   The same %s <%p> found in %s pointer list\n", objType.c_str(),
          obj, objType.c_str());
      #endif
   }

   #ifdef DEBUG_SC_REF_OBJECT
   MessageInterface::ShowMessage
      ("Spacecraft::SetHardware() <%p>'%s' returning true, total %s count = %d\n",
       this, GetName().c_str(), objType.c_str(), hwArray.size());
   #endif

   return true;
}


//------------------------------------------------------------------------------
// const std::string&  GetGeneratingString(Gmat::WriteMode mode,
//                     const std::string &prefix, const std::string &useName)
//------------------------------------------------------------------------------
/**
 * Produces a string, possibly multi-line, containing the text that produces an
 * object.
 *
 * @param mode Specifies the type of serialization requested.
 * @param prefix Optional prefix appended to the object's name
 * @param useName Name that replaces the object's name.
 *
 * @return A string containing the text.
 */
//------------------------------------------------------------------------------
const std::string& Spacecraft::GetGeneratingString(Gmat::WriteMode mode,
                                                   const std::string &prefix,
                                                   const std::string &useName)
{
   #ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("Spacecraft::GetGeneratingString() <%p><%s>'%s' entered, mode=%d, "
       "prefix='%s', useName='%s', \n", this, GetTypeName().c_str(),
       GetName().c_str(), mode, prefix.c_str(), useName.c_str());
   MessageInterface::ShowMessage
      ("   showPrefaceComment=%d, commentLine=<%s>\n   showInlineComment=%d "
       "inlineComment=<%s>\n",  showPrefaceComment, commentLine.c_str(),
       showInlineComment, inlineComment.c_str());
   #endif

   std::stringstream data;

   data.precision(GetDataPrecision());   // Crank up data precision so we don't lose anything
   std::string preface = "", nomme;

   if ((mode == Gmat::SCRIPTING) || (mode == Gmat::OWNED_OBJECT) ||
       (mode == Gmat::SHOW_SCRIPT))
      inMatlabMode = false;
   if (mode == Gmat::MATLAB_STRUCT || mode == Gmat::EPHEM_HEADER)
      inMatlabMode = true;

   if (useName != "")
      nomme = useName;
   else
      nomme = instanceName;

   if ((mode == Gmat::SCRIPTING) || (mode == Gmat::SHOW_SCRIPT))
   {
      std::string tname = typeName;
      data << "Create " << tname << " " << nomme << ";\n";
      preface = "GMAT ";
   }
   else if (mode == Gmat::EPHEM_HEADER)
   {
      data << typeName << " = " << "'" << nomme << "';\n";
      preface = "";
   }

   nomme += ".";

   if (mode == Gmat::OWNED_OBJECT)
   {
      preface = prefix;
      nomme = "";
   }

   preface += nomme;
   WriteParameters(mode, preface, data);

   generatingString = data.str();

   #ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("==========> <%p>'%s'\n%s\n", this, GetName().c_str(), generatingString.c_str());
   #endif

   #ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("Spacecraft::GetGeneratingString() <%p><%s>'%s' leaving\n",
       this, GetTypeName().c_str(), GetName().c_str());
   #endif

//   return generatingString;

   // Commented out all parameter writings are handled here (LOJ: 2009.11.23)
   // Then call the parent class method for preface and inline comments
   // Added back in to fix issue with no comment header being written before
   // Spacecraft section
   return SpaceObject::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
// void WriteParameters(std::string &prefix, GmatBase *obj,
//                      std::stringstream &stream)
//------------------------------------------------------------------------------
/**
 * Code that writes the parameter details for an object.
 *
 * @param prefix Starting portion of the script string used for the parameter.
 * @param obj    The object that is written.
 * @param stream The stream to which to write
 */
//------------------------------------------------------------------------------
void Spacecraft::WriteParameters(Gmat::WriteMode mode, std::string &prefix,
                                 std::stringstream &stream)
{
   #ifdef DEBUG_WRITE_PARAMETERS
   MessageInterface::ShowMessage("--- Entering SC::WriteParameters ...\n");
   MessageInterface::ShowMessage("--- mode = %d, prefix = %s\n",
   (Integer) mode, prefix.c_str());
   MessageInterface::ShowMessage(" --- stateType = %s, displayStateType = %s\n",
   stateType.c_str(), displayStateType.c_str());
   #endif
   Integer i;
   Gmat::ParameterType parmType;
   std::stringstream value;
   value.precision(GetDataPrecision());

   #ifdef __WRITE_ANOMALY_TYPE__
      bool showAnomaly = false;
      if (stateType == "Keplerian" || stateType == "ModKeplerian")
         showAnomaly = true;
   #endif

   Integer *parmOrder = new Integer[parameterCount];
   Integer parmIndex = 0;

   parmOrder[parmIndex++] = DATE_FORMAT_ID;
   parmOrder[parmIndex++] = SC_EPOCH_ID;
   parmOrder[parmIndex++] = COORD_SYS_ID;
   parmOrder[parmIndex++] = DISPLAY_STATE_TYPE_ID;
   parmOrder[parmIndex++] = ANOMALY_ID;
   parmOrder[parmIndex++] = ELEMENT1_ID;
   parmOrder[parmIndex++] = ELEMENT2_ID;
   parmOrder[parmIndex++] = ELEMENT3_ID;
   parmOrder[parmIndex++] = ELEMENT4_ID;
   parmOrder[parmIndex++] = ELEMENT5_ID;
   parmOrder[parmIndex++] = ELEMENT6_ID;
   parmOrder[parmIndex++] = DRY_MASS_ID;
   parmOrder[parmIndex++] = CD_ID;
   parmOrder[parmIndex++] = CR_ID;
   parmOrder[parmIndex++] = DRAG_AREA_ID;
   parmOrder[parmIndex++] = SRP_AREA_ID;
   parmOrder[parmIndex++] = FUEL_TANK_ID;
   parmOrder[parmIndex++] = THRUSTER_ID;
   parmOrder[parmIndex++] = ORBIT_STM;
   parmOrder[parmIndex++] = ORBIT_A_MATRIX;
   parmOrder[parmIndex++] = ELEMENT1UNIT_ID;
   parmOrder[parmIndex++] = ELEMENT2UNIT_ID;
   parmOrder[parmIndex++] = ELEMENT3UNIT_ID;
   parmOrder[parmIndex++] = ELEMENT4UNIT_ID;
   parmOrder[parmIndex++] = ELEMENT5UNIT_ID;
   parmOrder[parmIndex++] = ELEMENT6UNIT_ID;


   bool registered;
   for (i = 0; i < parameterCount; ++i)
   {
      registered = false;
      for (Integer j = 0; j < parmIndex; ++j)
      {
         if (parmOrder[j] == i)
         {
            registered = true;
            break;
         }
      }
      if (!registered)
         parmOrder[parmIndex++] = i;
   }

   Rvector6 repState = GetStateInRepresentation(displayStateType);

   #ifdef DEBUG_WRITE_PARAMETERS
   MessageInterface::ShowMessage
      ("   anomalyType=%s\n", anomalyType.c_str());
   MessageInterface::ShowMessage
      ("   stateType=%s, repState=%s\n", stateType.c_str(),
       repState.ToString().c_str());
   MessageInterface::ShowMessage
      ("   displayStateType=%s, repState=%s\n", displayStateType.c_str(),
       repState.ToString().c_str());
   #endif
   
   std::string fieldComment;
   std::string inlineFieldComment;
   for (i = 0; i < parameterCount; ++i)
   {
      if ((IsParameterReadOnly(parmOrder[i]) == false) &&
          (parmOrder[i] != J2000_BODY_NAME) &&
          (parmOrder[i] != TOTAL_MASS_ID)   &&
          (parmOrder[i] != STATE_TYPE_ID)   &&         // deprecated
          (parmOrder[i] != ATTITUDE))
      {
         parmType = GetParameterType(parmOrder[i]);
         fieldComment = GetAttributeCommentLine(parmOrder[i]);
         inlineFieldComment = GetInlineAttributeComment(parmOrder[i]);
         
         // Handle StringArray parameters separately
         if (parmType != Gmat::STRINGARRAY_TYPE &&
             parmType != Gmat::OBJECTARRAY_TYPE)
         {
            // Skip unhandled types
            if (
                (parmType != Gmat::UNSIGNED_INTARRAY_TYPE) &&
                (parmType != Gmat::RVECTOR_TYPE) &&
//                (parmType != Gmat::RMATRIX_TYPE) &&
                (parmType != Gmat::UNKNOWN_PARAMETER_TYPE)
               )
            {
               // Fill in the l.h.s.
               value.str("");
               if ((parmOrder[i] >= ELEMENT1_ID) &&
                   (parmOrder[i] <= ELEMENT6_ID))
               {
                  #ifdef DEBUG_WRITE_PARAMETERS
                  MessageInterface::ShowMessage("--- parmOrder[i] = %d,",
                  (Integer) parmOrder[i]);
                  MessageInterface::ShowMessage(" --- and that is for element %s\n",
                  (GetParameterText(parmOrder[i])).c_str());
                  #endif
                  value.precision(GetDataPrecision());
                  value << repState[parmOrder[i] - ELEMENT1_ID];
                  value.precision(GetDataPrecision());
               }
               else if (parmOrder[i] == DISPLAY_STATE_TYPE_ID)
               {
                  if (mode != Gmat::MATLAB_STRUCT)
                     value << displayStateType;
                  else
                     value << "'" << displayStateType << "'";
               }
               else if (parmOrder[i] == ANOMALY_ID)
               {
                  #ifdef __WRITE_ANOMALY_TYPE__
                  if (showAnomaly)
                  {
                     if (mode != Gmat::MATLAB_STRUCT)
                        value << anomalyType;
                     else
                        value << "'" << anomalyType << "'";
                  }
                  #endif
               }
               else
               {
                  #ifdef DEBUG_WRITE_PARAMETERS
                  MessageInterface::ShowMessage(
                  "--- about to call WriteParameterValue with parmOrder[i] = %d\n",
                  (Integer) parmOrder[i]);
                  MessageInterface::ShowMessage("--- and the string associated with it is %s\n",
                  GetParameterText(parmOrder[i]).c_str());
                  #endif
                  WriteParameterValue(parmOrder[i], value);
               }

               #ifdef DEBUG_WRITE_PARAMETERS
               MessageInterface::ShowMessage
                  ("   <%s> = <%s>\n", GetParameterText(parmOrder[i]).c_str(), value.str().c_str());
               #endif
               if (value.str() != "")
               {
                  // Write inline attribute comment (LOJ: 2013.03.01 for GMT-3353 fix)
                  //stream << prefix << GetParameterText(parmOrder[i])
                  //       << " = " << value.str() << ";\n";
                  stream << fieldComment << prefix << GetParameterText(parmOrder[i])
                         << " = " << value.str() << ";" << inlineFieldComment << "\n";
               }
            }
         }
         else
         {
            bool writeQuotes = inMatlabMode || parmType == Gmat::STRINGARRAY_TYPE;

            // Handle StringArrays
            StringArray sar = GetStringArrayParameter(parmOrder[i]);
            if (sar.size() > 0)
            {
               stream << prefix << GetParameterText(parmOrder[i]) << " = {";
               for (StringArray::iterator n = sar.begin(); n != sar.end(); ++n)
               {
                  if (n != sar.begin())
                     stream << ", ";
                  if (writeQuotes)
                     stream << "'";
                  stream << (*n);
                  if (writeQuotes)
                     stream << "'";
               }
               // Write inline attribute comment (LOJ: 2013.03.01 for GMT-3353 fix)
               //stream << "};\n";
               stream << "};" << inlineFieldComment << "\n";
            }
         }
      }
      // handle ATTITUDE differently
      else if (parmOrder[i] == ATTITUDE)
      {
         if (attitude)
         {
            if (inMatlabMode)
               stream << prefix << "Attitude = '" << attitude->GetAttitudeModelName() << "';" << inlineFieldComment << "\n";
            else
               stream << prefix << "Attitude = " << attitude->GetAttitudeModelName() << ";" << inlineFieldComment << "\n";
         }
         else
         {
            MessageInterface::ShowMessage
               ("*** INTERNAL ERROR *** attitude is NULL\n");
         }
      }
   }
   
   // Prep in case spacecraft "own" the attached hardware
   GmatBase *ownedObject;
   std::string nomme, newprefix;

   #ifdef DEBUG_OWNED_OBJECT_STRINGS
      MessageInterface::ShowMessage("\"%s\" has %d owned objects\n",
         instanceName.c_str(), GetOwnedObjectCount());
   #endif

   // @note Currently only attitude is considered owned object.
   // The properties of hardware such as tanks and thrusters are
   // not written out (LOJ: 2009.09.14)
   for (i = 0; i < GetOwnedObjectCount(); ++i)
   {
      newprefix   = prefix;
      ownedObject = GetOwnedObject(i);
      nomme       = ownedObject->GetName();

      #ifdef DEBUG_OWNED_OBJECT_STRINGS
          MessageInterface::ShowMessage(
             "   index %d <%p> has type %s and name \"%s\"\n",
             i, ownedObject, ownedObject->GetTypeName().c_str(),
             ownedObject->GetName().c_str());
      #endif

      if (nomme != "")
         newprefix += nomme + ".";
      //else if (GetType() == Gmat::FORCE_MODEL)  wcs - why is this here? GetType on s/c?
      //   newprefix += ownedObject->GetTypeName();
      stream << ownedObject->GetGeneratingString(Gmat::OWNED_OBJECT, newprefix);
   }

   delete [] parmOrder;
}


//------------------------------------------------------------------------------
// void UpdateElementLabels()
//------------------------------------------------------------------------------
/**
 * Code used to set the element labels.
 */
//------------------------------------------------------------------------------
void Spacecraft::UpdateElementLabels()
{
	UpdateElementLabels(displayStateType);
}

//------------------------------------------------------------------------------
// void UpdateElementLabels()
//------------------------------------------------------------------------------
/**
 * Code used to set the element labels.
 */
//------------------------------------------------------------------------------
void Spacecraft::UpdateElementLabels(std::string displayStateType)
{
   //if (stateType == "Cartesian")
   if (displayStateType == "Cartesian")
   {
      stateElementLabel[0] = "X";
      stateElementLabel[1] = "Y";
      stateElementLabel[2] = "Z";
      stateElementLabel[3] = "VX";
      stateElementLabel[4] = "VY";
      stateElementLabel[5] = "VZ";

      stateElementUnits[0] = "km";
      stateElementUnits[1] = "km";
      stateElementUnits[2] = "km";
      stateElementUnits[3] = "km/s";
      stateElementUnits[4] = "km/s";
      stateElementUnits[5] = "km/s";

      return;
   }

   //if (stateType == "Keplerian")
   if (displayStateType == "Keplerian")
   {
      stateElementLabel[0] = "SMA";
      stateElementLabel[1] = "ECC";
      stateElementLabel[2] = "INC";
      stateElementLabel[3] = "RAAN";
      stateElementLabel[4] = "AOP";
      stateElementLabel[5] = anomalyType;

      stateElementUnits[0] = "km";
      stateElementUnits[1] = "";
      stateElementUnits[2] = "deg";
      stateElementUnits[3] = "deg";
      stateElementUnits[4] = "deg";
      stateElementUnits[5] = "deg";

      return;
   }

   //if (stateType == "ModifiedKeplerian")
   if (displayStateType == "ModifiedKeplerian")
   {
      stateElementLabel[0] = "RadPer";
      stateElementLabel[1] = "RadApo";
      stateElementLabel[2] = "INC";
      stateElementLabel[3] = "RAAN";
      stateElementLabel[4] = "AOP";
      stateElementLabel[5] = anomalyType;

      stateElementUnits[0] = "km";
      stateElementUnits[1] = "km";
      stateElementUnits[2] = "deg";
      stateElementUnits[3] = "deg";
      stateElementUnits[4] = "deg";
      stateElementUnits[5] = "deg";

      return;
   }

   //if (stateType == "SphericalAZFPA")
   if (displayStateType == "SphericalAZFPA")
   {
      stateElementLabel[0] = "RMAG";
      stateElementLabel[1] = "RA";
      stateElementLabel[2] = "DEC";
      stateElementLabel[3] = "VMAG";
      stateElementLabel[4] = "AZI";
      stateElementLabel[5] = "FPA";

      stateElementUnits[0] = "km";
      stateElementUnits[1] = "deg";
      stateElementUnits[2] = "deg";
      stateElementUnits[3] = "km/s";
      stateElementUnits[4] = "deg";
      stateElementUnits[5] = "deg";

      return;
   }

   //if (stateType == "SphericalRADEC")
   if (displayStateType == "SphericalRADEC")
   {
      stateElementLabel[0] = "RMAG";
      stateElementLabel[1] = "RA";
      stateElementLabel[2] = "DEC";
      stateElementLabel[3] = "VMAG";
      stateElementLabel[4] = "RAV";
      stateElementLabel[5] = "DECV";

      stateElementUnits[0] = "km";
      stateElementUnits[1] = "deg";
      stateElementUnits[2] = "deg";
      stateElementUnits[3] = "km/s";
      stateElementUnits[4] = "deg";
      stateElementUnits[5] = "deg";

      return;
   }

   //if (stateType == "Equinoctial")
   if (displayStateType == "Equinoctial")
   {
      stateElementLabel[0] = "SMA";
      stateElementLabel[1] = "EquinoctialH";
      stateElementLabel[2] = "EquinoctialK";
      stateElementLabel[3] = "EquinoctialP";
      stateElementLabel[4] = "EquinoctialQ";
//      stateElementLabel[1] = "h";
//      stateElementLabel[2] = "k";
//      stateElementLabel[3] = "p";
//      stateElementLabel[4] = "q";
      stateElementLabel[5] = "MLONG";

      stateElementUnits[0] = "km";
      stateElementUnits[1] = "";
      stateElementUnits[2] = "";
      stateElementUnits[3] = "";
      stateElementUnits[4] = "";
      stateElementUnits[5] = "deg";

      return;
   }

   //if (stateType == "ModifiedEquinoctial") ; Modified by M.H.
   if (displayStateType == "ModifiedEquinoctial")
   {
      stateElementLabel[0] = "SemiLatusRectum";
      stateElementLabel[1] = "ModEquinoctialF";
      stateElementLabel[2] = "ModEquinoctialG";
      stateElementLabel[3] = "ModEquinoctialH";
      stateElementLabel[4] = "ModEquinoctialK";
      stateElementLabel[5] = "TLONG";

      stateElementUnits[0] = "km";
      stateElementUnits[1] = "";
      stateElementUnits[2] = "";
      stateElementUnits[3] = "";
      stateElementUnits[4] = "";
      stateElementUnits[5] = "deg";

      return;
   }
   
   //if (stateType == "Delaunay") ; Modified by M.H.
   if (displayStateType == "Delaunay")
   {
      stateElementLabel[0] = "Delaunayl";
      stateElementLabel[1] = "Delaunayg";
      stateElementLabel[2] = "DelaunayH";
      stateElementLabel[3] = "DelaunayL";
      stateElementLabel[4] = "DelaunayG";
      stateElementLabel[5] = "DelaunayH";

      stateElementUnits[0] = "deg";
      stateElementUnits[1] = "deg";
      stateElementUnits[2] = "deg";
      stateElementUnits[3] = "km^2/sec";
      stateElementUnits[4] = "km^2/sec";
      stateElementUnits[5] = "km^2/sec";

      return;
   }
   
   //if (stateType == "Planetodetic") ; Modified by M.H.
   if (displayStateType == "Planetodetic")
   {
      stateElementLabel[0] = "PlanetodeticRMAG";
      stateElementLabel[1] = "PlanetodeticLON";
      stateElementLabel[2] = "PlanetodeticLAT";
      stateElementLabel[3] = "PlanetodeticVMAG";
      stateElementLabel[4] = "PlanetodeticAZI";
      stateElementLabel[5] = "PlanetodeticHFPA";

      stateElementUnits[0] = "km";
      stateElementUnits[1] = "deg";
      stateElementUnits[2] = "deg";
      stateElementUnits[3] = "km/sec";
      stateElementUnits[4] = "deg";
      stateElementUnits[5] = "deg";

      return;
   }
}


//------------------------------------------------------------------------------
// Rvector6 GetStateInRepresentation(std::string rep, bool useDefaultCartesian = false)
//------------------------------------------------------------------------------
/**
 * Code used to obtain a state in a non-Cartesian representation.
 */
//------------------------------------------------------------------------------
Rvector6 Spacecraft::GetStateInRepresentation(std::string rep, bool useDefaultCartesian)
{
   #ifdef DEBUG_STATE_INTERFACE
      MessageInterface::ShowMessage("Spacecraft::GetStateInRepresentation(string): Constructing %s state\n", rep.c_str());
      MessageInterface::ShowMessage("Spacecraft::GetStateInRepresentation(string): using defaultCartesian = %s\n",
            (useDefaultCartesian? "true" : "false"));
   #endif

   Rvector6 csState;
   Rvector6 finalState;

   // First convert from the internal CS to the state CS
   if (internalCoordSystem != coordinateSystem)
   {
      Rvector6 inState(state.GetState());
      // if the whole state has not been set yet, we need to use the default coordinate system
      if (useDefaultCartesian)
         inState = defaultCartesian;

      #ifdef DEBUG_STATE_INTERFACE
         MessageInterface::ShowMessage("Spacecraft::GetStateInRepresentation(string): Using inState  = %s\n", inState.ToString().c_str());
         MessageInterface::ShowMessage("Spacecraft::GetStateInRepresentation(string): Now converting the inState from %s to %s\n", internalCoordSystem->GetName().c_str(),
               coordinateSystem->GetName().c_str());
      #endif

      coordConverter.Convert(GetEpoch(), inState, internalCoordSystem, csState,
         coordinateSystem);
      #ifdef DEBUG_STATE_INTERFACE
      MessageInterface::ShowMessage("inState successfully converted from %s to %s\n", internalCoordSystem->GetName().c_str(),
            coordinateSystem->GetName().c_str());
         MessageInterface::ShowMessage("Spacecraft::GetStateInRepresentation(string): state in %s  = %s\n", coordinateSystem->GetName().c_str(),
               csState.ToString().c_str());
      #endif
   }
   else
   {
      if (useDefaultCartesian)
         csState = defaultCartesian;
      else
         csState.Set(state.GetState());
   }

   // Then convert to the desired representation
   if (rep == "")
      rep = stateType;   // do I want displayStateType here?

   if (rep == "Cartesian")
   {
      finalState = csState;
      #ifdef DEBUG_STATE_INTERFACE
         MessageInterface::ShowMessage(
            "Spacecraft::GetStateInRepresentation(string): type is Cartesian, so no conversion done\n");
      #endif
   }
   else
   {
      #ifdef DEBUG_STATE_INTERFACE
         MessageInterface::ShowMessage("Spacecraft::GetStateInRepresentation(string): type is %s, so calling StateConversionUtil to convert\n",
            rep.c_str());
         MessageInterface::ShowMessage("Spacecraft::GetStateInRepresentation(string): Using originMu = %12.10f\n", originMu);
      #endif
      finalState = StateConversionUtil::Convert(csState, "Cartesian", rep,
                   originMu, originFlattening, originEqRadius, anomalyType);
   }

   #ifdef DEBUG_STATE_INTERFACE
      MessageInterface::ShowMessage(
         "Spacecraft::GetStateInRepresentation(string): %s state is "
         "[%.9lf %.9lf %.9lf %.14lf %.14lf %.14lf]\n",
         rep.c_str(), finalState[0], finalState[1],
         finalState[2], finalState[3], finalState[4],
         finalState[5]);
   #endif

   return finalState;
}


//------------------------------------------------------------------------------
// Rvector6 GetStateInRepresentation(Integer rep, bool useDefaultCartesian = false)
//------------------------------------------------------------------------------
/**
 * Code used to obtain a state in a non-Cartesian representation.
 */
//------------------------------------------------------------------------------
Rvector6 Spacecraft::GetStateInRepresentation(Integer rep, bool useDefaultCartesian)
{
   return GetStateInRepresentation(representations[rep], useDefaultCartesian);
}


//------------------------------------------------------------------------------
// void SetStateFromRepresentation(std::string rep, Rvector6 &st)
//------------------------------------------------------------------------------
/**
 * Code used to obtain a state in a non-Cartesian representation.
 */
//------------------------------------------------------------------------------
void Spacecraft::SetStateFromRepresentation(std::string rep, Rvector6 &st)
{
   #ifdef DEBUG_STATE_INTERFACE
      MessageInterface::ShowMessage(
         "Spacecraft::SetStateFromRepresentation: Setting %s state to %s\n",
         rep.c_str(), st.ToString(16).c_str());
   #endif

   // First convert from the representation to Cartesian
   static Rvector6 csState, finalState;

   if (rep == "Cartesian")
      csState = st;
   else
   {
      #ifdef DEBUG_STATE_INTERFACE
      MessageInterface::ShowMessage
         ("   rep is not Cartesian, so calling StateConversionUtil::Convert()\n");
      #endif
      csState = StateConversionUtil::Convert(st, rep, "Cartesian",
                originMu, originFlattening, originEqRadius, anomalyType);
   }

   #ifdef DEBUG_STATE_INTERFACE
      MessageInterface::ShowMessage(
         "Spacecraft::SetStateFromRepresentation: state has been converted\n");
   #endif

   if (internalCoordSystem == NULL)
      throw SpaceObjectException(" The spacecraft internal coordinate system is not set");
   if (coordinateSystem == NULL)
      throw SpaceObjectException(" The spacecraft coordinate system is not set");

   #ifdef DEBUG_STATE_INTERFACE
   MessageInterface::ShowMessage
      ("   Now convert to internal CS, internalCoordSystem=<%p>, coordinateSystem=<%p>\n",
       internalCoordSystem, coordinateSystem);
   #endif

   // Then convert to the internal CS
   if (internalCoordSystem != coordinateSystem)
   {
      #ifdef DEBUG_STATE_INTERFACE
      MessageInterface::ShowMessage
         ("   cs is not InternalCS, so calling coordConverter.Convert() at epoch %f\n",
          GetEpoch());
      #endif
      coordConverter.Convert(GetEpoch(), csState, coordinateSystem, finalState,
         internalCoordSystem);
   }
   else
      finalState = csState;

   for (int i=0; i<6; i++)
      state[i] = finalState[i];

   #ifdef DEBUG_STATE_INTERFACE
      MessageInterface::ShowMessage(
         "Spacecraft::SetStateFromRepresentation: Cartesian State is now\n   "
         "%.9lf %.9lf %.9lf %.14lf %.14lf %.14lf\n", state[0], state[1],
         state[2], state[3], state[4], state[5]);
   #endif
}


//------------------------------------------------------------------------------
// Real Spacecraft::GetElement(const std::string &label)
//------------------------------------------------------------------------------
/**
 * Code used to obtain a state element.
 *
 * @param <label> The test label for the element.
 *
 * @return The element's value, or -9999999999.999999 on failure.
 */
//------------------------------------------------------------------------------
Real Spacecraft::GetElement(const std::string &label)
{
   #ifdef DEBUG_GET_REAL
      MessageInterface::ShowMessage(
      "In SC::GetElement, asking for parameter %s\n", label.c_str());
   #endif
   Integer baseID;
   std::string rep = "";
   baseID = LookUpLabel(label,rep);
   #ifdef DEBUG_GET_REAL
   MessageInterface::ShowMessage(
   "In SC::GetElement, after LookUpLabel, id = %d, its string = \"%s\",  and rep = \"%s\"\n",
   baseID, (GetParameterText(baseID)).c_str(), rep.c_str());
   #endif
   Rvector6 stateInRep = GetStateInRepresentation(rep);
   #ifdef DEBUG_GET_REAL
      MessageInterface::ShowMessage(
      "In SC::GetElement, stateInRep = \n");
      for (Integer jj=0;jj<6;jj++)
         MessageInterface::ShowMessage("    %.12f\n", stateInRep[jj]);
   #endif
   // check for Anomaly data first
   if (label == "TA" || label == "EA" ||
       label == "MA" || label == "HA")
   {
      Real ta = StateConversionUtil::ConvertToTrueAnomaly(anomalyType, stateInRep[5], stateInRep[1]);
      return StateConversionUtil::ConvertFromTrueAnomaly(label, ta, stateInRep[1]);
   }
   else
   {
      if (baseID == ELEMENT1_ID) return stateInRep[0];
      if (baseID == ELEMENT2_ID) return stateInRep[1];
      if (baseID == ELEMENT3_ID) return stateInRep[2];
      if (baseID == ELEMENT4_ID) return stateInRep[3];
      if (baseID == ELEMENT5_ID) return stateInRep[4];
      if (baseID == ELEMENT6_ID) return stateInRep[5];
   }

   return -9999999999.999999;  // some kind of error
}


//------------------------------------------------------------------------------
// bool SetElement(const std::string &label, const Real &value)
//------------------------------------------------------------------------------
/**
 * Set a state element.
 *
 * @param <label> Label for the element -- 'X', 'Y', 'SMA', etc.
 * @param <value> New value for the element.
 *
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool Spacecraft::SetElement(const std::string &label, const Real &value)
{
   #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
   MessageInterface::ShowMessage
      ("In SC::SetElement, <%p> '%s', label=%s, value=%.12f\n", this,
       GetName().c_str(), label.c_str(), value);
   #endif
   std::string rep = "";
   Integer id = LookUpLabel(label, rep) - ELEMENT1_ID;

   #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
      MessageInterface::ShowMessage
         (" ************ In SC::SetElement, after LookUpLabel, ELEMENT1_ID = %d, id = %d, rep = %s, stateType = %s\n",
          ELEMENT1_ID, id, rep.c_str(), stateType.c_str());
   #endif

   // Determine if type really changed
   if (rep != "")
   {
      if (stateType != rep)
      {
         // 2007.05.24 - wcs - Bug 875 - because some elements are the same for
         // Keplerian and ModifiedKeplerian, make sure it only changes when it should
         if ( (stateType == "ModifiedKeplerian") && (rep == "Keplerian") &&
            (label != "SMA") && (label != "ECC") )
         {
            #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
               MessageInterface::ShowMessage
                  (" ************ In SC::SetElement, leaving stateType as ModifiedKeplerian\n");
            #endif
            // leave stateType as ModifiedKeplerian
         }
         else if ( (stateType == "SphericalRADEC") && (rep == "SphericalAZFPA") &&
                  (label != "AZI") && (label != "FPA") )
         {
            #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
               MessageInterface::ShowMessage
                  (" ************ In SC::SetElement, leaving stateType as SphericalRADEC\n");
            #endif
            // leave it as SphericalRADEC
         }
         /// 2010.03.22 - wcs - SMA could also be Equinoctial
         else if ( (stateType == "Equinoctial") && (rep == "Keplerian") &&
                  (label == "SMA") )
            // leave type as Equinoctial
         {
            #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
               MessageInterface::ShowMessage
                  (" ************ In SC::SetElement, leaving stateType as Equinoctial\n");
            #endif
            // leave state as Equinoctial
         }
         else
         {
            #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
               MessageInterface::ShowMessage
                  (" ************ In SC::SetElement, MODIFYING stateType from %s to %s\n",
                    stateType.c_str(), rep.c_str());
            #endif
            stateType = rep;
            // Check to see if the stateType requires a coordinate system with a Celestial Body
            // origin and if so, if the coordinate system meets that criterion
            bool needsCBOrigin = StateConversionUtil::RequiresCelestialBodyOrigin(stateType);
            if (needsCBOrigin && coordinateSystem && !coordinateSystem->HasCelestialBodyOrigin())
            {
               std::string errmsg = "The Spacecraft \"";
               errmsg += instanceName + "\" failed to set the orbit state because the state type is \"";
               errmsg += stateType + "\" and coordinate system \"";
               errmsg += coordinateSystem->GetName() + "\" does not have a celestial body at the origin.\n";
               throw SpaceObjectException(errmsg);
            }
         }
      }
      // Has the state type has been fully determined or not?  Only worry about this before the CS has been applied
      if (!csSet)
         SetPossibleInputTypes(label, stateType);

      // Get the true anomaly if needed
      if ((stateType == "Keplerian") || (stateType == "ModifiedKeplerian"))
      {
         // Grab trueAnomaly
         Rvector6 kep = GetStateInRepresentation("Keplerian");
         trueAnomaly = kep[5];
      }
   }


   #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
   if (id >= 0)
      MessageInterface::ShowMessage
         ("In SC::SetElement, after LookUpLabel, id+ELEMENT1_ID = %d, its "
          "string = \"%s\",  and rep = \"%s\"\n", id+ELEMENT1_ID,
          (GetParameterText(id+ELEMENT1_ID)).c_str(), rep.c_str());
      MessageInterface::ShowMessage
         ("In SC::SetElement, after LookUpLabel, its label = \"%s\" and its value = %12.10f\n",
               label.c_str(), value);

   #endif

   // parabolic and hyperbolic orbits not yet supported
   if ((label == "ECC") && value == 1.0)
   {
      SpaceObjectException se;
      se.SetDetails(errorMessageFormat.c_str(),
                    GmatStringUtil::ToString(value, GetDataPrecision()).c_str(),
                    "Eccentricity", "Real Number != 1.0");
      throw se;
   }
   // Equinoctial elements must be within bounds
   if (((label == "EquinoctialH") || (label == "EquinoctialK")) &&
       ((value < -1.0) || (value > 1.0)))
   {
      #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
         MessageInterface::ShowMessage
            ("In SC::SetElement, reached where the exception should be thrown!!!!\n");
      #endif
      SpaceObjectException se;
      se.SetDetails(errorMessageFormat.c_str(),
                    GmatStringUtil::ToString(value, GetDataPrecision()).c_str(),
                    label.c_str(), "-1.0 < Real Number < 1.0");
      throw se;
   }

   if ((id == 5) && (StateConversionUtil::IsValidAnomalyType(label)))
//      trueAnomaly.SetType(label);
      anomalyType = label;  // is this right?

   if (id >= 0)
   {
      // Only validate coupled elements in Assignment mode (i.e. before initialization).
      // Assume that errors in setting coupled elements in Command mode will be
      // caught when state conversion is requested.
      bool isValid = ValidateOrbitStateValue(rep, label, value, !isInitialized);

      if (isValid)
      {
         if (csSet)
         {
            #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
               MessageInterface::ShowMessage("In SC::SetElement, csSet = TRUE\n");
            #endif
            Rvector6 tempState = GetStateInRepresentation(rep);
            tempState[id] = value;
            SetStateFromRepresentation(rep, tempState);
         }
         else
         {
            #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
               MessageInterface::ShowMessage("In SC::SetElement, csSet = FALSE\n");
            #endif
            Real *tempState = state.GetState();
            tempState[id] = value;
         }

         #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
         Rvector6 vec6(state.GetState());
         MessageInterface::ShowMessage
            ("   CS was %sset, state is now\n   %s \n", (csSet ? "" : "NOT "),
             vec6.ToString().c_str());
         MessageInterface::ShowMessage
            ("In SC::SetElement, '%s', returning TRUE\n", GetName().c_str());
         #endif
         return true;
      }
   }

   #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
   MessageInterface::ShowMessage("In SC::SetElement, returning FALSE\n");
   #endif
   return false;
}


//------------------------------------------------------------------------------
// void LookUpLabel(std::string rep, Rvector6 &st)
//------------------------------------------------------------------------------
/**
 * Code used to obtain a state in a non-Cartesian representation.
 */
//------------------------------------------------------------------------------
Integer Spacecraft::LookUpLabel(const std::string &label, std::string &rep)
{
   #ifdef DEBUG_LOOK_UP_LABEL
      MessageInterface::ShowMessage("Spacecraft::LookUpLabel(%s)called\n",
         label.c_str());
   #endif
   Integer retval = -1;

   if (label == "Element1")
   {
      rep = stateType;
      return ELEMENT1_ID;
   }
   if (label == "Element2")
   {
      rep = stateType;
      return ELEMENT2_ID;
   }
   if (label == "Element3")
   {
      rep = stateType;
      return ELEMENT3_ID;
   }
   if (label == "Element4")
   {
      rep = stateType;
      return ELEMENT4_ID;
   }
   if (label == "Element5")
   {
      rep = stateType;
      return ELEMENT5_ID;
   }
   if (label == "Element6")
   {
      rep = stateType;
      return ELEMENT6_ID;
   }

   // Modified by M.H.
   if (label == "X" || label == "SMA" || label == "RadPer" || label == "RMAG" ||
	   label == "SemiLatusRectum" || label == "Delaunayl" || label == "PlanetodeticRMAG" )
      retval = ELEMENT1_ID;

   else if (label == "Y" || label == "ECC" || label == "RadApo" || label == "RA" ||
            label == "PEY" || label == "EquinoctialH" || label == "ModEquinoctialF" ||
            label == "Delaunayg" || label == "PlanetodeticLON" )
      retval = ELEMENT2_ID;

   else if (label == "Z" || label == "INC" || label == "DEC" || label == "PEX" ||
            label == "EquinoctialK" || label == "ModEquinoctialG" || label == "Delaunayh" ||
            label == "PlanetodeticLAT")
      retval = ELEMENT3_ID;

   else if (label == "VX" || label == "RAAN" || label == "VMAG" || label == "PNY" ||
            label == "EquinoctialP" || label == "ModEquinoctialH" || label == "DelaunayL" ||
            label == "PlanetodeticVMAG")
      retval = ELEMENT4_ID;

   else if (label == "VY" || label == "AOP" || label == "AZI" || label == "RAV" ||
            label == "PNX" || label == "EquinoctialQ" || label == "ModEquinoctialK" ||
            label == "DelaunayG" || label == "PlanetodeticAZI")
      retval = ELEMENT5_ID;

   else if (label == "VZ" || StateConversionUtil::IsValidAnomalyType(label) ||
            label == "FPA" || label == "DECV" || label == "MLONG" || label == "TLONG" ||
            label == "DelaunayH" || label == "PlanetodeticHFPA")
      retval = ELEMENT6_ID;

   rep = elementLabelMap[label];

   #ifdef DEBUG_LOOK_UP_LABEL
      MessageInterface::ShowMessage("Spacecraft::LookUpLabel(%s..) gives rep %s with retval = %d\n",
         label.c_str(), rep.c_str(), retval);
   #endif

   return retval;
}

//------------------------------------------------------------------------------
// void BuildElementLabelMap()
//------------------------------------------------------------------------------
/**
 * Set the mapping between elements and representations.
 */
//------------------------------------------------------------------------------
void Spacecraft::BuildElementLabelMap()
{
   if (elementLabelMap.size() == 0)
   {
      elementLabelMap["X"]  = "Cartesian";
      elementLabelMap["Y"]  = "Cartesian";
      elementLabelMap["Z"]  = "Cartesian";
      elementLabelMap["VX"] = "Cartesian";
      elementLabelMap["VY"] = "Cartesian";
      elementLabelMap["VZ"] = "Cartesian";

      elementLabelMap["SMA"]  = "Keplerian";
      elementLabelMap["ECC"]  = "Keplerian";
      elementLabelMap["INC"]  = "Keplerian";
      elementLabelMap["RAAN"] = "Keplerian";
      elementLabelMap["AOP"]  = "Keplerian";
      elementLabelMap["TA"]   = "Keplerian";
      elementLabelMap["EA"]   = "Keplerian";
      elementLabelMap["MA"]   = "Keplerian";
      elementLabelMap["HA"]   = "Keplerian";

      elementLabelMap["RadPer"] = "ModifiedKeplerian";
      elementLabelMap["RadApo"] = "ModifiedKeplerian";

      elementLabelMap["RMAG"] = "SphericalAZFPA";
      elementLabelMap["RA"]   = "SphericalAZFPA";
      elementLabelMap["DEC"]  = "SphericalAZFPA";
      elementLabelMap["VMAG"] = "SphericalAZFPA";
      elementLabelMap["AZI"]  = "SphericalAZFPA";
      elementLabelMap["FPA"]  = "SphericalAZFPA";

      elementLabelMap["RAV"]  = "SphericalRADEC";
      elementLabelMap["DECV"] = "SphericalRADEC";

      elementLabelMap["EquinoctialH"] = "Equinoctial";
      elementLabelMap["EquinoctialK"] = "Equinoctial";
      elementLabelMap["EquinoctialP"] = "Equinoctial";
      elementLabelMap["EquinoctialQ"] = "Equinoctial";
      elementLabelMap["MLONG"]        = "Equinoctial";

      // Modified by M.H.
      elementLabelMap["SemiLatusRectum"] = "ModifiedEquinoctial";
      elementLabelMap["ModEquinoctialF"] = "ModifiedEquinoctial";
      elementLabelMap["ModEquinoctialG"] = "ModifiedEquinoctial";
      elementLabelMap["ModEquinoctialH"] = "ModifiedEquinoctial";
      elementLabelMap["ModEquinoctialK"] = "ModifiedEquinoctial";
      elementLabelMap["TLONG"]           = "ModifiedEquinoctial";

      elementLabelMap["Delaunayl"] = "Delaunay";
      elementLabelMap["Delaunayg"] = "Delaunay";
      elementLabelMap["Delaunayh"] = "Delaunay";
      elementLabelMap["DelaunayL"] = "Delaunay";
      elementLabelMap["DelaunayG"] = "Delaunay";
      elementLabelMap["DelaunayH"] = "Delaunay";
      
      elementLabelMap["PlanetodeticRMAG"] = "Planetodetic";
      elementLabelMap["PlanetodeticLON"]  = "Planetodetic";
      elementLabelMap["PlanetodeticLAT"]  = "Planetodetic";
      elementLabelMap["PlanetodeticVMAG"] = "Planetodetic";
      elementLabelMap["PlanetodeticAZI"]  = "Planetodetic";
      elementLabelMap["PlanetodeticHFPA"] = "Planetodetic";
   }
}

//-------------------------------------------------------------------------
// bool HasDynamicParameterSTM(Integer parameterId)
//-------------------------------------------------------------------------
bool Spacecraft::HasDynamicParameterSTM(Integer parameterId)
{
   if (parameterId == CARTESIAN_X)
      return true;
   return SpaceObject::HasDynamicParameterSTM(parameterId);
}

//-------------------------------------------------------------------------
// Rmatrix* GetParameterSTM(Integer parameterId)
//-------------------------------------------------------------------------
Rmatrix* Spacecraft::GetParameterSTM(Integer parameterId)
{
   if (parameterId == CARTESIAN_X)
      return &orbitSTM;
   return SpaceObject::GetParameterSTM(parameterId);
}

//-------------------------------------------------------------------------
// Integer Spacecraft::HasParameterCovariances(Integer parameterId)
//-------------------------------------------------------------------------
Integer Spacecraft::HasParameterCovariances(Integer parameterId)
{
   if (parameterId == CARTESIAN_X)
      return 6;
   return SpaceObject::HasParameterCovariances(parameterId);
}

// Additions for the propagation rework

//-------------------------------------------------------------------------
// void Spacecraft::RecomputeStateAtEpoch(const GmatEpoch &toEpoch)
//-------------------------------------------------------------------------
void Spacecraft::RecomputeStateAtEpoch(const GmatEpoch &toEpoch)
{
   if (internalCoordSystem != coordinateSystem)
   {
      // First convert from the internal CS to the state CS at the old epoch
      Rvector6 inState(state.GetState());
      Rvector6 csState;
      Rvector6 finalState;
      coordConverter.Convert(GetEpoch(), inState, internalCoordSystem, csState,
         coordinateSystem);
      // Then convert back at the new epoch
      Real newEpoch = toEpoch;
      coordConverter.Convert(newEpoch, csState, coordinateSystem, finalState,
            internalCoordSystem);

      state[0] = finalState[0];
      state[1] = finalState[1];
      state[2] = finalState[2];
      state[3] = finalState[3];
      state[4] = finalState[4];
      state[5] = finalState[5];
   }
   // otherwise, state stays the same
}


//-------------------------------------------------------------------------
// bool VerifyAddHardware()
//-------------------------------------------------------------------------
// This function is used to verify Spacecraft's added hardware.
//
// return true if there is no error, false otherwise.
//-------------------------------------------------------------------------
bool Spacecraft::VerifyAddHardware()
{
   Gmat::ObjectType type;
   std::string subTypeName;
   GmatBase* obj;
   
   // 1. Verify all hardware in hardwareList are not NULL:
   for(ObjectArray::iterator i= hardwareList.begin(); i != hardwareList.end(); ++i)
   {
      obj = (*i);
      if (obj == NULL)
      {
         MessageInterface::ShowMessage("***Error***:One element of hardwareList = NULL\n");
         return false;
      }
   }
   
   // 2. Verify primary antenna to be in hardwareList:
   // 2.1. Create antenna list from hardwareList for searching:
   // extract all antenna from hardwareList and store to antennaList
   ObjectArray antennaList;
   for(ObjectArray::iterator i= hardwareList.begin(); i != hardwareList.end(); ++i)
   {
      obj = (*i);
      subTypeName = obj->GetTypeName();
      if (subTypeName == "Antenna")
         antennaList.push_back(obj);
   }
   
   // 2.2. Verify primary antenna of Receiver, Transmitter, and Transponder:
   GmatBase* antenna;
   GmatBase* primaryAntenna;
   std::string primaryAntennaName;
   bool verify = true;
   for(ObjectArray::iterator i= hardwareList.begin(); i != hardwareList.end(); ++i)
   {
      obj = (*i);
      type = obj->GetType();
      if (type == Gmat::HARDWARE)
      {
         subTypeName = obj->GetTypeName();
         if ((subTypeName == "Transmitter")||
             (subTypeName == "Receiver")||
             (subTypeName == "Transponder"))
         {
            // Get primary antenna:
            primaryAntennaName = obj->GetRefObjectName(Gmat::HARDWARE);
            primaryAntenna = obj->GetRefObject(Gmat::HARDWARE,primaryAntennaName);
            
            bool check;
            if (primaryAntenna == NULL)
            {
               MessageInterface::ShowMessage
                  ("***Error***:primary antenna of %s in %s's AddHardware list is NULL \n",
                   obj->GetName().c_str(), this->GetName().c_str());
               check = false;
            }
            else
            {
               // Check primary antenna of transmitter, receiver, or transponder is in antenna list:
               check = false;
               for(ObjectArray::iterator j= antennaList.begin(); j != antennaList.end(); ++j)
               {
                  antenna = (*j);
                  if (antenna == primaryAntenna)
                  {
                     check = true;
                     break;
                  }
                  else if (antenna->GetName() == primaryAntenna->GetName())
                  {
                     MessageInterface::ShowMessage
                        ("Primary antenna %s of %s is a clone of an antenna in %s's AddHardware\n",
                         primaryAntenna->GetName().c_str(), obj->GetName().c_str(),
                         this->GetName().c_str());
                  }
               }
               if (check == false)
               {
                  // Display error message:
                  MessageInterface::ShowMessage
                     ("***Error***:primary antenna of %s is not in %s's AddHardware\n",
                      obj->GetName().c_str(), this->GetName().c_str());
               }
            }
            
            verify = verify && check;
         }
      }
   }
   
   return verify;
}

//-------------------------------------------------------------------------
// Integer NumStateElementsSet()
//-------------------------------------------------------------------------
Integer Spacecraft::NumStateElementsSet()
{
   Integer stateSz = state.GetSize();
   Integer numSet  = stateSz;
   #ifdef DEBUG_SPACECRAFT_CS
      MessageInterface::ShowMessage("NumStateElementsSet() stateSz = %d, numSet = %d\n", stateSz, numSet);
   #endif
   for (Integer ii = 0; ii < stateSz; ii++)
      if (state[ii] == UNSET_ELEMENT_VALUE) numSet--;
   #ifdef DEBUG_SPACECRAFT_CS
      MessageInterface::ShowMessage("after loop, numSet = %d\n", numSet);
   #endif
   return numSet;
}

//-------------------------------------------------------------------------
// void SetPossibleInputTypes(const std::string& label, const std::string &rep)
//-------------------------------------------------------------------------
void  Spacecraft::SetPossibleInputTypes(const std::string& label, const std::string &rep)
{
   #ifdef DEBUG_SC_INPUT_TYPES
      MessageInterface::ShowMessage("Entering SetPossibleInputTypes: spacecraft = %s, label = %s, rep = %s\n",
            instanceName.c_str(), label.c_str(), rep.c_str());
      MessageInterface::ShowMessage("possibleInputTypes are:\n");
      for (unsigned int ii = 0; ii < possibleInputTypes.size(); ii++)
         MessageInterface::ShowMessage("      %d    %s\n", ii, possibleInputTypes.at(ii).c_str());
   #endif

   if (std::find(possibleInputTypes.begin(), possibleInputTypes.end(), rep) ==
         possibleInputTypes.end())
   {
      std::string errmsg = "Error: you have set orbital state elements not contained in the same state type.  ";
      errmsg += "This is only allowed after the BeginMissionSequence command.\n";
      throw SpaceObjectException(errmsg);
   }

   // determine the possible types

   // When there is only one possible type, reset the array to that one

   // Modified by M.H.
   if ((label == "X")            || (label == "Y")            || (label == "Z")            ||
       (label == "VX")           || (label == "VY")           || (label == "VZ")           ||
       (label == "ECC")          || (label == "RadPer")       || (label == "RadApo")       ||
       (label == "AZI")          || (label == "FPA")          ||
       (label == "RAV")          || (label == "DECV") ||
       (label == "EquinoctialH") || (label == "EquinoctialK") || (label == "EquinoctialP") ||
       (label == "EquinoctialQ") || (label == "MLONG") ||
       (label == "SemiLatusRectum") ||
       (label == "ModEquinoctialF") || (label == "ModEquinoctialG") || (label == "ModEquinoctialH") ||
       (label == "ModEquinoctialK") || (label == "TLONG") || (label == "Delaunayl") ||
       (label == "Delaunayg") || (label == "Delaunayh") || (label == "DelaunayL") ||
       (label == "DelaunayG") || (label == "DelaunayH") || (label == "PlanetodeticRMAG") ||
       (label == "PlanetodeticLON") || (label == "PlanetodeticLAT") || (label == "PlanetodeticVMAG") ||
       (label == "PlanetodeticAZI") || (label == "PlanetodeticHFPA") )
   {
      possibleInputTypes.clear();
      possibleInputTypes.push_back(rep);
   }


   // Remove impossible types from the array

   // 1) It could be Keplerian or Eauinoctial
   else if (label == "SMA")
   {
      possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "Cartesian"), possibleInputTypes.end());
      possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "ModifiedKeplerian"), possibleInputTypes.end());
      possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "SphericalAZFPA"), possibleInputTypes.end());
      possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "SphericalRADEC"), possibleInputTypes.end());
	  // Modified by M.H.
	  possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "ModifiedEquinoctial"), possibleInputTypes.end());
	  possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "Delaunay"), possibleInputTypes.end());
	  possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "Planetodetic"), possibleInputTypes.end());
   }
   // 2) It could be Keplerian or ModifiedKeplerian
   else if ((label == "INC") || (label == "RAAN") || (label == "AOP") ||
            (label == "TA")  || (label == "EA")   || (label == "MA")  ||
            (label == "HA"))
   {
      possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "Cartesian"), possibleInputTypes.end());
      possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "SphericalAZFPA"), possibleInputTypes.end());
      possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "SphericalRADEC"), possibleInputTypes.end());
      possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "Equinoctial"), possibleInputTypes.end());
	  // Modified by M.H.
	  possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "ModifiedEquinoctial"), possibleInputTypes.end());
	  possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "Delaunay"), possibleInputTypes.end());
	  possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "Planetodetic"), possibleInputTypes.end());
   }
   // 3) It could be SphericalAZFPA or SphericalRADEC
   else if ((label == "RMAG") || (label == "RA") || (label == "DEC") || (label == "VMAG"))
   {
      possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "Cartesian"), possibleInputTypes.end());
      possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "Keplerian"), possibleInputTypes.end());
      possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "ModifiedKeplerian"), possibleInputTypes.end());
      possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "Equinoctial"), possibleInputTypes.end());
	  // Modified by M.H.
	  possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "ModifiedEquinoctial"), possibleInputTypes.end());
	  possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "Delaunay"), possibleInputTypes.end());
	  possibleInputTypes.erase(std::remove(possibleInputTypes.begin(), possibleInputTypes.end(), "Planetodetic"), possibleInputTypes.end());
   }

   if (possibleInputTypes.size() < 1)
   {
      throw SpaceObjectException("Error determining input state type.\n");
   }
   #ifdef DEBUG_SC_INPUT_TYPES
      MessageInterface::ShowMessage("----> and leaving SetPossibleInputTypes: spacecraft = %s, label = %s, rep = %s\n",
            instanceName.c_str(), label.c_str(), rep.c_str());
      MessageInterface::ShowMessage("----> possibleInputTypes are now:\n");
      for (unsigned int ii = 0; ii < possibleInputTypes.size(); ii++)
         MessageInterface::ShowMessage("---->       %d    %s\n", ii, possibleInputTypes.at(ii).c_str());
   #endif
}

//-------------------------------------------------------------------------
// bool ValidateOrbitStateValue(const std::string &forRep, const std::string &withLabel,
//                              Real andValue,             bool checkCoupled)
//-------------------------------------------------------------------------
bool Spacecraft::ValidateOrbitStateValue(const std::string &forRep, const std::string &withLabel, Real andValue, bool checkCoupled)
{
   if (!checkCoupled)
   {
      #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
         MessageInterface::ShowMessage("In SC::SetElement, about to validate (no coupled element check) %s with value %le\n", withLabel.c_str(), andValue);
      #endif
      return StateConversionUtil::ValidateValue(withLabel, andValue, errorMessageFormat, GetDataPrecision());
   }

   // On option, check coupled elements
   bool validated = false;
   // if we're setting RadApo, and RadPer has been set, also check for value relative to RadPer
   if ((forRep == "ModifiedKeplerian") && (withLabel == "RadApo") && (state[0] != UNSET_ELEMENT_VALUE))
   {
      #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
         MessageInterface::ShowMessage("In SC::SetElement, about to validate %s with value %le when RadPer already set\n", withLabel.c_str(), andValue);
      #endif
         validated = StateConversionUtil::ValidateValue(withLabel, andValue, errorMessageFormat, GetDataPrecision(), "RadPer", state[0]);
   }

   // Check for SMA and ECC relative to each other, if necessary
   else if ((forRep == "Keplerian") && (withLabel == "ECC") && (state[0] != UNSET_ELEMENT_VALUE))
   {
      validated = StateConversionUtil::ValidateValue(withLabel, andValue, errorMessageFormat, GetDataPrecision(), "SMA", state[0]);
   }
   else if ((forRep == "Keplerian") && (withLabel == "SMA") && (state[1] != UNSET_ELEMENT_VALUE))
   {
      validated = StateConversionUtil::ValidateValue(withLabel, andValue, errorMessageFormat, GetDataPrecision(), "ECC", state[1]);
   }

   // Check for EquinoctialH and EquinoctialK relative to each other, if necessary
   else if ((forRep == "Equinoctial") && (withLabel == "EquinoctialH") && (state[1] != UNSET_ELEMENT_VALUE))
   {
      validated = StateConversionUtil::ValidateValue(withLabel, andValue, errorMessageFormat, GetDataPrecision(), "EquinoctialK", state[1]);
   }
   else if ((forRep == "Equinoctial") && (withLabel == "EquinoctialK") && (state[0] != UNSET_ELEMENT_VALUE))
   {
      validated = StateConversionUtil::ValidateValue(withLabel, andValue, errorMessageFormat, GetDataPrecision(), "EquinoctialH", state[1]);
   }

   // Otherwise, check the value
   else
   {
      #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
         MessageInterface::ShowMessage("In SC::SetElement, about to validate %s with value %le\n", withLabel.c_str(), andValue);
      #endif
         validated = StateConversionUtil::ValidateValue(withLabel, andValue, errorMessageFormat, GetDataPrecision());
   }
   return validated;
}
