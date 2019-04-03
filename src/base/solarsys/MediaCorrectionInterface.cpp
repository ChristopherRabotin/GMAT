//$Id: MediaCorrectionInterface.cpp 67 2010-06-17 21:56:16Z tdnguye2@NDC $
//------------------------------------------------------------------------------
//                         MediaCorrectionInterface
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


#include "MediaCorrectionInterface.hpp"
#include "Moderator.hpp"


//------------------------------------------------------------------------------
// MediaCorrectionInterface(const std::string &typeStr, const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * Standard constructor
 */
//------------------------------------------------------------------------------
MediaCorrectionInterface::MediaCorrectionInterface(const std::string &typeStr,
                                 const std::string &nomme) :
   GmatBase         (Gmat::MEDIA_CORRECTION, typeStr, nomme),
   dataPath         (""),
   solarSystem      (NULL),
   temperature      (295.1),      // 295.1K
   pressure         (1013.5),     // 1013.5 kPa
   humidityFraction (0.55),       // 55%
   waveLength       (0.0),
   range            (1.0),
   latitude         (0.0),
   longitude        (0.0)
{
   objectTypes.push_back(Gmat::MEDIA_CORRECTION);
//   objectTypeNames.push_back("MediaCorrection");

   modelName     = typeStr;
   model         = 0;
   modelTypeName = "";
}

//------------------------------------------------------------------------------
// ~MediaCorrectionInterface()
//------------------------------------------------------------------------------
/**
 * Donstructor
 */
//------------------------------------------------------------------------------
MediaCorrectionInterface::~MediaCorrectionInterface()
{
}

//------------------------------------------------------------------------------
// MediaCorrectionInterface(const MediaCorrectionInterface& mdc)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
MediaCorrectionInterface::MediaCorrectionInterface(const MediaCorrectionInterface& mdc):
   GmatBase           (mdc),
   dataPath           (mdc.dataPath),
   pressure           (mdc.pressure),
   temperature        (mdc.temperature),
   humidityFraction   (mdc.humidityFraction),
   waveLength         (mdc.waveLength),
   range              (mdc.range),
   latitude           (mdc.latitude),
   longitude          (mdc.longitude)
{
   model         = mdc.model;
   modelName     = mdc.modelName;
   modelTypeName = mdc.modelTypeName;
   solarSystem   = mdc.solarSystem;
}


//-----------------------------------------------------------------------------
// MediaCorrectionInterface& MediaCorrectionInterface::operator=(const MediaCorrectionInterface& mc)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param mc The MediaCorrectionInterface that is provides parameters for this one
 *
 * @return This MediaCorrectionInterface, configured to match mc
 */
//-----------------------------------------------------------------------------
MediaCorrectionInterface& MediaCorrectionInterface::operator=(const MediaCorrectionInterface& mc)
{
   if (this != &mc)
   {
      GmatBase::operator=(mc);

      model             = mc.model;
      modelName         = mc.modelName;
      modelTypeName     = mc.modelTypeName;
      solarSystem       = mc.solarSystem;

      dataPath          = mc.dataPath;

      temperature       = mc.temperature;
      pressure          = mc.pressure;
      humidityFraction  = mc.humidityFraction;
      waveLength        = mc.waveLength;
      range             = mc.range;
   }

   return *this;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs any pre-run initialization that the object needs.
 *
 * @return true unless initialization fails.
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::Initialize()
{
   if (IsInitialized())
      return true;

   if (IsOfType("Ionosphere"))
   {
      // Get path of data folder
      FileManager* fm = FileManager::Instance();
      dataPath = fm->GetPathname("DATA_PATH");

      isInitialized = true;
   }

   return true;
}


//------------------------------------------------------------------------------
// bool SetModel(Integer mod)
//------------------------------------------------------------------------------
/**
 * Set a correction model
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetModel(Integer mod)
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
bool MediaCorrectionInterface::SetModelName(std::string modName)
{
   modelName = modName;
   return true;
}


//------------------------------------------------------------------------------
// bool SetModelTypeName(Real T)
//------------------------------------------------------------------------------
/**
 * Set the name of the type of media correction model used
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetModelTypeName(std::string type)
{
	modelTypeName = type;
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
void MediaCorrectionInterface::SetSolarSystem(SolarSystem *ss)
{
   solarSystem = ss;
}


//------------------------------------------------------------------------------
// bool SetTemperature(Real T)
//------------------------------------------------------------------------------
/**
 * Set temperature
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetTemperature(Real T)
{
   temperature = T;
   return true;
}

//------------------------------------------------------------------------------
// bool SetPressure(Real P)
//------------------------------------------------------------------------------
/**
 * Set pressure
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetPressure(Real P)
{
   pressure = P;
   return true;
}


//------------------------------------------------------------------------------
// bool SetHumidityFraction(Real humFr)
//------------------------------------------------------------------------------
/**
 * Set humidity fraction
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetHumidityFraction(Real humFr)
{
   humidityFraction = humFr;
   return true;
}

//------------------------------------------------------------------------------
// bool SetElevationAngle(Real elevation)
//------------------------------------------------------------------------------
/**
 * Set elevation angle
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetElevationAngle(Real elevation)
{
   elevationAngle = elevation;
   return true;
}

//------------------------------------------------------------------------------
// bool SetRange(Real r)
//------------------------------------------------------------------------------
/**
 * Set range
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetRange(Real r)
{
   range = r;
   return true;
}

//------------------------------------------------------------------------------
// bool SetWaveLength(Real lambda)
//------------------------------------------------------------------------------
/**
 * Set wave length
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetWaveLength(Real lambda)
{
   waveLength = lambda;
   return true;
}

//------------------------------------------------------------------------------
// bool SetLatitude(Real lat)
//------------------------------------------------------------------------------
/**
 * Set latitude
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetLatitude(Real lat)
{
	latitude = lat;
	return true;
}

//------------------------------------------------------------------------------
// bool SetLongitude(Real lat)
//------------------------------------------------------------------------------
/**
 * Set longitude
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetLongitude(Real lon)
{
	longitude = lon;
	return true;
}
