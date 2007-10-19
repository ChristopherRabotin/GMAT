//$Header$
//------------------------------------------------------------------------------
//                              FiniteBurn
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/12/20
//
/**
 * Implements the FiniteBurn class used for maneuvers. 
 */
//------------------------------------------------------------------------------


#include "FiniteBurn.hpp"
#include "StringUtil.hpp"     // for ToString()

//#define DEBUG_BURN_ORIGIN
//#define DEBUG_RENAME 1

//---------------------------------
// static data
//---------------------------------

/// Labels used for the finite burn parameters.
const std::string
FiniteBurn::PARAMETER_TEXT[FiniteBurnParamCount - BurnParamCount] =
{
   "Thrusters",
   "Tanks",
   "BurnScaleFactor"
};

/// Types of the parameters used by finite burns.
const Gmat::ParameterType
FiniteBurn::PARAMETER_TYPE[FiniteBurnParamCount - BurnParamCount] =
{
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::REAL_TYPE,
};


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
//  FiniteBurn(const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * FiniteBurn constructor (default constructor).
 * 
 * @param <nomme> Name of the constructed object.
 */
//------------------------------------------------------------------------------
FiniteBurn::FiniteBurn(const std::string &nomme) :
   Burn              (Gmat::FINITE_BURN, "FiniteBurn", nomme),
   burnScaleFactor   (1.0),
   initialized       (false)
{
   objectTypes.push_back(Gmat::FINITE_BURN);
   objectTypeNames.push_back("FiniteBurn");
   parameterCount = FiniteBurnParamCount;
   
   thrusters.clear();
   tanks.clear();
}


//------------------------------------------------------------------------------
//  ~FiniteBurn()
//------------------------------------------------------------------------------
/**
 * FiniteBurn destructor.
 */
//------------------------------------------------------------------------------
FiniteBurn::~FiniteBurn()
{
}


//------------------------------------------------------------------------------
//  FiniteBurn(const FiniteBurn& fb)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param <fb> The FiniteBurn that is copied.
 */
//------------------------------------------------------------------------------
FiniteBurn::FiniteBurn(const FiniteBurn& fb) :
   Burn              (fb),
   thrusters         (fb.thrusters),
   tanks             (fb.tanks),
   burnScaleFactor   (fb.burnScaleFactor),
   initialized       (false)
{
   parameterCount = fb.parameterCount;
}


//------------------------------------------------------------------------------
//  FiniteBurn& operator=(const FiniteBurn& fb)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 * 
 * @param <fb> The FiniteBurn that is copied.
 * 
 * @return this instance, with parameters set to match fb.
 */
//------------------------------------------------------------------------------
FiniteBurn& FiniteBurn::operator=(const FiniteBurn& fb)
{
   if (&fb == this)
      return *this;
      
   Burn::operator=(fb);
   
   thrusters       = fb.thrusters;
   tanks           = fb.tanks;
   burnScaleFactor = fb.burnScaleFactor;
   initialized     = false;
      
   return *this;
}


//------------------------------------------------------------------------------
//  std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the name of the parameter with the input id.
 * 
 * @param <id> Integer id for the parameter.
 * 
 * @return The string name of the parameter.
 */
//------------------------------------------------------------------------------
std::string FiniteBurn::GetParameterText(const Integer id) const
{
   if (id >= BurnParamCount && id < FiniteBurnParamCount)
      return PARAMETER_TEXT[id - BurnParamCount];
      
   return Burn::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Gets the id corresponding to a named parameter.
 * 
 * @param <str> Name of the parameter.
 * 
 * @return The ID.
 */
//------------------------------------------------------------------------------
Integer FiniteBurn::GetParameterID(const std::string &str) const
{
   for (Integer i = BurnParamCount; i < FiniteBurnParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - BurnParamCount])
         return i;
   }

   return Burn::GetParameterID(str);
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
Gmat::ParameterType FiniteBurn::GetParameterType(const Integer id) const
{
   if (id >= BurnParamCount && id < FiniteBurnParamCount)
      return PARAMETER_TYPE[id - BurnParamCount];
      
   return Burn::GetParameterType(id);
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
std::string FiniteBurn::GetParameterTypeString(const Integer id) const
{
   return Burn::PARAM_TYPE_STRING[GetParameterType(id)];
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool FiniteBurn::IsParameterReadOnly(const Integer id) const
{
   if (/*(id == BURNAXES) ||*/ (id == VECTORFORMAT) || 
       (id == COORDINATESYSTEM) || (id == DELTAV1) || 
       (id == DELTAV2) || (id == DELTAV3))
      return true;

   return Burn::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
//  Real SetStringParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 * 
 * @param <id>    Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
bool FiniteBurn::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == THRUSTER)
   {
      if (find(thrusters.begin(), thrusters.end(), value) == thrusters.end())
         thrusters.push_back(value);
      initialized = false;
      return true;
   }
         
   if (id == FUEL_TANK)
   {
      if (find(tanks.begin(), tanks.end(), value) == tanks.end())
         tanks.push_back(value);
      initialized = false;
      return true;
   }
         
   return Burn::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
//  Real SetStringParameter(const Integer id, const Real value,
//                          const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a specific std::string element in an array.
 *
 * @param <id>    Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * @param <index> Index for the element
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool FiniteBurn::SetStringParameter(const Integer id, const std::string &value,
                                    const Integer index)
{
   Integer count;
   
   if (id == THRUSTER)
   {
      count = thrusters.size();
      if (index > count)
         throw BurnException("Attempting to write thruster " + value +
                  " past the allowed range for FiniteBurn " + instanceName);
      if (find(thrusters.begin(), thrusters.end(), value) != thrusters.end())
      {
         if (thrusters[index] == value)
            return true;
         throw BurnException("Thruster " + value +
                  " already set for FiniteBurn " + instanceName);
      }
      if (index == count)
         thrusters.push_back(value);
      else
         thrusters[index] = value;

      initialized = false;
      return true;
   }

   if (id == FUEL_TANK)
   {
      count = tanks.size();
      if (index > count)
         throw BurnException("Attempting to write tank " + value +
                  " past the allowed range for FiniteBurn " + instanceName);
      if (find(tanks.begin(), tanks.end(), value) != tanks.end())
      {
         if (tanks[index] == value)
            return true;
         throw BurnException("Tank " + value +
                  " already set for FiniteBurn " + instanceName);
      }
      if (index == count)
         tanks.push_back(value);
      else
         tanks[index] = value;

      initialized = false;
      return true;
   }

   return Burn::SetStringParameter(id, value, index);
}


//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Access an array of string data.
 * 
 * For the Burn classes, calls to this method get passed to the maneuver frame
 * manager when the user requests the frames that are available for the system.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a 
 *         StringArray.
 */
//---------------------------------------------------------------------------
const StringArray& FiniteBurn::GetStringArrayParameter(const Integer id) const
{
   if (id == THRUSTER)
      return thrusters;
      
   if (id == FUEL_TANK)
   {
      //MessageInterface::ShowMessage("Retrieving %d tanks\n", tanks.size());
      return tanks;
   }

   return Burn::GetStringArrayParameter(id);
}


//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Access the Real data associated with this burn.
 * 
 * @param <id> The integer ID for the parameter.
 *
 * @return The requested Real.
 */
//---------------------------------------------------------------------------
Real FiniteBurn::GetRealParameter(const Integer id) const
{
   if (id == BURN_SCALE_FACTOR)
      return burnScaleFactor;

   return Burn::GetRealParameter(id);
}


//---------------------------------------------------------------------------
//  Real setRealParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Change the Real data associated with this burn.
 * 
 * @param <id>    Integer ID of the parameter.
 * @param <value> New value for the parameter.
 *
 * @exception <BurnException> thrown if value is out of range
 * 
 * @return The value of the parameter at the end of the call.
 */
//---------------------------------------------------------------------------
Real FiniteBurn::SetRealParameter(const Integer id, const Real value)
{
   if (id == BURN_SCALE_FACTOR)
   {
      if (value > 0.0)
         burnScaleFactor = value;
      else
      {
         BurnException be;
         be.SetDetails(errorMessageFormat.c_str(),
                       GmatStringUtil::ToString(value, GetDataPrecision()).c_str(),
                       GetParameterText(id).c_str(), "Real Number > 0 ");
         throw be;
      }
      return burnScaleFactor;
   }
   
   return Burn::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by this class.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& FiniteBurn::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes = Burn::GetRefObjectTypeArray();
   refObjectTypes.push_back(Gmat::HARDWARE);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
const StringArray& FiniteBurn::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   refObjectNames.clear();
   
   refObjectNames = Burn::GetRefObjectNameArray(type);
   
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::HARDWARE)
   {
      refObjectNames.insert(refObjectNames.end(), thrusters.begin(), thrusters.end());
      refObjectNames.insert(refObjectNames.end(), tanks.begin(), tanks.end());
   }
   
   return refObjectNames;
}


//------------------------------------------------------------------------------
//  bool Fire(Real *burnData)
//------------------------------------------------------------------------------
/**
 * Fire does not currently perform any action for FiniteBurn objects.  The 
 * BeginManeuver/EndManeuver commands replace the actions that fire would 
 * perform.
 * 
 * @param <burnData> Pointer to an array that will be filled with the
 *                   acceleration and mass flow data.  The data returned in 
 *                   burnData has the format
 *                     burnData[0]  dVx/dt
 *                     burnData[1]  dVy/dt
 *                     burnData[2]  dVz/dt
 *                     burnData[3]  dM/dt
 *
 * @return true on success; throws on failure.
 */
//------------------------------------------------------------------------------
bool FiniteBurn::Fire(Real *burnData, Real epoch)
{
   #ifdef DEBUG_FINITE_BURN
      MessageInterface::ShowMessage("FiniteBurn::Fire entered for %s\n",
         instanceName.c_str());
   #endif

   if (initialized == false)
      Initialize();


   frame = frameman->GetFrameInstance(coordAxes);
   if (frame == NULL)
      throw BurnException("Maneuver frame undefined");
    
   Real *satState;
   if (sc)
      satState = sc->GetState().GetState();
   else
      throw BurnException("Maneuver initial state undefined (No spacecraft?)");
   
   Real state[6];
   
   // Transform from J2000 body state to burn origin state
   TransformJ2kToBurnOrigin(satState, state, epoch);

   #ifdef DEBUG_BURN_ORIGIN
      MessageInterface::ShowMessage("FiniteBurn Vectors:\n   "
         "Sat   = [%lf %lf %lf %lf %lf %lf\n   "
         "state = [%lf %lf %lf %lf %lf %lf\n   "
         "Frame = [%lf %lf %lf\n   " 
         "         %lf %lf %lf\n   "
         "         %lf %lf %lf]\n\n",
         satState[0], satState[1], satState[2], satState[3], satState[4], 
         satState[5], state[0], state[1], state[2], state[3], state[4], 
         state[5], frameBasis[0][0], frameBasis[0][1], frameBasis[0][2],
         frameBasis[1][0], frameBasis[1][1], frameBasis[1][2],
         frameBasis[2][0], frameBasis[2][1], frameBasis[2][2]);
   #endif
   
   // Set the state 6-vector from the associated spacecraft
   frame->SetState(state);
   // Calculate the maneuver basis vectors
   frame->CalculateBasis(frameBasis);
   
   // Accumulate the individual accelerations from the thrusters
   Real dm, tMass, tOverM, *dir, norm;
   deltaV[0] = deltaV[1] = deltaV[2] = 0.0;
   Thruster *current;
   
   tMass = sc->GetRealParameter("TotalMass");

   #ifdef DEBUG_FINITE_BURN
      MessageInterface::ShowMessage(
         "   Maneuvering spacecraft %s\n",
         sc->GetName().c_str());
      MessageInterface::ShowMessage(
         "   Position for burn:    %18le  %18le  %18le\n",
         (*state)[0], (*state)[1], (*state)[2]);
      MessageInterface::ShowMessage(
         "   Velocity for burn: %18le  %18le  %18le\n   Mass = %18le kg\n",
         (*state)[3], (*state)[4], (*state)[5], tMass);
   #endif
   
   for (StringArray::iterator i = thrusters.begin(); 
        i != thrusters.end(); ++i) {

      #ifdef DEBUG_FINITE_BURN
         MessageInterface::ShowMessage("   Accessing thruster %s\n", 
            (*i).c_str());
      #endif

      current = (Thruster *)sc->GetRefObject(Gmat::THRUSTER, *i);
      if (!current)
         throw BurnException("FiniteBurn::Fire requires thruster named " +
            (*i) + " on spacecraft " + sc->GetName());
            
      dir = current->direction;
      norm = sqrt(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]);
      if (norm == 0.0)
         throw BurnException("FiniteBurn::Fire thruster " + (*i) +
                             " on spacecraft " + sc->GetName() +
                             " has no direction.");
            
      dm = current->CalculateMassFlow();
      tOverM = current->thrust * current->thrustScaleFactor / (tMass * norm);
      deltaV[0] += dir[0] * tOverM;
      deltaV[1] += dir[1] * tOverM;
      deltaV[2] += dir[2] * tOverM;

      #ifdef DEBUG_FINITE_BURN
         MessageInterface::ShowMessage("   Thruster %s = %s details:\n", 
            (*i).c_str(), current->GetName().c_str());
         MessageInterface::ShowMessage(
            "      dM    = %16.13le\n      Mass  = %16.13lf\n"
            "      TSF   = %16.13lf\n      |Acc| = %16.13le\n      "
            "Acc   = [%16.13le   %16.13le   %16.13le]\n", dm, tMass, 
            current->thrustScaleFactor, tOverM,
            deltaV[0], deltaV[1], deltaV[2]);
      #endif
   }

   // Build the acceleration
   burnData[0] = deltaV[0]*frameBasis[0][0] +
                 deltaV[1]*frameBasis[0][1] +
                 deltaV[2]*frameBasis[0][2];
   burnData[1] = deltaV[0]*frameBasis[1][0] +
                 deltaV[1]*frameBasis[1][1] +
                 deltaV[2]*frameBasis[1][2];
   burnData[2] = deltaV[0]*frameBasis[2][0] +
                 deltaV[1]*frameBasis[2][1] +
                 deltaV[2]*frameBasis[2][2];

   #ifdef DEBUG_FINITE_BURN
      MessageInterface::ShowMessage(
         "   Acceleration from burn:  %18le  %18le  %18le\n   dm/dt: %18le\n",
         burnData[0], burnData[1], burnData[2], burnData[3]);
   #endif

   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the FiniteBurn.
 *
 * @return clone of the FiniteBurn.
 */
//------------------------------------------------------------------------------
GmatBase* FiniteBurn::Clone() const
{
   return (new FiniteBurn(*this));
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
void FiniteBurn::Copy(const GmatBase* orig)
{
   operator=(*((FiniteBurn *)(orig)));
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Renames reference object name used in this class.
 *
 * @param <type> reference object type.
 * @param <oldName> object name to be renamed.
 * @param <newName> new object name.
 * 
 * @return true if object name changed, false if not.
 */
//---------------------------------------------------------------------------
bool FiniteBurn::RenameRefObject(const Gmat::ObjectType type,
                                 const std::string &oldName,
                                 const std::string &newName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("FiniteBurn::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type == Gmat::HARDWARE)
   {
      for (UnsignedInt i=0; i<thrusters.size(); i++)
         if (thrusters[i] == oldName)
            thrusters[i] = newName;
   
      for (UnsignedInt i=0; i<tanks.size(); i++)
         if (tanks[i] == oldName)
            tanks[i] = newName;
   }
   
   return Burn::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// bool FiniteBurn::Initialize()
//------------------------------------------------------------------------------
/**
 * This method sets up the data structures and pointers for a finite burn.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool FiniteBurn::Initialize()
{
   /// @todo Consolidate Finite & Impulsive burn initialization into base class
   if (!sc)
      throw BurnException("FiniteBurn::Initialize() cannot access spacecraft");
   
   if (Burn::Initialize())
   {
      ObjectArray tankArray = sc->GetRefObjectArray(Gmat::FUEL_TANK);
      ObjectArray thrusterArray = sc->GetRefObjectArray(Gmat::THRUSTER);
   
      // Now set tank pointers for thrusters associated with this burn
      if (!tanks.empty())
      {
         // Look up the thruster(s)
         for (ObjectArray::iterator th = thrusterArray.begin(); 
              th != thrusterArray.end(); ++th)
         {
            for (StringArray::iterator thName = thrusters.begin();
                 thName != thrusters.end(); ++thName)
            {
               // Only act on thrusters assigned to this burn
               if ((*th)->GetName() == *thName)
               {
                  // Setup the tanks
                  (*th)->TakeAction("ClearTanks");
                  // Loop through each tank for the burn
                  for (StringArray::iterator tankName = tanks.begin();
                       tankName != tanks.end(); ++tankName)
                  {
                     ObjectArray::iterator tnk = tankArray.begin();
                     // Find the tank on the spacecraft
                     while (tnk != tankArray.end())
                     {
                        if ((*tnk)->GetName() == *tankName)
                        {
                           // Make the assignment
                           (*th)->SetStringParameter("Tank", *tankName);
                           (*th)->SetRefObject(*tnk, (*tnk)->GetType(), 
                                               (*tnk)->GetName());
                           break;
                        }
                        // Not found; keep looking 
                        ++tnk;
                        if (tnk == tankArray.end())
                           throw BurnException("FiniteBurn::Initialize() "
                              "cannot find tank " + (*tankName) + " for burn " +
                              instanceName);
                     }
                  }
               }
            }
         }
      }
      
      initialized = true;
   }
   
   return initialized;
}
