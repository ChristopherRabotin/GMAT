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
//				Changes:
//				  - Updated style using GMAT cpp style guide
//
//                           : 10/10/2003 - W. Waktola, Mission Applications Branch
//				Changes:
//				  - SetParameter() to SetRealParameter()
//				  - GetParameter() to GetRealParameter()
//				  - virtual char* GetParameterName(const int parm) const to
//				    virtual std::string GetParameterName(const int parm) const
//				  - GetParameterName() from if statements to switch statements
//
//                           : 10/16/2003 - W. Waktola, Missions Applications Branch
//				Changes:
//				  - All double types to Real types
//				  - All primitive int types to Integer types
//				Removals:
//				  - GetParameterName(), replaced by GetParameterText()
//				Additions:
//				  - IntegratorParamCount
//				  - PARAMTER_TEXT[]
//				  - PARAMETER_TYPE[]
//				  - GetIntegerParameter()
//				  - SetIntegerParameter()
//				  - GetParameterText()
//				  - GetParameterID()
//				  - GetParameterType()
//				  - GetParameterTypeString()
//
// **************************************************************************

#include "Integrator.hpp"
#include "gmatdefs.hpp"
#include "Propagator.hpp"

//---------------------------------
// static data
//---------------------------------
const std::string
Integrator::PARAMETER_TEXT[IntegratorParamCount] =
{
	"Maximum Estimated Integration Error",
	"Relative Error Control Threshold",
	"Stepsize Granularity (sec)",
	"Minimum Step Size (sec)",
	"Maximum Step Size (sec)",
	"Maximum number of failed steps allowed"
};

const Gmat::ParameterType
Integrator::PARAMETER_TYPE[IntegratorParamCount] =
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
 */
//------------------------------------------------------------------------------
Integrator::Integrator(Gmat::ObjectType id, const std::string &typeStr,
					   const std::string &nomme) :
    Propagator              (id, typeStr, nomme),
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
    errorThreshold          (0.10)
{
    parameterCount = numStepAttempts;
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
// Integrator::Integrator(const Integrator& i)
//------------------------------------------------------------------------------
/**
 * Copy constructor
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
    errorThreshold          (i.errorThreshold)
{
    parameterCount = numStepAttempts;
}

//------------------------------------------------------------------------------
// Integrator& Integrator::operator=(const Integrator& i)
//------------------------------------------------------------------------------
/**
 * Assignment operator
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

    return *this;
}

//------------------------------------------------------------------------------
// std::string Integrator::GetParameterText(const Integer id)
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
std::string Integrator::GetParameterText(const Integer id)
{
	switch (id)
	{
		case integrationAccuracy:
			return Integrator::PARAMETER_TEXT[id];
		case errorControlHold:
			return Integrator::PARAMETER_TEXT[id];
		case smallestTimeInterval:
			return Integrator::PARAMETER_TEXT[id];
		case minimumStepSize:
			return Integrator::PARAMETER_TEXT[id];
		case maximumStepSize:
			return Integrator::PARAMETER_TEXT[id];
		case numStepAttempts:
			return Integrator::PARAMETER_TEXT[id];
		default:
			return GmatBase::GetParameterText(id);
	}
}

//------------------------------------------------------------------------------
// Integer Integrator::GetParameterID(const std::string str)
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
Integer Integrator::GetParameterID(const std::string str)
{
	for (int i = 0; i < IntegratorParamCount; i++)
	{
		if (str == Integrator::PARAMETER_TEXT[i])
			return i;
	}

	return GmatBase::GetParameterID(str);
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
	switch (id)
	{
		case integrationAccuracy:
			return Integrator::PARAMETER_TYPE[id];
		case errorControlHold:
			return Integrator::PARAMETER_TYPE[id];
		case smallestTimeInterval:
			return Integrator::PARAMETER_TYPE[id];
		case minimumStepSize:
			return Integrator::PARAMETER_TYPE[id];
		case maximumStepSize:
			return Integrator::PARAMETER_TYPE[id];
		case numStepAttempts:
			return Integrator::PARAMETER_TYPE[id];
		default:
			return GmatBase::GetParameterType(id);
	}
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
	switch (id)
	{
		case stepSizeParameter:
			return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
		default:
			return GmatBase::GetParameterTypeString(id);
	}
}

//------------------------------------------------------------------------------
// Real Integrator::GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* Accessor method used to obtain a parameter value
 *
 *  For integrators, the user can use this function to get the integration
 *  accuracy (id == integrationAccuracy), toggle the fixed step propagation mode
 *  (RUN_FIXEDSTEP), and set the stepSize for fixed step mode (FIXED_STEPSIZE).
 *  If the RUN_FIXEDSTEP id is passed into the method, 1.0 is returned if running
 *  in fixed step mode, and 0.0 if in variable step mode.
 *
 *  @param id	integer id of the parameter being retrieved
 */
//------------------------------------------------------------------------------
Real Integrator::GetRealParameter(const Integer id)
{
    if (id == integrationAccuracy)
        return tolerance;

    else if (id == errorControlHold)
	{
        if (physicalModel)
            return physicalModel->GetErrorThreshold();
        return errorThreshold;
    }

    else if (id == smallestTimeInterval)
        return smallestTime;

    else if (id == minimumStepSize)
        return minimumStep;

    else if (id == maximumStepSize)
        return maximumStep;

    else return GmatBase::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real Integrator::SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
* Accessor method used to set a parameter value
 *
 * For integrators, the user can use this function to set the integration
 * accuracy (id == integrationAccuracy), toggle the fixed step propagation mode
 * (RUN_FIXEDSTEP), and set the stepSize for fixed step mode (FIXED_STEPSIZE).
 *
 * @param id	integer id of the parameter being set
 * @param value	The new value for the parameter
 */
//------------------------------------------------------------------------------
Real Integrator::SetRealParameter(const Integer id, const Real value)
{
    if (id == integrationAccuracy)
	{
        if (value <= 0.0)
            return false;
        tolerance = value;
    }

    else if (id == minimumStepSize)
	{
        if (value == 0.0)
            return false;
        minimumStep = fabs(value);
    }

    else if (id == maximumStepSize)
	{
        if (fabs(value) < minimumStep)
            return false;
        maximumStep = fabs(value);
    }

    else if (id == errorControlHold)
	{
        errorThreshold = fabs(value);
        if (physicalModel)
            physicalModel->SetErrorThreshold(errorThreshold);
    }

    else if (id == smallestTimeInterval)
        smallestTime = fabs(value);

    else
        return GmatBase::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// Integer Integrator::GetIntegerParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer Integrator::GetIntegerParameter(const Integer id)
{
	if (id == numStepAttempts)
        return maxStepAttempts;
	else
		return GmatBase::GetIntegerParameter(id);
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
	if (id == numStepAttempts)
	{
        if (value < 1.0)
            return GmatBase::SetIntegerParameter(id, value);;
        maxStepAttempts = (Integer)value;
    }
}

//------------------------------------------------------------------------------
// Integer Integrator::GetPropagatorOrder(void) const
//------------------------------------------------------------------------------
/**
 * Returns derivative order needed by the integrator; 1 by default
 */
//------------------------------------------------------------------------------
Integer Integrator::GetPropagatorOrder(void) const
{
    return 1;
}

//------------------------------------------------------------------------------
// void Integrator::SetPhysicalModel(PhysicalModel *pPhyscialModel)
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
    if (physicalModel) 
        physicalModel->SetErrorThreshold(errorThreshold);
}