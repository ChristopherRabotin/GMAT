//$Id$
//------------------------------------------------------------------------------
//                                  ObjectReferencedAxes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2005/03/02
//
/**
 * Implementation of the ObjectReferencedAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ObjectReferencedAxes.hpp"
#include "DynamicAxes.hpp"
#include "CoordinateSystemException.hpp"

#include "MessageInterface.hpp"

//#define DEBUG_OR_AXES

#ifdef DEBUG_OR_AXES
#include <iostream>
using namespace std; // ***************************** for debug only

#define DEBUG_ROT_MATRIX 1
#define DEBUG_REFERENCE_SETTING
#define DEBUG_CS_INIT 1
static Integer visitCount = 0;
#endif

//---------------------------------
// static data
//---------------------------------

const std::string
ObjectReferencedAxes::PARAMETER_TEXT[ObjectReferencedAxesParamCount - DynamicAxesParamCount] =
{
   "XAxis",
   "YAxis",
   "ZAxis",
   "Primary",
   "Secondary",
};

const Gmat::ParameterType
ObjectReferencedAxes::PARAMETER_TYPE[ObjectReferencedAxesParamCount - DynamicAxesParamCount] =
{
   Gmat::ENUMERATION_TYPE,
   Gmat::ENUMERATION_TYPE,
   Gmat::ENUMERATION_TYPE,
   Gmat::OBJECT_TYPE,
   Gmat::OBJECT_TYPE,
};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  ObjectReferencedAxes(const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base ObjectReferencedAxes structures
 * (default constructor).
 *
 * @param itsName Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
ObjectReferencedAxes::ObjectReferencedAxes(const std::string &itsName) :
DynamicAxes("ObjectReferenced",itsName),
primaryName   (""),
secondaryName (""),
primary       (NULL),
secondary     (NULL),
xAxis         (""),
yAxis         (""),
zAxis         ("")
{
   objectTypeNames.push_back("ObjectReferencedAxes");
   parameterCount = ObjectReferencedAxesParamCount;
}

//---------------------------------------------------------------------------
//  ObjectReferencedAxes(const std::string &itsType,
//                       const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base ObjectReferencedAxes structures
 * (constructor).
 *
 * @param itsType Type for the object.
 * @param itsName Name for the object.
 *
 */
//---------------------------------------------------------------------------
ObjectReferencedAxes::ObjectReferencedAxes(const std::string &itsType,
                                           const std::string &itsName) :
DynamicAxes(itsType,itsName),
primaryName   (""),
secondaryName (""),
primary       (NULL),
secondary     (NULL),
xAxis         (""),
yAxis         (""),
zAxis         ("")
{
   objectTypeNames.push_back("ObjectReferencedAxes");
   parameterCount = ObjectReferencedAxesParamCount;
}

//---------------------------------------------------------------------------
//  ObjectReferencedAxes(const ObjectReferencedAxes &orAxes);
//---------------------------------------------------------------------------
/**
 * Constructs base ObjectReferencedAxes structures used in derived classes,
 * by copying the input instance (copy constructor).
 *
 * @param orAxes  ObjectReferencedAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
ObjectReferencedAxes::ObjectReferencedAxes(const ObjectReferencedAxes &orAxes) :
DynamicAxes(orAxes),
primaryName   (orAxes.primaryName),
secondaryName (orAxes.secondaryName),
primary       (orAxes.primary),
secondary     (orAxes.secondary),
xAxis         (orAxes.xAxis),
yAxis         (orAxes.yAxis),
zAxis         (orAxes.zAxis)
{
   parameterCount = ObjectReferencedAxesParamCount;
}

//---------------------------------------------------------------------------
//  ObjectReferencedAxes& operator=(const ObjectReferencedAxes &orAxes)
//---------------------------------------------------------------------------
/**
 * Assignment operator for ObjectReferencedAxes structures.
 *
 * @param orAxes The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const ObjectReferencedAxes& ObjectReferencedAxes::operator=(
                            const ObjectReferencedAxes &orAxes)
{
   if (&orAxes == this)
      return *this;
   DynamicAxes::operator=(orAxes);
   primaryName   = orAxes.primaryName;
   secondaryName = orAxes.secondaryName;
   primary       = orAxes.primary;
   secondary     = orAxes.secondary;
   xAxis         = orAxes.xAxis;
   yAxis         = orAxes.yAxis;
   zAxis         = orAxes.zAxis;
   return *this;
}
//---------------------------------------------------------------------------
//  ~ObjectReferencedAxes(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
ObjectReferencedAxes::~ObjectReferencedAxes()
{
}

GmatCoordinate::ParameterUsage ObjectReferencedAxes::UsesPrimary() const
{
   return GmatCoordinate::REQUIRED;
}

GmatCoordinate::ParameterUsage ObjectReferencedAxes::UsesSecondary() const
{
   return GmatCoordinate::OPTIONAL_USE;
}

GmatCoordinate::ParameterUsage ObjectReferencedAxes::UsesXAxis() const
{
   return GmatCoordinate::OPTIONAL_USE;  // or REQUIRED?????  (2 of 3 are required)
}

GmatCoordinate::ParameterUsage ObjectReferencedAxes::UsesYAxis() const
{
   return GmatCoordinate::OPTIONAL_USE;
}

GmatCoordinate::ParameterUsage ObjectReferencedAxes::UsesZAxis() const
{
   return GmatCoordinate::OPTIONAL_USE;
}


//------------------------------------------------------------------------------
//  void SetPrimaryObject(SpacePoint *prim)
//------------------------------------------------------------------------------
/**
 * Method to set the primary body for this system.
 *
 * @param prim  the object to use as the primary body.
 *
 */
//------------------------------------------------------------------------------
void ObjectReferencedAxes::SetPrimaryObject(SpacePoint *prim)
{
   primary     = prim;
   primaryName = primary->GetName();
}

//------------------------------------------------------------------------------
//  void SetSecondaryObject(SpacePoint *second)
//------------------------------------------------------------------------------
/**
 * Optional method to set the secondary body for this system.
 *
 * @param second  the object to use as the secondary body.
 *
 */
//------------------------------------------------------------------------------
void ObjectReferencedAxes::SetSecondaryObject(SpacePoint *second)
{
   secondary     = second;
   secondaryName = secondary->GetName();
}

//------------------------------------------------------------------------------
//  void SetXAxis(const std::string &toValue)
//------------------------------------------------------------------------------
/**
 * Method to set the X-Axis for this system.
 *
 * @param toValue  string value representing the X-axis; allowed values are:
 *                 "R", "V", "N", "-R", "-V", "-N" (see GMAT math specs).
 *
 */
//------------------------------------------------------------------------------
void ObjectReferencedAxes::SetXAxis(const std::string &toValue)
{
   if ((toValue !=  "R") && (toValue !=  "V") && (toValue !=  "N") &&
       (toValue != "-R") && (toValue != "-V") && (toValue != "-N") &&
       (toValue !=  "r") && (toValue !=  "v") && (toValue !=  "n") &&
       (toValue != "-r") && (toValue != "-v") && (toValue != "-n") && (toValue !=  "") )
      throw CoordinateSystemException(
            "ObjectReferencedAxes - Invalid value for X-Axis");
   xAxis = toValue;
}

//------------------------------------------------------------------------------
//  void SetYAxis(const std::string &toValue)
//------------------------------------------------------------------------------
/**
 * Method to set the Y-Axis for this system.
 *
 * @param toValue  string value representing the Y-axis; allowed values are:
 *                 "R", "V", "N", "-R", "-V", "-N" (see GMAT math specs).
 *
 */
//------------------------------------------------------------------------------
void ObjectReferencedAxes::SetYAxis(const std::string &toValue)
{
   if ((toValue !=  "R") && (toValue !=  "V") && (toValue !=  "N") &&
       (toValue != "-R") && (toValue != "-V") && (toValue != "-N") &&
       (toValue !=  "r") && (toValue !=  "v") && (toValue !=  "n") &&
       (toValue != "-r") && (toValue != "-v") && (toValue != "-n") && (toValue !=  "") )
      throw CoordinateSystemException(
            "ObjectReferencedAxes - Invalid value for Y-Axis");
   yAxis = toValue;
}

//------------------------------------------------------------------------------
//  void SetZAxis(const std::string &toValue)
//------------------------------------------------------------------------------
/**
 * Method to set the Z-Axis for this system.
 *
 * @param toValue  string value representing the Z-axis; allowed values are:
 *                 "R", "V", "N", "-R", "-V", "-N" (see GMAT math specs).
 *
 */
//------------------------------------------------------------------------------
void ObjectReferencedAxes::SetZAxis(const std::string &toValue)
{
   if ((toValue !=  "R") && (toValue !=  "V") && (toValue !=  "N") &&
       (toValue != "-R") && (toValue != "-V") && (toValue != "-N") &&
       (toValue !=  "r") && (toValue !=  "v") && (toValue !=  "n") &&
       (toValue != "-r") && (toValue != "-v") && (toValue != "-n") && (toValue !=  "") )
      throw CoordinateSystemException(
            "ObjectReferencedAxes - Invalid value for Z-Axis");
   zAxis = toValue;
}

SpacePoint* ObjectReferencedAxes::GetPrimaryObject() const
{
   return primary;
}

SpacePoint* ObjectReferencedAxes::GetSecondaryObject() const
{
   return secondary;
}

std::string ObjectReferencedAxes::GetXAxis() const
{
   return xAxis;
}

std::string ObjectReferencedAxes::GetYAxis() const
{
   return yAxis;
}

std::string ObjectReferencedAxes::GetZAxis() const
{
   return zAxis;
}

void ObjectReferencedAxes::ResetAxes()
{
   xAxis = "";
   yAxis = "";
   zAxis = "";
}


//---------------------------------------------------------------------------
//  bool ObjectReferencedAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this ObjectReferencedAxes.
 *
 */
//---------------------------------------------------------------------------
bool ObjectReferencedAxes::Initialize()
{
   DynamicAxes::Initialize();
   // use defaults, per Steve's email of 2005.05.13
   if (xAxis == "" && yAxis == "" && zAxis == "")
   {
      xAxis = "R";
      zAxis = "N";
      yAxis = "";
   }
   return true;
}

//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the ObjectReferencedAxes.
 *
 * @return clone of the ObjectReferencedAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ObjectReferencedAxes::Clone() const
{
   return (new ObjectReferencedAxes(*this));
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool ObjectReferencedAxes::RenameRefObject(const Gmat::ObjectType type,
                                           const std::string &oldName,
                                           const std::string &newName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("ObjectReferencedAxes::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type != Gmat::CALCULATED_POINT)
      return true;

   if (primaryName == oldName)
      primaryName = newName;
   
   if (secondaryName == oldName)
      secondaryName = newName;
   
   return true;
}


//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string ObjectReferencedAxes::GetParameterText(const Integer id) const
{
   if (id >= DynamicAxesParamCount && id < ObjectReferencedAxesParamCount)
      return PARAMETER_TEXT[id - DynamicAxesParamCount];
   return DynamicAxes::GetParameterText(id);
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
 *
 */
//------------------------------------------------------------------------------
Integer ObjectReferencedAxes::GetParameterID(const std::string &str) const
{
   for (Integer i = DynamicAxesParamCount; i < ObjectReferencedAxesParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - DynamicAxesParamCount])
         return i;
   }
   
   return DynamicAxes::GetParameterID(str);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ObjectReferencedAxes::GetParameterType(const Integer id) const
{
   if (id >= DynamicAxesParamCount && id < ObjectReferencedAxesParamCount)
      return PARAMETER_TYPE[id - DynamicAxesParamCount];
   
   return DynamicAxes::GetParameterType(id);
}

//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string ObjectReferencedAxes::GetParameterTypeString(const Integer id) const
{
   return DynamicAxes::PARAM_TYPE_STRING[GetParameterType(id)];
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
std::string ObjectReferencedAxes::GetStringParameter(const Integer id) const
{
   if (id == X_AXIS)                return xAxis;
   if (id == Y_AXIS)                return yAxis;
   if (id == Z_AXIS)                return zAxis;
   if (id == PRIMARY_OBJECT_NAME)   return primaryName;
   if (id == SECONDARY_OBJECT_NAME) return secondaryName;

   return DynamicAxes::GetStringParameter(id);
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

bool ObjectReferencedAxes::SetStringParameter(const Integer id,
                                              const std::string &value)
{
   bool OK = false;
   if (id == X_AXIS)
   {
      xAxis = value;
      OK = true;
   }
   if (id == Y_AXIS)
   {
      yAxis = value;
      OK = true;
   }
   if (id == Z_AXIS)
   {
      zAxis = value;
      OK = true;
   }
   if (id == PRIMARY_OBJECT_NAME)
   {
      primaryName = value;
      OK = true;
   }
   if (id == SECONDARY_OBJECT_NAME)
   {
      secondaryName = value;
      OK = true;
   }
   if (OK) return true;
   
   return DynamicAxes::SetStringParameter(id, value);
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
std::string ObjectReferencedAxes::GetStringParameter(
                                  const std::string &label) const
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

bool ObjectReferencedAxes::SetStringParameter(const std::string &label,
                                              const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const Gmat::ObjectType type,
//                         const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method returns a reference object from the ObjectReferencedAxes class.
 *
 * @param type  type of the reference object requested
 * @param name  name of the reference object requested
 *
 * @return pointer to the reference object requested.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ObjectReferencedAxes::GetRefObject(const Gmat::ObjectType type,
                                             const std::string &name)
{
   switch (type)
   {
      case Gmat::SPACE_POINT:
         if ((primary) && (name == primaryName))     return primary;
         if ((secondary) && (name == secondaryName)) return secondary;
      default:
         break;
   }

   // Not handled here -- invoke the next higher GetRefObject call
   return DynamicAxes::GetRefObject(type, name);
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
const StringArray& ObjectReferencedAxes::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACE_POINT)
   {
      static StringArray refs;
      refs.clear();
      refs = DynamicAxes::GetRefObjectNameArray(type);

      if (find(refs.begin(), refs.end(), primaryName) == refs.end())
         refs.push_back(primaryName);
      if (find(refs.begin(), refs.end(), secondaryName) == refs.end())
         refs.push_back(secondaryName);
      if (find(refs.begin(), refs.end(), originName) == refs.end())
         refs.push_back(originName);
      if (find(refs.begin(), refs.end(), j2000BodyName) == refs.end())
         refs.push_back(j2000BodyName);
         
      #ifdef DEBUG_REFERENCE_SETTING
         MessageInterface::ShowMessage("+++ReferenceObjects:\n");
         for (StringArray::iterator i = refs.begin(); i != refs.end(); ++i)
            MessageInterface::ShowMessage("   %s\n", i->c_str());
      #endif

      return refs;
   }

   // Not handled here -- invoke the next higher GetRefObject call
   return DynamicAxes::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object for the ObjectReferencedAxes class.
 *
 * @param obj   pointer to the reference object
 * @param type  type of the reference object
 * @param name  name of the reference object
 *
 * @return true if successful; otherwise, false.
 *
 */
//------------------------------------------------------------------------------
bool ObjectReferencedAxes::SetRefObject(GmatBase *obj,
                                        const Gmat::ObjectType type,
                                        const std::string &name)
{
   // DJC changed from case to IsOfType 5/13/05.  Definitely Friday the 13th!
//   switch (type)
//   {
//      case Gmat::SPACE_POINT:
//      {
      if (obj->IsOfType(Gmat::SPACE_POINT))
      {
         if (name == primaryName)
         {
            #ifdef DEBUG_REFERENCE_SETTING
               MessageInterface::ShowMessage("Setting %s as primary for %s\n",
                  name.c_str(), instanceName.c_str());
            #endif
            primary = (SpacePoint*) obj;
         }
         if (name == secondaryName)
         {
            #ifdef DEBUG_REFERENCE_SETTING
               MessageInterface::ShowMessage("Setting %s as secondary for %s\n",
                  name.c_str(), instanceName.c_str());
            #endif
            secondary = (SpacePoint*) obj;
         }
         // add in the CoordinateBase SpacePoints here too because if
         // primaryName or secondaryName == originName or j2000BodyName
         // (which is likely), the origin and/or j2000Body will never be set
         if (name == originName)
         {
            #ifdef DEBUG_REFERENCE_SETTING
               MessageInterface::ShowMessage("Setting %s as origin for %s\n",
                  name.c_str(), instanceName.c_str());
            #endif
            origin = (SpacePoint*) obj;
         }
         if (name == j2000BodyName)
         {
             #ifdef DEBUG_REFERENCE_SETTING
               MessageInterface::ShowMessage("Setting %s as J2000body for %s\n",
                  name.c_str(), instanceName.c_str());
            #endif
           j2000Body = (SpacePoint*) obj;
         }
         return true;
      }
//      default:
//         break;
//   }

   // Not handled here -- invoke the next higher SetRefObject call
   return DynamicAxes::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  void CalculateRotationMatrix(const A1Mjd &atEpoch,
//                               bool forceComputation = false)
//---------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the ObjectReferencedAxes system.
 *
 * @param atEpoch  epoch at which to compute the roration matrix
 */
//---------------------------------------------------------------------------
void ObjectReferencedAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                                   bool forceComputation)
{
   if (!primary)
      throw CoordinateSystemException("Primary " + primaryName +
         " is not set in object referenced!\n");

   if ((xAxis == yAxis) || (xAxis == zAxis) ||
       (yAxis == zAxis))
      throw CoordinateSystemException(
            "For object referenced axes, axes are improperly defined.");
   if ((xAxis != "") && (yAxis != "") && (zAxis != ""))
      throw CoordinateSystemException(
            "For object referenced axes, too many axes are defined.");
   SpacePoint *useAsSecondary = secondary;
   if (!useAsSecondary)  useAsSecondary = origin;
   Rvector6 rv     = useAsSecondary->GetMJ2000State(atEpoch) -
                     primary->GetMJ2000State(atEpoch);
#ifdef DEBUG_ROT_MATRIX
   if (visitCount == 0)
   {
      cout.precision(30);
      cout << " ----------------- rv Primary to Secondary    = " << rv << endl;
      visitCount++;
   }
#endif
#ifdef DEBUG_ROT_MATRIX
   if (visitCount == 0)
   {
      cout.precision(30);
      cout << " ----------------- rv Earth to Moon (truncated)    = " << rv << endl;
      visitCount++;
   }
#endif
   Rvector3 a     =  useAsSecondary->GetMJ2000Acceleration(atEpoch) - 
                     primary->GetMJ2000Acceleration(atEpoch);
   Rvector3 r      = rv.GetR();
   Rvector3 v      = rv.GetV();
   Rvector3 n     =  Cross(r,v);
   Rvector3 rUnit = r.GetUnitVector();
   Rvector3 vUnit = v.GetUnitVector();
   Rvector3 nUnit = n.GetUnitVector();
   Real     rMag  = r.GetMagnitude();
   Real     vMag  = v.GetMagnitude();
   Real     nMag  = n.GetMagnitude();
   Rvector3 rDot  = (v / rMag) - (rUnit / rMag) * (rUnit * v);
   Rvector3 vDot  = (a / vMag) - (vUnit / vMag) * (vUnit * a);
   Rvector3 nDot = (Cross(r,a) / nMag) - (nUnit / nMag) * (Cross(r,a) * nUnit);
   Rvector3 xUnit, yUnit, zUnit, xDot, yDot, zDot;
   bool     xUsed = true, yUsed = true, zUsed = true;
   // determine the x-axis
   if ((xAxis == "R") || (xAxis == "r"))
   {
      xUnit = rUnit;
      xDot  = rDot;
   }
   else if ((xAxis == "-R") || (xAxis == "-r"))
   {
      xUnit = -rUnit;
      xDot  = -rDot;
   }
   else if ((xAxis == "V") || (xAxis == "v"))
   {
      xUnit = vUnit;
      xDot  = vDot;
   }
   else if ((xAxis == "-V") || (xAxis == "-v"))
   {
      xUnit = -vUnit;
      xDot  = -vDot;
   }
   else if ((xAxis == "N") || (xAxis == "n"))
   {
      xUnit = nUnit;
      xDot  = nDot;
   }
   else if ((xAxis == "-N") || (xAxis == "-n"))
   {
      xUnit = -nUnit;
      xDot  = -nDot;
   }
   else
   {
      xUsed = false;
   }
   // determine the y-axis
   if ((yAxis == "R") || (yAxis == "r"))
   {
      yUnit = rUnit;
      yDot  = rDot;
   }
   else if ((yAxis == "-R") || (yAxis == "-r"))
   {
      yUnit = -rUnit;
      yDot  = -rDot;
   }
   else if ((yAxis == "V") || (yAxis == "v"))
   {
      yUnit = vUnit;
      yDot  = vDot;
   }
   else if ((yAxis == "-V") || (yAxis == "-v"))
   {
      yUnit = -vUnit;
      yDot  = -vDot;
   }
   else if ((yAxis == "N") || (yAxis == "n"))
   {
      yUnit = nUnit;
      yDot  = nDot;
   }
   else if ((yAxis == "-N") || (yAxis == "-n"))
   {
      yUnit = -nUnit;
      yDot  = -nDot;
   }
   else
   {
      yUsed = false;
   }
   // determine the z-axis
   if ((zAxis == "R") || (zAxis == "r"))
   {
      zUnit = rUnit;
      zDot  = rDot;
   }
   else if ((zAxis == "-R") || (zAxis == "-r"))
   {
      zUnit = -rUnit;
      zDot  = -rDot;
   }
   else if ((zAxis == "V") || (zAxis == "v"))
   {
      zUnit = vUnit;
      zDot  = vDot;
   }
   else if ((zAxis == "-V") || (zAxis == "-v"))
   {
      zUnit = -vUnit;
      zDot  = -vDot;
   }
   else if ((zAxis == "N") || (zAxis == "n"))
   {
      zUnit = nUnit;
      zDot  = nDot;
   }
   else if ((zAxis == "-N") || (zAxis == "-n"))
   {
      zUnit = -nUnit;
      zDot  = -nDot;
   }
   else
   {
      zUsed = false;
   }
   // determine the third axis
   if (xUsed && yUsed && !zUsed)
   {
      zUnit = Cross(xUnit, yUnit);
      zDot  = Cross(xDot, yUnit) + Cross(xUnit, yDot);
   }
   else if (xUsed && zUsed && !yUsed)
   {
      yUnit = Cross(zUnit,xUnit);
      yDot  = Cross(zDot, xUnit) + Cross(zUnit, xDot);
   }
   else if (yUsed && zUsed && !xUsed)
   {
      xUnit = Cross(yUnit,zUnit);
      xDot  = Cross(yDot, zUnit) + Cross(yUnit, zDot);
   }
   else
   {
      throw CoordinateSystemException(
            "Object referenced axes are improperly defined.");
   }
   // Compute the rotation matrix
   rotMatrix(0,0) = xUnit(0);
   rotMatrix(0,1) = yUnit(0);
   rotMatrix(0,2) = zUnit(0);
   rotMatrix(1,0) = xUnit(1);
   rotMatrix(1,1) = yUnit(1);
   rotMatrix(1,2) = zUnit(1);
   rotMatrix(2,0) = xUnit(2);
   rotMatrix(2,1) = yUnit(2);
   rotMatrix(2,2) = zUnit(2);
   
   // Compute the rotation derivative matrix
   rotDotMatrix(0,0) = xDot(0);
   rotDotMatrix(0,1) = yDot(0);
   rotDotMatrix(0,2) = zDot(0);
   rotDotMatrix(1,0) = xDot(1);
   rotDotMatrix(1,1) = yDot(1);
   rotDotMatrix(1,2) = zDot(1);
   rotDotMatrix(2,0) = xDot(2);
   rotDotMatrix(2,1) = yDot(2);
   rotDotMatrix(2,2) = zDot(2);

   //loj: 12/29/05 just debug
//    MessageInterface::ShowMessage
//       ("===> rotMatrix=%s\n", rotMatrix.ToString().c_str());
   
#ifdef DEBUG_ROT_MATRIX
   cout.setf(ios::fixed);
   cout.precision(30);
   cout << " ----------------- rotMatrix    = " << rotMatrix << endl;
   cout.setf(ios::scientific);
   cout << " ----------------- rotDotMatrix = " << rotDotMatrix << endl;
#endif

   // Check for orthogonality - is this correct?
   // orthonormal instead? accuracy (tolerance)? 
   if (!rotMatrix.IsOrthogonal(1.0e-15))
      throw CoordinateSystemException(
  "Object referenced axes definition does not result in an orthogonal system.");
}
