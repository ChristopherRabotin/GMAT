//$Header$
//------------------------------------------------------------------------------
//                                  CoordinateSystem
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2004/12/22
//
/**
 * Implementation of the CoordinateSystem class.
 *
 * @note Code currently assumes that state vector passed in is an Rvector6.
 *       This will need updating soon.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateSystemException.hpp"
#include "CoordinateBase.hpp"
#include "Rmatrix33.hpp"


//---------------------------------
// static data
//---------------------------------

const std::string
CoordinateSystem::PARAMETER_TEXT[CoordinateSystemParamCount - CoordinateBaseParamCount] =
{
   "Axes",
   //"InternalState",
};

const Gmat::ParameterType
CoordinateSystem::PARAMETER_TYPE[CoordinateSystemParamCount - CoordinateBaseParamCount] =
{
   Gmat::OBJECT_TYPE,
   //Gmat::RVECTOR_TYPE,
};


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  CoordinateSystem(Gmat::ObjectType ofType, const std::string &itsType,
//                   const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs CoordinateSystem structures
 * (default constructor).
 *
 * @param ofType   Gmat::ObjectTypes enumeration for the object.
 * @param itsType  GMAT script string associated with this type of object.
 * @param itsName  Optional name for the object.  Defaults to "".
 *
 * @note There is no parameter free constructor for CoordinateSystem.  Derived 
 *       classes must pass in the ofType and itsType parameters.
 */
//---------------------------------------------------------------------------
CoordinateSystem::CoordinateSystem(const std::string &itsType,
                                   const std::string &itsName) :
CoordinateBase(Gmat::COORDINATE_SYSTEM,itsType,itsName),
axes               (NULL)
{
   objectTypes.push_back(Gmat::COORDINATE_SYSTEM);
   objectTypeNames.push_back("CoordinateSystem");
   parameterCount = CoordinateSystemParamCount;
}

//---------------------------------------------------------------------------
//  CoordinateSystem(const CoordinateSystem &coordSys);
//---------------------------------------------------------------------------
/**
 * Constructs base CoordinateSystem structures used in derived classes, 
 * by copying the input instance (copy constructor).
 *
 * @param sp  CoordinateSystem instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
CoordinateSystem::CoordinateSystem(const CoordinateSystem &coordSys) :
CoordinateBase (coordSys),
axes           (coordSys.axes)
{
}

//---------------------------------------------------------------------------
//  CoordinateSystem& operator=(const CoordinateSystem &coordSys)
//---------------------------------------------------------------------------
/**
* Assignment operator for CoordinateSystem structures.
 *
 * @param coordSys The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const CoordinateSystem& CoordinateSystem::operator=(
                        const CoordinateSystem &coordSys)
{
   if (&coordSys == this)
      return *this;
   CoordinateBase::operator=(coordSys);
   axes           = coordSys.axes;
   
   return *this;
}

//---------------------------------------------------------------------------
//  const bool operator==(const CoordinateSystem &coordSys)
//---------------------------------------------------------------------------
/**
 * Equality operator for CoordinateSystem structures.
 *
 * @param coordSys  The original that is being cjecked for equality with 
 *                   "this".
 *
 * @return true if "this" is the same as the input coordSys
 */
//---------------------------------------------------------------------------
const bool CoordinateSystem::operator==(const CoordinateSystem &coordSys)
{
   if (&coordSys == this)
      return true;
   if (axes->GetType() == (coordSys.axes)->GetType()) 
   {
      if (origin == coordSys.origin) return true;
      // need to check j2000Body too?
   }
   
   return false;
}

//---------------------------------------------------------------------------
//  ~CoordinateSystem(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
CoordinateSystem::~CoordinateSystem()
{
   if (axes) delete axes; 
}

GmatCoordinate::ParameterUsage CoordinateSystem::UsesEopFile() const
{
   if (axes) return axes->UsesEopFile();
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage CoordinateSystem::UsesItrfFile() const
{
   if (axes) return axes->UsesItrfFile();
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage CoordinateSystem::UsesEpoch() const
{
   if (axes) return axes->UsesEpoch();
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage CoordinateSystem::UsesPrimary() const
{
   if (axes) return axes->UsesPrimary();
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage CoordinateSystem::UsesSecondary() const
{
   if (axes) return axes->UsesSecondary();
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage CoordinateSystem::UsesXAxis() const
{
   if (axes) return axes->UsesXAxis();
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage CoordinateSystem::UsesYAxis() const
{
   if (axes) return axes->UsesYAxis();
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage CoordinateSystem::UsesZAxis() const
{
   if (axes) return axes->UsesZAxis();
   return GmatCoordinate::NOT_USED;
}

// methods to set parameters for the AxisSystems
void CoordinateSystem::SetPrimaryObject(SpacePoint *prim)
{
   if (axes) axes->SetPrimaryObject(prim);
}

void CoordinateSystem::SetSecondaryObject(SpacePoint *second)
{
   if (axes) axes->SetSecondaryObject(second);
}

void CoordinateSystem::SetEpoch(const A1Mjd &toEpoch)
{
   if (axes) axes->SetEpoch(toEpoch);
}

void CoordinateSystem::SetXAxis(const std::string &toValue)
{
   if (axes) axes->SetXAxis(toValue);
}

void CoordinateSystem::SetYAxis(const std::string &toValue)
{
   if (axes) axes->SetYAxis(toValue);
}

void CoordinateSystem::SetZAxis(const std::string &toValue)
{
   if (axes) axes->SetZAxis(toValue);
}

void CoordinateSystem::SetEopFile(EopFile *eopF)
{
   if (axes) axes->SetEopFile(eopF);
}

void CoordinateSystem::SetCoefficientsFile(ItrfCoefficientsFile *itrfF)
{
   if (axes) axes->SetCoefficientsFile(itrfF);
}

SpacePoint* CoordinateSystem::GetPrimaryObject() const
{
   if (axes) return axes->GetPrimaryObject();
   return NULL;
}

SpacePoint* CoordinateSystem::GetSecondaryObject() const
{
   if (axes) return axes->GetSecondaryObject();
   return NULL;
}

A1Mjd CoordinateSystem::GetEpoch() const
{
   if (axes) return axes->GetEpoch();
   return A1Mjd();  // does this make sense?
}

std::string CoordinateSystem::GetXAxis() const
{
   if (axes) return axes->GetXAxis();
   return "";
}

std::string CoordinateSystem::GetYAxis() const
{
   if (axes) return axes->GetYAxis();
   return "";
}

std::string CoordinateSystem::GetZAxis() const
{
   if (axes) return axes->GetZAxis();
   return "";
}

EopFile* CoordinateSystem::GetEopFile() const
{
   if (axes) return axes->GetEopFile();
   return NULL;
}

ItrfCoefficientsFile* CoordinateSystem::GetItrfCoefficientsFile()
{
   if (axes) return axes->GetItrfCoefficientsFile();
   return NULL;
}


//---------------------------------------------------------------------------
//  bool CoordinateSystem::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this CoordinateSystem.
 *
 */
//---------------------------------------------------------------------------
bool CoordinateSystem::Initialize()
{
   CoordinateBase::Initialize();
   if (axes)
   {
      axes->SetOriginName(originName);
      axes->SetJ2000BodyName(j2000BodyName);
      axes->SetRefObject(origin,Gmat::SPACE_POINT,originName);
      axes->SetRefObject(j2000Body,Gmat::SPACE_POINT,j2000BodyName);
      axes->SetSolarSystem(solar);
      axes->Initialize();
   }
   // wcs: as of 2004.12.22, assume it is MJ2000Eq, if undefined
   //if (!axes)
   //   throw CoordinateSystemException(
   //         "Axes are undefined for CoordinateSystem " + GetName());
   
   return true;
}


//------------------------------------------------------------------------------
//  Rvector  ToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
//                      bool coincident)
//------------------------------------------------------------------------------
/**
 * This method converts the input state, in 'this' axisSystem, to MJ2000Eq.
 *
 * @param epoch      epoch for which to do the conversion.
 * @param inState    input state to convert.
 * @param coincident are the systems coincident?.
 *
 * @return input state converted to MJ2000Eq.
 *
 */
//------------------------------------------------------------------------------
Rvector CoordinateSystem::ToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState, 
                                     bool coincident)
{
   Rvector internalState(inState.GetSize());
   if (axes)
   {
      if (!axes->RotateToMJ2000Eq(epoch,inState,internalState))
         throw CoordinateSystemException("Error rotating state to MJ2000Eq for "
                                         + instanceName);
   }
   else // assume this is MJ2000Eq, so no rotation is necessary
      internalState = inState;

   if (!coincident)
   {
      if (!TranslateToMJ2000Eq(epoch,internalState, internalState))
         throw CoordinateSystemException("Error translating state to MJ2000Eq for "
                                         + instanceName);
   }

   return internalState; // implicit copy constructor
}

//------------------------------------------------------------------------------
//  Rvector  FromMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
//                        bool coincident)
//------------------------------------------------------------------------------
/**
 * This method converts the input state, in MJ2000Eq axisSystem, to "this"
 * axisSystem.
 *
 * @param epoch      epoch for which to do the conversion.
 * @param inState    input state to convert.
 * @param coincident are the systems coincident?.
 *
 * @return input state converted from MJ2000Eq to 'this' axisSystem.
 *
 */
//------------------------------------------------------------------------------
Rvector CoordinateSystem::FromMJ2000Eq(const A1Mjd &epoch, const Rvector &inState, 
                                       bool coincident)
{
   Rvector internalState(inState.GetSize());
   if (!coincident)
   {
      if (!TranslateFromMJ2000Eq(epoch,inState, internalState))//,j2000Body))
         throw CoordinateSystemException("Error translating from MJ2000Eq for "
                                         + instanceName);
   }
   else
      internalState = inState;
      

   if (axes)
      if (!axes->RotateFromMJ2000Eq(epoch,internalState,internalState))//,j2000Body))
         throw CoordinateSystemException("Error rotating state from MJ2000Eq for "
                                         + instanceName);

   return internalState; // implicit copy constructor
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
* This method returns a clone of the Planet.
 *
 * @return clone of the Planet.
 *
 */
//------------------------------------------------------------------------------
GmatBase* CoordinateSystem::Clone() const
{
   return (new CoordinateSystem(*this));
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
std::string CoordinateSystem::GetParameterText(const Integer id) const
{
   if (id >= CoordinateBaseParamCount && id < CoordinateSystemParamCount)
      return PARAMETER_TEXT[id - CoordinateBaseParamCount];
   return CoordinateBase::GetParameterText(id);
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
Integer CoordinateSystem::GetParameterID(const std::string &str) const
{
   for (Integer i = CoordinateBaseParamCount; i < CoordinateSystemParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - CoordinateBaseParamCount])
         return i;
   }
   
   return CoordinateBase::GetParameterID(str);
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
Gmat::ParameterType CoordinateSystem::GetParameterType(const Integer id) const
{
   if (id >= CoordinateBaseParamCount && id < CoordinateSystemParamCount)
      return PARAMETER_TYPE[id - CoordinateBaseParamCount];
   
   return CoordinateBase::GetParameterType(id);
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
std::string CoordinateSystem::GetParameterTypeString(const Integer id) const
{
   return CoordinateBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The string stored for this parameter, or throw ab=n exception if
 *         there is no string association.
 */
//------------------------------------------------------------------------------
std::string CoordinateSystem::GetStringParameter(const Integer id) const
{
   if (id == AXES)
      if (axes)
         return axes->GetTypeName();
      else
         throw CoordinateSystemException("Axis system not set for "
                                         + instanceName);
      
   return CoordinateBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
//------------------------------------------------------------------------------
std::string CoordinateSystem::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const Gmat::ObjectType type,
//                         const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method returns a reference object from the CoordinateSystem class.
 *
 * @param type  type of the reference object requested
 * @param name  name of the reference object requested
 *
 * @return pointer to the reference object requested.
 *
 */
//------------------------------------------------------------------------------
GmatBase* CoordinateSystem::GetRefObject(const Gmat::ObjectType type,
                                         const std::string &name)
{
   switch (type)
   {
      case Gmat::AXIS_SYSTEM:
         return axes;
      default:
         break;
   }

   // Not handled here -- invoke the next higher GetRefObject call
   return CoordinateBase::GetRefObject(type, name);
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
const StringArray& CoordinateSystem::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   if (type == Gmat::UNKNOWN_OBJECT)
   {
      // Here we want the names of all named refence objects used in this
      // coordinate system.  (The axis system is not named, so we do not return
      // anything for it.)
      static StringArray refs = CoordinateBase::GetRefObjectNameArray(type);
      if (axes)
      {
         StringArray axisRefs = axes->GetRefObjectNameArray(type);
         for (StringArray::iterator i = axisRefs.begin();
              i != axisRefs.end(); ++i)
            if (find(refs.begin(), refs.end(), *i) == refs.end())
               refs.push_back(*i);
      }
      return refs;
   }

   // Not handled here -- invoke the next higher GetRefObject call
   return CoordinateBase::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
* This method sets a reference object for the CoordinateSystem class.
 *
 * @param obj   pointer to the reference object
 * @param type  type of the reference object 
 * @param name  name of the reference object
 *
 * @return true if successful; otherwise, false.
 *
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                    const std::string &name)
{ 
   bool retval = false;
   
   switch (type)
   {
      case Gmat::AXIS_SYSTEM:
      {
         axes = (AxisSystem*) obj;
         return true;
      }
      default:
         break;
   }
   
   // DJC added 5/9/05
   // Set ref object on owned axis system
   if (obj->IsOfType(Gmat::SPACE_POINT))
      retval = CoordinateBase::SetRefObject(obj, type, name);

   if (axes)
      retval |= axes->SetRefObject(obj, type, name);
      
   if (retval)
      return true;
   
   // Not handled here -- invoke the next higher SetRefObject call
   return CoordinateBase::SetRefObject(obj, type, name);
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  bool  TranslateToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
//                            Rvector &outState//,SpacePoint *j2000Body)
//------------------------------------------------------------------------------
/**
 * This method translates the input state, in "this" axisSystem to the MJ2000Eq
 * axisSystem.
 *
 * @param epoch      epoch for which to do the conversion.
 * @param inState    input state to convert.
 * @param outState   output (converted) state.
 * @param j2000Body  origin of the MJ2000Eq system. // currently set on the object
 *
 * @return true if successful; false if not.
 *
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::TranslateToMJ2000Eq(const A1Mjd &epoch,
                                           const Rvector &inState,
                                           Rvector &outState) //,
                                           //SpacePoint *j2000Body)
{
   if (origin == j2000Body)  
      outState = inState;
   else
   {
      // currently assuming an Rvector6 - update needed later?!?!   - awaiting Darrel on this
      // compute vector from origin of MJ2000Eq to origin of this system
      Rvector6 rif =  origin->GetMJ2000State(epoch) - 
                      (j2000Body->GetMJ2000State(epoch));
      outState = inState + rif;
   }
   return true;
}

//------------------------------------------------------------------------------
//  bool  TranslateFromMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
//                              Rvector &outState//,SpacePoint *j2000Body)
//------------------------------------------------------------------------------
/**
 * This method translates the input state, in MJ2000Eq axisSystem to 'this'
 * axisSystem.
 *
 * @param epoch      epoch for which to do the conversion.
 * @param inState    input state to convert.
 * @param outState   output (converted) state.
 * @param j2000Body  origin of the MJ2000Eq system. // currently set on the object
 *
 * @return true if successful; false if not.
 *
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::TranslateFromMJ2000Eq(const A1Mjd &epoch,
                                             const Rvector &inState,
                                             Rvector &outState) //,
                                             //SpacePoint *j2000Body)
{
   if (origin == j2000Body)  
      outState = inState;
   else
   {
      // currently assuming an Rvector6 - update needed later?!?!    - awaiting Darrel on this
      // compute vector from origin of of this system to origin of MJ2000Eq
      Rvector6 rif =  j2000Body->GetMJ2000State(epoch) - 
                      (origin->GetMJ2000State(epoch));
      outState = inState + rif;
   }
   return true;
}

