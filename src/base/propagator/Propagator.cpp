//$Id$
//------------------------------------------------------------------------------
//                              Propagator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// *** File Name : Propagator.cpp
// *** Created   : October 1, 2002
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
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

#include <sstream>
#include "Propagator.hpp"
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "PhysicalModel.hpp"
#include "ODEModel.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_PROP_RERUN
//#define DEBUG_INITIALIZATION

//---------------------------------
// static data
//---------------------------------
const std::string
Propagator::PARAMETER_TEXT[PropagatorParamCount - GmatBaseParamCount] =
{
    "InitialStepSize",
    "AlwaysUpdateStepsize"
};

const Gmat::ParameterType
Propagator::PARAMETER_TYPE[PropagatorParamCount - GmatBaseParamCount] =
{
    Gmat::REAL_TYPE,
    Gmat::BOOLEAN_TYPE
};

const Real Propagator::STEP_SIZE_TOLERANCE = 0.0001;   // 0.1 millisec
//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// Propagator(const std::string &typeStr, const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * Default base class constructor
 *
 * The default constructor for the Propagator class sets the stepSize to 60 
 * seconds, sets the member pointers to NULL, the dimension to 0, and the 
 * initialized flag to false.
 *
 * @param typeStr The scripted subtype for this Propagator
 * @param nomme The name of the object that gets constructed
 *
 */
//------------------------------------------------------------------------------
Propagator::Propagator(const std::string &typeStr,
                       const std::string &nomme)
    : GmatBase(Gmat::PROPAGATOR, typeStr, nomme),
      stepSize            (60.0),
      stepSizeBuffer      (60.0),
      resetInitialData    (true),
      alwaysUpdateStepsize(false),
      inState             (NULL),
      outState            (NULL),
      dimension           (0),
      physicalModel       (NULL),
      finalStep           (false),
      j2kBodyName         ("Earth"),
      j2kBody             (NULL),
      centralBody         ("Earth"),
      propOrigin          (NULL)
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
 *
 * @param p The object that is copied into this new one
 */
//------------------------------------------------------------------------------
Propagator::Propagator(const Propagator& p)
    : GmatBase            (p),
      stepSize            (p.stepSize),
      stepSizeBuffer      (p.stepSizeBuffer),
//      initialized         (false),
      resetInitialData    (true),
      alwaysUpdateStepsize(p.alwaysUpdateStepsize),
      inState             (NULL),
      outState            (NULL),
      dimension           (p.dimension),
      physicalModel       (NULL),
      finalStep           (false),
      j2kBodyName         (p.j2kBodyName),
      j2kBody             (NULL),
      centralBody         (p.centralBody),
      propOrigin          (NULL)
{
   isInitialized = false;
}

//------------------------------------------------------------------------------
// Propagator& Propagator::operator=(const Propagator& p)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param p The object that is provides data for into this one
 *
 * @return This propagator, configured to match p.
 */
//------------------------------------------------------------------------------
Propagator& Propagator::operator=(const Propagator& p)
{
    if (this == &p)
        return *this;

    GmatBase::operator=(p);

    stepSize       = p.stepSize;
    stepSizeBuffer = p.stepSizeBuffer;
    dimension      = p.dimension;

    inState = outState = NULL;
    physicalModel = NULL;

    isInitialized = false;
    resetInitialData = true;
    alwaysUpdateStepsize = p.alwaysUpdateStepsize;
    finalStep = false;    

    j2kBodyName = p.j2kBodyName;
    j2kBody     = NULL;
    centralBody = p.centralBody;
    propOrigin  = NULL;

    return *this;
}

//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Renames reference objects used in this class.
 *
 * @param <type> reference object type.
 * @param <oldName> object name to be renamed.
 * @param <newName> new object name.
 * 
 * @return true if object name changed, false if not.
 */
//---------------------------------------------------------------------------
bool Propagator::RenameRefObject(const Gmat::ObjectType type,
                                 const std::string &oldName,
                                 const std::string &newName)
{
   // There is nothing to check for now
   return true;
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
    if (id >= GmatBaseParamCount && id < PropagatorParamCount)
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
   for (Integer i = GmatBaseParamCount; i < PropagatorParamCount; ++i)
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
        
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
    if (id >= GmatBaseParamCount && id < PropagatorParamCount)
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
    if (id >= GmatBaseParamCount && id < PropagatorParamCount)
        return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
    else
        return GmatBase::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
// bool Propagator::IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Identifies parameters that are or are not scripted
 *
 * @param id ID of the parameter in question
 *
 * @return true if the parameter is not scripted, false if it is scripted
 */
//------------------------------------------------------------------------------
bool Propagator::IsParameterReadOnly(const Integer id) const
{
   if (id == AlwaysUpdateStepsize)
      return true;

   return GmatBase::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// bool Propagator::IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Identifies parameters that are or are not scripted
 *
 * @param label The script string of the parameter in question
 *
 * @return true if the parameter is not scripted, false if it is scripted
 */
//------------------------------------------------------------------------------
bool Propagator::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
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
    if (id == INITIAL_STEP_SIZE)
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
   if (id == INITIAL_STEP_SIZE)
   {
      if (GmatMathUtil::IsEqual(value, 0.0, STEP_SIZE_TOLERANCE))
      {
         std::stringstream ss;
         ss << "Initial Step Size must not be zero (tolerance = " << STEP_SIZE_TOLERANCE << " seconds).";
         throw PropagatorException(ss.str());
      }
      stepSizeBuffer = value;
      return stepSizeBuffer;
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
// Real GetRealParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Pass through method for getting a real parameter
 *
 * @param id The parameter ID
 * @param index Array index for the parameter
 *
 * @return The parameter value after the call
 */
//------------------------------------------------------------------------------
Real Propagator::GetRealParameter(const Integer id, const Integer index) const
{
   return GmatBase::GetRealParameter(id, index);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer row,
//       const Integer col) const
//------------------------------------------------------------------------------
/**
 * Pass through method for getting a real parameter
 *
 * @param id The parameter ID
 * @param row Array row index for the parameter
 * @param col Array column index for the parameter
 *
 * @return The parameter value after the call
 */
//------------------------------------------------------------------------------
Real Propagator::GetRealParameter(const Integer id, const Integer row,
      const Integer col) const
{
   return GmatBase::GetRealParameter(id, row, col);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Pass through method for setting a real parameter
 *
 * @param id The parameter ID
 * @param value The new parameter value
 * @param index Array index for the parameter
 *
 * @return The parameter value after the call
 */
//------------------------------------------------------------------------------
Real Propagator::SetRealParameter(const Integer id, const Real value,
      const Integer index)
{
   return GmatBase::SetRealParameter(id, value, index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value, const Integer row,
//       const Integer col)
//------------------------------------------------------------------------------
/**
 * Pass through method for getting a real parameter
 *
 * @param id The parameter ID
 * @param value The new parameter value
 * @param row Array row index for the parameter
 * @param col Array column index for the parameter
 *
 * @return The parameter value after the call
 */
//------------------------------------------------------------------------------
Real Propagator::SetRealParameter(const Integer id, const Real value,
      const Integer row, const Integer col)
{
   return GmatBase::SetRealParameter(id, value, row, col);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves Boolean parameters
 *
 * @param id ID of the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
bool Propagator::GetBooleanParameter(const Integer id) const
{
   if (id == AlwaysUpdateStepsize)
      return alwaysUpdateStepsize;

   return GmatBase::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets Boolean parameters
 *
 * @param id ID of the parameter
 * @param value The new parameter value
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
bool Propagator::SetBooleanParameter(const Integer id, const bool value)
{
   MessageInterface::ShowMessage("Setting ID %d to %s\n", id,
         (value ? "true" : "false"));

   if (id == AlwaysUpdateStepsize)
   {
      alwaysUpdateStepsize = value;
      return alwaysUpdateStepsize;
   }

   return GmatBase::SetBooleanParameter(id, value);
}

//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves Boolean parameters from an array of Booleans
 *
 * @param id ID of the parameter
 * @param index The parameter's index in the array
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
bool Propagator::GetBooleanParameter(const Integer id, const Integer index) const
{
   return GmatBase::GetBooleanParameter(id, index);
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets Boolean parameters in an array of Booleans
 *
 * @param id ID of the parameter
 * @param value The new parameter value
 * @param index The parameter's index in the array
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
bool Propagator::SetBooleanParameter(const Integer id, const bool value,
                                 const Integer index)
{
   return GmatBase::SetBooleanParameter(id, value, index);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* Retrieves Boolean parameters
*
* @param label Script label for the parameter
*
* @return The parameter value
 */
//------------------------------------------------------------------------------
bool Propagator::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets Boolean parameters
 *
 * @param label Script label for the parameter
 * @param value The new parameter value
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
bool Propagator::SetBooleanParameter(const std::string &label, const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves Boolean parameters from an array of Booleans
 *
 * @param label Script label for the parameter
 * @param index The parameter's index in the array
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
bool Propagator::GetBooleanParameter(const std::string &label,
                                 const Integer index) const
{
   return GetBooleanParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label, const bool value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets Boolean parameters in an array of Booleans
 *
 * @param label Script label for the parameter
 * @param value The new parameter value
 * @param index The parameter's index in the array
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
bool Propagator::SetBooleanParameter(const std::string &label, const bool value,
                                     const Integer index)
{
   return SetBooleanParameter(GetParameterID(label), value, index);
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
   if (UsesODEModel())
   {
      if (physicalModel != NULL)
      {
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("Propagator::Initialize() calling "
                  "physicalModel->Initialize() \n");
         #endif

         if ( physicalModel->Initialize() )
            isInitialized = true;

         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage(
               "Propagator::Initialize() initialized = %d\n", initialized);
         #endif

         inState  = physicalModel->GetState();
         outState = physicalModel->GetState();

         if ((resetInitialData) || (alwaysUpdateStepsize))
         {
            stepSize = stepSizeBuffer;
            resetInitialData = false;
         }
      }
      else
         throw PropagatorException("Propagator::Initialize -- Force model is "
               "not defined");
   }
   else
      isInitialized = true;

    
    if (!isInitialized)
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
 * Invoked to force a propagator reset if the PhysicalModel changes
 */
//------------------------------------------------------------------------------
void Propagator::Update(bool forwards)
{
   #ifdef DEBUG_PROP_RERUN
      static int count = 0;
      MessageInterface::ShowMessage(
         "Propagator::Update() called (iteration %d)\n", ++count);
   #endif
   if (resetInitialData)
   {
      stepSize = stepSizeBuffer;
      resetInitialData = false;
   }
   else
   {
      stepSize = (forwards ? fabs(stepSize) : -fabs(stepSize));
   }

   #ifdef DEBUG_PROP_RERUN
      MessageInterface::ShowMessage(
         "Propagator::Update() step size = %lf\n", stepSize);
   #endif
}

//------------------------------------------------------------------------------
// void Propagator::ResetInitialData()
//------------------------------------------------------------------------------
/**
 * Sets the propagator for the first call in a run.
 */
//------------------------------------------------------------------------------
void Propagator::ResetInitialData()
{
   resetInitialData = true;
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
// bool UsesODEModel()
//------------------------------------------------------------------------------
/**
 * Used to tell the PropSetup if an ODE model is needed for the propagator
 *
 * @return true if an ODEModel is required, false if not
 */
//------------------------------------------------------------------------------
bool Propagator::UsesODEModel()
{
   return true;
}


//------------------------------------------------------------------------------
// void SetPropStateManager(PropagationStateManager *sm)
//------------------------------------------------------------------------------
/**
 * Interface used by derived classes to set the PSM for propagators that need it
 *
 * This method by default does nothing.  Propagators that do not use an ODEModel
 * override the method to set the PSM for state updates.
 *
 * @param sm The propagation state manager
 *
 * This method is provided so that the Propagator derivatives that do not use
 * an ODE model support interfaces needed by the PropagationEnabledCommands.
 */
//------------------------------------------------------------------------------
void Propagator::SetPropStateManager(PropagationStateManager *sm)
{
   if (physicalModel)
      if (physicalModel->IsOfType(Gmat::ODE_MODEL))
         ((ODEModel*)(physicalModel))->SetPropStateManager(sm);
}


//------------------------------------------------------------------------------
// Integer GetDimension()
//------------------------------------------------------------------------------
/**
 * Returns the size of the propagation state vector
 *
 * @return The vector size
 *
 * This method is provided so that the Propagator derivatives that do not use
 * an ODE model support interfaces needed by the PropagationEnabledCommands.
 */
//------------------------------------------------------------------------------
Integer Propagator::GetDimension()
{
   if (physicalModel)
      return physicalModel->GetDimension();
   return 0;
}


//------------------------------------------------------------------------------
// Real* GetState()
//------------------------------------------------------------------------------
/**
 * Retrieves the propagation state vector
 *
 * @return The vector
 *
 * This method is provided so that the Propagator derivatives that do not use
 * an ODE model support interfaces needed by the PropagationEnabledCommands.
 */
//------------------------------------------------------------------------------
Real* Propagator::GetState()
{
   if (physicalModel)
      return physicalModel->GetState();
   return NULL;
}


//------------------------------------------------------------------------------
// Real* GetJ2KState()
//------------------------------------------------------------------------------
/**
 * Retrieces the J2000 body referenced propagation state vector
 *
 * @return The state vector in the correct coordinate frame
 *
 * This method is provided so that the Propagator derivatives that do not use
 * an ODE model support interfaces needed by the PropagationEnabledCommands.
 */
//------------------------------------------------------------------------------
Real* Propagator::GetJ2KState()
{
   if (physicalModel)
      return physicalModel->GetJ2KState();
   return NULL;
}


//------------------------------------------------------------------------------
// void UpdateSpaceObject(Real newEpoch)
//------------------------------------------------------------------------------
/**
 * Updates the propagated objects with data in the propagation state vector
 *
 * @param newEpoch The epoch for the data updates
 *
 * This method is provided so that the Propagator derivatives that do not use
 * an ODE model support interfaces needed by the PropagationEnabledCommands.
 */
//------------------------------------------------------------------------------
void Propagator::UpdateSpaceObject(Real newEpoch)
{
   if (physicalModel)
      if (physicalModel->IsOfType(Gmat::ODE_MODEL))
         ((ODEModel*)(physicalModel))->UpdateSpaceObject(newEpoch);
}


//------------------------------------------------------------------------------
// void UpdateFromSpaceObject()
//------------------------------------------------------------------------------
/**
 * Updates the propagation state vector from the data in the propagating objects
 *
 * This method is provided so that the Propagator derivatives that do not use
 * an ODE model support interfaces needed by the PropagationEnabledCommands.
 */
//------------------------------------------------------------------------------
void Propagator::UpdateFromSpaceObject()
{
   if (physicalModel)
      if (physicalModel->IsOfType(Gmat::ODE_MODEL))
         ((ODEModel*)(physicalModel))->UpdateFromSpaceObject();
}


//------------------------------------------------------------------------------
// void RevertSpaceObject()
//------------------------------------------------------------------------------
/**
 * Restores a propagation state vector that was buffered in the BufferState()
 * method.
 *
 * This method is provided so that the Propagator derivatives that do not use
 * an ODE model support interfaces needed by the PropagationEnabledCommands.
 */
//------------------------------------------------------------------------------
void Propagator::RevertSpaceObject()
{
   if (physicalModel)
      if (physicalModel->IsOfType(Gmat::ODE_MODEL))
         ((ODEModel*)(physicalModel))->RevertSpaceObject();
}


//------------------------------------------------------------------------------
// void BufferState()
//------------------------------------------------------------------------------
/**
 * Saves the propagation state vector so it can be reset later.
 *
 * This method is provided so that the Propagator derivatives that do not use
 * an ODE model support interfaces needed by the PropagationEnabledCommands.
 */
//------------------------------------------------------------------------------
void Propagator::BufferState()
{
   if (physicalModel)
      if (physicalModel->IsOfType(Gmat::ODE_MODEL))
         ((ODEModel*)(physicalModel))->BufferState();
}


//------------------------------------------------------------------------------
// Real GetTime()
//------------------------------------------------------------------------------
/**
 * Retrieves the propagator's time tracking parameter.
 *
 * This method is provided so that the Propagator derivatives that do not use
 * an ODE model support interfaces needed by the PropagationEnabledCommands.
 *
 * @return The time tracking parameter
 */
//------------------------------------------------------------------------------
Real Propagator::GetTime()
{
   if (physicalModel != NULL)
      return physicalModel->GetTime();

   return 0.0;
}


//------------------------------------------------------------------------------
// void SetTime(Real t)
//------------------------------------------------------------------------------
/**
 * Sets the propagator's time tracking parameter.
 *
 * This method is provided so that the Propagator derivatives that do not use
 * an ODE model support interfaces needed by the PropagationEnabledCommands.
 *
 * @param t The new value for the parameter
 */
//------------------------------------------------------------------------------
void Propagator::SetTime(Real t)
{
   if (physicalModel != NULL)
      physicalModel->SetTime(t);
}


//------------------------------------------------------------------------------
// bool PropagatesForward()
//------------------------------------------------------------------------------
/**
 * Accesses the current step direction
 *
 * @return true for forward propagation, false for backwards
 */
//------------------------------------------------------------------------------
bool Propagator::PropagatesForward()
{
   return (stepSize > 0.0 ? true : false);
}


//------------------------------------------------------------------------------
// void SetForwardPropagation(bool tf)
//------------------------------------------------------------------------------
/**
 * Sets the current propagation direction
 *
 * @param tf Flag indicating if the direction is forwards (true) or backwards
 */
//------------------------------------------------------------------------------
void Propagator::SetForwardPropagation(bool tf)
{
   stepSize       = (tf ? 1.0 : -1.0) * GmatMathUtil::Abs(stepSize);
   stepSizeBuffer = (tf ? 1.0 : -1.0) * GmatMathUtil::Abs(stepSizeBuffer);
}


//------------------------------------------------------------------------------
// bool Step(Real dt)
//------------------------------------------------------------------------------
/**
 * Evolves the physical model over the specified time
 * This method sets the default timestep to the input value, and then advances 
 * the system by one timestep.  If the system has not been initialized, then
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
    if (isInitialized)
    {
        stepSize = dt;
        return Step();
    }
    return false;
}


//------------------------------------------------------------------------------
// void SetAsFinalStep(bool fs)
//------------------------------------------------------------------------------
/**
 * Method used to toggle the finalStep flag.
 * 
 * @param fs The setting for the flag.
 */
//------------------------------------------------------------------------------
void Propagator::SetAsFinalStep(bool fs)
{
   #ifdef DEBUG_PROPAGATOR_FLOW
      MessageInterface::ShowMessage("Turning final step flag %s\n",
         (fs ? "On" : "Off"));
   #endif
   
   finalStep = fs;
}


//------------------------------------------------------------------------------
// bool RawStep(Real dt)
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


//------------------------------------------------------------------------------
// Protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void MoveToOrigin(Real newEpoch)
//------------------------------------------------------------------------------
/**
 * Provides the interface that Propagators that do not use ODEModels use to
 * translate from the J2000 body to the propagator's central body
 *
 * @param newEpoch The epoch of the state that is translated
 */
//------------------------------------------------------------------------------
void Propagator::MoveToOrigin(Real newEpoch)
{
}

//------------------------------------------------------------------------------
// void ReturnFromOrigin(Real newEpoch)
//------------------------------------------------------------------------------
/**
 * Provides the interface that Propagators that do not use ODEModels use to
 * translate from the propagator's central body to the J2000 body
 *
 * @param newEpoch The epoch of the state that is translated
 */
//------------------------------------------------------------------------------
void Propagator::ReturnFromOrigin(Real newEpoch)
{
}
