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
 * Declares base class of Rvector6 parameters.
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
             ParameterKey key = USER_PARAM,
             GmatBase *obj = NULL,
             const std::string &desc = "",
             const std::string &unit = "",
             bool isTimeParam = false);
    Rvec6Var(const Rvec6Var &copy);
    Rvec6Var& operator= (const Rvec6Var& right);
    virtual ~Rvec6Var();

    bool operator==(const Rvec6Var &right) const;
    bool operator!=(const Rvec6Var &right) const;

    virtual Rvector6 EvaluateRvector6();
    virtual Rvector6 GetRvector6() const;

    // The inherited methods from Parameter
    virtual const std::string* GetParameterList() const;
    
    // The inherited methods from GmatBase
    virtual Gmat::ParameterType GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;
    virtual std::string GetParameterText(const Integer id);
    virtual Integer GetParameterID(const std::string str);
    virtual Real GetRealParameter(const Integer id);
    virtual Real SetRealParameter(const Integer id, const Real value);

protected:
    
    enum
    {
        PARAM_1 = ParameterParamCount,
        PARAM_2,
        PARAM_3,
        PARAM_4,
        PARAM_5,
        PARAM_6,
        Rvec6VarParamCount
    };

    Rvector6 mValue;
        
    static const Gmat::ParameterType PARAMETER_TYPE[Rvec6VarParamCount];
    static const std::string PARAMETER_TEXT[Rvec6VarParamCount];
    
private:

};
#endif // Parameter_hpp
