//$Id$
//------------------------------------------------------------------------------
//                              PhysicalModel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// *** File Name : PhysicalModel.cpp
// *** Created   : October 1, 2002
// ***************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)      ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                   ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                              ***
// ***                                                                     ***
// ***  This software is subject to the Software Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Software Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is   ***
// ***  strictly prohibited.                                               ***
// ***                                                                     ***
// ***                                                                     ***
// ***  Header Version: July 12, 2002                                      ***
// ***************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 11/26/2002 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//
//                           : 1/8/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//
//                           : 2/5/2003 - D. Conway, Thinking Systems, Inc.
//                             Incorporated the Derivative class into this
//                             class and removed Derivative from the class
//                             heirarchy
//
//                           : 3/3/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated parameter strings to include units;
//                             Added code to switch between relative and
//                             absolute error calculations
//
//                           : 09/24/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - Updated style using GMAT cpp style guide//
//
//                           : 10/15/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - All double types to Real types
//                                - All primitive int types to Integer types
//                                - virtual char* GetParameterName(const int parm) const to
//                                  virtual std::string GetParameterName(const int parm) const
//                                - Changed GetParameterName() from if statements to switch statements
//                              Removals:
//                                - static Real parameterUndefined
//                                - SetUndefinedValue()
//                                - ParameterCount()
//                                - GetParameter()
//                                - SetParameter()
//                              Additions:
//                                - PARAMTER_TEXT[]
//                                - PARAMETER_TYPE[]
//                                - GetParameterText()
//                                - GetParameterID()
//                                - GetParameterType()
//                                - GetParameterTypeString()
//                                - GetRealParameter()
//                                - SetRealParameter()
//
//                           : 10/20/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - Fixed format.
//                                - parameterCount to PhysicalModelParamCount.
//                              Removals:
//                                - GetParameterName()
//
//                           : 10/23/2003 - D. Conway, Thinking Systems, Inc. &
//                                          W. Waktola, Missions Applications Branch
//                              Changes:
//                                - Changed constructor from PhysicalModel::PhysicalModel(void) to
//                                  PhysicalModel(Gmat::ObjectType typeId, const std::string &typeStr,
//                                  const std::string &nomme = "")
//                                - Added parameterCount = 1 in constructors
//                                - In SetErrorThreshold(), changed statement from relativeErrorThreshold = fabs(thold);
//                                  to relativeErrorThreshold = (thold >= 0.0 ? thold : -thold);
//
// **************************************************************************

#include "PhysicalModel.hpp"
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CelestialBody.hpp"
#include "MessageInterface.hpp"
#include "TimeTypes.hpp"


//#define PHYSICAL_MODEL_DEBUG_INIT
//#define DEBUG_INITIALIZATION
//#define DEBUG_PM_CONSTRUCT
//#define DEBUG_STATE_ALLOCATION

//#define DEBUG_MEMORY

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------
const std::string
PhysicalModel::PARAMETER_TEXT[PhysicalModelParamCount - GmatBaseParamCount] =
{
   "Epoch",
   "ElapsedSeconds",
   "BodyName",
   "DerivativeID",
};

const Gmat::ParameterType
PhysicalModel::PARAMETER_TYPE[PhysicalModelParamCount - GmatBaseParamCount] =
{
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::OBJECT_TYPE,
   Gmat::INTEGER_TYPE,
};

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// PhysicalModel(Gmat::ObjectType typeId, const std::string &typeStr,
//    const std::string &nomme = "")
//------------------------------------------------------------------------------
/**
 * Constructor for the Physical Model base class
 *
 * This constructor sets the size of the physical model to one variable, and
 * NULLs the state pointer.  Derived classes should set the dimension parameter
 * to a more appropriate value; the Initialize() method is used to allocate 
 * state data array.
 */
//------------------------------------------------------------------------------
PhysicalModel::PhysicalModel(Gmat::ObjectType id, const std::string &typeStr,
                             const std::string &nomme) :
   GmatBase                    (id, typeStr, nomme),
   body                        (NULL),
   forceOrigin                 (NULL),
   bodyName                    ("Earth"),
   dimension                   (1),
   stateChanged                (false),
   theState                    (NULL),
   modelState                  (NULL),
   rawState                    (NULL),
   epoch                       (21545.0),
   elapsedTime                 (0.0),
   prevElapsedTime             (0.0),
   deriv                       (NULL),
   relativeErrorThreshold      (0.10),
   solarSystem                 (NULL),
   fillCartesian               (true),
   cartesianStart              (0),
   cartesianCount              (0),
   fillSTM                     (false),
   stmStart                    (-1),
   stmCount                    (0),
   fillAMatrix                 (false),
   aMatrixStart                (-1),
   aMatrixCount                (0)
{
   objectTypes.push_back(Gmat::PHYSICAL_MODEL);
   objectTypeNames.push_back("PhysicalModel");
   parameterCount = PhysicalModelParamCount;

   // Do not allow ODE model changes in command mode
   blockCommandModeAssignment = true;
}
             
//------------------------------------------------------------------------------
// PhysicalModel::~PhysicalModel()
//------------------------------------------------------------------------------
/**
 * Destructor for the model
 * If the state array has been allocated, this destructor destroys it.
 */
//------------------------------------------------------------------------------
PhysicalModel::~PhysicalModel()
{
   #ifdef DEBUG_STATE_ALLOCATION
      MessageInterface::ShowMessage("Deleting PhysicalModel (type %s) at %p\n",
            typeName.c_str(), this);
   #endif

   if (rawState != modelState)
      if (rawState)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (rawState, "rawState", "PhysicalModel::~PhysicalModel()",
             "deleting rawState", this);
         #endif
         #ifdef DEBUG_STATE_ALLOCATION
            MessageInterface::ShowMessage("Deleting rawState at %p\n",
                  rawState);
         #endif
         delete [] rawState;
      }
   
   if (modelState)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (modelState, "modelState", "PhysicalModel::~PhysicalModel()",
          "deleting modelState", this);
      #endif

      #ifdef DEBUG_STATE_ALLOCATION
         MessageInterface::ShowMessage("Deleting modelState at %p\n",
               modelState);
      #endif
      delete [] modelState;
   }
   
   if (deriv)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (deriv, "deriv", "PhysicalModel::~PhysicalModel()",
          "deleting deriv", this);
      #endif
      delete [] deriv;
   }
}

//------------------------------------------------------------------------------
// PhysicalModel::PhysicalModel(const PhysicalModel& pm)
//------------------------------------------------------------------------------
/**
 * The copy constructor for the physical model
 */
//------------------------------------------------------------------------------
PhysicalModel::PhysicalModel(const PhysicalModel& pm) :
   GmatBase                    (pm),
   /// @note: Since the next two are global objects, assignment works
   body                        (pm.body),
   forceOrigin                 (pm.forceOrigin),
   bodyName                    (pm.bodyName),
   dimension                   (pm.dimension),
   stateChanged                (pm.stateChanged),
   theState                    (NULL),
   modelState                  (NULL),
   rawState                    (NULL),
   epoch                       (pm.epoch),
   elapsedTime                 (pm.elapsedTime),
   prevElapsedTime             (pm.prevElapsedTime),
   deriv                       (NULL),
   derivativeIds               (pm.derivativeIds),
   derivativeNames             (pm.derivativeNames),
   relativeErrorThreshold      (pm.relativeErrorThreshold),
   solarSystem                 (pm.solarSystem),
   fillCartesian               (pm.fillCartesian),
   cartesianStart              (pm.cartesianStart),
   cartesianCount              (pm.cartesianCount),
   fillSTM                     (pm.fillSTM),
   stmStart                    (pm.stmStart),
   stmCount                    (pm.stmCount),
   fillAMatrix                 (pm.fillAMatrix),
   aMatrixStart                (pm.aMatrixStart),
   aMatrixCount                (pm.aMatrixCount)
{
   if (pm.modelState != NULL) 
   {
      if (modelState != NULL)
      {
         #ifdef DEBUG_STATE_ALLOCATION
            MessageInterface::ShowMessage("Deleting modelState at %p\n",
                  modelState);
         #endif
         delete [] modelState;
         modelState = NULL;
      }
      modelState = new Real[dimension];

      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (modelState, "modelState", "PhysicalModel::PhysicalModel(copy)",
          "modelState = new Real[dimension]", this);
      #endif
      if (modelState != NULL) 
         memcpy(modelState, pm.modelState, dimension * sizeof(Real));
      else
         isInitialized = false;
   }
   else
   {
      if (modelState != NULL)
      {
         #ifdef DEBUG_STATE_ALLOCATION
            MessageInterface::ShowMessage("Deleting modelState at %p\n",
                  modelState);
         #endif
         delete [] modelState;
         modelState = NULL;
      }
   }
   rawState = modelState;
   
   if (pm.deriv != NULL) 
   {
      deriv = new Real[dimension];
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (deriv, "deriv", "PhysicalModel::PhysicalModel(copy)",
          "deriv = new Real[dimension]", this);
      #endif
      if (deriv != NULL) 
         memcpy(deriv, pm.deriv, dimension * sizeof(Real));
      else
         isInitialized = false;
   }
   else
      deriv = NULL;

   parameterCount = PhysicalModelParamCount;
}

//------------------------------------------------------------------------------
// PhysicalModel& PhysicalModel::operator=(const PhysicalModel& pm)
//------------------------------------------------------------------------------
/**
 * The assignment operator for the physical model
 */
//------------------------------------------------------------------------------
PhysicalModel& PhysicalModel::operator=(const PhysicalModel& pm)
{
   if (&pm == this)
      return *this;

   GmatBase::operator=(pm);
   #ifdef DEBUG_PM_CONSTRUCT
      MessageInterface::ShowMessage("Entering copy constructor for PM:  object name is: %s\n",
            pm.GetName().c_str());
      MessageInterface::ShowMessage("   epoch = %le\n", pm.epoch);
      MessageInterface::ShowMessage("   epoch from state = %le\n", (pm.theState)->GetEpoch());
   #endif
   
   /// @note: Since the next two are global objects, assignment works
   body        = pm.body;
   forceOrigin = pm.forceOrigin;

   bodyName        = pm.bodyName;
   dimension       = pm.dimension;
   isInitialized   = false; //pm.initialized;
   epoch           = pm.epoch;
   elapsedTime     = pm.elapsedTime;
   prevElapsedTime = pm.prevElapsedTime;
   relativeErrorThreshold = pm.relativeErrorThreshold;
   solarSystem     = pm.solarSystem;
   
   fillCartesian  = pm.fillCartesian;
   cartesianStart = pm.cartesianStart;
   cartesianCount = pm.cartesianCount;
   fillSTM        = pm.fillSTM;
   stmStart       = pm.stmStart;
   stmCount       = pm.stmCount;
   fillAMatrix    = pm.fillAMatrix;
   aMatrixStart   = pm.aMatrixStart;
   aMatrixCount   = pm.aMatrixCount;

   theState = pm.theState;

   if (pm.modelState) 
   {
      if (modelState) 
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (modelState, "modelState", "PhysicalModel::operator=()",
             "deleting modelState", this);
         #endif

         #ifdef DEBUG_STATE_ALLOCATION
            MessageInterface::ShowMessage("Deleting modelState at %p\n",
                  modelState);
         #endif
         delete [] modelState;
         modelState = NULL;
      }
      
      modelState = new Real[dimension];
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (modelState, "modelState", "ODEModel::operator=()",
          "modelState = new Real[dimension]", this);
      #endif
      
      if (modelState != NULL) 
         memcpy(modelState, pm.modelState, dimension * sizeof(Real));
      else
         isInitialized = false;
   
      stateChanged = pm.stateChanged;
   }

   rawState = modelState;
   
   if (pm.deriv) 
   {
      if (deriv)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (deriv, "deriv", "PhysicalModel::operator=()",
             "deleting deriv", this);
         #endif
         delete [] deriv;
         deriv = NULL;
      }
      
      deriv = new Real[dimension];
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (deriv, "deriv", "ODEModel::operator=()",
          "deriv = new Real[dimension]", this);
      #endif
      
      if (deriv != NULL) 
         memcpy(deriv, pm.deriv, dimension * sizeof(Real));
      else
         isInitialized = false;
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// CelestialBody* GetBody()
//------------------------------------------------------------------------------
/**
*
 */
//------------------------------------------------------------------------------
CelestialBody* PhysicalModel::GetBody()
{
   return body;
}

//------------------------------------------------------------------------------
// CelestialBody* GetBodyName()
//------------------------------------------------------------------------------
/**
 *
 */
//------------------------------------------------------------------------------
std::string PhysicalModel::GetBodyName()
{
   return bodyName;
}

//------------------------------------------------------------------------------
// void SetBody(CelestialBody *body)
//------------------------------------------------------------------------------
/**
 *
 */
//------------------------------------------------------------------------------
void PhysicalModel::SetBody(CelestialBody *theBody)
{  
   if (theBody != NULL)
   {
      if (body != NULL)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (deriv, "deriv", "PhysicalModel::SetBody()",
             "deleting deriv", this);
         #endif
         delete body;
      }
   }
   
   body = theBody;
   bodyName = body->GetName();
//    mu = theBody->GetGravitationalConstant();
}


void PhysicalModel::SetForceOrigin(CelestialBody* toBody)
{
    forceOrigin = toBody;
}

//------------------------------------------------------------------------------
// bool PhysicalModel::Initialize()
//------------------------------------------------------------------------------
/**
 * Prepare the physical model for use
 *
 * This method allocates the state and deriv arrays, and can be overridden to 
 * perform other actions for the system setup.
 *
 * Note that deriv is allocated even if it is not used.  This feature may be 
 * PhysicalModelremoved in a later release.
 */
//------------------------------------------------------------------------------
bool PhysicalModel::Initialize()
{ 
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage(
            "PhysicalModel::Initialize() entered for %s; dimension = %d\n",
            typeName.c_str(), dimension);
   #endif
      
   if ((rawState != NULL) && (rawState != modelState))
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (rawState, "rawState", "PhysicalModel::Initialize()",
          "deleting rawState", this);
      #endif

      #ifdef DEBUG_STATE_ALLOCATION
         MessageInterface::ShowMessage("Deleting rawState at %p\n", rawState);
      #endif
      delete [] rawState;
      rawState = NULL;
   }
   
   if (modelState) 
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (modelState, "modelState", "PhysicalModel::Initialize()",
          "deleting modelState", this);
      #endif

      #ifdef DEBUG_STATE_ALLOCATION
         MessageInterface::ShowMessage("Deleting modelState (for %s) at %p\n",
               typeName.c_str(), modelState);
      #endif
      delete [] modelState;
      modelState = NULL;
      rawState = NULL;
      
      isInitialized = false;
   }
   
   if (deriv)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (deriv, "deriv", "PhysicalModel::Initialize()",
          "deleting deriv", this);
      #endif
      delete [] deriv;
      deriv = NULL;
   }

   // MessageInterface::ShowMessage("PMInitialize setting dim = %d\n", dimension);   
   #ifdef DEBUG_STATE_ALLOCATION
      MessageInterface::ShowMessage("Allocating modelState (for %s) at ",
            typeName.c_str());
   #endif
   
   modelState = new Real[dimension];

   #ifdef DEBUG_STATE_ALLOCATION
      MessageInterface::ShowMessage("%p\n", modelState);
   #endif

   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (modelState, "modelState", "PhysicalModel::Initialize()",
       "modelState = new Real[dimension]", this);
   #endif
   
   if (modelState != NULL)
   {
      deriv = new Real[dimension];

      for (Integer i = 0; i < dimension; ++i)
         deriv[i] = 0.0;

      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (deriv, "deriv", "PhysicalModel::Initialize()",
          "deriv = new Real[dimension]", this);
      #endif
      if (deriv)
         isInitialized = true;
      else
         isInitialized = false;
   }
   rawState = modelState;
   
   return isInitialized;
}

//------------------------------------------------------------------------------
//  bool SetBody(const std::string &theBody)
//------------------------------------------------------------------------------
/**
 * This method sets the body for this PhysicalModel object.
 *
 * @param <theBody> body name to use.
 *
 * @return flag indicating success of the operation.
 */
//------------------------------------------------------------------------------
bool PhysicalModel::SetBody(const std::string &theBody)
{
   bodyName = theBody;
   // initialize the body
   if (solarSystem == NULL) throw ODEModelException( // or just return false?
                                                       "Solar System undefined for Harmonic Field.");
   body = solarSystem->GetBody(bodyName);  // catch errors here?
   return true;
}

//------------------------------------------------------------------------------
// void SetBodyName(const std::string &name)
//------------------------------------------------------------------------------
/**
 *
 */
//------------------------------------------------------------------------------
void PhysicalModel::SetBodyName(const std::string &theBody)
{
   bodyName = theBody;
}

//------------------------------------------------------------------------------
// Real PhysicalModel::GetErrorThreshold(void) const
//------------------------------------------------------------------------------
/**
 * Returns the threshold for switching between relative and absolute error
 */
//------------------------------------------------------------------------------
Real PhysicalModel::GetErrorThreshold(void) const
{
   return relativeErrorThreshold;
}

//------------------------------------------------------------------------------
// bool PhysicalModel::SetErrorThreshold(const Real thold)
//------------------------------------------------------------------------------
/**
 * Sets the threshold for switching between relative and absolute error
 *
 * @param thold         The new threshold value
 */
//------------------------------------------------------------------------------
bool PhysicalModel::SetErrorThreshold(const Real thold)
{
   relativeErrorThreshold = (thold >= 0.0 ? thold : -thold);
   return true;
}

//------------------------------------------------------------------------------
// Integer PhysicalModel::GetDimension()
//------------------------------------------------------------------------------
/**
 * Accessor method used by Propagator class to determine # of vars
 * The Propagator class evolves the system being modeled by advancing some
 * number of variables.  The count of the variables must be coordinated 
 * between the propagator and the physical model of the system; 
 * GetDimension() is called by the Propagator class to obtain this 
 * information from the PhysicalModel class.
*/
//------------------------------------------------------------------------------
Integer PhysicalModel::GetDimension()
{
   return dimension;
}

//------------------------------------------------------------------------------
// void PhysicalModel::SetDimension(Integer n)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set # of vars
 * Use this method to reset the count of the variables modeled by the physical 
 * model of the system for models that allow for changes in this value.
*/
//------------------------------------------------------------------------------
void PhysicalModel::SetDimension(Integer n)
{
   dimension = n;
   isInitialized = false;
}

//------------------------------------------------------------------------------
// Real * PhysicalModel::GetState()
//------------------------------------------------------------------------------
/**
 * Accessor method used to access the state array
 * Use this method with care -- it exposes the internal array of state data to
 * external users.  The Propagator and Integrator classes can use this access to
 * make system evelotion more efficient, but at the cost of loss of 
 * encapsulation of the state data.
 */
//------------------------------------------------------------------------------
Real * PhysicalModel::GetState()
{
   return modelState;
//   return rawState;
}

//------------------------------------------------------------------------------
// Real * PhysicalModel::GetJ2KState()
//------------------------------------------------------------------------------
/**
 * Accessor method used to access the J2000 body based state array
 * Use this method with care -- it exposes the internal array of state data to
 * external users.  The Propagator and Integrator classes can use this access to
 * make system evolution more efficient, but at the cost of loss of
 * encapsulation of the state data.
 */
//------------------------------------------------------------------------------
Real * PhysicalModel::GetJ2KState()
{
   return rawState;
}

//------------------------------------------------------------------------------
// void PhysicalModel::SetState(const Real * st)
//------------------------------------------------------------------------------
/**
 * Used to set the elements of the state array
 *
 * @param st    Array of data containing the desired values for the state elements
 */
//------------------------------------------------------------------------------
void PhysicalModel::SetState(const Real * st)
{
   #ifdef PHYSICAL_MODEL_DEBUG_INIT
      MessageInterface::ShowMessage(
         "PhysicalModel::SetState() called for %s<%s>\n", 
         typeName.c_str(), instanceName.c_str());
   #endif
   if (modelState != NULL)
   {
      for (Integer i = 0; i < dimension; i++)
         modelState[i] = st[i];
      stateChanged = true;
   }
}


void PhysicalModel::SetState(GmatState * st)
{
   #ifdef PHYSICAL_MODEL_DEBUG_INIT
      MessageInterface::ShowMessage(
         "PhysicalModel::SetState(GmatState) called for %s<%s>\n",
         typeName.c_str(), instanceName.c_str());
      MessageInterface::ShowMessage("   and epoch = %le\n", st->GetEpoch());
   #endif
   theState = st;
   epoch    = st->GetEpoch(); // ***
   if (dimension != st->GetSize())
      MessageInterface::ShowMessage("Dimension mismatch!!!\n");
   if (modelState != NULL)
      SetState(st->GetState());
}


//------------------------------------------------------------------------------
// const Real* PhysicalModel::GetDerivativeArray(void)
//------------------------------------------------------------------------------
/**
 * Accessor for the derivative array
 * This method returns a pointer to the derivative array.  The Predictor-
 * Correctors need this access in order to extrapolate the next state.
 */
//------------------------------------------------------------------------------
const Real* PhysicalModel::GetDerivativeArray()
{
   return deriv;
}

//------------------------------------------------------------------------------
// void PhysicalModel::IncrementTime(Real dt)
//------------------------------------------------------------------------------
/**
 * Used to increment the internal time counter
 *
 * @param dt    Amount of time to increment by (usually in seconds)
 */
//------------------------------------------------------------------------------
void PhysicalModel::IncrementTime(Real dt)
{
   #ifdef DEBUG_TIME_INCREMENT
      MessageInterface::ShowMessage("Increment time called; prevElapsedTime = "
            "%.12lf, elapsed time = %.12lf  ==> ", prevElapsedTime,
            elapsedTime);
   #endif
   prevElapsedTime = elapsedTime;
   elapsedTime += dt;
   stateChanged = true;
   #ifdef DEBUG_TIME_INCREMENT
      MessageInterface::ShowMessage("prevElapsedTime = %.12lf, elapsed time = "
            "%.12lf\n", prevElapsedTime, elapsedTime);
   #endif
}

//------------------------------------------------------------------------------
// Real PhysicalModel::GetTime()
//------------------------------------------------------------------------------
/**
 * Read accessor for the time elapsed 
 * Use this method to track the elapsed time for the model.  You can set the 
 * system to start from a non-zero time by setting the value for the elapsedTime
 * parameter to the desired start value.  See the SetTime parameter for the  
 * write accessor.
 */
//------------------------------------------------------------------------------
Real PhysicalModel::GetTime()
{
   return elapsedTime;
}

//------------------------------------------------------------------------------
// void PhysicalModel::SetTime(Real t)
//------------------------------------------------------------------------------
/**
 * Write accessor for the total time elapsed 
 * Use this method to set time for the model
 */
//------------------------------------------------------------------------------
void PhysicalModel::SetTime(Real t)
{
   elapsedTime = t;
}


//------------------------------------------------------------------------------
// bool PhysicalModel::GetDerivatives(Real * state, Real dt, Integer order,
//                                    const Integer id)
//------------------------------------------------------------------------------
/**
 * Method invoked to calculate derivatives
 * This method is invoked to fill the deriv array with derivative information 
 * for the system that is being integrated.  It uses the state and elapsedTime 
 * parameters, along with the time interval dt passed in as a parameter, to 
 * calculate the derivative information at time \f$t=t_0+t_{elapsed}+dt\f$.
 *
 * @param dt            Additional time increment for the derivatitive 
 *                      calculation; defaults to 0.
 * @param state         Pointer to the current state data.  This can differ
 *                      from the PhysicalModel state if the subscribing
 *                              integrator samples other state values during 
 *                              propagation.  (For example, the Runge-Kutta integrators 
 *                      do this during the stage calculations.)
 * @param order         The order of the derivative to be taken (first 
 *                              derivative, second derivative, etc)
 * @param id            ID for the type of derivative requested for models that
 *                      support more than one type.  Default value of -1
 *                      indicates that the default derivative model is
 *                      requested.  This number should be a StateElementId.
 *
 * @return              true if the call succeeds, false on failure.  This
 *                      default implementation always returns false.
 */
//------------------------------------------------------------------------------
bool PhysicalModel::GetDerivatives(Real * state, Real dt, Integer order,
      const Integer id)
{
   return false;
}

//------------------------------------------------------------------------------
// Rvector6 GetDerivativesForSpacecraft(Spacecraft *sc)
//------------------------------------------------------------------------------
/**
 * Retrieves the derivative vector for a Spacecraft acted on by the model.
 *
 * @param sc The Spacecraft that is located at the time and place of the
 *           derivative calculation
 *
 * @return The derivative vector for the spacecraft's state
 */
//------------------------------------------------------------------------------
Rvector6 PhysicalModel::GetDerivativesForSpacecraft(Spacecraft *sc)
{
   Rvector6 retval;

   throw ODEModelException("GetDerivativesForSpacecraft not implemented "
         "for the " + typeName + " physical model.");

   return retval;
}

//------------------------------------------------------------------------------
// Real PhysicalModel::EstimateError(Real * diffs, Real * answer) const
//------------------------------------------------------------------------------
/**
 * Interface used to estimate the error in the current step
 *
 * The method calculates the largest local estimate of the error from the 
 * integration given the components of the differences calculated by the 
 * integrator.  It returns the largest error estimate found.  
 *  
 * The default implementation returns the largest single relative component 
 * found based on the input arrays.  In other words, the implementation provided 
 * here returns the largest component of the following vector:
 *
 * \f[\vec \epsilon = |{{{EE}_n}\over{x_n^f - x_n^i}}|\f]
 *
 * subject to the discussion of the relativeErrorThreshold parameter, below.
 *  
 * There are several alternatives that users of this class can implement: the 
 * error could be calculated based on the largest error in the individual 
 * components of the state vector, as the magnitude of the state vector (that 
 * is, the L2 (rss) norm of the error estimate vector).  The estimated error 
 * should never be negative, so a return value less than 0.0 can be used to 
 * indicate an error condition.
 *
 * One item to note in this implementation is the relativeErrorThreshold local 
 * variable.  This parameter looks at the difference between the initial state 
 * of the variables and the state after the integration.  If that difference is 
 * smaller in magnitude than the value of relativeErrorThreshold, then the error 
 * value calculated is the absolute error; if it is larger, the calculated value 
 * is scaled by the difference.  In other words, given
 *
 * \f[\Delta^i = |r^i(t + \delta t) - r^i(t)|\f]
 *
 * this method will return the largest error in the final states if each 
 * component of \f$\Delta^i\f$ is smaller than relativeErrorThreshold, and will 
 * return the largest value of the error divided by the corresponding 
 * \f$\Delta^i\f$ if each component of \f$\Delta^i\f$ is larger than 
 * relativeErrorThreshold.  This property lets the integrators step over small
 * discontinuities (for example, shadow crossings for spacecraft orbital models) 
 * without hanging.
 *    
 * @param diffs         Array of differences calculated by the integrator.  This array 
 *                      must be the same size as the state vector
 * @param answer        Candidate new state from the integrator
 *
 * @returns             The maximum calculated error
 */
//------------------------------------------------------------------------------
Real PhysicalModel::EstimateError(Real * diffs, Real * answer) const
{
   Real retval = 0.0, err, delta;

   for (Integer i = 0; i < dimension; ++i)
   {
      delta = answer[i] - modelState[i];
      if (delta > relativeErrorThreshold)
         err = fabs(diffs[i] / delta);
      else
         err = fabs(diffs[i]);
      if (err > retval)
         retval = err;
   }

   return retval;
}

//------------------------------------------------------------------------------
// bool GetComponentMap(Integer * map, Integer order, Integer id) const
//------------------------------------------------------------------------------
/**
 * Used to get the mapping in the state variable between components 
 * This method is used to obtain a mapping between the elements of the state
 * vector.  It is used, for instance, to map the position components to the
 * velocity components for a spacecraft state vector so that the 
 * Runge-Kutta-Nystrom integrators can obtain the velocity information they 
 * need.  The default implementation simply returns false.
 *
 * When the model for the class can provide a map for the data elements, it will
 * fill in the array of elements with either a "no map" indicator of -1, or the 
 * mapping between the selected element and its corresponding derivative.  These
 * data are placed into the input "map" array, which must be an integer array
 * sized to match the dimension of the model.  The user also specifies the order
 * of the mapping; for instance, to obtain the mapping for first derivative 
 * information, the order is set to 1.
 *
 * An example of the return data is in order for this method.  Suppose that the 
 * state vector consists of six elements, (X, Y, Z, Vx, Vy, Vz).  A user can 
 * request the mapping for the first derivative components by calling 
 * GetComponentMap(map, 1).  The array, map, that is returned will contain these
 * data: (3, 4, 5, -1, -1, -1).
 *
 *  @param map          Array that will contain the mapping of the elements
 * @param order        The order for the mapping (1 maps 1st derivatives to
 *                     their base components, 2 maps 2nd derivatives, and so on)
 * @param id           Identifier for the particular set of derivatives
 *                     requested.  The default, -1, returns the complete map.
 *
 * @return             Returns true if a mapping was made, false otherwise.  A
 *                     false return value can be used to indicate that the
 *                     requested map is not available, and therefore that the
 *                     model may not be appropriate for the requested
 *                     operations.
 * 
 * @todo This method needs serious rework for the formation pieces in build 3.
 */
//------------------------------------------------------------------------------
bool PhysicalModel::GetComponentMap(Integer * map, Integer order,
      Integer id) const
{
   //    return false;
   int i6;

   if (order == 1) 
   {
      // Calculate how many spacecraft are in the model
      int satCount = (int)(dimension / 6);

      // Ensure we don't overrun the buffer (this is paranoia, since integer
      // division effectively rounds down, but perhaps a healthy paranoia)
      while (satCount * 6 > dimension)
         --satCount;

      for (int i = 0; i < satCount; ++i)
      {
         i6 = i * 6;
    
         map[ i6 ] = i6 + 3;
         map[i6+1] = i6 + 4;
         map[i6+2] = i6 + 5;
         map[i6+3] = -1;
         map[i6+4] = -1;
         map[i6+5] = -1;
      }
   }
    
   return true;

}

//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system pointer
 * 
 * @param ss Pointer to the solar system used in the modeling.
 */
//------------------------------------------------------------------------------
void PhysicalModel::SetSolarSystem(SolarSystem *ss)
{
   solarSystem = ss;
}

//------------------------------------------------------------------------------
// void PhysicalModel::SetSatelliteParameter(const Integer i, 
//                                           const std::string parmName, 
//                                           const Real parm)
//------------------------------------------------------------------------------
/**
 * Passes spacecraft parameters to the force model.
 * 
 * This default implementation just returns.
 * 
 * @param i ID for the spacecraft
 * @param parmName name of the Spacecraft parameter 
 * @param parm Parameter value
 */
//------------------------------------------------------------------------------
void PhysicalModel::SetSatelliteParameter(const Integer i, 
                                          const std::string parmName, 
                                          const Real parm,
                                          const Integer parmID)
{
}

void PhysicalModel::SetSatelliteParameter(const Integer i,
                                          Integer parmID,
                                          const Real parm)
{
}

//------------------------------------------------------------------------------
// void PhysicalModel::SetSatelliteParameter(const Integer i, 
//                                           const std::string parmName, 
//                                           const std::string parm)
//------------------------------------------------------------------------------
/**
 * Passes spacecraft parameters to the force model.
 * 
 * This default implementation just returns.
 * 
 * @param i ID for the spacecraft
 * @param parmName name of the Spacecraft parameter 
 * @param parm Parameter value
 */
//------------------------------------------------------------------------------
void PhysicalModel::SetSatelliteParameter(const Integer i, 
                                          const std::string parmName, 
                                          const std::string parm)
{
}


//------------------------------------------------------------------------------
// bool UsesSpacecraftOrigin()
//------------------------------------------------------------------------------
/**
 * Test to model to see if the Spacecraft origin is needed
 *
 * Identifies models that depend on the origin of the Spacecraft rather than the
 * force model origin.
 *
 * @return true if the model is spacecraft origin dependent, false if not
 */
//------------------------------------------------------------------------------
bool PhysicalModel::UsesSpacecraftOrigin()
{
   return false;
}


//------------------------------------------------------------------------------
// void PhysicalModel::SetSatelliteParameter(const Integer i,
//                                           const std::string parmName,
//                                           const std::string parm)
//------------------------------------------------------------------------------
/**
 * Resets the PhysicalModel to receive a new set of satellite parameters.
 *
 * This default implementation just returns.
 *
 * @param parmName name of the Spacecraft parameter.  The empty string clear all
 *                 of the satellite parameters for the PhysicalModel.
 */
//------------------------------------------------------------------------------
void PhysicalModel::ClearSatelliteParameters(const std::string parmName)
{
}


//------------------------------------------------------------------------------
// bool PhysicalModel::StateChanged(bool reset)
//------------------------------------------------------------------------------
// DJC: Probably should document this a bit better -- want me to do it? :
bool PhysicalModel::StateChanged(bool reset)
{
   bool retval = stateChanged;
   if (reset)
      stateChanged = false;
   return retval;
}


//------------------------------------------------------------------------------
// bool IsTransient()
//------------------------------------------------------------------------------
/**
 * Specifies whether the PhysicalModel is transient or always applied.
 * 
 * @return true if transient, false (the default) if not.
 */
//------------------------------------------------------------------------------
bool PhysicalModel::IsTransient()
{
   return false;
}


//------------------------------------------------------------------------------
// bool DepletesMass()
//------------------------------------------------------------------------------
/**
 * Detects mass depletion from a PhysicalModel
 *
 * @return true if the model depletes mass, false if it does not
 */
//------------------------------------------------------------------------------
bool PhysicalModel::DepletesMass()
{
   return false;
}


//------------------------------------------------------------------------------
// bool IsUserForce()
//------------------------------------------------------------------------------
/**
 * Specifies if a force is set by a user module.
 * 
 * Specifies whether the PhysicalModel is an "extra" force added by a plug-in or 
 * other user method.  Forces added to the ODEModel this way appear in the 
 * "UserDefined" field of the force model when it is written out or parsed.
 * 
 * @return true if the force should be in the "UserDefined" field.
 */
//------------------------------------------------------------------------------
bool PhysicalModel::IsUserForce()
{
   return false;
}


//------------------------------------------------------------------------------
// bool IsUnique(const std::string &forBody)
//------------------------------------------------------------------------------
/**
 * Specifies if a force is the one and only force, optionally at a given body.
 *
 * @param forBody The body at which the force must be unique, if body specific
 *
 * @return true if the force should be unique in the ODEModel.
 */
//------------------------------------------------------------------------------
bool PhysicalModel::IsUnique(const std::string &forBody)
{
   return false;
}


//------------------------------------------------------------------------------
// void SetPropList(ObjectArray *soList)
//------------------------------------------------------------------------------
/**
 * Sets the list of propagated space objects for transient forces.
 *
 * @param soList List of propagated space objects
 *
 * @note This default version does nothing.
 */
//------------------------------------------------------------------------------
void PhysicalModel::SetPropList(ObjectArray *soList)
{
}


//------------------------------------------------------------------------------
// bool CheckQualifier(const std::string &qualifier, const std::string &forType)
//------------------------------------------------------------------------------
/**
 * Ensures that the string qualifier applies to this model
 *
 * This method was added so that qualified force model settings can validate
 * that the force receiving a setting is correct.  It is used, for example, with
 * the full field gravity model strings of the form
 *
 *    Forces.GravityField.Earth.Order = 8;
 *
 * to ensure that the model is actually Earth based.
 *
 * @param qualifier The string qualifier
 * @param forType String identifying owned object type, if needed
 *
 * @return true if the qualifier matches the model, false if not
 */
//------------------------------------------------------------------------------
bool PhysicalModel::CheckQualifier(const std::string &qualifier,
      const std::string &forType)
{
   return true;
}


//------------------------------------------------------------------------------
// bool SupportsDerivative(Gmat::StateElementId id)
//------------------------------------------------------------------------------
/**
 * Identifies which derivatives are supported.
 *
 * Function used to determine if the physical model supports derivative
 * information for a specified type.  By default, physical models support
 * derivatives of the Cartesian state but no other components.  Override this
 * method to add support for other elements (e.g. mass depletion, the A-matrix,
 * the state transition matrix, and so forth).
 *
 * @param id State Element ID for the derivative type
 *
 * @return true if the type is supported, false otherwise.
 */
//------------------------------------------------------------------------------
bool PhysicalModel::SupportsDerivative(Gmat::StateElementId id)
{
   if (id == Gmat::CARTESIAN_STATE)
      return true;

   return false;
}

//------------------------------------------------------------------------------
// bool SetStart(Gmat::StateElementId id, Integer index, Integer quantity)
//------------------------------------------------------------------------------
/**
 * Sets indices for supported derivatives in the propagation state vector.
 *
 * Function used to set the start point and size information for the state
 * vector, so that the derivative information can be placed in the correct place
 * in the derivative vector.
 *
 * @param id State Element ID for the derivative type
 * @param index Starting index in the state vector for this type of derivative
 * @param quantity Number of objects that supply this type of data
 *
 * @return true if the type is supported, false otherwise.
 */
//------------------------------------------------------------------------------
bool PhysicalModel::SetStart(Gmat::StateElementId id, Integer index,
      Integer quantity)
{
   return false;
}

//---------------------------------
// inherited methods from GmatBase
//---------------------------------

//------------------------------------------------------------------------------
// std::string PhysicalModel::GetParameterText(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string PhysicalModel::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < PhysicalModelParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer PhysicalModel::GetParameterID(const std::string str)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer PhysicalModel::GetParameterID(const std::string &str) const
{
   for (int i = GmatBaseParamCount; i < PhysicalModelParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   return GmatBase::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType PhysicalModel::GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType PhysicalModel::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < PhysicalModelParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string PhysicalModel::GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string PhysicalModel::GetParameterTypeString(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < PhysicalModelParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return GmatBase::GetParameterTypeString(id);
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
bool PhysicalModel::IsParameterReadOnly(const Integer id) const
{
   if ((id == EPOCH)     || (id == ELAPSED_SECS) ||
       (id == BODY_NAME) || (id == DERIVATIVE_ID))
      return true;

   return GmatBase::IsParameterReadOnly(id);
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param label Text label for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool PhysicalModel::IsParameterReadOnly(const std::string &label) const
{
   if ((label == PARAMETER_TEXT[EPOCH - GmatBaseParamCount]) || 
       (label == PARAMETER_TEXT[ELAPSED_SECS - GmatBaseParamCount]) || 
       (label == PARAMETER_TEXT[BODY_NAME - GmatBaseParamCount]) ||
       (label == PARAMETER_TEXT[DERIVATIVE_ID - GmatBaseParamCount]))
      return true;

   return GmatBase::IsParameterReadOnly(label);
}

//------------------------------------------------------------------------------
// Real PhysicalModel::GetRealParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real PhysicalModel::GetRealParameter(const Integer id) const
{
   if (id == EPOCH)
      return epoch + elapsedTime / GmatTimeConstants::SECS_PER_DAY;
   if (id == ELAPSED_SECS)
      return elapsedTime;

   return GmatBase::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real PhysicalModel::SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real PhysicalModel::SetRealParameter(const Integer id, const Real value)
{
   if (id == ELAPSED_SECS) 
   {
      elapsedTime = value;
      return elapsedTime;
   }
   if (id == EPOCH) 
   {
      epoch = value;
      elapsedTime = 0.0;
      return epoch;
   }
   return GmatBase::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    <id>  Integer ID for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
std::string PhysicalModel::GetStringParameter(const Integer id) const
{
   if (id == BODY_NAME) return bodyName;
   return GmatBase::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    <id>    Integer ID for the parameter
 * @param    <value> The new value for the parameter
 */
//------------------------------------------------------------------------------
bool PhysicalModel::SetStringParameter(const Integer id,
                                       const std::string &value)
{
   #ifdef DEBUG_PM_SET
   MessageInterface::ShowMessage
      ("PhysicalModel::SetStringParameter() entered, id=%d, value='%s'\n",
       id, value.c_str());
   #endif
   
   if (id == BODY_NAME)
   {
      if (!solarSystem)
      {
         SetBodyName(value);
         return true;
      }
      else
      {
         return SetBody(value);
      }
//      if (!solarSystem) throw ODEModelException(
//          "In PhysicalModel, cannot set body, as no solar system has been set");
//      if (value != bodyName)
//      {
//         body = solarSystem->GetBody(value);
//         if (body)
//         {
//            bodyName = body->GetName();
//            return true;
//         }
//         else      return false;
//      }
//      else return true;
   }
   return GmatBase::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    <label>  Integer ID for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
std::string PhysicalModel::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    <label> Integer ID for the parameter
 * @param    <value> The new value for the parameter
 */
//------------------------------------------------------------------------------
bool PhysicalModel::SetStringParameter(const std::string &label,
                                       const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const Gmat::ObjectType type,
//                         const std::string &name)
//------------------------------------------------------------------------------
/**
* This method returns a reference object from the ObjectReferencedAxes class.
 *
 * @param type  type of the reference object requested
 * @param name  name of the reference object requested
 *
 * @return pointer to the reference object requested.
 *
 */
//------------------------------------------------------------------------------
GmatBase* PhysicalModel::GetRefObject(const Gmat::ObjectType type,
                                      const std::string &name)
{
   switch (type)
   {
      case Gmat::SPACE_POINT:
      case Gmat::CELESTIAL_BODY:
         if ((body) && (name == bodyName))     return body;
         break;
      default:
            break;
   }
   
   // Not handled here -- invoke the next higher GetRefObject call
   return GmatBase::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Returns the names of the reference object. (Derived classes should implement
 * this as needed.)
 *
 * @param <type> reference object type.  Gmat::UnknownObject returns all of the
 *               ref objects.
 *
 * @return The names of the reference object.
 */
//------------------------------------------------------------------------------
const StringArray& PhysicalModel::GetRefObjectNameArray(
                                  const Gmat::ObjectType type)
{
   if (type == Gmat::UNKNOWN_OBJECT)
   {
      static StringArray refs;
      
         refs.push_back(bodyName);
      
         #ifdef DEBUG_REFERENCE_SETTING
            MessageInterface::ShowMessage("+++ReferenceObjects:\n");
            for (StringArray::iterator i = refs.begin(); i != refs.end(); ++i)
               MessageInterface::ShowMessage("   %s\n", i->c_str());
         #endif
      
      return refs;
   }
   
   // Not handled here -- invoke the next higher GetRefObject call
   return GmatBase::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
* This method sets a reference object for the ObjectReferencedAxes class.
 *
 * @param obj   pointer to the reference object
 * @param type  type of the reference object
 * @param name  name of the reference object
 *
 * @return true if successful; otherwise, false.
 *
 */
//------------------------------------------------------------------------------
bool PhysicalModel::SetRefObject(GmatBase *obj,
                                 const Gmat::ObjectType type,
                                 const std::string &name)
{
   if (obj->IsOfType("CelestialBody"))
   {
      if (name == bodyName)
      {
         #ifdef DEBUG_REFERENCE_SETTING
            MessageInterface::ShowMessage("Setting %s as primary for %s\n",
                                          name.c_str(), instanceName.c_str());
         #endif
         body = (CelestialBody*) obj;
      }
      return true;
   }
   
   // Not handled here -- invoke the next higher SetRefObject call
   return GmatBase::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//       const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Passes reference objects into the model
 *
 * @param obj The object
 * @param type The type of object being passed into the model
 * @param name The name of the object
 * @param index The object's index when being set for an array of objects
 *
 * @return true if a reference was set, false if not
 */
//------------------------------------------------------------------------------
bool PhysicalModel::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                           const std::string &name, const Integer index)
{
   return GmatBase::SetRefObject(obj, type, name, index);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Accesses a reference object
 *
 * @param type The type of object being passed into the model
 * @param name The name of the object
 * @param index The object's index when being set for an array of objects
 *
 * @return The object
 */
//------------------------------------------------------------------------------
GmatBase* PhysicalModel::GetRefObject(const Gmat::ObjectType type,
                           const std::string &name, const Integer index)
{
   return GmatBase::GetRefObject(type, name, index);
}


// Support for extra derivative calcs

//------------------------------------------------------------------------------
// const IntegerArray& GetSupportedDerivativeIds()
//------------------------------------------------------------------------------
/**
 * Accesses the types of derivatives supported by this PhysicalModel
 *
 * @return The IDs of the supported derivatives
 */
//------------------------------------------------------------------------------
const IntegerArray& PhysicalModel::GetSupportedDerivativeIds()
{
   return derivativeIds;
}


//------------------------------------------------------------------------------
// const StringArray&  GetSupportedDerivativeNames()
//------------------------------------------------------------------------------
/**
 * Accesses the types of derivatives supported by this PhysicalModel
 *
 * @return The names of the supported derivatives
 */
//------------------------------------------------------------------------------
const StringArray&  PhysicalModel::GetSupportedDerivativeNames()
{
   return derivativeNames;
}

//------------------------------------------------------------------------------
// bool BuildModelState(GmatEpoch now, Real* state, Real* j2kState,
//       Integer dimension)
//------------------------------------------------------------------------------
/**
 * Takes internal state vector and translates it to the PhysicalModel's origin
 *
 * @param now The epoch of the transformation
 * @param state The transformed state -- this is the output vector, in MJ2000Eq
 *              coordinates at the PhysicalModel's origin
 * @param j3kState The input state vector.  This should be a vector in the
 *                 j2kBody's MJ2000Eq coordinates
 * @param dimension The size of the state vector.  This allows multiple position
 *                  velocity sextuplets to be processed in a single call.  If
 *                  dimension is not a multiple of 6, an exception is thrown.
 *
 * @return true is teh transformation succeeded, false if it failed.
 */
//------------------------------------------------------------------------------
bool PhysicalModel::BuildModelState(GmatEpoch now, Real* state, Real* j2kState,
      Integer dimension)
{
   bool retval = false;

   Integer count = dimension / 6;
   if (count*6 != dimension)
      throw ODEModelException("Error translating states when building the "
            "model state");

   if (body != NULL)
   {
      Rvector6 bodyState = body->GetMJ2000State(now);
      for (Integer i = 0; i < count; ++i)
      {
         Integer i6 = i*6;
         for (Integer j = 0; j < 6; ++j)
            state[i6 + j] = j2kState[i6 + j] - bodyState[j];
      }
      retval = true;
   }

   return retval;
}

