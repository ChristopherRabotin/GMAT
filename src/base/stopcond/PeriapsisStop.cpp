//$Header$
//------------------------------------------------------------------------------
//                              PeriapsisStop
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/16
//
/**
 * Implements Periapsis stopping condition class.
 */
//------------------------------------------------------------------------------
#include "PeriapsisStop.hpp"
#include "StopConditionException.hpp"
#include "Rvector6.hpp"
#include "Rvector3.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// PeriapsisStop(const std::string &name,
//               Parameter *epochParam, Parameter *stopParam,
//               const Real &goal, const Real &tol,
//               const Integer repeatCount,
//               RefFrame *refFrame,
//               Interpolator *interp)
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
PeriapsisStop::PeriapsisStop(const std::string &name,
                             Parameter *epochParam, Parameter *stopParam,
                             const Real &goal, const Real &tol,
                             const Integer repeatCount,
                             RefFrame *refFrame,
                             Interpolator *interp)
    : StopCondition(name, "PeriapsisStop", epochParam, stopParam, goal, tol,
                    repeatCount, refFrame, interp)
{
    mCartStateParam = NULL;
    mKepEccParam = NULL;
    
    SetParameter(stopParam);
}

//------------------------------------------------------------------------------
// PeriapsisStop(const PeriapsisStop &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 */
//------------------------------------------------------------------------------
PeriapsisStop::PeriapsisStop(const PeriapsisStop &copy)
    : StopCondition(copy)
{
}

//------------------------------------------------------------------------------
// PeriapsisStop& operator= (const PeriapsisStop &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 */
//------------------------------------------------------------------------------
PeriapsisStop& PeriapsisStop::operator= (const PeriapsisStop &right)
{
    if (this != &right)
        StopCondition::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// virtual ~PeriapsisStop()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
PeriapsisStop::~PeriapsisStop()
{
}

//-------------------------------------
// Inherited methods from StopCondition
//-------------------------------------

//------------------------------------------------------------------------------
// virtual bool AddParameter(Parameter *param)
//------------------------------------------------------------------------------
/**
 * @return true if apoapsis stopping condition has been met;
 *   false otherwise
 */
//------------------------------------------------------------------------------
bool PeriapsisStop::AddParameter(Parameter *param)
{    
    if (StopCondition::AddParameter(param))
    {
        return SetParameter(param);
    }

    return false;
}

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * @return true if apoapsis stopping condition has been met;
 *   false otherwise
 *
 * @exception <StopConditionException> thrown if unable to retrive parameter
 *   value
 *
 * @note count for repeat stop count will be implemented in the next build.
 *       Implementation code is from Swingby
 */
//------------------------------------------------------------------------------
bool PeriapsisStop::Evaluate()
{
    bool goalMet = false;
    Real epoch;
    Rvector6 r6val;
    Real stopEpoch;
    
    if (!Validate())
    {
        throw StopConditionException
            ("PeriapsisStop::Evaluate(): Validate() failed.");
    }
      
//      //loj: Do I really need to validate parameter before evaluate?
//      //     Is validating parameter in AddParameter enough?
//      if (mParameters[0]->Validate() == false)
//      {
//          throw StopConditionException
//              ("Cannot evaluate the stop condition: " + 
//               mParameters[0]->GetTypeName() + ":" + mParameters[0]->GetName() +
//               " Validate() failed");
//      }

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
    r6val = mCartStateParam->EvaluateRvector6();

    // compute position and velocity unit vectors
    Rvector3 pos = Rvector3(r6val[0], r6val[1], r6val[2]);
    Rvector3 vel = Rvector3(r6val[3], r6val[4], r6val[5]);
    Rvector3 R = pos.GetUnitVector();
    Rvector3 V = vel.GetUnitVector();
    
    // compute cos(90 - beta) as the dot product of the R and V vectors
    Real rdotv = R*V;
    mEpochBuffer[mBufferSize - 1] = epoch;
    mValueBuffer[mBufferSize - 1] = rdotv;
         
    // stop if at least 2 points set and eccentricity flag set, and either
    //   1) propagating backwards, and dot product goes - to +,
    //   2) propagating forwards,  and dot product goes + to - 
    Real goal = 0.0;

    //loj: ================================================= begin temp. code
    //loj: where do I get the following values?
    bool backwards = false; //loj: from Propagator?
    Real ecc = mKepEccParam->EvaluateReal();
    Real eTau = 1.0e-6; //loj: add to StopCondition parameter?
    bool eTauFlag = (ecc >= eTau);
    Real range = pos.GetMagnitude() + 100.0; //loj: add to StopCondition parameter?
    //loj: ================================================= end temp. code

    // Swngby code:
    // if elapsed time reached, and we are in range and eccentricity flag set ...

    if (pos.GetMagnitude() <= range && eTauFlag)
    {
        if (mNumValidPoints >= 2 &&
            ((backwards && mValueBuffer[mBufferSize-2] >= goal  &&
              goal > mValueBuffer[mBufferSize-1]) ||  
             (!backwards && mValueBuffer[mBufferSize-2] <= goal  &&
              goal < mValueBuffer[mBufferSize-1])))
        {
            goalMet = true;

            // now interpolate epoch
            mInterpolator->Clear();
            for (int i=0; i<mBufferSize; i++)
                mInterpolator->AddPoint(mValueBuffer[i], &mEpochBuffer[i]);
            if (mInterpolator->Interpolate(goal, &stopEpoch))
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
bool PeriapsisStop::Validate()
{
    bool valid = false;
    
    if (mNumParams >= 1)
    {
        if (mCartStateParam != NULL && mKepEccParam != NULL &&
            mEpochParam != NULL && mInterpolator != NULL)
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
bool PeriapsisStop::SetParameter(Parameter *param)
{
    bool status = false;
    
    if (param != NULL)
    {
        std::string paramType = param->GetTypeName();
        
        if (paramType == "CartStateParam")
        {
            if (mCartStateParam == NULL)
            {
                mCartStateParam = param;
                status = true;
            }
        }
        else if (paramType == "KepEccParam")
        {
            if (mKepEccParam == NULL)
            {
                mKepEccParam = param;
                status = true;
            }
        }
    }

    return status;
}
