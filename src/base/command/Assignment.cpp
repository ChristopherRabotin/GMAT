//$Header$
//------------------------------------------------------------------------------
//                                Assignment
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/03
//
/**
 * Implementation for the Assignment command class.
 * 
 * The assignment class is the Command class that handles commands of the form
 * 
 *     GMAT object.parameter = value;
 * 
 * Eventually (currently scheduled for build 4) this class will be extended to 
 * include numerical operators on the right side of the assignment line.
 */
//------------------------------------------------------------------------------


#include "Assignment.hpp"

Assignment::Assignment() :
    Command         ("GMAT")
{
}


Assignment::~Assignment()
{
}


Assignment::Assignment(const Assignment& a) :
    Command         (a)
{
}


Assignment& Assignment::operator=(const Assignment& a)
{
    if (this == &a)
        return *this;
        
    return *this;
}


bool Assignment::Initialize(void)
{
}


void Assignment::InterpretAction(void)
{
}


/** 
 * The method that is fired to perform the command.
 *
 * Derived classes implement this method to perform their actions on 
 * GMAT objects.
 *
 * @return true if the Command runs to completion, false if an error 
 *         occurs. 
 */
bool Assignment::Execute(void)
{
    return false;
}
