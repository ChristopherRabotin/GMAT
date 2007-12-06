//$Id$
//------------------------------------------------------------------------------
//                                  CoordinateBase
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
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
#include "MessageInterface.hpp"

//---------------------------------
// static data
//---------------------------------

const std::string
CoordinateBase::PARAMETER_TEXT[CoordinateBaseParamCount - GmatBaseParamCount] =
{
   "Origin",
   "J2000Body",
};

const Gmat::ParameterType
CoordinateBase::PARAMETER_TYPE[CoordinateBaseParamCount - GmatBaseParamCount] =
{
   Gmat::STRING_TYPE,
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
 * @param ofType   Gmat::ObjectTypes enumeration for the object.
 * @param itsType  GMAT script string associated with this type of object.
 * @param itsName  Optional name for the object.  Defaults to "".
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
j2000Body      (NULL),
j2000BodyName  ("Earth"),
solar          (NULL)
{
   objectTypeNames.push_back("CoordinateBase");
   parameterCount = CoordinateBaseParamCount;
}

//---------------------------------------------------------------------------
//  CoordinateBase(const CoordinateBase &coordBase);
//---------------------------------------------------------------------------
/**
 * Constructs base CoordinateBase structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param coordBase   CoordinateBase instance to copy to create "this" 
 *                    instance.
 */
//---------------------------------------------------------------------------
CoordinateBase::CoordinateBase(const CoordinateBase &coordBase) :
GmatBase(coordBase),
origin        (NULL),
originName    (coordBase.originName),
j2000Body     (NULL),
j2000BodyName (coordBase.j2000BodyName),
solar         (NULL)
{
}

//---------------------------------------------------------------------------
//  CoordinateBase& operator=(const CoordinateBase &coordBase)
//---------------------------------------------------------------------------
/**
 * Assignment operator for CoordinateBase structures.
 *
 * @param coordBase  The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const CoordinateBase& CoordinateBase::operator=(const CoordinateBase &coordBase)
{
   if (&coordBase == this)
      return *this;
   GmatBase::operator=(coordBase);
   origin        = NULL;
   originName    = coordBase.originName;
   j2000Body     = NULL;
   j2000BodyName = coordBase.j2000BodyName;
   solar         = NULL;
   
   return *this;
}

//---------------------------------------------------------------------------
//  ~CoordinateBase()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
CoordinateBase::~CoordinateBase()
{
}

//------------------------------------------------------------------------------
//  void  SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * This method sets the Solar System for the CoordinateBase class.
 *
 */
//------------------------------------------------------------------------------
void CoordinateBase::SetSolarSystem(SolarSystem *ss)
{
   if (ss == NULL)
      return;
   
   #ifdef DEBUG_SET_SS
   MessageInterface::ShowMessage
      ("CoordinateBase::SetSolarSystem() this=%s, solar=%p, ss=%p\n",
       GetName().c_str(), solar, ss);
   #endif
   
   if (solar != ss)
   {
      solar = ss;
      
      // set new origin 
      SpacePoint *sp = solar->GetBody(originName);
      if (sp != NULL)
         origin = sp;
      
      // set new j2000body
      sp = solar->GetBody(j2000BodyName);
      if (sp != NULL)
         j2000Body = sp;
      
      // set J2000Body of origin
      if (origin != NULL)
         origin->SetJ2000Body(solar->GetBody(origin->GetJ2000BodyName()));
      
      #ifdef DEBUG_SET_SS
      MessageInterface::ShowMessage
         ("   got new SolarSystem, origin=%p, j2000Body=%p\n", origin, j2000Body);
      #endif
   }
}

//------------------------------------------------------------------------------
//  void  SetOriginName(const std::string &toName)
//------------------------------------------------------------------------------
/**
 * This method sets the origin name for the CoordinateBase class.
 *
 */
//------------------------------------------------------------------------------
void CoordinateBase::SetOriginName(const std::string &toName)
{
   originName = toName;
}

//------------------------------------------------------------------------------
//  void  SetOrigin(SpacePoint *originPtr)
//------------------------------------------------------------------------------
/**
 * This method sets the origin for the CoordinateBase class.
 *
 */
//------------------------------------------------------------------------------
void CoordinateBase::SetOrigin(SpacePoint *originPtr)
{
   origin = originPtr;
}

//------------------------------------------------------------------------------
//  bool RequiresJ2000Body()
//------------------------------------------------------------------------------
/**
 * Identifies objects that need to have the J2000 body set in the Sandbox.
 *
 * @return true for all CoordinateBase objects.
 */
//------------------------------------------------------------------------------
bool CoordinateBase::RequiresJ2000Body()
{
   return true;
}

//------------------------------------------------------------------------------
//  void  SetJ2000BodyName(const std::string &toName)
//------------------------------------------------------------------------------
/**
 * This method sets the j2000Body for the CoordinateBase class.
 *
 */
//------------------------------------------------------------------------------
void CoordinateBase::SetJ2000BodyName(const std::string &toName)
{
   j2000BodyName = toName;
}

//------------------------------------------------------------------------------
//  void  SetJ2000Body(SpacePoint *j2000Ptr)
//------------------------------------------------------------------------------
/**
 * This method sets the j2000Body for the CoordinateBase class.
 *
 */
//------------------------------------------------------------------------------
void CoordinateBase::SetJ2000Body(SpacePoint *j2000Ptr)
{
   j2000Body = j2000Ptr;
}

//------------------------------------------------------------------------------
//  std::string  GetOriginName() const
//------------------------------------------------------------------------------
/**
 * This method returns the origin name for the CoordinateBase class.
 *
 * @return name of the origin for the CoordinateBase object
 */
//------------------------------------------------------------------------------
std::string CoordinateBase::GetOriginName() const
{
   if (origin) return origin->GetName();
   else        return originName;
}

//------------------------------------------------------------------------------
//  SpacePoint*  GetOrigin() const
//------------------------------------------------------------------------------
/**
* This method returns the origin pointer for the CoordinateBase class.
 *
 * @return pointer to the origin for the CoordinateBase object
 */
//------------------------------------------------------------------------------
SpacePoint* CoordinateBase::GetOrigin() const
{
   return origin;
}

//------------------------------------------------------------------------------
//  std::string  GetJ2000BodyName() const
//------------------------------------------------------------------------------
/**
 * This method returns the origin name for the CoordinateBase class.
 *
 * @return name of the origin for the CoordinateBase object
 */
//------------------------------------------------------------------------------
std::string CoordinateBase::GetJ2000BodyName() const
{
   if (j2000Body) return j2000Body->GetName();
   else           return j2000BodyName;
}

//------------------------------------------------------------------------------
//  SpacePoint*  GetJ2000Body() const
//------------------------------------------------------------------------------
/**
 * This method returns the j2000Body pointer for the CoordinateBase class.
 *
 * @return pointer to the j2000Body for the CoordinateBase object
 */
//------------------------------------------------------------------------------
SpacePoint* CoordinateBase::GetJ2000Body() const
{
   return j2000Body;
}


//------------------------------------------------------------------------------
//  bool  Initialize()
//------------------------------------------------------------------------------
/**
 * This method initializes the CoordinateBase class.
 *
 */
//------------------------------------------------------------------------------
bool CoordinateBase::Initialize()
{
   if (!origin)
      throw CoordinateSystemException(
            "Origin has not been defined for CoordinateBase object " +
            instanceName);
   if (!j2000Body) // ????????????????????????????????
      throw CoordinateSystemException(
            "j2000Body has not been defined for CoordinateBase object " +
            instanceName);
   if (!solar)
      throw CoordinateSystemException(
            "Solar System has not been defined for CoordinateBase object " +
            instanceName);
   return true;
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
 * @param id  Id for the requested parameter text.
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
 * @param str  string for the requested parameter.
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
 * @param id  ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string CoordinateBase::GetParameterTypeString(const Integer id) const
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
bool CoordinateBase::IsParameterReadOnly(const Integer id) const
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
bool CoordinateBase::IsParameterReadOnly(const std::string &label) const
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
 * @param id  ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string CoordinateBase::GetStringParameter(const Integer id) const
{
   if (id == ORIGIN_NAME)   
      return originName;
   else if (id == J2000_BODY_NAME)   
      return j2000BodyName;
   
   return GmatBase::GetStringParameter(id);
}

//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param id     ID for the requested parameter.
 * @param value  string value for the requested parameter.
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
       originName    = value; 
       return true;
    }
    else if (id == J2000_BODY_NAME) 
    {
       // Disabled for now -- just post a warning message if the user tries to 
       // set the J2000 body.
//       MessageInterface::ShowMessage(
//          "Warning: Attempting to set the read-only parameter 'J2000Body' "
//          "on the coordinate system '%s'\n", instanceName.c_str());
//       // j2000BodyName = value; 
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
 * @param    label  label ID for the parameter
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
 * @param    label  Integer ID for the parameter
 * @param    value  The new value for the parameter
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
 * @param type  type of the reference object requested
 * @param name  name of the reference object requested
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
         if (name == originName)    return origin;
         if (name == j2000BodyName) return j2000Body;
      default:
         break;
   }
   
   // Not handled here -- invoke the next higher GetRefObject call
   return GmatBase::GetRefObject(type, name);
}

// DJC added 5/9/05 to facilitate Sandbox initialization
//------------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Returns the names of the reference object. (Derived classes should implement
 * this as needed.)
 *
 * @param <type> reference object type.  Gmat::UnknownObject returns all of the
 *               ref objects.
 *
 * @return The names of the reference object.
 */
//------------------------------------------------------------------------------
const StringArray& CoordinateBase::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACE_POINT)
   {
      static StringArray refs;
      refs.clear();
      
      refs.push_back(originName);
      if (j2000BodyName != originName)
         refs.push_back(j2000BodyName);
      
      //for (unsigned int i=0; i<refs.size(); i++)
      //   MessageInterface::ShowMessage("======> cb obj[%d]=%s\n", i, refs[i].c_str());
      
      return refs;
   }
   
   // Not handled here -- invoke the next higher GetRefObject call
   return GmatBase::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object for the CoordinateBase class.
 *
 * @param obj   pointer to the reference object
 * @param type  type of the reference object 
 * @param name  name of the reference object
 *
 * @return true if successful; otherwise, false.
 *
 */
//------------------------------------------------------------------------------
bool CoordinateBase::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                  const std::string &name)
{
   if (obj == NULL)
      return false;

   #ifdef DEBUG_SET_REF
   MessageInterface::ShowMessage
      ("CoordinateBase::SetRefObject() <%s>, obj=%p, name=%s\n", GetName().c_str(),
       obj, name.c_str());
   #endif
   
   if (obj->IsOfType(Gmat::SPACE_POINT))
   {
      SpacePoint *sp = (SpacePoint*) obj;
      
      if (name == originName)
         origin = sp;
      
      if (name == j2000BodyName)
         j2000Body = sp;      
      
      if (origin != NULL)
         if (name == origin->GetJ2000BodyName())
            origin->SetJ2000Body(sp);
      
      return true;
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

