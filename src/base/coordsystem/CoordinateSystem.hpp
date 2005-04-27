//$Header$
//------------------------------------------------------------------------------
//                                  CoordinateSystem
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
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
   CoordinateSystem(const std::string &itsType,
                    const std::string &itsName = "");
   // copy constructor
   CoordinateSystem(const CoordinateSystem &coordSys);
   // operator = for assignment
   const CoordinateSystem& operator=(const CoordinateSystem &coordSys);
   // operator == for equality testing
   const bool operator==(const CoordinateSystem &coordSys);
   // destructor
   virtual ~CoordinateSystem();

   // virtual methods to check to see how/if an AxisSystem uses 
   // a particular parameter (calls through to its AxisSystem)
   virtual GmatCoordinate::ParameterUsage UsesEopFile() const;
   virtual GmatCoordinate::ParameterUsage UsesItrfFile() const;
   virtual GmatCoordinate::ParameterUsage UsesEpoch() const;
   virtual GmatCoordinate::ParameterUsage UsesPrimary() const;
   virtual GmatCoordinate::ParameterUsage UsesSecondary() const;
   virtual GmatCoordinate::ParameterUsage UsesXAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesYAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesZAxis() const;
   // methods to set parameters for the AxisSystems
   virtual void                  SetPrimaryObject(SpacePoint *prim);
   virtual void                  SetSecondaryObject(SpacePoint *second);
   virtual void                  SetEpoch(const A1Mjd &toEpoch);
   virtual void                  SetXAxis(const std::string &toValue);
   virtual void                  SetYAxis(const std::string &toValue);
   virtual void                  SetZAxis(const std::string &toValue);
   virtual void                  SetEopFile(EopFile *eopF);
   virtual void                  SetCoefficientsFile(
                                    ItrfCoefficientsFile *itrfF);

   virtual SpacePoint*           GetPrimaryObject() const;
   virtual SpacePoint*           GetSecondaryObject() const;
   virtual A1Mjd                 GetEpoch() const;
   virtual std::string           GetXAxis() const;
   virtual std::string           GetYAxis() const;
   virtual std::string           GetZAxis() const;
   virtual EopFile*              GetEopFile() const;
   virtual ItrfCoefficientsFile* GetItrfCoefficientsFile();
   
   
   // initializes the CoordinateSystem
   virtual bool Initialize();
   
   // methods to convert between this CoordinateSystem and MJ2000Eq
   virtual Rvector ToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState, 
                              bool coincident = false);  // j2000Body?
   
   virtual Rvector FromMJ2000Eq(const A1Mjd &epoch, const Rvector &inState, 
                                bool coincident = false);   // j2000Body?
   
   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone() const;

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
      //INTERNAL_STATE,   // currently, no access allowed
      CoordinateSystemParamCount
   };
   
   static const std::string PARAMETER_TEXT[
      CoordinateSystemParamCount - CoordinateBaseParamCount];
   
   static const Gmat::ParameterType PARAMETER_TYPE[
      CoordinateSystemParamCount - CoordinateBaseParamCount];
   
   virtual bool TranslateToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState, 
                                    Rvector &outState);
   // Rvector &outState, SpacePoint *j2000Body);
   
   virtual bool TranslateFromMJ2000Eq(const A1Mjd &epoch, const Rvector &inState, 
                                      Rvector &outState);
   // Rvector &outState, SpacePoint *j2000Body);
   
   /// axis system
   AxisSystem    *axes;
   
};
#endif // CoordinateSystem_hpp
