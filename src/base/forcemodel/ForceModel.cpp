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
//				Changes:
//				  - Updated style using GMAT cpp style guide
//				  - Changed FlightDynamicsForces class to ForceModel class
//
//                           : 10/20/2003 - W. Waktola, Missions Applications Branch
//				Changes:
//				  - All double types to Real types
//				  - All primitive int types to Integer types
//
//                           : 11/9/2003 - D. Conway, Thinking Systems, Inc.
//                             Overrode GetParameterCount so the count 
//                             increases based on the member forces
// **************************************************************************

#include "ForceModel.hpp"

#define normType -2

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
    derivatives       (NULL),
    forceCount        (0),  
    estimationMethod  (2.0)
{
    dimension = 6;
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
    if (derivatives)
        delete derivatives;
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
 * @param fdf	The original of the ForceModel that are copied
 */
//------------------------------------------------------------------------------
ForceModel::ForceModel(const ForceModel& fdf) :
    PhysicalModel    (fdf)
{
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
 * @param fdf	The original of the ForceModel that are copied
 */
//------------------------------------------------------------------------------
ForceModel& ForceModel::operator=(const ForceModel& fdf)
{
    if (&fdf == this)
        return *this;

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
 * DerivativeList), and should not ne destructed externally.  In addition, every 
 * force that is passed to this class needs to have a copy constructor and an 
 * assignment operator so that it can be cloned for distribution to multiple
 * satellites.
 * 
 * @param pPhysicalModel	The force that is being added to the force model
 */
//------------------------------------------------------------------------------
void ForceModel::AddForce(PhysicalModel *pPhysicalModel)
{
    if (pPhysicalModel == NULL)
        return;

    if (derivatives == NULL)
        derivatives = new DerivativeList;

    pPhysicalModel->SetDimension(dimension);

    derivatives->AddForce(pPhysicalModel);
    if (initialized)
        pPhysicalModel->Initialize();

    ++forceCount;  
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
    return true;
}


//------------------------------------------------------------------------------
// void ForceModel::UpdateSpacecraft(void)
//------------------------------------------------------------------------------
/**
 * Updates the state data for the spacecraft that use this force model.
 */
//------------------------------------------------------------------------------
void ForceModel::UpdateSpacecraft(void)
{
    if (spacecraft.size() > 0) {
        Integer j = 0;
        Integer stateSize = 6;
        std::vector<Spacecraft *>::iterator sat;
        Real *state;
        for (sat = spacecraft.begin(); sat != spacecraft.end(); ++sat) {
            state = (*sat)->GetState();
            memcpy(state, &modelState[j*stateSize], stateSize * sizeof(Real));
            ++j;
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
    if (spacecraft.size() > 0) {
        Integer j = 0;
        Integer stateSize = 6;
        std::vector<Spacecraft *>::iterator sat;
        Real *state;
        for (sat = spacecraft.begin(); sat != spacecraft.end(); ++sat) {
            state = (*sat)->GetState();
            memcpy(&modelState[j*stateSize], state, stateSize * sizeof(Real));
            ++j;
        }
    }
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
    if (spacecraft.size() > 0)
        satCount = spacecraft.size();
    
    dimension = stateSize * satCount;
    if (!PhysicalModel::Initialize())
        return false;

    if (spacecraft.size() == 0) {
        modelState[0] = 7000.0;
        modelState[1] =    0.0;
        modelState[2] = 1000.0;
        modelState[3] =    0.0;
        modelState[4] =    7.4;
        modelState[5] =    0.0;
    }
    else {
        Integer j = 0;
        std::vector<Spacecraft *>::iterator sat;
        Real *state;
        for (sat = spacecraft.begin(); sat != spacecraft.end(); ++sat) {
            state = (*sat)->GetState();
            memcpy(&modelState[j*stateSize], state, stateSize * sizeof(Real));
            ++j;
        }
    }

    DerivativeList *current = derivatives;

    while (current) 
    {
        current->GetDerivative()->SetDimension(dimension);
        current->GetDerivative()->Initialize();
        current->GetDerivative()->SetState(modelState);
        current = current->Next();
    }

    return true;
}

//------------------------------------------------------------------------------
// void ForceModel::IncrementTime(Real dt)
//------------------------------------------------------------------------------
void ForceModel::IncrementTime(Real dt)
{
    PhysicalModel::IncrementTime(dt);
    DerivativeList *current = derivatives;
    while (current) 
    {
        current->GetDerivative()->IncrementTime(dt);
        current = current->Next();
    }
}

//------------------------------------------------------------------------------
// void ForceModel::SetTime(Real t)
//------------------------------------------------------------------------------
void ForceModel::SetTime(Real t)
{
    PhysicalModel::SetTime(t);
    DerivativeList *current = derivatives;

    while (current) 
    {
        current->GetDerivative()->SetTime(t);
        current = current->Next();
    }
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

    DerivativeList *current = derivatives;
    const Real * ddt;
    while (current) 
    {
        ddt = current->GetDerivative()->GetDerivativeArray();
        if (!current->GetDerivative()->GetDerivatives(state, dt, order))
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
        current = current->Next();
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
 * @param diffs		Array of differences calculated by the integrator.  This array 
 *                  must be the same size as the state vector
 * @param answer	Candidate new state from the integrator
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


Integer ForceModel::GetParameterCount(void) const
{
    Integer count = parameterCount;
    
    DerivativeList *current = derivatives;
    while (current) 
    {
//        count += current->GetDerivative()->GetParameterCount() + 1;
        ++count;
        current = current->Next();
    }
    
    return count;
}


// Access methods 
std::string ForceModel::GetParameterText(const Integer id) const
{
    Integer count = parameterCount + forceCount, i;
      
    if ((id < count) && (id > 0)) {
        if (id >= parameterCount) {
            DerivativeList *current = derivatives;
            for (i = parameterCount; i < id; ++i)
                current = current->Next();
            return current->GetDerivative()->GetTypeName();
        }
    }
    
    return PhysicalModel::GetParameterText(id);
}

Integer ForceModel::GetParameterID(const std::string &str) const
{
    Integer i;
    Integer retval = PhysicalModel::GetParameterID(str);
    if (retval == -1) {
        // could be a member force
        i = parameterCount;
        DerivativeList *current = derivatives;
        while (current) {
            if (current->GetDerivative()->GetTypeName() == str) {
                retval = i;
                break;
            }
            current = current->Next();
            ++i;
        }
    }
    return retval;
}

Gmat::ParameterType ForceModel::GetParameterType(const Integer id) const
{
    if ((id >= parameterCount) && (id < parameterCount + forceCount)) 
        return Gmat::OBJECT_TYPE;
    return PhysicalModel::GetParameterType(id);
}

std::string ForceModel::GetParameterTypeString(const Integer id) const
{
    if ((id > parameterCount) && (id < parameterCount + forceCount)) 
        return PARAM_TYPE_STRING[Gmat::OBJECT_TYPE];
    return PhysicalModel::GetParameterTypeString(id);
}

std::string ForceModel::GetStringParameter(const Integer id) const
{
    Integer count = parameterCount + forceCount, i;
      
    if ((id < count) && (id > 0)) {
        if (id >= parameterCount) {
            DerivativeList *current = derivatives;
            for (i = parameterCount; i < id; ++i)
                current = current->Next();
            return current->GetDerivative()->GetTypeName();
        }
    }
    
    return PhysicalModel::GetStringParameter(id);

}

bool ForceModel::SetStringParameter(const Integer id, const std::string &value)
{
    Integer count = parameterCount + forceCount, i;
      
    if ((id < count) && (id > 0)) {
        if (id >= parameterCount) {     // Cannot set these yet
            return false;
        }
    }
    
    return PhysicalModel::SetStringParameter(id, value);
}

