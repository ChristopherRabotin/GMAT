//$Header$
//------------------------------------------------------------------------------
//                              SingleValueStop
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
 * Declares propagator single value stopping condition operations.
 */
//------------------------------------------------------------------------------
#ifndef SingleValueStop_hpp
#define SingleValueStop_hpp

#include "gmatdefs.hpp"
#include "RealTypes.hpp"
#include "StopCondition.hpp"
#include "Interpolator.hpp"

class GMAT_API SingleValueStop : public StopCondition
{
public:

    SingleValueStop(const std::string &name = "",
                    Parameter *epochParam = NULL, Parameter *stopParam = NULL,
                    const Real &goal = GmatBase::REAL_PARAMETER_UNDEFINED,
                    const Real &tol = GmatRealConst::REAL_TOL,
                    const Integer repeatCount = 1,
                    RefFrame *refFrame = NULL,
                    Interpolator *interp = NULL);
    SingleValueStop(const SingleValueStop &copy);
    SingleValueStop& operator= (const SingleValueStop &right); 
    virtual ~SingleValueStop();

    // The inherited methods from StopCondition
    virtual bool Evaluate();
    virtual bool Validate();

protected:
    
private:

};

#endif // SingleValueStop_hpp
