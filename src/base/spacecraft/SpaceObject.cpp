//$Header$
//------------------------------------------------------------------------------
//                              SpaceObject
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CI63P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/7/24
//
/**
 * Implements the base class used for spacecraft and formations. 
 */
//------------------------------------------------------------------------------


#include "SpaceObject.hpp"


const std::string SpaceObject::PARAMETER_TEXT[SpaceObjectParamCount - 
                                              GmatBaseParamCount] =
   {
      "Epoch"
   };


const Gmat::ParameterType SpaceObject::PARAMETER_TYPE[SpaceObjectParamCount - 
                                                      GmatBaseParamCount] =
   {
      Gmat::REAL_TYPE
   };


SpaceObject::SpaceObject(Gmat::ObjectType typeId, const std::string &typeStr, 
                         const std::string &nomme) :
   GmatBase       (typeId, typeStr, nomme)
{
}


SpaceObject::~SpaceObject()
{
}


SpaceObject::SpaceObject(const SpaceObject& so) :
   GmatBase       (so),
   state          (so.state)
{
}


SpaceObject& SpaceObject::operator=(const SpaceObject& so)
{
   if (this == &so)
      return *this;
      
   state = so.state;
   return *this;
}


PropState& SpaceObject::GetState()
{
   return state;
}


Real SpaceObject::GetEpoch()
{
   return state.GetEpoch();
}


Real SpaceObject::SetEpoch(const Real ep)
{
   return state.SetEpoch(ep);
}


Integer SpaceObject::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < SpaceObjectParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
}


std::string SpaceObject::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < SpaceObjectParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return SpaceObject::GetParameterText(id);
}


Gmat::ParameterType SpaceObject::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < SpaceObjectParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   
   return GmatBase::GetParameterType(id);
}


std::string SpaceObject::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


Real SpaceObject::GetRealParameter(const Integer id) const
{
   if (id == EPOCH_PARAM)
      return state.GetEpoch();
   return GmatBase::GetRealParameter(id);
}


Real SpaceObject::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


Real SpaceObject::SetRealParameter(const Integer id, const Real value)
{
   if (id == EPOCH_PARAM)
      return state.SetEpoch(value);
   return GmatBase::GetRealParameter(id);
}


Real SpaceObject::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}
