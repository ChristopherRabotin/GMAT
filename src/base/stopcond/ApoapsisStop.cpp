//$Header$
//------------------------------------------------------------------------------
//                              ApoapsisStop
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/15
//
/**
 * Implements Apoapsis stopping condition class.
 */
//------------------------------------------------------------------------------
#include "ApoapsisStop.hpp"
#include "StopConditionException.hpp"
#include "Rvector6.hpp"
#include "Rvector3.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// ApoapsisStop(const std::string &name, const std::string &desc,
//              Parameter *epochParam, Parameter *stopParam,
//              const Real &goal, const Real &tol,
//              const Integer repeatCount,
//              RefFrame *refFrame,
//              Interpolator *interp)
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
ApoapsisStop::ApoapsisStop(const std::string &name, const std::string &desc,
                           Parameter *epochParam, Parameter *stopParam,
                           const Real &goal, const Real &tol,
                           const Integer repeatCount, RefFrame *refFrame,
                           Interpolator *interp)
    : StopCondition(name, "ApoapsisStop", desc, epochParam, stopParam, goal, tol,
                    repeatCount, refFrame, interp)
{
    mCartStateParam = NULL;
    mKepEccParam = NULL;
    
    SetParameter(stopParam);
}

//------------------------------------------------------------------------------
// ApoapsisStop(const ApoapsisStop &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 */
//------------------------------------------------------------------------------
ApoapsisStop::ApoapsisStop(const ApoapsisStop &copy)
    : StopCondition(copy)
{
}

//------------------------------------------------------------------------------
// ApoapsisStop& operator= (const ApoapsisStop &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 */
//------------------------------------------------------------------------------
ApoapsisStop& ApoapsisStop::operator= (const ApoapsisStop &right)
{
    if (this != &right)
        StopCondition::operator=(right);

    return *this;
}

//------------------------------------------------------------------------------
// virtual ~ApoapsisStop()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ApoapsisStop::~ApoapsisStop()
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
bool ApoapsisStop::AddParameter(Parameter *param)
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
bool ApoapsisStop::Evaluate()
{
    bool goalMet = false;
    Real epoch;
    Rvector6 r6val;
    Real stopEpoch;
    
    if (!Validate())
    {
        throw StopConditionException
            ("ApoapsisStop::Evaluate(): Validate() failed.");
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

    //loj: ---------- begin temp. code
    //loj: where do I get the following values?
    bool backwards = false; //loj: from Propagator?
    Real ecc = mKepEccParam->EvaluateReal();
    Real eTau = 1.0e-6; //loj: as StopCondition parameter?
    bool eTauFlag = (ecc >= eTau);
    //loj: ---------- end temp. code
    
    if (mNumValidPoints >= 2 && eTauFlag &&
        ((backwards && mValueBuffer[mBufferSize-2] <= goal  &&
          goal <= mValueBuffer[mBufferSize-1]) ||  
         (!backwards && mValueBuffer[mBufferSize-2] >= goal  &&
          goal >= mValueBuffer[mBufferSize-1])))
    {
        goalMet = true;

        // now interpolate epoch
        mInterpolator->Clear();
        for (int i=0; i<mBufferSize; i++)
            mInterpolator->AddPoint(mValueBuffer[i], &mEpochBuffer[i]);
        if (mInterpolator->Interpolate(goal, &stopEpoch))
            mStopEpoch = stopEpoch;
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
bool ApoapsisStop::Validate()
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
bool ApoapsisStop::SetParameter(Parameter *param)
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
