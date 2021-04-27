//$Id$
//------------------------------------------------------------------------------
//                                ExecutionInterface
//------------------------------------------------------------------------------
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08.
//
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: 2018.08.14
//
/**
* Defines the ExecutionInterface class.
*/
//------------------------------------------------------------------------------
#include "ExecutionInterface.hpp"

//------------------------------------------------------------------------------
// ExecutionInterface()
//------------------------------------------------------------------------------
/**
* Constructor
*/
//------------------------------------------------------------------------------
ExecutionInterface::ExecutionInterface()
{

}

//------------------------------------------------------------------------------
// ExecutionInterface(const ExecutionInterface &copy)
//------------------------------------------------------------------------------
/**
* Copy constructor
*/
//------------------------------------------------------------------------------
ExecutionInterface::ExecutionInterface(const ExecutionInterface &copy)
{

}

//------------------------------------------------------------------------------
// ExecutionInterface& operator=(const ExecutionInterface &copy)
//------------------------------------------------------------------------------
/**
* Assignment operator
*/
//------------------------------------------------------------------------------
ExecutionInterface& ExecutionInterface::operator=(const ExecutionInterface &copy)
{
   return *this;
}

//------------------------------------------------------------------------------
// ~ExecutionInterface()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
ExecutionInterface::~ExecutionInterface()
{

}

//------------------------------------------------------------------------------
// Rmatrix GetStateArray(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
* Returns the array of state values from the requested phase
*
* @param phaseIdx The phase to recieve the state array from
* @return The phase's state array
*/
//------------------------------------------------------------------------------
Rmatrix ExecutionInterface::GetStateArray(Integer phaseIdx)
{
   return phaseList.at(phaseIdx)->GetStateArray();
}

//------------------------------------------------------------------------------
// Rmatrix GetControlArray(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
* Returns the array of control values from the requested phase
*
* @param phaseIdx The phase to recieve the control array from
* @return The phase's control array
*/
//------------------------------------------------------------------------------
Rmatrix ExecutionInterface::GetControlArray(Integer phaseIdx)
{
   return phaseList.at(phaseIdx)->GetControlArray();
}

//------------------------------------------------------------------------------
// Rvector GetTimeArray(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
* Returns the array of time values from the requested phase corresponding to
* each of the phase's mesh points
*
* @param phaseIdx The phase to recieve the time array from
* @return The phase's time array
*/
//------------------------------------------------------------------------------
Rvector ExecutionInterface::GetTimeArray(Integer phaseIdx)
{
   return phaseList.at(phaseIdx)->GetTimeVector();
}

//------------------------------------------------------------------------------
// void SetPhaseList(std::vector<Phase*> newPhaseList)
//------------------------------------------------------------------------------
/**
* Sets the list of phases of the trajectory
*
* @param newPhaseList The new list of phases
*/
//------------------------------------------------------------------------------
void ExecutionInterface::SetPhaseList(std::vector<Phase*> newPhaseList)
{
   if (newPhaseList.size() == 0)
   {
      throw LowThrustException("The phase list sent to the CSALT ExecutionInterface is "
         "empty.");
   }
   phaseList = newPhaseList;
}
