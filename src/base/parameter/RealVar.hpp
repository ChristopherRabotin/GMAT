//$Header$
//------------------------------------------------------------------------------
//                                  RealVar
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/08
//
/**
 * Declares base class of parameters.
 */
//------------------------------------------------------------------------------
#ifndef RealVar_hpp
#define RealVar_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"

class GMAT_API RealVar : public Parameter
{
public:

    RealVar(const std::string &name = "",
            const std::string &typeStr = "RealVar",
            ParameterKey key = USER_PARAM,
            GmatBase *obj = NULL,
            const std::string &desc = "",
            const std::string &unit = "",
            bool isTimeParam = false);
    RealVar(const RealVar &copy);
    RealVar& operator= (const RealVar& right);
    virtual ~RealVar();

    bool operator==(const RealVar &right) const;
    bool operator!=(const RealVar &right) const;

    virtual Real EvaluateReal() = 0;
    virtual Real GetReal() const;
    
    // The inherited methods from Parameter
    virtual const std::string* GetParameterList() const;

    // The inherited methods from GmatBase
    virtual Gmat::ParameterType GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;
    virtual std::string GetParameterText(const Integer id) const;
    virtual Integer GetParameterID(const std::string str) const; //loj: 3/25 const was missing
    virtual Real GetRealParameter(const std::string &label) const;
    virtual Real GetRealParameter(const Integer id) const;
    virtual Real SetRealParameter(const Integer id, const Real value);
    virtual Real SetRealParameter(const std::string &label, const Real value);
    
protected:

    Real mRealValue;
    
    enum
    {
        PARAM_1 = ParameterParamCount,
        RealVarParamCount
    };
    
    static const Gmat::ParameterType PARAMETER_TYPE[RealVarParamCount - ParameterParamCount];
    static const std::string PARAMETER_TEXT[RealVarParamCount - ParameterParamCount];
    
private:

};
#endif // RealVar_hpp
