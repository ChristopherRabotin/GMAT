//$Header$
//------------------------------------------------------------------------------
//                              ForceModel
//------------------------------------------------------------------------------
// *** File Name : ForceModel.cpp
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
//                           : 2/28/2003 - D. Conway, Thinking Systems, Inc.
//                             Filled in some code to model cases with 
//                             discontinuous forces (e.g. SRP)
//
//                           : 10/01/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//                                - Changed FlightDynamicsForces class to ForceModel class
//
//                           : 10/20/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - All double types to Real types
//                                - All primitive int types to Integer types
//
//                           : 11/9/2003 - D. Conway, Thinking Systems, Inc.
//                             Overrode GetParameterCount so the count 
//                             increases based on the member forces
// **************************************************************************

#include "ForceModel.hpp"
#include <string.h> // waw: added 03/10/04
#include "PointMassForce.hpp"
#include "MessageInterface.hpp"

#define DEBUG_FORCEMODEL 0
#define normType -2

//---------------------------------
// static data
//---------------------------------

// DJC: 06/16/04 Updated for scripting
//const std::string
//ForceModel::PARAMETER_TEXT[ForceModelParamCount - PhysicalModelParamCount] =
//{
//    "PointMass",
//    "FullField",
//    "Drag",        //loj: 3/19/04 This is also in PropSetup. Where do we want to handle?
//    "MagField",
//    "ForceList",
//};
const std::string
ForceModel::PARAMETER_TEXT[ForceModelParamCount - PhysicalModelParamCount] =
{
    "CentralBody",
    "PrimaryBodies",
    "PointMasses",
    "Drag",
    "SRP"
};


// DJC: 06/16/04 Updated for scripting
//const Gmat::ParameterType
//ForceModel::PARAMETER_TYPE[ForceModelParamCount - PhysicalModelParamCount] =
//{
//    Gmat::STRING_TYPE,
//    Gmat::STRING_TYPE,
//    Gmat::STRING_TYPE,
//    Gmat::STRING_TYPE,
//    Gmat::STRINGARRAY_TYPE,
//};
const Gmat::ParameterType
ForceModel::PARAMETER_TYPE[ForceModelParamCount - PhysicalModelParamCount] =
{
    Gmat::STRING_TYPE,
    Gmat::STRINGARRAY_TYPE,
    Gmat::STRINGARRAY_TYPE,
    Gmat::STRING_TYPE,
    Gmat::STRING_TYPE
};


std::map<std::string, std::string> ForceModel::scriptAliases;

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// ForceModel::ForceModel(void)
//------------------------------------------------------------------------------
/**
 * The constructor
 */
//------------------------------------------------------------------------------
//ForceModel::ForceModel(void) :
//PhysicalModel     (NULL, NULL, NULL),
//derivatives       (NULL),
//estimationMethod  (2.0)
//{
//    dimension = 6;
//}

//------------------------------------------------------------------------------
// ForceModel::ForceModel(Gmat::ObjectType id, const std::string &typeStr,
//                        const std::string &nomme))
//------------------------------------------------------------------------------
/**
 * The constructor
 */
//------------------------------------------------------------------------------
ForceModel::ForceModel(const std::string &nomme) :
PhysicalModel     (Gmat::FORCE_MODEL, "ForceModel", nomme),
//    derivatives       (NULL),  waw:  06/03/04
//    forceCount        (0),  waw: 05/06/04
previousState     (NULL),
estimationMethod  (2.0)
{
    numForces = 0;
    dimension = 6;
    currentForce = 0;  // waw: added 06/04/04
    parameterCount = ForceModelParamCount;
}

//------------------------------------------------------------------------------
// ForceModel::~ForceModel(void)
//------------------------------------------------------------------------------
/**
 * The destructor
 * The destructor deletes the list of PhysicalModel instances.
 */
//------------------------------------------------------------------------------
ForceModel::~ForceModel(void)
{
//  waw: 06/03/04
//    if (derivatives)
//        delete derivatives;
    if (previousState)
        delete [] previousState;
}

//------------------------------------------------------------------------------
// ForceModel::ForceModel(const ForceModel& fdf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * NOTE: The ForceModel copy constructor is not yet implemented.  This
 * method should be completed before the class is used in external code.
 *
 * @param fdf   The original of the ForceModel that are copied
 */
//------------------------------------------------------------------------------
ForceModel::ForceModel(const ForceModel& fdf) :
    PhysicalModel    (fdf)
{
   /// @todo Implement the copy constructor
}

//------------------------------------------------------------------------------
// ForceModel& ForceModel::operator=(const ForceModel& fdf)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 * 
 * NOTE: The ForceModel assignment operator is not yet implemented.  
 * This method should be completed before the class is used in external code.
 * 
 * @param fdf   The original of the ForceModel that are copied
 */
//------------------------------------------------------------------------------
ForceModel& ForceModel::operator=(const ForceModel& fdf)
{
   if (&fdf == this)
        return *this;

   /// @todo Implement the assignment operator
   return *this;
}

//------------------------------------------------------------------------------
// void ForceModel::AddForce(PhysicalModel *pPhysicalModel)
//------------------------------------------------------------------------------
/**
 * Method used to add a new force to the force model
 *
 * This method takes the pointer to the new force and adds it to the force model
 * list for later use.  Each force should supply first derivative information 
 * for elements 4 through 6 of a state vector, and zeros for the first three 
 * elements.  The forces should have the ability to act on state vectors for 
 * formations as well, by filling in elements (6*n+4) through (6*n+6) for larger
 * state vectors.
 * 
 * The force that is passed in is owned by this class (actually, by the member 
 * DerivativeList), and should not be destructed externally.  In addition, every 
 * force that is passed to this class needs to have a copy constructor and an 
 * assignment operator so that it can be cloned for distribution to multiple
 * satellites.
 * 
 * @param pPhysicalModel        The force that is being added to the force model
 */
//------------------------------------------------------------------------------
void ForceModel::AddForce(PhysicalModel *pPhysicalModel)
{
    //MessageInterface::ShowMessage("ForceModel::AddForce() entered\n");
    
    if (pPhysicalModel == NULL)
        return;
// waw: 06/03/04
//    if (derivatives == NULL)
//        derivatives = new DerivativeList;

    pPhysicalModel->SetDimension(dimension);
// waw: 06/03/04
//    derivatives->AddForce(pPhysicalModel);
    initialized = false;

    forceList.push_back(pPhysicalModel);
    numForces = forceList.size();
    
    //++forceCount;  waw: 05/06/04
}

//------------------------------------------------------------------------------
// void DeleteForce(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Deletes force from the force model.
 * @param    name   The force name to delete
 */
//------------------------------------------------------------------------------
void ForceModel::DeleteForce(const std::string &name)
{
    std::vector<PhysicalModel *>::iterator force;
    Integer i = 0;
    for (force = forceList.begin(); force != forceList.end(); force++) 
    {
        PhysicalModel *pm = forceList[i];
        std::string pmName = pm->GetName().c_str();
        
        if ( strcmp(name.c_str(), pmName.c_str()) == 0 )
        {
            forceList.erase(force);
            //delete pm;
            numForces = forceList.size();
            return;
        }
        i++;
    }
}

//------------------------------------------------------------------------------
// bool HasForce(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Search force in the force model.
 * @param  name The force name to look up
 * @return true if force exists, else false
 */
//------------------------------------------------------------------------------
bool ForceModel::HasForce(const std::string &name)
{
    std::vector<PhysicalModel *>::iterator force;
    Integer i = 0;
    for (force = forceList.begin(); force != forceList.end(); force++) 
    {
        PhysicalModel *pm = forceList[i];
        std::string pmName = pm->GetName().c_str();
        
        if ( strcmp(name.c_str(), pmName.c_str()) == 0 )
        {
            return true;
        }
        i++;
    }
    return false; 
}

//------------------------------------------------------------------------------
// Integer GetNumForces()
//------------------------------------------------------------------------------
Integer ForceModel::GetNumForces()
{
    return numForces;
}

//------------------------------------------------------------------------------
// StringArray& GetForceTypeNames()
//------------------------------------------------------------------------------
StringArray& ForceModel::GetForceTypeNames()
{
    forceTypeNames.clear();

    for (int i=0; i<numForces; i++)
    {
        forceTypeNames.push_back(forceList[i]->GetTypeName());
    }
        
    return forceTypeNames;
}

//------------------------------------------------------------------------------
// std::string GetForceTypeName(Integer index)
//------------------------------------------------------------------------------
std::string ForceModel::GetForceTypeName(Integer index)
{
    StringArray typeList = GetForceTypeNames();
    
    if (index >= 0 && index < numForces)
    {
        return typeList[index];
    }

    return "UNDEFINED_FORCE_TYPE";
}

//------------------------------------------------------------------------------
// void ClearSpacecraft()
//------------------------------------------------------------------------------
void ForceModel::ClearSpacecraft()
{
    spacecraft.clear();
}

//------------------------------------------------------------------------------
// PhysicalModel* GetForce(Integer index)
//------------------------------------------------------------------------------
PhysicalModel* ForceModel::GetForce(Integer index)
{
    if (index >= 0 && index < numForces)
    {
        return forceList[index];
    }
    
    return NULL;
}


//------------------------------------------------------------------------------
// bool ForceModel::AddSpacecraft(Spacecraft *sc)
//------------------------------------------------------------------------------
/**
 * Sets spacecraft that use this force model.
 *
 * @param sc The spacecraft
 *
 * @return true is the spacecraft is added to the list, false if it was already
 *         in the list, or if it is NULL.
 */
//------------------------------------------------------------------------------
bool ForceModel::AddSpacecraft(Spacecraft *sc)
{
    if (sc == NULL)
        return false;
    if (find(spacecraft.begin(), spacecraft.end(), sc) != spacecraft.end())
        return false;
    spacecraft.push_back(sc);

    // Quick fix for the epoch update
    epoch = sc->GetRealParameter(sc->GetParameterID("Epoch"));
    return true;
}


//------------------------------------------------------------------------------
// void ForceModel::UpdateSpacecraft(void)
//------------------------------------------------------------------------------
/**
 * Updates the state data for the spacecraft that use this force model.
 */
//------------------------------------------------------------------------------
void ForceModel::UpdateSpacecraft(Real newEpoch)
{
   if (spacecraft.size() > 0) {
      Integer j = 0;
      Integer stateSize = 6;
      Integer vectorSize = stateSize * sizeof(Real);
      std::vector<Spacecraft *>::iterator sat;
      Real *state;

      for (sat = spacecraft.begin(); sat != spacecraft.end(); ++sat) {
         state = (*sat)->GetState();
         memcpy(&previousState[j*stateSize], state, vectorSize);
         previousTime = 
            ((*sat)->GetRealParameter((*sat)->GetParameterID("Epoch")) - epoch)
            * 86400.0;
         memcpy(state, &modelState[j*stateSize], vectorSize);
         ++j;
            
         // Quick fix to get the epoch updated
         Real newepoch = epoch + elapsedTime / 86400.0;      

         //loj: 6/16/04 uncommented lines setting newEpoch.
         // for consecutive Propagate command, spacecraft epoch doesn't get updated
         
         // Update the epoch if it was passed in
         if (newEpoch != -1.0)
            newepoch = newEpoch;
         
         (*sat)->SetRealParameter((*sat)->GetParameterID("Epoch"), newepoch);
#if DEBUG_FORCEMODEL
         MessageInterface::ShowMessage
            ("ForceModel::UpdateSpacecraft() newepoch=%f\n", newepoch);
#endif
      }
   }
}


//------------------------------------------------------------------------------
// void ForceModel::UpdateSpacecraft(void)
//------------------------------------------------------------------------------
/**
 * Updates the model state data from the spacecraft state -- useful to revert
 * to a previous step.
 *
 * @note This method will need to be updated when the multi-step integrators are
 *       folded into the code
 */
//------------------------------------------------------------------------------
void ForceModel::UpdateFromSpacecraft(void)
{
    if (spacecraft.size() > 0) 
    {
        Integer j = 0;
        Integer stateSize = 6;
        std::vector<Spacecraft *>::iterator sat;
        Real *state;
        for (sat = spacecraft.begin(); sat != spacecraft.end(); ++sat) 
        {
            state = (*sat)->GetState();
            memcpy(&modelState[j*stateSize], state, stateSize * sizeof(Real));
            ++j;
        }
    }
}


//------------------------------------------------------------------------------
// void ForceModel::RevertSpacecraft(void)
//------------------------------------------------------------------------------
/**
 * Resets the model state data from the previous spacecraft state.
 *
 * @note This method will need to be updated when the multi-step integrators are
 *       folded into the code
 */
//------------------------------------------------------------------------------
void ForceModel::RevertSpacecraft(void)
{
   elapsedTime = previousTime;
   memcpy(modelState, previousState, dimension*sizeof(Real)); 
}


//------------------------------------------------------------------------------
// bool ForceModel::Initialize(void)
//------------------------------------------------------------------------------
/**
 * Initializes model and all contained models
 */
//------------------------------------------------------------------------------
bool ForceModel::Initialize(void)
{
    Integer stateSize = 6;      // Will change if we integrate more variables
    Integer satCount = 1;
    std::vector<Spacecraft *>::iterator sat;

    if (spacecraft.size() > 0)
        satCount = spacecraft.size();
    
    dimension = stateSize * satCount;
    if (!PhysicalModel::Initialize())
        return false;
    previousState = new Real[dimension];

    if (spacecraft.size() == 0) 
    {
        modelState[0] = 7000.0;
        modelState[1] =    0.0;
        modelState[2] = 1000.0;
        modelState[3] =    0.0;
        modelState[4] =    7.4;
        modelState[5] =    0.0;
    }
    else 
    {
        Integer j = 0;
        Real *state;
        for (sat = spacecraft.begin(); sat != spacecraft.end(); ++sat) 
        {
            state = (*sat)->GetState();
            memcpy(&modelState[j*stateSize], state, stateSize * sizeof(Real));
            ++j;
        }
    }
    
    previousTime = 0.0;
    memcpy(previousState, modelState, dimension*sizeof(Real));

//    DerivativeList *current = derivatives;  waw: 06/03/04
    Integer cf = currentForce;
    PhysicalModel *current = GetForce(cf);  // waw: added 06/04/04
    PhysicalModel *currentPm;

    // Variables used to set spacecraft parameters
    std::string parmName, stringParm;
    Real parm;
    Integer id, i;

    while (current) 
    {
//        currentPm = current->GetDerivative();  waw: 06/03/04
        currentPm = current;  // waw: added 06/04/04 
        currentPm->SetDimension(dimension);
        currentPm->SetSolarSystem(solarSystem);
        if (!currentPm->Initialize()) 
        {
           std::string msg = "Component force ";
           msg += currentPm->GetTypeName();
           msg += " failed to initialize";
           throw ForceModelException(msg.c_str());
        }
        currentPm->SetState(modelState);
        
        // Set spacecraft parameters for forces that need them
        i = 0;
        for (sat = spacecraft.begin(); sat != spacecraft.end(); ++sat) 
        {
            parmName = "Epoch";
            id = (*sat)->GetParameterID(parmName);
            parm = (*sat)->GetRealParameter(id);
            id = currentPm->GetParameterID(parmName);
            currentPm->SetRealParameter(id, parm);
        
            parmName = "ReferenceBody";
            id = (*sat)->GetParameterID(parmName);
            stringParm = (*sat)->GetStringParameter(id);
            currentPm->SetSatelliteParameter(i, parmName, stringParm);
        
            parmName = "DryMass";
            id = (*sat)->GetParameterID(parmName);
            parm = (*sat)->GetRealParameter(id);
            currentPm->SetSatelliteParameter(i, parmName, parm);
        
            parmName = "CoefficientDrag";
            id = (*sat)->GetParameterID(parmName);
            parm = (*sat)->GetRealParameter(id);
            currentPm->SetSatelliteParameter(i, parmName, parm);
            
            parmName = "DragArea";
            id = (*sat)->GetParameterID(parmName);
            parm = (*sat)->GetRealParameter(id);
            currentPm->SetSatelliteParameter(i, parmName, parm);
            
            parmName = "ReflectivityCoefficient";
            id = (*sat)->GetParameterID(parmName);
            parm = (*sat)->GetRealParameter(id);
            currentPm->SetSatelliteParameter(i, parmName, parm);
            
            ++i;
        }
        // current = current->Next(); waw: 06/04/04
        // waw: added 06/04/04
        cf++;
        current = GetForce(cf);
    }

    return true;
}

//------------------------------------------------------------------------------
// void ForceModel::IncrementTime(Real dt)
//------------------------------------------------------------------------------
void ForceModel::IncrementTime(Real dt)
{
    PhysicalModel::IncrementTime(dt);
// waw: 06/03/04
//    DerivativeList *current = derivatives;
//    while (current) 
//    {
//        current->GetDerivative()->IncrementTime(dt);
//        current = current->Next();
//    }
}

//------------------------------------------------------------------------------
// void ForceModel::SetTime(Real t)
//------------------------------------------------------------------------------
void ForceModel::SetTime(Real t)
{
    PhysicalModel::SetTime(t);
// waw: 06/03/04
//    DerivativeList *current = derivatives;
//
//    while (current) 
//    {
//        current->GetDerivative()->SetTime(t);
//        current = current->Next();
//    }
}

//------------------------------------------------------------------------------
// bool ForceModel::GetDerivatives(Real * state, Real dt, Integer order)
//------------------------------------------------------------------------------
/**
 * Returns the accumulated superposition of forces 
 * 
 * This method applies superposition of forces in order to calculate the total
 * acceleration applied to the state vector.
 * 
 * NOTE: GetDerivatives is not yet implemented.
 * 
 * @param    state   The current state vector
 * @param    dt      The current time interval from epoch
 * @param    order   Order of the derivative to be taken
 */
//------------------------------------------------------------------------------
bool ForceModel::GetDerivatives(Real * state, Real dt, Integer order)
{
    if (order > 2)
        return false;
    if (!initialized)
        return false;

    if (order == 1) //loj: changed from =
    {
        deriv[0] = state[3];
        deriv[1] = state[4];
        deriv[2] = state[5];
        deriv[3] = deriv[4] = deriv[5] = 0.0;
    }
    else 
    {
        deriv[0] = deriv[1] = deriv[2] = 
        deriv[3] = deriv[4] = deriv[5] = 0.0;
    }
// waw: 06/03/04
//    DerivativeList *current = derivatives;
    // waw: added 06/04/04
    Integer cf = currentForce;
    PhysicalModel *current = GetForce(cf);  

    const Real * ddt;
    while (current) 
    {
        // waw: 06/04/04
        //ddt = current->GetDerivative()->GetDerivativeArray();
        ddt = GetForce(cf)->GetDerivativeArray();
        //if (!current->GetDerivative()->GetDerivatives(state, dt, order)) 
        if (!current->GetDerivatives(state, dt, order))
            return false;

        if (order == 1) //loj: changed from =
        {
            deriv[3] += ddt[3];
            deriv[4] += ddt[4];
            deriv[5] += ddt[5];
        }
        else 
        {
            deriv[0] += ddt[0];
            deriv[1] += ddt[1];
            deriv[2] += ddt[2];
        }
        //current = current->Next(); waw: 06/04/04
        // waw: added 06/04/04
        cf++;
        current = GetForce(cf);
    }

    return true;
}

//------------------------------------------------------------------------------
// Real ForceModel::EstimateError(Real *diffs, Real *answer) const
//------------------------------------------------------------------------------
/**
 * Interface used to estimate the error in the current step
 * 
 * The method calculates the largest local estimate of the error from the 
 * integration given the components of the differences calculated bt the 
 * integrator.  It returns the largest error estimate found.  
 *   
 * The default implementation returns the largest single relative error 
 * component found based on the input arrays.  In other words, the 
 * implementation provided here returns the largest component of the following
 * vector:
 * 
 * \f[\vec \epsilon = |{{{EE}_n}\over{x_n^f - x_n^i}}|\f]
 *   
 * There are several alternatives that users of this class can implement: the 
 * error could be calculated based on the largest error in the individual 
 * components of the state vector, as the magnitude of the state vector (that 
 * is, the L2 (rss) norm of the error estimate vector).  The estimated error 
 * should never be negative, so a return value less than 0.0 can be used to 
 * indicate an error condition.
 *     
 * @param diffs         Array of differences calculated by the integrator.  This array 
 *                  must be the same size as the state vector
 * @param answer        Candidate new state from the integrator
 */
//------------------------------------------------------------------------------
Real ForceModel::EstimateError(Real *diffs, Real *answer) const
{
    if (estimationMethod == 1.0)
        return PhysicalModel::EstimateError(diffs, answer);

    Real retval = 0.0, err, mag, vec[3];

    for (int i = 0; i < dimension; i += 3) 
    {
        switch (normType) 
        {

            case -2:
                // Code for the L2 norm, based on sep from central body
                vec[0] = 0.5 * (answer[ i ] + modelState[ i ]); 
                vec[1] = 0.5 * (answer[i+1] + modelState[i+1]); 
                vec[2] = 0.5 * (answer[i+2] + modelState[i+2]);

                mag = vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2];        
                err = diffs[i]*diffs[i] + diffs[i+1]*diffs[i+1] + diffs[i+2]*diffs[i+2];
                if (mag >relativeErrorThreshold) 
                    err = sqrt(err / mag);
                else
                    err = sqrt(err);
                break;

            case -1:
                // L1 norm, based on sep from central body
                vec[0] = fabs(0.5 * (answer[ i ] + modelState[ i ])); 
                vec[1] = fabs(0.5 * (answer[i+1] + modelState[i+1])); 
                vec[2] = fabs(0.5 * (answer[i+2] + modelState[i+2]));

                mag = vec[0] + vec[1] + vec[2];        
                err = fabs(diffs[i]) + fabs(diffs[i+1]) + fabs(diffs[i+2]);
                if (mag >relativeErrorThreshold) 
                    err = err / mag;
                break;

            case 0:         // Report no error here
                return 0.0;

            case 1:
                // L1 norm
                vec[0] = fabs(answer[ i ] - modelState[ i ]); 
                vec[1] = fabs(answer[i+1] - modelState[i+1]); 
                vec[2] = fabs(answer[i+2] - modelState[i+2]);

                mag = vec[0] + vec[1] + vec[2];        
                err = fabs(diffs[i]) + fabs(diffs[i+1]) + fabs(diffs[i+2]);
                if (mag >relativeErrorThreshold) 
                    err = err / mag;
                break;

            case 2:
            default:
                // Code for the L2 norm
                vec[0] = answer[ i ] - modelState[ i ]; 
                vec[1] = answer[i+1] - modelState[i+1]; 
                vec[2] = answer[i+2] - modelState[i+2];

                mag = vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2];        
                err = diffs[i]*diffs[i] + diffs[i+1]*diffs[i+1] + diffs[i+2]*diffs[i+2];
                if (mag >relativeErrorThreshold) 
                    err = sqrt(err / mag);
                else
                    err = sqrt(err);
        }

        if (err > retval)
        {
            retval = err;
        }
    }

    return retval;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the ForceModel.
 *
 * @return clone of the ForceModel.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ForceModel::Clone(void) const
{
   return (new ForceModel(*this));
}

//------------------------------------------------------------------------------
// Integer GetParameterCount(void) const
//------------------------------------------------------------------------------
Integer ForceModel::GetParameterCount(void) const
{
    return parameterCount;
}


// Access methods 
//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string ForceModel::GetParameterText(const Integer id) const
{
    if (id >= PhysicalModelParamCount && id < ForceModelParamCount)
        return PARAMETER_TEXT[id - PhysicalModelParamCount];
    else
        return PhysicalModel::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer ForceModel::GetParameterID(const std::string &str) const
{
    for (int i = PhysicalModelParamCount; i < ForceModelParamCount; i++)
    {
        if (str == PARAMETER_TEXT[i - PhysicalModelParamCount])
            return i;
    }

    return PhysicalModel::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType ForceModel::GetParameterType(const Integer id) const
{
    if (id >= PhysicalModelParamCount && id < ForceModelParamCount)
        return PARAMETER_TYPE[id - PhysicalModelParamCount];
    else
        return PhysicalModel::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string ForceModel::GetParameterTypeString(const Integer id) const
{

    if (id >= PhysicalModelParamCount && id < ForceModelParamCount)
        return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - PhysicalModelParamCount)];
    else
        return PhysicalModel::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string ForceModel::GetStringParameter(const Integer id) const
{
    switch (id)
    {
//    case POINT_MASS:
//        //loj: what should we return here?
//        return "PointMassForce";
//    case FULL_FIELD:
//        return "TBD-FullFieldForce";
//    case DRAG:
//        return "TBD-DragForce";
//    case MAG_FIELD:
//        return "TBD-MagFieldForce";
    // DJC: 06/16/04 Updated for scripting
    case CENTRAL_BODY:
       return "Earth";
    case  DRAG:
       return "Exponential";
    case  SRP:
       return "On";
    default:
        return PhysicalModel::GetStringParameter(id);
    }
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string ForceModel::GetStringParameter(const std::string &label) const
{
    return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool ForceModel::SetStringParameter(const Integer id, const std::string &value)
{
    switch (id)
    {
//    case POINT_MASS:
//        {
//            PhysicalModel *pmf = new PointMassForce();
//            if (pmf != NULL)
//            {
//                if (pmf->SetStringParameter("Body", value))
//                {
//                    AddForce(pmf);
//                    return true;
//                }
//            }
//            return false;
//        }
//    case FULL_FIELD:
//        // build 3
//        return false;
//    case DRAG:
//        // build 3
//        return false;
//    case MAG_FIELD:
//        // build 3
//        return false;

    // DJC: 06/16/04 Updated for scripting
    case CENTRAL_BODY:
       return false;
    case PRIMARY_BODIES:
       return false;
    case POINT_MASSES:
       return false;
    case  DRAG:
       return false;
    case  SRP:
       return false;
    default:
        return PhysicalModel::SetStringParameter(id, value);
    }
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,const std::string &value)
//------------------------------------------------------------------------------
bool ForceModel::SetStringParameter(const std::string &label,
                                    const std::string &value)
{
    return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& ForceModel::GetStringArrayParameter(const Integer id) const
{
    switch (id)
    {
//    case FORCE_LIST:
//        return forceTypeNames;
    case PRIMARY_BODIES:
       return BuildBodyList("GravityField");
    case POINT_MASSES:
//       sar.clear();
//       sar.push_back("Sun");
//       sar.push_back("Moon");
//       return sar;
       return BuildBodyList("PointMassForce");
    default:
        return PhysicalModel::GetStringArrayParameter(id);
    }
}

//------------------------------------------------------------------------------
// StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const StringArray& ForceModel::GetStringArrayParameter(const std::string &label) const
{
    return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& BuildBodyList(std::string type) const
//------------------------------------------------------------------------------
const StringArray& ForceModel::BuildBodyList(std::string type) const
{
   static StringArray bodylist;
   bodylist.clear();
   
   // Run through list of forces, adding body names for GravityField instances
   std::vector<PhysicalModel*>::const_iterator i;
   
   std::string actualType = GetScriptAlias(type);
   
   for (i = forceList.begin(); i != forceList.end(); ++i) {
       if ((*i)->GetTypeName() == actualType) {
          bodylist.push_back((*i)->GetStringParameter("BodyName"));
       }
   }
   return bodylist;
}


//------------------------------------------------------------------------------
// void SetScriptAlias(const std::string& alias, const std::string& typeName)
//------------------------------------------------------------------------------
/**
 * Sets mapping between script descriptions of forces and their class names.
 * 
 * The GMAT script document specifies descriptors for some forces that is 
 * different from the actual class names, and in some cases actually uses more
 * than one name for the same force.  This method is used to build the map
 * between the script strings and the actual class names.  The constructor for
 * the Interpreter base class fills in this data (at least for now -- we might
 * move it to the Moderator's initialization once it is working and tested).
 * 
 * @param alias script string used for the force.
 * @param typeName actual class name as used by the factory that creates it.
 * 
 * @todo Put the initialization for force aliases in a convenient location.
 */
//------------------------------------------------------------------------------
void ForceModel::SetScriptAlias(const std::string& alias, 
                                const std::string& typeName)
{
   if (scriptAliases.find(alias) == scriptAliases.end()) {
      scriptAliases[alias] = typeName;
   }
}


//------------------------------------------------------------------------------
// std::string& GetScriptAlias(const std::string& alias)
//------------------------------------------------------------------------------
/**
 * Accesses mapping between script descriptions of forces and their class names.
 * 
 * This method provides access to class names given an alias in the script.
 * 
 * @param alias script string used for the force.
 * 
 * @return The class name.
 */
//------------------------------------------------------------------------------
std::string& ForceModel::GetScriptAlias(const std::string& alias)
{
   static std::string type;
   type = alias;
   if (scriptAliases.find(alias) != scriptAliases.end()) {
      type = scriptAliases[alias];
   }
   return type;
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Accesses an internal object used in the ForceModel.
 * 
 * This method provides access to the forces used in the ForceModel.  It is used
 * to set and read the specific force attributes -- for example, the file name 
 * used for the full field (GravityField) model.
 * 
 * @param type Base class type for the requested object.  Must be set to
 *             Gmat::PHYSICAL_MODEL for this build.
 * @param name String used for the object.
 * 
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
GmatBase* ForceModel::GetRefObject(const Gmat::ObjectType type,
                                   const std::string &name)
{
   if (type != Gmat::PHYSICAL_MODEL)
       throw ForceModelException("Only forces are accessed in ForceModel::GetRefObject");
   
   // Run through list of forces, adding body names for GravityField instances
   std::vector<PhysicalModel*>::const_iterator i;
   
   std::string actualType = GetScriptAlias(name);
   
   for (i = forceList.begin(); i != forceList.end(); ++i) {
       if ((*i)->GetTypeName() == actualType) {
          return *i;
       }
   }
   return NULL;
}

                                    
//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name, 
//                        const Integer index)
//------------------------------------------------------------------------------
/**
 * Accesses an internal object used in the ForceModel.
 * 
 * This method provides access to the forces used in the ForceModel.  It is used
 * to set and read the specific force attributes -- for example, the file name 
 * used for the full field (GravityField) model.  This version of the method 
 * provides a mechanism to access more than one object with the same type and
 * name by using an index to reach later intances.
 * 
 * @param type Base class type for the requested object.  Must be set to
 *             Gmat::PHYSICAL_MODEL for this build.
 * @param name String used for the object.
 * @param index Index into the list of objects of this type.
 * 
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
GmatBase* ForceModel::GetRefObject(const Gmat::ObjectType type, 
                                   const std::string &name, const Integer index)
{
   if (type != Gmat::PHYSICAL_MODEL)
       throw ForceModelException("Only forces are accessed in ForceModel::GetRefObject");
   
   // Run through list of forces, adding body names for GravityField instances
   std::vector<PhysicalModel*>::const_iterator i;
   std::string actualType = GetScriptAlias(name);
   Integer j = 0;
   
   for (i = forceList.begin(); i != forceList.end(); ++i) {
       if ((*i)->GetTypeName() == actualType) {
          ++j;
          if (j == index)
             return *i;       // Ignore names for forces.
       }
   }
   return NULL;
}


//------------------------------------------------------------------------------
// ObjectArray& GetRefObjectArray(const std::string& typeString)
//------------------------------------------------------------------------------
/**
 * Accesses arrays of internal objects used in the ForceModel.
 * 
 * @param typeString String used for the objects.
 * 
 * @return A reference to the ObjectArray.
 */
//------------------------------------------------------------------------------
ObjectArray& ForceModel::GetRefObjectArray(const std::string& typeString)
{
   static ObjectArray objects;
   objects.clear();
   
   // Run through list of forces, adding body names for GravityField instances
   std::vector<PhysicalModel*>::const_iterator i;
   std::string actualType = GetScriptAlias(typeString);
   
   if (typeString == "PhysicalModel") {
      for (i = forceList.begin(); i != forceList.end(); ++i) {
         objects.push_back(*i);       // Ignore names for forces.
      }
   }
   else {
      for (i = forceList.begin(); i != forceList.end(); ++i) {
          if ((*i)->GetTypeName() == actualType) {
             objects.push_back(*i);       // Ignore names for forces.
          }
      }
   }
   return objects;
}
