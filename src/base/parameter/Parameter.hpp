//$Header$
//------------------------------------------------------------------------------
//                                  Parameter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/09/16
//
/**
 * Declares base class of parameters.
 */
//------------------------------------------------------------------------------
#ifndef Parameter_hpp
#define Parameter_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Rvector6.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"

namespace GmatParam
{
   enum ParameterKey
   {
      SYSTEM_PARAM, USER_PARAM, KeyCount
   };
   
   enum DepObject
   {
      COORD_SYS, ORIGIN, NO_DEP, DepObjectCount
   };
};

class GMAT_API Parameter : public GmatBase
{
public:

   Parameter(const std::string &name, const std::string &typeStr,
             GmatParam::ParameterKey key, GmatBase *obj,
             const std::string &desc, const std::string &unit,
             GmatParam::DepObject depObj, Gmat::ObjectType ownerType,
             bool isTimeParam);
   Parameter(const Parameter &copy);
   Parameter& operator= (const Parameter& right);
   virtual ~Parameter();
   
   GmatParam::ParameterKey GetKey() const;
   Gmat::ObjectType GetOwnerType() const;
   bool IsTimeParameter() const;
   bool IsPlottable() const;
   bool IsCoordSysDependent() const;
   bool IsOriginDependent() const;
   bool NeedCoordSystem() const;
   
   void SetKey(const GmatParam::ParameterKey &key);
   
   bool operator==(const Parameter &right) const;
   bool operator!=(const Parameter &right) const;

   virtual std::string ToString();
   
   virtual Real GetReal() const;
   virtual Rvector6 GetRvector6() const;
   
   virtual void SetReal(Real val);
   virtual void SetRvector6(const Rvector6 &val);
   
   virtual Real EvaluateReal();
   virtual Rvector6 EvaluateRvector6();
   
   virtual const std::string* GetParameterList() const;
   
   //loj: 1/26/05 Added GetInternalCoordSystem(), SetInternalCoordSystem()
   virtual CoordinateSystem* GetInternalCoordSystem();
   
   virtual void SetInternalCoordSystem(CoordinateSystem *cs);
   virtual void SetSolarSystem(SolarSystem *ss);
   
   virtual bool Initialize();
   virtual bool Evaluate();
   
   // methods all SYSTEM_PARAM should implement
   virtual bool AddRefObject(GmatBase *object);
   virtual Integer GetNumRefObjects() const;
   virtual bool Validate();
   
   // DJC added 2/17/05 to enable var1 = var2
   virtual void        Copy(const GmatBase*);
   
   // methods inherited from GmatBase
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual bool IsParameterReadOnly(const Integer id) const;

   virtual UnsignedInt GetUnsignedIntParameter(const Integer id) const;
   virtual UnsignedInt GetUnsignedIntParameter(const std::string &label) const;
   virtual UnsignedInt SetUnsignedIntParameter(const Integer id,
                                               const UnsignedInt value);
   virtual UnsignedInt SetUnsignedIntParameter(const std::string &label,
                                               const UnsignedInt value);

   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool SetStringParameter(const Integer id, const std::string &value);
   virtual bool SetStringParameter(const std::string &label,
                                   const std::string &value);
protected:
   
   static const std::string PARAMETER_KEY_STRING[GmatParam::KeyCount];

   GmatParam::ParameterKey  mKey;
   std::string   mDesc;
   std::string   mUnit;
   std::string   mExpr;
   std::string   mDepObjectName;
   
   Gmat::ObjectType   mOwnerType;
   UnsignedInt   mColor;
   
   bool mIsTimeParam;
   bool mIsPlottable;
   bool mIsCoordSysDependent;
   bool mIsOriginDependent;
   bool mNeedCoordSystem;
   
   enum
   {
      OBJECT = GmatBaseParamCount,
      EXPRESSION,
      DESCRIPTION,
      UNIT,
      DEP_OBJECT,
      COLOR,
      ParameterParamCount
   };

   static const Gmat::ParameterType
      PARAMETER_TYPE[ParameterParamCount - GmatBaseParamCount];
   static const std::string
      PARAMETER_TEXT[ParameterParamCount - GmatBaseParamCount];
};
#endif // Parameter_hpp

