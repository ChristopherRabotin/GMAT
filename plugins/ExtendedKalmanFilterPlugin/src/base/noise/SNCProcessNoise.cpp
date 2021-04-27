//$Id$
//------------------------------------------------------------------------------
//                               SNCProcessNoise
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Emergent Space Technologies, Inc. under
// contract number NNG15CR67C
//
// Author: John McGreevy, Emergent Space Technologies, Inc.
// Created: 2018/09/19
/**
 * A class for process noise using the SNC model
 */
//------------------------------------------------------------------------------

#include "SNCProcessNoise.hpp"
#include "NoiseException.hpp"
#include "StringUtil.hpp"

//---------------------------------
// static data
//---------------------------------

const std::string
SNCProcessNoise::PARAMETER_TEXT[SNCProcessNoiseParamCount - ProcessNoiseBaseParamCount] =
{
   "AccelNoiseSigma",
};

const Gmat::ParameterType
SNCProcessNoise::PARAMETER_TYPE[SNCProcessNoiseParamCount - ProcessNoiseBaseParamCount] =
{
   Gmat::RVECTOR_TYPE,     // "AccelNoiseSigma",
};


//------------------------------------------------------------------------------
// SNCProcessNoise(const std::string &itsTypeName,
//       const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the new object
 */
//------------------------------------------------------------------------------
SNCProcessNoise::SNCProcessNoise(const std::string &name) :
   ProcessNoiseBase    ("SNCProcessNoise",name,"StateNoiseCompensation")
{
   objectTypeNames.push_back("SNCProcessNoise");

   parameterCount = SNCProcessNoiseParamCount;

   accelSigmaVec.Set(0.0, 0.0, 0.0);
}


//------------------------------------------------------------------------------
// ~SNCProcessNoise()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SNCProcessNoise::~SNCProcessNoise()
{
}


//------------------------------------------------------------------------------
// SNCProcessNoise(const SNCProcessNoise& pnm)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
SNCProcessNoise::SNCProcessNoise(
      const SNCProcessNoise& spn) :
   ProcessNoiseBase (spn),
   accelSigmaVec    (spn.accelSigmaVec)
{
}



//------------------------------------------------------------------------------
// SNCProcessNoise& operator=(const SNCProcessNoise& pnm)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
//------------------------------------------------------------------------------
SNCProcessNoise& SNCProcessNoise::operator=(
      const SNCProcessNoise& spn)
{
   if (this != &spn)
   {
      ProcessNoiseBase::operator=(spn);

      accelSigmaVec = spn.accelSigmaVec;
   }
   return *this;
}


//------------------------------------------------------------------------------
// Rmatrix66 GetProcessNoise(const GmatEpoch elapsedTime, const GmatTime &epoch)
//------------------------------------------------------------------------------
/**
 * Get the process noise for a specified elaped time
 *
 * @param elapsedTime The elapsed time to evaluate the process noise over.
 * @param epoch The epoch to evaluate the process noise coordinate conversion at.
 *
 * @return The process noise matrix.
 */
//------------------------------------------------------------------------------
Rmatrix66 SNCProcessNoise::GetProcessNoise(const GmatEpoch elapsedTime, const GmatTime &epoch)
{
   Rmatrix66 result(false);

   Real dt2 = elapsedTime*elapsedTime;
   Real dt3 = elapsedTime*dt2;

   for (UnsignedInt ii = 0U; ii < 3U; ii++)
   {
      result(ii,ii)     = accelSigmaVec(ii)*accelSigmaVec(ii)*dt3/3.;
      result(ii,ii+3)   = accelSigmaVec(ii)*accelSigmaVec(ii)*dt2/2.;
      result(ii+3,ii)   = accelSigmaVec(ii)*accelSigmaVec(ii)*dt2/2.;
      result(ii+3,ii+3) = accelSigmaVec(ii)*accelSigmaVec(ii)*elapsedTime;
   }

   ConvertMatrix(result, epoch);
   return result;
}


//------------------------------------------------------------------------------
//  SNCProcessNoise* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the SNCProcessNoise.
 *
 * @return clone of the SNCProcessNoise.
 *
 */
//------------------------------------------------------------------------------
GmatBase* SNCProcessNoise::Clone(void) const
{
   return (new SNCProcessNoise(*this));
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType SNCProcessNoise::GetParameterType(const Integer id) const
{
   if (id >= ProcessNoiseBaseParamCount && id < SNCProcessNoiseParamCount)
      return PARAMETER_TYPE[id - ProcessNoiseBaseParamCount];
   else
      return ProcessNoiseBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string SNCProcessNoise::GetParameterTypeString(const Integer id) const
{
   if (id >= ProcessNoiseBaseParamCount && id < SNCProcessNoiseParamCount)
      return ProcessNoiseBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return ProcessNoiseBase::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string SNCProcessNoise::GetParameterText(const Integer id) const
{
   if (id >= ProcessNoiseBaseParamCount && id < SNCProcessNoiseParamCount)
      return PARAMETER_TEXT[id - ProcessNoiseBaseParamCount];
   else
      return ProcessNoiseBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer SNCProcessNoise::GetParameterID(const std::string &str) const
{
   for (int i=ProcessNoiseBaseParamCount; i<SNCProcessNoiseParamCount; i++)
   {
      if (str == SNCProcessNoise::PARAMETER_TEXT[i - ProcessNoiseBaseParamCount])
         return i;
   }
   
   return ProcessNoiseBase::GetParameterID(str);
}

//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id,
//                        const Integer index) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
Real SNCProcessNoise::GetRealParameter(const Integer id,
                                         const Integer index) const
{
   if (id == ACCEL_SIGMA_VECTOR)
   {
      return accelSigmaVec(index);
   }

   return ProcessNoiseBase::GetRealParameter(id, index);
}

//---------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label,
//                       const Integer index) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
Real SNCProcessNoise::GetRealParameter(const std::string &label,
                                         const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}

//---------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value,
//                        const Integer index)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
Real SNCProcessNoise::SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index)
{
   if (id == ACCEL_SIGMA_VECTOR)
   {
      // Enforce size
      if (index >= accelSigmaVec.GetSize())
      {
         throw NoiseException("AccelNoiseSigma must be size 3");
      }
      accelSigmaVec.SetElement(index, value);
      return value;
   }

   return ProcessNoiseBase::SetRealParameter(id, value, index);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value,
//                        const Integer index)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real SNCProcessNoise::SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index)
{
   return SetRealParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const Rvector& GetRvectorParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rvector& SNCProcessNoise::GetRvectorParameter(const Integer id) const
{
   std::string name;

   switch (id)
   {
   case ACCEL_SIGMA_VECTOR:
      return accelSigmaVec;
   default:
      return ProcessNoiseBase::GetRvectorParameter(id);
   }
}

//------------------------------------------------------------------------------
// const Rvector& GetRvectorParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rvector& SNCProcessNoise::GetRvectorParameter(const std::string &label) const
{
   return GetRvectorParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const Rvector& SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rvector& SNCProcessNoise::SetRvectorParameter(const Integer id, const Rvector &value)
{
   switch (id)
   {
   case ACCEL_SIGMA_VECTOR:
      if (value.GetSize() == 3)
      {
         accelSigmaVec = value.GetRealArray();
      }
      else
      {
         NoiseException ne("");
         ne.SetDetails(errorMessageFormat.c_str(),
            GmatStringUtil::ToString(value.GetSize(), 16).c_str(),
            "Cd", "Real Number >= 0.0");
         throw ne;
      }
   default:
      return ProcessNoiseBase::SetRvectorParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// const Rvector& SetStringParameter(const std::string &label, const Rvector &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rvector& SNCProcessNoise::SetRvectorParameter(const std::string &label,
                                   const Rvector &value)
{
   return SetRvectorParameter(GetParameterID(label), value);
}
