//$Header$
//------------------------------------------------------------------------------
//                                  StringVar
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2005/1/7
//
/**
 * Declares StringVar class which handles std::string value. The string value
 * is stored in Parameter::mExpr.
 */
//------------------------------------------------------------------------------
#ifndef StringVar_hpp
#define StringVar_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"

class GMAT_API StringVar : public Parameter
{
public:

   StringVar(const std::string &name = "",
             const std::string &typeStr = "String",
             GmatParam::ParameterKey key = GmatParam::USER_PARAM,
             GmatBase *obj = NULL, const std::string &desc = "",
             const std::string &unit = "",
             GmatParam::DepObject depObj = GmatParam::NO_DEP,
             Gmat::ObjectType ownerType = Gmat::UNKNOWN_OBJECT,
             bool isTimeParam = false);
   StringVar(const StringVar &copy);
   StringVar& operator= (const StringVar& right);
   virtual ~StringVar();
   
   bool operator==(const StringVar &right) const;
   bool operator!=(const StringVar &right) const;

   // methods inherited from Parameter
   virtual std::string ToString();
   virtual std::string GetString() const;
   virtual std::string EvaluateString();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;
   virtual void Copy(const GmatBase*);
   
   virtual Integer GetParameterID(const std::string &str) const;
   
   virtual bool SetStringParameter(const Integer id, const std::string &value);
   virtual bool SetStringParameter(const std::string &label,
                                   const std::string &value);
   
protected:
   
   enum
   {
      VALUE = ParameterParamCount,
      StringVarParamCount
   };
   
   static const std::string
      PARAMETER_TEXT[StringVarParamCount - ParameterParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[StringVarParamCount - ParameterParamCount];
   
   std::string mStringValue;
   
private:

};
#endif // StringVar_hpp
