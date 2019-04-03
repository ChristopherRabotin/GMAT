//$Id: MediaCorrectionInterface.hpp 67 2010-06-17 21:56:16Z tdnguye2@NDC $
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
 * Interface of a media correction model.
 */
//------------------------------------------------------------------------------


#ifndef MediaCorrectionInterface_hpp
#define MediaCorrectionInterface_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "SolarSystem.hpp"


class GMAT_API MediaCorrectionInterface: public GmatBase
{
public:
   MediaCorrectionInterface(const std::string &typeStr,
                  const std::string &nomme = "");
   virtual ~MediaCorrectionInterface();
   MediaCorrectionInterface(const MediaCorrectionInterface& mdc);
   MediaCorrectionInterface& operator=(const MediaCorrectionInterface& mc);

   virtual bool Initialize();

   virtual bool SetModel(Integer mod);
   virtual bool SetModelName(std::string modName);
   virtual bool SetModelTypeName(std::string type);

   virtual void SetSolarSystem(SolarSystem *ss);

   virtual bool SetTemperature(Real T);
   virtual bool SetPressure(Real P);
   virtual bool SetHumidityFraction(Real humFr);
   virtual bool SetWaveLength(Real lambda);
   virtual bool SetElevationAngle(Real elevation);
   virtual bool SetRange(Real r);
   virtual bool SetLatitude(Real lat);
   virtual bool SetLongitude(Real lon);

   virtual RealArray Correction() = 0;

   /// @todo: Check this
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   Integer model;
   std::string modelName;
   std::string modelTypeName;

   SolarSystem *solarSystem;

   Real temperature;               // unit: K
   Real pressure;                  // unit: hPa
   Real humidityFraction;          // unit: no unit. It's range is from 0 to 1
   Real waveLength;                // unit: m
   Real elevationAngle;            // unit: radian
   Real range;                     // unit: m
   Real latitude;                  // unit: radian
   Real longitude;                 // unit: radian

   /// Contain path name of data folder. It needs for Ionosphere code to read ap.dat file and specify the epoch range
   std::string     dataPath;

};

#endif /* MediaCorrectionInterface_hpp */
