//$Header$
//------------------------------------------------------------------------------
//                              ApoapsisStop
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/15
//
/**
 * Declares Apoapsis stopping condition class.
 */
//------------------------------------------------------------------------------
#ifndef ApoapsisStop_hpp
#define ApoapsisStop_hpp

#include "gmatdefs.hpp"
#include "RealTypes.hpp"
#include "StopCondition.hpp"
#include "Interpolator.hpp"

class GMAT_API ApoapsisStop : public StopCondition
{
public:

    ApoapsisStop(const std::string &name = "", const std::string &desc = "",
                 Parameter *epochParam = NULL, Parameter *stopParam = NULL,
                 const Real &goal = GmatBase::REAL_PARAMETER_UNDEFINED,
                 const Real &tol = GmatRealConst::REAL_TOL,
                 const Integer repeatCount = 1,
                 RefFrame *refFrame = NULL,
                 Interpolator *interp = NULL);
    ApoapsisStop(const ApoapsisStop &copy);
    ApoapsisStop& operator= (const ApoapsisStop &right); 
    virtual ~ApoapsisStop();

    // The inherited methods from StopCondition
    virtual bool AddParameter(Parameter *param);
    virtual bool SetObjectOfParameter(Gmat::ObjectType objType, GmatBase *obj);
    virtual bool Evaluate();
    virtual bool Validate();

protected:

    Parameter *mCartStateParam;
    Parameter *mKepEccParam;

    virtual bool SetParameter(Parameter *param);
    
private:

};

#endif // ApoapsisStop_hpp
