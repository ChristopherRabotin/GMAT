//$Header$
//------------------------------------------------------------------------------
//                                  StopCondition
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
 * Implements StopCondition class.
 */
//------------------------------------------------------------------------------

#include "StopCondition.hpp"
#include "StopConditionException.hpp"
#include "MeanJ2000Equatorial.hpp"
#include "CubicSplineInterpolator.hpp"
#include "MessageInterface.hpp"

#if !defined __UNIT_TEST__
#include "Moderator.hpp"
#endif

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// StopCondition(const std::string &name, const std::string &desc,
//               Parameter *epochParam, Parameter *stopParam, const Real &goal,
//               const Real &tol, const Integer repeatCount, RefFrame *refFrame,
//               Interpolator *interp)
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
StopCondition::StopCondition(const std::string &name, const std::string &desc,
                             Parameter *epochParam, Parameter *stopParam,
                             const Real &goal, const Real &tol,
                             const Integer repeatCount, RefFrame *refFrame,
                             Interpolator *interp)
    : BaseStopCondition(name, desc, epochParam, stopParam, goal, tol,
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
    {
        BaseStopCondition::operator=(right);        
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
    if (mStopParam->Validate() == false)
    {
        throw StopConditionException
            ("Cannot evaluate the stop condition: " + 
             mStopParam->GetTypeName() + ":" + mStopParam->GetName() +
             " Validate() failed");
    }

    // set current epoch
    if (mUseInternalEpoch)
        epoch = mEpoch;
    else
        epoch = mEpochParam->EvaluateReal();
    
    // for time data we don't need to interpolate
    if (mStopParam->IsTimeParameter())
    {
        rval = mStopParam->EvaluateReal();
        
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
        //@todo loj: need to convert to RefFrame for certain stop condition type
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
        
        rval = mStopParam->EvaluateReal();
        mEpochBuffer[mBufferSize - 1] = epoch;
        mValueBuffer[mBufferSize - 1] = rval;
        
        //MessageInterface::ShowMessage("StopCondition::Evaluate() epoch=%f, rval=%f\n",
        //                              epoch, rval);

        //------------------------------------------------------------
        // Should handle Apoapsis, Periapsis parameters which need 
        // additional parameters for checking for stopping condition
        //------------------------------------------------------------
        if (mStopParamType == "Apoapsis" || mStopParamType == "Periapsis")
        {
            bool backwards = false; //loj: from Propagator?
            Real ecc = mEccParam->EvaluateReal();
 
            if (mStopParamType == "Apoapsis")
            {
                if (ecc >= mEccTol)
                {
                    if (mNumValidPoints >= 2 &&
                        ((backwards && mValueBuffer[mBufferSize-2] <= mGoal  &&
                          mGoal <= mValueBuffer[mBufferSize-1]) ||  
                         (!backwards && mValueBuffer[mBufferSize-2] >= mGoal  &&
                          mGoal >= mValueBuffer[mBufferSize-1])))
                    {
                        goalMet = true;
                    }
                }
            }
            else if (mStopParamType == "Periapsis")
            {
                 Real rmag = mRmagParam->EvaluateReal();
                
                if ((rmag <= mRange) && (ecc >= mEccTol))
                {
                    if (mNumValidPoints >= 2 &&
                        ((backwards && mValueBuffer[mBufferSize-2] >= mGoal  &&
                          mGoal > mValueBuffer[mBufferSize-1]) ||  
                         (!backwards && mValueBuffer[mBufferSize-2] <= mGoal  &&
                          mGoal < mValueBuffer[mBufferSize-1])))
                    {
                        goalMet = true;
                    }
                }
            }
        }
       else
        {
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
            }
        }

        if (goalMet)
        {
            // interpolate epoch
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
