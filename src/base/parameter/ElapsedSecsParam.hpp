//$Header$
//------------------------------------------------------------------------------
//                              ElapsedSecsParam
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/13
//
/**
 * Declares elapsed time in seconds parameter class.
 */
//------------------------------------------------------------------------------
#ifndef ElapsedSecsParam_hpp
#define ElapsedSecsParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "TimeData.hpp"

class GMAT_API ElapsedSecsParam : public RealParameter, TimeData
{
public:

    ElapsedSecsParam(const std::string &name = "",
                     GmatBase *obj = NULL,
                     const std::string &desc = "Spacecraft Elapsed Time in Seconds",
                     const std::string &unit = "Sec");
    ElapsedSecsParam(const ElapsedSecsParam &copy);
    const ElapsedSecsParam& operator= (const ElapsedSecsParam &right); 
    virtual ~ElapsedSecsParam();

    // The inherited methods from RealParameter
    virtual Real EvaluateReal();

    // The inherited methods from Parameter
    virtual bool AddObject(GmatBase *obj);
    virtual Integer GetNumObjects() const;
    virtual bool Validate();
    virtual bool Evaluate();

    // The inherited methods from GmatBase
    virtual Gmat::ParameterType GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;
    virtual std::string GetParameterText(const Integer id);
    virtual Integer GetParameterID(const std::string &str);
    virtual Real GetRealParameter(const Integer id);
    virtual Real GetRealParameter(const std::string &label);
    virtual Real SetRealParameter(const Integer id, const Real value);
    virtual Real SetRealParameter(const std::string &label, const Real value);
    
protected:
    
    enum
    {
        INITIAL_EPOCH = RealParameterCount,
        ElapsedSecsParamCount
    };

    static const Gmat::ParameterType PARAMETER_TYPE[ElapsedSecsParamCount];
    static const std::string PARAMETER_TEXT[ElapsedSecsParamCount];
    
private:
};

#endif // ElapsedSecsParam_hpp
