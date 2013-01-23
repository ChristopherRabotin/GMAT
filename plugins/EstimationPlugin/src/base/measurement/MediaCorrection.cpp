//$Id: MediaCorrection.cpp 67 2010-06-17 21:56:16Z tdnguye2@NDC $
//------------------------------------------------------------------------------
//                         MediaCorrection
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Tuan Dang Nguyen GSFC-NASA
// Created: 2010/06/17
//
/**
 * Media correction model.
 */
//------------------------------------------------------------------------------


#include "MediaCorrection.hpp"
#include "Moderator.hpp"


//------------------------------------------------------------------------------
// MediaCorrection(const std::string &typeStr, const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * Standard constructor
 */
//------------------------------------------------------------------------------
MediaCorrection::MediaCorrection(const std::string &typeStr,
											const std::string &nomme) :
	GmatBase		(Gmat::MEDIA_CORRECTION, typeStr, nomme)
{
   objectTypes.push_back(Gmat::MEDIA_CORRECTION);
   objectTypeNames.push_back("MediaCorrection");

   solarSystem  = Moderator::Instance()->GetSolarSystemInUse();
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
		GmatBase  (mdc)
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
