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
//  #include "CurrentTimeParam.hpp"
//  #include "ElapsedTimeParam.hpp"
//  #include "SpacecraftStateParam.hpp"
#include "StopConditionException.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// SingleValueStop(const std::string &name,
//                 const Real &goal, const Real &tol,
//                 Parameter *param,
//                 const std::string &paramDataName,
//                 const Integer repeatCount,
//                 RefFrame *refFrame,
//                 Interpolator *interp)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of stop condition
 * @param <goal> goal for stop condition test
 * @param <tol>  tolerance for stop condition test
 * @param <*param> Parameter object pointer to retrive parameter value
 * @param <paramDataName> name of parameter data
 * @param <repeatCount> repeat count for stop condition test
 * @param <*refFrame> RefFrame object pointer to be used for stop condition test
 * @param <*interp> Interpolator object pointer for interpolating stop epoch
 */
//------------------------------------------------------------------------------
SingleValueStop::SingleValueStop(const std::string &name,
                                 const Real &goal, const Real &tol,
                                 Parameter *parameter,
                                 const std::string &paramDataName,
                                 const Integer repeatCount,
                                 RefFrame *refFrame,
                                 Interpolator *interp)
   : StopCondition(name, "SingleValueStop", goal, tol, parameter,
                   repeatCount, refFrame, interp)
{
   mParamDataName = paramDataName;
   if (mParamDataName == "")
      mInitialized = false;

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
   mParamDataName = svsc.mParamDataName;   
   if (mParamDataName == "")
      mInitialized = false;
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
   {
      StopCondition::operator=(right);
      mParamDataName = right.mParamDataName;   
      if (mParamDataName == "")
         mInitialized = false;
   }

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
// void SetParameterDataName(const std::string &paramDataName)
//------------------------------------------------------------------------------
void SingleValueStop::SetParameterDataName(const std::string &paramDataName)
{
   //loj: Are we going to allow to set to different parameter data name?
   if (mParamDataName == "")
      mParamDataName = paramDataName;
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
 * @note Account for repeat stop count will be implemented in the next build.
 */
//------------------------------------------------------------------------------
bool SingleValueStop::Evaluate()
{
   bool goalMet = false;

   //loj: Need to rework on this
   
//     if ((mParameters[0] == NULL) || (mParamDataName == ""))
//        throw StopConditionException
//           ("Cannot evaluate the stop condition: Parameter or data name is not "
//            "set: Parameter = " + (int)mParameters[0] + " Name = " + mParamDataName);

//     if (mParameters[0] == NULL)
//        throw StopConditionException
//           ("Cannot evaluate the stop condition: Parameter is not set.");
      
//     if (mParamDataName == "")
//        throw StopConditionException
//           ("Cannot evaluate the stop condition: parameter data name is not "
//            "set:  Name = " + mParamDataName);
      
//     //-----------------------------------------------------------------
//     // time related stop condition doesn't need to interpolate stop epoch
//     // *** account for repeat count and tolerance will be implemented
//     //     in the next build.
//     //-----------------------------------------------------------------
//     if (mParamDataName == "CurrentTime")
//     {
//        CurrentTimeParam *param = dynamic_cast<CurrentTimeParam*>(mParameters[0]);
      
//        if (param == NULL)
//        {
//           throw StopConditionException
//              ("Parameter is not the type of CurrentTimeParam for retrieving " +
//               mParamDataName);
//        }
      
//        Integer id = param->GetParameterID(mParamDataName);
//        Real val = param->GetRealParameter(id);

//        if (val >= mGoal)
//           goalMet = true;
//     }
//     else if (mParamDataName == "ElapsedTimeInSeconds")
//     {
//        ElapsedTimeParam *param = dynamic_cast<ElapsedTimeParam*>(mParameters[0]);
      
//        if (param == NULL)
//        {
//           throw StopConditionException
//              ("Parameter is not the type of ElapsedTimeParam "
//               "for retrieving " + mParamDataName);
//        }
      
//        Integer id = param->GetParameterID(mParamDataName);
//        Real val = param->GetRealParameter(id);
      
//        if (val >= mGoal)
//           goalMet = true;
//     }
//     //-----------------------------------------------------------------
//     // else future build implementation -
//     //    *** need to interpolate stop epoch
//     //    *** need to convert to RefFrame for certain stop condition type
//     //      
//     //-----------------------------------------------------------------
//     else
//     {
//        if (!mInitialized)
//        {
//           throw StopConditionException
//              ("SingleValueStop::Evaluate(): The stopping condition ring buffer is "
//               "not initialized.");
//        }
      
//        mNumValidPoints = (mNumValidPoints < mBufferSize) ?
//           mNumValidPoints + 1 : mNumValidPoints;
      
//        // shift values to make room for newest value
//        for (int i=0; i<mBufferSize-1; i++)
//           mValueBuffer[i] = mValueBuffer[i+1];

//        // check for other single value here
//        if (mParamDataName == "X" ||
//            mParamDataName == "Y" ||
//            mParamDataName == "Z" ||
//            mParamDataName == "Vx" ||
//            mParamDataName == "Vy" ||
//            mParamDataName == "Vz" ||
//            mParamDataName == "MagOfPosition" ||
//            mParamDataName == "MagOfVelocity")
//        {
//           SpacecraftStateParam *param = dynamic_cast<SpacecraftStateParam*>(mParameters[0]);
         
//           if (param == NULL)
//           {
//              throw StopConditionException
//                 ("Parameter is not the type of SpacecraftStateParam "
//                  "for retrieving " + mParamDataName);
//           }

//           Integer id = param->GetParameterID(mParamDataName);

//           //loj:for the future build:
//           //GmatBase should use constant number instead of -1
//           //if name is not associated with id. (for the future build)
//           if (id == -1)
//           {
//              throw StopConditionException
//                 ("Parameter data name: " +  mParamDataName +
//                  " has no associated ID in the SpacecraftStateParam ");
//           }
         
//           Real val = param->GetRealParameter(id);

//           mValueBuffer[mBufferSize - 1] = val;
         
//           // Stop if at least 2 points set, and either
//           //   1) last value was more than goal, but current value is less,
//           //   2) last value was less than goal, but current value is more.
//           if (mNumValidPoints >= 2 &&
//               ((mValueBuffer[mBufferSize-2] <= mGoal  &&
//                 mGoal <= mValueBuffer[mBufferSize-1]) ||  
//                (mValueBuffer[mBufferSize-2] >= mGoal  &&
//                 mGoal >= mValueBuffer[mBufferSize-1]) ))
//           {
//              goalMet = true;
//           }
//        }
//     }
  
   return goalMet;
}

