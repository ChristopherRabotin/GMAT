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
#include "CubicSplineInterpolator.hpp"
#include "RealUtilities.hpp"           // for Abs()
#include "AngleUtil.hpp"               // for PutAngleInDegRange()
#include "MessageInterface.hpp"

//#define DEBUG_STOPCOND 2
//#define DEBUG_STOPCOND_PERIAPSIS 1

using namespace GmatMathUtil;

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// StopCondition(const std::string &name, const std::string &desc,
//               Parameter *epochParam, Parameter *stopParam, const Real &goal
//               const Real &tol, const Integer repeatCount,
//               Interpolator *interp)
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
StopCondition::StopCondition(const std::string &name, const std::string &desc,
                             Parameter *epochParam, Parameter *stopParam,
                             const Real &goal, const Real &tol,
                             const Integer repeatCount, //RefFrame *refFrame,
                             Interpolator *interp)
   : BaseStopCondition(name, desc, epochParam, stopParam, goal, tol,
                       //repeatCount, refFrame, interp)
                       repeatCount, interp)
{
   objectTypeNames.push_back("StopCondition");
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
   Real stopEpoch; //in A1Mjd
   
   if (mStopParam == NULL ||
       (mAllowGoalParam && mGoalParam == NULL))
      Initialize();
   
   //loj: 7/15/05 Added evaulating goal
   // evaluate goal
   if (mAllowGoalParam)
      mGoal = mGoalParam->EvaluateReal();
   
   // set current epoch
   if (mUseInternalEpoch)
      epoch = mEpoch;
   else
      epoch = mEpochParam->EvaluateReal();
   
   // for time data we don't need to interpolate
   if (mStopParam->IsTimeParameter())
   {
      rval = mStopParam->EvaluateReal();
      
      #if DEBUG_STOPCOND > 1
      MessageInterface::ShowMessage
         ("StopCondition::Evaluate() mUseInternalEpoch = %d, epoch = %f, "
          "mGoal = %f, rval = %f\n",  mUseInternalEpoch, epoch, mGoal, rval);
      #endif
      
      // handler for time based stopping for backwards prop
      Real mult = (mGoal >= 0.0 ? 1.0 : -1.0);
      if (mult*rval >= mult*mGoal)
      {
         std::string stopParamType = mStopParam->GetTypeName();
         
         if (stopParamType == "ElapsedSecs")
            mStopEpoch = mBaseEpoch + mGoal/86400.0;
         else if (stopParamType == "ElapsedDays")
            mStopEpoch = mBaseEpoch + mGoal;
         else if (stopParamType == "CurrA1MJD")
            mStopEpoch = mGoal;
         else
            throw StopConditionException
               ("StopCondition::Evaluate()::Unknown stop time param type:" +
                stopParamType + "\n");
         
         goalMet = true;
      }
   }
   else
   {
      mNumValidPoints = (mNumValidPoints < mBufferSize) ?
         mNumValidPoints + 1 : mNumValidPoints;
      
      #if DEBUG_STOPCOND > 1
      MessageInterface::ShowMessage("StopCondition::Evaluate() mNumValidPoints=%d, "
                                    "mBufferSize=%d\n", mNumValidPoints, mBufferSize);
      #endif
      
      // shift values to make room for newest value
      for (int i=0; i<mBufferSize-1; i++)
      {
         mEpochBuffer[i] = mEpochBuffer[i+1];
         mValueBuffer[i] = mValueBuffer[i+1];
      }
      
      rval = mStopParam->EvaluateReal();
      
      #if DEBUG_STOPCOND
      MessageInterface::ShowMessage
         ("StopCondition::Evaluate() epoch=%f, mStopParam=%s, rval=%f, mGoal=%f\n",
          epoch, mStopParam->GetName().c_str(), rval, mGoal);
      #endif
      
      mEpochBuffer[mBufferSize - 1] = epoch;
      mValueBuffer[mBufferSize - 1] = rval;
      
      //------------------------------------------------------------
      // Should handle Apoapsis, Periapsis parameters which need 
      // additional parameters for checking for stopping condition
      //------------------------------------------------------------
 
      if (mStopParamType == "Apoapsis")
      {
         goalMet = CheckOnApoapsis();
      }
      else if (mStopParamType == "Periapsis")
      {
         goalMet = CheckOnPeriapsis();
      }
      else
      {
         if (mStopParamType == "TA" || mStopParamType == "MA" ||
             mStopParamType == "EA")
         {
            goalMet = CheckOnAnomaly(rval);
         }
         else
         {
            // Stop if at least <mBufferSize> points set, and either
            //   1) last value was more than goal, but current value is less,
            //   2) last value was less than goal, but current value is more.
            if (mNumValidPoints >= mBufferSize &&
                ((mValueBuffer[mBufferSize-2] >= mGoal  &&
                  mValueBuffer[mBufferSize-1] <= mGoal) ||  
                 (mValueBuffer[mBufferSize-2] <= mGoal  &&
                  mValueBuffer[mBufferSize-1] >= mGoal) ))
            {
               goalMet = true;
            }
         }
      }
      
      if (goalMet)
      {
         // interpolate epoch
         mInterpolator->Clear();
         for (int i=0; i<mBufferSize; i++)
         {
            
            #if DEBUG_STOPCOND
            MessageInterface::ShowMessage
               ("StopCondition::Evaluate() i=%d, mValueBuffer=%f, "
                "mEpochBuffer=%f\n", i, mValueBuffer[i], mEpochBuffer[i]);
            #endif
            
            mInterpolator->AddPoint(mValueBuffer[i], &mEpochBuffer[i]);
         }
         
         if (mInterpolator->Interpolate(mGoal, &stopEpoch))
            mStopEpoch = stopEpoch;
         
         #if DEBUG_STOPCOND
         MessageInterface::ShowMessage
            ("StopCondition::Evaluate() mStopEpoch=%f\n", mStopEpoch);
         #endif
      }
   }
   
   return goalMet;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the StopCondition.
 *
 * @return clone of the StopCondition.
 *
 */
//------------------------------------------------------------------------------
GmatBase* StopCondition::Clone(void) const
{
   return (new StopCondition(*this));
}


//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// bool CheckOnPeriapsis()
//------------------------------------------------------------------------------
bool StopCondition::CheckOnPeriapsis()
{   
   //bool backwards = false; //loj: from Propagator?
   Real ecc = mEccParam->EvaluateReal();
   Real rmag = mRmagParam->EvaluateReal();
   
   #if DEBUG_STOPCOND_PERIAPSIS > 1
   MessageInterface::ShowMessage
      ("CheckOnPeriapsis() ecc=%f mEccTol=%f rmag=%f mRange=%f\n",
       ecc, mEccTol, rmag, mRange);
   #endif
   
   //----------------------------------------------------------------------
   // stop if at least <mBufferSize> points set, and either
   //   1) propagating backwards, and R*V goes + to -,
   //   2) propagating forwards, and R*V goes - to +
   //----------------------------------------------------------------------
   
   if ((rmag <= mRange) && (ecc >= mEccTol))
   {
      if (mNumValidPoints >= mBufferSize &&
          //((backwards && //loj: 7/20/04 use memeber data
          ((mBackwardsProp &&
            mValueBuffer[mBufferSize-2] >= mGoal  &&
            mValueBuffer[mBufferSize-1] <= mGoal) ||  
           //(!backwards &&
           (!mBackwardsProp &&
            mValueBuffer[mBufferSize-2] <= mGoal  &&
            mValueBuffer[mBufferSize-1] >= mGoal)))
      {
         #if DEBUG_STOPCOND_PERIAPSIS
         MessageInterface::ShowMessage
            ("StopCondition::CheckOnPeriapsis() ===> Stopping condition met: "
             "epoch=%f, mGoal=%f, ecc=%f, rmag=%f\n", mEpochBuffer[mBufferSize - 1],
             mGoal, ecc, rmag);
         #endif
         
         return true;
      }
   }
   
   #if DEBUG_STOPCOND_PERIAPSIS > 1
   MessageInterface::ShowMessage
      ("CheckOnPeriapsis() mGoal=%f last=%f curr=%f\n",
       mGoal, mValueBuffer[mBufferSize-2], mValueBuffer[mBufferSize-1]);
   #endif

   return false;
}


//------------------------------------------------------------------------------
// bool CheckOnApoapsis()
//------------------------------------------------------------------------------
bool StopCondition::CheckOnApoapsis()
{
   //bool backwards = false; //loj: from Propagator?
   Real ecc = mEccParam->EvaluateReal();
   
   //----------------------------------------------------------------------
   // Stop if at least <mBufferSize> points set and eccentricity flag set, and either
   //   1) propagating backwards, and R*V goes - to +,
   //   2) propagating forwards, and R*V goes + to -
   //----------------------------------------------------------------------
   
   if (ecc >= mEccTol)
   {
      if (mNumValidPoints >= mBufferSize &&
          //((backwards &&
          ((mBackwardsProp &&
            mValueBuffer[mBufferSize-2] <= mGoal  &&
            mValueBuffer[mBufferSize-1] >= mGoal) ||  
           //(!backwards &&
           (!mBackwardsProp &&
            mValueBuffer[mBufferSize-2] >= mGoal  &&
            mValueBuffer[mBufferSize-1] <= mGoal)))
      {
         return true;
      }
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool CheckOnAnomaly(Real anomaly)
//------------------------------------------------------------------------------
bool StopCondition::CheckOnAnomaly(Real anomaly)
{
   Real tempGoal = AngleUtil::PutAngleInDegRange(mGoal, 0.0, GmatMathUtil::TWO_PI_DEG);
   // set current current history to be between goal - pi and goal + pi
   mValueBuffer[mBufferSize-1] =
      AngleUtil::PutAngleInDegRange(anomaly, tempGoal - GmatMathUtil::PI_DEG, 
                                    tempGoal + GmatMathUtil::PI_DEG);
   Real diff = AngleUtil::PutAngleInDegRange
      (GmatMathUtil::Abs(tempGoal - mValueBuffer[mBufferSize-1]),
       0.0, GmatMathUtil::TWO_PI_DEG);
   
   #if DEBUG_STOPCOND
   MessageInterface::ShowMessage
      ("CheckOnAnomaly() mGoal=%f tempGoal=%f diff=%f last=%f curr=%f\n",
       mGoal, tempGoal, diff, mValueBuffer[mBufferSize-2],
       mValueBuffer[mBufferSize-1]);
   #endif
   
   //----------------------------------------------------------------------   
   // Stop if at least <mBufferSize> points set and difference is <= pi/2, and either
   //   1) last TA was less than goal, and current TA is greater, or
   //   2) last TA was greater than goal, and current TA is less 
   //----------------------------------------------------------------------
   if (mNumValidPoints >= mBufferSize && diff <= GmatMathUtil::PI_DEG/2.0 &&
       ((mValueBuffer[mBufferSize-2] <= tempGoal  &&
         mValueBuffer[mBufferSize-1] >= tempGoal) ||  
        (mValueBuffer[mBufferSize-2] >= tempGoal  &&
         mValueBuffer[mBufferSize-1] <= tempGoal) ))
   {
      return true;
   }
   
   return false;
}


