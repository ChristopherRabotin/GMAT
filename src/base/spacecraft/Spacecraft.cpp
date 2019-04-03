//$Id$
//------------------------------------------------------------------------------
//                                  Spacecraft
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
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
#include "CCSDSAttitude.hpp"
#include "FileManager.hpp"           // for GetFullPathname()
#include "AngleUtil.hpp"             // for PutAngleInDegRange()
#include "EphemManager.hpp"

#ifdef __USE_SPICE__
#include "SpiceAttitude.hpp"         // for SpiceAttitude - to set object name and ID
#endif

// Do we want to write anomaly type?
//#define __WRITE_ANOMALY_TYPE__


//#define DEBUG_SPACECRAFT
//#define DEBUG_SPACECRAFT_SET
//#define DEBUG_SPACECRAFT_SET_ELEMENT
//#define DEBUG_SC_INPUT_TYPES
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
//#define DEBUG_UPDATE_TOTAL_MASS
//#define DEBUG_SPICE_KERNELS
//#define DEBUG_HARDWARE
//#define DEBUG_GEN_STRING
//#define DEBUG_SC_ATTITUDE_DATA
//#define DEBUG_MULTIMAP
//#define DEBUG_SPAD_DATA
//#define DEBUG_FILEPATH
//#define DEBUG_DELETE_OWNED_OBJ
//#define DEBUG_POWER_SYSTEM
//#define DEBUG_SPACECRAFT_STM
//#define DEBUG_SC_NAIF_ID
//#define DEBUG_ESTIMATION
//#define DEBUG_ATTRIB_COMMENT
//#define DEBUG_CHANGE_EPOCH_PRECISION

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
      Gmat::STRING_TYPE,      // EstimationStateType
      Gmat::RMATRIX_TYPE,     // OrbitErrorCovariance
      Gmat::REAL_TYPE,        // Cd
      Gmat::REAL_TYPE,        // Cr
      Gmat::REAL_TYPE,        // CdSigma
      Gmat::REAL_TYPE,        // CrSigma
      Gmat::REAL_TYPE,        // DragArea
      Gmat::REAL_TYPE,        // SRPArea
      Gmat::OBJECTARRAY_TYPE, // Tanks
      Gmat::OBJECTARRAY_TYPE, // Thrusters
      Gmat::OBJECT_TYPE,      // PowerSystem
      Gmat::REAL_TYPE,        // TotalMass
      Gmat::STRING_TYPE,      // Id
      Gmat::OBJECT_TYPE,      // Attitude
      Gmat::RMATRIX_TYPE,     // OrbitSTM
      Gmat::RMATRIX_TYPE,     // OrbitAMatrix
      Gmat::RMATRIX_TYPE,     // FullSTM,
      Gmat::RMATRIX_TYPE,     // FullAMatrix,
      Gmat::INTEGER_TYPE,     // FullSTMRowCount,
      Gmat::STRING_TYPE,      // EphemerisName
      Gmat::FILENAME_TYPE,    // SPADSRPFile
      Gmat::FILENAME_TYPE,    // SPADSRPFileFullPath
      Gmat::REAL_TYPE,        // SPADSRPScaleFactor
      Gmat::REAL_TYPE,        // CartesianX
      Gmat::REAL_TYPE,        // CartesianY
      Gmat::REAL_TYPE,        // CartesianZ
      Gmat::REAL_TYPE,        // CartesianVX
      Gmat::REAL_TYPE,        // CartesianVY
      Gmat::REAL_TYPE,        // CartesianVZ
      Gmat::REAL_TYPE,        // Mass Flow
      Gmat::OBJECTARRAY_TYPE, // AddHardware
      Gmat::STRINGARRAY_TYPE, // SolveFors
      Gmat::STRINGARRAY_TYPE, // StmElementNames
      Gmat::REAL_TYPE,        // CD_EPSILON
      Gmat::REAL_TYPE,        // CR_EPSILON
      Gmat::FILENAME_TYPE,    // Model File
      Gmat::FILENAME_TYPE,    // ModelFileFullPath
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
      "EstimationStateType",
      "OrbitErrorCovariance",
      "Cd",
      "Cr",
      "CdSigma",
      "CrSigma",
      "DragArea",
      "SRPArea",
      "Tanks",
      "Thrusters",
      "PowerSystem",
      "TotalMass",
      "Id",
      "Attitude",
      "OrbitSTM",                   // May be able to replace with FullSTM
      "OrbitAMatrix",               // May be able to replace with FullAMatrix
      "FullSTM",
      "FullAMatrix",
      "FullSTMRowCount",
      "EphemerisName",
      "SPADSRPFile",
      "SPADSRPFileFullPath",
      "SPADSRPScaleFactor",
      "CartesianX",
      "CartesianY",
      "CartesianZ",
      "CartesianVX",
      "CartesianVY",
      "CartesianVZ",
      "MassFlow",
      "AddHardware",
      "SolveFors",                             // move solve-for parameter from batch estimator to solve-for object 
      "StmElementNames",
      "Cd_Epsilon",
      "Cr_Epsilon",
      "ModelFile",
      "ModelFileFullPath",
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
   // Modified Equinoctial by M.H.
   "SemilatusRectum",
   "ModEquinoctialF",
   "ModEquinoctialG",
   "ModEquinoctialH",
   "ModEquinoctialK",
   "TLONG",
   // Alternate Equnoctial by HYKim
   "AltEquinoctialP",
   "AltEquinoctialQ",
   // Delaunay	by M.H.
   "Delaunayl",
   "Delaunayg",
   "Delaunayh",
   "DelaunayL",
   "DelaunayG",
   "DelaunayH",
   // Planetodetic by M.H.
   "PlanetodeticRMAG",
   "PlanetodeticLON",
   "PlanetodeticLAT",
   "PlanetodeticVMAG",
   "PlanetodeticAZI",
   "PlanetodeticHFPA",
   // Hyperbolic Incoming Asymptotes by YK
   "IncomingRadPer",
   "IncomingC3Energy",
   "IncomingRHA",
   "IncomingDHA",
   "IncomingBVAZI",
   // Hyperbolic Outgoing Asymptotes
   "OutgoingRadPer",
   "OutgoingC3Energy",
   "OutgoingRHA",
   "OutgoingDHA",
   "OutgoingBVAZI",
   // Brouwer-Lyddane Mean Elements (short term) by YK
   "BrouwerShortSMA", 
   "BrouwerShortECC",
   "BrouwerShortINC",
   "BrouwerShortRAAN",
   "BrouwerShortAOP",
   "BrouwerShortMA",
   // Brouwer-Lyddane Mean Elements (long term) by YK
   "BrouwerLongSMA",
   "BrouwerLongECC",
   "BrouwerLongINC",
   "BrouwerLongRAAN",
   "BrouwerLongAOP",
   "BrouwerLongMA",
};

const Integer  Spacecraft::ATTITUDE_ID_OFFSET  = 20000;
const Real     Spacecraft::UNSET_ELEMENT_VALUE = -999.999;
Integer        Spacecraft::scNaifId = 10000;


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
   modelFileFullPath    (""),
   dryMass              (850.0),
   coeffDrag            (2.2),
   coeffDragSigma       (1.0e70),                 // set a large number to parameter's covariance
   dragArea             (15.0),
   srpArea              (1.0),
   reflectCoeff         (1.8),
   reflectCoeffSigma    (1.0e70),                 // set a large number to parameter's covariance
   orbitErrorCovariance (6,6),                    // 6x6 matrix
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
   uniqueStateTypeFound (false),
   spacecraftId         ("SatId"),
   attitudeModel        ("CoordinateSystemFixed"),
   attitude             (NULL),
   ephemerisName        (""),
   powerSystemName      (""),
   powerSystem          (NULL),
   totalMass            (850.0),
   initialDisplay       (false),
   csSet                (false),
   isThrusterSettingMode(false),
//   orbitSTM             (6,6),
//   orbitAMatrix         (6,6),
   fullSTM              (6,6),
   fullAMatrix          (6,6),
   fullSTMRowCount      (6),
   spadSRPFile          (""),
   spadSrpFileFullPath  (""),
   spadSRPScaleFactor   (1.0),
   spadSRPReader        (NULL),
   spadBFCS             (NULL),
   ephemMgr             (NULL),
   includeCartesianState(0),
   cdEpsilon            (0.0),
   crEpsilon            (0.0),
   constrainCd          (true),
   constrainCr          (true),
   csTransformMatrix    (6, 6),
   isCSTransformMatrixSet (false)
{
   #ifdef DEBUG_SPACECRAFT
   MessageInterface::ShowMessage
      ("Spacecraft::Spacecraft() <%p>'%s' entered\n", this, name.c_str());
   #endif
   
   objectTypes.push_back(Gmat::SPACECRAFT);
   objectTypeNames.push_back("Spacecraft");
   ownedObjectCount = 0;
   blockCommandModeAssignment = false;
      
   // Set up the appropriate naifID defaults
   naifId         = -(scNaifId * 1000) - 1;
   scNaifId++;
   naifIdRefFrame = naifId + 1000000;

   #ifdef DEBUG_SC_NAIF_ID
      MessageInterface::ShowMessage("NAIF ID for spacecraft %s set to %d\n",
            instanceName.c_str(), naifId);
      MessageInterface::ShowMessage("NAIF ID for spacecraft %s reference frame set to %d\n",
            instanceName.c_str(), naifIdRefFrame);
   #endif

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
   state.SetEpochGT(GmatTime(a1mjd));
   state.SetPrecisionTimeFlag(hasPrecisionTime);

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
   representations.push_back("AlternateEquinoctial"); // Modified by HYKim
   representations.push_back("Delaunay");
   representations.push_back("Planetodetic");
   representations.push_back("IncomingAsymptote");   // Mod by YK
   representations.push_back("OutgoingAsymptote");   // Mod by YK
   representations.push_back("BrouwerMeanShort");    // Mod by YK
   representations.push_back("BrouwerMeanLong");     // Mod by YK

   #ifdef DEBUG_MULTIMAP
   MessageInterface::ShowMessage
      ("Spacecraft constructor using state element labels mulimap\n");
   #endif

   parameterCount = SpacecraftParamCount;

   // Create a default unnamed attitude
   attitude = new CSFixed("");
   attitude->SetEpoch(state.GetEpoch());
   attitude->SetEpochGT(state.GetEpochGT());
   attitude->SetPrecisionTimeFlag(hasPrecisionTime);

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

   // Build element labels and units
   BuildStateElementLabelsAndUnits();

   // Initialize the STM to the identity matrix
//   orbitSTM(0,0) = orbitSTM(1,1) = orbitSTM(2,2) =
//   orbitSTM(3,3) = orbitSTM(4,4) = orbitSTM(5,5) = 1.0;

   fullSTM(0,0) = fullSTM(1,1) = fullSTM(2,2) =
   fullSTM(3,3) = fullSTM(4,4) = fullSTM(5,5) = 1.0;

   stmIndices.push_back(CARTESIAN_X);
   stmIndices.push_back(CARTESIAN_Y);
   stmIndices.push_back(CARTESIAN_Z);
   stmIndices.push_back(CARTESIAN_VX);
   stmIndices.push_back(CARTESIAN_VY);
   stmIndices.push_back(CARTESIAN_VZ);

//   orbitAMatrix(0,0) = orbitAMatrix(1,1) = orbitAMatrix(2,2) =
//   orbitAMatrix(3,3) = orbitAMatrix(4,4) = orbitAMatrix(5,5) = 1.0;

   fullAMatrix(0,0) = fullAMatrix(1,1) = fullAMatrix(2,2) =
   fullAMatrix(3,3) = fullAMatrix(4,4) = fullAMatrix(5,5) = 1.0;

   // Set default value to covariance matrix
   covariance.AddCovarianceElement("CartesianState", this);  // locationStart = 0
   covariance.AddCovarianceElement("Cd_Epsilon", this);      // locationStart = 6
   covariance.AddCovarianceElement("Cr_Epsilon", this);      // locationStart = 7

   for (Integer i = 0; i < 6; ++i)
      orbitErrorCovariance(i,i) = 1.0e70;

   Real Cr_EpsilonSigma = reflectCoeffSigma / reflectCoeff;
   Real Cd_EpsilonSigma = coeffDragSigma / coeffDrag;
   Rvector value(8, 1.0e70, 1.0e70, 1.0e70, 1.0e70, 1.0e70, 1.0e70,
      Cd_EpsilonSigma*Cd_EpsilonSigma, Cr_EpsilonSigma*Cr_EpsilonSigma);
   covariance.ConstructRHS(value, 0);
   
   // Load default model file
   // Find file name and full path (LOJ: 2014.06.17)
   //modelFile = FileManager::Instance()->GetFullPathname("SPACECRAFT_MODEL_FILE");
   
   // modelFile = FileManager::Instance()->GetFilename("SPACECRAFT_MODEL_FILE");
   // modelFileFullPath = FileManager::Instance()->FindPath(modelFile, "SPACECRAFT_MODEL_FILE", true, false);
   
   // Use GmatBase::GetFullPathFileName() (LOJ: 2014.06.24)
   modelFileFullPath = GmatBase::GetFullPathFileName(modelFile, GetName(), modelFile, "SPACECRAFT_MODEL_FILE", true);
   
   #ifdef DEBUG_FILEPATH
   MessageInterface::ShowMessage
      ("modelFile = '%s', modelFileFullPath = '%s'\n", modelFile.c_str(),
       modelFileFullPath.c_str());
   #endif
   
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
   DeleteOwnedObjects(true, true, true, true, true);

   if (spadSRPReader)  delete spadSRPReader;
   if (spadBFCS)       delete spadBFCS;
   if (ephemMgr)
   {
      ephemMgr->StopRecording();
      delete ephemMgr;
   }


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
   modelFileFullPath    (a.modelFileFullPath),
   scEpochStr           (a.scEpochStr),
   dryMass              (a.dryMass),
   coeffDrag            (a.coeffDrag),
   coeffDragSigma       (a.coeffDragSigma),
   dragArea             (a.dragArea),
   srpArea              (a.srpArea),
   reflectCoeff         (a.reflectCoeff),
   reflectCoeffSigma    (a.reflectCoeffSigma),
   orbitErrorCovariance (a.orbitErrorCovariance),
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
   uniqueStateTypeFound (a.uniqueStateTypeFound),
   coordSysMap          (a.coordSysMap),
   spacecraftId         (a.spacecraftId),
   attitudeModel        (a.attitudeModel),
   ephemerisName        (a.ephemerisName),
   powerSystemName      (a.powerSystemName),
   powerSystem          (NULL),
   totalMass            (a.totalMass),
   initialDisplay       (false),
   csSet                (a.csSet),
   isThrusterSettingMode(a.isThrusterSettingMode),
   fullSTMRowCount      (a.fullSTMRowCount),
   spadSRPFile          (a.spadSRPFile),
   spadSrpFileFullPath  (a.spadSrpFileFullPath),
   spadSRPScaleFactor   (a.spadSRPScaleFactor),
   spadSRPReader        (NULL),
   spadBFCS             (NULL),
   ephemMgr             (NULL),
   includeCartesianState(a.includeCartesianState),
   solveforNames        (a.solveforNames),
   stmElementNames      (a.stmElementNames),
   cdEpsilon            (a.cdEpsilon),
   crEpsilon            (a.crEpsilon),
   constrainCd          (a.constrainCd),
   constrainCr          (a.constrainCr),
   csTransformMatrix    (a.csTransformMatrix),
   isCSTransformMatrixSet (a.isCSTransformMatrixSet)
{
   #ifdef DEBUG_SPACECRAFT
   MessageInterface::ShowMessage
      ("Spacecraft::Spacecraft(copy) <%p>'%s' entered\n", this, GetName().c_str());
   #endif

   objectTypes.push_back(Gmat::SPACECRAFT);
   objectTypeNames.push_back("Spacecraft");
   parameterCount = a.parameterCount;
   ownedObjectCount = 0;

   //state[0] = a.state[0];
   //state[1] = a.state[1];
   //state[2] = a.state[2];
   //state[3] = a.state[3];
   //state[4] = a.state[4];
   //state[5] = a.state[5];
   state = a.state;

   trueAnomaly = a.trueAnomaly;

   stateElementLabel = a.stateElementLabel;
   stateElementUnits = a.stateElementUnits;
   representations   = a.representations;
   tankNames         = a.tankNames;
   thrusterNames     = a.thrusterNames;

   // resize the matrices first, then copy the contents
   Integer r,c;

   r = a.fullSTM.GetNumRows();
   c = a.fullSTM.GetNumColumns();
   fullSTM.SetSize(r,c);
   fullSTM = a.fullSTM;
   stmIndices = a.stmIndices;

   r = a.fullAMatrix.GetNumRows();
   c = a.fullAMatrix.GetNumColumns();
   fullAMatrix.SetSize(r,c);
   fullAMatrix = a.fullAMatrix;

   hardwareNames     = a.hardwareNames;
//   hardwareList      = a.hardwareList;

   obsoleteObjects.clear();
   // set cloned hardware
   CloneOwnedObjects(a.attitude, a.tanks, a.thrusters, a.powerSystem, a.hardwareList);            // made changes on 09/23/2014

   // Build element labels and units
   BuildStateElementLabelsAndUnits();


   // Set value to covariance matrix
   // Set state covariance
   Integer locationStart = covariance.GetSubMatrixLocationStart("CartesianState");
   covariance.ConstructRHS(orbitErrorCovariance, locationStart);

   // Set Cd covariance
   locationStart = covariance.GetSubMatrixLocationStart("Cd_Epsilon");
   // Real Cd_EpsilonSigma = coeffDragSigma / coeffDrag;
   Real Cd0 = coeffDrag / (1 + crEpsilon);                                            // Cd0 = Cd / (1+Cd_epsilon)
   Real Cd_EpsilonSigma = coeffDragSigma / Cd0;                                       // stdiv[Cd_epsilon] = stdiv[Cd]/Cd0
   covariance(locationStart, locationStart) = Cd_EpsilonSigma * Cd_EpsilonSigma;      // var[Cd_epsilon] = (stdiv[Cr_epsilon]) ^ 2
   // Set Cr covariance
   locationStart = covariance.GetSubMatrixLocationStart("Cr_Epsilon");
   // Real Cr_EpsilonSigma = reflectCoeffSigma / reflectCoeff;
   Real Cr0 = reflectCoeff / (1 + crEpsilon);                                         // Cr0 = Cr / (1+Cr_epsilon)
   Real Cr_EpsilonSigma = reflectCoeffSigma / Cr0;                                    // stdiv[Cr_epsilon] = stdiv[Cr]/Cr0
   covariance(locationStart, locationStart) = Cr_EpsilonSigma * Cr_EpsilonSigma;      // var[Cr_epsilon] = (stdiv[Cr_epsilon])^2


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

   // Preserve the maneuvering state through the cloning
   bool iAmManeuvering = isManeuvering;

   SpaceObject::operator=(a);

   ownedObjectCount     = a.ownedObjectCount;

   modelFile            = a.modelFile;
   modelID              = a.modelID;
   modelFileFullPath    = a.modelFileFullPath;
   scEpochStr           = a.scEpochStr;
   dryMass              = a.dryMass;
   coeffDrag            = a.coeffDrag;
   coeffDragSigma       = a.coeffDragSigma;
   dragArea             = a.dragArea;
   srpArea              = a.srpArea;
   reflectCoeff         = a.reflectCoeff;
   reflectCoeffSigma    = a.reflectCoeffSigma;
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
   uniqueStateTypeFound = a.uniqueStateTypeFound;
   coordSysMap          = a.coordSysMap;
   spacecraftId         = a.spacecraftId;
   attitudeModel        = a.attitudeModel;
   ephemerisName        = a.ephemerisName;
   solarSystem          = a.solarSystem;         // need to copy
   internalCoordSystem  = a.internalCoordSystem; // need to copy
   coordinateSystem     = a.coordinateSystem;    // need to copy

   csTransformMatrix      = a.csTransformMatrix;
   isCSTransformMatrixSet = a.isCSTransformMatrixSet;

   powerSystemName      = a.powerSystemName;
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

   //state[0] = a.state[0];
   //state[1] = a.state[1];
   //state[2] = a.state[2];
   //state[3] = a.state[3];
   //state[4] = a.state[4];
   //state[5] = a.state[5];
   state = a.state;

   stateElementLabel = a.stateElementLabel;
   stateElementUnits = a.stateElementUnits;
   representations   = a.representations;
   tankNames         = a.tankNames;
   thrusterNames     = a.thrusterNames;

   hardwareNames     = a.hardwareNames;
//   hardwareList      = a.hardwareList;

   obsoleteObjects.clear();

   // delete attached hardware, such as tanks and thrusters
   #ifdef DEBUG_SPACECRAFT
   MessageInterface::ShowMessage
      ("Spacecraft::Spacecraft(=) about to delete all owned objects\n");
      #endif

   // Preserve thrusters that are "turned on"
   StringArray activeThrusters;
   for (UnsignedInt i = 0; i < thrusters.size(); ++i)
   {
      if (thrusters[i]->GetBooleanParameter("IsFiring"))
         activeThrusters.push_back(thrusters[i]->GetName());
   }

   DeleteOwnedObjects(true, true, true, true, true);

   // then cloned owned objects
   #ifdef DEBUG_SPACECRAFT
   MessageInterface::ShowMessage
      ("Spacecraft::Spacecraft(=) about to clone all owned objects\n");
      #endif
   CloneOwnedObjects(a.attitude, a.tanks, a.thrusters, a.powerSystem, a.hardwareList);              // made changes on 09/23/2014

   for (UnsignedInt i = 0; i < activeThrusters.size(); ++i)
   {
      for (UnsignedInt j = 0; j < thrusters.size(); ++j)
      {
         if (thrusters[j]->GetName() == activeThrusters[i])
            thrusters[j]->SetBooleanParameter("IsFiring", true);
      }
   }
   
   // Restore the maneuvering state
   isManeuvering = iAmManeuvering ;

   // Build element labels and units
   BuildStateElementLabelsAndUnits();

   // resize the matrices first, then copy the contents
   Integer r,c;

   r = a.fullSTM.GetNumRows();
   c = a.fullSTM.GetNumColumns();
   fullSTM.SetSize(r,c);
   fullSTM = a.fullSTM;
   stmIndices = a.stmIndices;

   r = a.fullAMatrix.GetNumRows();
   c = a.fullAMatrix.GetNumColumns();
   fullAMatrix.SetSize(r,c);
   fullAMatrix = a.fullAMatrix;

   fullSTMRowCount = a.fullSTMRowCount;


   spadSRPFile        = a.spadSRPFile;
   spadSrpFileFullPath = a.spadSrpFileFullPath;
   spadSRPScaleFactor = a.spadSRPScaleFactor;
   spadSRPReader      = NULL;
   spadBFCS           = NULL;
   // We don't want to delete any ephem manager here or the event
   // location will not work
//   if (ephemMgr) delete ephemMgr;
//   ephemMgr           = NULL;

   includeCartesianState = a.includeCartesianState;

   solveforNames      = a.solveforNames;
   stmElementNames    = a.stmElementNames;

   cdEpsilon          = a.cdEpsilon;
   crEpsilon          = a.crEpsilon;
   constrainCd        = a.constrainCd;
   constrainCr        = a.constrainCr;


   orbitErrorCovariance     = a.orbitErrorCovariance;

   // Set value to covariance matrix
   // Set state covariance
   Integer locationStart = covariance.GetSubMatrixLocationStart("CartesianState");
   covariance.ConstructRHS(orbitErrorCovariance, locationStart);

   // Set Cd covariance
   locationStart = covariance.GetSubMatrixLocationStart("Cd_Epsilon");
   // Real Cd_EpsilonSigma = coeffDragSigma / coeffDrag; 
   Real Cd0 = coeffDrag / (1 + cdEpsilon);                                           // Cd0 = Cd/(1+Cd_epsilon)
   Real Cd_EpsilonSigma = coeffDragSigma / Cd0;                                      // stdiv[Cd_epsilon] = stdiv[Cd]/Cd0
   covariance(locationStart, locationStart) = Cd_EpsilonSigma * Cd_EpsilonSigma;     // var[Cd_epsilon] = (stdiv[Cd_epsilon])^2
   // Set Cr covariance
   locationStart = covariance.GetSubMatrixLocationStart("Cr_Epsilon");
   // Real Cr_EpsilonSigma = reflectCoeffSigma / reflectCoeff; 
   Real Cr0 = reflectCoeff / (1 + crEpsilon);                                        // Cr0 = Cr/(1+Cr_epsilon)
   Real Cr_EpsilonSigma = reflectCoeffSigma / Cr0;                                   // stdiv[Cr_epsilon] = stdiv[Cr]/Cr0
   covariance(locationStart, locationStart) = Cr_EpsilonSigma * Cr_EpsilonSigma;     // var[Cd_epsilon] = (stdiv[Cd_epsilon])^2


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
   if (powerSystem) powerSystem->SetSolarSystem(ss);
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
// EphemManager* GetEphemManager()
//---------------------------------------------------------------------------
EphemManager* Spacecraft::GetEphemManager()
{
   return ephemMgr;
}


//---------------------------------------------------------------------------
// std::string GetModelFile()
//---------------------------------------------------------------------------
std::string Spacecraft::GetModelFile()
{
   return modelFile;
}

//---------------------------------------------------------------------------
// std::string GetModelFileFullPath()
//---------------------------------------------------------------------------
std::string Spacecraft::GetModelFileFullPath()
{
   return modelFileFullPath;
}

//---------------------------------------------------------------------------
// int GetModelId()
//---------------------------------------------------------------------------
int Spacecraft::GetModelId()
{
   return modelID;
}


//---------------------------------------------------------------------------
// void SetModelId(int id)
//---------------------------------------------------------------------------
void Spacecraft::SetModelId(int id)
{
   modelID = id;
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
// StringArray GetStateElementLabels(const std::string &stateType)
//------------------------------------------------------------------------------
StringArray Spacecraft::GetStateElementLabels(const std::string &stateType)
{
   if (stateElementLabelsMap.find(stateType) != stateElementLabelsMap.end())
   {
      return stateElementLabelsMap[stateType];
   }
   else
   {
      StringArray labels;
      return labels;
   }
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
// Rvector3 GetSPADSRPArea(const Real ep, const Rvector3 &sunVector)
//------------------------------------------------------------------------------
Rvector3 Spacecraft::GetSPADSRPArea(const Real ep, const Rvector3 &sunVector)
{
   if (spadSRPReader == NULL)
   {
      // Changed to use full path spad file (LOJ: 2014.06.24)
      //if (spadSRPFile == "")
      if (spadSrpFileFullPath == "")
      {
         std::string errmsg = "SPAD data requested for Spacecraft ";
         errmsg            += instanceName + " but no SPAD file ";
         errmsg            += "has been provided.\n";
         throw SpaceObjectException(errmsg);
      }
      else
      {
         spadSRPReader = new SPADFileReader();
         //spadSRPReader->SetFile(spadSRPFile);
         spadSRPReader->SetFile(spadSrpFileFullPath);
         spadSRPReader->Initialize();
      }
   }
   if (spadBFCS == NULL)
   {
      spadBFCS  = CoordinateSystem::CreateLocalCoordinateSystem("bfcs", "BodyFixed", this,
                                    NULL, NULL, GetJ2000Body(), solarSystem, false);
      spadBFCS->SetAllowWithoutRates(true);
      spadBFCS->Initialize();
      #ifdef DEBUG_SPAD_DATA
         MessageInterface::ShowMessage(" -------> in SC, just created spadBFCS\n");
      #endif
   }
   #ifdef DEBUG_SPAD_DATA
      MessageInterface::ShowMessage(" -------> in SC, input to GetSPADSRPArea sun-to-sat  = %12.10f  %12.10f  %12.10f\n",
         sunVector[0], sunVector[1], sunVector[2]);
   #endif
   // Convert the sun-to-sat input vector to sat-to-sun
   Rvector6 sunBody, resI;
   Rvector6 sunSC(-sunVector[0], -sunVector[1], -sunVector[2], 0.0, 0.0, 0.0);
   // Convert the sat-to-sun vector to the body frame
   coordConverter.Convert(ep, sunSC, internalCoordSystem, sunBody, spadBFCS, true, true);
   Rvector3 sunBody3(sunBody[0], sunBody[1], sunBody[2]);
   #ifdef DEBUG_SPAD_DATA
      MessageInterface::ShowMessage(" -------> in SC, after conversion to BFCS, sat-to-sun-in-body = %12.10f  %12.10f  %12.10f\n",
            sunBody3[0], sunBody3[1], sunBody3[2]);
   #endif
   Rvector3 result1 = spadSRPScaleFactor * spadSRPReader->GetSRPArea(sunBody3);
   Rvector6 res6(result1[0], result1[1], result1[2], 0.0,0.0,0.0);
   // Convert the result back to inertial
   coordConverter.Convert(ep, res6, spadBFCS, resI, internalCoordSystem, true, true);
   Rvector3 result(resI[0], resI[1], resI[2]);

   return result;
}


//------------------------------------------------------------------------------
//  Real GetPowerGenerated()
//------------------------------------------------------------------------------
Real Spacecraft::GetPowerGenerated()
{
   if (!powerSystem)
   {
      std::string errmsg = "No power system set for spacecraft ";
      errmsg += instanceName + ".\n";
      throw SpaceObjectException(errmsg);
   }
   return powerSystem->GetPowerGenerated();
}

//------------------------------------------------------------------------------
//  Real GetThrustPower()
//------------------------------------------------------------------------------
Real Spacecraft::GetThrustPower()
{
   if (!powerSystem)
   {
      std::string errmsg = "No power system set for spacecraft ";
      errmsg += instanceName + ".\n";
      throw SpaceObjectException(errmsg);
   }
   return powerSystem->GetThrustPower();
}

//------------------------------------------------------------------------------
//  Real GetSpacecraftBusPower()
//------------------------------------------------------------------------------
Real Spacecraft::GetSpacecraftBusPower()
{
   if (!powerSystem)
   {
      std::string errmsg = "No power system set for spacecraft ";
      errmsg += instanceName + ".\n";
      throw SpaceObjectException(errmsg);
   }
   return powerSystem->GetSpacecraftBusPower();
}

//------------------------------------------------------------------------------
// RecordEphemeris()
// Record the Spacecraft ephemeris in the background (needed by Event Location)
//------------------------------------------------------------------------------
void Spacecraft::RecordEphemerisData()
// Set up the ephemMgr here - set the coord sys, obj ptr, etc.
{
   if (!ephemMgr)
   {
      ephemMgr = new EphemManager(true);  // false is temporary - to not delete files at the end
      ephemMgr->SetObject(this);
      // @todo - do I need to resend this, if the internalCoordSys ever changes?
      ephemMgr->SetCoordinateSystem(internalCoordSystem);
      ephemMgr->SetSolarSystem(solarSystem);
      ephemMgr->Initialize();
   }
   ephemMgr->RecordEphemerisData();
}

//------------------------------------------------------------------------------
// ProvideEphemerisData()
// Load the recorded ephemeris and start up another file to continue recording
//------------------------------------------------------------------------------
void Spacecraft::ProvideEphemerisData()
{
   // @todo Fill this in
   if (!ephemMgr)
      throw SpaceObjectException(
            "RecordEphemeris() must be called before ProvideEphemeris()\n");
   ephemMgr->ProvideEphemerisData();
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
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool Spacecraft::RenameRefObject(const UnsignedInt type,
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
      if (powerSystemName == oldName)
         powerSystemName = newName;
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
   Integer paramId = GetParameterID(text);
   #ifdef DEBUG_ATTRIB_COMMENT
   MessageInterface::ShowMessage
      ("   => parameter text='%s', id=%d\n", text.c_str(), paramId);
   MessageInterface::ShowMessage
      ("   CART_X=%d, EndMultipleReps=%d, ATTITUDE=%d, ATTITUDE_ID_OFFSET=%d\n",
       CART_X, EndMultipleReps, ATTITUDE, ATTITUDE_ID_OFFSET);
   #endif
   
   // Return attribute comment for multiple state reps here
   if (((paramId >= CART_X) && (paramId < EndMultipleReps)) || (paramId == ATTITUDE))
   {
      if (attribCommentLineMap.find(text) != attribCommentLineMap.end())
         comment = attribCommentLineMap[text];

      #ifdef DEBUG_ATTRIB_COMMENT
      MessageInterface::ShowMessage
         ("Spacecraft::GetAttributeCommentLine() Returning '%s' for '%s'\n",
          comment.c_str(), text.c_str());
      #endif
      return comment;
   }
   else if (paramId >= ATTITUDE_ID_OFFSET)
   {
      if (attitude)
         comment = attitude->GetAttributeCommentLine(paramId - ATTITUDE_ID_OFFSET);
      
      #ifdef DEBUG_ATTRIB_COMMENT
      MessageInterface::ShowMessage
         ("Spacecraft::GetAttributeCommentLine() Returning '%s' for Attitude\n",
          comment.c_str());
      #endif
      return comment;
   }
   else
   {
      #ifdef DEBUG_ATTRIB_COMMENT
      MessageInterface::ShowMessage
         ("Spacecraft::GetAttributeCommentLine() Returning SpaceObject::"
          "GetAttributeCommentLine()\n");
      #endif
      return SpaceObject::GetAttributeCommentLine(paramId);
   }
}

//---------------------------------------------------------------------------
//  void SetAttributeCommentLine(Integer index, const std::string &comment)
//---------------------------------------------------------------------------
void Spacecraft::SetAttributeCommentLine(Integer index,
                                         const std::string &comment)
{
   #ifdef DEBUG_ATTRIB_COMMENT
   MessageInterface::ShowMessage
      ("Spacecraft::SetAttributeCommentLine() entered, index=%d, comment='%s'\n",
       index, comment.c_str());
   #endif
   std::string text = GetParameterText(index);
   
   #ifdef DEBUG_ATTRIB_COMMENT
   MessageInterface::ShowMessage("   => parameter text='%s'\n", text.c_str());
   MessageInterface::ShowMessage
      ("   CART_X=%d, EndMultipleReps=%d, ATTITUDE=%d\n", CART_X, EndMultipleReps, ATTITUDE);
   #endif
   
   // Save attribute comment for multiple state reps here
   if (((index >= CART_X) && (index < EndMultipleReps)) || (index == ATTITUDE))
   {
      #ifdef DEBUG_ATTRIB_COMMENT
      MessageInterface::ShowMessage
         ("   => Setting attribCommentLineMap[%s] to '%s'\n", text.c_str(),
          comment.c_str());
      #endif
      attribCommentLineMap[text] = comment;
   }
   else if (index >= ATTITUDE_ID_OFFSET)
   {
      if (attitude)
         attitude->SetAttributeCommentLine(index - ATTITUDE_ID_OFFSET, comment);
   }
   else
      SpaceObject::SetAttributeCommentLine(index, comment);
   
   #ifdef DEBUG_ATTRIB_COMMENT
   MessageInterface::ShowMessage
      ("Spacecraft::SetAttributeCommentLine() leaving, index=%d\n\n", index);
   #endif
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
   Integer paramId = GetParameterID(text);
   #ifdef DEBUG_ATTRIB_COMMENT
   MessageInterface::ShowMessage
      ("   => parameter text='%s', id=%d\n", text.c_str(), paramId);
   #endif
   
   // Return inline attribute comment for multiple state reps here
   if (((paramId >= CART_X) && (paramId < EndMultipleReps)) || (paramId == ATTITUDE))
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
   else if (paramId >= ATTITUDE_ID_OFFSET)
   {
      if (attitude)
         comment = attitude->GetInlineAttributeComment(paramId - ATTITUDE_ID_OFFSET);
      #ifdef DEBUG_ATTRIB_COMMENT
      MessageInterface::ShowMessage
         ("Spacecraft::GetInlineAttributeComment() returning '%s' for Attitude\n",
          comment.c_str());
      #endif
      return comment;
   }
   else
   {
      #ifdef DEBUG_ATTRIB_COMMENT
      MessageInterface::ShowMessage
         ("Spacecraft::GetInlineAttributeComment() Returning SpaceObject::"
          "GetInlineAttributeComment()\n");
      #endif
      return SpaceObject::GetInlineAttributeComment(paramId);
   }
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
   MessageInterface::ShowMessage("   => parameter text='%s'\n", text.c_str());
   #endif
   
   // Save inline attribute comment for multiple state reps here
   if (((index >= CART_X) && (index < EndMultipleReps)) || (index == ATTITUDE))
   {
      #ifdef DEBUG_ATTRIB_COMMENT
      MessageInterface::ShowMessage
         ("   => Setting inlineAttribCommentMap[%s] to '%s'\n", text.c_str(),
          comment.c_str());
      #endif
      inlineAttribCommentMap[text] = comment;
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
   {
      #ifdef DEBUG_ATTRIB_COMMENT
      MessageInterface::ShowMessage
         ("   Calling SpaceObject::SetInlineAttributeComment()\n");
      #endif
      SpaceObject::SetInlineAttributeComment(index, comment);
   }
   
   #ifdef DEBUG_ATTRIB_COMMENT
   MessageInterface::ShowMessage
      ("Spacecraft::SetInlineAttributeComment() leaving, index=%d\n\n", index);
   #endif
}

//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * This method returns the name of the referenced objects.
 *
 * @return name of the reference object of the requested type.
 */
//------------------------------------------------------------------------------
std::string Spacecraft::GetRefObjectName(const UnsignedInt type) const
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
   refObjectTypes.push_back(Gmat::HARDWARE);  // includes PowerSystem
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
//  const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * This method returns an array with the names of the referenced objects.
 *
 * @return a vector with the names of objects of the requested type.
 */
//------------------------------------------------------------------------------
const StringArray&
Spacecraft::GetRefObjectNameArray(const UnsignedInt type)
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

      #ifdef DEBUG_POWER_SYSTEM
            MessageInterface::ShowMessage("Pushing back powerSystemName (\"%s\") to fullList ......\n",
                  powerSystemName.c_str());
      #endif
      // Add Spacecraft Power System name
      if (powerSystemName != "")
         fullList.push_back(powerSystemName);  // need to check power system for ref object names

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

      if (type == Gmat::POWER_SYSTEM)
      {
         #ifdef DEBUG_POWER_SYSTEM
               MessageInterface::ShowMessage("Pushing back powerSystemName (\"%s\") to POWER SYSTEM list ......\n",
                     powerSystemName.c_str());
         #endif
         // Add Spacecraft Power System name
         if (powerSystemName != "")
         fullList.push_back(powerSystemName);  // need to check power system for ref object names
         return fullList;
      }

      if (type == Gmat::HARDWARE)
      {
         fullList = tankNames;
         fullList.insert(fullList.end(), thrusterNames.begin(), thrusterNames.end());
         fullList.insert(fullList.end(), hardwareNames.begin(), hardwareNames.end());
         if (powerSystemName != "")
            fullList.push_back(powerSystemName);
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

//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const char *name)
//------------------------------------------------------------------------------
bool Spacecraft::SetRefObjectName(const UnsignedInt type, const char *name)
{
   return SetRefObjectName(type, std::string(name));
}

// DJC: Not sure if we need this yet...
//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
bool Spacecraft::SetRefObjectName(const UnsignedInt type, const std::string &name)
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
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name)
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
GmatBase* Spacecraft::GetRefObject(const UnsignedInt type,
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
         break;

      case Gmat::ATTITUDE:
         #ifdef DEBUG_SC_ATTITUDE
         MessageInterface::ShowMessage(
         "In SC::GetRefObject - returning Attitude pointer <%p>\n", attitude);
         #endif
         return attitude;
         break;

      case Gmat::HARDWARE:
          for (ObjectArray::iterator i = hardwareList.begin();
               i < hardwareList.end(); ++i)
          {
             if ((*i)->GetName() == name)
                return *i;
          }
          
      case Gmat::POWER_SYSTEM:
         if (powerSystem && (powerSystemName == name))
            return powerSystem;

      case Gmat::FUEL_TANK:
      case Gmat::CHEMICAL_FUEL_TANK:
      case Gmat::ELECTRIC_FUEL_TANK:
         for (ObjectArray::iterator i = tanks.begin();
              i < tanks.end(); ++i)
         {
            if ((*i)->GetName() == name)
               return *i;
         }

      case Gmat::THRUSTER:
      case Gmat::CHEMICAL_THRUSTER:
      case Gmat::ELECTRIC_THRUSTER:
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
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name)
//------------------------------------------------------------------------------
bool Spacecraft::SetRefObject(GmatBase *obj, const UnsignedInt type,
                              const std::string &name)
{
   #ifdef DEBUG_SC_REF_OBJECT
   MessageInterface::ShowMessage
      ("Entering SC::SetRefObject <%p>'%s', obj=<%p><%s>'%s'\n", this, GetName().c_str(),
       obj, obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL");
   MessageInterface::ShowMessage("=-=-=-=-=-= at start of SetRefObject, powerSystem = <%p>\n", powerSystem);
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
   if (type == Gmat::HARDWARE || type == Gmat::FUEL_TANK || type == Gmat::THRUSTER ||
       obj->IsOfType(Gmat::THRUSTER) || obj->IsOfType(Gmat::FUEL_TANK) ||
       type == Gmat::POWER_SYSTEM)
   {
      #ifdef DEBUG_SC_REF_OBJECT
      MessageInterface::ShowMessage
         ("Spacecraft::SetRefObject() tanks.size()=%d, thrusters.size()=%d\n",
          tanks.size(), thrusters.size());
      #endif

      // set fueltank
//      if (objType == "FuelTank")
      if (obj->IsOfType("FuelTank"))
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
                  #ifdef DEBUG_SC_REF_OBJECT
                  MessageInterface::ShowMessage
                     ("Spacecraft::SetRefObject() EXCEPTION thrown in AttachTanksToThrusters\n");
                  #endif
               }
            }
         }
         // Now we can delete the obsolete objects since they should not be
         // pointed to anymore
         for (unsigned int ii = 0; ii < obsoleteObjects.size(); ii++)
         {
            #ifdef DEBUG_SC_REF_OBJECT
            MessageInterface::ShowMessage
               ("Spacecraft::SetRefObject() deleting obsolete object %s<%p>\n",
                     obsoleteObjects.at(ii)->GetName().c_str(), obsoleteObjects.at(ii));
            #endif
            delete obsoleteObjects.at(ii);
         }
         obsoleteObjects.clear();
         return retval;
      }

      // set thruster
//      if (objType == "Thruster")
      if (obj->IsOfType("Thruster"))
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
         // Now we can delete the obsolete objects since they should not be
         // pointed to anymore
         for (unsigned int ii = 0; ii < obsoleteObjects.size(); ii++)
         {
            #ifdef DEBUG_SC_REF_OBJECT
            MessageInterface::ShowMessage
               ("Spacecraft::SetRefObject() deleting obsolete object %s<%p>\n",
                     obsoleteObjects.at(ii)->GetName().c_str(), obsoleteObjects.at(ii));
            #endif
            delete obsoleteObjects.at(ii);
         }
         obsoleteObjects.clear();
         return retval;
      }

      #ifdef DEBUG_POWER_SYSTEM
         MessageInterface::ShowMessage("=-=-=-=-=-= NOW, checking for powerSystem = <%p>\n", powerSystem);
      #endif
      if (obj->IsOfType("PowerSystem"))
         return SetPowerSystem(obj, powerSystemName);

      // set on hardware
      if (obj->GetType() == Gmat::HARDWARE)
      {
         bool retval = SetHardware(obj, hardwareNames, hardwareList);
         // Now we can delete the obsolete objects since they should not be
         // pointed to anymore
         for (unsigned int ii = 0; ii < obsoleteObjects.size(); ii++)
         {
            #ifdef DEBUG_SC_REF_OBJECT
            MessageInterface::ShowMessage
               ("Spacecraft::SetRefObject() deleting obsolete object %s<%p>\n",
                     obsoleteObjects.at(ii)->GetName().c_str(), obsoleteObjects.at(ii));
            #endif
            delete obsoleteObjects.at(ii);
         }
         obsoleteObjects.clear();
         return retval;
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
            throw;
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
      attitude->SetEpochGT(state.GetEpochGT());
      attitude->SetPrecisionTimeFlag(hasPrecisionTime);

      attitude->SetOwningSpacecraft(this);
      #ifdef __USE_SPICE__
         if (attitude->IsOfType("SpiceAttitude"))
            ((SpiceAttitude*) attitude)->SetObjectID(instanceName, naifId, naifIdRefFrame);
      #endif
      return true;
   }
   else
   {
      #ifdef DEBUG_SC_REF_OBJECT
      MessageInterface::ShowMessage
         ("In SC::SetRefObject, checking attitude ... which is %sNULL\n",
               (attitude? "NOT " : ""));
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
            bool setOnAttitude = attitude->SetRefObject(obj, type, objName);
            if (setOnAttitude) return true;
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
   #ifdef DEBUG_POWER_SYSTEM
      MessageInterface::ShowMessage("=-=-=-=-=-= EXITING SetRefObject, powerSystem = <%p>\n", powerSystem);
   #endif

   return SpaceObject::SetRefObject(obj, type, name);
}


//---------------------------------------------------------------------------
//  ObjectArray& GetRefObjectArray(const UnsignedInt type)
//---------------------------------------------------------------------------
/**
 * Obtains an array of GmatBase pointers by type.
 *
 * @param type The type of objects requested
 *
 * @return Reference to the array.
 */
//---------------------------------------------------------------------------
ObjectArray& Spacecraft::GetRefObjectArray(const UnsignedInt type)
{
   if (type == Gmat::HARDWARE)
      return hardwareList;
   if (type == Gmat::FUEL_TANK)
      return tanks;
   if (type == Gmat::THRUSTER)
      return thrusters;
   if (type == Gmat::POWER_SYSTEM)
   {
      static ObjectArray powerSysList;
      powerSysList.push_back(powerSystem);
      return powerSysList;
   }
   return SpaceObject::GetRefObjectArray(type);
}


//---------------------------------------------------------------------------
//  ObjectArray& GetRefObjectArray(const UnsignedInt type)
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
   if ((typeString == "PowerSystem") || (typeString == "PowerSystems"))
   {
      static ObjectArray powerSysList;
      powerSysList.push_back(powerSystem);
      return powerSysList;
   }
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
      {
         if (str == MULT_REP_STRINGS[ii])
         {
            #ifdef DEBUG_GET_REAL
            MessageInterface::ShowMessage(
            "In SC::GetParameterID, multiple reps found!! - str = %s and id = %d\n ",
            str.c_str(), (ii + CART_X));
            #endif
            return ii + CART_X;
         }
      }

      // Check for element label
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
      if ((str == "STM") || (str == "OrbitSTM"))
         return FULL_STM;

      if ((str == "AMatrix") || (str == "OrbitAMatrix"))
         return FULL_A_MATRIX;

//      if (str == "STM")
//         return ORBIT_STM;
//
//      if (str == "AMatrix")
//         return ORBIT_A_MATRIX;

      if ((str == "CartesianState") || (str == "CartesianX")) return CARTESIAN_X;
      if (str == "CartesianY" )  return CARTESIAN_Y;
      if (str == "CartesianZ" )  return CARTESIAN_Z;
      if (str == "CartesianVX")  return CARTESIAN_VX;
      if (str == "CartesianVY")  return CARTESIAN_VY;
      if (str == "CartesianVZ")  return CARTESIAN_VZ;

      if (str == "KeplerianState") return CARTESIAN_X;

      // check for hardware object
      if (hardwareNames.size() != 0)
      {
         for (Integer ii = 0; ii < hardwareNames.size(); ii++)
         {
            if (str == hardwareNames[ii])
               return ADD_HARDWARE;
         }
      }

      if (tankNames.size() != 0)
      {
         for (Integer ii = 0; ii < tankNames.size(); ii++)
         {
            if (str == tankNames[ii])
               return FUEL_TANK_ID;
         }
      }

      if (thrusterNames.size() != 0)
      {
         for (Integer ii = 0; ii < thrusterNames.size(); ii++)
         {
            if (str == thrusterNames[ii])
               return THRUSTER_ID;
         }
      }

      if (str == powerSystemName)
         return POWER_SYSTEM_ID;

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

   if ((id == ORBIT_STM) || (id == ORBIT_A_MATRIX) || (id == STMELEMENTS))
   {
      return true;
   }

   if ((id == FULL_STM) || (id == FULL_A_MATRIX) || (id == FULL_STM_ROWCOUNT))
   {
      return true;
   }

   if (id == MASS_FLOW)
   {
      return true;
   }

   if (id == MODEL_FILE_FULL_PATH || id == SPAD_SRP_FILE_FULL_PATH)
      return true;
   
   if ((id > MODEL_FILE) && (id < MODEL_MAX))
   {
      if (modelFile == "")
         return true;
      else
         return false;
   }

   if ((id == CD_EPSILON) || (id == CR_EPSILON))
      return true;

   // NAIF ID is not read-only for spacecraft
   if (id == NAIF_ID)  return false;

   // NAIF ID for the spacecraft reference frame is not read-only for spacecraft
   if (id == NAIF_ID_REFERENCE_FRAME)  return false;

   if (id == EPHEMERIS_NAME)
      return (ephemerisName == "");

   // if (id == STATE_TYPE) return true;   when deprecated stuff goes away

   if (id == ESTIMATION_STATE_TYPE_ID) return true;

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
   if (id == MODEL_FILE || id == MODEL_FILE_FULL_PATH)
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
// bool ParameterDvInitializesNonzero(const Integer id, const Integer r,
//       const Integer c) const
//------------------------------------------------------------------------------
/**
 * Specifies elements that initialize, for derivatives, to non-zero values
 *
 * @param id The parameter ID for the data
 * @param r The row index for matrix data
 * @param c The column index for matrix data
 *
 * @return true if the initial value is not 0, false if it is zero
 */
//------------------------------------------------------------------------------
bool Spacecraft::ParameterDvInitializesNonzero(const Integer id,
      const Integer r, const Integer c) const
{
   if ((id == ORBIT_STM) || (id == FULL_STM))
   {
      if ((r == c-3) && (r < 3))
         return true;
      return false;
   }

   if ((id == ORBIT_A_MATRIX) || (id == FULL_A_MATRIX))
   {
      if ((r == c-3) && (r < 3))
         return true;
      return false;
   }

   return SpaceObject::ParameterDvInitializesNonzero(id);
}

//------------------------------------------------------------------------------
// Real Spacecraft::ParameterDvInitialValue(const Integer id, const Integer r,
//       const Integer c) const
//------------------------------------------------------------------------------
/**
 * Retrieves the initial derivative value for non-zero initialization
 *
 * @param id The parameter ID for the data
 * @param r The row index for matrix data
 * @param c The column index for matrix data
 *
 * @return The initial value for the derivative
 */
//------------------------------------------------------------------------------
Real Spacecraft::ParameterDvInitialValue(const Integer id, const Integer r,
      const Integer c) const
{
   if ((r == c-3) && (r < 3))
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


GmatTime Spacecraft::GetGmatTimeParameter(const Integer id) const
{
   return SpaceObject::GetGmatTimeParameter(id);
}


GmatTime Spacecraft::SetGmatTimeParameter(const Integer id, const GmatTime value)
{
   return SpaceObject::SetGmatTimeParameter(id, value);
}


GmatTime Spacecraft::GetGmatTimeParameter(const std::string &label) const
{
   if (label == "A1Epoch")
      return state.GetEpochGT();

   return GetGmatTimeParameter(GetParameterID(label));
}


GmatTime Spacecraft::SetGmatTimeParameter(const std::string &label, GmatTime value)
{
   if (label == "A1Epoch")
   {
      state.SetEpoch(value.GetMjd());
      return state.SetEpochGT(value);
   }

   return SetGmatTimeParameter(GetParameterID(label), value);
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

   if (id == CD_ID)
      return coeffDrag * (1.0 + cdEpsilon);

   if (id == CR_ID)
      return reflectCoeff * (1.0 + crEpsilon);
   
   if (id == CD_SIGMA_ID)   return coeffDragSigma;
   if (id == CR_SIGMA_ID)   return reflectCoeffSigma;

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

   if (id == SPAD_SRP_SCALE_FACTOR) return spadSRPScaleFactor;

   if (id == MODEL_OFFSET_X)     return modelOffsetX;
   if (id == MODEL_OFFSET_Y)     return modelOffsetY;
   if (id == MODEL_OFFSET_Z)     return modelOffsetZ;
   if (id == MODEL_ROTATION_X)   return modelRotationX;
   if (id == MODEL_ROTATION_Y)   return modelRotationY;
   if (id == MODEL_ROTATION_Z)   return modelRotationZ;
   if (id == MODEL_SCALE)        return modelScale;

   if (id == CD_EPSILON)         return cdEpsilon;
   if (id == CR_EPSILON)         return crEpsilon;

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
   if (id == CD_SIGMA_ID)
   {
      if (value <= 0.0)
         throw SpaceObjectException("Error: a nonpositive number was set to CrSigma. A valid value has to be a positive number.\n");
      
      coeffDragSigma = value;
      // Real CdEpsilonSigma = coeffDragSigma / coeffDrag; 
      Real Cd0 = coeffDrag / (1 + cdEpsilon);                                      // Cd0 = Cd/(1+Cd_epsilon)
      Real CdEpsilonSigma = coeffDragSigma / Cd0;                                  // stdiv[Cd_epsilon] = stdiv[Cd] / Cd0
      Integer locationStart = covariance.GetSubMatrixLocationStart("Cd_Epsilon");
      covariance(locationStart, locationStart) = CdEpsilonSigma * CdEpsilonSigma;  // var[Cd_epsilon] = (stdiv[Cd_epsilon])^2

      return coeffDragSigma; 
   }
   if (id == CR_SIGMA_ID)
   {
      if (value <= 0.0)
         throw SpaceObjectException("Error: a nonpositive number was set to CdSigma. A valid value has to be a positive number.\n");

      reflectCoeffSigma = value;
      // Real CrEpsilonSigma = reflectCoeffSigma / reflectCoeff; 
      Real Cr0 = reflectCoeff / (1 + crEpsilon);                                    // Cr0 = Cr/(1+Cr_epsilon)
      Real CrEpsilonSigma = reflectCoeffSigma / Cr0;                                // stdiv[Cr_epsilon] = stdiv[Cr]/Cr0
      Integer locationStart = covariance.GetSubMatrixLocationStart("Cr_Epsilon");
      covariance(locationStart, locationStart) = CrEpsilonSigma * CrEpsilonSigma;   // var[Cr_epsilon] = (stdiv[Cr_epsilon])^2

      return reflectCoeffSigma;
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

         if (attOK)
         {
            attitude->SetEpoch(state.GetEpoch());
            attitude->SetEpochGT(state.GetEpochGT());
            attitude->SetPrecisionTimeFlag(hasPrecisionTime);
         }
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

   if (id == SPAD_SRP_SCALE_FACTOR)
   {
      if (value < (-GmatRealConstants::REAL_EPSILON))
      {
         SpaceObjectException soe("");
         soe.SetDetails(errorMessageFormat.c_str(),
                        GmatStringUtil::ToString(value, 16).c_str(),
                        "SPADSRPScaleFactor", "Real Number >= 0.0");
         throw soe;
      }
      spadSRPScaleFactor = value;
      return spadSRPScaleFactor;
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

   if (id == CD_EPSILON)
   {
      cdEpsilon = value;
      // Turn off strict constraint because we are estimating
      constrainCd = false;
      return cdEpsilon;
   }

   if (id == CR_EPSILON)
   {
      crEpsilon = value;
      // Turn off strict constraint because we are estimating
      constrainCr = false;
      return crEpsilon;
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
         if (attOK)
         {
            attitude->SetEpoch(state.GetEpoch());
            attitude->SetEpochGT(state.GetEpochGT());
            attitude->SetPrecisionTimeFlag(hasPrecisionTime);
         }
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
      state.SetEpochGT(GmatTime(value));

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
      if ((value >= 0.0) || (constrainCd == false))
      {
         coeffDrag = value;
         cdEpsilon = 0.0;
         if (value < 0.0)
            MessageInterface::ShowMessage("Warning: The Cd value %lf is "
                  "outside of the expected range of 0.0 <= Cd\n", value);
      }
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
      if (((value >= 0.0) && (value <= 2.0)) || (constrainCr == false))
      {
         reflectCoeff = value;
         crEpsilon = 0.0;
         if ((value < 0.0) || (value > 2.0))
            MessageInterface::ShowMessage("Warning: The Cr value %lf is "
                  "outside of the expected range of 0.0 <= Cr <= 2.0\n", value);
      }
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
   if (label == "Cd_Epsilon")
   {
      cdEpsilon = value;
      constrainCd = false;
      return cdEpsilon;
   }
   if (label == "Cr_Epsilon")
   {
      crEpsilon = value;
      constrainCr = false;
      return crEpsilon;
   }

   if (label == "TotalMass")// return totalMass;    // Don't change the total mass
      throw SpaceObjectException("The parameter \"TotalMass\" is a calculated "
            "parameter and cannot be set on the spacecraft " + instanceName);

   return SpaceObject::SetRealParameter(label, value);
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves an integer parameter value
 *
 * @param id Parameter ID for the requested field
 *
 * @return The value of the field
 */
//------------------------------------------------------------------------------
Integer Spacecraft::GetIntegerParameter(const Integer id) const
{
   if (id == FULL_STM_ROWCOUNT)
      return fullSTMRowCount;

   return SpaceObject::GetIntegerParameter(id);
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * Sets the value for an integer field
 *
 * @param id Parameter ID for the requested field
 * @param value The new value
 *
 * @return The value of the field
 */
//------------------------------------------------------------------------------
Integer Spacecraft::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == FULL_STM_ROWCOUNT)
   {
      fullSTMRowCount = value;
      fullSTM.SetSize(fullSTMRowCount, fullSTMRowCount, true);
      for (Integer i = 0; i < fullSTMRowCount; ++i)
         fullSTM(i,i) = 1.0;
      return fullSTMRowCount;
   }

   return SpaceObject::SetIntegerParameter(id, value);
}

//Integer Spacecraft::GetIntegerParameter(const Integer id, const Integer index) const
//{
//   return SpaceObject::GetIntegerParameter(id, index);
//}
//Integer Spacecraft::SetIntegerParameter(const Integer id, const Integer value, const Integer index)
//{
//   return SpaceObject::SetIntegerParameter(id, value, index);
//}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves an integer parameter value
 *
 * @param label Parameter label for the requested field
 *
 * @return The value of the field
 */
//------------------------------------------------------------------------------
Integer Spacecraft::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
 * Sets the value for an integer field
 *
 * @param label Parameter label for the requested field
 * @param value The new value
 *
 * @return The value of the field
 */
//------------------------------------------------------------------------------
Integer Spacecraft::SetIntegerParameter(const std::string &label, const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}

//Integer Spacecraft::GetIntegerParameter(const std::string &label, const Integer index) const
//{
//   return GetIntegerParameter(GetParameterID(label), index);
//}
//Integer Spacecraft::SetIntegerParameter(const std::string &label, const Integer value, const Integer index)
//{
//   return SetIntegerParameter(GetParameterID(label), value, index);
//}

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

    //if (id == ESTIMATION_STATE_TYPE_ID)
    //   return estimationStateType;

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

    if (id == SPAD_SRP_FILE)
       return spadSRPFile;
    
    if (id == SPAD_SRP_FILE_FULL_PATH)
       return spadSrpFileFullPath;
    
    if (id == MODEL_FILE)
       return modelFile;

    if (id == MODEL_FILE_FULL_PATH)
       return modelFileFullPath;

    if (id == POWER_SYSTEM_ID)
       return powerSystemName;

    if (id == EPHEMERIS_NAME)
       return ephemerisName;

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
         break;

      case SOLVEFORS:
         {
            if ((0 <= index)&(index < (Integer)(solveforNames.size())))
               return solveforNames[index];
            else
               return "";
         }
         break;

      case STMELEMENTS:
         if ((index >= 0) && (index < (Integer)stmElementNames.size()))
            return stmElementNames[index];
         else
            return "";
         break;

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
   if (id == SOLVEFORS)
      return solveforNames;
   if (id == STMELEMENTS)
      return stmElementNames;
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
         else if (newAttType == "CCSDS-AEM")
            newAtt = new CCSDSAttitude();
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
         bool attitudeOK = SetAttitudeAndCopyData(attitude, newAtt, true);
         if (!attitudeOK)
         {
            std::string errmsg = "Cannot copy attitude data to new attitude object!\n";
            throw SpaceObjectException(errmsg);
         }
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
      // trim off braces:
      if (GmatStringUtil::IsEnclosedWithBraces(value))
      {
         std::string value1 = GmatStringUtil::RemoveEnclosingString(value, "{}");
         if (GmatStringUtil::Trim(value1) == "")
            return true;                           // empty list of hardware. Nothing to add
      }

      // verify input value:
      if (GmatStringUtil::IsValidIdentity(value) == false)
      {
         throw GmatBaseException("Error: The value \"" + value + "\" cannot accepted for " + GetName() + ".AddHardware ");
         return false;
      }

      // Only add the hardware if it is not in the list already
      if (find(hardwareNames.begin(), hardwareNames.end(), value) ==
          hardwareNames.end())
      {
         hardwareNames.push_back(value);
      }
      return true;
   }

   if ((id == SOLVEFORS) || (id == STMELEMENTS))
   {
      //if ((id == SOLVEFORS) && (value.substr(0,2) == "{}"))
      //{
      //   solveforNames.clear();
      //   return true;
      //}

      if (id == SOLVEFORS)
      {
         // trim off braces:
         if (GmatStringUtil::IsEnclosedWithBraces(value))
         {
            std::string value1 = GmatStringUtil::RemoveEnclosingString(value, "{}");
            if (GmatStringUtil::Trim(value1) == "")
            {
               solveforNames.clear();               // empty list of solvefors.
               return true;
            }
         }

         // verify input value:
         if (GmatStringUtil::IsValidIdentity(value) == false)
         {
            throw GmatBaseException("Error: The value \"" + value + "\" cannot accepted for " + GetName() + ".SolveFors ");
            return false;
         }

         if ((value == "CartesianState") || (value == "KeplerianState"))
         {
            // Remove CartesianState or KeplerianState from the list
            for (Integer i = 0; i < solveforNames.size(); ++i)
            {
               if ((solveforNames[i] == "CartesianState") || (solveforNames[i] == "KeplerianState"))
               {
                  solveforNames[i] = solveforNames[solveforNames.size() - 1];
                  solveforNames.pop_back();
               }
            }
         }

         if (find(solveforNames.begin(), solveforNames.end(), value) == solveforNames.end())
         {
            // Only add the solvefor parameter if it is not in the list already
            solveforNames.push_back(value);
         }
      }

      // Make sure the solve-for list is in the STM
      for (UnsignedInt i = 0; i < solveforNames.size(); ++i)
      {
         if (find(stmElementNames.begin(), stmElementNames.end(), value) ==
               stmElementNames.end())
            stmElementNames.push_back(value);
      }

      // Add extras
      if (id == STMELEMENTS)
      {
         if (find(stmElementNames.begin(), stmElementNames.end(), value) ==
               stmElementNames.end())
            stmElementNames.push_back(value);
      }

      // Reset length and indices
      Integer length = 6;

      stmIndices.clear();
      stmIndices.push_back(GetParameterID("CartesianX"));
      stmIndices.push_back(GetParameterID("CartesianY"));
      stmIndices.push_back(GetParameterID("CartesianZ"));
      stmIndices.push_back(GetParameterID("CartesianVX"));
      stmIndices.push_back(GetParameterID("CartesianVY"));
      stmIndices.push_back(GetParameterID("CartesianVZ"));

      for (UnsignedInt i = 0; i < stmElementNames.size(); ++i)
      {
         // Cartesian state handled above
         if ((stmElementNames[i] != "CartesianState") && (stmElementNames[i] != "KeplerianState"))
         {
            length += GetEstimationParameterSize(GetParameterID(stmElementNames[i]));
            for (Integer j = 0; j < GetEstimationParameterSize(
                  GetParameterID(stmElementNames[i])); ++j)
            {
               stmIndices.push_back(GetParameterID(stmElementNames[i]));
               #ifdef DEBUG_SPACECRAFT_STM
                  MessageInterface::ShowMessage("Looking up %s --> %d\n",
                        stmElementNames[i].c_str(),
                        GetParameterID(stmElementNames[i]));
               #endif
            }
         }
      }

      #ifdef DEBUG_SPACECRAFT_STM
         MessageInterface::ShowMessage("Setting %s: STM has %d rows and "
               "columns\n", value.c_str(), length);
         for (UnsignedInt i = 0; i < stmElementNames.size(); ++i)
            MessageInterface::ShowMessage("   %d:  %s\n", i,
                  stmElementNames[i].c_str());
      #endif

      SetIntegerParameter(FULL_STM_ROWCOUNT, length);
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
      if (value != "Cartesian" && value != "Keplerian" && value != "ModifiedKeplerian" &&
          value != "SphericalAZFPA" && value != "SphericalRADEC" && value != "Equinoctial" &&
          value != "ModifiedEquinoctial" && value != "AlternateEquinoctial" &&
          value != "Delaunay" && value != "Planetodetic" &&
          value != "IncomingAsymptote" && value != "OutgoingAsymptote" &&
          value != "BrouwerMeanShort" && value != "BrouwerMeanLong")
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
   else if (id == POWER_SYSTEM_ID)
   {
      #ifdef DEBUG_POWER_SYSTEM
            MessageInterface::ShowMessage("powerSystemName being set to %s......\n",
                  value.c_str());
      #endif
      powerSystemName = value;
   }
   else if (id == EPHEMERIS_NAME)
   {
      ephemerisName = value;
   }
   else if (id == SPAD_SRP_FILE)
   {
      if (value != spadSRPFile)
      {
         delete spadSRPReader;
         spadSRPReader = NULL;
      }
      spadSRPFile = value;
      
      // Use FileManager::FindPath() for full path file name (2014.06.24)
      spadSrpFileFullPath = FileManager::Instance()->FindPath(value, "SPAD_SRP_FILE", true, false, true);
      // Write warning or throw an exception?
      if (spadSrpFileFullPath == "")
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** The SPAD SRP file '%s' does not exist for the spacecraft '%s'\n",
             spadSRPFile.c_str(), GetName().c_str());
      }
   }
   else if (id == MODEL_FILE)
   {
        modelFile = value;
      
      // Use FileManager::FindPath() for full path file name (2014.06.24)
      modelFileFullPath = FileManager::Instance()->FindPath(value, "SPACECRAFT_MODEL_FILE", true, false, true);
      if (modelFileFullPath == "")
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** The model file '%s' does not exist for the spacecraft '%s'\n",
             modelFile.c_str(), GetName().c_str());
      }
   }
   else if (id == ESTIMATION_STATE_TYPE_ID)
   {
      MessageInterface::ShowMessage("*** WARNING *** EstimationStateType is no longer used and will be removed in a future build.\n");
   }
   else
      return SpaceObject::SetStringParameter(id, value);


   #ifdef DEBUG_SC_SET_STRING
   MessageInterface::ShowMessage
      ("Spacecraft::SetStringParameter() returning true\n");
   #endif

   return true;
}

//---------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const char *value)
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
                                    const char *value)
{
   return SetStringParameter(label, std::string(value));
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
         // verify input value:
         if (GmatStringUtil::IsValidIdentity(value) == false)
         {
            throw GmatBaseException("Error: The value \"" + value + "\" cannot accepted for " + GetName() + ".AddHardware ");
            return false;
         }

         if (index < (Integer)hardwareNames.size())
            hardwareNames[index] = value;
         else
            // Only add the hardware if it is not in the list already
            if (find(hardwareNames.begin(), hardwareNames.end(), value) == hardwareNames.end())
               hardwareNames.push_back(value);

         return true;
      }
      break;

   case SOLVEFORS:
      {
         if (index < (Integer)solveforNames.size())
            solveforNames[index] = value;
         else
            // Only add the solvefor parameter if it is not in the list already
            if (find(solveforNames.begin(), solveforNames.end(), value) == solveforNames.end())
            {
               solveforNames.push_back(value);
            }

         return true;
      }
      break;

   case STMELEMENTS:
      if (index < stmElementNames.size())
         stmElementNames[index] = value;
      else
         if (find(stmElementNames.begin(), stmElementNames.end(), value) == stmElementNames.end())
         {
            stmElementNames.push_back(value);
         }
      return true;
      break;

   case FUEL_TANK_ID:
      {
         if (index < (Integer)tankNames.size())
            tankNames[index] = value;
         else
            // Only add the tank if it is not in the list already
            if (find(tankNames.begin(), tankNames.end(), value) == tankNames.end())
            {
               tankNames.push_back(value);
            }

         return true;
      }
      break;

   case THRUSTER_ID:
      {
         if (index < (Integer)thrusterNames.size())
            thrusterNames[index] = value;
         else
            // Only add the tank if it is not in the list already
            if (find(thrusterNames.begin(), thrusterNames.end(), value) ==
            thrusterNames.end())
            {
               thrusterNames.push_back(value);
            }
         return true;
      }
      break;

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
   {
//      // Copy over upper left 6x6 of the full matrix
//      for (UnsignedInt i = 0; i < 6; ++i)
//         for (UnsignedInt j = 0; j < 6; ++j)
//            // Cast to handle const for now
//            ((Rmatrix)orbitSTM)(i,j) = fullSTM(i,j);
//      return orbitSTM;
      return fullSTM;
   }

   if (id == ORBIT_A_MATRIX)
   {
//      // Copy over upper left 6x6 of the full matrix
//      for (UnsignedInt i = 0; i < 6; ++i)
//         for (UnsignedInt j = 0; j < 6; ++j)
//            // Cast to handle const for now
//            ((Rmatrix)orbitAMatrix)(i,j) = fullAMatrix(i,j);
//      return orbitAMatrix;
      return fullAMatrix;
   }

   if (id == FULL_STM)
      return fullSTM;

   if (id == FULL_A_MATRIX)
      return fullAMatrix;

//   if (id == ORBIT_COVARIANCE)
//      return covariance;

   if (id == ORBIT_ERROR_COVARIANCE_ID)
      return orbitErrorCovariance;

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
      #ifdef DEBUG_STM
         MessageInterface::ShowMessage("Setting orbit STM\n");
      #endif
//         orbitSTM = value;
//         return orbitSTM;
      fullSTM = value;
      return fullSTM;
   }

   if (id == ORBIT_A_MATRIX)
   {
      #ifdef DEBUG_STM
         MessageInterface::ShowMessage("Setting orbit A Matrix\n");
      #endif
//      orbitAMatrix = value;
//      return orbitAMatrix;
      fullAMatrix = value;
      return fullAMatrix;
   }

   if (id == FULL_STM)
   {
      #ifdef DEBUG_STM
         MessageInterface::ShowMessage("Setting full STM\n");
      #endif
      fullSTM = value;
      return fullSTM;
   }

   if (id == FULL_A_MATRIX)
   {
      #ifdef DEBUG_STM
         MessageInterface::ShowMessage("Setting full A Matrix\n");
      #endif
      fullAMatrix = value;
      return fullAMatrix;
   }

//   if (id == ORBIT_COVARIANCE)
//   {
//      covariance = value;
//      return covariance;
//   }

   if (id == ORBIT_ERROR_COVARIANCE_ID)
   {
      if (value.GetNumRows() != value.GetNumColumns())
         throw SpaceObjectException("Error: matrix set to " + GetName() + ".OrbitErrorCovariance parameter is not a square matrix.\n");
      if (value.GetNumRows() != 6)
         throw SpaceObjectException("Error: matrix set to " + GetName() + ".OrbitErrorCovariance parameter does not have size of 6x6.\n");

      orbitErrorCovariance = value;
      if (isInitialized)
      {
         // When it is not initialized, this setting will be done in Initialize() function
         Integer locationStart = covariance.GetSubMatrixLocationStart("CartesianState");
         for (Integer row = 0; row < value.GetNumRows(); ++row)
            for (Integer col = 0; col < value.GetNumColumns(); ++col)
               covariance(locationStart + row, locationStart + col) = value(row, col);
      }
      
      return orbitErrorCovariance;
   }

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
//      return orbitSTM(row, col);
      return fullSTM(row, col);

   if (id == ORBIT_A_MATRIX)
//      return orbitAMatrix(row, col);
      return fullAMatrix(row, col);

   if (id == FULL_STM)
      return fullSTM(row, col);

   if (id == FULL_A_MATRIX)
      return fullAMatrix(row, col);

   if (id == ORBIT_ERROR_COVARIANCE_ID)
   {

      if ((row < 0) || (row >= orbitErrorCovariance.GetNumRows()))
         throw SpaceObjectException("GetRealParameter: row requested for orbitErrorCovariance is out - of - range\n");
      if ((col < 0) || (col >= orbitErrorCovariance.GetNumColumns()))
         throw SpaceObjectException("GetRealParameter: column requested for orbitErrorCovariance is out - of - range\n");

      return orbitErrorCovariance(row, col);
   }

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
//      if ((row < 0) || (row >= orbitSTM.GetNumRows()))
//         throw SpaceObjectException("SetRealParameter: row requested for orbitSTM is out-of-range\n");
//      if ((col < 0) || (col >= orbitSTM.GetNumColumns()))
//         throw SpaceObjectException("SetRealParameter: col requested for orbitSTM is out-of-range\n");
//      orbitSTM(row, col) = value;
//      return orbitSTM(row, col);
      if ((row < 0) || (row >= fullSTM.GetNumRows()))
         throw SpaceObjectException("SetRealParameter: row requested for orbitSTM is out-of-range\n");
      if ((col < 0) || (col >= fullSTM.GetNumColumns()))
         throw SpaceObjectException("SetRealParameter: col requested for orbitSTM is out-of-range\n");
      fullSTM(row, col) = value;

      #ifdef DEBUG_SPACECRAFT_STM
         if ((row == col) && (row + 1 == fullSTMRowCount))
            MessageInterface::ShowMessage("Full STM:\n%s\n", fullSTM.ToString().c_str());
      #endif

      return fullSTM(row, col);
   }

   if (id == ORBIT_A_MATRIX)
   {
//      if ((row < 0) || (row >= orbitAMatrix.GetNumRows()))
//         throw SpaceObjectException("SetRealParameter: row requested for orbitAMatrix is out-of-range\n");
//      if ((col < 0) || (col >= orbitAMatrix.GetNumColumns()))
//         throw SpaceObjectException("SetRealParameter: col requested for orbitAMatrix is out-of-range\n");
//      orbitAMatrix(row, col) = value;
//      return orbitAMatrix(row, col);
      if ((row < 0) || (row >= fullAMatrix.GetNumRows()))
         throw SpaceObjectException("SetRealParameter: row requested for orbitAMatrix is out-of-range\n");
      if ((col < 0) || (col >= fullAMatrix.GetNumColumns()))
         throw SpaceObjectException("SetRealParameter: col requested for orbitAMatrix is out-of-range\n");
      fullAMatrix(row, col) = value;
      return fullAMatrix(row, col);
   }

   if (id == FULL_STM)
   {
      if ((row < 0) || (row >= fullSTM.GetNumRows()))
         throw SpaceObjectException("SetRealParameter: row requested for fullSTM is out-of-range\n");
      if ((col < 0) || (col >= fullSTM.GetNumColumns()))
         throw SpaceObjectException("SetRealParameter: col requested for fullSTM is out-of-range\n");
      fullSTM(row, col) = value;

      #ifdef DEBUG_SPACECRAFT_STM
         if ((row == col) && (row == fullSTMRowCount-1))
            MessageInterface::ShowMessage("Full STM; setting rc %d, %d:  \n%s\n", row, col, fullSTM.ToString(12).c_str());
      #endif

      return fullSTM(row, col);
   }

   if (id == FULL_A_MATRIX)
   {
      if ((row < 0) || (row >= fullAMatrix.GetNumRows()))
         throw SpaceObjectException("SetRealParameter: row requested for fullAMatrix is out-of-range\n");
      if ((col < 0) || (col >= fullAMatrix.GetNumColumns()))
         throw SpaceObjectException("SetRealParameter: col requested for fullAMatrix is out-of-range\n");
      fullAMatrix(row, col) = value;
      return fullAMatrix(row, col);
   }

   if (id == ORBIT_ERROR_COVARIANCE_ID)
   {
      if ((row < 0) || (row >= orbitErrorCovariance.GetNumRows()))
         throw SpaceObjectException("SetRealParameter: row requested for orbitErrorCovariance is out-of-range\n");
      if ((col < 0) || (col >= orbitErrorCovariance.GetNumColumns()))
         throw SpaceObjectException("SetRealParameter: column requested for orbitErrorCovariance is out-of-range\n");

      Integer locationStart = covariance.GetSubMatrixLocationStart("CartesianState");
      orbitErrorCovariance(row, col) = value;

      if (isInitialized)
         covariance(locationStart + row, locationStart + col) = value;

      return orbitErrorCovariance(row, col);
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
            if (attOK)
            {
               attitude->SetEpoch(state.GetEpoch());
               attitude->SetEpochGT(state.GetEpochGT());
               attitude->SetPrecisionTimeFlag(hasPrecisionTime);
            }
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
       (action == "RemoveThruster") || (action == "RemovePowerSystem"))
   {
      bool removeTank     = true, removeThruster = true;
      bool removePowerSys = true, removeAll      = false;
      if (action == "RemoveTank")
      {
         removePowerSys = false;
         removeThruster = false;
      }
      if (action == "RemoveThruster")
      {
         removePowerSys = false;
         removeTank     = false;
      }
      if (action == "RemovePowerSystem")
      {
         removeThruster = false;
         removeTank     = false;
      }
      if (actionData == "")
         removeAll = true;

      if (removePowerSys)
      {
         DeleteOwnedObjects(false, false, false, true, false);
         powerSystem = NULL;
//         powerSystemName = "";   // Is this correct?
      }

      if (removeThruster)
      {
         if (removeAll)
         {
            DeleteOwnedObjects(false, false, true, false, false);
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
            DeleteOwnedObjects(false, true, true, false, false);
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
      GmatTime currEpochGT = currEpoch;
      if (state.HasPrecisionTime())
         currEpochGT = state.GetEpochGT();
      
      bool isInLeapSecond = false;
      bool isUTC          = false;

      if (epochSystem != "")
      {
         if (epochSystem != "A1")
         {
            Integer epochSystemID = TimeConverterUtil::GetTimeTypeID(epochSystem);
            if (state.HasPrecisionTime())
            {
               currEpochGT = TimeConverterUtil::Convert(currEpochGT, 
                  TimeConverterUtil::A1, 
                  epochSystemID, 
                  GmatTimeConstants::JD_JAN_5_1941);
            }
            else
            {
               currEpoch = TimeConverterUtil::Convert(currEpoch,
                  TimeConverterUtil::A1,
                  epochSystemID,
                  GmatTimeConstants::JD_JAN_5_1941);
            }

            isInLeapSecond = TimeConverterUtil::HandleLeapSecond();
            if ((epochSystemID == TimeConverterUtil::UTCMJD) ||
                (epochSystemID == TimeConverterUtil::UTC))
               isUTC = true;
         }
      }

      if (epochFormat != "")
      {
         if (epochFormat == "Gregorian")
         {
            bool handleLeapSecond = isInLeapSecond && isUTC;
            if (state.HasPrecisionTime())
               scEpochStr = TimeConverterUtil::ConvertMjdToGregorian(currEpochGT.GetMjd(), handleLeapSecond);
            else
               scEpochStr = TimeConverterUtil::ConvertMjdToGregorian(currEpoch, handleLeapSecond);
         }
         else
         {
            std::stringstream timestream;
            timestream.precision(GetTimePrecision());
            if (state.HasPrecisionTime())
               timestream << currEpochGT.ToString();
            else
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
      for (Integer i = 0; i < fullSTMRowCount; ++i)
      {
         fullSTM(i,i) = 1.0;
         for (Integer j = i+1; j < fullSTMRowCount; ++j)
         {
            fullSTM(i,j) = fullSTM(j,i) = 0.0;
         }
      }
   }

   if (action == "ResetAMatrix")
   {
      for (Integer i = 0; i < fullSTMRowCount; ++i)
      {
         fullAMatrix(i,i) = 1.0;
         for (Integer j = i+1; j < fullSTMRowCount; ++j)
         {
            fullAMatrix(i,j) = fullAMatrix(j,i) = 0.0;
         }
      }
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
// UnsignedInt GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves object type of parameter of given id.
 *
 * @param <id> ID for the parameter.
 *
 * @return parameter ObjectType
 */
//---------------------------------------------------------------------------
UnsignedInt Spacecraft::GetPropertyObjectType(const Integer id) const
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
   case POWER_SYSTEM_ID:
      return Gmat::POWER_SYSTEM;
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

               // Connecting up to primary antenna
               bool found = false;
               for (UnsignedInt k = 0; k < hardwareList.size(); ++k)
               {
                  if (hardwareList[k]->GetName() == refs[j])
                  {
                     current->SetRefObject(hardwareList[k],
                        hardwareList[k]->GetType(), hardwareList[k]->GetName());
                     found = true;
                  }
               }
               if (found)
                  continue;

               // connecting up to ErrorModels
               GmatBase* obj = GetConfiguredObject(refs[j])->Clone();
               if (obj)
                  current->SetRefObject(obj, obj->GetType(), obj->GetName());
               else
                  GmatBaseException("Error: this name '" + refs[j] + "' was not defined in GMAT script.\n");
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

      if (powerSystem) powerSystem->Initialize();

//      if (!ephemMgr)
//      {
//         ephemMgr = new EphemManager(false);  // false is temporary - to not delete files at the end
//         ephemMgr->SetObject(this);
//         // @todo - do I need to resend this, if the internalCoordSys ever changes?
//         ephemMgr->SetCoordinateSystem(internalCoordSystem);
//         ephemMgr->SetSolarSystem(solarSystem);
//         ephemMgr->Initialize();
//      }

      // Set transformation matrix to convert from spacecraft's coordinate system to its internal coordinate system
      GetCoordinateSystemTransformMatrix();
      isCSTransformMatrixSet = true;

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
   GmatTime fromMjdGT = GmatTime(-999.999);
   GmatTime outMjdGT = GmatTime(-999.999);

   std::string outStr;

   #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage
         ("Spacecraft::SetEpoch() Converting from %s to A1ModJulian\n", epochType.c_str());
   #endif

   // remove enclosing quotes for the conversion
   std::string epNoQuote = GmatStringUtil::RemoveEnclosingString(ep, "'");
   TimeConverterUtil::Convert(epochType, fromMjdGT, epNoQuote, "A1ModJulian", outMjdGT, outStr);
   TimeConverterUtil::Convert(epochType, fromMjd, epNoQuote, "A1ModJulian", outMjd, outStr);

   if (hasPrecisionTime)
   {
      #ifdef DEBUG_DATE_FORMAT
         MessageInterface::ShowMessage
            ("Spacecraft::SetEpoch() Done converting from %s to A1ModJulian\n", epochType.c_str());
         MessageInterface::ShowMessage("    outMjdGT = %s\n", outMjdGT.ToString().c_str());
      #endif

      if (outMjdGT != GmatTime(-999.999))
      {
         RecomputeStateAtEpochGT(outMjdGT);
         state.SetEpoch(outMjd);
         state.SetEpochGT(outMjdGT);
         if (epochSet && !csSet)
         {
            std::string warnmsg = "*** WARNING *** You have set the epoch for Spacecraft ";
            warnmsg += instanceName + " more than once in assignment mode (i.e. before the BeginMissionSequence command).  ";
            warnmsg += "This may have unintended consequences and you should perform these ";
            warnmsg += "operations in command mode (i.e. after the BeginMissionSequence command).\n";
            MessageInterface::PopupMessage(Gmat::WARNING_, warnmsg);
         }
         epochSet = true;
         if (attitude)
         {
            attitude->SetEpochGT(outMjd);
            attitude->SetEpochGT(outMjdGT);
         }
      }
      else
      {
         #ifdef DEBUG_DATE_FORMAT
            MessageInterface::ShowMessage("Spacecraft::SetEpoch() oops!  outMjdGT = -999.999!!\n");
         #endif
      }

   }
   else
   {
      #ifdef DEBUG_DATE_FORMAT
         MessageInterface::ShowMessage
            ("Spacecraft::SetEpoch() Done converting from %s to A1ModJulian\n", epochType.c_str());
      #endif

      if (outMjd != -999.999)
      {
         RecomputeStateAtEpoch(outMjd);
         state.SetEpoch(outMjd);
         state.SetEpochGT(outMjdGT);
         if (epochSet && !csSet)
         {
            std::string warnmsg = "*** WARNING *** You have set the epoch for Spacecraft ";
            warnmsg += instanceName + " more than once in assignment mode (i.e. before the BeginMissionSequence command).  ";
            warnmsg += "This may have unintended consequences and you should perform these ";
            warnmsg += "operations in command mode (i.e. after the BeginMissionSequence command).\n";
            MessageInterface::PopupMessage(Gmat::WARNING_, warnmsg);
         }
         epochSet = true;
         if (attitude)
         {
            attitude->SetEpoch(outMjd);
            attitude->SetEpochGT(outMjdGT);
         }
      }
      else
      {
         #ifdef DEBUG_DATE_FORMAT
         MessageInterface::ShowMessage("Spacecraft::SetEpoch() oops!  outMjd = -999.999!!\n");
         #endif
      }
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
   if (hasPrecisionTime)
   {
      GmatTime a1mjdGT = GmatTime(a1mjd);
      RecomputeStateAtEpochGT(a1mjdGT);
      state.SetEpochGT(a1mjdGT);
      state.SetEpochGT(a1mjd);
   }
   else
   {
      RecomputeStateAtEpoch(a1mjd);
      state.SetEpoch(a1mjd);
      state.SetEpochGT(GmatTime(a1mjd));
   }
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
         break;
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
         retval = fullSTMRowCount * fullSTMRowCount;
         break;

      case Gmat::ORBIT_A_MATRIX:
         retval = fullSTMRowCount * fullSTMRowCount;
         break;
      case Gmat::MASS_FLOW:
         // todo: Access tanks for mass information to handle mass flow

         // For now, only allow one tank
         retval = 1;
         break;

      // All other values call up the hierarchy
      default:
         retval = SpaceObject::GetPropItemSize(item);
         break;
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
         break;        // Intentional drop through
   }

   return SpaceObject::PropItemNeedsFinalUpdate(item);
}


//------------------------------------------------------------------------------
// Integer GmatBase::GetEstimationParameterID(const std::string &param)
//------------------------------------------------------------------------------
/**
 * This method builds the parameter ID used in the estimation subsystem
 *
 * @param param The text name of the estimation parameter
 *
 * @return The ID used in estimation for the parameter
 */
//------------------------------------------------------------------------------
Integer Spacecraft::GetEstimationParameterID(const std::string &param)
{
   Integer id = type * ESTIMATION_TYPE_ALLOCATION; // Base for estimation ID

   try
   {
      Integer parmID = GetParameterID(param);

      // Handle special cases that do not directly manipulate the parameter
      if (param == "Cd")
         parmID = CD_EPSILON;
      if (param == "Cr")
         parmID = CR_EPSILON;

      id += parmID;
   }
   catch (BaseException &)
   {
      return -1;
   }

   return id;
}


std::string Spacecraft::GetParameterNameForEstimationParameter(const std::string &parmName)
{
   if (parmName == "Cd")
      return (GetParameterText(CD_EPSILON));
   if (parmName == "Cr")
      return (GetParameterText(CR_EPSILON));

   return SpaceObject::GetParameterNameForEstimationParameter(parmName);
}

std::string Spacecraft::GetParameterNameFromEstimationParameter(const std::string &parmName)
{
   if (parmName == "Cd_Epsilon")
      return (GetParameterText(CD_ID));
   if (parmName == "Cr_Epsilon")
      return (GetParameterText(CR_ID));

   return SpaceObject::GetParameterNameFromEstimationParameter(parmName);
}


//------------------------------------------------------------------------------
// bool IsEstimationParameterValid(const Integer item)
//------------------------------------------------------------------------------
/**
* This function is used to verify an estimation paramter is either valid or not
*
* @param item      Estimation parameter ID (Note that: it is defferent from object ParameterID)
*
* return           true if it is valid, false otherwise 
*/
//------------------------------------------------------------------------------
bool Spacecraft::IsEstimationParameterValid(const Integer item)
{
   bool retval = false;

   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;    // convert Estimation ID to object parameter ID
   
   switch (id)
   {
      case CARTESIAN_X:                      // It is compared to Spacecraft CARTESIAN_X parameter's ID
         retval = true;
         break;
      
      case CR_EPSILON:
         retval = true;
         break;

      case CD_EPSILON:
         retval = true;
         break;

      case Gmat::MASS_FLOW:          /// Is it correct ???? Spacecraft::SC_Param_ID::MASS_FLOW or Gmat::MASS_FLOW ???
         // todo: Access tanks for mass information to handle mass flow
         break;

      // All other values call up the hierarchy
      default:
         retval = SpaceObject::IsEstimationParameterValid(item);
         break;
   }

   return retval;
}


//------------------------------------------------------------------------------
// Integer GetEstimationParameterSize(const Integer item)
//------------------------------------------------------------------------------
Integer Spacecraft::GetEstimationParameterSize(const Integer item)
{
   Integer retval = 1;


   Integer id = (item > ESTIMATION_TYPE_ALLOCATION ?
                 item - type * ESTIMATION_TYPE_ALLOCATION : item);

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Spacecraft::GetEstimationParameterSize(%d)"
            " called; parameter ID is %d\n", item, id);
   #endif


   switch (id)
   {
      case CARTESIAN_X:
         retval = 6;
         break;
      case CD_ID:
         retval = 1;
         break;
      case CR_ID:
         retval = 1;
         break;

      case Gmat::MASS_FLOW:
         // todo: Access tanks for mass information to handle mass flow
         break;

      // All other values call up the hierarchy
      default:
         retval = SpaceObject::GetEstimationParameterSize(item);
         break;
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

      case CD_ID:
         retval = &coeffDrag;
         break;

      case CR_ID:
         retval = &reflectCoeff;
         break;

//      case Gmat::MASS_FLOW:
//         // todo: Access tanks for mass information to handle mass flow
//         break;

      // All other values call up the class heirarchy
      default:
         retval = SpaceObject::GetEstimationParameterValue(item);
         break;
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
      UnsignedInt objType = obj->GetType();

      // updates for PowerSystem
      if (objType == Gmat::POWER_SYSTEM)
      {
         if (powerSystem && (obj->GetName() == powerSystemName))
         {
            powerSystem->operator=((*(PowerSystem*) obj));
            if (isInitialized)
               isInitialized = powerSystem->IsInitialized();
         }
      }

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
      Attitude *newAtt = (Attitude*) (obj->Clone());
//      attitude = (Attitude*)(obj->Clone());
      bool attitudeOK = SetAttitudeAndCopyData(attitude, newAtt, true);
      if (!attitudeOK)
      {
         std::string errmsg = "Cannot copy attitude data to new attitude object!\n";
         throw SpaceObjectException(errmsg);
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
      UnsignedInt objType = obj->GetType();

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

      // updates for Power System
      if (objType == Gmat::POWER_SYSTEM)
         if (powerSystem && (obj->GetName() == powerSystemName))
            theClone = powerSystem;

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
      MessageInterface::ShowMessage("newMass = %12le, Mass change = %.12le; depleting \n",
            newMass, massChange);
   #endif

   // Find the active thruster(s) and THF tanks
   ObjectArray activeThrusters;
   ObjectArray thrustHistoryTanks;

   RealArray   flowrate;
   Real        totalFlow = 0.0, rate;
   for (ObjectArray::iterator i = thrusters.begin(); i != thrusters.end(); ++i)
   {
      if ((*i)->GetBooleanParameter("IsFiring"))
      {
         activeThrusters.push_back(*i);
         rate = ((Thruster*)(*i))->CalculateMassFlow();
         #ifdef DEBUG_MASS_FLOW
            MessageInterface::ShowMessage("Thruster %s returned %12.10f\n",
                  (*i)->GetName().c_str(), rate);
         #endif
         flowrate.push_back(rate);
         totalFlow += rate;
      }
   }

   for (ObjectArray::iterator i = tanks.begin(); i != tanks.end(); ++i)
   {
      if (((FuelTank*)(*i))->NoThrusterNeeded())
      {
         thrustHistoryTanks.push_back(*i);
      }
   }

   // Divide the mass flow between the tanks on each activeThrusters thruster
   UnsignedInt numberFiring = activeThrusters.size();
   UnsignedInt numberDraining = thrustHistoryTanks.size();

   if ((numberFiring <= 0) && (numberDraining <= 0) && (massChange != 0.0))
   {
      std::stringstream errmsg;
      errmsg.precision(15);
      errmsg << "Mass update " << massChange
             << " requested for " << instanceName
             << " but there are no active thrusters or thrust file tanks";
      throw SpaceObjectException(errmsg.str());
   }

   // For now, don't allow both modes
   if ((numberFiring > 0) && (numberDraining > 0))
   {
      std::stringstream errmsg;
      errmsg.precision(15);
      errmsg << "Mass update " << massChange
             << " requested for " << instanceName
             << " from thrusters and from a thrust file.  That mode is not "
             << "currently supported.";
      throw SpaceObjectException(errmsg.str());
   }

   Real dm;

   // todo: Check: This loop needs to be updated to split apart the flow based on plumbing
   for (UnsignedInt i = 0; i < activeThrusters.size(); ++i)
   {
      // Change the mass in each attached tank
      ObjectArray usedTanks = activeThrusters[i]->GetRefObjectArray(Gmat::HARDWARE);
      
      if (!GmatMathUtil::IsEqual(totalFlow,0.0))
      {
         dm = massChange * flowrate[i] / totalFlow;

         #ifdef DEBUG_MASS_FLOW
            MessageInterface::ShowMessage("flowrate = %12.10f, totalFlow = %12.10f\n",
                  flowrate[i], totalFlow);
            MessageInterface::ShowMessage("%.12le from %s = [ ", dm, activeThrusters[i]->GetName().c_str());
         #endif

            Rvector mixRatio = activeThrusters[i]->GetRvectorParameter("MixRatio");
         Real mixTotal = 0.0;
         for (UnsignedInt imix = 0; imix < mixRatio.GetSize(); ++imix)
            mixTotal += mixRatio[imix];
         Real dmt = dm / mixTotal;
         for (UnsignedInt j = 0; j < usedTanks.size(); ++j)
         {
            #ifdef DEBUG_MASS_FLOW
               MessageInterface::ShowMessage(" %.12le ", dmt);
            #endif
            usedTanks[j]->SetRealParameter("FuelMass",
                  (usedTanks[j]->GetRealParameter("FuelMass")) + dmt * mixRatio[j]);
         }
      }
      #ifdef DEBUG_MASS_FLOW
         MessageInterface::ShowMessage(" ] ");
      #endif
   }

   if (thrustHistoryTanks.size() > 0)
   {
      #ifdef DEBUG_MASS_FLOW
         MessageInterface::ShowMessage("Depleting mass thruster free: [");
      #endif

      dm = massChange / thrustHistoryTanks.size();
      for (UnsignedInt i = 0; i < thrustHistoryTanks.size(); ++i)
      {
         #ifdef DEBUG_MASS_FLOW
            MessageInterface::ShowMessage(" %.12le ", dm);
         #endif

         thrustHistoryTanks[i]->SetRealParameter("FuelMass",
               thrustHistoryTanks[i]->GetRealParameter("FuelMass") + dm);
      }
      #ifdef DEBUG_MASS_FLOW
         MessageInterface::ShowMessage(" ] ");
      #endif
   }

   return retval;
}


//------------------------------------------------------------------------------
// void DeleteOwnedObjects(bool deleteAttitude, bool deleteTanks, bool deleteThrusters,
//                         bool deletePowerSystem, bool otherHardware)
//------------------------------------------------------------------------------
/*
 * Deletes owned objects, such as attitude, tanks, and thrusters, and power system(s)
 */
//------------------------------------------------------------------------------
void Spacecraft::DeleteOwnedObjects(bool deleteAttitude, bool deleteTanks,
                                    bool deleteThrusters, bool deletePowerSystem,
                                    bool otherHardware)
{
   #ifdef DEBUG_DELETE_OWNED_OBJ
   MessageInterface::ShowMessage
      ("Spacecraft::DeleteOwnedObjects() <%p>'%s' entered, deleteAttitude=%d, "
       "deleteTanks=%d, deleteThrusters=%d, deletePowerSystem =%d, otherHardware=%d\n", this,
       GetName().c_str(), deleteAttitude, deleteTanks, deleteThrusters, deletePowerSystem,
       otherHardware);
   MessageInterface::ShowMessage(" **** Entering DeleteOwnedObjects,  powerSystem is <%p>\n", powerSystem);
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
            ("Spacecraft::DeleteOwnedObjects() <%p>'%s' deleted attitude, ownedObjectCount=%d\n",
             this, GetName().c_str(), ownedObjectCount);
         #endif
      }
   }
   // delete power system
   if (deletePowerSystem)
   {
      if (powerSystem)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (powerSystem, "powerSystem", "Spacecraft::DeleteOwnedObjects()",
             "deleting powerSystem of " + GetName(), this);
         #endif
         delete powerSystem;
         powerSystem = NULL;
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
   MessageInterface::ShowMessage(" **** Exiting DeleteOwnedObjects,  powerSystem is <%p>\n", powerSystem);
   #endif
}


//------------------------------------------------------------------------------
// void CloneOwnedObjects(Attitude *att, const ObjectArray &tnks,
//                        const ObjectArray &thrs, PowerSystem *pwrSys)
//------------------------------------------------------------------------------
/*
 * Clones input tanks and thrusters set as attached hardware.
 */
//------------------------------------------------------------------------------
void Spacecraft::CloneOwnedObjects(Attitude *att, const ObjectArray &tnks,
                                   const ObjectArray &thrs, PowerSystem *pwrSys,
                                   const ObjectArray &otherHardware)   // made changes on 09/23/2014
{
   #ifdef DEBUG_OBJ_CLONE
   MessageInterface::ShowMessage
      ("Spacecraft::CloneOwnedObjects() <%p>'%s' entered, att=<%p>, powerSystem <%p>, tank count = %d,"
       " thruster count = %d\n", this, GetName().c_str(), att, powerSystem, tnks.size(), thrs.size());
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

      bool attitudeOK = SetAttitudeAndCopyData(att, attitude, false);
      if (!attitudeOK)
      {
         std::string errmsg = "Cannot copy attitude data to new attitude object!\n";
         throw SpaceObjectException(errmsg);
      }
      #ifdef DEBUG_SC_OWNED_OBJECT
      MessageInterface::ShowMessage
         ("Spacecraft::CloneOwnedObjects() ------> new CLONED attitude <%p> is of type %s\n",
          attitude, attitude->GetTypeName().c_str());
      #endif


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

   // handle power system
   if (pwrSys)
   {
      GmatBase *clonedPwrSys = pwrSys->Clone();
      if (clonedPwrSys)
      {
         #ifdef DEBUG_SC_OWNED_OBJECT
         MessageInterface::ShowMessage
            ("Spacecraft::CloneOwnedObjects() cloning power system <%p>", pwrSys);
         #endif
         if (powerSystem) delete powerSystem;
         powerSystem = NULL;
         powerSystem = (PowerSystem*) clonedPwrSys;
         powerSystem->SetSolarSystem(solarSystem);
         powerSystem->SetSpacecraft(this);
         powerSystemName = powerSystem->GetName();
      }
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
   #ifdef DEBUG_OBJ_CLONE
   MessageInterface::ShowMessage
      ("Spacecraft::CloneOwnedObjects() <%p>'%s' EXITING, att=<%p>, powerSystem <%p>, tank count = %d,"
       " thruster count = %d\n", this, GetName().c_str(), att, powerSystem, tnks.size(), thrs.size());
   #endif

   // made changes on 09/23/2014
   // Clone other hardware
   for (UnsignedInt i = 0; i < otherHardware.size(); ++i)
   {
      // Search otherHardware[i] in hardwareList
      UnsignedInt j;
      for(j = 0; j < hardwareList.size(); ++j)
      {
         if (hardwareList[j]->GetName() == otherHardware[i]->GetName())
            break;
      }

      // If not found, clone otherHardware[i] and add to hardwareList
      if (j == hardwareList.size())
      {
         hardwareList.push_back(otherHardware[i]->Clone());
      }
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

//         bool isFound = false;
//         ObjectArray::iterator i;
//         for (i = hwArray.begin(); i != hwArray.end(); ++i)
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
//               isFound = true;
               #ifdef DEBUG_MEMORY
               MemoryTracker::Instance()->Remove
                  (old, old->GetName(), "Spacecraft::SetHardware()",
                   "deleting old cloned " + objType, this);
               #endif
//               delete old;
               // we don't want to delete this here, as it may still be pointed to.
               // this will be deleted later on in SetRefObject
               #ifdef DEBUG_SC_REF_OBJECT
               MessageInterface::ShowMessage
                  ("      Adding pointer %s<%p> to obsoleteObjects\n",
                        objName.c_str(), old);
               #endif
               obsoleteObjects.push_back(old);
               old = NULL;
               break;
            }
         }
//         if (isFound)
//            hwArray.erase(i);

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
// bool SetPowerSystem(GmatBase *obj, std::string &psName)
//------------------------------------------------------------------------------
bool Spacecraft::SetPowerSystem(GmatBase *obj, std::string &psName)
{
   #ifdef DEBUG_POWER_SYSTEM
      MessageInterface::ShowMessage("Entering SetPowerSystem with obj <%p> and name %s\n",
            obj, psName.c_str());
   #endif
   if (powerSystem != (PowerSystem*) obj)
   {
      if (powerSystemName == psName)
      {
         if (powerSystem) delete powerSystem;
         powerSystem = (PowerSystem*) obj->Clone();
         powerSystem->SetSolarSystem(solarSystem);
         powerSystem->SetSpacecraft(this);
      }
      else
      {
         #ifdef DEBUG_SC_REF_OBJECT
         MessageInterface::ShowMessage
            ("   The name '%s' not found in list\n", psName.c_str());
         #endif
      }
   }
   else
   {
      #ifdef DEBUG_SC_REF_OBJECT
      MessageInterface::ShowMessage
         ("   The same %s <%p> found in pointer list\n", psName.c_str());
      #endif
   }
   return true;    // or false?
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
   parmOrder[parmIndex++] = POWER_SYSTEM_ID;
   if (ephemerisName != "")
      parmOrder[parmIndex++] = EPHEMERIS_NAME;
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
   
   std::string fieldComment = "";
   std::string inlineFieldComment = "";
   for (i = 0; i < parameterCount; ++i)
   {
      if ((IsParameterReadOnly(parmOrder[i]) == false) &&
          (parmOrder[i] != J2000_BODY_NAME) &&
          (parmOrder[i] != TOTAL_MASS_ID)   &&
          (parmOrder[i] != STATE_TYPE_ID)   &&         // deprecated
          (parmOrder[i] != ATTITUDE))
      {
         parmType = GetParameterType(parmOrder[i]);
         
         if (mode != Gmat::OBJECT_EXPORT)
         {
            fieldComment = GetAttributeCommentLine(parmOrder[i]);
            inlineFieldComment = GetInlineAttributeComment(parmOrder[i]);
         }

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
      else
      {
         #ifdef DEBUG_WRITE_PARAMETERS
         MessageInterface::ShowMessage
            ("==> parmOrder[%d] = %d, '%s' is not handled\n",
             i, parmOrder[i], GetParameterText(parmOrder[i]).c_str());
         MessageInterface::ShowMessage
            ("==> It is %Read-Only\n", IsParameterReadOnly(parmOrder[i]) ? "" : "NOT ");
         #endif
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
void Spacecraft::UpdateElementLabels(const std::string &displayStateType)
{
   #ifdef DEBUG_MULTIMAP
   MessageInterface::ShowMessage("==> Spacecraft::UpdateElementLabels() using state labels multimap\n");
   #endif

   // Check if displayStateType found in the labels map
   if (stateElementLabelsMap.find(displayStateType) != stateElementLabelsMap.end())
      stateElementLabel = stateElementLabelsMap[displayStateType];
   else
   {
      throw SpaceObjectException
         ("*** INTERNAL ERROR *** The state element labels map has not been built for \"" +
          displayStateType + "\"\n");
   }

   // Check if displayStateType found in the units map
   if (stateElementUnitsMap.find(displayStateType) != stateElementUnitsMap.end())
      stateElementUnits = stateElementUnitsMap[displayStateType];
   else
   {
      throw SpaceObjectException
         ("*** INTERNAL ERROR *** The state element units map has not been built for \"" +
          displayStateType + "\"\n");
   }
   }


//------------------------------------------------------------------------------
// Rvector6 GetStateInRepresentation(std::string rep, bool useDefaultCartesian = false)
//------------------------------------------------------------------------------
/**
 * Code used to obtain a state in a non-Cartesian representation.
 */
//------------------------------------------------------------------------------
Rvector6 Spacecraft::GetStateInRepresentation(const std::string &rep, bool useDefaultCartesian)
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

      if (hasPrecisionTime)
         coordConverter.Convert(GetEpochGT(), inState, internalCoordSystem, csState, 
            coordinateSystem);
      else
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
   std::string newRep = rep; // LOJ: I made rep to be const std::string &rep (2014.04.17)
   if (rep == "")
      //rep = stateType;   // do I want displayStateType here?
      newRep = stateType;   // do I want displayStateType here?

   //if (rep == "Cartesian")
   if (newRep == "Cartesian")
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
                                       //rep.c_str());
            newRep.c_str());
         MessageInterface::ShowMessage("Spacecraft::GetStateInRepresentation(string): Using originMu = %12.10f\n", originMu);
      #endif
         //finalState = StateConversionUtil::Convert(csState, "Cartesian", rep,
      finalState = StateConversionUtil::Convert(csState, "Cartesian", newRep,
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
// void SetStateFromRepresentation(std::string rep, Rvector6 &st, ...)
//------------------------------------------------------------------------------
/**
 * Code used to obtain a state in a non-Cartesian representation.
 */
//------------------------------------------------------------------------------
void Spacecraft::SetStateFromRepresentation(const std::string &rep, Rvector6 &st,
                                            const std::string &label)
{
   #ifdef DEBUG_STATE_INTERFACE
      MessageInterface::ShowMessage(
         "Spacecraft::SetStateFromRepresentation: Setting %s state to %s, label = '%s'\n",
         rep.c_str(), st.ToString(16).c_str(), label.c_str());
   #endif

   if (internalCoordSystem == NULL)
      throw SpaceObjectException(" The spacecraft internal coordinate system is not set");
   if (coordinateSystem == NULL)
      throw SpaceObjectException(" The spacecraft coordinate system is not set");
   
   // Do validation on coordinate system (for GMT-4512 fix)
   if (rep == "Planetodetic")
   {
      #ifdef DEBUG_STATE_INTERFACE
      MessageInterface::ShowMessage
         ("The rep is Planetodetic, now checking for spacecraft coordiante system\n");
      #endif
      // If spacecraft coordinate system is not of BodyFixed axes, throw an error
      if (!((coordinateSystem->GetAxisSystem())->IsOfType("BodyFixedAxes")))
         throw SpaceObjectException
            ("The Planetodetic state type is only defined for coordinate systems with BodyFixed axes");
   }
   
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
   MessageInterface::ShowMessage
      ("Spacecraft::SetStateFromRepresentation: state has been converted\n");
   MessageInterface::ShowMessage
      ("   Now convert to internal CS, internalCoordSystem=<%p>, coordinateSystem=<%p>\n",
       internalCoordSystem, coordinateSystem);
   #endif

   // Then convert to the internal CS
   if (internalCoordSystem != coordinateSystem)
   {
      #ifdef DEBUG_STATE_INTERFACE
      if (hasPrecisionTime)
      MessageInterface::ShowMessage
            ("   cs is not InternalCS, so calling coordConverter.Convert() at epochGT %s\n",
            GetEpochGT().ToString().c_str());
      else
         MessageInterface::ShowMessage
         ("   cs is not InternalCS, so calling coordConverter.Convert() at epoch %.12lf\n",
          GetEpoch());
      #endif
      if (hasPrecisionTime)
         coordConverter.Convert(GetEpochGT(), csState, coordinateSystem, finalState, 
            internalCoordSystem);
      else
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
      ("\nIn SC::SetElement, <%p> '%s', label=%s, value=%.12f\n", this,
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
         {
            #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
               MessageInterface::ShowMessage
                  (" ************ In SC::SetElement, leaving stateType as Equinoctial\n");
            #endif
            // leave state as Equinoctial
         }
         else if ( (stateType == "AlternateEquinoctial") && (rep == "Equinoctial") &&
                   (label == "MLONG") )
         {
            #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
               MessageInterface::ShowMessage
                  (" ************ In SC::SetElement, leaving stateType as AlternateEquinoctial\n");
            #endif
            // leave state as AlternateEquinoctial
         }
         else if ( (stateType == "IncomingAsymptote") && (rep == "Keplerian") &&
                   (label == "TA") )
         {
            #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
               MessageInterface::ShowMessage
                  (" ************ In SC::SetElement, leaving stateType as IncomingAsymptote\n");
            #endif
            // leave it as IncomingAsymptote
         }
         else if ( (stateType == "OutgoingAsymptote") && (rep == "Keplerian") &&
                   (label == "TA") )
         {
            #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
               MessageInterface::ShowMessage
                  (" ************ In SC::SetElement, leaving stateType as OutgoingAsymptote\n");
            #endif
            // leave it as OutgoingAsymptote
         }
         else
         {
            #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
               MessageInterface::ShowMessage
                  (" ************ In SC::SetElement, MODIFYING stateType from %s to %s\n",
                    stateType.c_str(), rep.c_str());
            #endif
            stateType = rep;
            #ifdef DEBUG_SC_INPUT_TYPES
            MessageInterface::ShowMessage
               ("==> SC::SetElement() stateType set to %s for the label %s\n", stateType.c_str(), label.c_str());
            #endif
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
      {
         #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
         MessageInterface::ShowMessage
            ("   CS is not set so calling SetPossibleInputTypes(%s, %s)\n",
             label.c_str(), stateType.c_str());
         #endif
         SetPossibleInputTypes(label, stateType);
      }

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
   {
      MessageInterface::ShowMessage
         ("In SC::SetElement, after LookUpLabel, id+ELEMENT1_ID = %d, its "
          "string = \"%s\",  and rep = \"%s\"\n", id+ELEMENT1_ID,
          (GetParameterText(id+ELEMENT1_ID)).c_str(), rep.c_str());
      MessageInterface::ShowMessage
         ("In SC::SetElement, after LookUpLabel, its label = \"%s\" and its value = %12.10f\n",
               label.c_str(), value);
   }
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
            SetStateFromRepresentation(rep, tempState, label);
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

   
   // Use multimap to find out the element id
   #ifdef DEBUG_LOOK_UP_LABEL
   MessageInterface::ShowMessage("   LookUpLabel() using stateElementLabelsMap\n");
   #endif

   std::map<std::string, StringArray>::iterator iter;
   bool done = false;
   for (iter = stateElementLabelsMap.begin(); iter != stateElementLabelsMap.end(); ++iter)
   {
      StringArray labels = (*iter).second;
      for (UnsignedInt i = 0; i < labels.size(); i++)
      {
         #ifdef DEBUG_LOOK_UP_LABEL_MORE
         MessageInterface::ShowMessage("   => labels[%d] = %s\n", i, labels[i].c_str());
         #endif
         if (labels[i] == label)
         {
            std::string stateType = (*iter).first;
            // Handle shared labels with default type
            // If label not found in the defaultStateTypeMap or
            // label found in the defaultStateTypeMap and the type is default type
            // then set label id and rep
            if ((defaultStateTypeMap.find(label) == defaultStateTypeMap.end()) ||
                ((defaultStateTypeMap.find(label) != defaultStateTypeMap.end()) &&
                 (stateType == defaultStateTypeMap[label])))
            {
               // Add ELEMENT1_ID to return relative ID from ELEMENT1_ID in the SC_Param_ID
               retval = i + ELEMENT1_ID;
               rep = (*iter).first;
               done = true;
               break;
            }
         }
      }
      if (done)
         break;
   }

   #ifdef DEBUG_LOOK_UP_LABEL
      MessageInterface::ShowMessage("Spacecraft::LookUpLabel(%s..) gives rep %s with retval = %d\n",
         label.c_str(), rep.c_str(), retval);
   #endif

   return retval;
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
      return &fullSTM;
   if (parameterId == CD_EPSILON)
      return &fullSTM;
   if (parameterId == CR_EPSILON)
      return &fullSTM;
//      return &orbitSTM;

   return SpaceObject::GetParameterSTM(parameterId);
}


//------------------------------------------------------------------------------
// Integer GetStmRowId(const Integer forRow)
//------------------------------------------------------------------------------
/**
 * Retrieves the ID associated with a given row/column of the STM
 *
 * Note that since the STM is of the form d(r(t)) / d(r(t_o)), the numerator
 * setting for each row matches the denominator setting for each column.
 *
 * @param forRow The associated row (for the numerator) or column (denominator)
 *
 * @return The ID
 */
//------------------------------------------------------------------------------
Integer Spacecraft::GetStmRowId(const Integer forRow)
{
   Integer retval = -1;

   if ((forRow < (Integer)stmIndices.size()) && (forRow >= 0))
      retval = stmIndices[forRow];

   return retval;
}

//-------------------------------------------------------------------------
// Integer Spacecraft::HasParameterCovariances(Integer parameterId)
//-------------------------------------------------------------------------
/**
* This function is used to verify whether a parameter (with ID specified by 
* parameterId) having a covariance or not.
*
* @param parameterId      ID of a parameter
* @return                 size of covarian matrix associated with the parameter
*                         return -1 when the parameter has no covariance
*/
//-------------------------------------------------------------------------
Integer Spacecraft::HasParameterCovariances(Integer parameterId)
{
   // Dimension of spacraft's Cartesian state has to be 6.
   // If dimension is set to fullSTMRowCount, it will get wrong
   // when fullSTMRowCount accidently has a value different from 6.
   if (parameterId == CARTESIAN_X)
//      return fullSTMRowCount;
      return 6;

   if (parameterId == CR_EPSILON)
      return 1;

   if (parameterId == CD_EPSILON)
      return 1;

   return SpaceObject::HasParameterCovariances(parameterId);
}


//------------------------------------------------------------------------------
// Rmatrix* GetParameterCovariances(Integer parameterId)
//------------------------------------------------------------------------------
/**
* Get convariance of a give Spacecraft's parameter
*
* @param paramId     The Id of a spacecraft's parameter
*
* @return            Covariance matrix of the parameter specified by the 
*                    parameter Id
*/
//------------------------------------------------------------------------------
Rmatrix* Spacecraft::GetParameterCovariances(Integer parameterId)
{
   if (isInitialized)
      return covariance.GetCovariance(parameterId);
   else
      throw GmatBaseException("Error: cannot get " + GetName() + " spacecraft's covariance when it is not initialized.\n");
   return NULL;
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


void Spacecraft::RecomputeStateAtEpochGT(const GmatTime &toEpoch)
{
   if (internalCoordSystem != coordinateSystem)
   {
      // First convert from the internal CS to the spacecraft's CS at the old epoch
      Rvector6 inState(state.GetState());
      Rvector6 csState;
      Rvector6 finalState;
      coordConverter.Convert(GetEpochGT(), inState, internalCoordSystem, csState,
         coordinateSystem);
      // Then convert back at the new epoch
      GmatTime newEpoch = toEpoch;
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
   UnsignedInt type;
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
               check = false;
               //MessageInterface::ShowMessage
               //   ("***Error***:primary antenna of %s in %s's AddHardware list is not set.\n", 
               //    obj->GetName().c_str(), this->GetName().c_str());
               throw GmatBaseException("Error: Primary antenna of " + obj->GetName() + " in " 
                                       + GetName() + ".AddHardware list is not set.\n");
            }
            else
            {
               if (antennaList.empty())
                  throw GmatBaseException("Error: primary antenna of " + obj->GetName()
                                          + "is not set into " + GetName() + ".AddHardware\n");

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
                  //MessageInterface::ShowMessage 
                  //   ("***Error***:primary antenna of %s is not in %s's AddHardware\n",
                  //    obj->GetName().c_str(), this->GetName().c_str());
                  throw GmatBaseException("Error: Primary antenna of " + obj->GetName()
                     + " in " + GetName() + ".AddHardware list is not set.\n");
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
   MessageInterface::ShowMessage
      ("\nEntering SetPossibleInputTypes: spacecraft = '%s', label = '%s', rep = '%s'\n",
            instanceName.c_str(), label.c_str(), rep.c_str());
      MessageInterface::ShowMessage("possibleInputTypes are:\n");
      for (unsigned int ii = 0; ii < possibleInputTypes.size(); ii++)
         MessageInterface::ShowMessage("      %2d    %s\n", ii, possibleInputTypes.at(ii).c_str());
   MessageInterface::ShowMessage
      ("stateType = %s, uniqueStateTypeFound=%d\n", stateType.c_str(), uniqueStateTypeFound);
   #endif

   
   #ifdef DEBUG_MULTIMAP
   MessageInterface::ShowMessage
      ("==> Spacecraft::SetPossibleInputTypes() using state labels multimap\n");
   #endif
   
   int numRepCount = allElementLabelsMultiMap.count(label);
   std::string defaultStateType;
   StringArray tempInputTypes;
   
   #ifdef DEBUG_SC_INPUT_TYPES
   MessageInterface::ShowMessage("There are %d state reps with label '%s'\n", numRepCount, label.c_str());
   #endif
   
   // Check for valid label
   if (numRepCount < 1)
      throw SpaceObjectException("*** INTERNAL ERROR *** Found invalid element: \"" + label + "\"");
   
   // Get the range of state reps with element label
   std::pair<std::multimap<std::string, std::string>::iterator, std::multimap<std::string, std::string>::iterator> rangeIter;
   rangeIter = allElementLabelsMultiMap.equal_range(label);
   
   // Set default state rep to first one
   defaultStateType = (*(rangeIter.first)).second;
   
   // Loop through range of map of labels
   for (std::multimap<std::string, std::string>::iterator it2 = rangeIter.first;
        it2 != rangeIter.second; ++it2)
   {
      std::string tempStateType = (*it2).second;
      #ifdef DEBUG_SC_INPUT_TYPES
      MessageInterface::ShowMessage("   %s\n", tempStateType.c_str());
      #endif
      tempInputTypes.push_back(tempStateType);
   }
   
   // Update possible input types
   if (!uniqueStateTypeFound && possibleInputTypes.size() > 0)
   {
      std::string possibleType = possibleInputTypes[0];
      for (StringArray::iterator iter = possibleInputTypes.begin(); iter < possibleInputTypes.end(); ++iter)
      {
         StringArray labels = GetStateElementLabels(*iter);
         // If label not found in any of possible types then remove
         if (find(labels.begin(), labels.end(), label) == labels.end())
         {
            #ifdef DEBUG_SC_INPUT_TYPES
      MessageInterface::ShowMessage
               ("'%s' not found, so removing the type '%s' from possible input types\n", label.c_str(), (*iter).c_str());
            #endif
            iter = possibleInputTypes.erase(iter);
         }
      }
      
      // Check for empty possible input types
      if (possibleInputTypes.empty())
      {
         #ifdef DEBUG_SC_INPUT_TYPES
         MessageInterface::ShowMessage
            ("**** ERROR The the possible input type is empty for the label %s, "
             "so throwing exception\n", label.c_str());
         #endif
         throw SpaceObjectException("Error determining input state type for the element label \"" + label + "\".\n");
      }
      
      // If there is only one type left, set it as unique state type
      if (possibleInputTypes.size() == 1)
      {
         stateType = possibleInputTypes[0];
         uniqueStateTypeFound = true;
         #ifdef DEBUG_SC_INPUT_TYPES
         MessageInterface::ShowMessage("==> 1 stateType set to %s for the label %s\n", stateType.c_str(), label.c_str());
         #endif
      }
   }
   
   // If unique state type found, check for invalid element label
   if (uniqueStateTypeFound)
   {
      if (possibleInputTypes.size() == 1 &&
          find(tempInputTypes.begin(), tempInputTypes.end(), possibleInputTypes[0]) == tempInputTypes.end())
      {
         #ifdef DEBUG_SC_INPUT_TYPES
         MessageInterface::ShowMessage
         ("**** ERROR The label '%s' is not allowed in the the rep '%s', so throwing exception\n",
          label.c_str(), rep.c_str());
      #endif
      std::string errmsg = "Error: you have set orbital state elements not contained in the same state type.  ";
      errmsg += "This is only allowed after the BeginMissionSequence command.\n";
      throw SpaceObjectException(errmsg);
   }
   }
  
   // If there is only one state rep, clear and add it to possibleInputTypes
   if (numRepCount == 1)
   {
      #ifdef DEBUG_SC_INPUT_TYPES
      MessageInterface::ShowMessage
         ("Unique state type found for label '%s', uniqueStateTypeFound = %d\n", label.c_str(),
          uniqueStateTypeFound);
      #endif
   
      if (!uniqueStateTypeFound)
   {
         possibleInputTypes.clear();
         possibleInputTypes.push_back(defaultStateType);
         stateType = defaultStateType;
         uniqueStateTypeFound = true;
         #ifdef DEBUG_SC_INPUT_TYPES
         MessageInterface::ShowMessage("==> 2 stateType set to %s for the label %s\n", stateType.c_str(), label.c_str());
         #endif
   }
   }
   else
   {
      #ifdef DEBUG_SC_INPUT_TYPES
      MessageInterface::ShowMessage("Multiple state types found for label '%s'\n", label.c_str());
      #endif
      
      // If unique state type not found, add the state rep to possible types
      if (!uniqueStateTypeFound)
   {
         // Loop through range of temp state type
         for (unsigned int i = 0; i < tempInputTypes.size(); i++)
   {
            std::string tempType = tempInputTypes[i];
            if (find(possibleInputTypes.begin(), possibleInputTypes.end(), tempType) == possibleInputTypes.end())
               possibleInputTypes.push_back(tempType);
   }
   }
   }   
   
   if (possibleInputTypes.size() < 1)
   {
      throw SpaceObjectException("Error determining input state type.\n");
   }
   
   
   #ifdef DEBUG_SC_INPUT_TYPES
   MessageInterface::ShowMessage
      ("----> leaving SetPossibleInputTypes: spacecraft = %s, label = %s, rep = %s, stateType = %s\n",
       instanceName.c_str(), label.c_str(), rep.c_str(), stateType.c_str());
      MessageInterface::ShowMessage("----> possibleInputTypes are now:\n");
      for (unsigned int ii = 0; ii < possibleInputTypes.size(); ii++)
         MessageInterface::ShowMessage("---->       %d    %s\n", ii, possibleInputTypes.at(ii).c_str());
   #endif
}

//-------------------------------------------------------------------------
// bool ValidateOrbitStateValue(const std::string &forRep, const std::string &withLabel,
//                              Real andValue,             bool checkCoupled)
//-------------------------------------------------------------------------
bool Spacecraft::ValidateOrbitStateValue(const std::string &forRep, const std::string &withLabel,
                                         Real andValue, bool checkCoupled)
{
   #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
   MessageInterface::ShowMessage(
         "In SC::ValidateOrbitStateValue, about to validate %s with value %le for rep = %s, checkCoupled = %s\n",
         withLabel.c_str(), andValue, forRep.c_str(), (checkCoupled? "true" : "false"));
   #endif

   if (!checkCoupled)
   {
      #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
      MessageInterface::ShowMessage("In SC::ValidateOrbitStateValue, about to validate (no coupled element check) %s with value %le\n", withLabel.c_str(), andValue);
      #endif
      return StateConversionUtil::ValidateValue(withLabel, andValue, errorMessageFormat, GetDataPrecision());
   }

   // On option, check coupled elements
   bool validated = false;
   // if we're setting RadApo, and RadPer has been set, also check for value relative to RadPer
   if ((forRep == "ModifiedKeplerian") && (withLabel == "RadApo") && (state[0] != UNSET_ELEMENT_VALUE))
   {
      #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
      MessageInterface::ShowMessage("In SC::ValidateOrbitStateValue, about to validate %s with value %le when RadPer already set\n", withLabel.c_str(), andValue);
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
   else if ((forRep == "Equinoctial") && (withLabel == "EquinoctialH") && (state[2] != UNSET_ELEMENT_VALUE))
   {
      validated = StateConversionUtil::ValidateValue(withLabel, andValue, errorMessageFormat, GetDataPrecision(), "EquinoctialK", state[2]);
   }
   else if ((forRep == "Equinoctial") && (withLabel == "EquinoctialK") && (state[1] != UNSET_ELEMENT_VALUE))
   {
      validated = StateConversionUtil::ValidateValue(withLabel, andValue, errorMessageFormat, GetDataPrecision(), "EquinoctialH", state[1]);
   }
   else if ((forRep == "Delaunay") && (withLabel == "DelaunayH") && (state[4] != UNSET_ELEMENT_VALUE))
   {
      validated = StateConversionUtil::ValidateValue(withLabel, andValue, errorMessageFormat, GetDataPrecision(), "DelaunayG", state[4]);
   }
   else if ((forRep == "Delaunay") && (withLabel == "DelaunayG"))
   {
      try
      {
         if (state[5] != UNSET_ELEMENT_VALUE)
         {
            validated = StateConversionUtil::ValidateValue(withLabel, andValue, errorMessageFormat, GetDataPrecision(), "DelaunayH", state[5]);
         }
         if (state[3] != UNSET_ELEMENT_VALUE)
         {
            validated = StateConversionUtil::ValidateValue(withLabel, andValue, errorMessageFormat, GetDataPrecision(), "DelaunayL", state[3]);
         }
         // if neither of those throw an exception, check the DelaunayG value by itself
         validated = StateConversionUtil::ValidateValue(withLabel, andValue, errorMessageFormat, GetDataPrecision());
      }
      catch (BaseException &be)
      {
         // ValidateValue really doesn't return false when it fails; it just throws an exception
         throw;
      }
   }
   else if ((forRep == "Delaunay") && (withLabel == "DelaunayL") && (state[4] != UNSET_ELEMENT_VALUE))
   {
      validated = StateConversionUtil::ValidateValue(withLabel, andValue, errorMessageFormat, GetDataPrecision(), "DelaunayG", state[4]);
   }
   // Otherwise, check the value
   else
   {
      #ifdef DEBUG_SPACECRAFT_SET_ELEMENT
      MessageInterface::ShowMessage("In SC::ValidateOrbitStateValue, about to validate %s with value %le\n", withLabel.c_str(), andValue);
      #endif
         validated = StateConversionUtil::ValidateValue(withLabel, andValue, errorMessageFormat, GetDataPrecision());
   }
   return validated;
}

//-------------------------------------------------------------------------
// bool SetAttitudeAndCopyData(Attitude *oldAtt, Attitude *newAtt,
//                             bool deleteOldAtt = false)
//-------------------------------------------------------------------------
bool Spacecraft::SetAttitudeAndCopyData(Attitude *oldAtt, Attitude *newAtt,
                                        bool deleteOldAtt)
{
   #ifdef DEBUG_SC_ATTITUDE_DATA
      MessageInterface::ShowMessage("In SetAttitudeAndCopyData with oldAtt %s<%p> and newAtt %s<%p>\n",
            (oldAtt->GetAttitudeModelName()).c_str(), oldAtt,
            (newAtt->GetAttitudeModelName()).c_str(), newAtt);
   #endif
   if (oldAtt != NULL)
   {
      // Get reference objects from old attitude object:

      // Get reference coordinate system from old attitude object
      std::string oldAttCSName     = oldAtt->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      GmatBase    *oldAttCS        = oldAtt->GetRefObject(Gmat::COORDINATE_SYSTEM, oldAttCSName);
      // Get reference body from old attitude object
      std::string oldAttBodyName   = oldAtt->GetRefObjectName(Gmat::CELESTIAL_BODY);
      GmatBase    *oldAttBody      = oldAtt->GetRefObject(Gmat::CELESTIAL_BODY, oldAttBodyName);

      #ifdef DEBUG_SC_ATTITUDE_DATA
         MessageInterface::ShowMessage("   old CS name = %s, old body name = %s\n",
               oldAttCSName.c_str(), oldAttBodyName.c_str());
         if (!oldAttCS) MessageInterface::ShowMessage("   oldAttCS is NULL!!!\n");
         if (!oldAttBody) MessageInterface::ShowMessage("   oldAttBody is NULL!!!\n");
      #endif

      // Set the reference objects on the new attitude
      newAtt->SetRefObjectName(Gmat::COORDINATE_SYSTEM, oldAttCSName);
      newAtt->SetRefObject(oldAttCS, Gmat::COORDINATE_SYSTEM, oldAttCSName);
      newAtt->SetRefObjectName(Gmat::CELESTIAL_BODY, oldAttBodyName);
      newAtt->SetRefObject(oldAttBody, Gmat::CELESTIAL_BODY, oldAttBodyName);

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

      if (deleteOldAtt)
      {
         delete oldAtt;
         ownedObjectCount--;
      }
   }
   newAtt->SetEpoch(state.GetEpoch());
   newAtt->SetEpochGT(state.GetEpochGT());
   newAtt->SetPrecisionTimeFlag(hasPrecisionTime);

   newAtt->SetOwningSpacecraft(this);
   newAtt->NeedsReinitialization();

   // Set attitude to the new one
   attitude = newAtt;
   ownedObjectCount++;

   attitudeModel = newAtt->GetAttitudeModelName();

   return true;
}

//------------------------------------------------------------------------------
// void BuildStateElementLabelsAndUnits()
//------------------------------------------------------------------------------
/**
 * Builds map for state element labels and units for each state type
 */
//------------------------------------------------------------------------------
void Spacecraft::BuildStateElementLabelsAndUnits()
{
   StringArray elementLabels;
   StringArray elementUnits;
   elementLabels.assign(6, "");
   elementUnits.assign(6, "");
   
   // Clear the multimap before refilling it
   allElementLabelsMultiMap.clear();

   //-----------------------------------
   // Cartesian
   //-----------------------------------
   elementLabels[0] = "X";
   elementLabels[1] = "Y";
   elementLabels[2] = "Z";
   elementLabels[3] = "VX";
   elementLabels[4] = "VY";
   elementLabels[5] = "VZ";
   
   elementUnits[0] = "km";
   elementUnits[1] = "km";
   elementUnits[2] = "km";
   elementUnits[3] = "km/s";
   elementUnits[4] = "km/s";
   elementUnits[5] = "km/s";
   
   stateElementLabelsMap["Cartesian"] = elementLabels;
   stateElementUnitsMap["Cartesian"] = elementUnits;
   
   // Build all labels to state type map
   allElementLabelsMultiMap.insert(std::make_pair("X",  "Cartesian"));
   allElementLabelsMultiMap.insert(std::make_pair("Y",  "Cartesian"));
   allElementLabelsMultiMap.insert(std::make_pair("Z",  "Cartesian"));
   allElementLabelsMultiMap.insert(std::make_pair("VX", "Cartesian"));
   allElementLabelsMultiMap.insert(std::make_pair("VY", "Cartesian"));
   allElementLabelsMultiMap.insert(std::make_pair("VZ", "Cartesian"));
   
   //-----------------------------------
   // Keplerian   
   //-----------------------------------
   elementLabels[0] = "SMA";
   elementLabels[1] = "ECC";
   elementLabels[2] = "INC";
   elementLabels[3] = "RAAN";
   elementLabels[4] = "AOP";
   elementLabels[5] = anomalyType;
   
   elementUnits[0] = "km";
   elementUnits[1] = "";
   elementUnits[2] = "deg";
   elementUnits[3] = "deg";
   elementUnits[4] = "deg";
   elementUnits[5] = "deg";
   
   stateElementLabelsMap["Keplerian"] = elementLabels;
   stateElementUnitsMap["Keplerian"] = elementUnits;

   allElementLabelsMultiMap.insert(std::make_pair("SMA",  "Keplerian"));
   allElementLabelsMultiMap.insert(std::make_pair("ECC",  "Keplerian"));
   allElementLabelsMultiMap.insert(std::make_pair("INC",  "Keplerian"));
   allElementLabelsMultiMap.insert(std::make_pair("RAAN", "Keplerian"));
   allElementLabelsMultiMap.insert(std::make_pair("AOP",  "Keplerian"));
   allElementLabelsMultiMap.insert(std::make_pair("TA",   "Keplerian"));
   allElementLabelsMultiMap.insert(std::make_pair("EA",   "Keplerian"));
   allElementLabelsMultiMap.insert(std::make_pair("MA",   "Keplerian"));
   allElementLabelsMultiMap.insert(std::make_pair("HA",   "Keplerian"));
   
   //-----------------------------------
   // ModifiedKeplerian
   //-----------------------------------
   elementLabels[0] = "RadPer";
   elementLabels[1] = "RadApo";
   elementLabels[2] = "INC";
   elementLabels[3] = "RAAN";
   elementLabels[4] = "AOP";
   elementLabels[5] = anomalyType;
   
   elementUnits[0] = "km";
   elementUnits[1] = "km";
   elementUnits[2] = "deg";
   elementUnits[3] = "deg";
   elementUnits[4] = "deg";
   elementUnits[5] = "deg";
   
   stateElementLabelsMap["ModifiedKeplerian"] = elementLabels;
   stateElementUnitsMap["ModifiedKeplerian"] = elementUnits;
   
   allElementLabelsMultiMap.insert(std::make_pair("RadPer", "ModifiedKeplerian"));
   allElementLabelsMultiMap.insert(std::make_pair("RadApo", "ModifiedKeplerian"));
   allElementLabelsMultiMap.insert(std::make_pair("INC",    "ModifiedKeplerian"));
   allElementLabelsMultiMap.insert(std::make_pair("RAAN",   "ModifiedKeplerian"));
   allElementLabelsMultiMap.insert(std::make_pair("AOP",    "ModifiedKeplerian"));
   allElementLabelsMultiMap.insert(std::make_pair("TA",     "ModifiedKeplerian"));
   allElementLabelsMultiMap.insert(std::make_pair("EA",     "ModifiedKeplerian"));
   allElementLabelsMultiMap.insert(std::make_pair("MA",     "ModifiedKeplerian"));
   allElementLabelsMultiMap.insert(std::make_pair("HA",     "ModifiedKeplerian"));
   
   //-----------------------------------
   // SphericalAZFPA
   //-----------------------------------
   elementLabels[0] = "RMAG";
   elementLabels[1] = "RA";
   elementLabels[2] = "DEC";
   elementLabels[3] = "VMAG";
   elementLabels[4] = "AZI";
   elementLabels[5] = "FPA";
   
   elementUnits[0] = "km";
   elementUnits[1] = "deg";
   elementUnits[2] = "deg";
   elementUnits[3] = "km/s";
   elementUnits[4] = "deg";
   elementUnits[5] = "deg";
   
   stateElementLabelsMap["SphericalAZFPA"] = elementLabels;
   stateElementUnitsMap["SphericalAZFPA"] = elementUnits;
   
   allElementLabelsMultiMap.insert(std::make_pair("RMAG", "SphericalAZFPA"));
   allElementLabelsMultiMap.insert(std::make_pair("RA",   "SphericalAZFPA"));
   allElementLabelsMultiMap.insert(std::make_pair("DEC",  "SphericalAZFPA"));
   allElementLabelsMultiMap.insert(std::make_pair("VMAG", "SphericalAZFPA"));
   allElementLabelsMultiMap.insert(std::make_pair("AZI",  "SphericalAZFPA"));
   allElementLabelsMultiMap.insert(std::make_pair("FPA",  "SphericalAZFPA"));
   
   //-----------------------------------
   // SphericalRADEC
   //-----------------------------------   
   elementLabels[0] = "RMAG";
   elementLabels[1] = "RA";
   elementLabels[2] = "DEC";
   elementLabels[3] = "VMAG";
   elementLabels[4] = "RAV";
   elementLabels[5] = "DECV";
   
   elementUnits[0] = "km";
   elementUnits[1] = "deg";
   elementUnits[2] = "deg";
   elementUnits[3] = "km/s";
   elementUnits[4] = "deg";
   elementUnits[5] = "deg";
   
   stateElementLabelsMap["SphericalRADEC"] = elementLabels;
   stateElementUnitsMap["SphericalRADEC"] = elementUnits;
   
   allElementLabelsMultiMap.insert(std::make_pair("RMAG", "SphericalRADEC"));
   allElementLabelsMultiMap.insert(std::make_pair("RA",   "SphericalRADEC"));
   allElementLabelsMultiMap.insert(std::make_pair("DEC",  "SphericalRADEC"));
   allElementLabelsMultiMap.insert(std::make_pair("VMAG", "SphericalRADEC"));
   allElementLabelsMultiMap.insert(std::make_pair("RAV",  "SphericalRADEC"));
   allElementLabelsMultiMap.insert(std::make_pair("DECV", "SphericalRADEC"));
   
   //-----------------------------------
   // Equinoctial
   //-----------------------------------
   elementLabels[0] = "SMA";
   elementLabels[1] = "EquinoctialH";
   elementLabels[2] = "EquinoctialK";
   elementLabels[3] = "EquinoctialP";
   elementLabels[4] = "EquinoctialQ";
   elementLabels[5] = "MLONG";
   
   elementUnits[0] = "km";
   elementUnits[1] = "";
   elementUnits[2] = "";
   elementUnits[3] = "";
   elementUnits[4] = "";
   elementUnits[5] = "deg";
   
   stateElementLabelsMap["Equinoctial"] = elementLabels;
   stateElementUnitsMap["Equinoctial"] = elementUnits;
   
   allElementLabelsMultiMap.insert(std::make_pair("SMA",          "Equinoctial"));
   allElementLabelsMultiMap.insert(std::make_pair("EquinoctialH", "Equinoctial"));
   allElementLabelsMultiMap.insert(std::make_pair("EquinoctialK", "Equinoctial"));
   allElementLabelsMultiMap.insert(std::make_pair("EquinoctialP", "Equinoctial"));
   allElementLabelsMultiMap.insert(std::make_pair("EquinoctialQ", "Equinoctial"));
   allElementLabelsMultiMap.insert(std::make_pair("MLONG",        "Equinoctial"));
   
   //-----------------------------------
   // ModifiedEquinoctial
   //-----------------------------------
   elementLabels[0] = "SemilatusRectum";
   elementLabels[1] = "ModEquinoctialF";
   elementLabels[2] = "ModEquinoctialG";
   elementLabels[3] = "ModEquinoctialH";
   elementLabels[4] = "ModEquinoctialK";
   elementLabels[5] = "TLONG";
   
   elementUnits[0] = "km";
   elementUnits[1] = "";
   elementUnits[2] = "";
   elementUnits[3] = "";
   elementUnits[4] = "";
   elementUnits[5] = "deg";
   
   stateElementLabelsMap["ModifiedEquinoctial"] = elementLabels;
   stateElementUnitsMap["ModifiedEquinoctial"] = elementUnits;
   
   allElementLabelsMultiMap.insert(std::make_pair("SemilatusRectum", "ModifiedEquinoctial"));
   allElementLabelsMultiMap.insert(std::make_pair("ModEquinoctialF", "ModifiedEquinoctial"));
   allElementLabelsMultiMap.insert(std::make_pair("ModEquinoctialG", "ModifiedEquinoctial"));
   allElementLabelsMultiMap.insert(std::make_pair("ModEquinoctialH", "ModifiedEquinoctial"));
   allElementLabelsMultiMap.insert(std::make_pair("ModEquinoctialK", "ModifiedEquinoctial"));
   allElementLabelsMultiMap.insert(std::make_pair("TLONG",           "ModifiedEquinoctial"));
   
   //-----------------------------------
   // AlternateEquinoctial
   //-----------------------------------
   elementLabels[0] = "SMA";
   elementLabels[1] = "EquinoctialH";
   elementLabels[2] = "EquinoctialK";
   elementLabels[3] = "AltEquinoctialP";
   elementLabels[4] = "AltEquinoctialQ";
   elementLabels[5] = "MLONG";
   
   elementUnits[0] = "km";
   elementUnits[1] = "";
   elementUnits[2] = "";
   elementUnits[3] = "";
   elementUnits[4] = "";
   elementUnits[5] = "deg";
   
   stateElementLabelsMap["AlternateEquinoctial"] = elementLabels;
   stateElementUnitsMap["AlternateEquinoctial"] = elementUnits;
   
   allElementLabelsMultiMap.insert(std::make_pair("SMA",             "AlternateEquinoctial"));
   allElementLabelsMultiMap.insert(std::make_pair("EquinoctialH",    "AlternateEquinoctial"));
   allElementLabelsMultiMap.insert(std::make_pair("EquinoctialK",    "AlternateEquinoctial"));
   allElementLabelsMultiMap.insert(std::make_pair("AltEquinoctialP", "AlternateEquinoctial"));
   allElementLabelsMultiMap.insert(std::make_pair("AltEquinoctialQ", "AlternateEquinoctial"));
   allElementLabelsMultiMap.insert(std::make_pair("MLONG",           "AlternateEquinoctial"));
   
   //-----------------------------------
   // Delaunay
   //-----------------------------------
   elementLabels[0] = "Delaunayl";
   elementLabels[1] = "Delaunayg";
   elementLabels[2] = "Delaunayh";
   elementLabels[3] = "DelaunayL";
   elementLabels[4] = "DelaunayG";
   elementLabels[5] = "DelaunayH";
   
   elementUnits[0] = "deg";
   elementUnits[1] = "deg";
   elementUnits[2] = "deg";
   elementUnits[3] = "km^2/sec";
   elementUnits[4] = "km^2/sec";
   elementUnits[5] = "km^2/sec";
   
   stateElementLabelsMap["Delaunay"] = elementLabels;
   stateElementUnitsMap["Delaunay"] = elementUnits;
   
   allElementLabelsMultiMap.insert(std::make_pair("Delaunayl", "Delaunay"));
   allElementLabelsMultiMap.insert(std::make_pair("Delaunayg", "Delaunay"));
   allElementLabelsMultiMap.insert(std::make_pair("Delaunayh", "Delaunay"));
   allElementLabelsMultiMap.insert(std::make_pair("DelaunayL", "Delaunay"));
   allElementLabelsMultiMap.insert(std::make_pair("DelaunayG", "Delaunay"));
   allElementLabelsMultiMap.insert(std::make_pair("DelaunayH", "Delaunay"));
   
   //-----------------------------------
   // Planetodetic
   //-----------------------------------
   elementLabels[0] = "PlanetodeticRMAG";
   elementLabels[1] = "PlanetodeticLON";
   elementLabels[2] = "PlanetodeticLAT";
   elementLabels[3] = "PlanetodeticVMAG";
   elementLabels[4] = "PlanetodeticAZI";
   elementLabels[5] = "PlanetodeticHFPA";
   
   elementUnits[0] = "km";
   elementUnits[1] = "deg";
   elementUnits[2] = "deg";
   elementUnits[3] = "km/sec";
   elementUnits[4] = "deg";
   elementUnits[5] = "deg";
   
   stateElementLabelsMap["Planetodetic"] = elementLabels;
   stateElementUnitsMap["Planetodetic"] = elementUnits;
   
   allElementLabelsMultiMap.insert(std::make_pair("PlanetodeticRMAG", "Planetodetic"));
   allElementLabelsMultiMap.insert(std::make_pair("PlanetodeticLON",  "Planetodetic"));
   allElementLabelsMultiMap.insert(std::make_pair("PlanetodeticLAT",  "Planetodetic"));
   allElementLabelsMultiMap.insert(std::make_pair("PlanetodeticVMAG", "Planetodetic"));
   allElementLabelsMultiMap.insert(std::make_pair("PlanetodeticAZI",  "Planetodetic"));
   allElementLabelsMultiMap.insert(std::make_pair("PlanetodeticHFPA", "Planetodetic"));
   
   //-----------------------------------
   // IncomingAsymptote
   //-----------------------------------
   elementLabels[0] = "IncomingRadPer";
   elementLabels[1] = "IncomingC3Energy";
   elementLabels[2] = "IncomingRHA";
   elementLabels[3] = "IncomingDHA";
   elementLabels[4] = "IncomingBVAZI";
   elementLabels[5] = "TA";
   
   elementUnits[0] = "km";
   elementUnits[1] = "km^2/sec^2";
   elementUnits[2] = "deg";
   elementUnits[3] = "deg";
   elementUnits[4] = "deg";
   elementUnits[5] = "deg";
   
   stateElementLabelsMap["IncomingAsymptote"] = elementLabels;
   stateElementUnitsMap["IncomingAsymptote"] = elementUnits;
   
   allElementLabelsMultiMap.insert(std::make_pair("IncomingRadPer",   "IncomingAsymptote"));
   allElementLabelsMultiMap.insert(std::make_pair("IncomingC3Energy", "IncomingAsymptote"));
   allElementLabelsMultiMap.insert(std::make_pair("IncomingRHA",      "IncomingAsymptote"));
   allElementLabelsMultiMap.insert(std::make_pair("IncomingDHA",      "IncomingAsymptote"));
   allElementLabelsMultiMap.insert(std::make_pair("IncomingBVAZI",    "IncomingAsymptote"));
   allElementLabelsMultiMap.insert(std::make_pair("TA",               "IncomingAsymptote"));
   
   //-----------------------------------
   // OutgoingAsymptote
   //-----------------------------------
   elementLabels[0] = "OutgoingRadPer";
   elementLabels[1] = "OutgoingC3Energy";
   elementLabels[2] = "OutgoingRHA";
   elementLabels[3] = "OutgoingDHA";
   elementLabels[4] = "OutgoingBVAZI";
   elementLabels[5] = "TA";
   
   elementUnits[0] = "km";
   elementUnits[1] = "km^2/sec^2";
   elementUnits[2] = "deg";
   elementUnits[3] = "deg";
   elementUnits[4] = "deg";
   elementUnits[5] = "deg";
   
   stateElementLabelsMap["OutgoingAsymptote"] = elementLabels;
   stateElementUnitsMap["OutgoingAsymptote"] = elementUnits;
   
   allElementLabelsMultiMap.insert(std::make_pair("OutgoingRadPer",   "OutgoingAsymptote"));
   allElementLabelsMultiMap.insert(std::make_pair("OutgoingC3Energy", "OutgoingAsymptote"));
   allElementLabelsMultiMap.insert(std::make_pair("OutgoingRHA",      "OutgoingAsymptote"));
   allElementLabelsMultiMap.insert(std::make_pair("OutgoingDHA",      "OutgoingAsymptote"));
   allElementLabelsMultiMap.insert(std::make_pair("OutgoingBVAZI",    "OutgoingAsymptote"));
   allElementLabelsMultiMap.insert(std::make_pair("TA",               "OutgoingAsymptote"));
   
   //-----------------------------------
   // BrouwerMeanShort
   //-----------------------------------
   elementLabels[0] = "BrouwerShortSMA";
   elementLabels[1] = "BrouwerShortECC";
   elementLabels[2] = "BrouwerShortINC";
   elementLabels[3] = "BrouwerShortRAAN";
   elementLabels[4] = "BrouwerShortAOP";
   elementLabels[5] = "BrouwerShortMA";
   
   elementUnits[0] = "km";
   elementUnits[1] = "";
   elementUnits[2] = "deg";
   elementUnits[3] = "deg";
   elementUnits[4] = "deg";
   elementUnits[5] = "deg";
   
   stateElementLabelsMap["BrouwerMeanShort"] = elementLabels;
   stateElementUnitsMap["BrouwerMeanShort"] = elementUnits;
   
   allElementLabelsMultiMap.insert(std::make_pair("BrouwerShortSMA",  "BrouwerMeanShort"));
   allElementLabelsMultiMap.insert(std::make_pair("BrouwerShortECC",  "BrouwerMeanShort"));
   allElementLabelsMultiMap.insert(std::make_pair("BrouwerShortINC",  "BrouwerMeanShort"));
   allElementLabelsMultiMap.insert(std::make_pair("BrouwerShortRAAN", "BrouwerMeanShort"));
   allElementLabelsMultiMap.insert(std::make_pair("BrouwerShortAOP",  "BrouwerMeanShort"));
   allElementLabelsMultiMap.insert(std::make_pair("BrouwerShortMA",   "BrouwerMeanShort"));
   
   //-----------------------------------
   // BrouwerMeanLong
   //-----------------------------------
   elementLabels[0] = "BrouwerLongSMA";
   elementLabels[1] = "BrouwerLongECC";
   elementLabels[2] = "BrouwerLongINC";
   elementLabels[3] = "BrouwerLongRAAN";
   elementLabels[4] = "BrouwerLongAOP";
   elementLabels[5] = "BrouwerLongMA";
   
   elementUnits[0] = "km";
   elementUnits[1] = "";
   elementUnits[2] = "deg";
   elementUnits[3] = "deg";
   elementUnits[4] = "deg";
   elementUnits[5] = "deg";
   
   stateElementLabelsMap["BrouwerMeanLong"] = elementLabels;
   stateElementUnitsMap["BrouwerMeanLong"] = elementUnits;
   
   allElementLabelsMultiMap.insert(std::make_pair("BrouwerLongSMA",  "BrouwerMeanLong"));
   allElementLabelsMultiMap.insert(std::make_pair("BrouwerLongECC",  "BrouwerMeanLong"));
   allElementLabelsMultiMap.insert(std::make_pair("BrouwerLongINC",  "BrouwerMeanLong"));
   allElementLabelsMultiMap.insert(std::make_pair("BrouwerLongRAAN", "BrouwerMeanLong"));
   allElementLabelsMultiMap.insert(std::make_pair("BrouwerLongAOP",  "BrouwerMeanLong"));
   allElementLabelsMultiMap.insert(std::make_pair("BrouwerLongMA",   "BrouwerMeanLong"));
   
   // Build default state type map for shared labels
   defaultStateTypeMap["AOP"]          = "Keplerian";
   defaultStateTypeMap["DEC"]          = "SphericalAZFPA";
   defaultStateTypeMap["EquinoctialH"] = "Equinoctial";
   defaultStateTypeMap["EquinoctialK"] = "Equinoctial";
   defaultStateTypeMap["INC"]          = "Keplerian";
   defaultStateTypeMap["MLONG"]        = "Equinoctial";
   defaultStateTypeMap["RA"]           = "SphericalAZFPA";
   defaultStateTypeMap["RAAN"]         = "Keplerian";
   defaultStateTypeMap["RMAG"]         = "SphericalAZFPA";
   defaultStateTypeMap["SMA"]          = "Keplerian";
   defaultStateTypeMap["TA"]           = "Keplerian";
   defaultStateTypeMap["VMAG"]         = "SphericalAZFPA";
}


//------------------------------------------------------------------------------
// void Spacecraft::IsManeuvering()
//------------------------------------------------------------------------------
/**
 * Toggles the isManeuvering flag
 *
 * @param mnvrFlag The desired maneuverign state
 *
 * @return The actual maneuvering state
 */
//------------------------------------------------------------------------------
void Spacecraft::IsManeuvering(bool mnvrFlag)
{
   if (mnvrFlag == false)
   {
      // Check for active thrusters and toggle the flag is all are inactive
      bool setting = false;
      for (UnsignedInt i = 0; i < thrusters.size(); ++i)
         setting |= thrusters[i]->GetBooleanParameter(thrusters[i]->GetParameterID("IsFiring"));
      isManeuvering = setting | mnvrFlag;
   }
   else
      isManeuvering = mnvrFlag;
}


Rmatrix66 Spacecraft::GetCoordinateSystemTransformMatrix()
{
   if (!isCSTransformMatrixSet)
   {
      if (internalCoordSystem != coordinateSystem)
      {
         Rvector inputState(6, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
         Rvector outputState(6);
         if (hasPrecisionTime)
            coordConverter.Convert(GetEpochGT(), inputState, coordinateSystem, outputState, 
               internalCoordSystem);
         else
            coordConverter.Convert(GetEpoch(), inputState, coordinateSystem, outputState,
               internalCoordSystem);
         Rmatrix33 r = coordConverter.GetLastRotationMatrix();
         Rmatrix33 rdot = coordConverter.GetLastRotationDotMatrix();

         // Set value for top 3 lines in the matrix
         for (Integer i = 0; i < 3; ++i)
         {
            for (Integer j = 0; j < 6; ++j)
            {
               if (j < 3)
                  csTransformMatrix(i, j) = r(i, j);
               else
                  csTransformMatrix(i, j) = 0.0;
            }
         }

         // Set value for bottom 3 lines in the matrix
         for (Integer i = 3; i < 6; ++i)
         {
            for (Integer j = 0; j < 6; ++j)
            {
               if (j < 3)
                  csTransformMatrix(i, j) = rdot(i - 3, j);
               else
                  csTransformMatrix(i, j) = r(i - 3, j - 3);
            }
         }
      }
      else
      {
         // Set to 6x6 identity matrix
         for (Integer i = 0; i < 6; ++i)
         {
            for (Integer j = 0; j < 6; ++j)
            {
               csTransformMatrix(i, j) = ((i == j) ? 1.0 : 0.0);
            }
         }
      }
   }

   return csTransformMatrix;
}


bool Spacecraft::SetPrecisionTimeFlag(bool onOff)
{
   // When epoch precision is not changed, state will be the same
   // When precision of epoch changes from low to high or from high to low, it needs to recompute the state
   if ((onOff == true) && (hasPrecisionTime == false))
      RecomputeStateDueToChangeOfEpochPrecision(true);
   else if ((onOff == false) && (hasPrecisionTime == true))
      RecomputeStateDueToChangeOfEpochPrecision(false);

   return SpaceObject::SetPrecisionTimeFlag(onOff);
}


void Spacecraft::RecomputeStateDueToChangeOfEpochPrecision(bool fromLowToHi)
{
#ifdef DEBUG_CHANGE_EPOCH_PRECISION
   MessageInterface::ShowMessage("Enter Spacecraft::RecomputeStateDueToChangeOfEpochPrecision(%s)\n", (fromLowToHi?"true":"false"));
   if (fromLowToHi)
      MessageInterface::ShowMessage("Epoch precision changes from low to high\n");
   else
      MessageInterface::ShowMessage("Epoch precision changes from high to low\n");
#endif

   Rvector6 csState;
   Rvector6 finalState(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

   // 1. Get internal state
   Rvector6 inState(state.GetState());

#ifdef DEBUG_CHANGE_EPOCH_PRECISION
   MessageInterface::ShowMessage("   internal CS = <%s,%p>  spacecraft's CS = <%s,%p>\n", internalCoordSystem->GetName().c_str(), internalCoordSystem, coordinateSystem->GetName().c_str(), coordinateSystem);
   MessageInterface::ShowMessage("   at epoch = %.12lf   epochGT = %s\n", GetEpoch(), GetEpochGT().ToString().c_str());
   MessageInterface::ShowMessage("   internal state = [%.12lf   %.12lf   %.12lf   %.12lf   %.12lf   %.12lf]\n", inState[0], inState[1], inState[2], inState[3], inState[4], inState[5]);
#endif

   // 2. Correction state due to the change of epoch precision
   if (fromLowToHi)
   {
      // Case 1: precision changes from low to high:
      // Convert state from internal coordinate system to object coordinate system in low precision epoch
      coordConverter.Convert(GetEpoch(), inState, internalCoordSystem, csState, coordinateSystem);
      
      // Convert state from object coordinate system to internal coordinate system in high precision epoch
      coordConverter.Convert(GetEpochGT(), csState, coordinateSystem, finalState, internalCoordSystem);
   }
   else
   {
      // Case 2: precision changes from high to low:
      // Convert state from internal coordinate system to object coordinate system in high precision epoch
      coordConverter.Convert(GetEpochGT(), inState, internalCoordSystem, csState, coordinateSystem);

      // Convert state from object coordinate system to internal coordinate system in low precision epoch
      coordConverter.Convert(GetEpoch(), csState, coordinateSystem, finalState, internalCoordSystem);
   }

   // 3. Update state in the internal coordinate system
   state[0] = finalState[0];
   state[1] = finalState[1];
   state[2] = finalState[2];
   state[3] = finalState[3];
   state[4] = finalState[4];
   state[5] = finalState[5];

#ifdef DEBUG_CHANGE_EPOCH_PRECISION
   MessageInterface::ShowMessage("   final state = [%.12lf   %.12lf   %.12lf   %.12lf   %.12lf   %.12lf]\n", state[0], state[1], state[2], state[3], state[4], state[5]);
   MessageInterface::ShowMessage("Exit Spacecraft::RecomputeStateDueToChangeOfEpochPrecision(%s)\n", (fromLowToHi ? "true" : "false"));
#endif

}

