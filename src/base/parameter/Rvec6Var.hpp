//$Header$
//------------------------------------------------------------------------------
//                                  Rvec6Var
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/03/12
//
/**
 * Declares base class of parameters returning Rvector6.
 */
//------------------------------------------------------------------------------
#ifndef Rvec6Var_hpp
#define Rvec6Var_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"
#include "Rvector6.hpp"

class GMAT_API Rvec6Var : public Parameter
{
public:

   Rvec6Var(const std::string &name = "",
            const std::string &typeStr = "Rvec6Var",
            GmatParam::ParameterKey key = GmatParam::USER_PARAM,
            GmatBase *obj = NULL, const std::string &desc = "",
            const std::string &unit = "",
            GmatParam::DepObject depObj = GmatParam::NO_DEP,
            Gmat::ObjectType ownerType = Gmat::UNKNOWN_OBJECT);
   Rvec6Var(const Rvec6Var &copy);
   Rvec6Var& operator= (const Rvec6Var& right);
   virtual ~Rvec6Var();
   
   bool operator==(const Rvec6Var &right) const;
   bool operator!=(const Rvec6Var &right) const;
   
   virtual std::string ToString();
   
   virtual Rvector6 GetRvector6() const;
   virtual Rvector6 EvaluateRvector6();
   
   // methods inherited from Parameter
   virtual const std::string* GetParameterList() const;
   
   // methods inherited from GmatBase
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   
   virtual Real GetRealParameter(const Integer id) const;
   virtual Real SetRealParameter(const Integer id, const Real value);

protected:
    
   enum
   {
      VALUE1 = ParameterParamCount,
      VALUE2,
      VALUE3,
      VALUE4,
      VALUE5,
      VALUE6,
      Rvec6VarParamCount
   };

   Rvector6 mRvec6Value;
        
   static const Gmat::ParameterType
   PARAMETER_TYPE[Rvec6VarParamCount - ParameterParamCount];
   static const std::string
   PARAMETER_TEXT[Rvec6VarParamCount - ParameterParamCount];
    
private:

};
#endif // Parameter_hpp
