//$Header$
//------------------------------------------------------------------------------
//                                  CoordinateBase
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G and MOMS Task order 124
//
// Author: Wendy C. Shoan
// Created: 2004/12/20
//
/**
 * Implementation of the CoordinateBase class.  This is the base class for the 
 * CoordinateSystem and AxisSystem classes.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CoordinateBase.hpp"
#include "CoordinateSystemException.hpp"
#include "SolarSystem.hpp"
#include "SpacePoint.hpp"

//---------------------------------
// static data
//---------------------------------

const std::string
CoordinateBase::PARAMETER_TEXT[CoordinateBaseParamCount - GmatBaseParamCount] =
{
   "OriginName",
};

const Gmat::ParameterType
CoordinateBase::PARAMETER_TYPE[CoordinateBaseParamCount - GmatBaseParamCount] =
{
   Gmat::STRING_TYPE,
};


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  CoordinateBase(Gmat::ObjectType ofType, const std::string &itsType,
//             const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base CoordinateBase structures used in derived classes
 * (default constructor).
 *
 * @param <ofType>  Gmat::ObjectTypes enumeration for the object.
 * @param <itsType> GMAT script string associated with this type of object.
 * @param <itsName> Optional name for the object.  Defaults to "".
 *
 * @note There is no parameter free constructor for CoordinateBase.  Derived 
 *       classes must pass in the typeId and typeStr parameters.
 */
//---------------------------------------------------------------------------
CoordinateBase::CoordinateBase(Gmat::ObjectType ofType, 
                               const std::string &itsType,
                               const std::string &itsName) :
GmatBase(ofType,itsType,itsName),
origin         (NULL),
originName     ("Earth"),
solar          (NULL)
{
}

//---------------------------------------------------------------------------
//  CoordinateBase(const CoordinateBase &coordBase);
//---------------------------------------------------------------------------
/**
 * Constructs base CoordinateBase structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param <sp>  CoordinateBase instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
CoordinateBase::CoordinateBase(const CoordinateBase &coordBase) :
GmatBase(coordBase),
origin     (NULL),
originName (coordBase.originName),
solar      (coordBase.solar)
{
}

//---------------------------------------------------------------------------
//  CoordinateBase& operator=(const CoordinateBase &coordBase)
//---------------------------------------------------------------------------
/**
 * Assignment operator for CoordinateBase structures.
 *
 * @param <sp> The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const CoordinateBase& CoordinateBase::operator=(const CoordinateBase &coordBase)
{
   if (&coordBase == this)
      return *this;
   origin     = coordBase.origin;
   originName = coordBase.originName;
   solar      = coordBase.solar;
   
   return *this;
}
//---------------------------------------------------------------------------
//  ~CoordinateBase(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
CoordinateBase::~CoordinateBase()
{
}

//------------------------------------------------------------------------------
//  void  Initialize()
//------------------------------------------------------------------------------
/**
 * This method initializes the CoordinateBase class.
 *
 */
//------------------------------------------------------------------------------
void CoordinateBase::Initialize()
{
   if (!origin)
      throw CoordinateSystemException(
            "Origin has not been defined for CoordinateBase object");
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
 *
 */
//------------------------------------------------------------------------------
std::string CoordinateBase::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < CoordinateBaseParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
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
Integer CoordinateBase::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < CoordinateBaseParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
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
Gmat::ParameterType CoordinateBase::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < CoordinateBaseParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   
   return GmatBase::GetParameterType(id);
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
std::string CoordinateBase::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
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
 *
 */
//------------------------------------------------------------------------------
std::string CoordinateBase::GetStringParameter(const Integer id) const
{
   if (id == ORIGIN_NAME)   
   {
      if (origin) return origin->GetName();
      else        return originName;
   }

   return GmatBase::GetStringParameter(id);
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
 */
//------------------------------------------------------------------------------

 bool CoordinateBase::SetStringParameter(const Integer id, 
                                         const std::string &value)
{
   if (id == ORIGIN_NAME) 
   {
      originName = value; 
      return true;
   }
   
   return GmatBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    <label>  label ID for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
std::string CoordinateBase::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
* Accessor method used to get a parameter value
 *
 * @param    <label> Integer ID for the parameter
 * @param    <value> The new value for the parameter
 */
//------------------------------------------------------------------------------

 bool CoordinateBase::SetStringParameter(const std::string &label,
                                       const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const Gmat::ObjectType type,
//                         const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method returns a reference object from the CoordinateBase class.
 *
 * @param <type>  type of the reference object requested
 * @param <name>  name of the reference object requested
 *
 * @return pointer to the reference object requested.
 *
 */
//------------------------------------------------------------------------------
GmatBase* CoordinateBase::GetRefObject(const Gmat::ObjectType type,
                                       const std::string &name)
{
   switch (type)
   {
      case Gmat::SPACE_POINT:
         return origin;
      default:
         break;
   }
   
   // Not handled here -- invoke the next higher GetRefObject call
   return GmatBase::GetRefObject(type, name);
}

//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object for the CoordinateBase class.
 *
 * @param <obj>   pointer to the reference object
 * @param <type>  type of the reference object 
 * @param <name>  name of the reference object
 *
 * @return true if successful; otherwise, false.
 *
 */
//------------------------------------------------------------------------------
bool CoordinateBase::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                  const std::string &name)
{
   
   switch (type)
   {
      case Gmat::SPACE_POINT:
      {
         origin = (SpacePoint*) obj;
         return true;
      }
      default:
         break;
   }
   
   // Not handled here -- invoke the next higher SetRefObject call
   return GmatBase::SetRefObject(obj, type, name);
}

// WCS - leaving these in (cut-and-paste from SpacePoint) just in case ...
// DJC Added, 12/16/04
// This seems like it should NOT be needed, but GCC seems to be confused about 
// the overloaded versions of the following six methods:

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value from a vector of strings, 
 * given the input parameter ID and the index into the vector. 
 *
 * @param id ID for the requested parameter.
 * @param index index for the particular string requested.
 * 
 * @return The requested string.
 */
//------------------------------------------------------------------------------
/*
std::string CoordinateBase::GetStringParameter(const Integer id, 
                                           const Integer index) const
{
   return GmatBase::GetStringParameter(id, index);
}
*/

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value, 
//                         const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets a value on a string parameter value in a vector of strings, 
 * given the input parameter ID, the value, and the index into the vector. 
 *
 * @param id ID for the requested parameter.
 * @param value The new string.
 * @param index index for the particular string requested.
 * 
 * @return true if successful; otherwise, false.
 */
//------------------------------------------------------------------------------
/*
bool CoordinateBase::SetStringParameter(const Integer id, 
                                    const std::string &value, 
                                    const Integer index)
{
   return GmatBase::SetStringParameter(id, value, index);
}

*/
//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label, 
//                                const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value from a vector of strings, 
 * given the label associated with the input parameter and the index into the 
 * vector. 
 *
 * @param label String identifier for the requested parameter.
 * @param index index for the particular string requested.
 * 
 * @return The requested string.
 */
//------------------------------------------------------------------------------
/*
std::string CoordinateBase::GetStringParameter(const std::string &label, 
                                           const Integer index) const
{
   return GmatBase::GetStringParameter(label, index);
}
*/

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value, 
//                         const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets a value on a string parameter value in a vector of strings, 
 * given the label associated with the input parameter and the index into the 
 * vector. 
 *
 * @param label String identifier for the requested parameter.
 * @param value The new string.
 * @param index index for the particular string requested.
 * 
 * @return true if successful; otherwise, false.
 */
//------------------------------------------------------------------------------
/*
bool CoordinateBase::SetStringParameter(const std::string &label, 
                                    const std::string &value, 
                                    const Integer index)
{
   return GmatBase::SetStringParameter(label, value, index);
}
*/

