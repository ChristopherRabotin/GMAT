//$Header$
//------------------------------------------------------------------------------
//                                  CalculatedPoint
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Wendy C. Shoan
// Created: 2005/04/04
//
/**
 * Implementation of the CalculatedPoint class.
 *
 * @note This is an abstract class.
 */
//------------------------------------------------------------------------------

#include <vector>
#include "gmatdefs.hpp"
#include "SpacePoint.hpp"
#include "CalculatedPoint.hpp"
#include "SolarSystemException.hpp"
#include <algorithm>              // for find()

//---------------------------------
// static data
//---------------------------------
const std::string
CalculatedPoint::PARAMETER_TEXT[CalculatedPointParamCount - SpacePointParamCount] =
{
   "NumberOfBodies",
   "BodyNames",
};

const Gmat::ParameterType
CalculatedPoint::PARAMETER_TYPE[CalculatedPointParamCount - SpacePointParamCount] =
{
   Gmat::INTEGER_TYPE,
   Gmat::OBJECTARRAY_TYPE,
//   Gmat::STRINGARRAY_TYPE,
};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  CalculatedPoint(const std::string &ptType, 
//                  const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the CalculatedPoint class
 * (default constructor).
 *
 * @param <ptType>  string representation of its body type
 * @param <itsName> parameter indicating the name of the CalculatedPoint.
 */
//------------------------------------------------------------------------------
CalculatedPoint::CalculatedPoint(const std::string &ptType, 
                                 const std::string &itsName) :
SpacePoint(Gmat::CALCULATED_POINT, ptType, itsName),
numberOfBodies (0)
{
   objectTypes.push_back(Gmat::CALCULATED_POINT);
   objectTypeNames.push_back("CalculatedPoint");
   parameterCount = CalculatedPointParamCount;
}

//------------------------------------------------------------------------------
//  CalculatedPoint(const CalculatedPoint &cp)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the CalculatedPoint class as a copy of the
 * specified CalculatedPoint class (copy constructor).
 *
 * @param <cp> CalculatedPoint object to copy.
 */
//------------------------------------------------------------------------------
CalculatedPoint::CalculatedPoint(const CalculatedPoint &cp) :
SpacePoint          (cp)
{
   // copy the list of body pointers
   for (unsigned int i = 0; i < (cp.bodyList).size(); i++)
   {
      bodyList.push_back((cp.bodyList).at(i));
   }
   // copy the list of body names
   for (unsigned int i = 0; i < (cp.bodyNames).size(); i++)
   {
      bodyNames.push_back((cp.bodyNames).at(i));
   }
   numberOfBodies = (Integer) bodyList.size();
}

//------------------------------------------------------------------------------
//  CalculatedPoint& operator= (const CalculatedPoint& cp)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the CalculatedPoint class.
 *
 * @param <cp> the CalculatedPoint object whose data to assign to "this"
 *             calculated point.
 *
 * @return "this" CalculatedPoint with data of input CalculatedPoint cp.
 */
//------------------------------------------------------------------------------
CalculatedPoint& CalculatedPoint::operator=(const CalculatedPoint &cp)
{
   if (&cp == this)
      return *this;

   SpacePoint::operator=(cp);
   bodyNames.clear();
   bodyList.clear();
   // copy the list of body pointers
   for (unsigned int i = 0; i < (cp.bodyList).size(); i++)
   {
      bodyList.push_back((cp.bodyList).at(i));
   }
   // copy the list of body names
   for (unsigned int i = 0; i < (cp.bodyNames).size(); i++)
   {
      bodyNames.push_back((cp.bodyNames).at(i));
   }
   numberOfBodies = (Integer) bodyList.size();
   return *this;
}

//------------------------------------------------------------------------------
//  ~CalculatedPoint()
//------------------------------------------------------------------------------
/**
 * Destructor for the CalculatedPoint class.
 */
//------------------------------------------------------------------------------
CalculatedPoint::~CalculatedPoint()
{
   // nothing to do here ..... hm .. hm .. hm
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
 *
 */
//------------------------------------------------------------------------------
std::string CalculatedPoint::GetParameterText(const Integer id) const
{
   if (id >= SpacePointParamCount && id < CalculatedPointParamCount)
      return PARAMETER_TEXT[id - SpacePointParamCount];
   return SpacePoint::GetParameterText(id);
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
 *
 */
//------------------------------------------------------------------------------
Integer     CalculatedPoint::GetParameterID(const std::string &str) const
{
   for (Integer i = SpacePointParamCount; i < CalculatedPointParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SpacePointParamCount])
         return i;
   }
   
   // Special handler for "Add" - per Steve 2005.05.18
   if (str == "Add") return BODY_NAMES;
   
   return SpacePoint::GetParameterID(str);
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
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType CalculatedPoint::GetParameterType(const Integer id) const
{
   if (id >= SpacePointParamCount && id < CalculatedPointParamCount)
      return PARAMETER_TYPE[id - SpacePointParamCount];
      
   return SpacePoint::GetParameterType(id);
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
 *
 */
//------------------------------------------------------------------------------
std::string CalculatedPoint::GetParameterTypeString(const Integer id) const
{
   return SpacePoint::PARAM_TYPE_STRING[GetParameterType(id)];
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
bool CalculatedPoint::IsParameterReadOnly(const Integer id) const
{
   if (id == NUMBER_OF_BODIES)
      return true;

   return SpacePoint::IsParameterReadOnly(id);
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
bool CalculatedPoint::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  Integer  GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Integer parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer     CalculatedPoint::GetIntegerParameter(const Integer id) const
{
   if (id == NUMBER_OF_BODIES)      return bodyList.size();
   return SpacePoint::GetIntegerParameter(id); 
}

//------------------------------------------------------------------------------
//  Integer  GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the Integer parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer     CalculatedPoint::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label)); 
}

//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID and index.
 *
 * @param <id>    ID for the requested parameter.
 * @param <index> index into the array of strings, for the requested parameter.
 *
 * @return  string value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string CalculatedPoint::GetStringParameter(const Integer id,
                                                const Integer index) const
{
   if (id == BODY_NAMES)             
   {
      try
      {
         return bodyNames.at(index);
      }
      catch (const std::exception &oor)
      {
         throw SolarSystemException("CalculatedPoint error: index out-of-range.");
      }
   }

   return SpacePoint::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param <id>    ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 *
 * @note - This should not be necessary here, but GCC is getting confused 
 *         cbout this method.
 *
 */
//------------------------------------------------------------------------------
std::string CalculatedPoint::GetStringParameter(const Integer id) const
{
   return SpacePoint::GetStringParameter(id);
}

//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const std::string &label, 
//                                  const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter label and index.
 *
 * @param <label> label for the requested parameter.
 * @param <index> index into the array of strings, for the requested parameter.
 *
 * @return  string value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string CalculatedPoint::GetStringParameter(const std::string &label,
                                                const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> string value for the requested parameter.
 *
 * @return  success flag.
 *
 * @note - This should not be necessary here, but GCC is getting confused 
 *         cbout this method.
 *
 */
//------------------------------------------------------------------------------
bool CalculatedPoint::SetStringParameter(const Integer id, 
                                         const std::string &value)
{        
   if (id == BODY_NAMES)
   {
      if (find(bodyNames.begin(), bodyNames.end(), value) == bodyNames.end())
      {
         bodyNames.push_back(value);
//          return true;
      }
      
//       return false;     // Name was already in the list
      
      // loj: 9/19/06 - for new Interpreter work
      // It's OK if body was already in the list
      return true;
   }

   return SpacePoint::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const std::string &label, 
//                                  const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 * @param <value> string value for the requested parameter.
 *
 * @return  success flag.
 *
 * @note - This should not be necessary here, but GCC is getting confused 
 *         cbout this method.
 *
 */
//------------------------------------------------------------------------------
bool CalculatedPoint::SetStringParameter(const std::string &label, 
                                         const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  bool  SetStringParameter(const Integer id, const std::string value.
//                           const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID, and the index.
 *
 * @param <id>    ID for the requested parameter.
 * @param <value> string value for the requested parameter.
 * @param <index> index into the array of strings.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool  CalculatedPoint::SetStringParameter(const Integer id,
                                          const std::string &value,
                                          const Integer index) 
{
   if (id == BODY_NAMES)
   {
      if ((index < 0) || (index > (Integer) bodyNames.size()))
         return false;  // throw an exception here?
      if (index == (Integer) bodyNames.size())
      {
         bodyNames.push_back(value);
         return true;
      }
      else  // replace current name
      {
         bodyNames.at(index) = value; 
         return true;
      }
   }
   
   return SpacePoint::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
//  bool  SetStringParameter(const std::string &label, const std::string value.
//                           const Integer index)
//------------------------------------------------------------------------------
/**
* This method sets the string parameter value, given the input
 * parameter label, and the index.
 *
 * @param <label> label for the requested parameter.
 * @param <value> string value for the requested parameter.
 * @param <index> index into the array of strings.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool  CalculatedPoint::SetStringParameter(const std::string &label,
                                          const std::string &value,
                                          const Integer index) 
{
   return SetStringParameter(GetParameterID(label),value,index);
}

//------------------------------------------------------------------------------
//  const StringArray&   GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* This method returns the StringArray parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
const StringArray& CalculatedPoint::GetStringArrayParameter(const Integer id) const
{
   if (id == BODY_NAMES)
   {
      return bodyNames;
   }
   
   return SpacePoint::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
//  const StringArray&   GetStringArrayParameter((const std::string &label) 
//                       const
//------------------------------------------------------------------------------
/**
 * This method returns the StringArray parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
const StringArray& CalculatedPoint::GetStringArrayParameter(
                                    const std::string &label) const
{   
   return GetStringArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name,
//                        const Integer index)
//------------------------------------------------------------------------------
/**
 * Returns the reference object pointer.
 *
 * @param <type>  type of the reference object.
 * @param <name>  name of the reference object.
 * @param <inde>x Index into the object array.
 * 
 * @return reference object pointer.
 */
//------------------------------------------------------------------------------
GmatBase* CalculatedPoint::GetRefObject(const Gmat::ObjectType type,
                                        const std::string &name,
                                        const Integer index)
{
   if (type == Gmat::SPACE_POINT) 
   {
      try
      {
         return bodyList.at(index);
      }
      catch (const std::exception &oor)
      {
         throw SolarSystemException(
               "CalculatedPoint error: index out-of-range.");
      }
   }
   return SpacePoint::GetRefObject(type, name, index);
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                   const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the reference object.
 *
 * @param <obj>   reference object pointer.
 * @param <type>  type of the reference object.
 * @param <name>  name of the reference object.
 * @param <index> Index into the object array.
 *
 * @return success of the operation.
 */
//------------------------------------------------------------------------------
bool CalculatedPoint::SetRefObject(GmatBase *obj, 
                                   const Gmat::ObjectType type,
                                   const std::string &name)
{
    //loj: 5/11/05 Added CELESTIAL_BODY and CALCULATED_POINT
   if (type == Gmat::SPACE_POINT || type == Gmat::CELESTIAL_BODY ||
       type == Gmat::CALCULATED_POINT)
   {
      // first check to see if it's already in the list
      for (unsigned int i = 0; i < bodyList.size(); i++)
      {
         if (bodyList.at(i) == (SpacePoint*) obj) return false;
      }
      
      bodyList.push_back((SpacePoint*) obj);
      numberOfBodies++;
      return true;
   }
   return SpacePoint::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Interface used to support user actions.
 *
 * @param <type>    reference object type.
 * @param <oldName> object name to be renamed.
 * @param <newName> new object name.
 * 
 * @return true if object name changed, false if not.
 */
//------------------------------------------------------------------------------
bool CalculatedPoint::RenameRefObject(const Gmat::ObjectType type,
                                      const std::string &oldName,
                                      const std::string &newName)
{
   if (type == Gmat::CALCULATED_POINT)
   {
      for (unsigned int i=0; i< bodyNames.size(); i++)
      {
         if (bodyNames[i] == oldName)
             bodyNames[i] = newName;
      }
   }
   return SpacePoint::RenameRefObject(type, oldName, newName);
}

//------------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Returns the names of the reference object. 
 *
 * @param <type> reference object type.  Gmat::UnknownObject returns all of the
 *               ref objects.
 *
 * @return The names of the reference object.
 */
//------------------------------------------------------------------------------
const StringArray& CalculatedPoint::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACE_POINT)
   {
      return bodyNames;
   }
   
   // Not handled here -- invoke the next higher GetRefObject call
   return SpacePoint::GetRefObjectNameArray(type);
}

//------------------------------------------------------------------------------
// bool TakeAction(const std::string &action, const std::string &actionData)
//------------------------------------------------------------------------------
/**
 * Interface used to support user actions.
 *
 * @param <action> The string descriptor for the requested action.
 * @param <actionData> Optional data used for the action.
 *
 * @return true if the action was performed, false if not.
 */
//------------------------------------------------------------------------------
bool CalculatedPoint::TakeAction(const std::string &action,
                                 const std::string &actionData)
{
   if (action == "ClearBodies")
   {
      bodyNames.clear();
      bodyList.clear();
      numberOfBodies = 0;
      return true; 
   }
   return SpacePoint::TakeAction(action, actionData);
}



//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time
