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
 * Implements BaseStopCondition class.
 */
//------------------------------------------------------------------------------
#include "BaseStopCondition.hpp"
#include "StopConditionException.hpp"
#include "MeanJ2000Equatorial.hpp"
#include "CubicSplineInterpolator.hpp"
#include "Moderator.hpp"
#include "MessageInterface.hpp"


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// BaseStopCondition(const std::string &name, const std::string &typeStr,
//               Parameter *epochParam, Parameter *stopParam, const Real &goal,
//               const Real &tol, const Integer repeatCount, RefFrame *refFrame,
//               Interpolator *interp)
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
BaseStopCondition::BaseStopCondition(const std::string &name,
                                     const std::string &typeStr,
                                     const std::string &desc,
                                     Parameter *epochParam, Parameter *stopParam,
                                     const Real &goal, const Real &tol,
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

    mInitialized = false;
    mUseInternalEpoch = false;
    
    if (epochParam == NULL)
        mUseInternalEpoch = true;

    //loj: 3/22/04
    // Create default Interpolator
    if (mInterpolator == NULL)
    {
        mInterpolator = new CubicSplineInterpolator("InternalInterpolator");
    }
    
    //loj: 3/22/04
    // Create default RefFrame
    if (mInterpolator == NULL)
    {
        mRefFrame = new MeanJ2000Equatorial("InternalRefFrame");
    }
    
    Initialize();
}

//------------------------------------------------------------------------------
// BaseStopCondition(const BaseStopCondition &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 */
//------------------------------------------------------------------------------
BaseStopCondition::BaseStopCondition(const BaseStopCondition &copy)
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
// BaseStopCondition& operator= (const BaseStopCondition &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 */
//------------------------------------------------------------------------------
BaseStopCondition& BaseStopCondition::operator= (const BaseStopCondition &right)
{
    if (this != &right)
    {
        GmatBase::operator=(right);        
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
// virtual ~BaseStopCondition()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BaseStopCondition::~BaseStopCondition()
{
}

//------------------------------------------------------------------------------
// bool SetInterpolator(Interpolator *interp)
//------------------------------------------------------------------------------
/**
 * Sets Interpolator to interpolate stop condition epoch.
 *
 * @return true if Interplator is successfully set; false otherwise
 */
//------------------------------------------------------------------------------
bool BaseStopCondition::SetInterpolator(Interpolator *interp)
{
    if (interp != NULL)
    {
        if (mInterpolator->GetName() == "InternalInterpolator")
            delete mInterpolator;
        
        mInterpolator = interp;
        Initialize();
        return true;
    }
    else
    {
        //throw StopCondException("BaseStopCondition:: The interpolator is already set.");
        return false;
    }
}

//------------------------------------------------------------------------------
// bool SetInterpolator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets Interpolator to interpolate stop condition epoch.
 *
 * @return true if Interplator is successfully set; false otherwise
 */
//------------------------------------------------------------------------------
bool BaseStopCondition::SetInterpolator(const std::string &name)
{
    bool status = false;
    
    Moderator *theModerator = Moderator::Instance();
    
    if (name != "")
    {
        // get Interpolator pointer
        Interpolator *interp = theModerator->GetInterpolator(name);
        if (interp != NULL)
        {
            status = SetInterpolator(interp);
        }
    }

    return status;
}

//------------------------------------------------------------------------------
// bool SetRefFrame(RefFrame *refFrame)
//------------------------------------------------------------------------------
/**
 * Sets reference frame
 *
 * @return true if RefFrame is successfully set; false otherwise
 */
//------------------------------------------------------------------------------
bool BaseStopCondition::SetRefFrame(RefFrame *refFrame)
{
    if (refFrame != NULL)
    {
        if (mRefFrame->GetName() == "InternalRefFrame")
            delete mRefFrame;
            
        mRefFrame = refFrame;
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
// bool SetRefFrame(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets reference frame
 *
 * @return true if RefFrame is successfully set; false otherwise
 */
//------------------------------------------------------------------------------
bool BaseStopCondition::SetRefFrame(const std::string &name)
{
    bool status = false;

    Moderator *theModerator = Moderator::Instance();
    
    if (name != "")
    {
        // get RefFrame pointer
        RefFrame *refFrame = theModerator->GetRefFrame(name);
        if (refFrame != NULL)
        {
            status = SetRefFrame(refFrame);
        }
    }

    return status;
}

//------------------------------------------------------------------------------
// bool SetEpochParameter(Parameter *epochParam)
//------------------------------------------------------------------------------
/**
 * Sets epoch parameter which will be used to interpolate stop epoch
 *
 * @return true if EpochParam is successfully set; false otherwise
 */
//------------------------------------------------------------------------------
bool BaseStopCondition::SetEpochParameter(Parameter *epochParam)
{
    if (epochParam != NULL)
    {
        mEpochParam = epochParam;
        mUseInternalEpoch = false;
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
// bool SetEpochParameter(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets epoch parameter which will be used to interpolate stop epoch
 *
 * @return true if EpochParam is successfully set; false otherwise
 */
//------------------------------------------------------------------------------
bool BaseStopCondition::SetEpochParameter(const std::string &name)
{
    bool status = false;
    Moderator *theModerator = Moderator::Instance();
    
    if (name != "")
    {
        // get parameter pointer
        Parameter *param = theModerator->GetParameter(name);
        if (param != NULL)
        {
            //MessageInterface::ShowMessage("BaseStopCondition::SetEpochParameter() name = %s\n",
            //                               param->GetName().c_str());
            status = SetEpochParameter(param);
        }
    }

    return status;
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
bool BaseStopCondition::SetObjectOfParameter(Gmat::ObjectType objType, GmatBase *obj)
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
bool BaseStopCondition::AddParameter(Parameter *param)
{
    bool added = false;
    
    //loj: 2/26/04 Do I really need to validate parameter before add?
    //if (param->Validate())
    //{
        mParameters.push_back(param);
        mParamNames.push_back(param->GetName());
        mNumParams = mParameters.size();
        
        if (param->IsTimeParameter())
            mInitialized = true;
        else
            Initialize();
        
        SetParameter(param);
        added = true;
    //}

    return added;
}

//------------------------------------------------------------------------------
// virtual bool AddParameter(const std::string &paramName)
//------------------------------------------------------------------------------
bool BaseStopCondition::AddParameter(const std::string &paramName)
{
    bool status = false;
    Moderator *theModerator = Moderator::Instance();
    
    if (paramName != "")
    {
        // get parameter pointer
        Parameter *param = theModerator->GetParameter(paramName);
        if (param != NULL)
        {
            //MessageInterface::ShowMessage("BaseStopCondition::AddParameter() name = %s\n",
            //                               param->GetName().c_str());
            AddParameter(param);
            status = true;
        }
    }

    return status;
}

//------------------------------------------------------------------------------
// virtual Real BaseStopCondition::GetStopEpoch()
//------------------------------------------------------------------------------
/**
 * @return epoch at stop condition met.
 */
//------------------------------------------------------------------------------
Real BaseStopCondition::GetStopEpoch()
{
    return mStopEpoch;
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
void BaseStopCondition::Initialize()
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
        if (!mUseInternalEpoch && mEpochParam == NULL)
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
/*
 * All derived class should implement this method
 */
//------------------------------------------------------------------------------
bool BaseStopCondition::SetParameter(Parameter *param)
{
    return false;
}

//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void CopyDynamicData()
//------------------------------------------------------------------------------
void BaseStopCondition::CopyDynamicData(const BaseStopCondition &stopCond)
{
    mNumValidPoints = stopCond.mNumValidPoints;
      
    mBufferSize = stopCond.mBufferSize;
    for (int i=0; i<mBufferSize; i++)
    {
        mEpochBuffer[i] = stopCond.mEpochBuffer[i];
        mValueBuffer[i] = stopCond.mValueBuffer[i];
    }
}

