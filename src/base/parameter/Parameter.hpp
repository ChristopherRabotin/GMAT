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

   virtual StringArray& GetObjectTypeNames();
   virtual StringArray& GetObjectNames();
   virtual GmatBase* GetObject(const std::string &objTypeName);
   //virtual GmatBase* GetObject(Gmat::ObjectType objType,
   //                            const std::string &objName);
   virtual bool SetObject(Gmat::ObjectType objType,
                          const std::string &objName,
                          GmatBase *obj);
   
   virtual void SetSolarSystem(SolarSystem *ss); //loj: 6/24/04 added
   
   // methods all SYSTEM_PARAM should implement
   virtual bool AddObject(const std::string &name); //loj: 5/26/04 will be removed
   virtual bool AddObject(GmatBase *object);
   //virtual bool AddObject(const std::string &objType,
   //                       const std::string &objName, GmatBase *object);
   virtual Integer GetNumObjects() const;
   virtual bool Evaluate();
   virtual bool Validate();
   virtual void Initialize();
   
   // methods inherited from GmatBase
   virtual std::string GetRefObjectName(const Gmat::ObjectType type) const;
   virtual bool SetRefObjectName(const Gmat::ObjectType type,
                                 const std::string &name);
   virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name);
   virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name = "");

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

   void ManageObject(GmatBase *obj);
    
   static const std::string PARAMETER_KEY_STRING[KeyCount];

   ParameterKey  mKey;
   std::string   mDesc;
   std::string   mUnit;
   std::string   mExpr;
   
   UnsignedInt   mColor; //loj: 5/26/04 use UnsignedInt instead of string
   
   bool mIsTimeParam;
   bool mIsPlottable; //loj: 5/26/04 added
   
   StringArray mObjectTypeNames;
   StringArray mObjectNames;
   Integer mNumObjects;

   enum
   {
      OBJECT = GmatBaseParamCount,
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

