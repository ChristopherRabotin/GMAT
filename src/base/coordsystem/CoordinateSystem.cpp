//$Id$
//------------------------------------------------------------------------------
//                                  CoordinateSystem
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
#include "ObjectReferencedAxes.hpp"
#include "MJ2000EqAxes.hpp"
#include "TopocentricAxes.hpp"
#include "BodyFixedAxes.hpp"
#include "Rmatrix33.hpp"
#include "GmatGlobal.hpp"               // for GetEopFile(), GetItrfCoefficientsFile()
#include "MessageInterface.hpp"

#include <algorithm>                    // Required by GCC 4.3

//#define DEBUG_RENAME 1
//#define DEBUG_INPUTS_OUTPUTS
//#define DEBUG_DESTRUCTION
//#define DEBUG_CONSTRUCTION
//#define DEBUG_CS_INIT 1
//#define DEBUG_CS_SET_REF
//#define DEBUG_TRANSLATION
//#define DEBUG_CS_CREATE

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

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
   "Epoch",
};

const Gmat::ParameterType
CoordinateSystem::PARAMETER_TYPE[CoordinateSystemParamCount - CoordinateBaseParamCount] =
{
   Gmat::OBJECT_TYPE,     // "Axes",
   Gmat::REAL_TYPE,       // "UpdateInterval",
   Gmat::BOOLEAN_TYPE,    // "OverrideOriginInterval",
   //Gmat::RVECTOR_TYPE,    // "InternalState",
   Gmat::REAL_TYPE,       // "Epoch",
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
   {
      axes = (AxisSystem*)coordSys.axes->Clone();
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (axes, "clonedAxes", "CoordinateSystem Copy Constructor",
          "(AxisSystem*) obj->Clone()");
      #endif
   }
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
   
   if (axes)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (axes, axes->GetTypeName(), "CoordinateSystem::operator=",
          "deleting axes");
      #endif
      delete axes;
   }
   
   if (coordSys.axes)
   {
      axes = (AxisSystem*)coordSys.axes->Clone();
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (axes, "clonedAxes", "CoordinateSystem Assignment operator",
          "(AxisSystem*) obj->Clone()");
      #endif
   }
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
   
   if (axes)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (axes, axes->GetTypeName(), "CoordinateSystem::~CoordinateSystem()",
          "deleting axes");
      #endif
      delete axes;
   }
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

bool CoordinateSystem::AreAxesOfType(const std::string &ofType) const
{
   if (!axes) return false;
   return axes->IsOfType(ofType);
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
         ("   solar=<%p>, originName='%s', j2000BodyName='%s', origin=<%p>'%s', "
          "j2000Body=<%p>'%s'\n", solar, originName.c_str(), j2000BodyName.c_str(),
          origin, origin->GetName().c_str(), j2000Body, j2000Body->GetName().c_str());
      #endif
      
      axes->SetSolarSystem(solar);
      axes->SetOriginName(originName);
      axes->SetJ2000BodyName(j2000BodyName);
      axes->SetRefObject(origin,Gmat::SPACE_POINT,originName);
      axes->SetRefObject(j2000Body,Gmat::SPACE_POINT,j2000BodyName);
      axes->SetCoordinateSystemName(instanceName);
      axes->Initialize();
   }
   
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

//------------------------------------------------------------------------------
// void ToMJ2000Eq(const A1Mjd &epoch, const Real *inState, Real *outState,
//                 bool coincident, bool forceComputation)
//------------------------------------------------------------------------------
void CoordinateSystem::ToMJ2000Eq(const A1Mjd &epoch, const Real *inState, 
                                  Real *outState,     bool coincident,
                                  bool forceComputation)
{
   #ifdef DEBUG_INPUTS_OUTPUTS
   MessageInterface::ShowMessage
      ("In CS::ToMJ2000Eq, inState = \n   %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
       inState[0], inState[1], inState[2], inState[3],inState[4], inState[5]);
   MessageInterface::ShowMessage
      ("   axes=<%p> '%s'\n", axes, axes ? axes->GetName().c_str() : "NULL");
   #endif
   Real internalState[6];
   if (axes)
   {
      if (!axes->RotateToMJ2000Eq(epoch, inState, internalState, forceComputation))
         throw CoordinateSystemException("Error rotating state to MJ2000Eq for "
                                         + instanceName);
      #ifdef DEBUG_INPUTS_OUTPUTS
      Rvector6 rv(internalState);
      MessageInterface::ShowMessage
         ("In CS::ToMJ2000Eq, Rotated to MJ2000Eq, internalState=\n   %s\n",
          rv.ToString().c_str());
      #endif
   }
   else // assume this is MJ2000Eq, so no rotation is necessary
      for (Integer i=0; i<6;i++) internalState[i] = inState[i];
   
   #ifdef DEBUG_TRANSLATION
      MessageInterface::ShowMessage("In ToMJ2000Eq for %s, coincident = %s\n",
            instanceName.c_str(), (coincident? "TRUE" : "FALSE"));
   #endif
   if (!coincident)
   {
      if (!TranslateToMJ2000Eq(epoch, internalState, outState))
         throw CoordinateSystemException("Error translating state to MJ2000Eq for "
                                         + instanceName);
      #ifdef DEBUG_INPUTS_OUTPUTS
      Rvector6 rv(outState);
      MessageInterface::ShowMessage
         ("In CS::ToMJ2000Eq, translated to MJ2000Eq, outState=\n   %s\n",
          rv.ToString().c_str());
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
      "In CS::FromMJ2000Eq (2), translation happening\n");
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
      "In CS::FromMJ2000Eq (1), translation happening\n");
    #endif
   }
   else
      for (Integer i=0; i<6; i++) internalState[i] = inState[i];
      

   if (axes)
   {
      #ifdef DEBUG_INPUTS_OUTPUTS
         MessageInterface::ShowMessage("About to call RotateFromMJ2000Eq for object %s\n",
               (GetName()).c_str());
      #endif
      //if (!axes->RotateFromMJ2000Eq(epoch,internalState,internalState))//,j2000Body))
      //   throw CoordinateSystemException("Error rotating state from MJ2000Eq for "
      //                                   + instanceName);
      if (!axes->RotateFromMJ2000Eq(epoch,internalState,outState, 
          forceComputation))//,j2000Body))
         throw CoordinateSystemException("Error rotating state from MJ2000Eq for "
                                         + instanceName);
   }
   else
   {
      #ifdef DEBUG_INPUTS_OUTPUTS
         MessageInterface::ShowMessage("AXES are NULL in CS::FromMJ2000Eq (1) for c.s.of type %s\n",
               (GetTypeName()).c_str());
      #endif
      for (Integer i=0; i<6; i++) outState[i]    = internalState[i];
   }
                                         
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


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
bool CoordinateSystem::IsParameterReadOnly(const Integer id) const
{
   if ((id == EPOCH) || (id == UPDATE_INTERVAL) ||
       (id == OVERRIDE_ORIGIN_INTERVAL))
      return true;
   
   return CoordinateBase::IsParameterReadOnly(id);
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
bool CoordinateSystem::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
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
   else if (id == EPOCH) 
   {
      if (axes) return axes->GetRealParameter("Epoch");
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
   #ifdef DEBUG_CS_SET
   MessageInterface::ShowMessage
      ("CoordinateSystem::SetRealParameter()entered, axes=<%p>, id=%d, "
       "value=%f\n", axes, id, value);
   #endif
   
   if (id == UPDATE_INTERVAL)
   {
      if (axes)
         return axes->SetRealParameter("UpdateInterval", value);
   }
   else if (id == EPOCH)
   {
      if (axes)
         return axes->SetRealParameter("Epoch", value);
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
   {
      if (axes)
         return axes->GetTypeName();
      else
         throw CoordinateSystemException("Axis system not set for "
                                         + instanceName);
   }
      
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
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::SetStringParameter(const Integer id,
                                          const std::string &value)
{
   switch (id)
   {
   case AXES:
      return true;
   default:
      return CoordinateBase::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::SetStringParameter(const std::string &label,
                                          const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
bool CoordinateSystem::GetBooleanParameter(const Integer id) const
{
   if (id == OVERRIDE_ORIGIN_INTERVAL) 
   {
      if (axes) return axes->GetBooleanParameter("OverrideOriginInterval");
      else      return false;  // or throw an exception here?
   }
   return CoordinateBase::GetBooleanParameter(id); 
}

//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
bool CoordinateSystem::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
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
      // This is a way to get origin, primary and secondary from GmatFunction
      // without casting CoordinateSystem * (LOJ: 2009.12.18)
      // @see GmatFunction::BuildUnusedGlobalObjectList()
      case Gmat::SPACE_POINT:
         if (name == "_GFOrigin_")
            return origin;
         if (name == "_GFPrimary_")
            return GetPrimaryObject();
         if (name == "_GFSecondary_")
            return GetSecondaryObject();
      default:
         break;
   }

   // Not handled here -- invoke the next higher GetRefObject call
   return CoordinateBase::GetRefObject(type, name);
}


//---------------------------------------------------------------------------
// bool IsOwnedObject(Integer id) const;
//---------------------------------------------------------------------------
bool CoordinateSystem::IsOwnedObject(Integer id) const
{
   if (id == AXES)
      return true;
   else
      return false;
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
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::HasRefObjectTypeArray()
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
   #ifdef DEBUG_CS_SET_REF
      MessageInterface::ShowMessage
         ("Entering CS::SetRefObject with obj of type %s and name '%s'\n",
          (obj->GetTypeName()).c_str(), name.c_str());
   #endif
   if (obj == NULL)
      return false;
   
   bool retval = false;
   
   switch (type)
   {
      case Gmat::AXIS_SYSTEM:
      {
         #ifdef DEBUG_CS_SET_AXIS
         MessageInterface::ShowMessage
            ("CoordinateSystem::SetRefObject() %s, before axes=%p, obj=%p\n",
             GetName().c_str(), axes, obj);
         MessageInterface::ShowMessage("CS::SetRefObject - object is of type AXIS_SYSTEM\n");
         #endif
         
         AxisSystem *oldAxis = axes;
         
         axes = (AxisSystem*) obj->Clone();
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (axes, "clonedAxes", "CoordinateSystem::SetRefObject()",
             "(AxisSystem*) obj->Clone()");
         #endif
         axes->SetName("");
         ownedObjectCount = 1;
         
         #ifdef DEBUG_CS_SET_AXIS
         MessageInterface::ShowMessage
            ("CoordinateSystem::SetRefObject() %s, after axes=%p\n",
             GetName().c_str(), axes);
         #endif
         if (oldAxis)
         {
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (oldAxis, oldAxis->GetTypeName(), "CoordinateSystem::SetRefObject()",
                "deleting oldAxis");
            #endif
            delete oldAxis;
            oldAxis = NULL;
         }
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
   #ifdef DEBUG_CS_SET_REF
      MessageInterface::ShowMessage("CS::SetRefObject - object is NOT of type AXIS_SYSTEM\n");
   #endif
   
   // Not handled here -- invoke the next higher SetRefObject call
   return CoordinateBase::SetRefObject(obj, type, name);
}


//----------------------------
// public static methods
//----------------------------
//------------------------------------------------------------------------------
// static CoordinateSystem* CreateLocalCoordinateSystem(
//                     const std::string &csName, const std::string &axesType,
//                     SpacePoint *origin, SpacePoint *primary,
//                     SpacePoint *secondary, SpacePoint *j2000Body,
//                     SolarSystem *solarSystem)
//------------------------------------------------------------------------------
/**
 * Creates CoordinateSystem with VNB, LVLH, MJ2000Eq, and SpacecraftBody axis.
 *
 * @param  csName      Name of the cooridnate system
 * @param  axesType    Axes system type name
 * @param  origin      Axes system origin body pointer
 * @param  primary     Axes system primary body pointer
 * @param  secondary   Axes system secondary body pointer
 * @param  j2000Body   Axes system J2000 body pointer
 * @param  solarSystem Solar system pointer
 *
 * @return new CoordinateSystem pointer if successful, NULL otherwise
 */
//------------------------------------------------------------------------------
CoordinateSystem* CoordinateSystem::CreateLocalCoordinateSystem(
                     const std::string &csName, const std::string &axesType,
                     SpacePoint *origin, SpacePoint *primary,
                     SpacePoint *secondary, SpacePoint *j2000Body,
                     SolarSystem *solarSystem)
{
   #ifdef DEBUG_CS_CREATE
   MessageInterface::ShowMessage
      ("CoordinateSystem::CreateLocalCoordinateSystem() entered\n   csName='%s', "
       "axesType='%s', origin=<%p>, primary=<%p>, secondary=<%p>\n   j2000Body=<%p>, "
       "solarSystem=<%p>\n", csName.c_str(), axesType.c_str(), origin, primary,
       secondary, j2000Body, solarSystem);
   #endif
   // check for NULL pointers
   if (origin == NULL || j2000Body == NULL || solarSystem == NULL)
      return NULL;
   
   CoordinateSystem *localCS = NULL;
   AxisSystem       *theAxes = NULL;
   
   // check for supported axes name - these are used at least in the Burn code
   if (axesType == "VNB" || axesType == "LVLH" || axesType == "SpacecraftBody")
   {
      if (primary == NULL || secondary == NULL)
         return NULL;
      
      localCS  = new CoordinateSystem("CoordinateSystem",csName);
      theAxes  = new ObjectReferencedAxes(csName);
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (localCS, "localCS", "CoordinateSystem::CreateLocalCoordinateSystem()",
          "new CoordinateSystem()");
      MemoryTracker::Instance()->Add
         (theAxes, "theAxes", "CoordinateSystem::CreateLocalCoordinateSystem()",
          "new ObjectReferencedAxes()");
      #endif
      
      theAxes->SetStringParameter("Primary", primary->GetName());
      theAxes->SetStringParameter("Secondary", secondary->GetName());
      theAxes->SetRefObject(origin, Gmat::SPACE_POINT, origin->GetName());
      theAxes->SetRefObject(primary, Gmat::SPACE_POINT, primary->GetName());
      theAxes->SetRefObject(secondary, Gmat::SPACE_POINT, secondary->GetName());
      
      if (axesType == "VNB")
      {
         theAxes->SetStringParameter("XAxis", "V");
         theAxes->SetStringParameter("YAxis", "N");
         localCS->SetStringParameter("Origin", secondary->GetName());
         localCS->SetRefObject(secondary, Gmat::SPACE_POINT, secondary->GetName());
      }
      else if (axesType == "LVLH")
      {
         theAxes->SetStringParameter("XAxis", "-R");
         theAxes->SetStringParameter("YAxis", "-N");
         localCS->SetStringParameter("Origin", secondary->GetName());
         localCS->SetRefObject(secondary, Gmat::SPACE_POINT, secondary->GetName());
      }
      else if (axesType == "SpacecraftBody")
      {
         localCS->SetStringParameter("Origin", j2000Body->GetName());
      }
      
      localCS->SetRefObject(theAxes, Gmat::AXIS_SYSTEM, theAxes->GetName());
      localCS->SetRefObject(j2000Body, Gmat::SPACE_POINT, j2000Body->GetName());
      localCS->SetSolarSystem(solarSystem);
      localCS->Initialize();
      
      #ifdef DEBUG_CS_CREATE
      MessageInterface::ShowMessage
         ("   Local CS %s<%p> created with AxisSystem <%p>:\n"
          "      axesType    = '%s'\n      Origin      = <%p>'%s'\n"
          "      Primary     = <%p>'%s'\n      Secondary   = <%p>'%s'\n"
          "      j2000body   = <%p>'%s'\n", csName.c_str(), localCS, theAxes, axesType.c_str(),
          localCS->GetOrigin(), localCS->GetOriginName().c_str(), localCS->GetPrimaryObject(),
          localCS->GetPrimaryObject() ? localCS->GetPrimaryObject()->GetName().c_str() : "NULL",
          localCS->GetSecondaryObject(),
          localCS->GetSecondaryObject() ? localCS->GetSecondaryObject()->GetName().c_str() : "NULL",
          localCS->GetJ2000Body(),
          localCS->GetJ2000Body() ? localCS->GetJ2000Body()->GetName().c_str() : "NULL");
      #endif
      
      // Since CoordinateSystem clones AxisSystem, delete it from here
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (theAxes, "theAxes", "CoordinateSystem::CreateLocalCoordinateSystem()",
          "deleting theAxes");
      #endif
      delete theAxes;
      return localCS;
   }
   // these are needed at least by the Measurement code
   else if ((axesType == "MJ2000Eq") || (axesType == "Topocentric") ||
            (axesType == "BodyFixed" ))
   {
      localCS = new CoordinateSystem("CoordinateSystem",csName);
      if (axesType == "MJ2000Eq")
         theAxes = new MJ2000EqAxes(csName);
      else if (axesType == "Topocentric")
         theAxes = new TopocentricAxes(csName);
      else
         theAxes = new BodyFixedAxes(csName);
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (localCS, "localCS", "CreateLocalCoordinateSystem()",
          "new CoordinateSystem()");
      MemoryTracker::Instance()->Add
         (theAxes, "theAxes", "CreateLocalCoordinateSystem()",
          "new ObjectReferencedAxes()");
      #endif
      
      GmatGlobal *gmatGlobal = GmatGlobal::Instance();
      
      #ifdef DEBUG_CS_CREATE
      MessageInterface::ShowMessage
         ("   eop=<%p>, itrf=<%p>\n", gmatGlobal->GetEopFile(),
          gmatGlobal->GetItrfCoefficientsFile());
      #endif
      
      if (theAxes->UsesEopFile() == GmatCoordinate::REQUIRED)
         theAxes->SetEopFile(gmatGlobal->GetEopFile());
      
      if (theAxes->UsesItrfFile() == GmatCoordinate::REQUIRED)
         theAxes->SetCoefficientsFile(gmatGlobal->GetItrfCoefficientsFile());
      
      localCS->SetStringParameter("Origin", origin->GetName());
      localCS->SetRefObject(origin, Gmat::SPACE_POINT, origin->GetName());
      localCS->SetRefObject(theAxes, Gmat::AXIS_SYSTEM, theAxes->GetName());
      localCS->SetRefObject(j2000Body, Gmat::SPACE_POINT, j2000Body->GetName());
      localCS->SetSolarSystem(solarSystem);
      localCS->Initialize();
      
      #ifdef DEBUG_CS_CREATE
      MessageInterface::ShowMessage
         ("   Local CS %s<%p> created with AxisSystem <%p>:\n"
          "      axesType    = '%s'\n      Origin      = <%p>'%s'\n"
          "      Primary     = <%p>'%s'\n      Secondary   = <%p>'%s'\n"
          "      j2000body   = <%p>'%s'\n", csName.c_str(), localCS, theAxes, axesType.c_str(),
          localCS->GetOrigin(), localCS->GetOriginName().c_str(), localCS->GetPrimaryObject(),
          localCS->GetPrimaryObject() ? localCS->GetPrimaryObject()->GetName().c_str() : "NULL",
          localCS->GetSecondaryObject(),
          localCS->GetSecondaryObject() ? localCS->GetSecondaryObject()->GetName().c_str() : "NULL",
          localCS->GetJ2000Body(),
          localCS->GetJ2000Body() ? localCS->GetJ2000Body()->GetName().c_str() : "NULL");
      #endif
      
      // Since CoordinateSystem clones AxisSystem, delete it from here
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (theAxes, "theAxes", "CoordinateSystem::CreateLocalCoordinateSystem()",
          "deleting theAxes");
      #endif
      delete theAxes;
      return localCS;
   }
   else
   {
      MessageInterface::ShowMessage
         ("**** ERROR **** CoordinateSystem::CreateLocalCoordinateSystem() cannot "
          "create CoordinateSystem, axes name \"%s\" is not supported\n", axesType.c_str());
      return NULL;
   }
   
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  bool  TranslateToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
//                            Rvector &outState)
//------------------------------------------------------------------------------
/**
 * This method translates the input state, in "this" axisSystem to the MJ2000Eq
 * axisSystem.
 *
 * @param epoch      epoch for which to do the conversion.
 * @param inState    input state to convert.
 * @param outState   output (converted) state.
 *
 * @return true if successful; false if not.
 *
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::TranslateToMJ2000Eq(const A1Mjd &epoch,
                                           const Rvector &inState,
                                           Rvector &outState)
{
   #ifdef DEBUG_TRANSLATION
      MessageInterface::ShowMessage(
            "In TranslateToMJ2000Eq, coord. system is %s, origin is %s and j2000Body is %s\n",
            (axes->GetTypeName()).c_str(), (origin->GetName()).c_str(), (j2000Body->GetName()).c_str());
   #endif
   if (origin == j2000Body)  
      outState = inState;
   else
   {
      // currently assuming an Rvector6 - update needed later?!?!   - awaiting Darrel on this
      // compute vector from origin of MJ2000Eq to origin of this system
      Rvector6 rif =  origin->GetMJ2000State(epoch) - 
                      (j2000Body->GetMJ2000State(epoch));
      outState = inState + rif;
      #ifdef DEBUG_TRANSLATION
         Rvector6 tmpOrigin = origin->GetMJ2000State(epoch);
         MessageInterface::ShowMessage(
            "In translation, origin state is %12.17f  %12.17f  %12.17f \n",
            tmpOrigin[0], tmpOrigin[1], tmpOrigin[2]);
         MessageInterface::ShowMessage(
            "                                %12.17f  %12.17f  %12.17f \n",
            tmpOrigin[3], tmpOrigin[4], tmpOrigin[5]);
         Rvector6 tmpJ2000 = j2000Body->GetMJ2000State(epoch);
         MessageInterface::ShowMessage(
            "In translation, J2000Body state is %12.17f  %12.17f  %12.17f \n",
            tmpJ2000[0], tmpJ2000[1], tmpJ2000[2]);
         MessageInterface::ShowMessage(
            "                                %12.17f  %12.17f  %12.17f \n",
            tmpJ2000[3], tmpJ2000[4], tmpJ2000[5]);
         MessageInterface::ShowMessage(
            "In translation, outState is %12.17f  %12.17f  %12.17f \n",
            outState[0], outState[1], outState[2]);
         MessageInterface::ShowMessage(
            "                            %12.17f  %12.17f  %12.17f \n",
            outState[3], outState[4], outState[5]);
      #endif
   }
   return true;
}

//------------------------------------------------------------------------------
// bool TranslateToMJ2000Eq(const A1Mjd &epoch, const Real *inState,
//                          Real *outState)
//------------------------------------------------------------------------------
bool CoordinateSystem::TranslateToMJ2000Eq(const A1Mjd &epoch,
                                           const Real *inState,
                                           Real *outState)
{
   #ifdef DEBUG_TRANSLATION
      MessageInterface::ShowMessage(
            "In TranslateToMJ2000Eq, coord. system is %s, origin is %s and j2000Body is %s\n",
            (axes->GetTypeName()).c_str(), (origin->GetName()).c_str(), (j2000Body->GetName()).c_str());
   #endif
   #ifdef DEBUG_INPUTS_OUTPUTS
   MessageInterface::ShowMessage
      ("In CS::TranslateToMJ2000Eq, inState = %.17f  %.17f  %.17f  %.17f  %.17f  "
       "%.17f\n", inState[0], inState[1], inState[2], inState[3],inState[4], inState[5]);
   #endif
   if (origin == j2000Body)  
      for (Integer i=0; i<6; i++) outState[i] = inState[i];
   else
   {
      #ifdef DEBUG_INPUTS_OUTPUTS
      Rvector6 originState = origin->GetMJ2000State(epoch);
      Rvector6 j2000BodyState = j2000Body->GetMJ2000State(epoch);
      MessageInterface::ShowMessage
         ("   originState =\n   %s\n", originState.ToString().c_str());
      MessageInterface::ShowMessage
         ("   j2000BodyState =\n   %s\n", j2000BodyState.ToString().c_str());
      #endif
      
      Rvector6 rif =  origin->GetMJ2000State(epoch) -
                      (j2000Body->GetMJ2000State(epoch));
      
      #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage("   rif = %s\n", rif.ToString().c_str());
      #endif
      const Real *toRif = rif.GetDataVector();
      for (Integer i=0; i<6; i++)  outState[i] = inState[i] + toRif[i];
   }
   return true;
}

//------------------------------------------------------------------------------
//  bool  TranslateFromMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
//                              Rvector &outState)
//------------------------------------------------------------------------------
/**
 * This method translates the input state, in MJ2000Eq axisSystem to 'this'
 * axisSystem.
 *
 * @param epoch      epoch for which to do the conversion.
 * @param inState    input state to convert.
 * @param outState   output (converted) state.
 *
 * @return true if successful; false if not.
 *
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::TranslateFromMJ2000Eq(const A1Mjd &epoch,
                                             const Rvector &inState,
                                             Rvector &outState)
{
   #ifdef DEBUG_TRANSLATION
      MessageInterface::ShowMessage(
            "In TranslateFromMJ2000Eq, coord. system is %s, origin is %s and j2000Body is %s\n",
            (axes->GetTypeName()).c_str(), (origin->GetName()).c_str(), (j2000Body->GetName()).c_str());
   #endif
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

//------------------------------------------------------------------------------
// bool TranslateFromMJ2000Eq(const A1Mjd &epoch, const Real *inState,
//                            Real *outState)
//------------------------------------------------------------------------------
bool CoordinateSystem::TranslateFromMJ2000Eq(const A1Mjd &epoch,
                                             const Real *inState,
                                             Real *outState)
{
   #ifdef DEBUG_TRANSLATION
      MessageInterface::ShowMessage(
            "In TranslateFromMJ2000Eq, coord. system is %s, origin is %s and j2000Body is %s\n",
            (axes->GetTypeName()).c_str(), (origin->GetName()).c_str(), (j2000Body->GetName()).c_str());
   #endif
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

