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
 * Declares propagator stopping condition class.
 */
//------------------------------------------------------------------------------
#ifndef StopCondition_hpp
#define StopCondition_hpp

#include "gmatdefs.hpp"
#include "RealTypes.hpp"
#include "paramdefs.hpp"
#include "GmatBase.hpp"
#include "Interpolator.hpp"
#include "RefFrame.hpp"

class GMAT_API StopCondition : public GmatBase
{
public:

    StopCondition(const std::string &name,
                  const std::string typeStr = "StopCondition",
                  Parameter *epochParam = NULL, Parameter *stopParam = NULL,
                  const Real &goal = GmatBase::REAL_PARAMETER_UNDEFINED,
                  const Real &tol = GmatRealConst::REAL_TOL,
                  const Integer repeatCount = 1,
                  RefFrame *refFrame = NULL,
                  Interpolator *interp = NULL);
    StopCondition(const StopCondition &copy);
    StopCondition& operator= (const StopCondition &right); 
    virtual ~StopCondition();

    bool IsInitialized();
    Integer GetNumParameters();
    Integer GetBufferSize();

    Real GetGoal();
    Real GetTolerance();
    ParameterPtrArray GetParameters() const;
    RefFrame* GetRefFrame();
    Interpolator* GetInterpolator();

    void SetGoal(const Real &goal);
    void SetTolerance(const Real &tol);
    bool SetInterpolator(Interpolator *interp);
    bool SetRefFrame(RefFrame *refFrame);
    bool SetEpochParameter(Parameter *epochParam);
    
    Real GetStopEpoch();
    
    virtual bool AddParameter(Parameter *param);

    /**
     * Evaluates stopping condition.
     * All derived classes must implement this method.
     */
    virtual bool Evaluate() = 0;
    /**
     * Validates stopping condition.
     * All derived classes must implement this method.
     */
    virtual bool Validate() = 0;

protected:

    void Initialize();
    virtual void SetParameter(Parameter *param);

    Real mGoal;
    Real mTolerance;
    Integer mRepeatCount;
    RefFrame *mRefFrame;
    Interpolator *mInterpolator;

    Parameter *mEpochParam;
    ParameterPtrArray mParameters;
    Integer mNumParams;

    /// ring buffer for epoches
    RealArray mEpochBuffer;
    /// ring buffer for associated values
    RealArray mValueBuffer;
    Integer mNumValidPoints;
    Integer mBufferSize;
    Real mStopEpoch;
    
    bool mInitialized;

private:
    StopCondition();
    void CopyDynamicData(const StopCondition &stopCond);
};

#endif // StopCondition_hpp
