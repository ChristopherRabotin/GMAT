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

    BaseStopCondition(const std::string &name = "",
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

    bool IsInitialized();
    Integer GetBufferSize();
    std::string& GetDescription();
    Parameter* GetEpochParameter();
    Parameter* GetStopParameter();
    RefFrame* GetRefFrame();    
    Interpolator* GetInterpolator();
    
    void SetDescription(const std::string &desc);
    bool SetInterpolator(Interpolator *interp);
    bool SetInterpolator(const std::string &name);
    bool SetRefFrame(RefFrame *refFrame);
    bool SetRefFrame(const std::string &name);
    bool SetEpochParameter(Parameter *param);
    bool SetEpochParameter(const std::string &name);
    bool SetStopParameter(Parameter *param);
    bool SetStopParameter(const std::string &name);
    
    Real GetStopEpoch();
    
    virtual bool SetObjectOfParameter(Gmat::ObjectType objType, GmatBase *obj);

    virtual bool Evaluate() = 0;
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

    void Initialize();

    Real mEpoch;
    Real mGoal;
    Real mTolerance;
    Real mEccTol;
    Real mRange;
    Integer mRepeatCount;
    RefFrame *mRefFrame;
    Interpolator *mInterpolator;
    
    std::string mDescription;
    std::string mStopParamType;
    
    Parameter *mEpochParam;
    Parameter *mStopParam;
    Parameter *mEccParam;
    Parameter *mRmagParam;
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
    bool mNeedInterpolator;
    bool mNeedRefFrame;
    
    enum
    {
        EPOCH = 0,
        EPOCH_VAR,
        STOP_VAR,
        GOAL,
        TOLERANCE,
        ECC_TOL,
        RANGE,
        REPEAT_COUNT,
        INTERPOLATOR,
        REF_FRAME,
        BaseStopConditionParamCount,
    };
    
    static const Gmat::ParameterType PARAMETER_TYPE[BaseStopConditionParamCount];
    static const std::string PARAMETER_TEXT[BaseStopConditionParamCount];

private:
    void CopyDynamicData(const BaseStopCondition &stopCond);
};

#endif // BaseStopCondition_hpp
