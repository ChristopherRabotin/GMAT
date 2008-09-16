//$Header$
//------------------------------------------------------------------------------
//                                  SpacePoint
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G and MOMS Task order 124
//
// Author: Wendy C. Shoan
// Created: 2004/12/07
//
/**
 * Implementation of the SpacePoint class.  This is the base class for SpaceObject
 * (base class for Spacecraft and Formation), CelestialBody (base class for Star,
 * Planet, and Moon), and CalculatedPoint (base class for LibrationPoint and
 * Barycenter)  - any object that can be used as an origin, primary, or
 * secondary when defining a CoordinateSystem.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "SpacePoint.hpp"
#include "A1Mjd.hpp"
#include "Rvector6.hpp"
#include "Rvector3.hpp"

#include "MessageInterface.hpp"


//#define DEBUG_J2000_STATE

//---------------------------------
// static data
//---------------------------------

const std::string
SpacePoint::PARAMETER_TEXT[SpacePointParamCount - GmatBaseParamCount] =
{
   "J2000BodyName",
};

const Gmat::ParameterType
SpacePoint::PARAMETER_TYPE[SpacePointParamCount - GmatBaseParamCount] =
{
   Gmat::STRING_TYPE,
};


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  SpacePoint(Gmat::ObjectType ofType, const std::string &itsType,
//             const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base SpacePoint structures used in derived classes
 * (default constructor).
 *
 * @param <ofType>  Gmat::ObjectTypes enumeration for the object.
 * @param <itsType> GMAT script string associated with this type of object.
 * @param <itsName> Optional name for the object.  Defaults to "".
 *
 * @note There is no parameter free constructor for SpacePoint.  Derived 
 *       classes must pass in the typeId and typeStr parameters.
 */
//---------------------------------------------------------------------------
SpacePoint::SpacePoint(Gmat::ObjectType ofType, const std::string &itsType,
                       const std::string &itsName) :
GmatBase(ofType,itsType,itsName),
j2000Body      (NULL),
j2000BodyName  ("Earth")
{
   objectTypes.push_back(Gmat::SPACE_POINT);
   objectTypeNames.push_back("SpacePoint");
}

//---------------------------------------------------------------------------
//  SpacePoint(const SpacePoint &sp);
//---------------------------------------------------------------------------
/**
 * Constructs base SpacePoint structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param <sp>  SpacePoint instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
SpacePoint::SpacePoint(const SpacePoint &sp) :
GmatBase(sp),
j2000Body     (NULL),
j2000BodyName (sp.j2000BodyName)
{
}

//---------------------------------------------------------------------------
//  SpacePoint& operator=(const SpacePoint &sp)
//---------------------------------------------------------------------------
/**
 * Assignment operator for SpacePoint structures.
 *
 * @param <sp> The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const SpacePoint& SpacePoint::operator=(const SpacePoint &sp)
{
   if (&sp == this)
      return *this;
   j2000Body     = sp.j2000Body;
   j2000BodyName = sp.j2000BodyName;

   return *this;
}
//---------------------------------------------------------------------------
//  ~SpacePoint(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
SpacePoint::~SpacePoint()
{
}

//------------------------------------------------------------------------------
//  bool RequiresJ2000Body()
//------------------------------------------------------------------------------
/**
 * Identifies objects that need to have the J2000 body set in the Sandbox.
 *
 * @return true for all SpacePoint objects.
 */
//------------------------------------------------------------------------------
bool SpacePoint::RequiresJ2000Body()
{
   return true;
}


// methods for accessing the bodyName or body pointer
//------------------------------------------------------------------------------
//  const std::string  GetJ2000BodyName() const
//------------------------------------------------------------------------------
/**
 * This method returns the j2000 Body name.
 *
 * @return J2000 Body name.
 *
 */
//------------------------------------------------------------------------------
const std::string SpacePoint::GetJ2000BodyName() const
{
   if (j2000Body) return j2000Body->GetName();
   else           return j2000BodyName;
}

SpacePoint* SpacePoint::GetJ2000Body() const
{
   return j2000Body;
}

bool SpacePoint::SetJ2000BodyName(const std::string &toName)
{
   j2000BodyName = toName;
   return true;
}

void SpacePoint::SetJ2000Body(SpacePoint* toBody)
{
   #ifdef DEBUG_J2000_STATE
      if (toBody)
         MessageInterface::ShowMessage("Setting J2000 body to %s for %s\n",
            toBody->GetName().c_str(), instanceName.c_str());
      else
         MessageInterface::ShowMessage("Setting J2000 body to NULL for %s\n",
            instanceName.c_str());
   #endif
   
   j2000Body = toBody;

   #ifdef DEBUG_J2000_STATE
      if (j2000Body)
         MessageInterface::ShowMessage("J2000 body is now set\n");
   #endif
}

const Rvector3 SpacePoint::GetMJ2000Acceleration(const A1Mjd &atTime)
{
   return Rvector3(0.0,0.0,0.0);
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
std::string SpacePoint::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < SpacePointParamCount)
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
Integer SpacePoint::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < SpacePointParamCount; i++)
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
Gmat::ParameterType SpacePoint::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < SpacePointParamCount)
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
std::string SpacePoint::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
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
bool SpacePoint::IsParameterReadOnly(const Integer id) const
{
   if (id == J2000_BODY_NAME)
      return true;

   return GmatBase::IsParameterReadOnly(id);
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
bool SpacePoint::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
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
std::string SpacePoint::GetStringParameter(const Integer id) const
{
   if (id == J2000_BODY_NAME)   
   {
      if (j2000Body) return j2000Body->GetName();
      else           return j2000BodyName;
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

 bool SpacePoint::SetStringParameter(const Integer id, 
                                    const std::string &value)
{
   if (id == J2000_BODY_NAME) 
   {
      j2000BodyName = value; 
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
std::string SpacePoint::GetStringParameter(const std::string &label) const
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

 bool SpacePoint::SetStringParameter(const std::string &label,
                                       const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const Gmat::ObjectType type,
//                         const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method returns a reference object from the SpacePoint class.
 *
 * @param <type>  type of the reference object requested
 * @param <name>  name of the reference object requested
 *
 * @return pointer to the reference object requested.
 *
 */
//------------------------------------------------------------------------------
GmatBase* SpacePoint::GetRefObject(const Gmat::ObjectType type,
                                   const std::string &name)
{
   switch (type)
   {
      case Gmat::SPACE_POINT:
         return j2000Body;
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
 * This method sets a reference object for the SpacePoint class.
 *
 * @param <obj>   pointer to the reference object
 * @param <type>  type of the reference object 
 * @param <name>  name of the reference object
 *
 * @return true if successful; otherwise, false.
 *
 */
//------------------------------------------------------------------------------
bool SpacePoint::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                              const std::string &name)
{
   
   switch (type)
   {
      case Gmat::SPACE_POINT:
      {
         j2000Body = (SpacePoint*) obj;
         return true;
      }
      default:
         break;
   }
   
   // Not handled here -- invoke the next higher SetRefObject call
   return GmatBase::SetRefObject(obj, type, name);
}


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
std::string SpacePoint::GetStringParameter(const Integer id, 
                                           const Integer index) const
{
   return GmatBase::GetStringParameter(id, index);
}


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
bool SpacePoint::SetStringParameter(const Integer id, 
                                    const std::string &value, 
                                    const Integer index)
{
   return GmatBase::SetStringParameter(id, value, index);
}


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
std::string SpacePoint::GetStringParameter(const std::string &label, 
                                           const Integer index) const
{
   return GmatBase::GetStringParameter(label, index);
}


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
bool SpacePoint::SetStringParameter(const std::string &label, 
                                    const std::string &value, 
                                    const Integer index)
{
   return GmatBase::SetStringParameter(label, value, index);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name, 
//                        const Integer index)
//------------------------------------------------------------------------------
/**
 * This method returns a pointer to a reference object contained in a vector of
 * objects in the SpacePoint class.
 *
 * @param type type of the reference object requested
 * @param name name of the reference object requested
 * @param index index for the particular object requested.
 *
 * @return pointer to the reference object requested.
 */
//------------------------------------------------------------------------------
GmatBase* SpacePoint::GetRefObject(const Gmat::ObjectType type,
                                   const std::string &name, 
                                   const Integer index)
{
   return GmatBase::GetRefObject(type, name, index);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                   const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets a pointer to a reference object in a vector of objects in 
 * the SpacePoint class.
 *
 * @param obj The reference object.
 * @param type type of the reference object requested
 * @param name name of the reference object requested
 * @param index index for the particular object requested.
 *
 * @return true if successful; otherwise, false.
 */
//------------------------------------------------------------------------------
bool SpacePoint::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                              const std::string &name, const Integer index)
{
   return GmatBase::SetRefObject(obj, type, name, index);
}
