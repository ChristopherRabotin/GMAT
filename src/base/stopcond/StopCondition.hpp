//$Header$
//------------------------------------------------------------------------------
//                              StopCondition
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/10/09
//
/**
 * Declares stopping condition class.
 */
//------------------------------------------------------------------------------
#ifndef StopCondition_hpp
#define StopCondition_hpp

#include "gmatdefs.hpp"
#include "BaseStopCondition.hpp"
#include "RealTypes.hpp"
#include "Interpolator.hpp"

class GMAT_API StopCondition : public BaseStopCondition
{
public:

    StopCondition(const std::string &name = "",
                  const std::string &desc = "",
                  Parameter *epochParam = NULL, Parameter *stopParam = NULL,
                  const Real &goal = GmatBase::REAL_PARAMETER_UNDEFINED,
                  const Real &tol = GmatRealConst::REAL_TOL,
                  const Integer repeatCount = 1,
                  RefFrame *refFrame = NULL,
                  Interpolator *interp = NULL);
    StopCondition(const StopCondition &copy);
    StopCondition& operator= (const StopCondition &right); 
    virtual ~StopCondition();

    bool SetSingleParameter(Parameter *param);
    
    // methods inherited from BaseStopCondition
    virtual bool Evaluate();
    virtual bool Validate();

    // methods inherited from GmatBase
    virtual std::string GetParameterText(const Integer id) const;
    virtual Integer GetParameterID(const std::string &str) const;
    virtual Gmat::ParameterType GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;

    virtual Integer GetIntegerParameter(const Integer id) const;
    virtual Integer GetIntegerParameter(const std::string &label) const;
    virtual Integer SetIntegerParameter(const Integer id,
                                        const Integer value);
    virtual Integer SetIntegerParameter(const std::string &label,
                                        const Integer value);

    virtual Real GetRealParameter(const Integer id) const;
    virtual Real GetRealParameter(const std::string &label) const;
    virtual Real SetRealParameter(const Integer id, const Real value);
    virtual Real SetRealParameter(const std::string &label, const Real value);

    virtual std::string GetStringParameter(const Integer id) const;
    virtual std::string GetStringParameter(const std::string &label) const;
    virtual bool SetStringParameter(const Integer id, const std::string &value);
    virtual bool SetStringParameter(const std::string &label,
                                    const std::string &value);
    
protected:
    // The inherited methods from StopCondition
    virtual bool SetParameter(Parameter *param);

    enum
    {
        EPOCH = 0,
        EPOCH_VAR,
        STOP_VAR,
        GOAL,
        TOLERANCE,
        REPEAT_COUNT,
        INTERPOLATOR,
        REF_FRAME,
        StopConditionParamCount,
    };
    
    static const Gmat::ParameterType PARAMETER_TYPE[StopConditionParamCount];
    static const std::string PARAMETER_TEXT[StopConditionParamCount];

private:

};

#endif // StopCondition_hpp
