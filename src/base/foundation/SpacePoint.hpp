//$Header$
//------------------------------------------------------------------------------
//                                  SpacePoint
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G and MOMS Task order 124
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2004/12/08
//
/**
 * Definition of the SpacePoint class.  This is the base class for SpaceObject
 * (base class for Spacecraft and Formation), CelestialBody (base class for Star,
 * Planet, and Moon), and CalculatedPoint (base class for LibrationPoint and
 * Barycenter)  - any object that can be used as an origin, primary, or
 * secondary when defining a CoordinateSystem.
 *
 * @note Methods returning the point's state information at time atTime return
 * the state in a frame with axes aligned with the MJ2000 Earth Equatorial 
 * Coordinate System but based at the origin j2000Body.
 *
 */
//------------------------------------------------------------------------------

#ifndef SpacePoint_hpp
#define SpacePoint_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "A1Mjd.hpp"
#include "Rvector6.hpp"
#include "Rvector3.hpp"

class GMAT_API SpacePoint : public GmatBase
{
public:

   // default constructor
   SpacePoint(Gmat::ObjectType ofType, const std::string &itsType,
              const std::string &itsName = "");
   // copy constructor
   SpacePoint(const SpacePoint &sp);
   // operator = 
   const SpacePoint& operator=(const SpacePoint &sp);
   // destructor
   virtual ~SpacePoint();
   
   // methods for accessing the bodyName or body pointer
   const std::string  GetJ2000BodyName() const;
   SpacePoint*        GetJ2000Body() const;
   bool               SetJ2000BodyName(const std::string &toName);
   void               SetJ2000Body(SpacePoint* toBody);
   
   //---------------------------------------------------------------------------
   //  Rvector6& GetMJ2000State(const A1Mjd &atTime)
   //---------------------------------------------------------------------------
   /**
    * Method returning a reference to an Rvector6 containing the MJ2000 state
    * of the SpacePoint at the time atTime.
    *
    * @param <atTime> Time for which the state is requested.
    *
    * @return state of the SpacePoint at time atTime.
    *
    * @note This method is pure virtual and must be implemented by the 
    *       'leaf' (non-abstract derived) classes.
    */
   //---------------------------------------------------------------------------
   virtual const Rvector6& GetMJ2000State(const A1Mjd &atTime)    = 0;

   //---------------------------------------------------------------------------
   //  Rvector3& GetMJ2000Position(const A1Mjd &atTime)
   //---------------------------------------------------------------------------
   /**
    * Method returning a reference to an Rvector3 containing the MJ2000 position
    * of the SpacePoint at the time atTime.
    *
    * @param <atTime> Time for which the position is requested.
    *
    * @return position of the SpacePoint at time atTime.
    *
    * @note This method is pure virtual and must be implemented by the 
    *       'leaf' (non-abstract derived) classes.
    */
   //---------------------------------------------------------------------------
   virtual const Rvector3& GetMJ2000Position(const A1Mjd &atTime) = 0;

   //---------------------------------------------------------------------------
   //  Rvector3& GetMJ2000Velocity(const A1Mjd &atTime)
   //---------------------------------------------------------------------------
   /**
    * Method returning a reference to an Rvector3 containing the MJ2000 velocity
    * of the SpacePoint at the time atTime.
    *
    * @param <atTime> Time for which the velocity is requested.
    *
    * @return velocity of the SpacePoint at time atTime.
    *
    * @note This method is pure virtual and must be implemented by the 
    *       'leaf' (non-abstract derived) classes.
    */
   //---------------------------------------------------------------------------
   virtual const Rvector3& GetMJ2000Velocity(const A1Mjd &atTime) = 0;
   
   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   //virtual GmatBase*       Clone(void) const;

   // Parameter access methods - overridden from GmatBase 
   virtual std::string     GetParameterText(const Integer id) const;     
   virtual Integer         GetParameterID(const std::string &str) const; 
   virtual Gmat::ParameterType
                           GetParameterType(const Integer id) const;
   virtual std::string     GetParameterTypeString(const Integer id) const;
   
   virtual std::string     GetStringParameter(const Integer id) const;
   virtual bool            SetStringParameter(const Integer id, 
                                              const std::string &value);
   virtual std::string     GetStringParameter(const std::string &label) const;
   virtual bool            SetStringParameter(const std::string &label, 
                                              const std::string &value);
   virtual GmatBase*       GetRefObject(const Gmat::ObjectType type,
                                        const std::string &name);
   virtual bool            SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                        const std::string &name = "");
   
   // DJC Added, 12/16/04
   virtual std::string     GetStringParameter(const Integer id, 
                                              const Integer index) const;
   virtual bool            SetStringParameter(const Integer id, 
                                              const std::string &value, 
                                              const Integer index);
   virtual std::string     GetStringParameter(const std::string &label, 
                                              const Integer index) const;
   virtual bool            SetStringParameter(const std::string &label, 
                                              const std::string &value, 
                                              const Integer index);
   virtual GmatBase*       GetRefObject(const Gmat::ObjectType type,
                                        const std::string &name, 
                                        const Integer index);
   virtual bool            SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                        const std::string &name, 
                                        const Integer index);
   
   
protected:

   enum
   {
      J2000_BODY_NAME = GmatBaseParamCount,
      SpacePointParamCount
   };
   
   static const std::string PARAMETER_TEXT[SpacePointParamCount - GmatBaseParamCount];
   
   static const Gmat::ParameterType PARAMETER_TYPE[SpacePointParamCount - GmatBaseParamCount];
   
   
   /// Origin for the return coordinate system (aligned with the MJ2000 Earth
   /// Equatorial coordinate system)
   SpacePoint      *j2000Body;  
   /// Name for the J2000 body
   std::string     j2000BodyName;
   
};
#endif // SpacePoint_hpp
