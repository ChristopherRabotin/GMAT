//$Header$
//------------------------------------------------------------------------------
//                                  CoordinateBase
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2004/12/20
//
/**
 * Definition of the CoordinateBase class.  This is the base class for the 
 * CoordinateSystem and AxisSystem classes.
 *
 */
//------------------------------------------------------------------------------

#ifndef CoordinateBase_hpp
#define CoordinateBase_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "SolarSystem.hpp"
#include "SpacePoint.hpp"

class GMAT_API CoordinateBase : public GmatBase
{
public:

   // default constructor
   CoordinateBase(Gmat::ObjectType ofType, const std::string &itsType,
                  const std::string &itsName = "");
   // copy constructor
   CoordinateBase(const CoordinateBase &coordBase);
   // operator = 
   const CoordinateBase& operator=(const CoordinateBase &coordBase);
   // destructor
   virtual ~CoordinateBase();
   
   virtual void                SetSolarSystem(SolarSystem *ss);
   virtual void                SetOriginName(const std::string &toName);
   virtual void                SetOrigin(SpacePoint *originPtr);
   virtual void                SetJ2000BodyName(const std::string &toName);
   virtual void                SetJ2000Body(SpacePoint *j2000Ptr);
   virtual std::string         GetOriginName() const;
   virtual SpacePoint*         GetOrigin() const;
   virtual std::string         GetJ2000BodyName() const;
   virtual SpacePoint*         GetJ2000Body() const;
   
   // initializes the CoordinateBase
   virtual void Initialize();
   
   
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
   
   
protected:

   enum
   {
      ORIGIN_NAME = GmatBaseParamCount,
      J2000_BODY_NAME,
      CoordinateBaseParamCount
   };
   
   static const std::string PARAMETER_TEXT[CoordinateBaseParamCount - GmatBaseParamCount];
   
   static const Gmat::ParameterType PARAMETER_TYPE[CoordinateBaseParamCount - GmatBaseParamCount];
   
   
   /// Origin for the return coordinate system (aligned with the MJ2000 Earth
   /// Equatorial coordinate system)
   SpacePoint      *origin;  
   /// Name for the origin body
   std::string     originName;
   /// j2000Body for the system
   SpacePoint      *j2000Body;  
   /// Name for the origin body
   std::string     j2000BodyName;
   /// pointer to the solar system
   SolarSystem     *solar;
   
};
#endif // CoordinateBase_hpp
