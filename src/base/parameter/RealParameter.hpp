//$Header$
//------------------------------------------------------------------------------
//                                  RealParameter
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
#ifndef RealParameter_hpp
#define RealParameter_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"

class GMAT_API RealParameter : public Parameter
{
public:

    RealParameter(const std::string &name, const std::string &typeStr,
                  ParameterKey key, GmatBase *obj,
                  const std::string &desc, const std::string &unit,
                  bool isTimeParam);
    RealParameter(const RealParameter &param);
    RealParameter& operator= (const RealParameter& right);
    virtual ~RealParameter();

    bool operator==(const RealParameter &right) const;
    bool operator!=(const RealParameter &right) const;

    virtual Real EvaluateReal() = 0;
    virtual Real GetReal() const;
    
    // The inherited methods from Parameter
    virtual const std::string* GetParameterList() const;

    // The inherited methods from GmatBase
    virtual Gmat::ParameterType GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;
    virtual std::string GetParameterText(const Integer id);
    virtual Integer GetParameterID(const std::string str);
    virtual Real GetRealParameter(const Integer id);
    virtual Real GetRealParameter(const std::string &label);
    virtual Real SetRealParameter(const Integer id, const Real value);
    virtual Real SetRealParameter(const std::string &label, const Real value);
    
protected:

    Real mValue;
    
    enum
    {
        PARAM_1 = 0,
        RealParameterCount
    };
    
    static const Gmat::ParameterType PARAMETER_TYPE[RealParameterCount];
    static const std::string PARAMETER_TEXT[RealParameterCount];
    
private:

};
#endif // RealParameter_hpp
