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
// SingleValueStop(const std::string &name,
//                 Parameter *param,
//                 const Real &goal, const Real &tol,
//                 const Integer repeatCount,
//                 RefFrame *refFrame,
//                 Interpolator *interp)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of stop condition
 * @param <param> Parameter object pointer to retrive parameter value
 * @param <goal> goal for stop condition test
 * @param <tol>  tolerance for stop condition test
 * @param <repeatCount> repeat count for stop condition test
 * @param <refFrame> RefFrame object pointer to be used for stop condition test
 * @param <interp> Interpolator object pointer for interpolating stop epoch
 */
//------------------------------------------------------------------------------
SingleValueStop::SingleValueStop(const std::string &name,
                                 Parameter *param,
                                 const Real &goal, const Real &tol,
                                 const Integer repeatCount,
                                 RefFrame *refFrame,
                                 Interpolator *interp)
    : StopCondition(name, "SingleValueStop", param, goal, tol,
                    repeatCount, refFrame, interp)
{
}

//------------------------------------------------------------------------------
// SingleValueStop(const SingleValueStop &svsc)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 */
//------------------------------------------------------------------------------
SingleValueStop::SingleValueStop(const SingleValueStop &svsc)
    : StopCondition(svsc)
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
    Real val;
    
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
        val = mParameters[0]->EvaluateReal();
        if (val >= mGoal)
            goalMet = true;
    }
    else
    {
        //-----------------------------------------------------------------
        //    *** need to interpolate stop epoch
        //    *** need to convert to RefFrame for certain stop condition type
        //-----------------------------------------------------------------
        if (!mInitialized)
        {
            throw StopConditionException
                ("SingleValueStop::Evaluate(): The stopping condition ring buffer is "
                 "not initialized.");
        }
      
        mNumValidPoints = (mNumValidPoints < mBufferSize) ?
            mNumValidPoints + 1 : mNumValidPoints;
      
        // shift values to make room for newest value
        for (int i=0; i<mBufferSize-1; i++)
            mValueBuffer[i] = mValueBuffer[i+1];

        val = mParameters[0]->EvaluateReal();
        mValueBuffer[mBufferSize - 1] = val;
         
        // Stop if at least 2 points set, and either
        //   1) last value was more than goal, but current value is less,
        //   2) last value was less than goal, but current value is more.
        if (mNumValidPoints >= 2 &&
            ((mValueBuffer[mBufferSize-2] <= mGoal  &&
              mGoal <= mValueBuffer[mBufferSize-1]) ||  
             (mValueBuffer[mBufferSize-2] >= mGoal  &&
              mGoal >= mValueBuffer[mBufferSize-1]) ))
        {
            goalMet = true;
        }
    }
  
    return goalMet;
}


//------------------------------------------------------------------------------
// virtual bool Validate()
//------------------------------------------------------------------------------
/**
 * @return true if all necessary objects have been met; false otherwise
 */
//------------------------------------------------------------------------------
bool SingleValueStop::Validate()
{
    //loj: need any other checks here?
    return mInitialized;
}
