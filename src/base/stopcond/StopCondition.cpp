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
 * Defines propagator stopping condition operations.
 */
//------------------------------------------------------------------------------
#include "StopCondition.hpp"
#include "StopConditionException.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// StopCondition(const std::string &name, const std::string typeStr,
//               Parameter *param, const Real &goal, const Real &tol
//               const Integer repeatCount, RefFrame *refFrame, Interpolator *interp)
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
StopCondition::StopCondition(const std::string &name, const std::string typeStr,
                             Parameter *param, const Real &goal, const Real &tol,
                             const Integer repeatCount, RefFrame *refFrame,
                             Interpolator *interp)
    : GmatBase(Gmat::STOP_CONDITION, typeStr, name)
{
    mGoal = goal;
    mTolerance = tol;
    mRepeatCount = repeatCount;
    mRefFrame = refFrame;
    mInterpolator = interp;
    
    mNumParams = 0;
    mNumValidPoints = 0;
    mBufferSize = 0;
   
    if (param != NULL)
        AddParameter(param);
       
    Initialize();
}

//------------------------------------------------------------------------------
// StopCondition(const StopCondition &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 */
//------------------------------------------------------------------------------
StopCondition::StopCondition(const StopCondition &copy)
    : GmatBase(copy)
{
    mGoal = copy.mGoal;
    mTolerance = copy.mTolerance;
    mRepeatCount = copy.mRepeatCount;
    mRefFrame = copy.mRefFrame;
    mInterpolator = copy.mInterpolator;
    mNumParams = copy.mNumParams;

    mParameters.reserve(mNumParams);
    for (int i=0; i<mNumParams; i++)
        mParameters[i] = copy.mParameters[i];

    Initialize();
    CopyDynamicData(copy);
}

//------------------------------------------------------------------------------
// StopCondition& operator= (const StopCondition &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 */
//------------------------------------------------------------------------------
StopCondition& StopCondition::operator= (const StopCondition &right)
{
    if (this != &right)
    {
        mGoal = right.mGoal;
        mTolerance = right.mTolerance;
        mRepeatCount = right.mRepeatCount;
        mRefFrame = right.mRefFrame;
        mInterpolator = right.mInterpolator;
        mNumParams = right.mNumParams;

        mParameters.reserve(mNumParams);
        for (int i=0; i<mNumParams; i++)
            mParameters[i] = right.mParameters[i];

        Initialize();
        CopyDynamicData(right);
    }

    return *this;
}

//------------------------------------------------------------------------------
// virtual ~StopCondition()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
StopCondition::~StopCondition()
{
}

//------------------------------------------------------------------------------
// bool IsInitialized()
//------------------------------------------------------------------------------
/**
 * @return true if dynamic data has been initialized; false otherwise
 */
//------------------------------------------------------------------------------
bool StopCondition::IsInitialized()
{
    return mInitialized;
}

//------------------------------------------------------------------------------
// Integer GetNumParameters()
//------------------------------------------------------------------------------
/**
 * @return number of parameter objects
 */
//------------------------------------------------------------------------------
Integer StopCondition::GetNumParameters()
{
    return mNumParams;
}

//------------------------------------------------------------------------------
// Integer GetBufferSize()
//------------------------------------------------------------------------------
/**
 * @return ring buffer size given by the interpolator.
 */
//------------------------------------------------------------------------------
Integer StopCondition::GetBufferSize()
{
    return mBufferSize;
}

//------------------------------------------------------------------------------
// Real GetGoal()
//------------------------------------------------------------------------------
Real StopCondition::GetGoal()
{
    return mGoal;
}

//------------------------------------------------------------------------------
// Real GetTolerance()
//------------------------------------------------------------------------------
Real StopCondition::GetTolerance()
{
    return mTolerance;
}

//------------------------------------------------------------------------------
// ParameterPtrArray GetParameters() const
//------------------------------------------------------------------------------
ParameterPtrArray StopCondition::GetParameters() const
{
    return mParameters;
}

//------------------------------------------------------------------------------
// RefFrame* GetRefFrame()
//------------------------------------------------------------------------------
RefFrame* StopCondition::GetRefFrame()
{
    return mRefFrame;
}

//------------------------------------------------------------------------------
// Interpolator* GetInterpolator()
//------------------------------------------------------------------------------
Interpolator* StopCondition::GetInterpolator()
{
    return mInterpolator;
}

//------------------------------------------------------------------------------
// void SetGoal(const Real &goal)
//------------------------------------------------------------------------------
void StopCondition::SetGoal(const Real &goal)
{
    mGoal = goal;
}

//------------------------------------------------------------------------------
// void SetTolerance(const Real &tol)
//------------------------------------------------------------------------------
void StopCondition::SetTolerance(const Real &tol)
{
    mTolerance = tol;
}

//------------------------------------------------------------------------------
// bool SetInterpolator(Interpolator *interp)
//------------------------------------------------------------------------------
/**
 * Sets Interpolator to interpolate stop condition epoch.
 *
 * @return true if Interplator is set; false if Interpolator has already been set.
 */
//------------------------------------------------------------------------------
bool StopCondition::SetInterpolator(Interpolator *interp)
{
    if (mInterpolator == NULL)
    {
        mInterpolator = interp;
        Initialize();
        return true;
    }
    else
    {
        //throw StopConditionException("StopCondition:: The interpolator is already set.");
        return false;
    }
}

//------------------------------------------------------------------------------
// bool SetRefFrame(RefFrame *refFrame)
//------------------------------------------------------------------------------
/**
 * Sets reference frame
 *
 * @return true if RefFrame is set; false if RefFrame has already been set.
 */
//------------------------------------------------------------------------------
bool StopCondition::SetRefFrame(RefFrame *refFrame)
{
    if (mRefFrame == NULL)
    {
        mRefFrame = refFrame;
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
// bool AddParameter(Parameter *param)
//------------------------------------------------------------------------------
/**
 * Add parameter to stop condition.
 *
 * @return true if parameter has added to array.
 */
//------------------------------------------------------------------------------
bool StopCondition::AddParameter(Parameter *param)
{
    bool added = false;
    
    //loj: Do I really need to validate parameter before add?
    if (param->Validate())
    {
        mParameters.push_back(param);
        mNumParams = mParameters.size();
        added = true;
        if (param->IsTimeParameter())
            mInitialized = true;
    }

    return added;
}

//------------------------------------------------------------------------------
// virtual Real StopCondition::GetStopEpoch()
//------------------------------------------------------------------------------
/**
 * @note This method will be implemented in the future build.
 */
//------------------------------------------------------------------------------
Real StopCondition::GetStopEpoch()
{
    //loj: temporary code
    return GmatBase::REAL_PARAMETER_UNDEFINED;
}

//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
void StopCondition::Initialize()
{
    //loj: How about mRefFrame?
    //loj: Do I need RefFram* for all stopping condition?
   
    mInitialized = false;

    if (mNumParams >= 1)
        mInitialized = true;

    // if parameter is non-time parameter, need interpolator
    bool needInterp = false;
    for (int i=0; i<mNumParams; i++)
    {
        if (mParameters[i]->IsTimeParameter() == false)
            needInterp = true;
    }

    if (needInterp)
    {
        if (mInterpolator == NULL)
        {
            mInitialized = false;
        }
        else
        {
            mBufferSize = mInterpolator->GetBufferSize();
            mEpochBuffer.reserve(mBufferSize);
            mValueBuffer.reserve(mBufferSize);
      
            for (int i=0; i<mBufferSize; i++)
            {
                mEpochBuffer[i] = 0.0;
                mValueBuffer[i] = 0.0;
            }
      
            mNumValidPoints = 0;
            mInitialized = true;
        }
    }
}

//------------------------------------------------------------------------------
// void CopyDynamicData()
//------------------------------------------------------------------------------
void StopCondition::CopyDynamicData(const StopCondition &stopCond)
{
    mNumValidPoints = stopCond.mNumValidPoints;
      
    mBufferSize = stopCond.mBufferSize;
    for (int i=0; i<mBufferSize; i++)
    {
        mEpochBuffer[i] = stopCond.mEpochBuffer[i];
        mValueBuffer[i] = stopCond.mValueBuffer[i];
    }
}

