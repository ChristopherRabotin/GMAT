//$Id$
//------------------------------------------------------------------------------
//                            GroundStation
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2008/08/01

// Modified:
//    2010.06.03 Tuan Nguyen
//      - Add AddHardware parameter and verify added hardware
//    2010.03.23 Steve Hughes/Thomas Grubb
//      - Changed initialize method to use statetype = "Spherical" instead
//        of deprecated "geographical" state type
//    2010.03.19 Thomas Grubb
//      - Overrode Copy method
//
/**
 * Implements the Groundstation class used to model ground based tracking stations.
 */
//------------------------------------------------------------------------------

#include "GroundStation.hpp"
#include "MessageInterface.hpp"
#include "AssetException.hpp"


//#define DEBUG_OBJECT_MAPPING
//#define DEBUG_INIT
//#define DEBUG_HARDWARE
//#define TEST_GROUNDSTATION


//---------------------------------
// static data
//---------------------------------

/// Labels used for the ground station parameters.
const std::string
GroundStation::PARAMETER_TEXT[GroundStationParamCount - BodyFixedPointParamCount] =
   {
      "Id",
      "AddHardware",				// made changes by Tuan Nguyen
   };

const Gmat::ParameterType
GroundStation::PARAMETER_TYPE[GroundStationParamCount - BodyFixedPointParamCount] =
   {
      Gmat::STRING_TYPE,
      Gmat::OBJECTARRAY_TYPE,		// made changes by Tuan Nguyen
   };



//---------------------------------
// public methods
//---------------------------------


//---------------------------------------------------------------------------
//  GroundStation(const std::string &itsName)
//---------------------------------------------------------------------------
/**
 * Constructs a GroundStation object (default constructor).
 *
 * @param <itsName> Optional name for the object.  Defaults to "".
 */
//---------------------------------------------------------------------------
GroundStation::GroundStation(const std::string &itsName) :
   GroundstationInterface    ("GroundStation", itsName),
   stationId                 ("StationId")
{
   objectTypeNames.push_back("GroundStation");
   parameterCount = GroundStationParamCount;

   bfcsName   = "EarthFixed";
   mj2kcsName = "EarthMJ2000Eq";
}

//---------------------------------------------------------------------------
// ~GroundStation()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
GroundStation::~GroundStation()
{
	// delete all hardware
	for (UnsignedInt i = 0; i < hardwareList.size(); ++i)
		delete hardwareList[i];
}

//---------------------------------------------------------------------------
//  GroundStation(const GroundStation& gs)
//---------------------------------------------------------------------------
/**
 * Constructs a new GroundStation by copying the input instance (copy
 * constructor).
 *
 * @param gs  GroundStation instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
GroundStation::GroundStation(const GroundStation& gs) :
   GroundstationInterface        (gs),
   stationId             (gs.stationId)
{
	hardwareNames 		= gs.hardwareNames;		// made changes by Tuan Nguyen
// hardwareList 		= gs.hardwareList;		// should it be cloned ????
}


//---------------------------------------------------------------------------
//  GroundStation& operator=(const GroundStation& gs)
//---------------------------------------------------------------------------
/**
 * Assignment operator for GroundStations.
 *
 * @param gs The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
GroundStation& GroundStation::operator=(const GroundStation& gs)
{
   if (&gs != this)
   {
      GroundstationInterface::operator=(gs);

      stationId 	  = gs.stationId;
      hardwareNames = gs.hardwareNames;		// made changes by Tuan Nguyen
//      hardwareList	= gs.hardwareList;		// should it be cloned ????
   }

   return *this;
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
void GroundStation::Copy(const GmatBase* orig)
{
   operator=(*((GroundStation *)(orig)));
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Makes a copy of this instance and returns it as a GmatBase pointer.
 *
 * @return The clone of the GroundStation, as a GmatBase pointer.
 */
//------------------------------------------------------------------------------
GmatBase* GroundStation::Clone() const
{
   return new GroundStation(*this);
}



//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer GroundStation::GetParameterID(const std::string & str) const
{
   for (Integer i = BodyFixedPointParamCount; i < GroundStationParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - BodyFixedPointParamCount])
         return i;
   }

   return GroundstationInterface::GetParameterID(str);
}

//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
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
std::string GroundStation::GetParameterText(const Integer id) const
{
   if (id >= BodyFixedPointParamCount && id < GroundStationParamCount)
      return PARAMETER_TEXT[id - BodyFixedPointParamCount];
   return GroundstationInterface::GetParameterText(id);
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
std::string GroundStation::GetParameterTypeString(const Integer id) const
{
   return GroundstationInterface::PARAM_TYPE_STRING[GetParameterType(id)];
}

//---------------------------------------------------------------------------
//  std::string GetParameterUnit(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the unit for the parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return unit for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string GroundStation::GetParameterUnit(const Integer id) const
{
   return GroundstationInterface::GetParameterUnit(id);
}

//---------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the enumerated type of the object.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The enumeration for the type of the parameter, or
 *         UNKNOWN_PARAMETER_TYPE.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType GroundStation::GetParameterType(const Integer id) const
{
   if (id >= BodyFixedPointParamCount && id < GroundStationParamCount)
      return PARAMETER_TYPE[id - BodyFixedPointParamCount];

   return GroundstationInterface::GetParameterType(id);
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
bool GroundStation::IsParameterReadOnly(const Integer id) const
{
   return GroundstationInterface::IsParameterReadOnly(id);
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
bool GroundStation::IsParameterReadOnly(const std::string & label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method retrieves a string parameter
 *
 * @param id The ID for the parameter
 *
 * @return The string parameter
 */
//------------------------------------------------------------------------------
std::string GroundStation::GetStringParameter(const Integer id) const
{
   if (id == STATION_ID)
      return stationId;

   return GroundstationInterface::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string & value)
//------------------------------------------------------------------------------
/**
 * This method sets a string parameter
 *
 * @param id The ID for the parameter
 * @param value The new string value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool GroundStation::SetStringParameter(const Integer id,
      const std::string & value)
{
   if (id == STATION_ID)
   {
      if (IsValidID(value))
      {
         stationId = value;
         return true;
      }
      else
      {
         AssetException ae;
         ae.SetDetails(errorMessageFormat.c_str(), value.c_str(), "Id",
							  "Must begin with a letter; may contain letters, integers, dashes, underscores");
         throw ae;
      }
   }

   // made changes by Tuan Nguyen
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

   return GroundstationInterface::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string & label) const
//------------------------------------------------------------------------------
/**
 * This method retrieves a string parameter
 *
 * @param label The string label for the parameter
 *
 * @return The parameter
 */
//------------------------------------------------------------------------------
std::string GroundStation::GetStringParameter(const std::string & label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string & label, const std::string & value)
//------------------------------------------------------------------------------
/**
 * This method sets a string parameter
 *
 * @param label The string label for the parameter
 * @param value The new string value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool GroundStation::SetStringParameter(const std::string & label,
      const std::string & value)
{
   return SetStringParameter(GetParameterID(label), value);
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
std::string GroundStation::GetStringParameter(const Integer id,
      const Integer index) const
{
//   MessageInterface::PopupMessage(Gmat::WARNING_, "GroundStation::GetStringParameter(id, index)");
   if (index < 0)
   {
      AssetException ex;
      ex.SetDetails("The index %d is out-of-range for field \"%s\"", index,
                    GetParameterText(id).c_str());
      throw ex;
   }

   // made changes by Tuan Nguyen
   switch (id)
   {
      case ADD_HARDWARE:
         {
            if ((0 <= index)&&(index < (Integer)hardwareNames.size()))
			   return hardwareNames[index];
			else
			   return "";
		 }
      default:
         break;      // intentional drop through
   }

   return GroundstationInterface::GetStringParameter(id, index);
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
std::string GroundStation::GetStringParameter(const std::string & label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string & label,
//       const std::string & value, const Integer index)
//------------------------------------------------------------------------------
/**
 * This method calls the base class method.  It is provided for overload
 * compatibility.  See the base class description for a full description.
 */
//------------------------------------------------------------------------------
bool GroundStation::SetStringParameter(const std::string & label,
      const std::string & value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id,
//       const std::string & value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a specific string in a StringArray
 *
 * This method changes a specific string in a StringArray if a string has been
 * set at the location selected by the index value.  If the index exceeds the
 * size of the name array, the participant name is added to the end of the list.
 *
 * @param id The ID for the StringArray parameter that is being changed
 * @param value The string that needs to be placed in the StringArray
 * @param index The location for the string in the list.  If index exceeds the
 *              size of the StringArray, the string is added to the end of the
 *              array
 *
 * @return true If the string was processed
 */
//------------------------------------------------------------------------------
bool GroundStation::SetStringParameter(const Integer id,
      const std::string & value, const Integer index)
{
   if (index < 0)
   {
      AssetException ex;
      ex.SetDetails("The index %d is out-of-range for field \"%s\"", index,
                    GetParameterText(id).c_str());
      throw ex;
   }

   switch (id)
   {
	  case ADD_HARDWARE:				// made changes by Tuan Nguyen
		 // Only add the hardware if it is not in the list already
		 if (find(hardwareNames.begin(), hardwareNames.end(), value) ==
   	            hardwareNames.end())
		 {
			 //hardwareNames[index] = value;		// This line causes system collapse due to index beyond array's size
			 hardwareNames.push_back(value);
		 }
		 return true;

	  default:
		 break;		// intentional drop through
   }

   return GroundstationInterface::SetStringParameter(id, value, index);
}

//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Accesses StringArray parameters.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//---------------------------------------------------------------------------
const StringArray& GroundStation::GetStringArrayParameter(const Integer id) const
{
   if (id == ADD_HARDWARE)
      return hardwareNames;

   return GroundstationInterface::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accesses StringArray parameters.
 *
 * @param label The script string for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& GroundStation::GetStringArrayParameter(
      const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool GroundStation::RenameRefObject(const Gmat::ObjectType type,
                                 const std::string &oldName,
                                 const std::string &newName)
{
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
      return true;
   }

   return GroundstationInterface::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// const StringArray&
// GroundStation::GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Gets an array of referenced object names
 *
 * @param type The type of referenced objects
 *
 * @return a StringArray containing a list of the referenced object names
 */
//------------------------------------------------------------------------------
const StringArray&
GroundStation::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   #ifdef DEBUG_INIT
      MessageInterface::ShowMessage("GroundStation::GetRefObjectNameArray(%d)",
            type);
   #endif

   static StringArray retval;
   retval.clear();

   switch(type)
   {
      case Gmat::UNKNOWN_OBJECT:
      case Gmat::HARDWARE:
         retval = hardwareNames;
         if (type == Gmat::HARDWARE)
            break;

      default:
         break;
   }

   // Now pick up the objects that the base classes need
   StringArray baseNames = GroundstationInterface::GetRefObjectNameArray(type);
   for (UnsignedInt i = 0; i < baseNames.size(); ++i)
      retval.push_back(baseNames[i]);

   #ifdef DEBUG_INIT
      MessageInterface::ShowMessage("Groundstation Ref Object Name Array:\n");
      for (UnsignedInt i = 0; i < retval.size(); ++i)
         MessageInterface::ShowMessage("   %s\n", retval[i].c_str());
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// GmatBase* GroundStation::GetRefObject(const Gmat::ObjectType type,
//                                     const std::string &name)
//------------------------------------------------------------------------------
/**
 * Gets the referenced object specified by type and its name.
 *
 * @param type The type of the referenced object
 * @param name The name of the referenced object
 *
 * @return the referenced object match with type and name.
 */
//------------------------------------------------------------------------------
GmatBase* GroundStation::GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name)
{
   switch (type)
   {
      case Gmat::UNKNOWN_OBJECT:
      case Gmat::HARDWARE:
		 for(ObjectArray::iterator i = hardwareList.begin();
					i < hardwareList.end(); ++i)
		 {
		    if ((*i)->GetName() == name)
			   return (*i);
		 }
		 break;

      default:
    	 break;
   }
   return GroundstationInterface::GetRefObject(type, name);
}

//------------------------------------------------------------------------------
// bool GroundStation::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                                     const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Sets the referenced object to a given object specified by type and its name.
 *
 * @param obj The object which is assigned to the referenced object.
 * @param type The type of the referenced object
 * @param name The name of the referenced object
 *
 * @return true when the assignment is successful.
 */
//------------------------------------------------------------------------------
bool GroundStation::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name)
{
   #ifdef DEBUG_INIT
      MessageInterface::ShowMessage("GroundStation::SetRefObject(obj, "
            "type, name):  obj.name = %s  type = %d  name = %s\n",
            obj->GetName().c_str(), type, name.c_str());
   #endif

   if (obj == NULL)
	  return false;

   if (type == Gmat::HARDWARE)	// work for hardware
   {
      if (obj->GetType() == Gmat::HARDWARE)
      {
    	 // Don't add if it's already there
     	 bool hardwareRegistered = false;
     	 for (UnsignedInt i=0; i < hardwareList.size(); ++i)
     	 {
     		if (hardwareList[i]->GetName() == obj->GetName())
     		{
     		   hardwareRegistered = true;
     		   break;
     		}
     	 }
     	 if (!hardwareRegistered)
     	 {
     		hardwareList.push_back(obj->Clone());	// a hardware needs to be cloned
     	 }
     	 return true;
      }

      return false;		// <-- throw here; It was supposed to be hardware, but isn't.
   }

   return GroundstationInterface::SetRefObject(obj, type, name);
}

ObjectArray& GroundStation::GetRefObjectArray(const Gmat::ObjectType type)
{
	switch(type)
	{
	case Gmat::HARDWARE:
		return hardwareList;

	default:
		break;
	}

	return GroundstationInterface::GetRefObjectArray(type);
}


ObjectArray& GroundStation::GetRefObjectArray(const std::string& typeString)
{
	if (typeString == "Hardware")
	{
		return hardwareList;
	}

	return GroundstationInterface::GetRefObjectArray(typeString);
}



//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool GroundStation::HasRefObjectTypeArray()
{
   return true;
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
const ObjectTypeArray& GroundStation::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   GroundstationInterface::GetRefObjectTypeArray();
   refObjectTypes.push_back(Gmat::HARDWARE);
   return refObjectTypes;
}



//-------------------------------------------------------------------------
// This function is used to verify GroundStation's added hardware.
//
// return true if there is no error, false otherwise.
//-------------------------------------------------------------------------
// made changes by Tuan Nguyen
bool GroundStation::VerifyAddHardware()
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
							  primaryAntenna->GetName().c_str(), obj->GetName().c_str(), this->GetName().c_str());
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


//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
bool GroundStation::Initialize()
{
   #ifdef DEBUG_INIT
      MessageInterface::ShowMessage("GroundStation::Initializing %s\n", instanceName.c_str());
   #endif

   // Call the parent class to initialize its data
   GroundstationInterface::Initialize();

   // Initialize hardware data
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


   // made changes by Tuan Nguyen
   // verify GroundStation's referenced objects
   if (VerifyAddHardware() == false)	// verify add hardware
	  return false;

   return true;
}


bool GroundStation::IsEstimationParameterValid(const Integer id)
{
   bool retval = false;

   return retval;
}

Integer GroundStation::GetEstimationParameterSize(const Integer item)
{
   Integer retval = 1;

//   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;

   retval = GroundstationInterface::GetEstimationParameterSize(item);

   return retval;
}


Real* GroundStation::GetEstimationParameterValue(const Integer item)
{
   Real *retval = NULL;

//   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;

   retval = GroundstationInterface::GetEstimationParameterValue(item);

   return retval;
}

//------------------------------------------------------------------------------
// bool  IsValidID(const std::string &id)
//------------------------------------------------------------------------------
/**
 * Checks to see if the input string represents a valid station ID.
 *
 * As of 2010.12.23, valid IDs must:
 *    - start with a letter
 * As of 2010.12.23, valid IDs may:
 *    - contain letters & integers
 *    - contain only the following special characters:  dashes, underscores
 *
 * @param id string for the station ID.
 *
 * @return true if input is a valid ID; false otherwise.
 */
//------------------------------------------------------------------------------
bool GroundStation::IsValidID(const std::string &id)
{
   // first character must be a letter
   if (!isalpha(id[0]))  return false;

   // each character must be a letter, an integer, a dash, or an underscore
   unsigned int sz = id.size();
   for (unsigned int ii = 0; ii < sz; ii++)
      if (!isalnum(id[ii]) && id[ii] != '-' && id[ii] != '_') return false;

   return true;
}

