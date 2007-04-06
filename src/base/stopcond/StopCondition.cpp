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

//#define DEBUG_BUFFER_FILLING

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
                             const Integer repeatCount,
                             Interpolator *interp)
   : BaseStopCondition(name, desc, epochParam, stopParam, goal, tol,
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
 * @return true if single parameter value stopping condition has been met;
 *   false otherwise
 *
 * @exception <StopConditionException> thrown if unable to retrieve parameter
 *   value
 *
 * @note count for repeat stop count will be implemented in the next build.
 */
//------------------------------------------------------------------------------
bool StopCondition::Evaluate()
{
   bool goalMet = false, readyToTest = true;
   Real epoch;
   Real currentParmValue;
   
   if (mStopParam == NULL || (mAllowGoalParam && mGoalParam == NULL))
      Initialize();
      
   #ifdef DEBUG_BUFFER_FILLING
      MessageInterface::ShowMessage(
         "StopCondition::Evaluate called;");
   #endif
   
   // evaluate goal
   if (mAllowGoalParam)
      mGoal = mGoalParam->EvaluateReal();

   // set current epoch
   if (mUseInternalEpoch)
      epoch = mEpoch;
   else
      epoch = mEpochParam->EvaluateReal();
      
   currentParmValue = mStopParam->EvaluateReal();

   if (isAngleParameter)
   {
      mGoal = AngleUtil::PutAngleInDegRange(mGoal, 0.0, GmatMathUtil::TWO_PI_DEG);
      
      readyToTest = CheckOnAnomaly(currentParmValue);
      if (!readyToTest)
      {
         previousValue = currentParmValue;
         previousEpoch = epoch;
      }
   }
   
   if (isApoapse)
   {
      readyToTest = CheckOnApoapsis();
      if (!readyToTest)
      {
         previousValue = currentParmValue;
         previousEpoch = epoch;
      }
   }
         
   if (isPeriapse)
   {
      readyToTest = CheckOnPeriapsis();
      if (!readyToTest)
      {
         previousValue = currentParmValue;
         previousEpoch = epoch;
      }
   }

   if (mNumValidPoints <= 2)
   {
      previousValue = currentParmValue;
      previousEpoch = epoch;
      ++mNumValidPoints;
   }
   
  
// *****************************************************************************
// Ignore update to time params for now 
// *****************************************************************************
   // for time data we don't need to interpolate
   if (mStopParam->IsTimeParameter())
   {
      #if DEBUG_STOPCOND > 1
      MessageInterface::ShowMessage
         ("StopCondition::Evaluate() mUseInternalEpoch = %d, epoch = %f, "
          "mGoal = %f, currentParmValue = %f, previousEpoch = %f\n",  
          mUseInternalEpoch, epoch, 
          mGoal, currentParmValue, previousEpoch);
      #endif
      
      // handler for time based stopping for backwards prop
      Real mult = (mGoal >= 0.0 ? 1.0 : -1.0);

      #if DEBUG_STOPCOND > 1
      MessageInterface::ShowMessage
         ("StopCondition::Evaluate() mult*currentParmValue=%g, mult*mGoal=%g, "
          "diff=%g, mTolerance=%g\n",  mult*currentParmValue, mult*mGoal, 
          Abs(mult*currentParmValue - mult*mGoal), mTolerance);
      #endif

      // use time tolerance = 1.0e-6 for equality test
      //if (mult*currentParmValue >= mult*mGoal)
      if ((Abs(mult*currentParmValue - mult*mGoal) <= 1.0e-6) ||
          (mult*currentParmValue >= mult*mGoal))
      {
         std::string stopParamType = mStopParam->GetTypeName();
         
         if (stopParamType == "ElapsedSecs")
            mStopEpoch = mBaseEpoch + mGoal/86400.0;
         else if (stopParamType == "ElapsedDays")
            mStopEpoch = mBaseEpoch + mGoal;
         //else if (stopParamType == "CurrA1MJD")
         else if (stopParamType == "A1ModJulian")
            mStopEpoch = mGoal;
         else
            throw StopConditionException
               ("StopCondition::Evaluate()::Unknown stop time param type:" +
                stopParamType + "\n");
         
         goalMet = true;
      }
      else
         previousEpoch = epoch;
      

      #if DEBUG_STOPCOND > 1
      MessageInterface::ShowMessage
         ("StopCondition::Evaluate() time goalMet = %d, mStopEpoch=%f, \n",
          goalMet, mStopEpoch);
      #endif
      
   }
// *****************************************************************************
// End of time parameter block
// *****************************************************************************
   else
   {
      Real min, max;
      min = (currentParmValue<previousValue ? currentParmValue : previousValue);
      max = (currentParmValue>previousValue ? currentParmValue : previousValue);

      #ifdef DEBUG_STOP_COND
         MessageInterface::ShowMessage(
            "Evaluating: min = %lf, max = %lf, goal = %lf\n", min, max, mGoal);
      #endif
      
      if ((min != max) && readyToTest)
      {
         if ((mGoal >= min) && (mGoal <= max))
         {
            goalMet = true;
            mStopInterval = (epoch - previousEpoch) * 86400.0;
      
            #ifdef DEBUG_STOP_COND
               MessageInterface::ShowMessage(
                  "Previous Epoch = %.12lf, Value = %.12lf\n", previousEpoch,
                  previousValue);
            #endif

         }
         else
         {
            // Save the found values in for next time through
            previousEpoch = epoch;
            previousValue = currentParmValue;
         }
      }
   }
   
   #ifdef DEBUG_BUFFER_FILLING
      MessageInterface::ShowMessage(
         "Value = %.12lf, Previous = %.12lf; Goal (%.12lf) %s\n", 
         currentParmValue, previousValue, mGoal, (goalMet ? "met" : "not met"));
   #endif
   
   return goalMet;
}


//------------------------------------------------------------------------------
// bool IsTimeCondition()
//------------------------------------------------------------------------------
/**
 * Checks to see if teh stopping condition is a time based condition.
 * 
 * @return true if this is a time based stopping condition, false otherwise.
 */
//------------------------------------------------------------------------------
bool StopCondition::IsTimeCondition()
{
   return mStopParam->IsTimeParameter();
}


//------------------------------------------------------------------------------
// bool AddToBuffer(bool isInitialPoint)
//------------------------------------------------------------------------------
/**
 * Adds a data point to the ring buffer used for interpolation.
 * 
 * @param <isInitialPoint> Flag indicating if this is the first point for this 
 * stop evaluation.
 * 
 * @return true if a stop epoch can be evaluated; false if more points are 
 * required.
 */
//------------------------------------------------------------------------------
bool StopCondition::AddToBuffer(bool isInitialPoint)
{
   #ifdef DEBUG_BUFFER_FILLING
      MessageInterface::ShowMessage(
         "StopCondition::AddToBuffer(%s) called, internalEpoch is %s\n",
         (isInitialPoint ? "true" : "false"),
         (mUseInternalEpoch ? "true" : "false"));
   #endif
   
   // For now, skip time cases because they are already built
   if (IsTimeCondition())
      return true;
   
   // Flag indicating if both the ring buffer is full, and the stop condition 
   // is bracketed
   bool retval = false;

   Real epoch;
   Real currentParmValue;
   Real stopEpoch; //in A1Mjd
   
   // evaluate goal
   if (mAllowGoalParam)
      mGoal = mGoalParam->EvaluateReal();
   
   // set current epoch
   if (mUseInternalEpoch)
      epoch = mEpoch;
   else
      epoch = mEpochParam->EvaluateReal();
      
   currentParmValue = mStopParam->EvaluateReal();

   // Force anomalies to handle wrapping
   if (isAngleParameter)
   {
      mGoal = AngleUtil::PutAngleInDegRange(mGoal, 0.0, GmatMathUtil::TWO_PI_DEG);
      if (!CheckOnAnomaly(currentParmValue))
         return false;
   }

   #ifdef DEBUG_BUFFER_FILLING
      MessageInterface::ShowMessage(
         "  New point: %.12lf, %.12lf\n",
         epoch, currentParmValue);
   #endif

   // Actions taken to initialize the ring buffer
   if (isInitialPoint)
   {
      // Reset the internal buffer and the point count
      mNumValidPoints = 1;  // We always have the data for the initial point
      
      for (int i = 0; i < mBufferSize; ++i)
      {
         mValueBuffer[i] = 0.0;
         mEpochBuffer[i] = 0.0;
      }
      
      // Fill in the data for the first point
      mValueBuffer[mBufferSize-1] = previousValue;
      
      if (mUseInternalEpoch)
         mEpochBuffer[mBufferSize-1] = 0.0; 
      else
         mEpochBuffer[mBufferSize-1] = previousEpoch;
   }

   // Roll values in the ring buffer to make room for newest value
   for (int i = 0; i < mBufferSize-1; ++i)
   {
      mEpochBuffer[i] = mEpochBuffer[i+1];
      mValueBuffer[i] = mValueBuffer[i+1];
   }

   // To do: handle the special cases: apsides and angles

   // Fill in the next data point
   mValueBuffer[mBufferSize-1] = currentParmValue;
   mEpochBuffer[mBufferSize-1] = epoch;
   ++mNumValidPoints;
   
   // Only start looking for a solution when the ring buffer is full   
   if (mNumValidPoints >= mBufferSize)
   {
      Real minVal = mValueBuffer[0], maxVal = mValueBuffer[mBufferSize - 1];
      for (int i = 0; i < mBufferSize; ++i)
      {
         if (minVal > mValueBuffer[i])
            minVal = mValueBuffer[i];
         if (maxVal < mValueBuffer[i])
            maxVal = mValueBuffer[i];
      }
      
      #ifdef DEBUG_BUFFER_FILLING
         MessageInterface::ShowMessage("Min value = %.12lf, Max = %.12lf\n",
            minVal, maxVal);
      #endif

      // If -- and only if -- the goal is bracketed, then interpolate the epoch
      if ((mGoal >= minVal) && (mGoal <= maxVal))
      {
         // Prep the interpolator
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
         
         // Finally, if we can interpolate an epoch, we have success!
         if (mInterpolator->Interpolate(mGoal, &stopEpoch))
         {
            mStopEpoch = stopEpoch;
            retval = true;
         }
         
         #if DEBUG_STOPCOND
         MessageInterface::ShowMessage
            ("StopCondition::Evaluate() mStopEpoch=%f\n", mStopEpoch);
         #endif
      }
   }

   #ifdef DEBUG_BUFFER_FILLING
      MessageInterface::ShowMessage("Valid points: %d, Buffer contents:\n",
         mNumValidPoints);
      for (int i=0; i<mBufferSize; i++)
      {
         MessageInterface::ShowMessage
            ("   [%d]   %.12lf  %.12lf\n", i, mEpochBuffer[i], mValueBuffer[i]);
      }
   #endif


   return retval;
}


//------------------------------------------------------------------------------
// Real GetStopEpoch()
//------------------------------------------------------------------------------
/**
 * Calculates the time to step (in seconds) needed to reach the stop epoch.
 * 
 * @return the time to step (in seconds)
 */
//------------------------------------------------------------------------------
Real StopCondition::GetStopEpoch()
{
   if (IsTimeCondition())
   {
      if (previousEpoch == 0.0)
         return 0.0;
      return (BaseStopCondition::GetStopEpoch() - previousEpoch) * 86400.0;
   }
      
   Real stopEpoch = 0.0;
   
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
   else
      throw StopConditionException("Unable to interpolate a stop epoch");
      
   return mStopEpoch;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the StopCondition.
 *
 * @return clone of the StopCondition.
 *
 */
//------------------------------------------------------------------------------
GmatBase* StopCondition::Clone() const
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
   // Eccentricity must be large enough to keep asculations from masking the
   // stop point
   Real ecc = mEccParam->EvaluateReal();
   Real rmag = mRmagParam->EvaluateReal();  // ???
   
   //----------------------------------------------------------------------
   // A necessary condition for periapse stop: when moving forward in time,
   // R dot V must cross from negative to positive, so previous value must be 
   // less than the goal value. 
   //----------------------------------------------------------------------
   if ((rmag <= mRange) && (ecc >= mEccTol))
   {
      if (( mBackwardsProp && (previousValue >= mGoal))  ||  
          (!mBackwardsProp && (previousValue <= mGoal)))
         return true;
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool CheckOnApoapsis()
//------------------------------------------------------------------------------
bool StopCondition::CheckOnApoapsis()
{
   // Eccentricity must be large enough to keep asculations from masking the
   // stop point
   Real ecc = mEccParam->EvaluateReal();
   
   //----------------------------------------------------------------------
   // A necessary condition for apoapse stop: when moving forward in time,
   // R dot V must cross from positive to negative, so previous value must be 
   // greater than the goal value. 
   //----------------------------------------------------------------------
   
   if (ecc >= mEccTol)
   {
      if ((mBackwardsProp && (previousValue <= mGoal))  ||  
          (!mBackwardsProp && (previousValue >= mGoal)))
         return true;
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool CheckOnAnomaly(Real anomaly)
//------------------------------------------------------------------------------
bool StopCondition::CheckOnAnomaly(Real &anomaly)
{
   Real tempGoal = AngleUtil::PutAngleInDegRange(mGoal, 0.0, GmatMathUtil::TWO_PI_DEG);
   
   #ifdef DEBUG_STOP_COND
      MessageInterface::ShowMessage(
         "CheckOnAnomaly(%.12lf), tempGoal = %.12lf\n", anomaly, tempGoal);
   #endif
      
   // set current current history to be between goal - pi and goal + pi
   anomaly =
      AngleUtil::PutAngleInDegRange(anomaly, tempGoal - GmatMathUtil::PI_DEG, 
                                    tempGoal + GmatMathUtil::PI_DEG);
   Real diff = AngleUtil::PutAngleInDegRange
      (GmatMathUtil::Abs(tempGoal - anomaly),
       0.0, GmatMathUtil::TWO_PI_DEG);
   
   if (diff <= GmatMathUtil::PI_DEG/2.0)
      return true;
   
   return false;
}


