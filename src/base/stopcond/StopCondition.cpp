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
// Created: 2003/10/10
//
/**
 * Implements StopCondition class.
 */
//------------------------------------------------------------------------------
#include "StopCondition.hpp"
#include "StopConditionException.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// static data
//---------------------------------

const std::string
StopCondition::PARAMETER_TEXT[StopConditionParamCount] =
{
    "Epoch",
    "EpochVar",
    "StopVar",
    "Goal",
    "Tol",
    "Repeat",
    "Interpolator",
    "RefFrame",
};

const Gmat::ParameterType
StopCondition::PARAMETER_TYPE[StopConditionParamCount] =
{
    Gmat::REAL_TYPE,
    Gmat::STRING_TYPE,
    Gmat::STRING_TYPE,
    Gmat::REAL_TYPE,
    Gmat::REAL_TYPE,
    Gmat::INTEGER_TYPE,
    Gmat::STRING_TYPE,
    Gmat::STRING_TYPE,
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// StopCondition(const std::string &name, const std::string &desc,
//               Parameter *epochParam, Parameter *stopParam,
//               const Real &goal, const Real &tol, const Integer repeatCount,
//               RefFrame *refFrame, Interpolator *interp)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of stop condition
 * @param <epochParam> Parameter object pointer to retrive epoch value
 * @param <stopParam> Parameter object pointer to retrive stop value
 * @param <goal> goal for stop condition test
 * @param <tol>  tolerance for stop condition test
 * @param <repeatCount> repeat count for stop condition test
 * @param <refFrame> RefFrame object pointer to be used for stop condition test
 * @param <interp> Interpolator object pointer for interpolating stop epoch
 */
//------------------------------------------------------------------------------
StopCondition::StopCondition(const std::string &name, const std::string &desc,
                             Parameter *epochParam, Parameter *stopParam,
                             const Real &goal, const Real &tol,
                             const Integer repeatCount, RefFrame *refFrame,
                             Interpolator *interp)
    : BaseStopCondition(name, "StopCondition", desc, epochParam, stopParam, goal, tol,
                        repeatCount, refFrame, interp)
{
}

//------------------------------------------------------------------------------
// StopCondition(const StopCondition &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 */
//------------------------------------------------------------------------------
StopCondition::StopCondition(const StopCondition &copy)
    : BaseStopCondition(copy)
{
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
        BaseStopCondition::operator=(right);

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
// bool SetSingleParameter(Parameter *param)
//------------------------------------------------------------------------------
/*
 * Sets single parameter. This method is another way of setting the parameter.
 */
//------------------------------------------------------------------------------
bool StopCondition::SetSingleParameter(Parameter *param)
{
    bool status = false;
    
    //loj: Do I really need to validate parameter before add?
    if (param->Validate())
    {
        if (mNumParams > 0)
            mParameters[0] = param;
        SetParameter(param);
        status = true;
    }

    return status;
}


//-----------------------------------------
// Inherited methods from BaseStopCondition
//-----------------------------------------

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * @return true if sigle parameter value stopping condition has been met;
 *   false otherwise
 *
 * @exception <StopConditionException> thrown if unable to retrive parameter
 *   value
 *
 * @note count for repeat stop count will be implemented in the next build.
 */
//------------------------------------------------------------------------------
bool StopCondition::Evaluate()
{
    bool goalMet = false;
    Real epoch;
    Real rval;
    Real stopEpoch;
    
    if (!Validate())
    {
        throw StopConditionException
            ("StopCondition::Evaluate(): Validate() failed.");
    }
      
    //loj: Do I really need to validate parameter before evaluate?
    //     Is validating parameter in AddParameter enough?
    if (mParameters[0]->Validate() == false)
    {
        throw StopConditionException
            ("Cannot evaluate the stop condition: " + 
             mParameters[0]->GetTypeName() + ":" + mParameters[0]->GetName() +
             " Validate() failed");
    }

    // set current epoch
    if (mUseInternalEpoch)
        epoch = mEpoch;
    else
        epoch = mEpochParam->EvaluateReal();
    
    // for time data we don't need to interpolate
    if (mParameters[0]->IsTimeParameter())
    {
        rval = mParameters[0]->EvaluateReal();
        
        //MessageInterface::ShowMessage("StopCondition::Evaluate() mUseInternalEpoch = %d, "
        //                              "epoch = %f, mGoal = %f, rval = %f\n",
        //                              mUseInternalEpoch, epoch, mGoal, rval);
        
        if (rval >= mGoal)
        {
            //mStopEpoch = rval; //Should we return this?
            mStopEpoch = mGoal;
            goalMet = true;
        }
    }
    else
    {
        //-----------------------------------------------------------------
        //    *** need to interpolate stop epoch
        //    *** need to convert to RefFrame for certain stop condition type
        //-----------------------------------------------------------------
        mNumValidPoints = (mNumValidPoints < mBufferSize) ?
            mNumValidPoints + 1 : mNumValidPoints;

        //MessageInterface::ShowMessage("StopCondition::Evaluate() mNumValidPoints=%d, "
        //                              "mBufferSize=%d\n", mNumValidPoints, mBufferSize);
        
        // shift values to make room for newest value
        for (int i=0; i<mBufferSize-1; i++)
        {
            mEpochBuffer[i] = mEpochBuffer[i+1];
            mValueBuffer[i] = mValueBuffer[i+1];
        }
        
        //loj: 3/23/04 moved to before "if" epoch = mEpochParam->EvaluateReal();
        rval = mParameters[0]->EvaluateReal();
        mEpochBuffer[mBufferSize - 1] = epoch;
        mValueBuffer[mBufferSize - 1] = rval;
        
        MessageInterface::ShowMessage("StopCondition::Evaluate() epoch=%f, rval=%f\n",
                                      epoch, rval);
        
        // Stop if at least <mBufferSize> points set, and either
        //   1) last value was more than goal, but current value is less,
        //   2) last value was less than goal, but current value is more.
        if (mNumValidPoints >= mBufferSize &&
            ((mValueBuffer[mBufferSize-mBufferSize] <= mGoal  &&
              mGoal <= mValueBuffer[mBufferSize-1]) ||  
             (mValueBuffer[mBufferSize-mBufferSize] >= mGoal  &&
              mGoal >= mValueBuffer[mBufferSize-1]) ))
        {
            goalMet = true;

            // now interpolate epoch
            mInterpolator->Clear();
            for (int i=0; i<mBufferSize; i++)
            {
                MessageInterface::ShowMessage("StopCondition::Evaluate() i=%d, mValueBuffer=%f, "
                                              "mEpochBuffer=%f\n", i, mValueBuffer[i],
                                              mEpochBuffer[i]);
                mInterpolator->AddPoint(mValueBuffer[i], &mEpochBuffer[i]);
            }
            
            if (mInterpolator->Interpolate(mGoal, &stopEpoch))
                mStopEpoch = stopEpoch;
            
            MessageInterface::ShowMessage("StopCondition::Evaluate() mStopEpoch=%f\n", mStopEpoch);
        }
    }
  
    return goalMet;
}


//------------------------------------------------------------------------------
// virtual bool Validate()
//------------------------------------------------------------------------------
/**
 * @return true if all necessary objects have been set; false otherwise
 */
//------------------------------------------------------------------------------
bool StopCondition::Validate()
{
    bool valid = false;
    
    if (mNumParams >= 1)
    {
        if (mParameters[0]->IsTimeParameter())
            valid = true;
        else
            if ((mUseInternalEpoch || mEpochParam != NULL) && mInterpolator != NULL)
            {
                valid = true;
            }
            else
            {
                MessageInterface::ShowMessage
                    ("StopCondition::Validate() mNumParams=%d, mUseInternalEpoch=%d, "
                     "mEpochParam=%d, mInterpolator=%d\n", mNumParams, mUseInternalEpoch,
                     mEpochParam, mInterpolator);
            }
    }
    else
    {
        MessageInterface::ShowMessage("StopCondition::Validate() mNumParams = %d\n",
                                      mNumParams);
    }
    return valid;
}

//---------------------------------
// methods inherited from GmatBase
//---------------------------------

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string StopCondition::GetParameterText(const Integer id) const
{
    if (id >= 0 && id < StopConditionParamCount)
        return PARAMETER_TEXT[id];
    else
        return GmatBase::GetParameterText(id);
    
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer StopCondition::GetParameterID(const std::string &str) const
{
    //MessageInterface::ShowMessage("StopCondition::GetParameterID() str = %s\n", str.c_str());
    
    for (int i=0; i<StopConditionParamCount; i++)
    {
        if (str == PARAMETER_TEXT[i])
            return i;
    }
   
    return GmatBase::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType StopCondition::GetParameterType(const Integer id) const
{
    if (id >= 0 && id < StopConditionParamCount)
        return PARAMETER_TYPE[id];
    else
        return GmatBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string StopCondition::GetParameterTypeString(const Integer id) const
{
    if (id >= 0 && id < StopConditionParamCount)
        return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
    else
       return GmatBase::GetParameterTypeString(id);
    
}

//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer StopCondition::GetIntegerParameter(const Integer id) const
{
    switch (id)
    {
    case REPEAT_COUNT:
        return mRepeatCount;
    default:
        return GmatBase::GetIntegerParameter(id);
    }
}

//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
Integer StopCondition::GetIntegerParameter(const std::string &label) const
{
    return GetIntegerParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer StopCondition::SetIntegerParameter(const Integer id,
                                           const Integer value)
{
    switch (id)
    {
    case REPEAT_COUNT:
        mRepeatCount = value;
        return mRepeatCount;
    default:
        return GmatBase::SetIntegerParameter(id, value);
    }
}

//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
Integer StopCondition::SetIntegerParameter(const std::string &label,
                                           const Integer value)
{
    return SetIntegerParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
Real StopCondition::GetRealParameter(const Integer id) const
{
    switch (id)
    {
    case GOAL:
        return mGoal;
    case TOLERANCE:
        return mTolerance;
    case EPOCH:
        return mEpoch;
    default:
        return GmatBase::GetRealParameter(id);
    }
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
Real StopCondition::GetRealParameter(const std::string &label) const
{
    return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
Real StopCondition::SetRealParameter(const Integer id, const Real value)
{
    switch (id)
    {
    case GOAL:
        mGoal = value;
        return mGoal;
    case TOLERANCE:
        mTolerance = value;
        return mTolerance;
    case EPOCH:
        mEpoch = value;
        return mEpoch;
    default:
        return GmatBase::GetRealParameter(id);
    }
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
Real StopCondition::SetRealParameter(const std::string &label, const Real value)
{
    //MessageInterface::ShowMessage("StopCondition::SetRealParameter() label=%s, "
    //                              "value=%d\n", label.c_str(), value);
    
    return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string StopCondition::GetStringParameter(const Integer id) const
{
    switch (id)
    {
    case EPOCH_VAR:
        return mEpochParam->GetTypeName();
    case STOP_VAR:
        return mParamNames[0]; // return first parameter name
    case INTERPOLATOR:
        if (mInterpolator != NULL)
            return mInterpolator->GetTypeName();
        else
            return "UndefinedInterpolator";
    case REF_FRAME:
        if (mRefFrame != NULL)
            return mRefFrame->GetTypeName();
        else
            return "UndefinedRefFrame";
    default:
        return GmatBase::GetStringParameter(id);
    }
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string StopCondition::GetStringParameter(const std::string &label) const
{
    //MessageInterface::ShowMessage("StopCondition::GetStringParameter() label = %s\n",
    //                              label.c_str());

    return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool StopCondition::SetStringParameter(const Integer id, const std::string &value)
{
    //MessageInterface::ShowMessage("StopCondition::SetStringParameter() id = %d, "
    //                              "value = %s \n", id, value.c_str());
    
    switch (id)
    {
    case EPOCH_VAR:
        return SetEpochParameter(value);
    case STOP_VAR:
        return AddParameter(value);
    case INTERPOLATOR:
        return SetInterpolator(value);
    case REF_FRAME:
        return SetRefFrame(value);
    default:
        return GmatBase::SetStringParameter(id, value);
    }
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool StopCondition::SetStringParameter(const std::string &label,
                                       const std::string &value)
{
    //MessageInterface::ShowMessage("StopCondition::SetStringParameter() label = %s, "
    //                              "value = %s \n", label.c_str(), value.c_str());

    return SetStringParameter(GetParameterID(label), value);
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// virtual bool SetParameter(Parameter *param)
//------------------------------------------------------------------------------
/**
 * Sets parameter used in checking stop condition.
 */
//------------------------------------------------------------------------------
bool StopCondition::SetParameter(Parameter *param)
{
    // There is no extra parameters to set for single parameter

    return true;
}
