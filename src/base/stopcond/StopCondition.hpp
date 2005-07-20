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
 * Declares StopCondition class.
 */
//------------------------------------------------------------------------------
#ifndef StopCondition_hpp
#define StopCondition_hpp

#include "gmatdefs.hpp"
#include "RealTypes.hpp"
#include "BaseStopCondition.hpp"

class GMAT_API StopCondition : public BaseStopCondition
{
public:

   StopCondition(const std::string &name = "",
                 const std::string &desc = "",
                 Parameter *epochParam = NULL,
                 Parameter *stopParam = NULL,
                 const Real &goal = GmatBase::REAL_PARAMETER_UNDEFINED,
                 const Real &tol = STOP_COND_TOL,
                 const Integer repeatCount = 1,
                 //RefFrame *refFrame = NULL,
                 Interpolator *interp = NULL);
   StopCondition(const StopCondition &copy);
   StopCondition& operator= (const StopCondition &right); 
   virtual ~StopCondition();

   virtual bool Evaluate();

   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

   bool CheckOnPeriapsis();
   bool CheckOnApoapsis();
   bool CheckOnAnomaly(Real anomaly);
   
private:
   
};

#endif // StopCondition_hpp
