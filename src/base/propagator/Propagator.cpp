//$Header$
//------------------------------------------------------------------------------
//                              Propagator
//------------------------------------------------------------------------------
// *** File Name : Propagator.cpp
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
//                           : 12/18/2002 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//
//                           : 3/3/2003 - D. Conway, Thinking Systems, Inc.
//                             Updates for changes in parm strings
//
//                           : 09/23/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//
//                           : 10/09/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - virtual char* GetParameterName(const int parm) const to
//                                  virtual std::string GetParameterName(const int parm) const
//                                - GetParameterName() from if statements to switch statements
//                              Additions:
//                                - GetParameterType()
//                                - GetParameterText()
//                                - GetParameterTypeString()
//                                - GetRealParameter()
//                                - SetRealParameter()
//                              Removals:
//                                - GetParameter()
//                                - SetParameter()
//                                - ParameterCount()
//
//                           : 10/15/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - All double types to Real types
//                                - All primitive int types to Integer types
//                - STEP_SIZE_PARAMETER to stepSizeParameter
//                              Removals:
//                                - static Real parameterUndefined
//                                - SetUndefinedValue()
//                                - GetParameterName(), replaced by GetParameterText()
//                              Additions:
//                                - PARAMTER_TEXT[]
//                                - PARAMETER_TYPE[]
//
// **************************************************************************

#include "Propagator.hpp"
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "PhysicalModel.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_PROP_RERUN

//---------------------------------
// static data
//---------------------------------
const std::string
Propagator::PARAMETER_TEXT[PropagatorParamCount - GmatBaseParamCount] =
{
    //loj: 3/18/04 field name should not have blank spaces
    //"Step Size (sec)"
    "InitialStepSize"
};

const Gmat::ParameterType
Propagator::PARAMETER_TYPE[PropagatorParamCount - GmatBaseParamCount] =
{
    Gmat::REAL_TYPE
};

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// Propagator::Propagator(void)
//------------------------------------------------------------------------------
/**
 * Default base class constructor
 * The default constructor for the Propagator class sets the stepSize to 60 
 * seconds, sets the member pointers to NULL, the dimension to 0, and the 
 * initialized flag to false.
 */
//------------------------------------------------------------------------------
Propagator::Propagator(const std::string &typeStr,
                       const std::string &nomme)
    : GmatBase(Gmat::PROPAGATOR, typeStr, nomme),
      stepSize            (60.0),
      stepSizeBuffer      (60.0),
      initialized         (false),
      inState             (NULL),
      outState            (NULL),
      dimension           (0),
      physicalModel       (NULL)
{
    // GmatBase data
   objectTypes.push_back(Gmat::PROPAGATOR);
   objectTypeNames.push_back("Propagator");
   parameterCount = PropagatorParamCount;
}

//------------------------------------------------------------------------------
// Propagator::~Propagator()
//------------------------------------------------------------------------------
/**
 * Base class destructor
 * The base class destructor does not perform any actions
 */
//------------------------------------------------------------------------------
Propagator::~Propagator()
{
}

//------------------------------------------------------------------------------
// Propagator::Propagator(const Propagator& p)
//------------------------------------------------------------------------------
/**
 * The copy constructor
 */
//------------------------------------------------------------------------------
Propagator::Propagator(const Propagator& p)
    : GmatBase            (p),
      stepSize            (p.stepSize),
      stepSizeBuffer      (p.stepSizeBuffer),
      initialized         (false),
      inState             (NULL),
      outState            (NULL),
      dimension           (p.dimension),
      physicalModel       (NULL)
{
    // GmatBase data
    parameterCount = PropagatorParamCount;
}

//------------------------------------------------------------------------------
// Propagator& Propagator::operator=(const Propagator& p)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
//------------------------------------------------------------------------------
Propagator& Propagator::operator=(const Propagator& p)
{
    if (this == &p)
        return *this;

    stepSize       = p.stepSize;
    stepSizeBuffer = p.stepSizeBuffer;
    dimension      = p.dimension;

    inState = outState = NULL;
    physicalModel = NULL;

    initialized = false;

    return *this;
}

//------------------------------------------------------------------------------
// std::string Propagator::GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string Propagator::GetParameterText(const Integer id) const
{
    if (id >= STEP_SIZE && id < PropagatorParamCount)
        return PARAMETER_TEXT[id - GmatBaseParamCount];
    else
        return GmatBase::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer Propagator::GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer Propagator::GetParameterID(const std::string &str) const
{
    for (Integer i = STEP_SIZE; i < PropagatorParamCount; i++)
    {
        if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
            return i;
    }
        
    return GmatBase::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType Propagator::GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Propagator::GetParameterType(const Integer id) const
{
    if (id >= STEP_SIZE && id < PropagatorParamCount)
        return PARAMETER_TYPE[id - GmatBaseParamCount];
    else
        return GmatBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string Propagator::GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string Propagator::GetParameterTypeString(const Integer id) const
{
    if (id >= STEP_SIZE && id < PropagatorParamCount)
        return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
    else
        return GmatBase::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// Real Propagator::GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real Propagator::GetRealParameter(const Integer id) const
{
    if (id == STEP_SIZE)
        return stepSizeBuffer;
        
    return GmatBase::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
Real Propagator::GetRealParameter(const std::string &label) const
{
    Integer id = GetParameterID(label);
    
    return GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real Propagator::SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real Propagator::SetRealParameter(const Integer id, const Real value)
{
    if (id == STEP_SIZE)
    {
        stepSizeBuffer = value;
        return stepSize;
    }
    return GmatBase::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
Real Propagator::SetRealParameter(const std::string &label, const Real value)
{
    return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// bool Propagator::Initialize()
//------------------------------------------------------------------------------
/**
 * Initialize the propagation system
 * This method is used to perform any internal initialization needed by the 
 * propagator, and to initialize the corresponding PhysicalModel.
 * This implementation sets the inState and outState pointers to point to the
 * PhysicalModel's state vector.  You can change that choice by overriding this
 * method in your derived classes.
 */
//------------------------------------------------------------------------------
bool Propagator::Initialize()
{
    if (physicalModel != NULL) 
    {
        //MessageInterface::ShowMessage("Propagator::Initialize() calling physicalModel->Initialize() \n");
        if ( physicalModel->Initialize() )
            initialized = true;

        //MessageInterface::ShowMessage("Propagator::Initialize() initialized = %d\n",
        //                              initialized);

        inState  = physicalModel->GetState();
        outState = physicalModel->GetState();
        
        stepSize = stepSizeBuffer;
    }
    else
       throw PropagatorException("Propagator::Initialize -- Force model is not defined");
    
    if (!initialized)
       throw PropagatorException("Propagator failed to initialize");
       
    return true;
}

//------------------------------------------------------------------------------
// void Propagator::SetPhysicalModel(PhysicalModel *pPhyscialModel)
//------------------------------------------------------------------------------
/**
 * Connects the propagation system to the physical model
 *
 * @param pPhyscialModel    Pointer to the physical model
 */
//------------------------------------------------------------------------------
void Propagator::SetPhysicalModel(PhysicalModel *pPhysicalModel)
{
    physicalModel = pPhysicalModel;
}

//------------------------------------------------------------------------------
// void Propagator::Update()
//------------------------------------------------------------------------------
/**
 * Envoked to force a propagator reset if the PhysicalModel changes
 */
//------------------------------------------------------------------------------
void Propagator::Update()
{
   #ifdef DEBUG_PROP_RERUN
      static int count = 0;
      MessageInterface::ShowMessage(
         "Propagator::Update() called (iteration %d)\n", ++count);
   #endif
   stepSize = stepSizeBuffer;
}

//------------------------------------------------------------------------------
// const Real * Propagator::AccessOutState()
//------------------------------------------------------------------------------
/**
 * Provide a look at the output state
 */
//------------------------------------------------------------------------------
const Real * Propagator::AccessOutState()
{
    return outState;
}

//------------------------------------------------------------------------------
// Integer Propagator::GetPropagatorOrder() const
//------------------------------------------------------------------------------
/**
 * Returns derivative order needed by the propagator; 0 for analytic
 */
//------------------------------------------------------------------------------
Integer Propagator::GetPropagatorOrder() const
{
    return 0;
}

//------------------------------------------------------------------------------
// bool Propagator::Step(Real dt)
//------------------------------------------------------------------------------
/**
 * Evolves the physical model over the specified time
 * This method sets the default timestep to the input value, and then advances 
 * the system by one timestep.  If the ssytem has not been initialized, then
 * no action is taken.  
 * If the step is taken successfully, the method returns true; otherwise, it 
 * returns false.
 *
 * @param dt    The timestep to take
 */
//------------------------------------------------------------------------------
bool Propagator::Step(Real dt)
{
    #ifdef DEBUG_PROPAGATOR_FLOW
       MessageInterface::ShowMessage("^");
    #endif
    if (initialized)
    {
        stepSize = dt;
        return Step();
    }
    return false;
}

//------------------------------------------------------------------------------
// bool Propagator::RawStep(Real dt)
//------------------------------------------------------------------------------
/**
 * Method used to take an uncontrolled step of a specified size
 * This method sets the stepSize to the input value and then calls the class's 
 * RawStep() method.
 * 
 * @param dt    The desired raw step size
 */
//------------------------------------------------------------------------------
bool Propagator::RawStep(Real dt)
{
    #ifdef DEBUG_PROPAGATOR_FLOW
       MessageInterface::ShowMessage("!");
    #endif
    Real ctlstepsize = stepSize;
    stepSize = dt;
    bool retval = RawStep();
    stepSize = ctlstepsize;
    return retval;
}

