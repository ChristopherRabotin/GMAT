//$Header$
//------------------------------------------------------------------------------
//                              AtmosphereModel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
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

#include <math.h>
#include <stdio.h>

#include "GmatBase.hpp"
#include "SolarFluxFileReader.hpp"
#include "AtmosphereException.hpp"

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
   AtmosphereModel(const std::string &typeStr);
   virtual ~AtmosphereModel();
    
   // copy constructor - needed by child classes (for Clone)
   AtmosphereModel(const AtmosphereModel& am);
   AtmosphereModel& operator=(const AtmosphereModel& am);
    
   /**
    * Calculates the atmospheric density at a specified location.
    *
    * Density if the core calculation provided by classes derived from this one.
    * The outpur array, density, must contain the density at the requested
    * locations, expressed in kg / m^3.
    *
    *  @param position  The input vector of spacecraft states
    *  @param density   The array of output densities
    *  @param epoch     The current TAIJulian epoch
    *  @param count     The number of spacecraft contained in position
    */
   virtual bool Density(Real *position, Real *density, Real epoch = 21545.0,
                        Integer count = 1) = 0;
   void SetSunVector(Real *sv);
   void SetCentralBodyVector(Real *cv);
   virtual void SetSolarSystem(SolarSystem *ss);
   void SetCentralBody(CelestialBody *cb);

   // Methods overridden from GmatBase
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   virtual Real        GetRealParameter(const Integer id) const;
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value);

   // Flux file methods
   void SetSolarFluxFile(std::string file);
   void SetNewFileFlag(bool flag);
   void SetOpenFileFlag(bool flag);
   void CloseFile();

protected:
   /// Solar flux binary file reader
   SolarFluxFileReader *fileReader;
   /// The solarsystem
   SolarSystem *solarSystem;
   /// The central body
   CelestialBody *mCentralBody;
   /// Pointer to the binary file
   FILE *solarFluxFile;
   /// Solar flux file name
   std::string fileName;  // waw: Added 06/29/04
   /// Vector from the central body to the sun
   Real                    *sunVector;
   /// Name of the central body
   std::string             centralBody;
   /// Location of the central body
   Real                    *centralBodyLocation;
   /// Central body radius
   Real                    cbRadius;
   /// New file?
   bool newFile;
   /// File read?
   bool fileRead;
    
   // Values used if a file is not set
   /// Nominal value of F10.7 to use
   Real                    nominalF107;
   /// Nominal 3 month average of the F10.7 data
   Real                    nominalF107a;
   /// Nominal geomagnetic index
   Real                    nominalAp;
   
   enum {
      NOMINAL_FLUX = GmatBaseParamCount,
      NOMINAL_AVERGE_FLUX,
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
