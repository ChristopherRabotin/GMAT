//------------------------------------------------------------------------------
//                              ImplicitRK
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Youngkwang Kim
// Created: 2017.01.24
//
/**
* From original MATLAB prototype:
*  Author: S. Hughes.  steven.p.hughes@nasa.gov
*
* Explanation goes here.
*
*/
//------------------------------------------------------------------------------
#include "LobattoIIIASeparated.hpp"

#include "LowThrustException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_LOBATTOIIIA_SEPARATED

//------------------------------------------------------------------------------
// LobattoIIIASeparated()
//------------------------------------------------------------------------------
/**
 * default constructor
 *
 */
//------------------------------------------------------------------------------
LobattoIIIASeparated::LobattoIIIASeparated() :
   ImplicitRungeKutta(),
   isSeparated (false)
{
   // do nothing
}

//------------------------------------------------------------------------------
// LobattoIIIASeparated(const LobattoIIIASeparated &copy)
//------------------------------------------------------------------------------
/**
 * copy constructor
 *
 */
//------------------------------------------------------------------------------
LobattoIIIASeparated::LobattoIIIASeparated(const LobattoIIIASeparated &copy) :
   ImplicitRungeKutta(copy)
{
   isSeparated = copy.isSeparated;
};

//------------------------------------------------------------------------------
// operator=(const LobattoIIIASeparated &copy)
//------------------------------------------------------------------------------
/**
 * operator =
 *
 */
//------------------------------------------------------------------------------
LobattoIIIASeparated& LobattoIIIASeparated::operator=(
                                            const LobattoIIIASeparated &copy)
{
   if (&copy == this)
      return *this;
   
   ImplicitRungeKutta::operator=(copy);

   isSeparated = copy.isSeparated;

   return *this;
};

//------------------------------------------------------------------------------
// ~LobattoIIIASeparated()
//------------------------------------------------------------------------------
/**
 * destructor
 *
 */
//------------------------------------------------------------------------------
LobattoIIIASeparated::~LobattoIIIASeparated()
{
   // do nothing
}

//------------------------------------------------------------------------------
// bool GetIsSeparated()
//------------------------------------------------------------------------------
/**
 * This method returns the flag indicating whether or not the ??? is separated
 *
 * @return  true if separated; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool LobattoIIIASeparated::GetIsSeparated()
{ 
   return isSeparated; 
};

//------------------------------------------------------------------------------
// Rvector GetFuncVecFromArray(Rmatrix funcArray,
//                             Integer numStates,
//                             Real deltaT)
//------------------------------------------------------------------------------
/**
 * This method returns ???
 *
 * @return  the function vector
 *
 */
//------------------------------------------------------------------------------
Rvector LobattoIIIASeparated::GetFuncVecFromArray(Rmatrix funcArray,
                                                  Integer numStates,
                                                  Real deltaT)
{
   Rvector funcVec(numStates*numPointsPerStep);
   for (Integer funcIdx = 0; funcIdx < numPointsPerStep; funcIdx++)
   {
      for (Integer stateIdx = 0; stateIdx < numStates; stateIdx++)
         funcVec(numStates*funcIdx + stateIdx) =
                                         deltaT*funcArray(stateIdx, funcIdx);
   }
   return funcVec;
};
