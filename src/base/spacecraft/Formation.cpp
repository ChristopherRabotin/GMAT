//$Header$
//------------------------------------------------------------------------------
//                              Formation
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CI63P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/7/24
//
/**
 * Implements the class used for formations. 
 */
//------------------------------------------------------------------------------


#include "Formation.hpp"
#include <algorithm>          // for find()

//#define DEBUG_FORMATION
//#define DEBUG_FORMATION_ACTIONS

#include "MessageInterface.hpp"


//---------------------------------
// static data
//---------------------------------
const std::string
Formation::PARAMETER_TEXT[FormationParamCount - SpaceObjectParamCount] =
{
   "Add",
   "Remove",
   "Clear"
};


const Gmat::ParameterType
Formation::PARAMETER_TYPE[FormationParamCount - SpaceObjectParamCount] =
{
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRING_TYPE,     
   Gmat::BOOLEAN_TYPE
};


//---------------------------------
// public members
//---------------------------------

//------------------------------------------------------------------------------
// Formation(Gmat::ObjectType typeId, const std::string &typeStr,
//           const std::string &instName)
//------------------------------------------------------------------------------
/**
 * Default constructor.
 *
 * @param <typeId>   Gmat::ObjectType of the constructed object.
 * @param <typeStr>  String describing the type of object created.
 * @param <instName> Name of the constructed instance.
 */
//------------------------------------------------------------------------------
Formation::Formation(Gmat::ObjectType typeId, const std::string &typeStr,
                     const std::string &instName) :
   SpaceObject    (typeId, typeStr, instName),
   dimension      (0),
   satCount       (0)
{
   objectTypes.push_back(Gmat::FORMATION);
   objectTypeNames.push_back("Formation");

   parameterCount = FormationParamCount;
}


//------------------------------------------------------------------------------
// ~Formation()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Formation::~Formation()
{
}


//------------------------------------------------------------------------------
// Formation(const Formation& orig)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <orig> Formation that is copied onto this one.
 */
//------------------------------------------------------------------------------
Formation::Formation(const Formation& orig) :
   SpaceObject    (orig),
   componentNames (orig.componentNames),
   dimension      (orig.dimension),
   satCount       (orig.satCount)
{
   parameterCount = FormationParamCount;
   
   // No need to clear components here because this is a fresh object, and 
   // nothing has been loaded into it yet
   //components.clear();
}


//------------------------------------------------------------------------------
// Formation& operator=(const Formation& orig)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <so> SpaceObject that is copied onto this one.
 *
 * @return this instance, configured like the input instance.
 */
//------------------------------------------------------------------------------
Formation& Formation::operator=(const Formation& orig)
{
   if (this != &orig)
   {
      SpaceObject::operator=(orig);
      
      componentNames = orig.componentNames;
      dimension      = orig.dimension;
      satCount       = orig.satCount;
      
      // Don't clear components here because the assignment operator is used 
      // DURING propagation to evaluate stopping conditions 
      // components.clear();
   }
   #ifdef DEBUG_FORMATION
      MessageInterface::ShowMessage(
         "Formation::operator= : Formation '%s' has %d components\n", 
         instanceName.c_str(), components.size());
   #endif
   
   return *this;
}


//------------------------------------------------------------------------------
// Utility methods used by the CoordinateSystem code
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const Rvector6 GetMJ2000State(const A1Mjd &atTime)
//------------------------------------------------------------------------------
/**
 * Access the MJ2000 state for this SpaceObject.
 *
 * @param <atTime> Epoch for the state data.
 *
 * @return The Cartesian MJ2000 state.
 *
 * @todo Implement GetMJ2000State in the derived classes, and remove this
 *       implementation.
 */
//------------------------------------------------------------------------------
const Rvector6 Formation::GetMJ2000State(const A1Mjd &atTime)
{
   satCount = components.size();
   
   if (j2000Body == NULL)
      throw SpaceObjectException("MJ2000 body not yet set for " +
         instanceName);

   // First calculate the geometric center of the formation
   Rvector6 centerState;

   PropState ps = GetState();
   Real *st = ps.GetState();
   
   if (satCount == 0)
   {
      MessageInterface::ShowMessage(
         "Warning: Attempting to find MJ2000 state for an empty formation\n");
      return centerState;
   }

   // The Formation PropState contains state data for the spacecraft, tanks, and
   // (eventually) attitude.  The first 6*satcount elements are the spacecraft
   // position and velocity data.
   for (UnsignedInt i = 0; i < satCount; ++i)
   {
      for (Integer j = 0; j < 6; ++j)
         centerState[j] += st[i*6+j];
   }
   centerState /= satCount;

   // Then calculate the J2000 data based on that state
   Rvector6 bodyState = j2000Body->GetMJ2000State(atTime);

   // If origin is NULL, assume it is set at the J2000 origin.
   if (origin)
   {
      Rvector6 offset = origin->GetMJ2000State(atTime);
      bodyState -= offset;
   }

   return centerState - bodyState;
}


//------------------------------------------------------------------------------
// Accessor methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param <type> Type of the object that is renamed.
 * @param <oldName> The current name for the object.
 * @param <newName> The name the object has when this operation is complete.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool Formation::RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName)
{
   #if DEBUG_RENAME
      MessageInterface::ShowMessage
         ("Formation::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
          GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   // Formation needs to know about spacecraft or other formation only
   if (type != Gmat::SPACECRAFT && type != Gmat::FORMATION )
      return true;
   
   //@todo replace any object name here
   for (UnsignedInt i=0; i<componentNames.size(); i++)
   {
      if (componentNames[i] == oldName)
         componentNames[i] = newName;
   }
   
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Achieve.
 *
 * @return clone of the Achieve.
 */
//------------------------------------------------------------------------------
GmatBase* Formation::Clone() const
{
   return new Formation(*this);
}


//------------------------------------------------------------------------------
// void ParametersHaveChanged(bool flag)
//------------------------------------------------------------------------------
/**
 * Uset to set or clear the parmsChanged flag from outside of the SpaceObject.
 * 
 * @param <flag>  The new value for the flag.
 */
//------------------------------------------------------------------------------
void Formation::ParametersHaveChanged(bool flag)
{
   parmsChanged = flag;
   // Set the flag for all of the members
   for (std::vector<SpaceObject*>::iterator i = components.begin(); 
        i != components.end(); ++i)
      (*i)->ParametersHaveChanged(flag);
}



//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Formation::GetParameterText(const Integer id) const
{
   if (id >= SpaceObjectParamCount && id < FormationParamCount)
      return PARAMETER_TEXT[id - SpaceObjectParamCount];
   return SpaceObject::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer Formation::GetParameterID(const std::string &str) const
{
   for (Integer i = SpaceObjectParamCount; i < FormationParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SpaceObjectParamCount])
         return i;
   }
   
   return SpaceObject::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Formation::GetParameterType(const Integer id) const
{
   if (id >= SpaceObjectParamCount && id < FormationParamCount)
      return PARAMETER_TYPE[id - SpaceObjectParamCount];
      
   return SpaceObject::GetParameterType(id);
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
bool Formation::IsParameterReadOnly(const Integer id) const
{
   if ((id == REMOVED_SPACECRAFT) || (id == CLEAR_NAMES))
      return true;

   return SpaceObject::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve a boolean parameter.
 *
 * @param <id> The id for the parameter.
 *
 * @return the boolean value for this parameter.
 */
//------------------------------------------------------------------------------
bool Formation::GetBooleanParameter(const Integer id) const
{
   switch (id)
   {
      case CLEAR_NAMES:
         return false;
      default:
         return SpaceObject::GetBooleanParameter(id);
   }
}

//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieve a boolean parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return the boolean value for this parameter.
 */
//------------------------------------------------------------------------------
bool Formation::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a boolean parameter.
 *
 * This method is used to clear the Formation's list of spacecraft.
 *
 * @param <id> The id for the parameter.
 * @param <value> The new value for the parameter.
 *
 * @return the boolean value for this parameter.
 */
//------------------------------------------------------------------------------
bool Formation::SetBooleanParameter(const Integer id, const bool value)
{
   #ifdef DEBUG_FORMATION_ACTIONS
      MessageInterface::ShowMessage(
         "Formation::SetBooleanParameter called with id = %d\n", id);
   #endif
 
   switch (id)
   {
      case CLEAR_NAMES:
         return ClearSpacecraftList();
      default:
         return SpaceObject::SetBooleanParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a boolean parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new value for the parameter.
 *
 * @return the boolean value for this parameter.
 */
//------------------------------------------------------------------------------
bool Formation::SetBooleanParameter(const std::string &label,
                                    const bool value)
{
   #ifdef DEBUG_FORMATION_ACTIONS
      MessageInterface::ShowMessage(
         "Formation::SetBooleanParameter called with label = %s\n",
         label.c_str());
   #endif

   return SetBooleanParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Formation::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param <id>    ID for the requested parameter.
 * @param <value> string value for the requested parameter.
 *
 * @return  success flag.
 */
//------------------------------------------------------------------------------
bool Formation::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == ADDED_SPACECRAFT)
   {
      if (find(componentNames.begin(), componentNames.end(), value) != 
              componentNames.end())
         return false;
      componentNames.push_back(value);

      #ifdef DEBUG_FORMATION_ACTIONS
         MessageInterface::ShowMessage("%s%s%s\n",
            "Formation \"", instanceName.c_str(),
            "\" now consists of these spacecraft names:");
         for (StringArray::iterator i = componentNames.begin(); 
              i != componentNames.end(); ++i)
            MessageInterface::ShowMessage("    \"%s\"\n", i->c_str());
      #endif

      parmsChanged = true;
      return true;
   }
   if (id == REMOVED_SPACECRAFT)
   {
      return RemoveSpacecraft(value);
   }
   return SpaceObject::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param <id>    ID for the parameter whose value to change.
 * @param <value> value for the parameter.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real Formation::SetRealParameter(const Integer id, const Real value)
{
   Real retval = SpaceObject::SetRealParameter(id, value);
      
   if (id == EPOCH_PARAM)
   {
      if (retval != value)
         throw SpaceObjectException(
            "Formation update returned incorrect epoch");
      // Update the epoch on the constituent pieces
      for (std::vector<SpaceObject*>::iterator i = components.begin();
           i != components.end(); ++i)
      {
         retval = (*i)->SetRealParameter(id, value);
         if (retval != value)
            throw SpaceObjectException(
               "Formation constituent returned incorrect epoch");
      }
   }
   
   return retval;
}


//------------------------------------------------------------------------------
//  Real  SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param <label> String description for the parameter value.
 * @param <value> value for the parameter.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real Formation::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value,
//                          const Integer index)
//------------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <id>    The integer ID for the parameter.
 * @param <value> The new string for this parameter.
 * @param <index> Index for parameters in arrays. 
 *
 * @return true if the string is stored, false if not.
 */
//------------------------------------------------------------------------------
bool Formation::SetStringParameter(const Integer id, const std::string &value,
                                   const Integer index)
{
   if (id == ADDED_SPACECRAFT)
   {
      return false;
   }
   if (id == REMOVED_SPACECRAFT)
   {
      return RemoveSpacecraft(value);
   }
   return SpaceObject::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string  Formation::GetStringParameter(const Integer id) const
{
   if (id == REMOVED_SPACECRAFT)
   {
      return "";
   }
   return SpaceObject::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param <id>    ID for the requested parameter.
 * @param <index> Index for the requested parameter.
 *
 * @return  string value of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string  Formation::GetStringParameter(const Integer id,
                                           const Integer index) const
{
   if (id == REMOVED_SPACECRAFT)
   {
      return "";
   }
   return SpaceObject::GetStringParameter(id, index);
}



//------------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& Formation::GetStringArrayParameter(const Integer id) const
{
   if (id == ADDED_SPACECRAFT)
      return componentNames;
   return SpaceObject::GetStringArrayParameter(id);
}


const StringArray& 
         Formation::GetStringArrayParameter(const std::string &label) const
{
   if (label == "Add")
      return componentNames;
   return SpaceObject::GetStringArrayParameter(label);
}


//------------------------------------------------------------------------------
// GmatBase* Formation::GetRefObject(const Gmat::ObjectType type,
//                                   const std::string &name,
//                                   const Integer index)
//------------------------------------------------------------------------------
/**
 * Accessor for referenced objects.
 *
 * @param <type>  reference object type.
 * @param <name>  reference object name.
 * @param <index> Which reference object array to search.
 *
 * @return Pointer to the reference object.
 *
 * @note This method is implemented to ensure that polymorphism doen't confuse
 *       some compilers.
 */
//------------------------------------------------------------------------------
GmatBase* Formation::GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name,
                                  const Integer index)
{
   return SpaceObject::GetRefObject(type, name, index);
}


//------------------------------------------------------------------------------
// bool Formation::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                              const std::string &name)
//------------------------------------------------------------------------------
/**
 * Accessor used to set or replace a referenced object.
 *
 * @param <obj>   the reference object.
 * @param <type>  reference object type.
 * @param <name>  reference object name.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool Formation::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name)
{
   SpaceObject *so;
   
   if (type == Gmat::SPACECRAFT)
   {
      so = ((SpaceObject*)(obj));
      if (find(components.begin(), components.end(), so) == components.end())
      {
         PropState *ps = &(so->GetState());
         Integer size = ps->GetSize();
         dimension += size;
         Real newepoch = so->GetEpoch();
         if (components.size() == 0)
            state.SetEpoch(newepoch);
         else
            if (state.GetEpoch() != newepoch)
            {
               char errorMsg[256];
               sprintf(errorMsg, "Epochs (%lf) and (%lf) are not synchronized "
                       "in the formation %s", newepoch, state.GetEpoch(),
                       instanceName.c_str());
              
               throw SpaceObjectException(errorMsg);
            }
         components.push_back(so);
      }
      
      return true;
   }
   
   return SpaceObject::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// bool Formation::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                              const std::string &name)
//------------------------------------------------------------------------------
/**
 * Accessor used to set or replace a referenced object.
 *
 * @param <obj>   the reference object.
 * @param <type>  reference object type.
 * @param <name>  reference object name.
 * @param <index> index specifying which array is used.
 *
 * @return true on success.
 *
 * @note This method is implemented to ensure that polymorphism doen't confuse
 *       some compilers.
 */
//------------------------------------------------------------------------------
bool Formation::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                    const std::string &name,
                                    const Integer index)
{
   return SpaceObject::SetRefObject(obj, type, name, index);
}


//------------------------------------------------------------------------------
//  ObjectArray& GetRefObjectArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Obtains an array of GmatBase pointers by type.
 *
 * @param <type> The type of objects requested
 *
 * @return Reference to the array.
 */
//------------------------------------------------------------------------------
ObjectArray& Formation::GetRefObjectArray(const Gmat::ObjectType type)
{
   static ObjectArray oa;
   oa.clear();
   if ((type == Gmat::SPACECRAFT) || (type == Gmat::FORMATION))
   {
      for (std::vector<SpaceObject *>::iterator i = components.begin();
           i != components.end(); ++i)
         if ((*i)->GetType() == type)
            oa.push_back(*i);
      return oa;
   }

   if (type == Gmat::SPACEOBJECT)
   {
      for (std::vector<SpaceObject *>::iterator i = components.begin();
           i != components.end(); ++i)
         oa.push_back(*i);
      return oa;
   }
      
   return SpaceObject::GetRefObjectArray(type);
}


//------------------------------------------------------------------------------
//  ObjectArray& GetRefObjectArray(const std::string& typeString)
//------------------------------------------------------------------------------
/**
 * Obtains an array of GmatBase pointers by type.
 *
 * @param <typeString> The type name of objects requested
 *
 * @return Reference to the array.
 */
//------------------------------------------------------------------------------
ObjectArray& Formation::GetRefObjectArray(const std::string& typeString)
{
   Gmat::ObjectType id = Gmat::UNKNOWN_OBJECT;
   if (typeString == "Spacecraft") 
      id = Gmat::SPACECRAFT;
   if (typeString == "Formation") 
      id = Gmat::FORMATION;
   if (typeString == "SpaceObject") 
      id = Gmat::SPACEOBJECT;
      
   if (id != Gmat::UNKNOWN_OBJECT)
      return GetRefObjectArray(id);
      
   return SpaceObject::GetRefObjectArray(typeString);
}


//------------------------------------------------------------------------------
// void BuildState()
//------------------------------------------------------------------------------
/**
 * Constructs a PropState for a Formation.
 */
//------------------------------------------------------------------------------
void Formation::BuildState()
{
   #ifdef DEBUG_FORMATION_ACTIONS
      MessageInterface::ShowMessage("%s%s%s%d\n",
         "In BuildState, Formation \"", instanceName.c_str(),
         "\" has dimension ", dimension);
   #endif

   if (dimension <= 0)
      throw SpaceObjectException(
         "Error building Formation state; no spacecraft are set");

   // Setup the PropState
   Real *data = new Real[dimension], *st;
   Integer j = 0, k;
   PropState *ps;

   if (state.GetSize() < dimension)
      state.SetSize(dimension);
   
   for (std::vector<SpaceObject*>::iterator i = components.begin();
        i != components.end(); ++i)
   {
      if ((*i) == NULL)
         throw SpaceObjectException(
            "Error building Formation state; member spacecraft not set");
      ps = &((*i)->GetState());
      st = ps->GetState();
      for (k = 0; k < ps->GetSize(); ++k)
      {
         data[j + k] = st[k];
      }
      j += ps->GetSize();
   }
   
   #ifdef DEBUG_FORMATION_ACTIONS
      MessageInterface::ShowMessage("%s%s%s\n",
         "In BuildState, Formation \"", instanceName.c_str(),
         "\" consists of these spacecraft names:");
      for (StringArray::iterator i = componentNames.begin(); 
           i != componentNames.end(); ++i)
         MessageInterface::ShowMessage("    \"%s\"\n", i->c_str());
  
      MessageInterface::ShowMessage("%s%s%s\n",
         "In BuildState, Formation \"", instanceName.c_str(),
         "\" consists of these spacecraft:");
      for (std::vector<SpaceObject *>::iterator j = components.begin(); 
           j < components.end(); ++j)
      {
         MessageInterface::ShowMessage("    \"%s\"\n", (*j)->GetName().c_str());
      }
   #endif
   
   if (!state.SetState(data, dimension))
      throw SpaceObjectException("Error building Formation state");
}


//------------------------------------------------------------------------------
// void UpdateElements()
//------------------------------------------------------------------------------
/**
 * Updates the member SpaceObjects using the data in the Formation PropState.
 */
//------------------------------------------------------------------------------
void Formation::UpdateElements()
{
   Integer size, index = 0;
   PropState *ps;
   for (std::vector<SpaceObject*>::iterator i = components.begin();
        i != components.end(); ++i)
   {
      ps = &((*i)->GetState());
      size = ps->GetSize();
      memcpy(ps->GetState(), &((state.GetState())[index]), size*sizeof(Real));
      index += size;
      if ((*i)->GetType() == Gmat::FORMATION)
         ((Formation*)(*i))->UpdateElements();
   }
}


//------------------------------------------------------------------------------
// void UpdateState()
//------------------------------------------------------------------------------
/**
 * Updates the internal PropState data from the member SpaceObjects.
 */
//------------------------------------------------------------------------------
void Formation::UpdateState()
{
   Integer size, index = 0;
   PropState *ps;
   
   Real ep0 = 0.0, ep;
   for (std::vector<SpaceObject*>::iterator i = components.begin();
        i != components.end(); ++i)
   {
      if (i == components.begin())
      {
         ep0 = (*i)->GetEpoch();
         #ifdef DEBUG_FORMATION
            MessageInterface::ShowMessage("Base epoch is %.12lf\n", ep0);
         #endif
      }
      else
      {
         ep = (*i)->GetEpoch();
         if (ep != ep0)
            MessageInterface::ShowMessage(
               "WARNING!  Formation Member Epochs are not synchronized!\n"
               "First spacecraft epoch is %.12lf, but %s has epoch %.12lf\n", 
               ep0, (*i)->GetName().c_str(), ep);
      }
      ps = &((*i)->GetState());
      size = ps->GetSize();
      
      #ifdef DEBUG_FORMATION_UPDATES
         MessageInterface::ShowMessage(
            "Formation: Updating(%d to %d) from %s::%s\n", 
            index, index + size - 1, instanceName.c_str(), (*i)->GetName().c_str());
      #endif
      
      memcpy(&((state.GetState())[index]), ps->GetState(), size*sizeof(Real));
      index += size;
      if ((*i)->GetType() == Gmat::FORMATION)
         ((Formation*)(*i))->UpdateState();
   }
   
   SetEpoch(ep0);
}


//------------------------------------------------------------------------------
// bool TakeAction(const std::string &action, const std::string &actionData)
//------------------------------------------------------------------------------
/**
 * This method performs action.
 *
 * @param <action> action to perform
 * @param <actionData> action data associated with action
 *
 * @return true if action successfully performed
 */
//------------------------------------------------------------------------------
bool Formation::TakeAction(const std::string &action,
                           const std::string &actionData)
{
   if (action == "Clear")
   {
      return ClearSpacecraftList();
   }
   else if (action == "Remove")
   {
      return RemoveSpacecraft(actionData);
   }
   
   return false;
}


void Formation::ClearLastStopTriggered()
{
   lastStopTriggered = "";
   // Clear it on all members
   for (std::vector <SpaceObject *>::iterator i = components.begin(); 
        i != components.end(); ++i)
      (*i)->ClearLastStopTriggered();
      
   #ifdef DEBUG_STOPCONDITION_TRACKING
      MessageInterface::ShowMessage(
         "Cleared stop identifier from Formation \"%s\"\n", 
         instanceName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// bool ClearSpacecraftList()
//------------------------------------------------------------------------------
/*
 * Clears the list of spacecraft.
 *
 * @return true if the list was cleared.
 */
//------------------------------------------------------------------------------
bool Formation::ClearSpacecraftList()
{
   #ifdef DEBUG_FORMATION_ACTIONS
      MessageInterface::ShowMessage(
         "Formation::ClearSpacecraftList() called\n");
   #endif
   componentNames.clear();
   components.clear();
   return true;
}


//------------------------------------------------------------------------------
// bool RemoveSpacecraft(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Removes spacecraft from the spacecraft list
 *
 * @param <name> spacecraft name to be removed from the list
 *
 * @return true if spacecraft was removed from the list, false otherwise
 */
//------------------------------------------------------------------------------
bool Formation::RemoveSpacecraft(const std::string &name)
{
   #ifdef DEBUG_FORMATION_ACTIONS
      MessageInterface::ShowMessage("%s%s\"\n",
         "Formation::RemoveSpacecraft called to remove \"", name.c_str());
   #endif

   StringArray::iterator scPos = 
      find(componentNames.begin(), componentNames.end(), name);
   
   if (scPos != componentNames.end())
   {
      componentNames.erase(scPos);

      // Now remove the pointer from the component list
      std::vector<SpaceObject *>::iterator j;
      SpaceObject *current;
      for (j = components.begin(); j < components.end(); ++j)
      {
         current = *j;
         if (current->GetName() == name)
         {
            components.erase(j);
            break;
         }
      }

      #ifdef DEBUG_FORMATION_ACTIONS
         MessageInterface::ShowMessage("%s%s%s\n",
            "Formation \"", instanceName.c_str(),
            "\" now consists of these spacecraft names:");
         for (StringArray::iterator i = componentNames.begin(); 
              i != componentNames.end(); ++i)
            MessageInterface::ShowMessage("    \"%s\"\n", i->c_str());
     
         MessageInterface::ShowMessage("%s%s%s\n",
            "Formation \"", instanceName.c_str(),
            "\" now consists of these spacecraft:");
         for (j = components.begin(); j < components.end(); ++j)
         {
            MessageInterface::ShowMessage(
               "    \"%s\"\n", (*j)->GetName().c_str());
         }
      #endif

      parmsChanged = true;
      return true;
   }

   #ifdef DEBUG_FORMATION_ACTIONS
      MessageInterface::ShowMessage("%s%s\"\n",
         "Formation::RemoveSpacecraft Did not find \"", name.c_str());
   #endif

   return false;
}
