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
// Created: 2003/10/10
//
/**
 * Defines propagator single value stopping condition operations.
 */
//------------------------------------------------------------------------------
#include "SingleValueStop.hpp"
#include "StopConditionException.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// SingleValueStop(const std::string &name, const std::string &desc,
//                 Parameter *epochParam, Parameter *stopParam,
//                 const Real &goal, const Real &tol,
//                 const Integer repeatCount,
//                 RefFrame *refFrame,
//                 Interpolator *interp)
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
SingleValueStop::SingleValueStop(const std::string &name, const std::string &desc,
                                 Parameter *epochParam, Parameter *stopParam,
                                 const Real &goal, const Real &tol,
                                 const Integer repeatCount,
                                 RefFrame *refFrame,
                                 Interpolator *interp)
    : StopCondition(name, "SingleValueStop", desc, epochParam, stopParam, goal, tol,
                    repeatCount, refFrame, interp)
{
}

//------------------------------------------------------------------------------
// SingleValueStop(const SingleValueStop &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 */
//------------------------------------------------------------------------------
SingleValueStop::SingleValueStop(const SingleValueStop &copy)
    : StopCondition(copy)
{
}

//------------------------------------------------------------------------------
// SingleValueStop& operator= (const SingleValueStop &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 */
//------------------------------------------------------------------------------
SingleValueStop& SingleValueStop::operator= (const SingleValueStop &right)
{
    if (this != &right)
        StopCondition::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// virtual ~SingleValueStop()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
SingleValueStop::~SingleValueStop()
{
}

//------------------------------------------------------------------------------
// bool SetSingleParameter(Parameter *param)
//------------------------------------------------------------------------------
/*
 * Sets single parameter. This method is another way of setting the parameter.
 */
//------------------------------------------------------------------------------
bool SingleValueStop::SetSingleParameter(Parameter *param)
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


//-------------------------------------
// Inherited methods from StopCondition
//-------------------------------------

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
bool SingleValueStop::Evaluate()
{
    bool goalMet = false;
    Real epoch;
    Real rval;
    Real stopEpoch;
    
    if (!Validate())
    {
        throw StopConditionException
            ("SingleValueStop::Evaluate(): Validate() failed.");
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

    // for time data we don't need to interpolate
    if (mParameters[0]->IsTimeParameter())
    {
        rval = mParameters[0]->EvaluateReal();
        if (rval >= mGoal)
            goalMet = true;
    }
    else
    {
        //-----------------------------------------------------------------
        //    *** need to interpolate stop epoch
        //    *** need to convert to RefFrame for certain stop condition type
        //-----------------------------------------------------------------
        mNumValidPoints = (mNumValidPoints < mBufferSize) ?
            mNumValidPoints + 1 : mNumValidPoints;
      
        // shift values to make room for newest value
        for (int i=0; i<mBufferSize-1; i++)
            mValueBuffer[i] = mValueBuffer[i+1];

        epoch = mEpochParam->EvaluateReal();
        rval = mParameters[0]->EvaluateReal();
        mEpochBuffer[mBufferSize - 1] = epoch;
        mValueBuffer[mBufferSize - 1] = rval;
         
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
                mInterpolator->AddPoint(mValueBuffer[i], &mEpochBuffer[i]);
            if (mInterpolator->Interpolate(mGoal, &stopEpoch))
                mStopEpoch = stopEpoch;
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
bool SingleValueStop::Validate()
{
    bool valid = false;
    
    if (mNumParams >= 1)
    {
        if (mParameters[0]->IsTimeParameter())
            valid = true;
        else
            if (mEpochParam != NULL && mInterpolator != NULL)
                valid = true;
    }
    
    return valid;
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
bool SingleValueStop::SetParameter(Parameter *param)
{
    // There is no extra parameters to set for single parameter

    return true;
}
