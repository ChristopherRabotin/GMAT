//$Header$
//------------------------------------------------------------------------------
//                              PeriapsisStop
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/16
//
/**
 * Declares Periapsis stopping condition class.
 */
//------------------------------------------------------------------------------
#ifndef PeriapsisStop_hpp
#define PeriapsisStop_hpp

#include "gmatdefs.hpp"
#include "RealTypes.hpp"
#include "StopCondition.hpp"
#include "Interpolator.hpp"

class GMAT_API PeriapsisStop : public StopCondition
{
public:

    PeriapsisStop(const std::string &name = "",
                  Parameter *epochParam = NULL, Parameter *stopParam = NULL,
                  const Real &goal = GmatBase::REAL_PARAMETER_UNDEFINED,
                  const Real &tol = GmatRealConst::REAL_TOL,
                  const Integer repeatCount = 1,
                  RefFrame *refFrame = NULL,
                  Interpolator *interp = NULL);
    PeriapsisStop(const PeriapsisStop &copy);
    PeriapsisStop& operator= (const PeriapsisStop &right); 
    virtual ~PeriapsisStop();

    // The inherited methods from StopCondition
    virtual bool AddParameter(Parameter *param);
    virtual bool Evaluate();
    virtual bool Validate();

protected:

    Parameter *mCartStateParam;
    Parameter *mKepEccParam;

    virtual void SetParameter(Parameter *param);
    
private:

};

#endif // PeriapsisStop_hpp
