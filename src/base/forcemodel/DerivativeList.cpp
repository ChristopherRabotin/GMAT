//$Header$
//------------------------------------------------------------------------------
//                              DerivativeList
//------------------------------------------------------------------------------
// *** File Name : DerivativeList.cpp
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
//                           : 10/02/2003 - W. Waktola, Missions Applications Branch
//				Changes:
//				  - Updated style using GMAT cpp style guide
//
// **************************************************************************

#include "DerivativeList.hpp"

//------------------------------------------------------------------------------
// DerivativeList::DerivativeList(void)
//------------------------------------------------------------------------------
/**
 * The default constructor
 */
//------------------------------------------------------------------------------
DerivativeList::DerivativeList(void) :
    derivative      (NULL),
    next            (NULL)
{
}

//------------------------------------------------------------------------------
// DerivativeList::~DerivativeList(void)
//------------------------------------------------------------------------------
/**
 * The destructor 
 * This destructor destroys its owned force and calls the destructor for the 
 * next link in the list.
 */
//------------------------------------------------------------------------------
DerivativeList::~DerivativeList(void)
{
    if (next)
        delete next;
    if (derivative)
        delete derivative;
}

//------------------------------------------------------------------------------
// DerivativeList::DerivativeList(const DerivativeList& dl)
//------------------------------------------------------------------------------
/**
 * The copy constructor
 * The copy constructor for this class makes a copy of the passed in list, 
 * including copies of each derivative.  It recurses through the list until
 * it reaches the end.  Users of the DerivativeList class must provide copy
 * constructors and assignment operators for each PhysicalModel that is
 * stored in the list.
 *
 * NOTE: The DerivativeList copy constructor is not yet implemented.  This 
 * method should be completed before the class is used in external code.
 *
 * @param dl	The list that is being copied
 */
//------------------------------------------------------------------------------
DerivativeList::DerivativeList(const DerivativeList& dl)
{
}

//------------------------------------------------------------------------------
// DerivativeList& DerivativeList::operator=(const DerivativeList& dl)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 *
 * The assignment operator for this class copies the passed in list into this 
 * instance by deleting the member PhysicalModel and the remaining elements in 
 * the list, and then iterating though the passed in list and reconstructing its
 * members in this instance.
 *  
 * Users of the DerivativeList class must provide copy constructors and 
 * assignment operators for each Derivative that is stored in the list.
 *
 * NOTE: The DerivativeList assignment operator is not yet implemented.  This 
 * method should be completed before the class is used in external code.
 *
 * @param dl	The list that is being copied
 */
//------------------------------------------------------------------------------
DerivativeList& DerivativeList::operator=(const DerivativeList& dl)
{
    if (&dl == this)
        return *this;

    return *this;
}

//------------------------------------------------------------------------------
// void DerivativeList::AddForce(PhysicalModel *pPhysicalModel)
//------------------------------------------------------------------------------
/**
 * Assign a new force to the collection
 * This function manages growth in the linked list of forces; as a new force is
 * added to the force model, it is passed to the list of forces and the 
 * appropriate force pointers are established.  The DerivativeList owns each of
 * the forces, and destroys them (via delete) when the list is destroyed.
 *
 *
 @param pPhysicalModel     The new force for the force model
 */
//------------------------------------------------------------------------------
void DerivativeList::AddForce(PhysicalModel *pPhysicalModel)
{
    // Don't do anything if the passed in pointer is NULL
    if (pPhysicalModel == NULL)
        return;

    // If this instance doesn't have a force, assign the passed in force to it
    if (derivative == NULL) 
    {
        derivative = pPhysicalModel;
        return;
    }

    // Otherwise add it to the next list member, creating it if necessary
    if (next == NULL) 
    {
        next = new DerivativeList;
    }
    next->AddForce(pPhysicalModel);
}

//------------------------------------------------------------------------------
// PhysicalModel * DerivativeList::GetDerivative(void)
//------------------------------------------------------------------------------
/**
 * Returns the pointer to the owned force
 */
//------------------------------------------------------------------------------
PhysicalModel* DerivativeList::GetDerivative(void)
{
    return derivative;
}

//------------------------------------------------------------------------------
// DerivativeList * DerivativeList::Next(void)
//------------------------------------------------------------------------------
/**
 * Returns the next force in the list
 */
//------------------------------------------------------------------------------
DerivativeList* DerivativeList::Next(void)
{
    return next;
}
