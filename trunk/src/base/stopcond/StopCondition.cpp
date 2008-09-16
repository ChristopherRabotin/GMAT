//$Id$
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
#include "NotAKnotInterpolator.hpp"
#include "RealUtilities.hpp"           // for Abs()
#include "AngleUtil.hpp"               // for PutAngleInDegRange()
#include "KeplerianParameters.hpp"     // for KepEcc()
#include "SphericalParameters.hpp"     // for SphRMag()
#include "StringUtil.hpp"              // for ReplaceName()
#include "MessageInterface.hpp"


//#define DEBUG_BASE_STOPCOND 1
//#define DEBUG_BASE_STOPCOND_INIT 1
//#define DEBUG_BASE_STOPCOND_GET 1
//#define DEBUG_BASE_STOPCOND_SET 1
//#define DEBUG_RENAME
//#define DEBUG_STOPCOND 2
//#define DEBUG_STOPCOND_PERIAPSIS 1
//#define DEBUG_BUFFER_FILLING
//#define DEBUG_CYCLIC_PARAMETERS


using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------
const Real StopCondition::STOP_COND_TOL = 1.0e-11;

const std::string
StopCondition::PARAMETER_TEXT[StopConditionParamCount - GmatBaseParamCount] =
{
   "BaseEpoch",
   "Epoch",
   "EpochVar",
   "StopVar",
   "Goal",
   "Repeat",
};

const Gmat::ParameterType
StopCondition::PARAMETER_TYPE[StopConditionParamCount - GmatBaseParamCount] =
{
   Gmat::REAL_TYPE,    //"BaseEpoch",
   Gmat::REAL_TYPE,    //"Epoch",
   Gmat::STRING_TYPE,  //"EpochVar",
   Gmat::STRING_TYPE,  //"StopVar",
   Gmat::STRING_TYPE,  //"Goal",
   Gmat::INTEGER_TYPE, //"Repeat",
};

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
   : GmatBase(Gmat::STOP_CONDITION, "StopCondition", name),
     mBaseEpoch         (0.0),
     mEpoch             (0.0),
     mGoal              (goal),
     mRepeatCount       (repeatCount),
     mSolarSystem       (NULL),
     mInterpolator      (interp),
     mDescription       (desc),
     mStopParamType     (""),
     mStopParamName     (""),
     mEpochParamName    (""),
     mGoalStr           ("0.0"),
     mStopParam         (stopParam),
     mGoalParam         (NULL),
     mEpochParam        (epochParam),
     mEccParam          (NULL),
     mRmagParam         (NULL),
     mNumValidPoints    (0),
     mBufferSize        (0),
     mStopEpoch         (REAL_PARAMETER_UNDEFINED),
     mStopInterval      (0.0),
     previousEpoch      (-999999.999999),
     previousValue      (-999999.999999),
     mUseInternalEpoch  (false),
     mInitialized       (false),
     mNeedInterpolator  (false),
     mAllowGoalParam    (false),
     mBackwardsProp     (false),
     activated          (true),
     isAngleParameter   (false),
     isCyclicCondition(false),
     isPeriapse         (false),
     isApoapse          (false),
     scCycleType        (NOT_CYCLIC)
{
   objectTypeNames.push_back("StopCondition");

   objectTypes.push_back(Gmat::STOP_CONDITION);
   objectTypeNames.push_back("StopCondition");
   
   if (mStopParam != NULL)
   {
      mStopParamType = mStopParam->GetTypeName();
      mStopParamName = mStopParam->GetName();
   }
   
   if (epochParam == NULL)
      mUseInternalEpoch = true;
   
   // Create default Interpolator
   if (mInterpolator == NULL)
      mInterpolator = new NotAKnotInterpolator("InternalInterpolator");
}


//------------------------------------------------------------------------------
// StopCondition(const StopCondition &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 */
//------------------------------------------------------------------------------
StopCondition::StopCondition(const StopCondition &copy)
   : GmatBase(copy),
     mBaseEpoch         (copy.mBaseEpoch),
     mEpoch             (copy.mEpoch),
     mGoal              (copy.mGoal),
     mRepeatCount       (copy.mRepeatCount),
     mSolarSystem       (copy.mSolarSystem),
     mDescription       (copy.mDescription),
     mStopParamType     (copy.mStopParamType),
     mStopParamName     (copy.mStopParamName),
     mEpochParamName    (""),
     mGoalStr           (copy.mGoalStr),
     mStopParam         (copy.mStopParam),
     mGoalParam         (copy.mGoalParam),
     mEpochParam        (copy.mEpochParam),
     mEccParam          (NULL),
     mRmagParam         (NULL),
     mNumValidPoints    (0),
     mBufferSize        (0),
     mStopEpoch         (copy.mStopEpoch),
     mStopInterval      (0.0),
     previousEpoch      (-999999.999999),
     previousValue      (-999999.999999),
     mUseInternalEpoch  (copy.mUseInternalEpoch),
     mInitialized       (copy.mInitialized),
     mNeedInterpolator  (copy.mNeedInterpolator),
     mAllowGoalParam    (copy.mAllowGoalParam),
     mBackwardsProp     (copy.mBackwardsProp),
     activated          (copy.activated),
     
     isAngleParameter   (copy.isAngleParameter),
     isCyclicCondition(copy.isCyclicCondition),
     isPeriapse         (copy.isPeriapse),
     isApoapse          (copy.isApoapse)
{
   mAllRefObjectNames = copy.mAllRefObjectNames;  // Is this correct?

   if (copy.mInterpolator != NULL)
      if (copy.mInterpolator->GetName() == "InternalInterpolator")
         mInterpolator = (Interpolator*)copy.mInterpolator->Clone();

   if (copy.mEccParam != NULL)
      mEccParam = (Parameter*)copy.mEccParam->Clone();
   if (copy.mRmagParam != NULL)
      mRmagParam = (Parameter*)copy.mRmagParam->Clone();
   
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
      GmatBase::operator=(right);
      
      mAllRefObjectNames = right.mAllRefObjectNames; // Is this correct?
      
      mBaseEpoch = right.mBaseEpoch;
      mEpoch = right.mEpoch;
      mGoal = right.mGoal;
      mRepeatCount = right.mRepeatCount;
   
      if (right.mInterpolator != NULL)
         if (right.mInterpolator->GetName() == "InternalInterpolator")
            mInterpolator = (Interpolator*)right.mInterpolator->Clone();
   
      mSolarSystem = right.mSolarSystem;
      mDescription = right.mDescription;
      mStopParamType = right.mStopParamType;
      mStopParamName = right.mStopParamName;
      mGoalStr = right.mGoalStr;
      
      mStopEpoch = right.mStopEpoch;
      mStopInterval = right.mStopInterval;
      mStopParam = right.mStopParam;
      mEpochParam = right.mEpochParam;
      
      mGoalParam = (Parameter*)right.mGoalParam;
         
      if (right.mEccParam != NULL)
         mEccParam = (Parameter*)right.mEccParam->Clone();
      else
         mEccParam = NULL;
         
      if (right.mRmagParam != NULL)
         mRmagParam = (Parameter*)right.mRmagParam->Clone();
      else
         mRmagParam = NULL;
      
      mInitialized = right.mInitialized;
      mUseInternalEpoch = right.mUseInternalEpoch;
      mNeedInterpolator = right.mNeedInterpolator;
      mAllowGoalParam = right.mAllowGoalParam;
      mBackwardsProp = right.mBackwardsProp;
      activated = right.activated;
      
      previousEpoch = -999999.999999;
      previousValue = -999999.999999;
      
      isAngleParameter = right.isAngleParameter;
      isCyclicCondition = right.isCyclicCondition;
      isPeriapse       = right.isPeriapse;
      isApoapse        = right.isApoapse;
      
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
   if (mEccParam != NULL)
      delete mEccParam;

   if (mRmagParam != NULL)
      delete mRmagParam;

   if (mInterpolator != NULL)
   {
      if (mInterpolator->GetName() == "InternalInterpolator")
      {
         delete mInterpolator;
      }
   }
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
         "StopCondition::Evaluate called\n");
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
   else if (isCyclicCondition)
   {
      readyToTest = CheckCyclicCondition(currentParmValue);
      
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

   if (mNumValidPoints == 0)
   {
      previousValue = currentParmValue;
      previousEpoch = epoch;
      ++mNumValidPoints;
      return false;
   }
   
   if (!mStopParam->IsTimeParameter())
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
         if ((mGoal >= min) && (mGoal <= max) && activated)
         {
            goalMet = true;
            mStopInterval = (epoch - previousEpoch) * 86400.0;
      
            #ifdef DEBUG_STOP_COND
               MessageInterface::ShowMessage(
                  "Previous Epoch = %.12lf, Epoch  %.12lf, Values = [%.12lf  %.12lf], "
                  "StopInterval = %.12lf\n", previousEpoch, epoch,
                  previousValue, currentParmValue, mStopInterval);
            #endif
         }
         else if (activated)
         {
            // Save the found values in for next time through
            previousEpoch = epoch;
            previousValue = currentParmValue;
         }
      }
   }
   else
   // for time data we don't need to interpolate
   {
      Real prevGoalDiff = previousValue - mGoal,
           currGoalDiff = currentParmValue - mGoal;
                       
      Real direction = 
           (currGoalDiff - prevGoalDiff > 0.0 ? 1.0 : -1.0);

      #if DEBUG_STOPCOND > 1
         MessageInterface::ShowMessage(
            "prev = %15.9lf, curr = %15.9lf, lhs = %15.9lf, rhs = %15.9lf, "
            "direction = %15.9lf, \n", previousValue, currentParmValue, 
            prevGoalDiff, currGoalDiff, direction);
      #endif

      if (mNumValidPoints == 1)
         if (((2.0*mGoal - currentParmValue - previousValue) * direction) < 0.0)
            MessageInterface::ShowMessage(
               "Warning!  Time based stopping condition \"%s\" = %.10lf will "
               "never be satisfied\n",
               instanceName.c_str(), mGoal);
      
      // Goal met if it falls between previous and current values
      if ((currGoalDiff*direction >= 0.0) && 
          (prevGoalDiff*direction <= 0.0) && activated)
      {
         goalMet = true;
         
         #if DEBUG_STOPCOND > 1
            MessageInterface::ShowMessage
               ("StopCondition::Evaluate() mUseInternalEpoch = %d, "
               "epoch = %15.9lf, mGoal = %15.9lf, "
               "currentParmValue = %15.9lf, "
               "previousValue = %15.9lf, "
               "GoalDiffs = [%15.9lf  %15.9lf]\n",  
                mUseInternalEpoch, epoch, 
                mGoal, currentParmValue, previousValue, 
                prevGoalDiff, currGoalDiff);
         #endif
      }
      else if (activated)
      {
         previousValue = currentParmValue;
         previousEpoch = epoch;
      }
   }
   
   #ifdef DEBUG_BUFFER_FILLING
      MessageInterface::ShowMessage(
         "Value = %.12lf, Previous = %.12lf; Goal (%.12lf) %s, epoch = %15.12lf\n", 
         currentParmValue, previousValue, mGoal, (goalMet ? "met" : "not met"), epoch);
   #endif
   
   ++mNumValidPoints;
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
   else if(isCyclicCondition)
   {
      mGoal = PutInRange(mGoal, 0.0, GmatMathUtil::TWO_PI_DEG);
      if (!CheckCyclicCondition(currentParmValue))
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
      
      #if DEBUG_STOPCOND
         MessageInterface::ShowMessage
            ("StopCondition::GetStopEpoch()\n   Previous = %15.9lf\n   "
            "Current = %15.9lf\n   Goal = %15.9lf", previousEpoch, previousValue, 
            mGoal);
      #endif

      Real dt = (mGoal - previousValue) * GetTimeMultiplier();
      return dt;
   }
      
   Real stopEpoch = 0.0;
   
   #if DEBUG_STOPCOND
      MessageInterface::ShowMessage
         ("StopCondition::Evaluate()\n   Ring buffer:\n");
   #endif

   mInterpolator->Clear();
   for (int i=0; i<mBufferSize; i++)
   {
      #if DEBUG_STOPCOND
         MessageInterface::ShowMessage
            ("      i=%d, mValueBuffer=%.12lf, "
             "mEpochBuffer=%.12lf\n", i, mValueBuffer[i], mEpochBuffer[i]);
      #endif
      
      mInterpolator->AddPoint(mValueBuffer[i], &mEpochBuffer[i]);
   }
   
   if (mInterpolator->Interpolate(mGoal, &stopEpoch))
      mStopEpoch = stopEpoch;
   else
      throw StopConditionException("Unable to interpolate a stop epoch");
  
   #if DEBUG_STOPCOND
      MessageInterface::ShowMessage
         ("   Interpolated epoch = %.12lf\n", mStopEpoch);
   #endif
      
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
   // Eccentricity must be large enough to keep osculations from masking the
   // stop point
   Real ecc = mEccParam->EvaluateReal();
//   Real rmag = mRmagParam->EvaluateReal();  // ???
   
   //----------------------------------------------------------------------
   // A necessary condition for periapse stop: when moving forward in time,
   // R dot V must cross from negative to positive, so previous value must be 
   // less than the goal value. 
   //----------------------------------------------------------------------
//   if ((rmag <= mRange) && (ecc >= mEccTol))
   if (ecc >= 1.0e-6)
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
   if (mEccParam == NULL)
      throw StopConditionException
         ("StopCondition::CheckOnApoapsis() ECC parameter has NULL pointer.\n");
   
   // Eccentricity must be large enough to keep asculations from masking the
   // stop point
   
   Real ecc = mEccParam->EvaluateReal();
   
   //----------------------------------------------------------------------
   // A necessary condition for apoapse stop: when moving forward in time,
   // R dot V must cross from positive to negative, so previous value must be 
   // greater than the goal value. 
   //----------------------------------------------------------------------
   
//   if (ecc >= mEccTol)
   if (ecc >= 1.0e-6)
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


//------------------------------------------------------------------------------
// bool CheckCyclicCondition(Real &value)
//------------------------------------------------------------------------------
/**
 * Performs range setting for cyclic stopping conditions, and checks to see that
 * the condition falls near the goal.
 * 
 * @param <value> Reference to the value of the parameter. On return from this 
 *                call, the input variable is changed to fall in the valid range
 *                for the variable, mapped around the current goal.
 * 
 * @return true if the stopping condition is near the goal, false otherwise.
 */ 
//------------------------------------------------------------------------------
bool StopCondition::CheckCyclicCondition(Real &value)
{
   Real min, max;
   bool retval = false;
   
   if (GetRange(min, max))
   {
      Real range2 = (max - min) / 2.0;
      mGoal = PutInRange(mGoal, min, max);
      value = PutInRange(value, mGoal - range2, 
                   mGoal + range2);
      previousValue = PutInRange(previousValue, mGoal - range2, 
                   mGoal + range2);

      #ifdef DEBUG_CYCLIC_PARAMETERS
         MessageInterface::ShowMessage(
            "Cyclic condition: Goal = %lf, currValue = %lf, prevValue = %lf\n",
            mGoal, value, previousValue);
      #endif
             
      if (fabs(mGoal - value) < (range2 / 2.0))
         retval = true;
   }
   
   return retval;
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool StopCondition::Initialize()
{
   #if DEBUG_BASE_STOPCOND_INIT
   MessageInterface::ShowMessage("StopCondition::Initialize() entered\n");
   #endif
   
   mInitialized = false;
   isApoapse = false;
   isPeriapse = false;
   mStopInterval = 0.0;
   
   // clear local parameters
   if (mEccParam != NULL)
      delete mEccParam;

   if (mRmagParam != NULL)
      delete mRmagParam;

   mEccParam = NULL;
   mRmagParam = NULL;

   if (Validate())
   {
      if (mStopParamType == "Apoapsis" ||
          mStopParamType == "Periapsis")
      {
         mGoal = 0.0;
         mAllowGoalParam = false;
         if (mStopParamType == "Apoapsis")
            isApoapse = true;
         if (mStopParamType == "Periapsis")
            isPeriapse = true;
      }
            
      std::string paramTypeName = mStopParam->GetTypeName();

      if (paramTypeName == "TA" || paramTypeName == "MA" ||
          paramTypeName == "EA" || paramTypeName == "Longitude")
      {
         isCyclicCondition = true;
         
         if (mStopParamType == "TA" || mStopParamType == "MA" ||
             mStopParamType == "EA")
            scCycleType = ZERO_360;
         if (paramTypeName == "Longitude")
            scCycleType = PLUS_MINUS_180;
      }
      
      if (mNeedInterpolator)
      {
         mBufferSize = mInterpolator->GetBufferSize();
         mEpochBuffer.reserve(mBufferSize);
         mValueBuffer.reserve(mBufferSize);
         
         for (int i=0; i<mBufferSize; i++)
         {
            mEpochBuffer.push_back(0.0);
            mValueBuffer.push_back(0.0);
         }
         
         mNumValidPoints = 0;
         mInitialized = true;
      }
      else
      {
         mInitialized = true;
      }
   }
   
   #if DEBUG_BASE_STOPCOND_INIT
   MessageInterface::ShowMessage
      ("StopCondition::Initialize() returning mInitialized=%d\n", mInitialized);
   #endif
   
   return mInitialized;
}


//------------------------------------------------------------------------------
// virtual bool Validate()
//------------------------------------------------------------------------------
/**
 * @return true if all necessary objects have been set; false otherwise
 *
 * @exception thrown if necessary object pointer is NULL
 */
//------------------------------------------------------------------------------
bool StopCondition::Validate()
{   
   #if DEBUG_STOPCOND_INIT   
   MessageInterface::ShowMessage
      ("StopCondition::Validate() mUseInternalEpoch=%d, mEpochParam=<%p>, "
       "mStopParam=<%p>, mAllowGoalParam=%d, mGoalParam=<%p>\n", mUseInternalEpoch,
       mEpochParam, mStopParam, mAllowGoalParam, mGoalParam);
   #endif
   
   // check on epoch parameter
   if (!mUseInternalEpoch && mEpochParam == NULL)
   {
      throw StopConditionException
         ("StopCondition::Validate() epoch parameter: " + mEpochParamName +
          " has NULL pointer.\n");
   }
   
   // check on stop parameter
   if (mStopParam == NULL)
   {
      throw StopConditionException
         ("StopCondition::Validate() stop parameter: " + mStopParamName +
          " has NULL pointer.\n");
   }
   
   // check on interpolator
   if (mStopParam->IsTimeParameter())
   {
      mNeedInterpolator = false;
      // Set time parameter type
      std::string timeTypeName = mStopParam->GetTypeName();
      if (timeTypeName == "ElapsedSecs")
         stopParamTimeType = SECOND_PARAM;
      else if (timeTypeName == "ElapsedDays")
         stopParamTimeType = DAY_PARAM;
      else if (timeTypeName.find("ModJulian") != std::string::npos)
         stopParamTimeType = EPOCH_PARAM;
      else
         stopParamTimeType = UNKNOWN_PARAM_TIME_TYPE;
      
      #if DEBUG_BASE_STOPCOND_INIT   
         MessageInterface::ShowMessage(
            "Stop parameter \"%s\" has time type %d\n", timeTypeName.c_str(), 
            stopParamTimeType);
      #endif
   }
   else
   {
      if (mInterpolator == NULL)
      {
         throw StopConditionException
            ("StopCondition::Validate() Interpolator has NULL pointer.\n");
//            ("StopCondition::Validate() Interpolator: " + mInterpolatorName +
//             " has NULL pointer.\n");
      }
         
      mNeedInterpolator = true;
   }
   
   // check on goal parameter
   if (mAllowGoalParam && mGoalParam == NULL)
      throw StopConditionException
         ("StopCondition::Validate() goal parameter: " + mGoalStr +
          " has NULL pointer.\n");
   
   // Apoapsis and Periapsis need additional parameters
   if (mStopParamType == "Apoapsis" ||
       mStopParamType == "Periapsis")
   {
      // check on Ecc parameter
      if (mEccParam  == NULL)
      {
         #if DEBUG_BASE_STOPCOND
         MessageInterface::ShowMessage
            ("StopCondition::Validate(): Creating KepEcc...\n");
         #endif
         
         mEccParam = new KepEcc("");
         
         mEccParam->AddRefObject
            (mStopParam->GetRefObject(Gmat::SPACECRAFT, 
                                      mStopParam->GetRefObjectName(Gmat::SPACECRAFT)));
         mEccParam->AddRefObject
            (mStopParam->GetRefObject(Gmat::COORDINATE_SYSTEM, 
                                      mStopParam->GetRefObjectName(Gmat::COORDINATE_SYSTEM)));
         
         mEccParam->AddRefObject
            (mStopParam->GetRefObject(Gmat::SPACE_POINT, 
                                      mStopParam->GetRefObjectName(Gmat::SPACE_POINT)),
             true);
         
         mEccParam->SetInternalCoordSystem(mStopParam->GetInternalCoordSystem());            
         mEccParam->AddRefObject(mSolarSystem);
         mEccParam->Initialize();
      }
      
      // check on SphRMag parameter if "Periapsis"
      if (mStopParamType == "Periapsis")
      {
         if (mRmagParam == NULL)
         {
            #if DEBUG_BASE_STOPCOND_INIT
            MessageInterface::ShowMessage
               ("StopCondition::Validate(): Creating SphRMag...\n");
            #endif
            
            std::string depObjName = mStopParam->GetStringParameter("DepObject");
            
            #if DEBUG_BASE_STOPCOND_INIT
            MessageInterface::ShowMessage
               ("StopCondition::Validate() depObjName of mStopParam=%s\n",
                depObjName.c_str());
            #endif
            
            mRmagParam = new SphRMag("");
            mRmagParam->SetStringParameter("DepObject", depObjName);
            
            mRmagParam->AddRefObject
               (mStopParam->GetRefObject(Gmat::SPACECRAFT,
                                         mStopParam->GetRefObjectName(Gmat::SPACECRAFT)));
            mRmagParam->AddRefObject
               (mStopParam->GetRefObject(Gmat::COORDINATE_SYSTEM,
                                         mStopParam->GetRefObjectName(Gmat::COORDINATE_SYSTEM)));
            mRmagParam->AddRefObject
               (mStopParam->GetRefObject(Gmat::SPACE_POINT,
                                         mStopParam->GetRefObjectName(Gmat::SPACE_POINT)),
                true);
            
            mRmagParam->SetInternalCoordSystem(mStopParam->GetInternalCoordSystem());
            mRmagParam->AddRefObject(mSolarSystem);
            mRmagParam->Initialize();
            
//            // set mRange
//            if (depObjName == "Earth")
//               mRange = 5.0e5;
//            else if (depObjName == "Luna")
//               //mRange = 2.0e5; // Swingby has this.
//               mRange = 5.0e5;
//            else
//               mRange = 1.0e10;
         }
      }
   }
   
   #if DEBUG_BASE_STOPCOND_INIT
   MessageInterface::ShowMessage
      ("StopCondition::Validate() mUseInternalEpoch=%d, mEpochParam=%p, "
       "mInterpolator=%p\n   mStopParamType=%s, mStopParamName=%s, mStopParam=%s<%p>\n",
       mUseInternalEpoch, mEpochParam, mInterpolator, mStopParamType.c_str(),
       mStopParamName.c_str(), mStopParam->GetTypeName().c_str(), mStopParam);
   #endif
   
   return true;
}

void StopCondition::Reset()
{
   mNumValidPoints = 0;
}

Real StopCondition::GetStopInterval()
{
   return mStopInterval;
}


//------------------------------------------------------------------------------
// bool IsInitialized()
//------------------------------------------------------------------------------
bool StopCondition::IsInitialized()
{
   return mInitialized;
}


//------------------------------------------------------------------------------
// Integer GetBufferSize()
//------------------------------------------------------------------------------
Integer StopCondition::GetBufferSize()
{
   return mBufferSize;
}


//------------------------------------------------------------------------------
// std::string& GetDescription()
//------------------------------------------------------------------------------
std::string& StopCondition::GetDescription()
{
   return mDescription;
}


//------------------------------------------------------------------------------
// Parameter* GetEpochParameter()
//------------------------------------------------------------------------------
Parameter* StopCondition::GetEpochParameter()
{
   return mEpochParam;
}


//------------------------------------------------------------------------------
// Parameter* GetStopParameter()
//------------------------------------------------------------------------------
Parameter* StopCondition::GetStopParameter()
{
   return mStopParam;
}


//------------------------------------------------------------------------------
// Parameter* GetStopParameter()
//------------------------------------------------------------------------------
Parameter* StopCondition::GetGoalParameter()
{
   return mGoalParam;
}


//------------------------------------------------------------------------------
// Interpolator* GetInterpolator()
//------------------------------------------------------------------------------
Interpolator* StopCondition::GetInterpolator()
{
   return mInterpolator;
}


//------------------------------------------------------------------------------
// void SetDescription(const std::string &desc)
//------------------------------------------------------------------------------
void StopCondition::SetDescription(const std::string &desc)
{
   mDescription = desc;
}


//------------------------------------------------------------------------------
// void SetPropDirection(Real dir)
//------------------------------------------------------------------------------
void StopCondition::SetPropDirection(Real dir)
{   
   if (dir >= 1.0)
      mBackwardsProp = false;
   else
      mBackwardsProp = true;
   
   #if DEBUG_BASE_STOPCOND
   MessageInterface::ShowMessage
      ("StopCondition::SetPropDirection() dir=%f, mBackwardsProp=%d\n",
       dir, mBackwardsProp);
   #endif
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *solarSystem)
//------------------------------------------------------------------------------
/**
 * Sets SolarSystem
 */
//------------------------------------------------------------------------------
void StopCondition::SetSolarSystem(SolarSystem *solarSystem)
{
   if (solarSystem != NULL)
   {
      mSolarSystem = solarSystem;
   }
   else
   {
      throw StopConditionException(
         "Attempting to set solar system with NULL pointer.");
   }
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
bool StopCondition::SetInterpolator(Interpolator *interp)
{
   if (interp != NULL)
   {
      if (mInterpolator->GetName() == "InternalInterpolator")
         delete mInterpolator;
      
      mInterpolator = interp;
      return true;
   }
   else
   {
      return false;
   }
}


//------------------------------------------------------------------------------
// bool SetEpochParameter(Parameter *param)
//------------------------------------------------------------------------------
/**
 * Sets epoch parameter which will be used to interpolate stop epoch
 *
 * @return true if EpochParam is successfully set; false otherwise
 */
//------------------------------------------------------------------------------
bool StopCondition::SetEpochParameter(Parameter *param)
{
   if (param != NULL)
   {
      mEpochParam = param;
      mUseInternalEpoch = false;
      return true;
   }
   else
   {
      return false;
   }
}


//------------------------------------------------------------------------------
// virtual bool SetStopParameter(Parameter *param)
//------------------------------------------------------------------------------
/**
 * Add parameter to stop condition.
 *
 * @return true if parameter has added to array.
 */
//------------------------------------------------------------------------------
bool StopCondition::SetStopParameter(Parameter *param)
{
   #ifdef DEBUG_STOP_PARAM
   MessageInterface::ShowMessage
      ("StopCondition::SetStopParameter() param=<%p>\n", param);
   #endif
   
   if (param != NULL)
   {
      mStopParam = param;
      mStopParamType = mStopParam->GetTypeName();
      
      if (param->IsTimeParameter())
         mInitialized = true;
      
      return true;
   }
   
   return false;
}


//------------------------------------------------------------------------------
// virtual bool SetGoalParameter(Parameter *param)
//------------------------------------------------------------------------------
/**
 * Sets goal parameter to stop condition.
 *
 * @return true if parameter has added to array.
 */
//------------------------------------------------------------------------------
bool StopCondition::SetGoalParameter(Parameter *param)
{
   #ifdef DEBUG_STOPCOND_SET
   MessageInterface::ShowMessage
      ("StopCondition::SetGoalParameter() param=<%p>\n", param);
   #endif
   
   mGoalParam = param;
   return true;
}


//------------------------------------------------------------------------------
// void SetGoalString(const std::string &str)
//------------------------------------------------------------------------------
void StopCondition::SetGoalString(const std::string &str)
{
   mGoalStr = str;
   
   // remove leading blanks
   std::string::size_type pos = mGoalStr.find_first_not_of(' ');
   mGoalStr.erase(0, pos);
   
   // if str is just a number
   if (isdigit(mGoalStr[0]) || mGoalStr[0] == '.' || mGoalStr[0] == '-')
   {
      mGoal = atof(mGoalStr.c_str());
      mAllowGoalParam = false;
   }
   else
   {
      mAllowGoalParam = true;
   }
   
   #if DEBUG_BASE_STOPCOND
   MessageInterface::ShowMessage
      ("StopCondition::SetGoalString() mAllowGoalParam=%d, mGoalStr=<%s>, "
       "mGoal=%le\n", mAllowGoalParam, mGoalStr.c_str(), mGoal);
   #endif
   
}


//------------------------------------------------------------------------------
// virtual bool SetSpacecraft(Spacecraft *sc)
//------------------------------------------------------------------------------
/**
 * Sets spacecraft pointer to internal parameter used in stop condition.
 *
 * @return true if spacecraft has been set.
 */
//------------------------------------------------------------------------------
bool StopCondition::SetSpacecraft(SpaceObject *sc)
{
   if (mEccParam != NULL)
      mEccParam->SetRefObject(sc, Gmat::SPACECRAFT, sc->GetName());
   
   if (mRmagParam != NULL)
      mRmagParam->SetRefObject(sc, Gmat::SPACECRAFT, sc->GetName());
   
   return true;
}


//---------------------------------
// methods inherited from GmatBase
//---------------------------------

//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool StopCondition::RenameRefObject(const Gmat::ObjectType type,
                                    const std::string &oldName,
                                    const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("StopCondition::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type != Gmat::SPACECRAFT && type != Gmat::PARAMETER)
      return true;
   
   //set new StopCondition name
   std::string name = GetName();
   std::string::size_type pos = name.find(oldName);
   if (pos != name.npos)
   {
      name = GmatStringUtil::ReplaceName(name, oldName, newName);
      SetName(name);
   }
   
   //set new epoch parameter name
   pos = mEpochParamName.find(oldName);
   if (pos != mEpochParamName.npos)
      mEpochParamName = GmatStringUtil::ReplaceName(mEpochParamName, oldName, newName);
   
   //set new stop parameter name
   pos = mStopParamName.find(oldName);
   if (pos != mStopParamName.npos)
      mStopParamName = GmatStringUtil::ReplaceName(mStopParamName, oldName, newName);
   
   //set new stop goal string
   pos = mGoalStr.find(oldName);
   if (pos != mStopParamName.npos)
      mGoalStr = GmatStringUtil::ReplaceName(mGoalStr, oldName, newName);
   
   return true;
}


//---------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//---------------------------------------------------------------------------
/**
 * Retrieves reference object name array for given type. It will return all
 * object names if type is Gmat::UNKNOWN_NAME.
 *
 * @param <type> object type
 * @return reference object name.
 */
//------------------------------------------------------------------------------
const StringArray&
StopCondition::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   mAllRefObjectNames.clear();
   
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::PARAMETER)
   {
      mAllRefObjectNames.push_back(mStopParamName);
      if (mAllowGoalParam)
         mAllRefObjectNames.push_back(mGoalStr);
   }
   
   return mAllRefObjectNames;
}


//---------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                   const std::string &name)
//---------------------------------------------------------------------------
bool StopCondition::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                 const std::string &name)
{
   #if DEBUG_BASE_STOPCOND_GET
   MessageInterface::ShowMessage
      ("StopCondition::SetRefObject() Name=%s, mStopParamName=%s, mGoalStr=%s\n",
       this->GetName().c_str(), mStopParamName.c_str(), mGoalStr.c_str());
   #endif
   
   if (type == Gmat::PARAMETER)
   {
      if (name == mStopParamName)
         SetStopParameter((Parameter*)obj);
      if (name == mGoalStr)
         SetGoalParameter((Parameter*)obj);
      if (name == mEpochParamName)
         SetEpochParameter((Parameter*)obj);
      
      return true;
   }
   else if (type == Gmat::INTERPOLATOR)
   {
      SetInterpolator((Interpolator*)obj);
      return true;
   }
   
   return GmatBase::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string StopCondition::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < StopConditionParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterText(id);
    
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer StopCondition::GetParameterID(const std::string &str) const
{
   #if DEBUG_BASE_STOPCOND_GET > 1
   MessageInterface::ShowMessage
      ("StopCondition::GetParameterID() str = %s\n", str.c_str());
   #endif
   
   for (int i=GmatBaseParamCount; i<StopConditionParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType StopCondition::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < StopConditionParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string StopCondition::GetParameterTypeString(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < StopConditionParamCount)
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
//   case TOLERANCE:
//      return mTolerance;
//   case ECC_TOL:
//      return mEccTol;
//   case RANGE:
//      return mRange;
   case BASE_EPOCH:
      return mBaseEpoch;
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
//   case TOLERANCE:
//      mTolerance = value;
//      return mTolerance;
//   case ECC_TOL:
//      mEccTol = value;
//      return mEccTol;
//   case RANGE:
//      mRange = value;
//      return mRange;
   case BASE_EPOCH:
      mBaseEpoch = value;
      return mBaseEpoch;
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
   #if DEBUG_BASE_STOPCOND_SET
   MessageInterface::ShowMessage
      ("StopCondition::SetRealParameter() label=%s, "
       "value=%le\n", label.c_str(), value);
   #endif
   
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
      return mEpochParamName;
   case STOP_VAR:
      return mStopParamName;
   case GOAL:
      return mGoalStr;
//   case INTERPOLATOR:
//      return mInterpolatorName;
   default:
      return GmatBase::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string StopCondition::GetStringParameter(const std::string &label) const
{
   #if DEBUG_BASE_STOPCOND_GET
   MessageInterface::ShowMessage
      ("StopCondition::GetStringParameter() label = %s\n", label.c_str());
   #endif
   
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool StopCondition::SetStringParameter(const Integer id, const std::string &value)
{
   #if DEBUG_BASE_STOPCOND_SET
   MessageInterface::ShowMessage
      ("StopCondition::SetStringParameter() id = %d, value = %s \n",
       id, value.c_str());
   #endif
   
   switch (id)
   {
   case EPOCH_VAR:
      mEpochParamName = value;
      return true;
   case STOP_VAR:
      mStopParamName = value;
      return true;
   case GOAL:
      SetGoalString(value);
      return true;
//   case INTERPOLATOR:
//      mInterpolatorName = value;
//      return true;
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
   #if DEBUG_BASE_STOPCOND_SET
   MessageInterface::ShowMessage
      ("StopCondition::SetStringParameter() label = %s, "
       "value = %s \n", label.c_str(), value.c_str());
   #endif
   
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Real GetStopValue()
//------------------------------------------------------------------------------
/**
 * Used to access the last calculated stopping condition value.
 * 
 * @return The last calculated value obtained from the mStopParam member.
 */ 
//------------------------------------------------------------------------------
Real StopCondition::GetStopValue()
{
   return previousValue;
}


//------------------------------------------------------------------------------
// Real GetStopDifference()
//------------------------------------------------------------------------------
/**
 * Used to access the difference between the last calculated stopping condition 
 * value and the desired value.
 * 
 * @return The difference between the goal and achieved value (goal - achieved).
 */ 
//------------------------------------------------------------------------------
Real StopCondition::GetStopDifference()
{
   Real goalValue, achievedValue;
   if (mGoalParam)
      goalValue = mGoalParam->EvaluateReal();
   else
      goalValue = mGoal;
      
   #ifdef DEBUG_STOPCOND
      MessageInterface::ShowMessage("%s goal (%s) = %16.9lf, value = %16.9lf\n", 
         instanceName.c_str(), 
         (mGoalParam ? mGoalParam->GetName().c_str() : "Fixed goal"), goalValue,
         mStopParam->EvaluateReal());
   #endif
      
   achievedValue = mStopParam->EvaluateReal();
   Real min, max, delta;
   GetRange(min, max);
   delta = (max - min) * 0.5;
   if (IsCyclicParameter())
      achievedValue = PutInRange(achievedValue, goalValue-delta, 
            goalValue + delta);
   
   return goalValue - achievedValue;
}


//------------------------------------------------------------------------------
// Real GetStopGoal()
//------------------------------------------------------------------------------
/**
 * Used to access the desired stopping value.
 * 
 * @return The goal value.
 */ 
//------------------------------------------------------------------------------
Real StopCondition::GetStopGoal()
{
//   return mGoal;
   Real goalValue;
   if (mGoalParam)
      goalValue = mGoalParam->EvaluateReal();
   else
      goalValue = mGoal;
   return goalValue;
}


//------------------------------------------------------------------------------
// Real GetStopTime()
//------------------------------------------------------------------------------
/**
 * Used to access the last calculated stopping condition epoch.
 * 
 * @return The last calculated value obtained from the mEpochParam member.
 */ 
//------------------------------------------------------------------------------
Real StopCondition::GetStopTime()
{
   return previousEpoch;
}


//------------------------------------------------------------------------------
// Real GetTimeMultiplier()
//------------------------------------------------------------------------------
/**
 * Used to find the conversion factor for changing time parameters to seconds.
 * 
 * @return The factor.
 */ 
//------------------------------------------------------------------------------
Real StopCondition::GetTimeMultiplier()
{
   switch (stopParamTimeType)
   {
      case MINUTE_PARAM:
         return 60.0;

      case HOUR_PARAM:
         return 3600.0;
      
      case DAY_PARAM:
      case EPOCH_PARAM:
         return 86400.0;
      
      // All intentional fall-throughs
      case SECOND_PARAM:
      case NOT_TIME_PARAM:
      case UNKNOWN_PARAM_TIME_TYPE:
      default:
         return 1.0;
   };
   
   return 1.0;
}


//------------------------------------------------------------------------------
// bool IsCyclicParameter()
//------------------------------------------------------------------------------
/**
 * Access method used to determine if the stop parameter is cyclic.
 * 
 * @return true for cyclic parameters, false if the parameter is not cyclic.
 */ 
//------------------------------------------------------------------------------
bool StopCondition::IsCyclicParameter()
{
   return isCyclicCondition;
}


//------------------------------------------------------------------------------
// bool GetRange(Real &min, Real &max)
//------------------------------------------------------------------------------
/**
 * Retrieves the minimum and maximum values for the range variable.
 * 
 * @param <min> Reference to the variable that, upon return, contains the range 
 *              minimum
 * @param <max> Reference to the variable that, upon return, contains the range 
 *              maximum
 * 
 * @return true if the references were filled in, false if there has a problem 
 *         setting the range values.
 */ 
//------------------------------------------------------------------------------
bool StopCondition::GetRange(Real &min, Real &max)
{
   bool retval = true;
   if (!isCyclicCondition)
      retval = false;
   else
   {
      switch (scCycleType)
      {
         case ZERO_90:
            min = 0.0;
            max = 90.0;
            break;
            
         case ZERO_180:
            min = 0.0;
            max = 180.0;
            break;
            
         case ZERO_360:
            min = 0.0;
            max = 360.0;
            break;
            
         case PLUS_MINUS_90:
            min = -90.0;
            max = 90.0;
            break;
            
         case PLUS_MINUS_180:
            min = -180.0;
            max = 180.0;
            break;
            
         case OTHER_CYCLIC:   // Intentional drop-through
         case NOT_CYCLIC:     // Intentional drop-through
         default:
            retval = false;
      };
   }
   
   return retval;
}


//------------------------------------------------------------------------------
// Real PutInRange(const Real value, const Real min, const Real max, 
//                 bool isReflection)
//------------------------------------------------------------------------------
/**
 * Returns the parameter value that matches the input value for a cyclic 
 * parameter, remapped to fall between the minimum and maximum values allowed.
 * 
 * @param <value>        The input value of the parameter
 * @param <min>          The minimum value of the parameter
 * @param <max>          The maximum value of the parameter
 * @param <isReflection> Flag indicating if the parameter should be reflected 
 *                       into the valid range.  This flag is not yet used.
 * 
 * @return the parameter value, mapped into the valid range.
 */ 
//------------------------------------------------------------------------------
Real StopCondition::PutInRange(const Real value, const Real min, const Real max,
                                bool isReflection)
{
   if (min >= max)
      throw StopConditionException("PutInRange received bad range limits\n");
   Real range = max - min, retval = value;
   
   while (retval < min)
      retval += range;

   while (retval > max)
      retval -= range;
      
   if (isReflection)
   {
      // Not yet needed or implemented
   }
      
   return retval;
}

//------------------------------------------------------------------------------
// void SkipEvaluation(bool shouldSkip)
//------------------------------------------------------------------------------
/**
 * Function used to toggle stopping conditions is active or inactive.
 * 
 * @param <shouldSkip> Flag indicating the status.  If this parameter is true,
 *                     the stopping condition is deactivated; if false, it is
 *                     activated. 
 */ 
//------------------------------------------------------------------------------
void StopCondition::SkipEvaluation(bool shouldSkip)
{
   if (mStopParam->GetName().find("ModJulian") == std::string::npos)
      activated = !shouldSkip;
}


//------------------------------------------------------------------------------
// void UpdateBuffer()
//------------------------------------------------------------------------------
/**
 * Sets the most recent value of the stop parameter and epoch to be the 
 * previously calculated values. 
 */ 
//------------------------------------------------------------------------------
void StopCondition::UpdateBuffer()
{
   previousValue = mStopParam->EvaluateReal();

   // evaluate goal in case needed for cyclics
   if (mAllowGoalParam)
      mGoal = mGoalParam->EvaluateReal();
   
   // set current epoch
   Real epoch;
   if (mUseInternalEpoch)
      epoch = mEpoch;
   else
      epoch = mEpochParam->EvaluateReal();
      
      // Force anomalies to handle wrapping
   if (isAngleParameter)
   {
      mGoal = AngleUtil::PutAngleInDegRange(mGoal, 0.0, GmatMathUtil::TWO_PI_DEG);
      CheckOnAnomaly(previousValue);
   }
   else if(isCyclicCondition)
   {
      mGoal = PutInRange(mGoal, 0.0, GmatMathUtil::TWO_PI_DEG);
      CheckCyclicCondition(previousValue);
   }
   
   previousEpoch = epoch;

   #ifdef DEBUG_CYCLIC_PARAMETERS
      MessageInterface::ShowMessage(
         "Updated \"previous\" values to [previousValue  previousEpoch]"
         " = [%.12lf  %.12lf]\n", previousValue, previousEpoch);
   #endif
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

   if ((Integer)mEpochBuffer.size() < mBufferSize)
      mEpochBuffer.reserve(mBufferSize);

   if ((Integer)mValueBuffer.size() < mBufferSize)
      mValueBuffer.reserve(mBufferSize);
      
   for (int i=0; i<mBufferSize; i++)
   {
      mEpochBuffer[i] = stopCond.mEpochBuffer[i];
      mValueBuffer[i] = stopCond.mValueBuffer[i];
   }
}
