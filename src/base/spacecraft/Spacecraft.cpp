//$Header$
//------------------------------------------------------------------------------
//                                  Spacecraft 
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Joey Gurganus, Reworked by D. Conway
// Created: 2003/10/22
// 
/**
 * Implements the Spacecraft base class. 
 */ 
//------------------------------------------------------------------------------

#include "Spacecraft.hpp"
#include "MessageInterface.hpp"
#include <sstream>

//#define DEBUG_SPACECRAFT 1 
//#define DEBUG_SPACECRAFT_SET 1 
//#define DEBUG_SPACECRAFT_CS 1 
//#define DEBUG_RENAME 1
//#define DEBUG_DATE_FORMAT
//#define DEBUG_STATE_INTERFACE

#if DEBUG_SPACECRAFT
#include <iostream>
#include <sstream> 
#endif


#define TIME_PRECISION 18

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
      Gmat::STRING_TYPE,      // StateType
      Gmat::STRING_TYPE,      // AnomalyType
      Gmat::STRING_TYPE,      // CoordinateSystem
      Gmat::REAL_TYPE,        // DryMass
      Gmat::STRING_TYPE,      // DateFormat
      Gmat::REAL_TYPE,        // Cd
      Gmat::REAL_TYPE,        // Cr
      Gmat::REAL_TYPE,        // DragArea
      Gmat::REAL_TYPE,        // SRPArea
      Gmat::STRINGARRAY_TYPE, // Tanks
      Gmat::STRINGARRAY_TYPE, // Thrusters
      Gmat::REAL_TYPE         // TotalMass
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
   };


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
Spacecraft::Spacecraft(const std::string &name, const std::string &typeStr) :
   SpaceObject          (Gmat::SPACECRAFT, typeStr, name),
   scEpoch              ("21545.000000000"),
   dryMass              (850.0),
   coeffDrag            (2.2),
   dragArea             (15.0),
   srpArea              (1.0),
   reflectCoeff         (1.8),
   epochSystem          ("TAI"),
   epochFormat          ("ModJulian"),
   dateFormat           ("TAIModJulian"),  // Should be A1ModJulian
   stateType            ("Cartesian"),
   anomalyType          ("TA"),
   internalCoordSystem  (NULL),
   coordinateSystem     (NULL),
   coordSysName         ("EarthMJ2000Eq"),
   attitude             (NULL),
   displayEpoch         ("21545.000000000"),   
   displayDateFormat    ("TAIModJulian"),  // Should be A1ModJulian
   totalMass            (850.0),
   initialDisplay       (false),
   csSet                (false)
{
   //MessageInterface::ShowMessage("=====> Spacecraft::Spacecraft(%s) entered\n",
   //                              name.c_str());
   
   objectTypes.push_back(Gmat::SPACECRAFT);
   objectTypeNames.push_back("Spacecraft");

   state.SetEpoch(21545.0);
   
   state[0] = 7100.0;
   state[1] = 0.0;
   state[2] = 1300.0;
   state[3] = 0.0;
   state[4] = 7.35;
   state[5] = 1.0;

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
   
   parameterCount = SpacecraftParamCount;
   
   BuildElementLabelMap();
   
   //MessageInterface::ShowMessage("=====> Spacecraft::Spacecraft(%s) exiting\n",
   //                              name.c_str());
}


//---------------------------------------------------------------------------
//  ~Spacecraft()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
Spacecraft::~Spacecraft()
{
   // Delete the attached hardware (it was set as clones)
   for (ObjectArray::iterator i = tanks.begin(); i < tanks.end(); ++i)
      delete *i;
   for (ObjectArray::iterator i = thrusters.begin(); i < thrusters.end(); ++i)
      delete *i;
}


//---------------------------------------------------------------------------
//  Spacecraft(const Spacecraft &a)
//---------------------------------------------------------------------------
/**
 * Copy Constructor for base Spacecraft structures.
 *
 * @param <a> The original that is being copied.
 */
Spacecraft::Spacecraft(const Spacecraft &a) :
   SpaceObject          (a),
   scEpoch              (a.scEpoch),
   dryMass              (a.dryMass),
   coeffDrag            (a.coeffDrag),
   dragArea             (a.dragArea),
   srpArea              (a.srpArea),
   reflectCoeff         (a.reflectCoeff),
   epochSystem          (a.epochSystem),
   epochFormat          (a.epochFormat),
   dateFormat           (a.dateFormat),
   stateType            (a.stateType),
   anomalyType          (a.anomalyType),
   internalCoordSystem  (a.internalCoordSystem),
   coordinateSystem     (a.coordinateSystem),      // Check this one...
   coordSysName         (a.coordSysName),
   attitude             (a.attitude),              // Check this one too ...
   stateConverter       (a.stateConverter),
   coordConverter       (a.coordConverter),
   displayEpoch         (a.displayEpoch),   
   displayDateFormat    (a.displayDateFormat),
   totalMass            (a.totalMass),
   initialDisplay       (false),
   csSet                (a.csSet)
{
   objectTypes.push_back(Gmat::SPACECRAFT);
   objectTypeNames.push_back("Spacecraft");
   parameterCount = a.parameterCount;

   state.SetEpoch(a.state.GetEpoch());
   
   state[0] = a.state[0];
   state[1] = a.state[1];
   state[2] = a.state[2];
   state[3] = a.state[3];
   state[4] = a.state[4];
   state[5] = a.state[5];
   anomaly  = a.anomaly;
   
   stateElementLabel = a.stateElementLabel;
   stateElementUnits = a.stateElementUnits;
   representations   = a.representations;
   tankNames         = a.tankNames;
   thrusterNames     = a.thrusterNames;

   BuildElementLabelMap();
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
 * 
 * @todo Determine how to handle hardware when copying Spacecraft objects.
 */
Spacecraft& Spacecraft::operator=(const Spacecraft &a)
{
   // Don't do anything if copying self
   if (&a == this)
      return *this;

   SpaceObject::operator=(a);

   scEpoch              = a.scEpoch;
   dryMass              = a.dryMass;
   coeffDrag            = a.coeffDrag;
   dragArea             = a.dragArea;
   srpArea              = a.srpArea;
   reflectCoeff         = a.reflectCoeff;
   epochSystem          = a.epochSystem;
   epochFormat          = a.epochFormat;
   dateFormat           = a.dateFormat;
   stateType            = a.stateType;
   anomalyType          = a.anomalyType;
   coordSysName         = a.coordSysName;
   attitude             = a.attitude,        // correct?
   stateConverter       = a.stateConverter;
   coordConverter       = a.coordConverter;
   displayEpoch         = a.displayEpoch;   
   displayDateFormat    = a.displayDateFormat;
   totalMass            = a.totalMass;
   initialDisplay       = false;
   csSet                = a.csSet;
   anomaly              = a.anomaly;
//   tanks                = a.tanks;
//   thrusters            = a.thrusters;

//   MessageInterface::ShowMessage("Anomaly has type %s, copied from %s\n", 
//      anomaly.GetType().c_str(), a.anomaly.GetType().c_str());

   state.SetEpoch(a.state.GetEpoch());
   
   state[0] = a.state[0];
   state[1] = a.state[1];
   state[2] = a.state[2];
   state[3] = a.state[3];
   state[4] = a.state[4];
   state[5] = a.state[5];
   
   if (a.csSet)
   {
      coordinateSystem = a.coordinateSystem;
      csSet = true;
   }
   
   internalCoordSystem = a.internalCoordSystem;

   stateElementLabel = a.stateElementLabel;
   stateElementUnits = a.stateElementUnits;
   representations   = a.representations;
   tankNames         = a.tankNames;
   thrusterNames     = a.thrusterNames;
   
   BuildElementLabelMap();
   
   return *this;
}


//---------------------------------------------------------------------------
// CoordinateSystem* GetInternalCoordSystem()
//---------------------------------------------------------------------------
CoordinateSystem* Spacecraft::GetInternalCoordSystem()
{
   return internalCoordSystem;
}


//---------------------------------------------------------------------------
// void SetInternalCoordSystem(CoordinateSystem *cs)
//---------------------------------------------------------------------------
void Spacecraft::SetInternalCoordSystem(CoordinateSystem *cs)
{
   #if DEBUG_SPACECRAFT_CS
      MessageInterface::ShowMessage
         ("Spacecraft::SetInternalCoordSystem() cs=%d on %s\n", cs, 
         instanceName.c_str());
   #endif
   
   if (internalCoordSystem != cs)
   {
      internalCoordSystem = cs;
      if (coordinateSystem == NULL)
         coordinateSystem = cs;
   }
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
   #if DEBUG_SPACECRAFT_SET
      MessageInterface::ShowMessage("Spacecraft::SetState(Rvector6)\n");
    /*     ("Spacecraft::SetState(Rvector6) cartesianState=%s\n",
          cartState.ToString().c_str());*/
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
void Spacecraft::SetState(const std::string &elementType, Real *instate)
{
   #if DEBUG_SPACECRAFT
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
      newState = stateConverter.Convert(instate, elementType, 
         stateType, anomaly);
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
//  PropState& GetState() 
//------------------------------------------------------------------------------
/**
 * "Unhide" the SpaceObject method.
 * 
 * @return the core PropState.   
 */
//------------------------------------------------------------------------------
PropState& Spacecraft::GetState()
{
   return SpaceObject::GetState();
}

//------------------------------------------------------------------------------
//  Rvector6 GetState(std::string &rep) 
//------------------------------------------------------------------------------
/**
 * Get the converted Cartesian states from states in different coordinate type.
 * 
 * @return converted Cartesian states   
 */
//------------------------------------------------------------------------------
Rvector6 Spacecraft::GetState(std::string rep) 
{
   #ifdef DEGUG_STATE_INTERFACE
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
 * Get the converted Cartesian states from states in different coordinate type.
 * 
 * @return converted Cartesian states   
 */
//------------------------------------------------------------------------------
Rvector6 Spacecraft::GetState(Integer rep) 
{
   rvState = GetStateInRepresentation(rep);
   return rvState;
}


//------------------------------------------------------------------------------
//  Rvector6 GetCartesianState() 
//------------------------------------------------------------------------------
/**
 * Get the converted Cartesian states from states in different coordinate type.
 * 
 * @return converted Cartesian states   
 */
//------------------------------------------------------------------------------
Rvector6 Spacecraft::GetCartesianState() 
{   
//   Real *tempState = state.GetState();
//
//   for (int i=0; i<6; i++)
//      rvState[i] = tempState[i];
//   
   MessageInterface::ShowMessage("GetCartesianState() is obsolete; "
      "use GetState(\"Cartesian\") or GetState(%d) instead.\n", CARTESIAN_ID);
   return GetState("Cartesian");//rvState;
}

//------------------------------------------------------------------------------
//  Rvector6 GetKeplerianState() 
//------------------------------------------------------------------------------
/**
 * Get the converted Keplerian states from states in different coordinate type.
 * 
 * @return converted Keplerain states   
 */
//------------------------------------------------------------------------------
Rvector6 Spacecraft::GetKeplerianState() 
{
//   rvState = stateConverter.Convert(state.GetState(), stateType,
//                "Keplerian",anomaly);
//
//   return rvState;
   MessageInterface::ShowMessage("GetKeplerianState() is obsolete; "
      "use GetState(\"Keplerian\") or GetState(%d) instead.\n", KEPLERIAN_ID);
   return GetState("Keplerian");
}

//------------------------------------------------------------------------------
//  Rvector6 GetModifiedKeplerianState() 
//------------------------------------------------------------------------------
/**
 * Get the converted Modified Keplerian states from states in different 
 * coordinate type.
 * 
 * @return converted Modified Keplerain states   
 */
//------------------------------------------------------------------------------
Rvector6 Spacecraft::GetModifiedKeplerianState() 
{
//   rvState = stateConverter.Convert(state.GetState(),stateType,
//                "ModifiedKeplerian",anomaly);
//   return (rvState);
   MessageInterface::ShowMessage("GetModifiedKeplerianState() is obsolete; "
      "use GetState(\"ModifiedKeplerian\") or GetState(%d) instead.\n",
      MODIFIED_KEPLERIAN_ID);
   return GetState("ModifiedKeplerian");
}


Anomaly Spacecraft::GetAnomaly() const
{
   return anomaly;
}

Rmatrix33 Spacecraft::GetAttitude(Real a1mjdTime) const
{
   if (attitude) return attitude->GetCosineMatrix(a1mjdTime);
   else 
      return Rmatrix33();  // temporary - return identity matrix
   //   throw SpaceObjectException("No attitude object set for spacecraft.");
}

Rvector3  Spacecraft::GetAngularVelocity(Real a1mjdTime) const
{
   if (attitude) return attitude->GetAngularVelocity(a1mjdTime);
   else 
      return Rvector3(); // temporary - return zero vector
   //   throw SpaceObjectException("No attitude object set for spacecraft.");
}




//------------------------------------------------------------------------------
//  std::string GetDisplayDateFormat() const 
//------------------------------------------------------------------------------
/**
 * Get the display's dateformat of epoch.
 * 
 * @return date format. 
 */
//------------------------------------------------------------------------------
std::string Spacecraft::GetDisplayDateFormat() const 
{
   MessageInterface::ShowMessage("GetDisplayDateFormat() is deprecated; "
      "use GetStringParameter(\"DateFormat\") instead.\n");
//   return displayDateFormat;
   return GetStringParameter("DateFormat");
}

//---------------------------------------------------------------------------
// void SetDisplayDateFormat(const std::string &dateType) 
//---------------------------------------------------------------------------
/**
 * Set the display's dateformat of epoch.
 * 
 * @param <dateType> date type given. 
 */
void Spacecraft::SetDisplayDateFormat(const std::string &dateType) 
{
   MessageInterface::ShowMessage(
      "SetDisplayDateFormat(const std::string &) is deprecated; "
      "use SetDateFormat(const std::string &dateType) instead\n");
      
   SetDateFormat(dateType);
}


//------------------------------------------------------------------------------
// void SetDisplayDateFormat(const std::string &dateType) 
//------------------------------------------------------------------------------
/**
 * Set the display's date format of epoch.
 * 
 * @param <dateType> date type given. 
 */
//------------------------------------------------------------------------------
void Spacecraft::SetDateFormat(const std::string &dateType) 
{
   #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage("Spacecraft::SetDateFormat() "
         "Setting date format to %s; initial epoch is %s\n", 
         dateType.c_str(), scEpoch.c_str());
   #endif
   
   // 1. Save old data
   std::string oldEpochSystem = epochSystem;
   std::string oldEpochFormat = epochFormat;
   
   //MessageInterface::ShowMessage
   //   ("==> dateType=%s, epochSystem=%s, oldEpochFormat=%s\n",
   //    dateType.c_str(), epochSystem.c_str(), oldEpochFormat.c_str());
   
   // 2. Break apart into time type and string format
   Integer loc = dateType.find("ModJulian", 0);
   if (loc == -1)
      loc = dateType.find("Gregorian", 0);
   if (loc == 0)
      throw SpaceObjectException("Error parsing time format '" +
         dateType + "'; could not find 'Gregorian' or 'ModJulian' substring.");
   epochSystem = dateType.substr(0, loc);
   epochFormat = dateType.substr(loc);
   
   #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage
         ("Spacecraft::SetDateFormat() %s breaks into system %s and format %s\n",
         dateType.c_str(), epochSystem.c_str(), epochFormat.c_str());
   #endif
   
   if (!TimeConverterUtil::ValidateTimeSystem(epochSystem))
   {
      std::string badEpochSystem = epochSystem;
      epochSystem = oldEpochSystem;
      epochFormat = oldEpochFormat;
      MessageInterface::ShowMessage("Invalid epoch system:%s\n", badEpochSystem.c_str());
      throw SpaceObjectException("'" + badEpochSystem + 
         "' is not a valid time system");
   }
   
   dateFormat = epochSystem + epochFormat;
   //MessageInterface::ShowMessage("==> dateFormat=%s\n", dateFormat.c_str());
   
   // 3. Generate converted string
   Real startEpoch=-999.999, finalEpoch=-999.999;
   std::stringstream str;
   str.precision(TIME_PRECISION);
   
   //MessageInterface::ShowMessage("==> BEFORE scEpoch=%s, startEpoch=%.10f\n",
   //                              scEpoch.c_str(), startEpoch);
   
   if (oldEpochFormat == "ModJulian")
   {
      str.str(scEpoch);
      str >> startEpoch;
   }
   else
   {
      startEpoch = TimeConverterUtil::ConvertGregorianToMjd(scEpoch);
   }
   
   //MessageInterface::ShowMessage("==> AFTER  scEpoch=%s, startEpoch=%.10f\n",
   //                              scEpoch.c_str(), startEpoch);
   
   #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage(
         "     Converting %s from oldEpochSystem:%s to epochSystem:%s gives", scEpoch.c_str(),
         oldEpochSystem.c_str(), epochSystem.c_str());
   #endif

   if (oldEpochSystem != epochSystem)
   {
      #ifdef DEBUG_DATE_FORMAT
         MessageInterface::ShowMessage("(%s != %s)\n", oldEpochSystem.c_str(), 
            epochSystem.c_str());
      #endif

      // 20.02.06 - arg: changed to use enum types instead of strings
//      finalEpoch = TimeConverterUtil::Convert(startEpoch, oldEpochSystem, 
//         epochSystem, GmatTimeUtil::JD_JAN_5_1941);
    
      Integer oldEpochSysID = TimeConverterUtil::GetTimeTypeID(oldEpochSystem);
      Integer epochSysID = TimeConverterUtil::GetTimeTypeID(epochSystem);
      finalEpoch = TimeConverterUtil::Convert(startEpoch, oldEpochSysID, 
         epochSysID, GmatTimeUtil::JD_JAN_5_1941);

   }
   else
   {
      #ifdef DEBUG_DATE_FORMAT
         MessageInterface::ShowMessage("(%s == %s)\n", oldEpochSystem.c_str(), 
            epochSystem.c_str());
      #endif

      finalEpoch = startEpoch;
   }

   #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage(
         "\nfinal epoch (MJD) = %.10lf;", finalEpoch);
   #endif

   if (epochFormat == "ModJulian")
   {
      std::stringstream epstr("");
      epstr.precision(TIME_PRECISION);
      epstr << finalEpoch;
      
      #ifdef DEBUG_DATE_FORMAT
         MessageInterface::ShowMessage(
            "epoch stream (MJD) = %s;", epstr.str().c_str());
      #endif

      epstr >> scEpoch;
   }
   else
      scEpoch = TimeConverterUtil::ConvertMjdToGregorian(finalEpoch);

   #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage(
         "  resulting epoch is %s\n", scEpoch.c_str());
   #endif
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
      MessageInterface::ShowMessage("Cloning %s (%x) to %x\n", 
         instanceName.c_str(), this, clone);
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
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Spacecraft::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type != Gmat::HARDWARE)
      return true;
   
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
   
   return true;
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * This method returns a name of the referenced objects.
 *
 * @return a name of objects of the requested type.
 */
std::string Spacecraft::GetRefObjectName(const Gmat::ObjectType type) const
{
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      return coordSysName;
   }
   return SpaceObject::GetRefObjectName(type);
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
   static StringArray fullList;  // Maintain scope if the full list is requested

   if (type == Gmat::UNKNOWN_OBJECT)
   {
      fullList.clear();      
      fullList.push_back(coordSysName);
      return fullList;      
   }
   else
   {
      if (type == Gmat::FUEL_TANK)
         return tankNames;
      if (type == Gmat::THRUSTER)
         return thrusterNames;

      if (type == Gmat::HARDWARE) {
         fullList.clear();
         fullList = tankNames;
         for (StringArray::iterator i = thrusterNames.begin();
              i < thrusterNames.end(); ++i)
            fullList.push_back(*i);
         return fullList;
      }
      
      if (type == Gmat::COORDINATE_SYSTEM)
      {
         fullList.clear();
         fullList.push_back(coordSysName);
         return fullList;
      }
   }
   
   return SpaceObject::GetRefObjectNameArray(type);
}


// DJC: Not sure if we need this yet...
bool Spacecraft::SetRefObjectName(const Gmat::ObjectType type, const std::string &name)
{
   if (type == Gmat::COORDINATE_SYSTEM)
   {
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
GmatBase* Spacecraft::GetRefObject(const Gmat::ObjectType type, 
                                   const std::string &name)
{
//MessageInterface::ShowMessage("Accessing ref object on %s of type ", instanceName.c_str());
   // This switch statement intentionally drops through without breaks, so that
   // the search in the tank and thruster name lists only need to be coded once. 
   switch (type)
   {
      case Gmat::COORDINATE_SYSTEM:
//MessageInterface::ShowMessage("CoordinateSystem named %s\n", name.c_str());
         return coordinateSystem;
         
      case Gmat::ATTITUDE:
//MessageInterface::ShowMessage("CoordinateSystem named %s\n", name.c_str());
         return attitude;
         
      case Gmat::HARDWARE:
      case Gmat::FUEL_TANK:
//MessageInterface::ShowMessage("FuelTank named %s\n", name.c_str());
         for (ObjectArray::iterator i = tanks.begin(); 
              i < tanks.end(); ++i) {
            if ((*i)->GetName() == name)
               return *i;
         }
      
      case Gmat::THRUSTER:
//MessageInterface::ShowMessage("Thruster named %s\n", name.c_str());
         for (ObjectArray::iterator i = thrusters.begin(); 
              i < thrusters.end(); ++i) {
            if ((*i)->GetName() == name)
               return *i;
         }
         
         // Other Hardware cases go here...

         return NULL;      // Hardware requested, but not in the hardware lists
         
      default:
         break;
   }

//MessageInterface::ShowMessage("Unknown named %s\n", name.c_str());
   return SpaceObject::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type, 
//                   const std::string &name)
//------------------------------------------------------------------------------
bool Spacecraft::SetRefObject(GmatBase *obj, const Gmat::ObjectType type, 
                              const std::string &name)
{
   if (type == Gmat::HARDWARE) {
      std::string typeStr = obj->GetTypeName();
    
      if (typeStr == "FuelTank") {
         if (find(tanks.begin(), tanks.end(), obj) == tanks.end()) {
            tanks.push_back(obj);
            return true;
         }
         return false;
      }
      
      if (typeStr == "Thruster") {
         if (find(thrusters.begin(), thrusters.end(), obj) == thrusters.end()) {
            thrusters.push_back(obj);
            return true;
         }
         return false;
      }
      
      return false;
   }
   else if (type == Gmat::COORDINATE_SYSTEM)
   {
      CoordinateSystem *cs = (CoordinateSystem*)obj;
      
      #if DEBUG_SPACECRAFT_CS
         MessageInterface::ShowMessage
            ("Spacecraft::SetRefObject() coordinateSystem=%d, cs=%d on %s\n",
             coordinateSystem, cs, instanceName.c_str());
      #endif
      
      if (coordinateSystem != cs)
         coordinateSystem = cs;
         
      TakeAction("ApplyCoordinateSystem");

      return true;
   }
   else if (type == Gmat::ATTITUDE)
   {
      attitude = (Attitude*) obj;
      // set epoch ...
      attitude->SetEpoch(state.GetEpoch());
      return true;
   }
   
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
ObjectArray& Spacecraft::GetRefObjectArray(const Gmat::ObjectType type)
{
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
ObjectArray& Spacecraft::GetRefObjectArray(const std::string& typeString)
{
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
Integer Spacecraft::GetParameterID(const std::string &str) const
{
   #ifdef DEBUG_PARM_PERFORMANCE
      MessageInterface::ShowMessage("Spacecraft::GetParameterID(%s)\n", str.c_str());
   #endif

   if (str == "Element1" || str == "X" || str == "SMA" || str == "RadPer" ||
       str == "RMAG")  
      return ELEMENT1_ID;

   if (str == "Element2" || str == "Y" || str == "ECC" || str == "RadApo" ||
       str == "RA" || str == "PECCY") 
      return ELEMENT2_ID;

   if (str == "Element3" || str == "Z" || str == "INC" || str == "DEC" ||
       str == "PECCX")
      return ELEMENT3_ID;

   if (str == "Element4" || str == "VX" || str == "RAAN" || str == "VMAG" ||
       str == "PNY") 
      return ELEMENT4_ID;

   if (str == "Element5" || str == "VY" || str == "AOP" || str == "AZI" 
       || str == "RAV" || str == "PNX")
      return ELEMENT5_ID;

   if (str == "Element6" || str == "VZ" || str == "TA" || str == "MA" ||
       str == "EA" || str == "FPA" || str == "DECV" || str == "MLONG") 
      return ELEMENT6_ID;

   for (Integer i = SpaceObjectParamCount; i < SpacecraftParamCount; ++i)
   {
      if (str == PARAMETER_LABEL[i - SpaceObjectParamCount])
         return i;
   }
    
   return SpaceObject::GetParameterID(str);
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
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool Spacecraft::IsParameterReadOnly(const Integer id) const
{
   if ((id >= ELEMENT1UNIT_ID) && (id <= ELEMENT6UNIT_ID))
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


std::string Spacecraft::GetParameterText(const Integer id) const
{
   // Handle the dynamic labels for teh elements first
   if (id == ELEMENT1_ID || id == ELEMENT2_ID || id == ELEMENT3_ID 
       || id == ELEMENT4_ID || id == ELEMENT5_ID || id == ELEMENT6_ID)
      return stateElementLabel[id - ELEMENT1_ID];

   if ((id >= SpaceObjectParamCount) && (id < SpacecraftParamCount))
      return PARAMETER_LABEL[id - SpaceObjectParamCount];

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
   if ((id >= SpaceObjectParamCount) && (id < SpacecraftParamCount))
      return PARAMETER_TYPE[id - SpaceObjectParamCount];

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
//    return SpaceObject::PARAM_TYPE_STRING[GetParameterType(id)];
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
Real Spacecraft::GetRealParameter(const Integer id) const
{
   if (id == ELEMENT1_ID) return state[0];
   if (id == ELEMENT2_ID) return state[1];
   if (id == ELEMENT3_ID) return state[2];
   if (id == ELEMENT4_ID) return state[3];
   if (id == ELEMENT5_ID) return state[4];
   if (id == ELEMENT6_ID) return state[5];
          
   if (id == DRY_MASS_ID) return dryMass;
   if (id == CD_ID) return coeffDrag;
   if (id == CR_ID) return reflectCoeff;
   if (id == DRAG_AREA_ID) return dragArea;
   if (id == SRP_AREA_ID) return srpArea;
    
   if (id == TOTAL_MASS_ID) 
   { 
      return UpdateTotalMass();
   }
    
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
Real Spacecraft::GetRealParameter(const std::string &label) const
{
   // Performance!
    if (label == "A1Epoch")
       return state.GetEpoch();
   
    // First check with anomaly
    if (label == "TA" || label == "MA" || label == "EA" || label == "HA")
    {
       return anomaly.GetValue();
    }
      
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
Real Spacecraft::SetRealParameter(const Integer id, const Real value)
{
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

   return SpaceObject::SetRealParameter(id, value);
}

//---------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value)
//---------------------------------------------------------------------------
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
Real Spacecraft::SetRealParameter(const std::string &label, const Real value)
{
   // First try to set as a state element
   if (SetElement(label, value))
      return value;
      
   if (label == "A1Epoch")
   {
      state.SetEpoch(value);
      return value;
   }

   if (label == "DryMass")
   {
      parmsChanged = true;
      return dryMass = value;
   }
   if (label == "Cd")
   {
      parmsChanged = true;
      return coeffDrag = value;
   }
   if (label == "DragArea")
   {
      parmsChanged = true;
      return dragArea = value;
   }
   if (label == "SRPArea")
   {
      parmsChanged = true;
      return srpArea = value;
   }
   if (label == "Cr")
   {
      parmsChanged = true;
      return reflectCoeff = value;
   }

   if (label == "TotalMass") return totalMass;    // Don't change the total mass

   return SpaceObject::SetRealParameter(label, value);
}

//---------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
std::string Spacecraft::GetStringParameter(const Integer id) const
{
    if (id == SC_EPOCH_ID)
       return scEpoch;
    
    if (id == DATE_FORMAT_ID)
       return dateFormat;

    if (id == STATE_TYPE_ID)
       return stateType; 

    if (id == ANOMALY_ID)
       return anomaly.GetType(); 
    
    if (id == COORD_SYS_ID)
       return coordSysName; 

    if ((id >= ELEMENT1UNIT_ID) && (id <= ELEMENT6UNIT_ID))
       return stateElementUnits[id - ELEMENT1UNIT_ID];

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
std::string Spacecraft::GetStringParameter(const std::string &label) const
{
//   if (label == "StateType")
//      return stateType;
//   
   return GetStringParameter(GetParameterID(label));
}

// Added 11/15/04 to handle tanks and thrusters
//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Accesses lists of tank and thruster (and, eventually, other hardware) names.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a 
 *         StringArray.
 */
const StringArray& Spacecraft::GetStringArrayParameter(const Integer id) const
{
   if (id == FUEL_TANK_ID)
      return tankNames;
   if (id == THRUSTER_ID)
      return thrusterNames;
   return SpaceObject::GetStringArrayParameter(id);
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
bool Spacecraft::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_SPACECRAFT_STRINGS
      MessageInterface::ShowMessage
         ("Spacecraft::SetStringParameter() string parameter %d (%s) to %s\n", 
         id, GetParameterText(id).c_str(), value.c_str());
   #endif

   if ((id < SpaceObjectParamCount) || (id >= SpacecraftParamCount))
      return SpaceObject::SetStringParameter(id, value);
      
   if ((GetParameterType(id) != Gmat::STRING_TYPE) && 
       (GetParameterType(id) != Gmat::STRINGARRAY_TYPE))
      throw SpaceObjectException("Parameter " + GetParameterText(id) +
         " is being accessed as a string, but it is of type " +
         GetParameterTypeString(GetParameterType(id)));

   if (id == SC_EPOCH_ID)
   {
      // Validate first...

      // and set the epoch value in the state
      SetEpoch(value);
   }   
   else if (id == DATE_FORMAT_ID)
   {
      SetDateFormat(value);
//      if (value != "TAIModJulian" && value != "UTCModJulian" && 
//          value != "TAIGregorian" && value != "UTCGregorian")
//      {
//         return GmatBase::SetStringParameter(id, value);
//      }
//      dateFormat = value;
//      displayDateFormat = value;

   }
   else if (id == STATE_TYPE_ID)
   {  
      // Check for invalid input then return unknown value from GmatBase 
      if (value != "Cartesian" && value != "Keplerian" && 
          value != "ModifiedKeplerian" && value != "SphericalAZFPA" && 
          value != "SphericalRADEC" && value != "Equinoctial")
      {   
         throw SpaceObjectException("Unknown state element representation: " + 
            value);
      }

      if ((value == "Keplerian") || (value == "ModifiedKeplerian"))
      {
         // Load anomaly with the state data
         Rvector6 kep = GetStateInRepresentation("Keplerian");
         anomaly.SetSMA(kep[0]);
         anomaly.SetECC(kep[1]);
         anomaly.SetValue(kep[5]);
      }
      
      stateType = value;
      UpdateElementLabels();
   }
   else if (id == ANOMALY_ID)
   {
      // Check for invalid input then return unknown value from GmatBase 
      if (anomaly.IsInvalid(value))
      {   
         return GmatBase::SetStringParameter(id, value);
      }
      #if DEBUG_SPACECRAFT
          MessageInterface::ShowMessage("\nSpacecraft::SetStringParamter()..."
             "\n   After change, Anomaly info -> a: %f, e: %f, %s: %f\n", 
             anomaly.GetSMA(),anomaly.GetECC(),anomaly.GetType().c_str(),
             anomaly.GetValue());   
      #endif

      anomaly.SetType(value);
      UpdateElementLabels();
      
      #if DEBUG_SPACECRAFT
          MessageInterface::ShowMessage(
             "\n   After change, Anomaly info -> a: %lf, e: %lf, %s: %lf\n", 
             anomaly.GetSMA(), anomaly.GetECC(), anomaly.GetType().c_str(),
             anomaly.GetValue());   
      #endif
      if ((stateType == "Keplerian") || 
          (stateType == "ModifiedKeplerian"))
         rvState[5] = anomaly.GetValue();   // @todo: add state[5]?
   }
   else if (id == COORD_SYS_ID) 
   {
      parmsChanged = true;
      coordSysName = value;
   }
   else if (id == FUEL_TANK_ID) 
   {
      // Only add the tank if it is not in the list already
      if (find(tankNames.begin(), tankNames.end(), value) == tankNames.end()) 
      {
          tankNames.push_back(value);
      }
   }
   else // id == THRUSTER_ID 
   { 
      // Only add the tank if it is not in the list already
      if (find(thrusterNames.begin(), thrusterNames.end(), value) == 
          thrusterNames.end()) 
      {
         thrusterNames.push_back(value);
      }
   }

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
bool Spacecraft::SetStringParameter(const std::string &label, 
                                    const std::string &value)
{
   #if DEBUG_SPACECRAFT
       MessageInterface::ShowMessage("\nSpacecraft::SetStringParameter(\"%s\", \"%s\") enters\n", label.c_str(), value.c_str() ); 
       Integer id = GetParameterID(label);
       MessageInterface::ShowMessage("GetParameterText: %s\n", GetParameterText(id).c_str());
       MessageInterface::ShowMessage("Spacecraft::SetStringParameter exits sooner\n\n"); 
   #endif

   return SetStringParameter(GetParameterID(label),value);
}

//---------------------------------------------------------------------------
//  bool TakeAction(const std::string &action, const std::string &actionData)
//---------------------------------------------------------------------------
/**
 * Set the epoch.
 * 
 */
bool Spacecraft::TakeAction(const std::string &action, 
                            const std::string &actionData)
{
   if (action == "SetupHardware") 
   {
      // Attach tanks to thrusters
      StringArray tankNommes;
      GmatBase *tank;

      for (ObjectArray::iterator i = thrusters.begin(); 
           i < thrusters.end(); ++i) {
         tankNommes = (*i)->GetStringArrayParameter("Tank");

         for (StringArray::iterator j = tankNommes.begin(); 
              j < tankNommes.end(); ++j) {

            // Look up the tank in the hardware list
            tank = NULL;
            for (ObjectArray::iterator k = tanks.begin(); k < tanks.end(); ++k)
               if ((*k)->GetName() == *j) {
                  tank = *k;
                  break;
               }

            if (tank)
               (*i)->SetRefObject(tank, tank->GetType(), tank->GetName());
            else
               throw SpaceObjectException("Cannot find tank \"" + (*j) +
                                          "\" in spacecraft \"" + instanceName +
                                          "\"\n");
         }
      }
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
                  thrusters.erase(i);
         }
      }

      if (removeTank) 
      {
         if (removeAll) 
         {
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
                  tanks.erase(i);
         }
      }

      return true;
   }

   if (action == "ApplyCoordinateSystem")
   {
      if (!stateConverter.SetMu(coordinateSystem))
      {      throw SpaceObjectException(
                   "\nError:  Spacecraft has empty coordinate system\n");
      }
      
      if (csSet == false)
      {
         Rvector6 st(state.GetState());
         SetStateFromRepresentation(stateType, st);
         
         csSet = true;
      }
   }
   
   // 6/12/06 - arg: reset scEpoch to epoch from prop state
   if (action == "UpdateEpoch")
   {
      Real currEpoch = state.GetEpoch();
      
      if (epochSystem != "")
      {
         if (epochSystem != "A1")
            currEpoch = TimeConverterUtil::Convert(currEpoch, 
                          TimeConverterUtil::A1,  
                          TimeConverterUtil::GetTimeTypeID(epochSystem),
                          GmatTimeUtil::JD_JAN_5_1941);
      }
      
      if (epochFormat != "")
      {  
         if (epochFormat == "Gregorian")
            scEpoch = TimeConverterUtil::ConvertMjdToGregorian(currEpoch);
         else
         {
            std::stringstream timestream;
            timestream.precision(TIME_PRECISION);
            timestream << currEpoch;
            scEpoch = timestream.str();
         }
      }

      return true;
   }
      
   return SpaceObject::TakeAction(action, actionData);
}

//---------------------------------------------------------------------------
//  Real GetDisplayEpoch() const
//---------------------------------------------------------------------------
/**
 * Get the display's epoch.
 * 
 * @return display's epoch. 
 *
 */
std::string Spacecraft::GetDisplayEpoch()
{
   MessageInterface::ShowMessage("Spacecraft::GetDisplayEpoch is deprecated; "
   "use GetStringParameter(\"Epoch\") instead.\n");
//   if (initialDisplay)
//      SetInitialDisplay();

//   return displayEpoch;
   return GetStringParameter("Epoch");
}

//---------------------------------------------------------------------------
//  bool SetDisplayEpoch(const std::string &value) 
//---------------------------------------------------------------------------
/**
 * Set the display's epoch.
 * 
 * @param <value> Epoch input from GUI. 
 * 
 * @return flag indicator (true - successful; otherwise false) 
 *
 */
bool Spacecraft::SetDisplayEpoch(const std::string &value) 
{
   MessageInterface::ShowMessage(
      "Spacecraft::SetDisplayEpoch is deprecated; "
      "use SetEpoch(const std::string &value) instead.\n");
      
   SetEpoch(value);
   return true;
}

//---------------------------------------------------------------------------
//  std::string GetDisplayCoordType() const
//---------------------------------------------------------------------------
/**
 * Get the coordinate type of display's state.
 * 
 * @return coordintate type of display's state 
 *
 */
std::string Spacecraft::GetDisplayCoordType() const
{
//MessageInterface::ShowMessage("Spacecraft::GetDisplayCoordType not implemented yet.\n");
   return stateType;
}

//---------------------------------------------------------------------------
//  void SetDisplayCoordType(const std::string &coordType) 
//---------------------------------------------------------------------------
/**
 * Set the coordinate type of display's state.
 * 
 * @param <coordType> Coordintate type given. 
 *
 */
void Spacecraft::SetDisplayCoordType(const std::string &coordType) 
{
   MessageInterface::ShowMessage(
      "Spacecraft::SetDisplayCoordType is deprecated; "
      "use SetStringParameter(\"StateType\") instead.\n");
}

//---------------------------------------------------------------------------
//  Real* GetDisplayState() 
//---------------------------------------------------------------------------
/**
 * Get the display's state.
 * 
 * @return display's state. 
 *
 */
Real* Spacecraft::GetDisplayState()
{
//    if (initialDisplay)
//       SetInitialDisplay();
//
//    return displayState;
   MessageInterface::ShowMessage(
      "Spacecraft::GetDisplayState not fully implemented yet.\n");
   return state.GetState();
}

//---------------------------------------------------------------------------
//  void SetDisplayState(const Real *s) 
//---------------------------------------------------------------------------
/**
 * Set the display's state.
 * 
 * @param <s> Input of state from GUI. 
 *
 */
void Spacecraft::SetDisplayState(const Real *s) 
{
//    SetDisplayState(Rvector6(s[0],s[1],s[2],s[3],s[4],s[5]));
   MessageInterface::ShowMessage("Spacecraft::SetDisplayState not fully implemented yet.\n");

   Real *posvel = state.GetState();
   posvel[0] = s[0];
   posvel[1] = s[1];
   posvel[2] = s[2];
   posvel[3] = s[3];
   posvel[4] = s[4];
   posvel[5] = s[5];
}

//---------------------------------------------------------------------------
//  void SetDisplayState(const Rvector6 s) 
//---------------------------------------------------------------------------
/**
 * Set the display's state.
 * 
 * @param <s> Input of state from GUI. 
 *
 */
void Spacecraft::SetDisplayState(const Rvector6 s) 
{
MessageInterface::ShowMessage("Spacecraft::SetDisplayState not fully implemented yet.\n");
   Real *posvel = state.GetState();
   posvel[0] = s[0];
   posvel[1] = s[1];
   posvel[2] = s[2];
   posvel[3] = s[3];
   posvel[4] = s[4];
   posvel[5] = s[5];
}

//---------------------------------------------------------------------------
//  void SaveDisplay() 
//---------------------------------------------------------------------------
/**
 * Save the display state for updating the internal states.
 * 
 *
 */
void Spacecraft::SaveDisplay()
{
   MessageInterface::ShowMessage("SaveDisplay is deprecated; "
      "you can safely remove all calls to it.\n");
}

//---------------------------------------------------------------------------
//  bool Initialize()
//---------------------------------------------------------------------------
/** 
 * Initialize the default values of spacecraft information. 
 * 
 * @return always success unless the coordinate system is empty 
 * 
 */
bool Spacecraft::Initialize()
{
   #if DEBUG_SPACECRAFT_CS
      MessageInterface::ShowMessage("Spacecraft::Initialize() "
         "internalCoordSystem=%d, coordinateSystem=%d\n", internalCoordSystem, 
         coordinateSystem);
   
      if (internalCoordSystem)
         MessageInterface::ShowMessage("   internalCoordSystem is '%s'\n", 
            internalCoordSystem->GetName().c_str());
      if (coordinateSystem)
         MessageInterface::ShowMessage("   coordinateSystem is '%s'\n", 
            coordinateSystem->GetName().c_str());
          
      MessageInterface::ShowMessage
         ("   stateType=%s, state=\n   %.9f, %.9f, %.9f, %.14f, %.14f, %f.14\n",
          stateType.c_str(), state[0], state[1], state[2], state[3],
          state[4], state[5]);
   #endif
   
   // Set the mu if CelestialBody is there through coordinate system's origin;   
   // Otherwise, discontine process and send the error message   
   if (!stateConverter.SetMu(coordinateSystem))
   {      
      throw SpaceObjectException("Spacecraft has empty coordinate system");
   }   
   if (!attitude)
   {
      MessageInterface::ShowMessage("Spacecraft %s has no defined attitude object.\n",
                     instanceName.c_str());
      //throw SpaceObjectException("Spacecraft has no attitude set.");
   }
   else
      attitude->Initialize();
   
   return true;
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
      MessageInterface::ShowMessage("Spacecraft::SetEpoch() Setting epoch to %s\n",
                                    ep.c_str());
   #endif

   // 1. Validate that the input string is the correct format.
   if (TimeConverterUtil::ValidateTimeFormat(epochFormat, ep) == false)
   {
      throw SpaceObjectException("Invalid epoch, '" + ep + 
         "', specified; this value is not a valid " + epochFormat +
         " epoch.");
   }
   
   // 2. Construct the Real epoch data and set it on the PropState.
   Real now;
   if (epochFormat == "ModJulian")
   {
      std::stringstream timestream;
      timestream.precision(TIME_PRECISION);
      timestream << ep;
      timestream >> now;
   }
   else
   {
      now = TimeConverterUtil::ConvertGregorianToMjd(ep);
   }

   // PropState epoch is a.1
   // 20.02.06 - arg: changed to use enum types instead of strings
//   state.SetEpoch(TimeConverterUtil::Convert(now, epochSystem, "A1",
//      GmatTimeUtil::JD_JAN_5_1941));
   
   //MessageInterface::ShowMessage("==> Spacecraft::SetEpoch() now=%.11f, scEpoch=%s\n",
   //                              now, scEpoch.c_str());

   Real newEpoch =
      TimeConverterUtil::Convert(now, 
                                 TimeConverterUtil::GetTimeTypeID(epochSystem), 
                                 TimeConverterUtil::A1,  GmatTimeUtil::JD_JAN_5_1941);
   //TimeConverterUtil::A1,  GmatTimeUtil::JD_NOV_17_1858);

   state.SetEpoch(newEpoch);
   if (attitude) attitude->SetEpoch(newEpoch);
   
//    state.SetEpoch(TimeConverterUtil::Convert(now, 
//       TimeConverterUtil::GetTimeTypeID(epochSystem), 
//       TimeConverterUtil::A1,  GmatTimeUtil::JD_JAN_5_1941));
   
   // 3. Save the string.
   if (epochFormat == "ModJulian")
   {
      std::stringstream timestream;
      timestream.precision(TIME_PRECISION);
      timestream << now;
      timestream >> scEpoch;
   }
   else
   {
      scEpoch = TimeConverterUtil::ConvertMjdToGregorian(now);
   }

   //MessageInterface::ShowMessage("==> Spacecraft::SetEpoch() newEpoch=%.11f, scEpoch=%s\n",
   //                              newEpoch, scEpoch.c_str());
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
   totalMass = dryMass;
   for (ObjectArray::iterator i = tanks.begin(); i < tanks.end(); ++i) {
      totalMass += (*i)->GetRealParameter("FuelMass");
   }
   
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
   Real tmass = dryMass;
   for (ObjectArray::const_iterator i = tanks.begin(); i < tanks.end(); ++i) {
      tmass += (*i)->GetRealParameter("FuelMass");
   }
   
   return tmass;
}


//------------------------------------------------------------------------------
// const std::string&  GetGeneratingString(Gmat::WriteMode mode,
//                const std::string &prefix, const std::string &useName)
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
   std::stringstream data;

   data.precision(15);   // Crank up data precision so we don't lose anything
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
   
   if (mode == Gmat::OWNED_OBJECT) {
      preface = prefix;
      nomme = "";
   }
   
   preface += nomme;
   WriteParameters(mode, preface, data);
   
   generatingString = data.str();
   
   return generatingString;
}


//------------------------------------------------------------------------------
// void WriteParameters(std::string &prefix, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Code that writes the parameter details for an object.
 * 
 * @param prefix Starting portion of the script string used for the parameter.
 * @param obj The object that is written.
 */
//------------------------------------------------------------------------------
void Spacecraft::WriteParameters(Gmat::WriteMode mode, std::string &prefix, 
                                 std::stringstream &stream)
{
   Integer i;
   Gmat::ParameterType parmType;
   std::stringstream value;
   value.precision(15); 
   
   bool showAnomaly = false;
   
   // Set the parameter order for output
   Integer parmOrder[parameterCount], parmIndex = 0;
   parmOrder[parmIndex++] = DATE_FORMAT_ID;
   parmOrder[parmIndex++] = SC_EPOCH_ID;
   parmOrder[parmIndex++] = COORD_SYS_ID;
   parmOrder[parmIndex++] = STATE_TYPE_ID;
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

   Rvector6 genState; 
   coordConverter.Convert(A1Mjd(GetEpoch()), Rvector6(GetState().GetState()), 
                          internalCoordSystem, 
                          genState, coordinateSystem);
   
   Rvector6 repState = GetStateInRepresentation(stateType);
      
   for (i = 0; i < parameterCount; ++i)
   {
      if ((IsParameterReadOnly(parmOrder[i]) == false) &&
          (parmOrder[i] != J2000_BODY_NAME) &&
          (parmOrder[i] != TOTAL_MASS_ID))
      {
         parmType = GetParameterType(parmOrder[i]);
         
         // Handle StringArray parameters separately
         if (parmType != Gmat::STRINGARRAY_TYPE)
         {
            // Skip unhandled types
            if (
                (parmType != Gmat::UNSIGNED_INTARRAY_TYPE) &&
                (parmType != Gmat::RVECTOR_TYPE) &&
                (parmType != Gmat::RMATRIX_TYPE) &&
                (parmType != Gmat::UNKNOWN_PARAMETER_TYPE)
               )
            {
               // Fill in the l.h.s.
               value.str("");
               if ((parmOrder[i] >= ELEMENT1_ID) && 
                   (parmOrder[i] <= ELEMENT6_ID))
               {
                  value.precision(18); 
                  value << repState[parmOrder[i] - ELEMENT1_ID]; // genState[parmOrder[i] - ELEMENT1_ID];
                  value.precision(15); 
               }
               else if (parmOrder[i] == STATE_TYPE_ID)
               {
                  if (mode != Gmat::MATLAB_STRUCT)
                     value << stateType;
                  else
                     value << "'" << stateType << "'"; 
               }
               else
                  WriteParameterValue(parmOrder[i], value);
               
               if ((showAnomaly == false) && (parmOrder[i] == ANOMALY_ID))
                  value.str("");
               
               if (value.str() != "")
                  stream << prefix << GetParameterText(parmOrder[i])
                         << " = " << value.str() << ";\n";
            }
         }
         else
         {
            // Handle StringArrays
            StringArray sar = GetStringArrayParameter(parmOrder[i]);
            if (sar.size() > 0)
            {
               stream << prefix << GetParameterText(parmOrder[i]) << " = {";
               for (StringArray::iterator n = sar.begin(); n != sar.end(); ++n)
               {
                  if (n != sar.begin())
                     stream << ", ";
                  if (inMatlabMode)
                     stream << "'";
                  stream << (*n);
                  if (inMatlabMode)
                     stream << "'";
               }
               stream << "};\n";
            }
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

   for (i = 0; i < GetOwnedObjectCount(); ++i)
   {
      newprefix = prefix;
      ownedObject = GetOwnedObject(i);
      nomme = ownedObject->GetName();
      
      #ifdef DEBUG_OWNED_OBJECT_STRINGS
          MessageInterface::ShowMessage(
             "   id %d has type %s and name \"%s\"\n",
             i, ownedObject->GetTypeName().c_str(),
             ownedObject->GetName().c_str());
      #endif

      if (nomme != "")
         newprefix += nomme + ".";
      else if (GetType() == Gmat::FORCE_MODEL)
         newprefix += ownedObject->GetTypeName();
      stream << ownedObject->GetGeneratingString(Gmat::OWNED_OBJECT, newprefix);
   }
   
//   isForDisplay = coordType;
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
   if (stateType == "Cartesian")
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

   if (stateType == "Keplerian")
   {
      stateElementLabel[0] = "SMA";
      stateElementLabel[1] = "ECC";
      stateElementLabel[2] = "INC";
      stateElementLabel[3] = "RAAN";
      stateElementLabel[4] = "AOP";
      stateElementLabel[5] = anomaly.GetType();

      stateElementUnits[0] = "km";
      stateElementUnits[1] = "";
      stateElementUnits[2] = "deg";
      stateElementUnits[3] = "deg";
      stateElementUnits[4] = "deg";
      stateElementUnits[5] = "deg";

      return;
   }

   if (stateType == "ModifiedKeplerian")
   {
      stateElementLabel[0] = "RadPer";
      stateElementLabel[1] = "RadApo";
      stateElementLabel[2] = "INC";
      stateElementLabel[3] = "RAAN";
      stateElementLabel[4] = "AOP";
      stateElementLabel[5] = anomaly.GetType();

      stateElementUnits[0] = "km";
      stateElementUnits[1] = "km";
      stateElementUnits[2] = "deg";
      stateElementUnits[3] = "deg";
      stateElementUnits[4] = "deg";
      stateElementUnits[5] = "deg";

      return;
   }

   if (stateType == "SphericalAZFPA")
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
    
   if (stateType == "SphericalRADEC")
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
      
   if (stateType == "Equinoctial")
   {
      stateElementLabel[0] = "SMA";
        stateElementLabel[1] = "PEY";
        stateElementLabel[2] = "PEX";
        stateElementLabel[3] = "PNY";
        stateElementLabel[4] = "PNX";
        stateElementLabel[5] = "MLONG";
          
      stateElementUnits[0] = "km";
      stateElementUnits[1] = "?";
      stateElementUnits[2] = "?";
      stateElementUnits[3] = "?";
      stateElementUnits[4] = "?";
      stateElementUnits[5] = "?";         
      
      return;
   }
   
   
}


//------------------------------------------------------------------------------
// Rvector6 GetStateInRepresentation(std::string rep)
//------------------------------------------------------------------------------
/**
 * Code used to obtain a state in a non-Cartesian representation.
 */
//------------------------------------------------------------------------------
Rvector6 Spacecraft::GetStateInRepresentation(std::string rep)
{
   #ifdef DEBUG_STATE_INTERFACE
      MessageInterface::ShowMessage(
         "Spacecraft::GetStateInRepresentation(string): Constructing %s state\n", 
         rep.c_str());
   #endif

   Rvector6 csState;
   Rvector6 finalState;
   
   // First convert from the internal CS to the state CS
   if (internalCoordSystem != coordinateSystem)
   {
      Rvector6 inState(state.GetState());
      coordConverter.Convert(GetEpoch(), inState, internalCoordSystem, csState, 
         coordinateSystem);
   }
   else
   {
      csState.Set(state.GetState());
   }
   
   // Then convert to the desired representation
   if (rep == "")
      rep = stateType;
   
   if (rep == "Cartesian")
      finalState = csState;
   else
      finalState = stateConverter.Convert(csState, "Cartesian", rep, anomaly);
   
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
// Rvector6 GetStateInRepresentation(Integer rep)
//------------------------------------------------------------------------------
/**
 * Code used to obtain a state in a non-Cartesian representation.
 */
//------------------------------------------------------------------------------
Rvector6 Spacecraft::GetStateInRepresentation(Integer rep)
{
   #ifdef DEBUG_STATE_INTERFACE
      MessageInterface::ShowMessage(
         "Spacecraft::GetStateInRepresentation(int): Constructing %s state\n", 
         representations[rep].c_str());
   #endif
   Rvector6 csState;
   Rvector6 finalState;
   
   // First convert from the internal CS to the state CS
   if (internalCoordSystem != coordinateSystem)
   {
      Rvector6 inState(state.GetState());
      coordConverter.Convert(GetEpoch(), inState, internalCoordSystem, csState, 
         coordinateSystem);
   }
   else
   {
      csState.Set(state.GetState());
   }
   
   // Then convert to the desired representation
   if (rep == CARTESIAN_ID)
      finalState = csState;
   else
   {
      finalState = stateConverter.Convert(csState, "Cartesian", 
         representations[rep], anomaly);
   }
   
   #ifdef DEBUG_STATE_INTERFACE
      MessageInterface::ShowMessage(
         "Spacecraft::GetStateInRepresentation(int): %s state is "
         "[%.9lf %.9lf %.9lf %.14lf %.14lf %.14lf]\n", 
         representations[rep].c_str(), finalState[0], finalState[1], 
         finalState[2], finalState[3], finalState[4], 
         finalState[5]);
   #endif

   return finalState;
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
         rep.c_str(), st.ToString().c_str());
   #endif

   // First convert from the representation to Cartesian   
   static Rvector6 csState, finalState;
   
   if (rep == "Cartesian")
      csState = st;
   else
      csState = stateConverter.Convert(st, rep, "Cartesian", anomaly);
      
   // Then convert to the internal CS
   if (internalCoordSystem != coordinateSystem)
      coordConverter.Convert(GetEpoch(), csState, coordinateSystem, finalState, 
         internalCoordSystem);
   else
      finalState = csState;

   state[0] = finalState[0];
   state[1] = finalState[1];
   state[2] = finalState[2];
   state[3] = finalState[3];
   state[4] = finalState[4];
   state[5] = finalState[5];
   
   #ifdef DEBUG_STATE_INTERFACE
      MessageInterface::ShowMessage(
         "Spacecraft::SetStateFromRepresentation: State is now "
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
   std::string rep = "";
   Integer id = LookUpLabel(label, rep);
   Real retval = -9999999999.999999;

   if (id != -1) 
   {  
      Rvector6 tempState = GetStateInRepresentation(rep);
      retval = tempState[id];
   }
   
   return retval;
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
   std::string rep = "";
   Integer id = LookUpLabel(label, rep) - ELEMENT1_ID;

   if ((id == 5) && (!anomaly.IsInvalid(label)))
      anomaly.SetType(label);
   
   if (id >= 0)
   {
      if (csSet)
      {
         Rvector6 tempState = GetStateInRepresentation(rep);
         tempState[id] = value;
         SetStateFromRepresentation(rep, tempState);
      }
      else
      {
         Real *tempState = state.GetState();
         tempState[id] = value;
      }
      
      return true;
   }
   
   return false;
}


//------------------------------------------------------------------------------
// void SetStateFromRepresentation(std::string rep, Rvector6 &st)
//------------------------------------------------------------------------------
/**
 * Code used to obtain a state in a non-Cartesian representation.
 */
//------------------------------------------------------------------------------
Integer Spacecraft::LookUpLabel(const std::string &label, std::string &rep)
{
   Integer retval = -1;
   
   if (label == "X" || label == "SMA" || label == "RadPer" || label == "RMAG")
      retval = ELEMENT1_ID;

   if (label == "Y" || label == "ECC" || label == "RadApo" || label == "RA")
      retval = ELEMENT2_ID;

   if (label == "Z" || label == "INC" || label == "DEC")  
      retval = ELEMENT3_ID;

   if (label == "VX" || label == "RAAN" || label == "VMAG")  
      retval = ELEMENT4_ID;

   if (label == "VY" || label == "AOP" || label == "AZI" || label == "RAV")  
      retval = ELEMENT5_ID;

   if (label == "VZ" || !anomaly.IsInvalid(label) ||
        label == "FPA" || label == "DECV")  
      retval = ELEMENT6_ID;
   
   rep = elementLabelMap[label];
   
   #ifdef DEBUG_STATE_INTERFACE
      MessageInterface::ShowMessage("Spacecraft::LookUpLabel(%s..) gives rep %s\n",
         label.c_str(), rep.c_str());
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
      elementLabelMap["X"] = "Cartesian";
      elementLabelMap["Y"] = "Cartesian";
      elementLabelMap["Z"] = "Cartesian";
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
   }
}

   
