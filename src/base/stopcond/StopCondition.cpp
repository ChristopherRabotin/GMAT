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
// StopCondition(const std::string &name, const std::string &typeStr,
//               Parameter *epochParam, Parameter *stopParam, const Real &goal,
//               const Real &tol, const Integer repeatCount, RefFrame *refFrame,
//               Interpolator *interp)
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
StopCondition::StopCondition(const std::string &name, const std::string &typeStr,
                             const std::string &desc, Parameter *epochParam,
                             Parameter *stopParam, const Real &goal, const Real &tol,
                             const Integer repeatCount, RefFrame *refFrame,
                             Interpolator *interp)
    : GmatBase(Gmat::STOP_CONDITION, typeStr, name)
{
    mGoal = goal;
    mTolerance = tol;
    mRepeatCount = repeatCount;
    mRefFrame = refFrame;
    mInterpolator = interp;
    mEpochParam = epochParam;
    mDescription = desc;
    
    mNumParams = 0;
    mNumValidPoints = 0;
    mBufferSize = 0;
    mStopEpoch = REAL_PARAMETER_UNDEFINED;
    
    if (stopParam != NULL)
        AddParameter(stopParam);
       
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
    mEpochParam = copy.mEpochParam;
    mNumParams = copy.mNumParams;
    mStopEpoch = copy.mStopEpoch;
    
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
        mEpochParam = right.mEpochParam;
        mNumParams = right.mNumParams;
        mStopEpoch = right.mStopEpoch;

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
// bool SetEpochParameter(Parameter *epochParam)
//------------------------------------------------------------------------------
/**
 * Sets epoch parameter which will be used to interpolate stop epoch
 *
 * @return true if epoch paraemter is set;
 *         false if epoch paraemter has already been set.
 */
//------------------------------------------------------------------------------
bool StopCondition::SetEpochParameter(Parameter *epochParam)
{
    if (mEpochParam == NULL)
    {
        mEpochParam = epochParam;
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
// virtual bool SetObjectOfParameter(Gmat::ObjectType objType, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Sets object to parameter used in stop condition.
 *
 * @return true if object has been set.
 */
//------------------------------------------------------------------------------
bool StopCondition::SetObjectOfParameter(Gmat::ObjectType objType, GmatBase *obj)
{
    return false; // base class doesn't know which object is used
}

//------------------------------------------------------------------------------
// virtual bool AddParameter(Parameter *param)
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
    
    //loj: 2/26/04 Do I really need to validate parameter before add?
    //if (param->Validate())
    //{
        mParameters.push_back(param);
        mNumParams = mParameters.size();
        if (param->IsTimeParameter())
            mInitialized = true;
        
        SetParameter(param);
        added = true;
    //}

    return added;
}

//------------------------------------------------------------------------------
// virtual Real StopCondition::GetStopEpoch()
//------------------------------------------------------------------------------
/**
 * @return epoch at stop condition met.
 */
//------------------------------------------------------------------------------
Real StopCondition::GetStopEpoch()
{
    return mStopEpoch;
}

//---------------------------------
// protected methods
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
        // need epoches for interpolator
        if (mEpochParam == NULL)
            mInitialized = false;
        
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
// virtual bool SetParameter(Parameter *param)
//------------------------------------------------------------------------------
bool StopCondition::SetParameter(Parameter *param)
{
    return false;
}

//---------------------------------
// private methods
//---------------------------------

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

