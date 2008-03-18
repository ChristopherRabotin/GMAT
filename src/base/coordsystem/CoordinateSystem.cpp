//$Id$
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
#include "MessageInterface.hpp"

//#define DEBUG_RENAME 1
//#define DEBUG_INPUTS_OUTPUTS
//#define DEBUG_DESTRUCTION
//#define DEBUG_CONSTRUCTION
//#define DEBUG_CS_INIT 1

//---------------------------------
// static data
//---------------------------------

const std::string
CoordinateSystem::PARAMETER_TEXT[CoordinateSystemParamCount - CoordinateBaseParamCount] =
{
   "Axes",
   "UpdateInterval",
   "OverrideOriginInterval",
   //"InternalState",
};

const Gmat::ParameterType
CoordinateSystem::PARAMETER_TYPE[CoordinateSystemParamCount - CoordinateBaseParamCount] =
{
   Gmat::OBJECT_TYPE,
   Gmat::REAL_TYPE,
   Gmat::BOOLEAN_TYPE,
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
   #ifdef DEBUG_CONSTRUCTION
      MessageInterface::ShowMessage("Now constructing CS %s of type %s\n",
         itsName.c_str(), itsType.c_str());
   #endif
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
CoordinateBase (coordSys)
//axes           (coordSys.axes)
{
   if (coordSys.axes)
      axes = (AxisSystem*)coordSys.axes->Clone();
   else
      axes = NULL;
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
   //axes           = coordSys.axes;
   
   if (coordSys.axes)
      axes = (AxisSystem*)coordSys.axes->Clone();
   else
      axes = NULL;
   
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
   #ifdef DEBUG_DESTRUCTION
   MessageInterface::ShowMessage("---> Entering CoordinateSystem destructor for %s\n",
   instanceName.c_str());
   #endif
   
   if (axes) delete axes;
   axes = NULL;
   #ifdef DEBUG_DESTRUCTION
   MessageInterface::ShowMessage("---> LEAVING CoordinateSystem destructor for %s\n",
   instanceName.c_str());
   #endif
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

GmatCoordinate::ParameterUsage CoordinateSystem::UsesNutationUpdateInterval() const
{
   if (axes) return axes->UsesNutationUpdateInterval();
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

Rmatrix33 CoordinateSystem::GetLastRotationMatrix() const
{
   if (!axes) 
      throw CoordinateSystemException("No AxisSystem defined for " 
            + instanceName);
   return axes->GetLastRotationMatrix();
}

void CoordinateSystem::GetLastRotationMatrix(Real *mat) const
{
   if (!axes) 
      throw CoordinateSystemException("No AxisSystem defined for " 
            + instanceName);
   axes->GetLastRotationMatrix(mat);
}

Rmatrix33 CoordinateSystem::GetLastRotationDotMatrix() const
{
   if (!axes) 
      throw CoordinateSystemException("No AxisSystem defined for " 
            + instanceName);
   return axes->GetLastRotationDotMatrix();
}

void CoordinateSystem::GetLastRotationDotMatrix(Real *mat) const
{
   if (!axes) 
      throw CoordinateSystemException("No AxisSystem defined for " 
            + instanceName);
   axes->GetLastRotationDotMatrix(mat);
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
   #if DEBUG_CS_INIT
   MessageInterface::ShowMessage
      ("CoordinateSystem::Initialize() csName=%s, axes=%p\n",
       GetName().c_str(), axes);
   #endif

   CoordinateBase::Initialize();
   
   if (axes)
   {
      #if DEBUG_CS_INIT
      MessageInterface::ShowMessage
         ("   originName=%s, origin=%s, j2000BodyName=%s, j2000Body=%s\n",
          originName.c_str(), origin->GetName().c_str(),
          j2000BodyName.c_str(), j2000Body->GetName().c_str());
      #endif
      
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
//                      bool coincident, bool forceComputation)
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
                                     bool coincident, 
                                     bool forceComputation)
{
   static Rvector internalState;
   static Rvector finalState;
    #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::ToMJ2000Eq, inState = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      inState[0], inState[1], inState[2], inState[3],inState[4], inState[5]);
    #endif
   internalState.SetSize(inState.GetSize());
   finalState.SetSize(inState.GetSize());
   
   if (axes)
   {
      if (!axes->RotateToMJ2000Eq(epoch,inState,internalState, forceComputation))
         throw CoordinateSystemException("Error rotating state to MJ2000Eq for "
                                         + instanceName);
   }
   else // assume this is MJ2000Eq, so no rotation is necessary
      internalState = inState;

   if (!coincident)
   {
      //if (!TranslateToMJ2000Eq(epoch,internalState, internalState))
      //   throw CoordinateSystemException("Error translating state to MJ2000Eq for "
      //                                   + instanceName);
      if (!TranslateToMJ2000Eq(epoch,internalState, finalState))
         throw CoordinateSystemException("Error translating state to MJ2000Eq for "
                                         + instanceName);
    #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::ToMJ2000Eq, translation happening\n");
    #endif
   }
   else
      finalState = internalState;

   #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::ToMJ2000Eq, internalState = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      internalState[0], internalState[1], internalState[2], 
      internalState[3],internalState[4], internalState[5]);
      MessageInterface::ShowMessage(
      "In CS::ToMJ2000Eq, finalState = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      finalState[0], finalState[1], finalState[2], 
      finalState[3],finalState[4], finalState[5]);
   #endif
   return finalState; // implicit copy constructor
   //return internalState; // implicit copy constructor
}

void CoordinateSystem::ToMJ2000Eq(const A1Mjd &epoch, const Real *inState, 
                                  Real *outState,     bool coincident,
                                  bool forceComputation)
{
   Real internalState[6];
   if (axes)
   {
      if (!axes->RotateToMJ2000Eq(epoch,inState,internalState, forceComputation))
         throw CoordinateSystemException("Error rotating state to MJ2000Eq for "
                                         + instanceName);
   }
   else // assume this is MJ2000Eq, so no rotation is necessary
      for (Integer i=0; i<6;i++) internalState[i] = inState[i];

   if (!coincident)
   {
      //if (!TranslateToMJ2000Eq(epoch,internalState, internalState))
      //   throw CoordinateSystemException("Error translating state to MJ2000Eq for "
      //                                   + instanceName);
      if (!TranslateToMJ2000Eq(epoch,internalState, outState))
         throw CoordinateSystemException("Error translating state to MJ2000Eq for "
                                         + instanceName);
    #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::ToMJ2000Eq, translation happening\n");
    #endif
   }
   else
      for (Integer i=0; i<6; i++)  outState[i] = internalState[i];
}

//------------------------------------------------------------------------------
//  Rvector  FromMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
//                        bool coincident, bool forceComputation)
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
                                       bool coincident,
                                       bool forceComputation)
{
   static Rvector internalState;
   static Rvector finalState;
    #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::FromMJ2000Eq, inState = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      inState[0], inState[1], inState[2], inState[3],inState[4], inState[5]);
    #endif
   internalState.SetSize(inState.GetSize());
   finalState.SetSize(inState.GetSize());
   if (!coincident)
   {
      if (!TranslateFromMJ2000Eq(epoch,inState, internalState))//,j2000Body))
         throw CoordinateSystemException("Error translating from MJ2000Eq for "
                                         + instanceName);
    #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::FromMJ2000Eq, translation happening\n");
    #endif
   }
   else
      internalState = inState;
      

   if (axes)
   {
      //if (!axes->RotateFromMJ2000Eq(epoch,internalState,internalState))//,j2000Body))
      //   throw CoordinateSystemException("Error rotating state from MJ2000Eq for "
      //                                   + instanceName);
      if (!axes->RotateFromMJ2000Eq(epoch,internalState,finalState,
          forceComputation))//,j2000Body))
         throw CoordinateSystemException("Error rotating state from MJ2000Eq for "
                                         + instanceName);
   }
   else
      finalState    = internalState;
                                         

   #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::FromMJ2000Eq, internalState = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      internalState[0], internalState[1], internalState[2], 
      internalState[3],internalState[4], internalState[5]);
      MessageInterface::ShowMessage(
      "In CS::FromMJ2000Eq, finalState = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      finalState[0], finalState[1], finalState[2], 
      finalState[3], finalState[4], finalState[5]);
   #endif
   return finalState; // implicit copy constructor
   //return internalState; // implicit copy constructor
}


void CoordinateSystem::FromMJ2000Eq(const A1Mjd &epoch, const Real *inState, 
                                    Real *outState,  bool coincident,
                                    bool forceComputation)
{
   Real internalState[6];
   if (!coincident)
   {
      if (!TranslateFromMJ2000Eq(epoch,inState, internalState))//,j2000Body))
         throw CoordinateSystemException("Error translating from MJ2000Eq for "
                                         + instanceName);
    #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::FromMJ2000Eq, translation happening\n");
    #endif
   }
   else
      for (Integer i=0; i<6; i++) internalState[i] = inState[i];
      

   if (axes)
   {
      //if (!axes->RotateFromMJ2000Eq(epoch,internalState,internalState))//,j2000Body))
      //   throw CoordinateSystemException("Error rotating state from MJ2000Eq for "
      //                                   + instanceName);
      if (!axes->RotateFromMJ2000Eq(epoch,internalState,outState, 
          forceComputation))//,j2000Body))
         throw CoordinateSystemException("Error rotating state from MJ2000Eq for "
                                         + instanceName);
   }
   else
      for (Integer i=0; i<6; i++) outState[i]    = internalState[i];
                                         
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


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void CoordinateSystem::Copy(const GmatBase* orig)
{
   operator=(*((CoordinateSystem *)(orig)));
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool CoordinateSystem::RenameRefObject(const Gmat::ObjectType type,
                                       const std::string &oldName,
                                       const std::string &newName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("CoordinateSystem::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type == Gmat::CALCULATED_POINT || type == Gmat::SPACECRAFT)
      if (originName == oldName)
         originName = newName;
   
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
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the real value, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real CoordinateSystem::GetRealParameter(const Integer id) const
{
   if (id == UPDATE_INTERVAL) 
   {
      if (axes) return axes->GetRealParameter("UpdateInterval");
   }
   return CoordinateBase::GetRealParameter(id);
}

//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value) 
//------------------------------------------------------------------------------
/**
 * This method sets the real value, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 * @param value to use to set the parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real CoordinateSystem::SetRealParameter(const Integer id, const Real value)
{
   if (id == UPDATE_INTERVAL) 
   {
      if (axes) return axes->SetRealParameter("UpdateInterval", value);
   }
   return CoordinateBase::SetRealParameter(id,value);
}

//------------------------------------------------------------------------------
//  Real  GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the real value, given the input parameter label.
 *
 * @param label label for the requested parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real CoordinateSystem::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  Real  SetRealParameter(const std::string &label, const Real value) 
//------------------------------------------------------------------------------
/**
 * This method sets the real value, given the input parameter label.
 *
 * @param label label for the requested parameter.
 * @param value to use to set the parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real CoordinateSystem::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
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

bool CoordinateSystem::GetBooleanParameter(const Integer id) const
{
   if (id == OVERRIDE_ORIGIN_INTERVAL) 
   {
      if (axes) return axes->GetBooleanParameter("OverrideOriginInterval");
      else      return false;  // or throw an exception here?
   }
   return CoordinateBase::GetBooleanParameter(id); 
}

bool CoordinateSystem::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}

bool CoordinateSystem::SetBooleanParameter(const Integer id,
                                     const bool value)
{
   if (id == OVERRIDE_ORIGIN_INTERVAL)
   {
      if (axes)
         return axes->SetBooleanParameter("OverrideOriginInterval", value);
      else  
         return false;  // or throw an exception here?
   }
   return CoordinateBase::SetBooleanParameter(id, value);
}

bool CoordinateSystem::SetBooleanParameter(const std::string &label,
                                     const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
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


//---------------------------------------------------------------------------
//  GmatBase* GetOwnedObject(Integer whichOne)
//---------------------------------------------------------------------------
/**
 * Access GmatBase objects belonging to this instance.
 *
 * This method is used to access the axis system when a script is written to a
 * file.
 *
 * @param <whichOne> The index for this owned object.
 *
 * @return Pointer to the owned object.
 */
//---------------------------------------------------------------------------
GmatBase* CoordinateSystem::GetOwnedObject(Integer whichOne)
{
   if (whichOne == 0)
      return axes;

   return CoordinateBase::GetOwnedObject(whichOne);
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
const ObjectTypeArray& CoordinateSystem::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::SPACE_POINT);
   return refObjectTypes;
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
   #if DEBUG_CS_REF_OBJECT
   MessageInterface::ShowMessage
      ("CoordinateSystem::GetRefObjectNameArray() type=%d(%s)\n", type,
       GetObjectTypeString(type).c_str());
   #endif
   
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACE_POINT)
   {
      // Here we want the names of all named refence objects used in this
      // coordinate system.  (The axis system is not named, so we do not return
      // anything for it.)
      static StringArray refs;
      refs.clear();
      
      refs = CoordinateBase::GetRefObjectNameArray(type);
      
      #if DEBUG_CS_REF_OBJECT
      for (unsigned int i=0; i<refs.size(); i++)
         MessageInterface::ShowMessage("   cs obj=%s\n", refs[i].c_str());
      #endif
      
      if (axes)
      {
         static StringArray axisRefs;
         axisRefs.clear();
         
         axisRefs = axes->GetRefObjectNameArray(type);
         
         #if DEBUG_CS_REF_OBJECT
         for (unsigned int i=0; i<axisRefs.size(); i++)
            MessageInterface::ShowMessage("   axisRef=%s\n", axisRefs[i].c_str());
         #endif
         
         for (StringArray::iterator i = axisRefs.begin(); i != axisRefs.end(); ++i)
            if (find(refs.begin(), refs.end(), *i) == refs.end())
               if( (*i) != "")
               {
                  #if DEBUG_CS_REF_OBJECT
                  MessageInterface::ShowMessage("      axis obj=%s\n", (*i).c_str());
                  #endif
                  refs.push_back(*i);
               }
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
   if (obj == NULL)
      return false;
   
   bool retval = false;
   
   switch (type)
   {
      case Gmat::AXIS_SYSTEM:
      {
//          MessageInterface::ShowMessage
//             ("===> CoordinateSystem::SetRefObject() %s, before axes=%p, obj=%p\n",
//              GetName().c_str(), axes, obj);

         AxisSystem *oldAxis = axes;
         
         axes = (AxisSystem*) obj->Clone();
         axes->SetName("");
         ownedObjectCount = 1;
         
//          MessageInterface::ShowMessage
//             ("===> CoordinateSystem::SetRefObject() %s, after axes=%p\n",
//              GetName().c_str(), axes);

         if (oldAxis)
            delete oldAxis;
         
         return true;
      }
      default:
         break;
   }
   
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

bool CoordinateSystem::TranslateToMJ2000Eq(const A1Mjd &epoch,
                                           const Real *inState,
                                           Real *outState)
{
   if (origin == j2000Body)  
      for (Integer i=0; i<6; i++) outState[i] = inState[i];
   else
   {
      Rvector6 rif =  origin->GetMJ2000State(epoch) - 
                      (j2000Body->GetMJ2000State(epoch));
      const Real *toRif = rif.GetDataVector();
      for (Integer i=0; i<6; i++)  outState[i] = inState[i] + toRif[i];
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

bool CoordinateSystem::TranslateFromMJ2000Eq(const A1Mjd &epoch,
                                             const Real *inState,
                                             Real *outState) //,
                                             //SpacePoint *j2000Body)
{
   if (origin == j2000Body)  
      for (Integer i=0; i<6; i++) outState[i] = inState[i];
   else
   {
      Rvector6 rif =  j2000Body->GetMJ2000State(epoch) - 
                      (origin->GetMJ2000State(epoch));
      const Real *toRif = rif.GetDataVector();
      for (Integer i=0; i<6; i++) outState[i] = inState[i] + toRif[i];
   }
   return true;
}

