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
// Author:  Joey Gurganus
// Created: 2003/10/22
//
/**
 * Implements the Spacecraft base class. 
 */
//------------------------------------------------------------------------------

#include "Spacecraft.hpp"
#include "StateVectorException.hpp"
#include "MessageInterface.hpp"

// #define DEBUG_SPACECRAFT 1 

#if DEBUG_SPACECRAFT
#include <iostream>
#include <sstream> 
#endif

// Spacecraft parameter types
const Gmat::ParameterType   
   Spacecraft::PARAMETER_TYPE[SC_ParamCount - SpaceObjectParamCount] =
   {
      Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE,
      Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::STRING_TYPE, Gmat::STRING_TYPE,
      Gmat::STRING_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE,
      Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::STRINGARRAY_TYPE,
      Gmat::STRINGARRAY_TYPE, Gmat::REAL_TYPE
   };

//-------------------------------------
// public methods
//-------------------------------------

//---------------------------------------------------------------------------
//  Spacecraft()
//---------------------------------------------------------------------------
/**
 * Creates default constructor.
 *
 */
Spacecraft::Spacecraft() : 
    SpaceObject    (Gmat::SPACECRAFT,"Spacecraft","")
{
    objectTypes.push_back(Gmat::SPACECRAFT);
    objectTypeNames.push_back("Spacecraft");
    DefineDefaultSpacecraft();
    parameterCount = SC_ParamCount;
}

//---------------------------------------------------------------------------
//  Spacecraft(const std::string &name)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <name> Optional name for the object.  Defaults to "".
 *
 */
Spacecraft::Spacecraft(const std::string &name) :
    SpaceObject    (Gmat::SPACECRAFT, "Spacecraft", name)
{
    objectTypes.push_back(Gmat::SPACECRAFT);
    objectTypeNames.push_back("Spacecraft");
    DefineDefaultSpacecraft();
    parameterCount = SC_ParamCount;
}

//---------------------------------------------------------------------------
//  Spacecraft(const std::string &typeStr, const std::string &name)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <typeStr> GMAT script string associated with this type of object.
 * @param <name> Optional name for the object.  Defaults to "".
 *
 */
Spacecraft::Spacecraft(const std::string &typeStr, const std::string &name) :
    SpaceObject    (Gmat::SPACECRAFT, typeStr, name)
{
    objectTypes.push_back(Gmat::SPACECRAFT);
    objectTypeNames.push_back("Spacecraft");
    DefineDefaultSpacecraft();
    parameterCount = SC_ParamCount;
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
    SpaceObject    (a)
{
    parameterCount = a.parameterCount;
    InitializeDataMethod(a);
}

//---------------------------------------------------------------------------
//  ~Spacecraft(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
Spacecraft::~Spacecraft(void)
{
   // Delete the attached hardware (it was set as clones)
   for (ObjectArray::iterator i = tanks.begin(); i < tanks.end(); ++i)
      delete *i;
   for (ObjectArray::iterator i = thrusters.begin(); i < thrusters.end(); ++i)
      delete *i;
}

//---------------------------------------------------------------------------
//  Spacecraft& operator=(const Spacecraft &a)
//---------------------------------------------------------------------------
/**
 * Assignment operator for Spacecraft structures.
 *
 * @param <a> The original that is being copied.
 *
 * @return Reference to this object
 * 
 * @todo Determine how to handle hardware when copying Spacecraft objects.
 */
Spacecraft& Spacecraft::operator=(const Spacecraft &a)
{
    SpaceObject::operator=(a);
    // Don't do anything if copying self
    if (&a == this)
        return *this;

    // Duplicate the member data        
    InitializeDataMethod(a);

    return *this;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Spacecraft.
 *
 * @return clone of the Spacecraft.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Spacecraft::Clone(void) const
{
   return (new Spacecraft(*this));
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
#if DEBUG_SPACECRAFT
   MessageInterface::ShowMessage("\n*** Spacecraft::GetRefObjectName() ***\n");
#endif

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
#if DEBUG_SPACECRAFT
   MessageInterface::ShowMessage("\n*** Spacecraft::GetRefObjectNameArray()"
                                 " ****\n");
#endif

   static StringArray fullList;  // Maintain scope if the full list is requested

   //loj: 4/28/05 Added UNKNOWN_OBJECT
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


//------------------------------------------------------------------------------
// bool SetRefObjectName(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets the reference objects.
 * 
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 *
 * @return true on success, false on failure.
 */
bool Spacecraft::SetRefObjectName(const Gmat::ObjectType type, 
                                  const std::string &name)
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
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 *
 * @return reference object pointer.
 */
GmatBase* Spacecraft::GetRefObject(const Gmat::ObjectType type, 
                                   const std::string &name)
{
   // This switch statement intentionally drops through without breaks, so that
   // the search in the tank and thruster name lists only need to be coded once. 
   switch (type)
   {
      case Gmat::COORDINATE_SYSTEM:
         return coordinateSystem;
         
      case Gmat::HARDWARE:
      case Gmat::FUEL_TANK:
         for (ObjectArray::iterator i = tanks.begin(); 
              i < tanks.end(); ++i) {
            if ((*i)->GetName() == name)
               return *i;
         }
      
      case Gmat::THRUSTER:
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

   return SpaceObject::GetRefObject(type, name);
}


//---------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                   const std::string &name)
//---------------------------------------------------------------------------
/**
 * Sets the referenced object.
 *
 * @param <obj> reference object pointer.
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 *
 * @return success of the operation.
 */
bool Spacecraft::SetRefObject(GmatBase *obj, const Gmat::ObjectType type, 
                              const std::string &name)
{
#if DEBUG_SPACECRAFT
    MessageInterface::ShowMessage("\n*** Spacecraft::SetRefObject() ***\n");
#endif

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
      coordinateSystem = (CoordinateSystem*)obj;
      return true;
   }

   return SpaceObject::SetRefObject(obj, type, name);
}

//bool Spacecraft::SetRefObject(GmatBase *obj, const Gmat::ObjectType type, 
//                              const std::string &name, const Integer index)
//{
////   if (type == Gmat::FUEL_TANK)
////      return tankNames;
////   if (type == Gmat::THRUSTER)
////      return thrusterNames;
//   return SpaceObject::SetRefObject(obj, type, name, index);
//}


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

    if (epoch.IsValidFormat(str)) return EPOCH_PARAM;

    if (str == "Element1" || stateVector.IsElement(1,str))
       return ELEMENT1_ID;

    if (str == "Element2" || stateVector.IsElement(2,str))
       return ELEMENT2_ID;

    if (str == "Element3" || stateVector.IsElement(3,str))
       return ELEMENT3_ID;

    if (str == "Element4" || stateVector.IsElement(4,str))
       return ELEMENT4_ID;

    if (str == "Element5" || stateVector.IsElement(5,str))
       return ELEMENT5_ID;

    if (str == "Element6" || stateVector.IsElement(6,str))
       return ELEMENT6_ID;

    if (str == "StateType") return STATE_TYPE_ID;
    if (str == "AnomalyType") return ANOMALY_ID; 
    if (str == "CoordinateSystem") return COORD_SYS_ID;

    // Drag Coefficient  
    if (str == "Cd") return CD_ID;    

    // Reflectivity Coefficient
    if (str == "Cr") return CR_ID;

    if (str == "DragArea") return DRAG_AREA_ID;

    if (str == "DryMass") return DRY_MASS_ID;

    if (str == "SRPArea") return SRP_AREA_ID;

    // Representation specific values
    if (str == "Position") {
        /// @todo: Force the representation to Cartesian -- later build 
        return ELEMENT1_ID;
    }
    if (str == "Velocity") {
        /// @todo: Force the representation to Cartesian -- later build 
        return ELEMENT4_ID;
    }

    // Added for hardware support 9/13/04, djc
    if (str == "Tanks") return FUEL_TANK_ID;

    if (str == "Thrusters") return THRUSTER_ID;
    
    if (str == "TotalMass") return TOTAL_MASS_ID;
    
    return SpaceObject::GetParameterID(str);
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
std::string Spacecraft::GetParameterText(const Integer id) const
{
#if DEBUG_SPACECRAFT
    MessageInterface::ShowMessage(
        "\n*** Spacecraft::GetParameterText(%d) ***\n", id);
#endif

    if (id == EPOCH_PARAM)
       return epoch.GetLabel();

    if (id >= ELEMENT1_ID && id <= ELEMENT6_ID)
    {
       Integer tempID = id - ELEMENT1_ID + 1; 

#if DEBUG_SPACECRAFT
       MessageInterface::ShowMessage("\nid(%d) is between element#1(%d) "
                                     " and element#6(%d)",id,ELEMENT1_ID,
                                     ELEMENT6_ID);
       MessageInterface::ShowMessage("\ntempID = %d and "
                                     "SpaceObjectParamCount = %d\n",
                                     tempID,SpaceObjectParamCount);
#endif

       return stateVector.GetLabel(tempID);
    }


    if (id == STATE_TYPE_ID) return "StateType";
    if (id == ANOMALY_ID) return "AnomalyType";

    if (id == COORD_SYS_ID) return "CoordinateSystem";

    if (id == DRY_MASS_ID) return "DryMass";
    
    if (id == CD_ID) return "Cd" ;

    if (id == CR_ID) return "Cr";

    if (id == DRAG_AREA_ID) return "DragArea";

    if (id == SRP_AREA_ID) return "SRPArea";

    // Added for hardware support 9/13/04, djc
    if (id == FUEL_TANK_ID) return "Tanks";

    if (id == THRUSTER_ID) return "Thrusters";
    
    if (id == TOTAL_MASS_ID) return "TotalMass";

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
   if (id >= SpaceObjectParamCount && id < SC_ParamCount)
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
#if DEBUG_SPACECRAFT
   std::string label = GetParameterText(id);
   MessageInterface::ShowMessage("\n*** Spacecraft::GetRealParameter(%d), "
                                 "label = %s\n", id, label.c_str());
#endif

    if (id >= ELEMENT1_ID && id <= ELEMENT6_ID)
    {
       Integer tempID = id - ELEMENT1_ID + 1;
       try
       {
          return stateVector.GetElement(tempID);
       }
       catch(StateVectorException &sve)
       {
          throw SpaceObjectException(sve.GetMessage());         
       } 
    }
          
    if (id == DRY_MASS_ID) return dryMass;

    if (id == CD_ID) return coeffDrag;
    if (id == CR_ID) return reflectCoeff;
    if (id == DRAG_AREA_ID) return dragArea;
    if (id == SRP_AREA_ID) return srpArea;
    
    if (id == TOTAL_MASS_ID) { 
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
   Integer id = GetParameterID(label);
#if DEBUG_SPACECRAFT
  MessageInterface::ShowMessage("\n*** Spacecraft::GetRealParameter(%s), id = "
                                 "%d",label.c_str(), id);
#endif

   if (stateVector.IsElement(label))
      return (stateVector.GetElement(label));
     
   return GetRealParameter(id);
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
#if DEBUG_SPACECRAFT
   std::string label = GetParameterText(id);
   MessageInterface::ShowMessage(
       "\nSpacecraft::SetRealParameter(%d,%f) enters...\n\t"
       "stateType = %s, label = %s\n", id, value,
       stateVector.GetType().c_str(), label.c_str());
#endif

    if (id >= ELEMENT1_ID && id <= ELEMENT6_ID)
    {
       // Return value if setting the element is successful 
       if (stateVector.SetElement(id,value))
       { 
          SetPropState();

          return stateVector.GetElement(id);
       }
    }

    if (id == DRY_MASS_ID) return dryMass = value;

    if (id == CD_ID) return coeffDrag = value;
    if (id == CR_ID) return reflectCoeff = value;
    if (id == DRAG_AREA_ID) return dragArea = value;
    if (id == SRP_AREA_ID) return srpArea = value;
                                                 // Don't change the total mass
    if (id == TOTAL_MASS_ID) return totalMass;    

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
#if DEBUG_SPACECRAFT
   Integer id = GetParameterID(label);
   std::string actualLabel = GetParameterText(id);
   MessageInterface::ShowMessage(
       "\nSpacecraft::SetRealParameter(%s,%f) enters...\n\t"
       "stateType = %s, \n\t"
       "id = %d, actualLabel = %s\n", label.c_str(), value,
       stateVector.GetType().c_str(), id, actualLabel.c_str());
#endif

   if (stateVector.SetElement(label,value))
   {
      SetPropState();

      return stateVector.GetElement(label);
   }

   return SetRealParameter(GetParameterID(label),value);
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
    if (id == EPOCH_PARAM)
       return epoch.GetValue();

    if (id == STATE_TYPE_ID)
       return stateVector.GetType(); 

    if (id == ANOMALY_ID)
       return stateVector.GetAnomalyType(); 

    if (id == COORD_SYS_ID)
       return coordSysName; 

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
#if DEBUG_SPACECRAFT
   MessageInterface::ShowMessage("\nSpacecraft::GetStringParameter(\"%s\")",
                     label.c_str());
   MessageInterface::ShowMessage("\n.......ID = %d ********** ",
                     GetParameterID(label));
   MessageInterface::ShowMessage("(%s) ********** \n",
                     GetStringParameter(GetParameterID(label)).c_str());
#endif

   if (epoch.IsValidFormat(label))
      return epoch.GetValue(label); 
          
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
#if DEBUG_SPACECRAFT
    MessageInterface::ShowMessage("\n***Spacecraft::SetStringParameter(%d,%s)",
                                  id, value.c_str()); 
#endif

    if (id != EPOCH_PARAM && id != STATE_TYPE_ID && id != ANOMALY_ID
        && id != COORD_SYS_ID && id != FUEL_TANK_ID && id != THRUSTER_ID)
       return SpaceObject::SetStringParameter(id, value);

    if (id == EPOCH_PARAM)
    {
       if (epoch.SetValue(value))
       {
          SetEpoch(epoch.GetRealValue(),false);
          return true;
       }
       return SpaceObject::SetStringParameter(id, value);
    }
    else if (id == STATE_TYPE_ID)
    {  
       // Check for invalid input then return unknown value from GmatBase 
       if (!stateVector.SetType(value)) 
          return SpaceObject::SetStringParameter(id, value);
    }
    else if (id == ANOMALY_ID)
    {
       // Check for invalid input then return unknown value from GmatBase 
       if (!stateVector.SetAnomalyType(value))
          return SpaceObject::SetStringParameter(id, value);
    }
    else if (id == COORD_SYS_ID) 
    {
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
    else  // id == THRUSTER_ID
    {   
       // Only add the tank if it is not in the list already
       if (find(thrusterNames.begin(), thrusterNames.end(), 
           value) == thrusterNames.end()) {
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
    MessageInterface::ShowMessage("\nSpacecraft::SetStringParameter(\"%s\""
                                  ",\"%s\") enters... \n", 
                                  label.c_str(),value.c_str());
    Integer id = GetParameterID(label);
    MessageInterface::ShowMessage("\nSpacecraft::SetStringParameter: "
                                  "GetParameterText -> %s", 
                                  GetParameterText(id).c_str());
#endif

    if (epoch.IsValidFormat(label))
    {
       if (epoch.SetValue(label,value))
       {
          SetEpoch(epoch.GetRealValue(),false);
          return true;
       }

       // Otherwise, send it to upper-level for failure
       return SpaceObject::SetStringParameter(label,value);
    }

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
   if (action == "SetupHardware") {
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
       (action == "RemoveThruster")) {

      bool removeTank = true, removeThruster = true, removeAll = false;
      if (action == "RemoveTank")
         removeThruster = false;
      if (action == "RemoveThruster")
         removeTank = false;
      if (actionData == "")
         removeAll = true;
         
      if (removeThruster) {
         if (removeAll) {
            thrusters.clear();
            thrusterNames.clear();
         }
         else {
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

      if (removeTank) {
         if (removeAll) {
            tanks.clear();
            tankNames.clear();
         }
         else {
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
   
   return SpaceObject::TakeAction(action, actionData);
}

//---------------------------------------------------------------------------
//  Rvector6 GetStateVector() const
//---------------------------------------------------------------------------
/**
 * Get the state.
 *
 * @return state values.
 *
 */
Rvector6 Spacecraft::GetStateVector() const
{
   return stateVector.GetValue();
}

//---------------------------------------------------------------------------
//  Rvector6 GetStateVector(const std::string &elementType) const
//---------------------------------------------------------------------------
/**
 * Get the state in a given element type.
 *
 * @param <elementType>  Element type to be needed for the conversion
 *
 * @return state in the conversion to a given element type
 *
 */
Rvector6 Spacecraft::GetStateVector(const std::string &elementType) const
{
   return stateVector.GetValue(elementType);
}


//---------------------------------------------------------------------------
//  bool SetState(const Rvector6 instate)
//---------------------------------------------------------------------------
/**
 * Set the given states to Cartesian states.
 *
 * @param <instate> element state.
 * 
 * @return true if successful; otherwise, false
 */
bool Spacecraft::SetStateVector(const Rvector6 instate)
{
#if DEBUG_SPACECRAFT
   MessageInterface::ShowMessage
      ("Spacecraft::SetStateVector(\"Rvector\"), stateType = %s\n",
       stateVector.GetType().c_str());
#endif

   return SetStateVector(stateVector.GetType(),instate);
}


//---------------------------------------------------------------------------
//  bool SetStateVector(const std::string &elementType, 
//                      const Rvector6 instate)
//---------------------------------------------------------------------------
/**
 * Set the given states with given state type to Cartesian states.
 *
 * @param <elementType> element type.
 * @param <instate>     element state.
 * 
 * @return true if successful; otherwise, false
 */
bool Spacecraft::SetStateVector(const std::string &elementType,
                                const Rvector6 instate)
{
#if DEBUG_SPACECRAFT
   MessageInterface::ShowMessage
      ("Spacecraft::SetStateVector(%s, \"Rvector\"), stateType = %s\n",
       elementType.c_str(), stateVector.GetType().c_str());
#endif

   if (!stateVector.SetValue(elementType,instate))
      return false;

   SetPropState();

   return true;
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
#if DEBUG_SPACECRAFT
    MessageInterface::ShowMessage("\n*** Spacecraft::Initialize() ***\n");
#endif 

   // Set the mu if CelestialBody is there thru coordinate system's origin;   
   // Otherwise, discontine process and send the error message   
   if (!stateVector.SetCoordSys(coordinateSystem))
   {      throw SpaceObjectException(
                "\nError:  Spacecraft has empty coordinate system\n");
   }   

   return true;
}


//-------------------------------------
// private methods
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
// void DefineDefaultSpacecraft()
//------------------------------------------------------------------------------
void Spacecraft::DefineDefaultSpacecraft()
{
   // Initialization for PropState
   state.SetEpoch(21545.0);
   state[0] = 7100.0;
   state[1] = 0.0;
   state[2] = 1300.0;
   state[3] = 0.0;
   state[4] = 7.35;
   state[5] = 1.0;

   // Initialize epoch and state vector 
   epoch.SetValue(state.GetEpoch());
   stateVector.SetValue(state.GetState());    // @todo:  need improvement

   // Get the keplerian state and then initialize anomaly
   Rvector6 tempKepl = GetStateVector("Keplerian");  

   coordSysName = "EarthMJ2000Eq";

   dryMass = 850.0;
   coeffDrag = 2.2;
   dragArea = 15.0;
   srpArea = 1.0;
   reflectCoeff = 1.8;
}

//---------------------------------------------------------------------------
//  void InitializeDataMethod(const Spacecraft &s)
//---------------------------------------------------------------------------
/**
 * Initialize data method from the parameter for copying spacecraft structures.
 *
 * @param <s> The original that is being copied.
 */
void Spacecraft::InitializeDataMethod(const Spacecraft &s)
{
#if DEBUG_SPACECRAFT
    MessageInterface::ShowMessage("\n***Spacecraft::InitializeDataMethod() "
            "enters ***\nstateType = %s",s.stateVector.GetType().c_str());
#endif
    // Duplicate the member data
    epoch = s.epoch;
    stateVector = s.stateVector;
    coordSysName = s.coordSysName; //loj: 4/28/05 Added

    for (int i = 0; i < 6; ++i)
    {
       state[i] = s.state[i];
    }

    dryMass = s.dryMass;
    coeffDrag = s.coeffDrag;
    dragArea = s.dragArea;
    srpArea = s.srpArea;
    reflectCoeff = s.reflectCoeff;

    tankNames = s.tankNames;
    thrusterNames = s.thrusterNames;
}

//---------------------------------------------------------------------------
//  void SetPropState() 
//---------------------------------------------------------------------------
/**
 * Set the PropState in cartesian state from the current state.
 *
 */
void Spacecraft::SetPropState()
{
   Rvector6 tempState = stateVector.GetValue(); 

   if (stateVector.GetType() != "Cartesian")
      tempState = stateVector.GetValue("Cartesian");
 
   for (Integer i=0; i < 6; ++i)
       state[i] =  tempState[i];
}
