//$Header$
//------------------------------------------------------------------------------
//                              ElapsedDaysParam
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
 * Declares elapsed time in days parameter class.
 */
//------------------------------------------------------------------------------
#ifndef ElapsedDaysParam_hpp
#define ElapsedDaysParam_hpp

#include "gmatdefs.hpp"
#include "RealParameter.hpp"
#include "TimeData.hpp"

class GMAT_API ElapsedDaysParam : public RealParameter, TimeData
{
public:

    ElapsedDaysParam(const std::string &name = "",
                     GmatBase *obj = NULL,
                     const std::string &desc = "Spacecraft Elapsed Time in Days",
                     const std::string &unit = "Days");
    ElapsedDaysParam(const ElapsedDaysParam &param);
    const ElapsedDaysParam& operator= (const ElapsedDaysParam &right); 
    virtual ~ElapsedDaysParam();

    // The inherited methods from RealParameter
    virtual Real EvaluateReal();

    // The inherited methods from Parameter
    virtual bool AddObject(GmatBase *obj);
    virtual Integer GetNumObjects() const;
    virtual void Evaluate();
    virtual bool Validate();

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
    
    ElapsedDaysParam();
    
    enum
    {
        INITIAL_EPOCH = RealParameterCount,
        ElapsedDaysParamCount
    };

    static const Gmat::ParameterType PARAMETER_TYPE[ElapsedDaysParamCount];
    static const std::string PARAMETER_TEXT[ElapsedDaysParamCount];
    
private:
};

#endif // ElapsedDaysParam_hpp
