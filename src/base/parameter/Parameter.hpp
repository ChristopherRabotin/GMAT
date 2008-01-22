//$Id$
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
#include "Rmatrix.hpp"
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
             bool isTimeParam, bool isSettable, bool isPlottable,
             bool isReportable);
   Parameter(const Parameter &copy);
   Parameter& operator= (const Parameter& right);
   virtual ~Parameter();
   
   GmatParam::ParameterKey GetKey() const;
   Gmat::ObjectType GetOwnerType() const;
   Gmat::ParameterType GetReturnType() const;
   bool IsTimeParameter() const;
   bool IsPlottable() const;
   bool IsReportable() const;
   bool IsSettable() const;
   bool IsCoordSysDependent() const;
   bool IsOriginDependent() const;
   bool NeedCoordSystem() const;
   
   void SetKey(const GmatParam::ParameterKey &key);
   
   bool operator==(const Parameter &right) const;
   bool operator!=(const Parameter &right) const;

   virtual std::string ToString();
   
   virtual Real GetReal() const;
   virtual Rvector6 GetRvector6() const;
   virtual const Rmatrix& GetRmatrix() const;
   virtual std::string GetString() const;
   
   virtual void SetReal(Real val);
   virtual void SetRvector6(const Rvector6 &val);
   virtual void SetRmatrix(const Rmatrix &mat);
   virtual void SetString(const std::string &val);
   
   virtual Real EvaluateReal();
   virtual Rvector6 EvaluateRvector6();
   virtual Rmatrix EvaluateRmatrix();
   virtual std::string EvaluateString();
   
   virtual const std::string* GetParameterList() const;
   
   virtual CoordinateSystem* GetInternalCoordSystem();
   
   virtual void SetInternalCoordSystem(CoordinateSystem *cs);
   virtual void SetSolarSystem(SolarSystem *ss);
   
   virtual bool Initialize();
   virtual bool Evaluate();
   
   // methods all SYSTEM_PARAM should implement
   virtual bool AddRefObject(GmatBase *object, bool replaceName = false);
   virtual Integer GetNumRefObjects() const;
   virtual bool Validate();
   
   // methods inherited from GmatBase
   virtual void        Copy(const GmatBase*);
   virtual bool RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName);
   
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
   
   virtual const std::string  GetCommentLine(Integer which = 1);
   virtual void               SetCommentLine(const std::string &comment,
                                             Integer which = 0);
   
protected:
   
   static const std::string PARAMETER_KEY_STRING[GmatParam::KeyCount];
   
   GmatParam::ParameterKey  mKey;
   std::string   mDesc;
   std::string   mUnit;
   std::string   mExpr;
   std::string   mDepObjectName;
   std::string   mCommentLine2;
   
   Gmat::ObjectType   mOwnerType;
   Gmat::ParameterType mReturnType;
   GmatParam::DepObject mDepObj;
   UnsignedInt   mColor;
   
   bool mIsTimeParam;
   bool mIsPlottable;
   bool mIsReportable;
   bool mIsSettable;
   bool mIsCoordSysDependent;
   bool mIsOriginDependent;
   bool mNeedCoordSystem;
   bool mIsCommentFromCreate;
   
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

