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
//               const Real &goal, const Real &tol, Parameter *parameter,
//               const Integer repeatCount, RefFrame *refFrame, Interpolator *interp)
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
StopCondition::StopCondition(const std::string &name, const std::string typeStr,
                             const Real &goal, const Real &tol,
                             Parameter *parameter,
                             const Integer repeatCount, RefFrame *refFrame,
                             Interpolator *interp)
   : GmatBase(Gmat::STOP_CONDITION, typeStr, name)
{
   mGoal = goal;
   mTolerance = tol;
   mRepeatCount = repeatCount;
   mRefFrame = refFrame;
   mInterpolator = interp;
   
   mParameters.reserve(1);
   mParameters[0] = parameter;
   
   mNumParams = 0;
   mNumValidPoints = 0;
   mBufferSize = 0;
   
   // need at least one Parameter object
   if (mParameters[0] == NULL)
      mInitialized = false;
   else
      mNumParams = 1;
   
   Initialize();
}

//------------------------------------------------------------------------------
// StopCondition(const StopCondition &sc)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 */
//------------------------------------------------------------------------------
StopCondition::StopCondition(const StopCondition &sc)
   : GmatBase(sc)
{
   mGoal = sc.mGoal;
   mTolerance = sc.mTolerance;
   mRepeatCount = sc.mRepeatCount;
   mRefFrame = sc.mRefFrame;
   mInterpolator = sc.mInterpolator;
   
   mParameters.reserve(1);

   Initialize();
   CopyDynamicData(sc);
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
      
      mParameters.reserve(1);

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
// Parameter* GetFirstParameter()
//------------------------------------------------------------------------------
Parameter* StopCondition::GetFirstParameter()
{
   return mParameters[0];
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
// void SetFirstParameter(Parameter *param)
//------------------------------------------------------------------------------
void StopCondition::SetFirstParameter(Parameter *param)
{
   //loj: Are we going to allow to set to different parameter pointer?
   if (mParameters[0] == NULL)
      mParameters[0] = param;
}

//------------------------------------------------------------------------------
// void SetInterpolator(Interpolator *interp)
//------------------------------------------------------------------------------
/**
 * Sets Interpolator to interpolate stop condition epoch.
 */
//------------------------------------------------------------------------------
void StopCondition::SetInterpolator(Interpolator *interp)
{
   if (!mInitialized)
   {
      mInterpolator = interp;
      Initialize();
   }
   else
   {
      throw StopConditionException("StopCondition:: The interpolator is already set.");
   }
}

//------------------------------------------------------------------------------
// void AddParameter(Parameter *param)
//------------------------------------------------------------------------------
/**
 * Add parameter to stop condition if more than one Parameter object is needed
 * for evaluating stop condtion.
 *
 * @note This method will be implemented in the future builds. This may not
 * need.
 */
//------------------------------------------------------------------------------
void StopCondition::AddParameter(Parameter *param)
{
   // do nothing for now
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
   
   if (mParameters[0] == NULL)
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

//------------------------------------------------------------------------------
// void CopyDynamicData()
//------------------------------------------------------------------------------
void StopCondition::CopyDynamicData(const StopCondition &sc)
{
   mNumValidPoints = sc.mNumValidPoints;
   mNumParams = sc.mNumParams;
   
   for (int i=0; i<mNumParams; i++)
      mParameters[i] = sc.mParameters[i];
   
   mBufferSize = sc.mBufferSize;
   for (int i=0; i<mBufferSize; i++)
   {
      mEpochBuffer[i] = sc.mEpochBuffer[i];
      mValueBuffer[i] = sc.mValueBuffer[i];
   }
}

