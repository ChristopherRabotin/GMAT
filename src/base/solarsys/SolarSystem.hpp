//$Header$
//------------------------------------------------------------------------------
//                                  SolarSystem
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy C. Shoan
// Created: 2004/01/28
//
/**
 * This is the SolarSystem class.
 *
 * @note The SolarSystem will contain pointers to all of the celestial bodies
 *       currently in use; NOTE *** for Build 2, this is Sun/Earth/Moon only***
 *       For Build 3, all planets (except Sedna?) are included.
 * @note It is assumed no classes will be derived from this one.
 */
//------------------------------------------------------------------------------


// Class (initial shell only) automatically generated by Dev-C++ New Class wizard

#ifndef SolarSystem_hpp
#define SolarSystem_hpp

#include <list>
#include <string>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CelestialBody.hpp"
#include "PlanetaryEphem.hpp"
/**
 * SolarSystem class, containing pointers to all of the objects currently in
 * use.
 *
 * The SolarSystem class manages all objects that are currently defined for the
 * specified Solar System.  NOTE - For Build 2, the default Solar System
 * contains only the Sun, Earth, and Moon.
 */
class SolarSystem : public GmatBase
{
public:
   

   // class default constructor - creates default solar system
   // for Build 2 - this is Sun, Earth, Moon only
   SolarSystem(std::string withName = "");
   // copy constructor
   SolarSystem(const SolarSystem &ss);
   // operator=
   SolarSystem& operator=(const SolarSystem &ss);
   // class destructor
   ~SolarSystem();

   // method to add a body to the solar system
   bool           AddBody(CelestialBody* cb);
   // method to return a body of the solar system, given its name
   CelestialBody* GetBody(std::string withName);

   // method to return an array of the names of the bodies included in
   // this solar system
   const StringArray&   GetBodiesInUse() const;
   // method to return a flag indicating whether or not the specified
   // body is in use for this solar system
   bool           IsBodyInUse(std::string theBody);

   // methods to gt the source and analytic model flags
   Gmat::PosVelSource   GetPosVelSource() const;
   Gmat::AnalyticMethod GetAnalyticMethod() const;
   std::string          GetSourceFileName() const;
   bool                 GetOverrideTimeSystem() const;
   Real                 GetEphemUpdateInterval() const;
   StringArray          GetValidModelList(Gmat::ModelType m, 
                                          const std::string &forBody);

   // methods to set the source, source file, and analytic method for each
   // of the bodies in use
   bool SetSource(Gmat::PosVelSource pvSrc);
   bool SetSource(const std::string &pvSrc);
   bool SetSourceFile(PlanetaryEphem *src);
   bool SetAnalyticMethod(Gmat::AnalyticMethod aM);
   bool SetAnalyticMethod(const std::string &aM);
   
   bool SetOverrideTimeSystem(bool overrideIt);
   bool SetEphemUpdateInterval(Real intvl);
   bool AddValidModelName(Gmat::ModelType m, const std::string &forBody,
                          const std::string &theModel);
   bool RemoveValidModelName(Gmat::ModelType m, const std::string &forBody,
                             const std::string &theModel);
   
   // Parameter access methods - overridden from GmatBase do I need these???
   virtual std::string    GetParameterText(const Integer id) const;     // const?
   virtual Integer        GetParameterID(const std::string &str) const; // const?
   virtual Gmat::ParameterType
                          GetParameterType(const Integer id) const;
   virtual std::string    GetParameterTypeString(const Integer id) const;

   virtual Integer        GetIntegerParameter(const Integer id) const; // const?
   //virtual Integer        SetIntegerParameter(const Integer id,
   //                                           const Integer value);
   virtual Integer        GetIntegerParameter(const std::string &label) const;
   //virtual Integer        SetIntegerParameter(const std::string &label,
   //                                           const Integer value);
   //virtual Real           GetRealParameter(const Integer id) const;
   //virtual Real           SetRealParameter(const Integer id,
   //                                        const Real value);
   //virtual std::string    GetStringParameter(const Integer id) const; // const?
   //virtual bool           SetStringParameter(const Integer id,
   //                                          const std::string &value); // const?
   virtual Real           GetRealParameter(const Integer id) const;
   virtual Real           GetRealParameter(const std::string &label) const;
   virtual Real           SetRealParameter(const Integer id,
                                           const Real value);
   virtual Real           SetRealParameter(const std::string &label,
                                           const Real value);
   virtual bool           GetBooleanParameter(const Integer id) const; 
   virtual bool           GetBooleanParameter(const std::string &label) const; 
   virtual bool           SetBooleanParameter(const Integer id,
                                              const bool value); 
   virtual bool           SetBooleanParameter(const std::string &label,
                                              const bool value);
   virtual const StringArray& GetStringArrayParameter(const Integer id) const;
   virtual const StringArray& GetStringArrayParameter(const std::string &label) const;

      
   // all classes derived from GmatBase must supply this Clone method
   virtual SolarSystem* Clone(void) const;

   /// default names for each of the possible celestial bodies in the solar system
   static const std::string SUN_NAME;
   
   static const std::string MERCURY_NAME;
   
   static const std::string VENUS_NAME;
   
   static const std::string EARTH_NAME;
   static const std::string MOON_NAME;
   
   static const std::string MARS_NAME;
   static const std::string PHOBOS_NAME;
   static const std::string DEIMOS_NAME;
   
   static const std::string JUPITER_NAME;
   static const std::string METIS_NAME;
   static const std::string ADRASTEA_NAME;
   static const std::string AMALTHEA_NAME;
   static const std::string THEBE_NAME;
   static const std::string IO_NAME;
   static const std::string EUROPA_NAME;
   static const std::string GANYMEDE_NAME;
   static const std::string CALLISTO_NAME;
   
   static const std::string SATURN_NAME;
   static const std::string PAN_NAME;
   static const std::string ATLAS_NAME;
   static const std::string PROMETHEUS_NAME;
   static const std::string PANDORA_NAME;
   static const std::string EPIMETHEUS_NAME;
   static const std::string JANUS_NAME;
   static const std::string MIMAS_NAME;
   static const std::string ENCELADUS_NAME;
   static const std::string TETHYS_NAME;
   static const std::string TELESTO_NAME;
   static const std::string CALYPSO_NAME;
   static const std::string DIONE_NAME;
   static const std::string HELENE_NAME;
   static const std::string RHEA_NAME;
   static const std::string TITAN_NAME;
   static const std::string IAPETUS_NAME;
   static const std::string PHOEBE_NAME;
   
   static const std::string URANUS_NAME;
   static const std::string CORDELIA_NAME;
   static const std::string OPHELIA_NAME;
   static const std::string BIANCA_NAME;
   static const std::string CRESSIDA_NAME;
   static const std::string DESDEMONA_NAME;
   static const std::string JULIET_NAME;
   static const std::string PORTIA_NAME;
   static const std::string ROSALIND_NAME;
   static const std::string BELINDA_NAME;
   static const std::string PUCK_NAME;
   static const std::string MIRANDA_NAME;
   static const std::string ARIEL_NAME;
   static const std::string UMBRIEL_NAME;
   static const std::string TITANIA_NAME;
   static const std::string OBERON_NAME;

   static const std::string NEPTUNE_NAME;
   static const std::string NAIAD_NAME;
   static const std::string THALASSA_NAME;
   static const std::string DESPINA_NAME;
   static const std::string GALATEA_NAME;
   static const std::string LARISSA_NAME;
   static const std::string PROTEUS_NAME;
   static const std::string TRITON_NAME;

   static const std::string PLUTO_NAME;
   static const std::string CHARON_NAME;
   
   // add other moons, asteroids, comets, as needed
   // what do we do about libration points??
   
protected:
   enum
   {
      BODIES_IN_USE = GmatBaseParamCount,
      NUMBER_OF_BODIES,
      OVERRIDE_TIME_SYSTEM,
      EPHEM_UPDATE_INTERVAL,
      SolarSystemParamCount
   };
   
   
   static const std::string PARAMETER_TEXT[SolarSystemParamCount - GmatBaseParamCount];
   
   static const Gmat::ParameterType PARAMETER_TYPE[SolarSystemParamCount - GmatBaseParamCount];
   
    
   Gmat::PosVelSource    pvSrcForAll;
   Gmat::AnalyticMethod  anMethodForAll;
   PlanetaryEphem*       pE;
   bool                  overrideTimeForAll;
   Real                  ephemUpdateInterval;

private:

   // method to find a body in the solar system, given its name
   CelestialBody* FindBody(std::string withName);

   /// list of the celestial bodies that are included in this solar system
   std::list<CelestialBody*> bodiesInUse;

   /// the names of the bodies in use
   StringArray bodyStrings;  // is this needed, or just a convenience?
      
};

#endif // SolarSystem_hpp

