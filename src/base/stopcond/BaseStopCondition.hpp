//$Header$
//------------------------------------------------------------------------------
//                              BaseStopCondition
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
 * Declares BaseStopCondition class.
 */
//------------------------------------------------------------------------------
#ifndef BaseStopCondition_hpp
#define BaseStopCondition_hpp

#include "gmatdefs.hpp"
#include "RealTypes.hpp"
#include "paramdefs.hpp"
#include "GmatBase.hpp"
#include "Interpolator.hpp"
#include "RefFrame.hpp"

class GMAT_API BaseStopCondition : public GmatBase
{
public:

    BaseStopCondition(const std::string &name,
                      const std::string &typeStr = "BaseStopCondition",
                      const std::string &desc = "",
                      Parameter *epochParam = NULL,
                      Parameter *stopParam = NULL,
                      const Real &goal = GmatBase::REAL_PARAMETER_UNDEFINED,
                      const Real &tol = GmatRealConst::REAL_TOL,
                      const Integer repeatCount = 1,
                      RefFrame *refFrame = NULL,
                      Interpolator *interp = NULL);
    BaseStopCondition(const BaseStopCondition &copy);
    BaseStopCondition& operator= (const BaseStopCondition &right); 
    virtual ~BaseStopCondition();

    bool IsInitialized()
        { return mInitialized; }
    Integer GetNumParameters()
        { return mNumParams; }
    Integer GetBufferSize()
        { return mBufferSize; }
    
    Real GetGoal()
        { return mGoal; }
    Real GetTolerance()
        { return mTolerance; }
    Integer GetRepeatCount()
        { return mRepeatCount; }
    std::string& GetDescription()
        { return mDescription; }
    
    ParameterPtrArray GetParameters() const
        { return mParameters; }
    RefFrame* GetRefFrame()
        { return mRefFrame; }
    Interpolator* GetInterpolator()
        { return mInterpolator; }
    
    void SetGoal(const Real &goal)
        { mGoal = goal; }
    void SetTolerance(const Real &tol)
        { mTolerance = tol; }
    void SetRepeatCount(const Integer &repeatCount)
        { mRepeatCount = repeatCount; }
    void SetDescription(const std::string &desc)
        { mDescription = desc; }

    bool SetInterpolator(Interpolator *interp);
    bool SetInterpolator(const std::string &name);
    bool SetRefFrame(RefFrame *refFrame);
    bool SetRefFrame(const std::string &name);
    bool SetEpochParameter(Parameter *epochParam);
    bool SetEpochParameter(const std::string &name);
    
    Real GetStopEpoch();
    
    virtual bool SetObjectOfParameter(Gmat::ObjectType objType, GmatBase *obj);
    virtual bool AddParameter(Parameter *param);
    virtual bool AddParameter(const std::string &paramName);

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
    virtual bool SetParameter(Parameter *param);

    Real mEpoch;
    Real mGoal;
    Real mTolerance;
    Integer mRepeatCount;
    RefFrame *mRefFrame;
    Interpolator *mInterpolator;
    std::string mDescription;
    
    Parameter *mEpochParam;
    ParameterPtrArray mParameters;
    StringArray mParamNames;
    Integer mNumParams;

    /// ring buffer for epoches
    RealArray mEpochBuffer;
    /// ring buffer for associated values
    RealArray mValueBuffer;
    Integer mNumValidPoints;
    Integer mBufferSize;
    Real mStopEpoch;

    bool mUseInternalEpoch;
    bool mInitialized;

private:
    BaseStopCondition();
    void CopyDynamicData(const BaseStopCondition &stopCond);
};

#endif // BaseStopCondition_hpp
