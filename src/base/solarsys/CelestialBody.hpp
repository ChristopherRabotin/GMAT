//$Header$
//------------------------------------------------------------------------------
//                                  CelestialBody
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
 * This is the base class for celestial bodies.
 *
 * @note This is an abstract class.
 * @note Class (initial shell only) automatically generated by Dev-C++ New
 *       Class wizard (heavily modified after that)
 */
//------------------------------------------------------------------------------

#ifndef CelestialBody_hpp
#define CelestialBody_hpp

#include <math.h>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "A1Mjd.hpp"
#include "PlanetaryEphem.hpp"
#include "AtmosphereManager.hpp"
#include "AtmosphereModel.hpp"
#include "Rmatrix.hpp"
#include "Rvector6.hpp"

// Add needed things to Gmat namespace
namespace Gmat
{
   // possible sources of position and velocity data for celestial bodies
   enum PosVelSource
   {
      ANALYTIC = 0,
      SLP,
//      DE_102,
      DE_200,
//      DE_202,
//      DE_403,
      DE_405,
//      DE_406,
//      EPHEMERIS,           // do we need more?
      PosVelSourceCount
   };

   // if using an analytical method, which one?
   enum AnalyticMethod
   {
      NO_ANALYTIC_METHOD = 0,
      TWO_BODY,
      EARTH_ANALYTIC,
      MOON_ANALYTIC,
      NUM_ANALYTIC,
      AnalyticMethodCount
   };

   // possible types of celestial bodies
   enum BodyType
   {
      STAR = 0,
      PLANET,
      MOON,
      ASTEROID,
      COMET,
      BodyTypeCount
   };
};

/**
 * CelestialBody base class, from which all types of celestial bodies will derive.
 *
 * The CelestialBody class is primarily a base class, from which all types of
 * celestial bodies will derive.  CelestialBody itself derives from GmatBase.
 */
class GMAT_API CelestialBody : public GmatBase
{
public:
   // additional constructor, specifying body type (as string) and name
   CelestialBody(std::string itsBodyType, std::string name);
   // additional constructor, specifying type (as Gmat::BodyType) and name
   CelestialBody(Gmat::BodyType itsBodyType, std::string name);
   // copy constructor
   CelestialBody(const CelestialBody &cb);
   // operator=
   CelestialBody& operator=(const CelestialBody &cb);
   // destructor
   virtual ~CelestialBody();

   // method to return the state (position and velocity) of the body at
   // the specified time, using the specified method
   virtual const Rvector6&      GetState(A1Mjd atTime);
   virtual const Rvector6&      GetState(Real atTime); 
   
   // methods to return the body type, central body, gravitational constant,
   // radius, mass, posvel source, and analytic method 
   virtual Gmat::BodyType       GetBodyType() const;
   virtual const std::string&   GetCentralBody() const;
   virtual Real                 GetGravitationalConstant();
   virtual Real                 GetEquatorialRadius();
   virtual Real                 GetFlattening() const;
   virtual Real                 GetPolarRadius();
   virtual Real                 GetMass();
   virtual Gmat::PosVelSource   GetPosVelSource() const;
   virtual Gmat::AnalyticMethod GetAnalyticMethod() const;
   virtual bool                 GetUsePotentialFile() const;
   virtual const Rvector3&      GetAngularVelocity();             // rad/sec
   // get the Hour Angle, in degrees, from Prime Meridian westward
   // Note: Mean Sidereal Time is -HourAngle
   virtual Real                 GetHourAngle(A1Mjd atTime); 
   //virtual const Rmatrix&       GetHarmonicCoefficientsSij(); // const??
   //virtual const Rmatrix&       GetHarmonicCoefficientsCij(); // const??
   //virtual const Rmatrix&       GetCoefDriftS();
   //virtual const Rmatrix&       GetCoefDriftC();
   //virtual Integer              GetDegree();
   //virtual Integer              GetOrder();
   virtual const StringArray&   GetSupportedAtmospheres() const;
   virtual std::string          GetAtmosphereModelType();
   virtual AtmosphereModel*     GetAtmosphereModel(const std::string& type = "");
   virtual bool                 GetDensity(Real *position, Real *density,
                                        Real epoch = 21545.0,
                                        Integer count = 1);
   

   // methods to return the body type, central body,
   // posvel source, and analytic method
   virtual bool           SetBodyType(Gmat::BodyType bType);
   virtual bool           SetCentralBody(const std::string &cBody);
   virtual bool           SetGravitationalConstant(Real newMu);
   virtual bool           SetEquatorialRadius(Real newEqRadius);
   virtual bool           SetFlattening(Real flat);
   //virtual bool           SetPolarRadius(Real newPolarRadius);
   //virtual bool           SetMass(Real newMass);
   virtual bool           SetSource(Gmat::PosVelSource pvSrc);
   virtual bool           SetSourceFile(PlanetaryEphem *src);
   virtual bool           SetAnalyticMethod(Gmat::AnalyticMethod aM);
   virtual bool           SetUsePotentialFile(bool useIt);
   
   virtual bool           SetAtmosphereModel(std::string toAtmModel);
   virtual bool           SetPotentialFilename(const std::string &fn);
   //virtual bool           SetPhysicalParameters(Real bodyMass, Real bodyEqRad,
   //                                             Real bodyPolarRad, Real bodyMu,
   //                                             Integer coeffSize, Rmatrix& bodySij,
   //                                             Rmatrix& bodyCij);
   
   // Parameter access methods - overridden from GmatBase
   virtual std::string    GetParameterText(const Integer id) const;     // const?
   virtual Integer        GetParameterID(const std::string &str) const; // const?
   virtual Gmat::ParameterType
                          GetParameterType(const Integer id) const;
   virtual std::string    GetParameterTypeString(const Integer id) const;

   virtual Real           GetRealParameter(const Integer id) const;
   virtual Real           SetRealParameter(const Integer id,
                                           const Real value);
   virtual Integer        GetIntegerParameter(const Integer id) const; // const?
   virtual Integer        SetIntegerParameter(const Integer id,
                                              const Integer value); // const?
   virtual std::string    GetStringParameter(const Integer id) const; // const?
   virtual bool           SetStringParameter(const Integer id, 
                                             const std::string &value); // const?
   virtual bool           GetBooleanParameter(const Integer id) const; // const?
   virtual bool           SetBooleanParameter(const Integer id,
                                              const bool value); // const?
   virtual const Rvector&    GetRvectorParameter(const Integer id) const;
   virtual const Rvector&    SetRvectorParameter(const Integer id,
                                                 const Rvector &value);
   virtual const Rvector&    GetRvectorParameter(const std::string &label) const;
   virtual const Rvector&    SetRvectorParameter(const std::string &label,
                                                 const Rvector &value);
   //virtual const Rmatrix&    GetRmatrixParameter(const Integer id) const;
   //virtual const Rmatrix&    SetRmatrixParameter(const Integer id,
   //                                              const Rmatrix &value);
   //virtual const Rmatrix&    GetRmatrixParameter(const std::string &label) const;
   //virtual const Rmatrix&    SetRmatrixParameter(const std::string &label,
   //                                              const Rmatrix &value);
   virtual const StringArray& GetStringArrayParameter(const Integer id) const;

   // need methods to get/set stateTime (a1MJD type)?

   
   //------------------------------------------------------------------------------
   // virtual CelestialBody* Clone(void) const
   //------------------------------------------------------------------------------
   /**
     * Method used to create a copy of the object
     * Each class that can be instantiated provides an implementation of this
     * method so that copies can be made from a base class pointer.
     */
   //------------------------------------------------------------------------------
   //virtual CelestialBody* Clone(void) const = 0;

   // strings representing the possible celestial body types
   static const std::string BODY_TYPE_STRINGS[Gmat::BodyTypeCount];
   static const std::string POS_VEL_STRINGS[Gmat::PosVelSourceCount];
   static const std::string ANALYTIC_METHOD_STRINGS[Gmat::AnalyticMethodCount];

   // local constants
   //static const Integer MAX_DEGREE            = 360;
   //static const Integer MAX_ORDER             = 360;
   //static const Integer GRAV_MAX_DRIFT_DEGREE = 2;
   static const Integer BUFSIZE               = 256;

protected:

   enum
   {
      BODY_TYPE = GmatBaseParamCount, 
      MASS,
      EQUATORIAL_RADIUS,
      FLATTENING,
      POLAR_RADIUS,
      MU,
      POS_VEL_SOURCE,
      ANALYTIC_METHOD,
      STATE,
      STATE_TIME,
      //ORDER,
      //DEGREE,
      CENTRAL_BODY,
      BODY_NUMBER,
      REF_BODY_NUMBER,
      SOURCE_FILENAME,
      SOURCE_FILE,
      USE_POTENTIAL_FILE_FLAG,
      POTENTIAL_FILE_NAME,
      ANGULAR_VELOCITY,
      //COEFFICIENT_SIZE,
      //SIJ,
      //CIJ,
      HOUR_ANGLE,
      ATMOS_MODEL_NAME,
      SUPPORTED_ATMOS_MODELS,
      CelestialBodyParamCount
   };
   static const std::string PARAMETER_TEXT[CelestialBodyParamCount - GmatBaseParamCount];

   static const Gmat::ParameterType PARAMETER_TYPE[CelestialBodyParamCount - GmatBaseParamCount];
   
   // body type of the body
   Gmat::BodyType         bodyType;
   // mass
   Real                   mass;
   /// radius of the body at the equator
   Real                   equatorialRadius;
   /// flattening - used to compute polar radius
   Real                   flattening;
   /// radius of the body from center to the pole
   Real                   polarRadius;
   /// gravitational constant in km^3/s^2
   Real                   mu;
   /// source for position and velocity
   Gmat::PosVelSource     posVelSrc;
   /// analytic method to use
   Gmat::AnalyticMethod   analyticMethod;
   /// state of the body 0-2 position 3-5 velocity
   Rvector6               state ;
   // time of the state
   A1Mjd                  stateTime;

   /// central body around which this body revolves
   std::string            centralBody;
   /// body number for the SLP file
   Integer                bodyNumber;
   /// body number of origin of coordinate system for file
   Integer                referenceBodyNumber;
   /// name of file that is the source of position and velocity for this body
   std::string            sourceFilename;
   // the source file
   PlanetaryEphem*        theSourceFile;

   bool                   usePotentialFile;
   std::string            potentialFileName;
   Rvector3               angularVelocity;
   Real                   hourAngle;
   AtmosphereManager*     atmManager;
   AtmosphereModel*       atmModel;

   // has the potential file been read already?
   bool                   potentialFileRead;

   // defaults if potential file is not used
   Real                   defaultMu;
   Real                   defaultEqRadius;
   
   // order of the gravity model
   //Integer                order;     // are these the same as coefficientSize?
   // degree of the gravity model
   //Integer                degree;     // are these the same as coefficientSize?
   /// date and time of start of source file
   //A1Mjd                  sourceStart;      // currently unused
   /// date and time of end of sourcce file
   //A1Mjd                  sourceEnd;        // currently unused
   //Integer                coefficientSize;      // n   // same as degree, order above?
   //Rmatrix                Cbar, Sbar;
   //Rmatrix                dCbar, dSbar; // from original GravityField
   //Rmatrix                sij;                  // n x n
   //Rmatrix                cij;                  // n x n
   
   // initialze the body
   void Initialize(std::string withBodyType = "Planet");
   // read the potential file, if requested
   bool          ReadPotentialFile();
   bool          ReadCofFile();
   bool          ReadGrvFile();
   bool          ReadDatFile();
   //bool          ReadCofFile(Integer& fileDeg, Integer& fileOrd);
   //bool          ReadGrvFile(Integer& fileDeg, Integer& fileOrd);
   //bool          ReadDatFile(Integer& fileDeg, Integer& fileOrd);
   
   bool          IsBlank(char* aLine);
   
   //Integer       defaultCoefSize;
   //Rmatrix       defaultSij;
   //Rmatrix       defaultCij;

private:

};
#endif // CelestialBody_hpp

