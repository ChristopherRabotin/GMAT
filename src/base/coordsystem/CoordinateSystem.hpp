//$Header$
//------------------------------------------------------------------------------
//                                  CoordinateSystem
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G and MOMS Task order 124
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2004/12/22
//
/**
 * Definition of the CoordinateSystem class.
 *
 */
//------------------------------------------------------------------------------

#ifndef CoordinateSystem_hpp
#define CoordinateSystem_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CoordinateBase.hpp"
#include "AxisSystem.hpp"
#include "Rvector.hpp"
#include "A1Mjd.hpp"

class GMAT_API CoordinateSystem : public CoordinateBase
{
public:

   // default constructor
   CoordinateSystem(Gmat::ObjectType ofType, const std::string &itsType,
                   const std::string &itsName = "");
   // copy constructor
   CoordinateSystem(const CoordinateSystem &coordSys);
   // operator = for assignment
   const CoordinateSystem& operator=(const CoordinateSystem &coordSys);
   // operator == for equality testing
   const bool operator==(const CoordinateSystem &coordSys);
   // destructor
   virtual ~CoordinateSystem();
   
   // initilaizes the CoordinateSystem
   virtual void Initialize();
   
   // methods to convert between this CoordinateSystem and MJ2000Eq
   virtual Rvector ToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState, 
                            SpacePoint *j2000Body);
   
   virtual Rvector FromMJ2000Eq(const A1Mjd &epoch, const Rvector &inState, 
                              SpacePoint *j2000Body);
   
   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   //virtual GmatBase*       Clone(void) const;

   // Parameter access methods - overridden from GmatBase 
   virtual std::string     GetParameterText(const Integer id) const;     
   virtual Integer         GetParameterID(const std::string &str) const; 
   virtual Gmat::ParameterType
                           GetParameterType(const Integer id) const;
   virtual std::string     GetParameterTypeString(const Integer id) const;
   
   virtual GmatBase*       GetRefObject(const Gmat::ObjectType type,
                                        const std::string &name);
   virtual bool            SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                        const std::string &name = "");
   
protected:

   enum
   {
      AXES = CoordinateBaseParamCount,
      INTERNAL_STATE,   // currently, no access allowed
      CoordinateSystemParamCount
   };
   
   static const std::string PARAMETER_TEXT[
      CoordinateSystemParamCount - CoordinateBaseParamCount];
   
   static const Gmat::ParameterType PARAMETER_TYPE[
      CoordinateSystemParamCount - CoordinateBaseParamCount];
   
   virtual bool TranslateToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState, 
                                  Rvector &outState, SpacePoint *j2000Body);
   
   virtual bool TranslateFromMJ2000Eq(const A1Mjd &epoch, const Rvector &inState, 
                                    Rvector &outState, SpacePoint *j2000Body);
   
   /// axis system
   AxisSystem    *axes;
   /// internal state
   Rvector       internalState;
   
};
#endif // CoordinateSystem_hpp
