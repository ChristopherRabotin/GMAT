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

class GMAT_API Parameter : public GmatBase
{
public:

   enum ParameterKey
   {
      SYSTEM_PARAM, USER_PARAM, KeyCount
   };

   Parameter(const std::string &name, const std::string &typeStr,
             ParameterKey key, GmatBase *obj, const std::string &desc,
             const std::string &unit, bool isTimeParam);
   Parameter(const Parameter &copy);
   Parameter& operator= (const Parameter& right);
   virtual ~Parameter();

   ParameterKey GetKey() const;
   bool IsTimeParameter() const;
   bool IsPlottable() const;
   
   void SetKey(const ParameterKey &key);
    
   bool operator==(const Parameter &right) const;
   bool operator!=(const Parameter &right) const;

   virtual std::string ToString(); //loj: 9/7/04 added
   
   virtual Real GetReal();
   virtual Rvector6 GetRvector6();
   
   virtual Real EvaluateReal();
   virtual Rvector6 EvaluateRvector6();
   
   virtual const std::string* GetParameterList() const;
  
   virtual void SetSolarSystem(SolarSystem *ss);
   
   //================== loj: 9/13/04 obsolete ===================
   //virtual GmatBase* GetObject(const std::string &objTypeName);
   //virtual bool SetObject(Gmat::ObjectType objType,
   //                       const std::string &objName,
   //                       GmatBase *obj);
   //virtual bool AddObject(const std::string &name);
   //============================================================

   virtual void Initialize();
   virtual bool Evaluate();
   
   // methods all SYSTEM_PARAM should implement
   virtual bool AddRefObject(GmatBase *object);
   virtual Integer GetNumRefObjects() const;
   virtual bool Validate();
   
   
   // methods inherited from GmatBase
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

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
   
   static const std::string PARAMETER_KEY_STRING[KeyCount];

   ParameterKey  mKey;
   std::string   mDesc;
   std::string   mUnit;
   std::string   mExpr;
   
   UnsignedInt   mColor;
   
   bool mIsTimeParam;
   bool mIsPlottable;
   
   enum
   {
      OBJECT = GmatBaseParamCount, //loj: this will be removed eventually
      SPACECRAFT, //loj: 9/13/04 added
      EXPRESSION,
      DESCRIPTION,
      UNIT,
      COLOR,
      ParameterParamCount
   };

   static const Gmat::ParameterType
      PARAMETER_TYPE[ParameterParamCount - GmatBaseParamCount];
   static const std::string
      PARAMETER_TEXT[ParameterParamCount - GmatBaseParamCount];
};
#endif // Parameter_hpp

