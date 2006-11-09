//$Header$
//------------------------------------------------------------------------------
//                              Integrator
//------------------------------------------------------------------------------
// *** File Name : Integrator.cpp
// *** Created   : October 1, 2002
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
// ***                                                                    ***
// ***  Copyright U.S. Government 2002                                    ***
// ***  Copyright United States Government as represented by the          ***
// ***  Administrator of the National Aeronautics and Space               ***
// ***  Administration                                                    ***
// ***                                                                    ***
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 11/26/2002 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//
//                           : 1/7/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//
//                           : 2/19/2003 - D. Conway, Thinking Systems, Inc.
//                             Removed parms for fixed step mode
//
//                           : 3/3/2003 - D. Conway, Thinking Systems, Inc.
//                             Added error control threshold; moved time 
//                             granularity into this class
//
//                           : 09/26/2003 - W. Waktola, Mission Applications Branch
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//
//                           : 10/10/2003 - W. Waktola, 
//                                          Mission Applications Branch
//                              Changes:
//                                - SetParameter() to SetRealParameter()
//                                - GetParameter() to GetRealParameter()
//                                - virtual char* 
//                                  GetParameterName(const int parm) const to
//                                  virtual std::string 
//                                  GetParameterName(const int parm) const
//
//                           : 10/16/2003 - W. Waktola, 
//                                          Missions Applications Branch
//                              Changes:
//                                - All double types to Real types
//                                - All primitive int types to Integer types
//                              Removals:
//                                - GetParameterName(), replaced by 
//                                  GetParameterText()
//                              Additions:
//                                - IntegratorParamCount
//                                - PARAMTER_TEXT[]
//                                - PARAMETER_TYPE[]
//                                - GetIntegerParameter()
//                                - SetIntegerParameter()
//                                - GetParameterText()
//                                - GetParameterID()
//                                - GetParameterType()
//                                - GetParameterTypeString()
//
// **************************************************************************

#include "Integrator.hpp"
#include "MessageInterface.hpp"
#include "PropagatorException.hpp"
#include <sstream>
#include <string.h>

//---------------------------------
// static data
//---------------------------------
const std::string
Integrator::PARAMETER_TEXT[IntegratorParamCount - PropagatorParamCount] =
{
    "Accuracy",
    "ErrorThreshold",
    "SmallestInterval",
    "MinStep",
    "MaxStep",
    "MaxStepAttempts",
};

const Gmat::ParameterType
Integrator::PARAMETER_TYPE[IntegratorParamCount - PropagatorParamCount] =
{
    Gmat::REAL_TYPE,
    Gmat::REAL_TYPE,
    Gmat::REAL_TYPE,
    Gmat::REAL_TYPE,
    Gmat::REAL_TYPE,
    Gmat::INTEGER_TYPE
};

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// Integrator::Integrator(void)
//------------------------------------------------------------------------------
/**
 * Default constructor for the Integrator
 *
 * @param <typeStr> Integrator type
 * @param <nomme>  Integrator name
 */
//------------------------------------------------------------------------------
Integrator::Integrator(const std::string &typeStr, const std::string &nomme)
    : Propagator              (typeStr, nomme),
      tolerance               (1.0e-11),
      fixedStep               (false),
      fixedStepsize           (300.0),
      minimumStep             (0.001),                // 1 msec
      maximumStep             (2700.0),               // 45 minutes
      smallestTime            (1.0e-6),
      stepAttempts            (0),
      maxStepAttempts         (50),
      stepTaken               (0.0),
      timeleft                (stepSize),    
      ddt                     (NULL),
      errorEstimates          (NULL),
      errorThreshold          (0.10),
      derivativeOrder         (1)
{
    parameterCount = IntegratorParamCount;
}

//------------------------------------------------------------------------------
// Integrator::Integrator(const Integrator& i)
//------------------------------------------------------------------------------
/**
 * Copy constructor that replicates the Integrator.
 *
 * @param <i> Integrator used for copying
 */
//------------------------------------------------------------------------------
Integrator::Integrator(const Integrator& i) :
    Propagator              (i),
    tolerance               (i.tolerance),
    fixedStep               (i.fixedStep),
    fixedStepsize           (i.fixedStepsize),
    minimumStep             (i.minimumStep),
    maximumStep             (i.maximumStep),
    smallestTime            (i.smallestTime),
    stepAttempts            (0),
    maxStepAttempts         (i.maxStepAttempts),
    stepTaken               (0.0),
    timeleft                (i.timeleft),    
    ddt                     (NULL),
    errorEstimates          (NULL),
    errorThreshold          (i.errorThreshold),
    derivativeOrder         (i.derivativeOrder)
{
    parameterCount = IntegratorParamCount;
}

//------------------------------------------------------------------------------
// Integrator& Integrator::operator=(const Integrator& i)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Integrator
 *
 * @param <i> Integrator used for assignment
 */
//------------------------------------------------------------------------------
Integrator& Integrator::operator=(const Integrator& i)
{
    if (this == &i)
        return *this;

    Propagator::operator=(i);
    tolerance = i.tolerance;
    fixedStep = i.fixedStep;
    fixedStepsize = i.fixedStepsize;
    stepTaken = 0.0;
    timeleft = i.timeleft;

    minimumStep = i.minimumStep;
    maximumStep = i.maximumStep;
    stepAttempts = 0;
    maxStepAttempts = i.maxStepAttempts;
    
    derivativeOrder = i.derivativeOrder;
    
    smallestTime = i.smallestTime;  
    ddt = NULL;
    errorEstimates = NULL;
    errorThreshold = i.errorThreshold;

    return *this;
}

//------------------------------------------------------------------------------
// Integrator::~Integrator(void)
//------------------------------------------------------------------------------
/**
 * Destructor for the Integrator
 */
//------------------------------------------------------------------------------
Integrator::~Integrator(void)
{
    if (errorEstimates)
        delete [] errorEstimates;
}

//------------------------------------------------------------------------------
// std::string Integrator::GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
std::string Integrator::GetParameterText(const Integer id) const
{
    if (id >= PropagatorParamCount && id < IntegratorParamCount)
        return PARAMETER_TEXT[id - PropagatorParamCount];
    else
        return Propagator::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer Integrator::GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
Integer Integrator::GetParameterID(const std::string &str) const
{
    for (int i = PropagatorParamCount; i < IntegratorParamCount; i++)
    {
        if (str == PARAMETER_TEXT[i - PropagatorParamCount])
            return i;
    }
    return Propagator::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType Integrator::GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Integrator::GetParameterType(const Integer id) const
{
    if (id >= PropagatorParamCount && id < IntegratorParamCount)
        return PARAMETER_TYPE[id - PropagatorParamCount];
    else
        return Propagator::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string Integrator::GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string Integrator::GetParameterTypeString(const Integer id) const
{
    if (id >= PropagatorParamCount && id < IntegratorParamCount)
        return GmatBase::PARAM_TYPE_STRING
		                      [GetParameterType(id - PropagatorParamCount)];
    else
        return Propagator::GetParameterTypeString(id);
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool Integrator::IsParameterReadOnly(const Integer id) const
{
   if ((id == ERROR_THRESHOLD) || (id == SMALLEST_INTERVAL))
      return true;

   return Propagator::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// Real Integrator::GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 *  For integrators, the user can use this function to get the integration
 *  accuracy (id == ACCURACY), toggle the fixed step propagation mode
 *  (RUN_FIXEDSTEP), and set the stepSize for fixed step mode (FIXED_STEPSIZE).
 *  If the RUN_FIXEDSTEP id is passed into the method, 1.0 is returned if running
 *  in fixed step mode, and 0.0 if in variable step mode.
 *
 *  @param id   integer id of the parameter being retrieved
 */
//------------------------------------------------------------------------------
Real Integrator::GetRealParameter(const Integer id) const
{
   switch (id)
   {
      case ACCURACY:
	     return tolerance;
	  case ERROR_THRESHOLD:
	     if (physicalModel)
            return physicalModel->GetErrorThreshold();
		 return errorThreshold;
	  case SMALLEST_INTERVAL:
	     return smallestTime;
	  case MIN_STEP:
	     return minimumStep;
	  case MAX_STEP:
	     return maximumStep;
	  default:
	     return Propagator::GetRealParameter(id);   
   }    
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real Integrator::GetRealParameter(const std::string &label) const
{
    return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// Real Integrator::SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * For integrators, the user can use this function to set the integration
 * accuracy (id == ACCURACY), toggle the fixed step propagation mode
 * (RUN_FIXEDSTEP), and set the stepSize for fixed step mode (FIXED_STEPSIZE).
 *
 * @param id    integer id of the parameter being set
 * @param value The new value for the parameter
 */
//------------------------------------------------------------------------------
Real Integrator::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case ACCURACY:
//      if (value <= 0.0)
//         throw PropagatorException(
//         "Integrator::SetRealParameter -- Accuracy value is set to <= 0.0");
//      tolerance = value;
//      return value;
      if (value >= 0.0)
         tolerance = value;
      else
      {
         std::stringstream buffer;
         buffer << value;
//         throw PropagatorException(
//            "The value of " + buffer.str() + " for the integrator accuracy is not"
//            " an allowed value. The allowed values are [Real Number >= 0.0].");
            throw PropagatorException(
               "The value of \"" + buffer.str() + "\" for field \"Accuracy\""
               " on object \"" + instanceName + "\" is not an allowed value.\n"
               "The allowed values are: [ Real Number >= 0.0 ]. ");
      }
      return value;
   case MIN_STEP:
//      if (value == 0.0)
//         throw PropagatorException(
//                   "Integrator::SetRealParameter --Minimum step is set to 0.0");
//      minimumStep = fabs(value);
//      return value;
      if (value >= 0.0)
         minimumStep = value;
      else
      {
         std::stringstream buffer;
         buffer << value;
//         throw PropagatorException(
//            "The value of " + buffer.str() + " for the integrator minimum step size "
//            "is not an allowed value. The allowed values are "
//            "[Real Number >= 0, MinStep <= MaxStep].");
            throw PropagatorException(
               "The value of \"" + buffer.str() + "\" for field \"Min Step\""
               " on object \"" + instanceName + "\" is not an allowed value.\n"
               "The allowed values are: [ Real Number >= 0.0, MinStep <= MaxStep ].");
      }
      return value;
   case MAX_STEP:
      // @todo waw: temporarily commented out, to be uncommented 
      // once Edwin updates his scripts to support this
      // if (fabs(value) <= minimumStep)
//      if (fabs(value) < minimumStep)
//         throw PropagatorException(
//      "Integrator::SetRealParameter -- Maximum step is set less than minimum step.");
//      maximumStep = fabs(value);
//      return value;
      if (value >= 0.0)
//       if (fabs(value) <= minimumStep)
         if (fabs(value) < minimumStep)
            throw PropagatorException(
               "Integrator::SetRealParameter -- Maximum step is set less than minimum step.");
         else
            maximumStep = value;
      else
      {
         std::stringstream buffer;
         buffer << value;
//         throw PropagatorException(
//            "The value of " + buffer.str() + " for the integrator minimum step size "
//            "is not an allowed value. The allowed values are "
//            "[Real Number >= 0, MinStep <= MaxStep].");
            throw PropagatorException(
               "The value of \"" + buffer.str() + "\" for field \"Max Step\""
               " on object \"" + instanceName + "\" is not an allowed value.\n"
               "The allowed values are: [ Real Number >= 0.0, MinStep <= MaxStep ].");
      }
      return value;
   case ERROR_THRESHOLD:
      errorThreshold = fabs(value);
      if (physicalModel)
         physicalModel->SetErrorThreshold(errorThreshold);
	  else
	     throw PropagatorException(
                      "Integrator::SetRealParameter -- PhysicalModel is NULL.");
      return value;
   case SMALLEST_INTERVAL:
      smallestTime = fabs(value);
      return value;
   default:
      return Propagator::SetRealParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real Integrator::SetRealParameter(const std::string &label, const Real value)
{
    return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// Integer Integrator::GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer Integrator::GetIntegerParameter(const Integer id) const
{
    if (id == MAX_STEP_ATTEMPTS)
        return maxStepAttempts;
    else
        return Propagator::GetIntegerParameter(id);
}

//------------------------------------------------------------------------------
// Integer Integrator::GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer Integrator::GetIntegerParameter(const std::string &label) const
{
    return GetIntegerParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// Integer Integrator::SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer Integrator::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == MAX_STEP_ATTEMPTS)
   {
//        if (value < 1)
//            return GmatBase::INTEGER_PARAMETER_UNDEFINED;
//        maxStepAttempts = value;
//        return value;
      if (value > 0)
         maxStepAttempts = value;
      else
      {
         std::stringstream buffer;
         buffer << value;
//         throw PropagatorException(
//            "The value of " + buffer.str() + " for the integrator Max Step Attempts"
//            " a is not an allowed value. The allowed values are [Integer > 0].");
            throw PropagatorException(
               "The value of \"" + buffer.str() + "\" for field \"Max Step Attempts\""
               " on object \"" + instanceName + "\" is not an allowed value.\n"
               "The allowed values are: [ Integer > 0 ]. ");
      }
      return value;
   }
    
   return Propagator::SetIntegerParameter(id, value);
}

//------------------------------------------------------------------------------
// Integer Integrator::SetIntegerParameter(const std::string &label, 
//                                                          const Integer value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer Integrator::SetIntegerParameter(const std::string &label, const Integer value)
{
    return SetIntegerParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// void SetPhysicalModel(PhysicalModel *pPhyscialModel)
//------------------------------------------------------------------------------
/**
 * Connects the propagation system to the physical model 
 *
 * This method calls Propagator::SetPhysicalModel(), and then sets the error 
 * threshold for the physical model to the local value.
 *
 * @param pPhyscialModel Pointer to the physical model
 */
//------------------------------------------------------------------------------
void Integrator::SetPhysicalModel(PhysicalModel *pPhyscialModel)
{
    Propagator::SetPhysicalModel(pPhyscialModel);
    if (physicalModel != NULL) 
        physicalModel->SetErrorThreshold(errorThreshold);
	else
	   throw PropagatorException(
                      "Integrator::SetPhysicalModel -- PhyscialModel is NULL.");
}


//------------------------------------------------------------------------------
// Real GetStepTaken()
//------------------------------------------------------------------------------
/**
 * Returns the most recent step taken by the integrator.
 * 
 * This method is used to coordinate the steps taken by multiple integrators 
 * running concurrently.  An example of this usage can be found in 
 * the Propagate command -- see Propagate::TakeAStep.
 *
 * @return The step taken, in seconds.
 */
//------------------------------------------------------------------------------
Real Integrator::GetStepTaken()
{
   return stepTaken;
}

//------------------------------------------------------------------------------
// Integer Integrator::GetPropagatorOrder() const
//------------------------------------------------------------------------------
/**
 * Returns derivative order needed by the integrator; 1 by default
 */
//------------------------------------------------------------------------------
Integer Integrator::GetPropagatorOrder() const
{
    return 1;
}
