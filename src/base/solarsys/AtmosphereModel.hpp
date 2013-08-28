//$Id$
//------------------------------------------------------------------------------
//                              AtmosphereModel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/21
//
/**
 * Base class for the atmosphere models
 */
//------------------------------------------------------------------------------

#ifndef AtmosphereModel_hpp
#define AtmosphereModel_hpp

#include "GmatBase.hpp"
#include "SolarFluxFileReader.hpp"
#include "AtmosphereException.hpp"
#include "TimeTypes.hpp"

class SolarSystem;          // Forward reference
class CelestialBody;

/**
 * Base class for atmosphere models.
 * 
 * Atmosphere models are used to model drag forces and other aerodynamics.
 */ 
class GMAT_API AtmosphereModel : public GmatBase
{
public:
   AtmosphereModel(const std::string &typeStr, const std::string &name = "");
   virtual ~AtmosphereModel();
   
   // copy constructor - needed by child classes (for Clone)
   AtmosphereModel(const AtmosphereModel& am);
   AtmosphereModel& operator=(const AtmosphereModel& am);
    
   //---------------------------------------------------------------------------
   // bool Density(Real *position, Real *density, Real epoch = GmatTimeConstants::MJD_OF_J2000,
   //              Integer count = 1)
   //---------------------------------------------------------------------------
   /**
    * Calculates the atmospheric density at a specified location.
    *
    * Density is the core calculation provided by classes derived from this one.
    * The output array, density, must contain the density at the requested
    * locations, expressed in kg / m^3.
    *
    * @param <position>  The input vector of spacecraft states, in MJ2000Eq
    *                      coordinates.
    * @param <density>   The array of output densities.
    * @param <epoch>     The current TAI modified Julian epoch.
    * @param <count>     The number of spacecraft contained in position.
    *
    * @return true on success, false if a problem is encountered.
    */
   //---------------------------------------------------------------------------
   virtual bool Density(Real *position, Real *density, Real epoch = GmatTimeConstants::MJD_OF_J2000,
                        Integer count = 1) = 0;

   void         SetSunVector(Real *sv);
   void         SetCentralBodyVector(Real *cv);
   virtual void SetSolarSystem(SolarSystem *ss);
   virtual void SetCentralBody(CelestialBody *cb);
   void         SetUpdateParameters(Real interval, GmatEpoch epoch = -1.0);
   virtual void SetInternalCoordSystem(CoordinateSystem *cs);
   virtual void SetCbJ2000CoordinateSystem(CoordinateSystem *cs);
   void         SetFixedCoordinateSystem(CoordinateSystem *cs);
   Real*        GetAngularVelocity(const Real GmatEpoch = -1.0);
   void         BuildAngularVelocity(const Real GmatEpoch);
   void         UpdateAngularVelocity(const Real GmatEpoch);
   void         SetKpApConversionMethod(Integer method);
   Real         ConvertKpToAp(const Real kp);

   // Extra methods some models may support
   virtual bool HasWindModel();
   virtual bool Wind(Real *position, Real* wind, Real ep,
						   Integer count = 1);
   virtual bool HasTemperatureModel();
   virtual bool Temperature(Real *position, Real *temperature, 
                            Real epoch = GmatTimeConstants::MJD_OF_J2000,
                            Integer count = 1);
   virtual bool HasPressureModel();
   virtual bool Pressure(Real *position, Real *pressure, 
                         Real epoch = GmatTimeConstants::MJD_OF_J2000,
                         Integer count = 1);


   // Methods overridden from GmatBase
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;
   
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   virtual Real        GetRealParameter(const Integer id) const;
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value);

   // Flux file methods
   void SetSolarFluxFile(const std::string &file);
   void SetNewFileFlag(bool flag);
   void SetOpenFileFlag(bool flag);
   void CloseFile();

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// Solar flux binary file reader
   SolarFluxFileReader     *fileReader;
   /// The solarsystem
   SolarSystem             *solarSystem;
   /// The central body
   CelestialBody           *mCentralBody;
   /// Pointer to the binary file
   FILE                    *solarFluxFile;
   /// Solar flux file name
   std::string             fileName;
   /// Vector from the central body to the sun
   Real                    *sunVector;
   /// Name of the central body
   std::string             centralBody;
   /// Location of the central body
   Real                    *centralBodyLocation;
   /// Central body radius
   Real                    cbRadius;
   /// Central body flattening factor
   Real                    cbFlattening;
   /// Flag indicating that the flux file name is set to a new value
   bool                    newFile;
   /// Flag indicating that the flux file has been opened and read once already
   bool                    fileRead;
    
   // Values used if a file is not set
   /// Nominal value of F10.7 to use.
   Real                    nominalF107;
   /// Nominal 3 month average of the F10.7 data.
   Real                    nominalF107a;
   /// Nominal geomagnetic (planetary) index, \f$K_p\f$, the value users access.
   Real                    nominalKp;
   /// Nominal geomagnetic planetary amplitude, calculated via Vallado eq. 8-31.
   Real                    nominalAp;
   /// Index used to select Kp/Ap conversion method.  Default is a table lookup
   Integer                 kpApConversion;
   /// Internal coordinate system used for conversions
   CoordinateSystem        *mInternalCoordSystem;
   /// MJ2000 CS for the central body
   CoordinateSystem        *cbJ2000;
   /// Body fixed CS for the central body
   CoordinateSystem        *cbFixed;
   /// Angular velocity of the central body
   Real                    angVel[3];
   /// Update interval for the angular momentum vector
   Real                    wUpdateInterval;
   /// Most recent update epoch for the angular momentum
   GmatEpoch               wUpdateEpoch;
   /// Most recent geodetic height calculated
   Real                    geoHeight;
   /// Most recent geodetic latitude calculated
   Real                    geoLat;
   /// Most recent geodetic longitude calculated
   Real                    geoLong;
   /// Specify geodetic coordinate system or gecentric to be used for calculation		// made changes by TUAN NGUYEN
   bool                    useGeodetic;													// made changes by TUAN NGUYEN
   /// Most recent GHA calculated
   Real                    gha;
   /// GHA epoch
   Real                    ghaEpoch;

   
   Real                    CalculateGeodetics(Real *position,
                                 GmatEpoch when = -1.0,
                                 bool includeLatLong = false);

   Real                    CalculateGeocentrics(Real *position,
                                 GmatEpoch when = -1.0,
                                 bool includeLatLong = false);

   enum
   {
      NOMINAL_FLUX = GmatBaseParamCount,
      NOMINAL_AVERAGE_FLUX,
      NOMINAL_MAGNETIC_INDEX,
      AtmosphereModelParamCount
   };
   
   static const std::string
      PARAMETER_TEXT[AtmosphereModelParamCount - GmatBaseParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[AtmosphereModelParamCount - GmatBaseParamCount];

private:  
    
};

#endif // AtmosphereModel_hpp
