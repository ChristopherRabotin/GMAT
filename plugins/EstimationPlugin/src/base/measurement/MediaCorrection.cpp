//$Id: MediaCorrection.cpp 67 2010-06-17 21:56:16Z  $
//------------------------------------------------------------------------------
//                         MediaCorrection
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Tuan Dang Nguyen GSFC-NASA
// Created: 2010/06/17
//
/**
 * Media correction model.
 */
//------------------------------------------------------------------------------


#include "MediaCorrection.hpp"


//------------------------------------------------------------------------------
// MediaCorrection(const std::string &typeStr, const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * Standard constructor
 */
//------------------------------------------------------------------------------
MediaCorrection::MediaCorrection(const std::string &typeStr,
											const std::string &nomme) :
	MediaCorrectionInterface (typeStr, nomme),
	solarSystem     (NULL)
{
   objectTypes.push_back(Gmat::MEDIA_CORRECTION);
   objectTypeNames.push_back("MediaCorrection");

   modelName	= typeStr;
   model 		= 0;
}

//------------------------------------------------------------------------------
// ~MediaCorrection()
//------------------------------------------------------------------------------
/**
 * Donstructor
 */
//------------------------------------------------------------------------------
MediaCorrection::~MediaCorrection()
{
}

//------------------------------------------------------------------------------
// MediaCorrection(const MediaCorrection& mdc)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
MediaCorrection::MediaCorrection(const MediaCorrection& mdc):
//		GmatBase  (mdc)
   MediaCorrectionInterface(mdc)
{
	model		= mdc.model;
	modelName   = mdc.modelName;
	solarSystem = mdc.solarSystem;
}


//-----------------------------------------------------------------------------
// MediaCorrection& MediaCorrection::operator=(const MediaCorrection& mc)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param mc The MediaCorrection that is provides parameters for this one
 *
 * @return This MediaCorrection, configured to match mc
 */
//-----------------------------------------------------------------------------
MediaCorrection& MediaCorrection::operator=(const MediaCorrection& mc)
{
   if (this != &mc)
   {
      GmatBase::operator=(mc);

   	model = mc.model;
   	modelName = mc.modelName;
	solarSystem = mc.solarSystem;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone
 */
//------------------------------------------------------------------------------
GmatBase* MediaCorrection::Clone() const
{
	return new MediaCorrection(*this);
}

//------------------------------------------------------------------------------
// bool SetModel(Integer mod)
//------------------------------------------------------------------------------
/**
 * Set a correction model
 */
//------------------------------------------------------------------------------
bool MediaCorrection::SetModel(Integer mod)
{
	model = mod;
	return true;
}


//------------------------------------------------------------------------------
// bool SetModelName(std::string modName)
//------------------------------------------------------------------------------
/**
 * Set a correction model name
 */
//------------------------------------------------------------------------------
bool MediaCorrection::SetModelName(std::string modName)
{
	modelName = modName;
	return true;
}

//------------------------------------------------------------------------------
//  void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system pointer in oreder to access needed physical parameter
 * value(s).
 */
//------------------------------------------------------------------------------
void MediaCorrection::SetSolarSystem(SolarSystem *ss)
{
   solarSystem = ss;
}


//------------------------------------------------------------------------------
// RealArray Correction()
//------------------------------------------------------------------------------
/**
 * Make a media correction
 */
//------------------------------------------------------------------------------
RealArray MediaCorrection::Correction()
{
	RealArray result;

	return result;
}
