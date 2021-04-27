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
#include "LobattoIIIA_4HSOrder.hpp"

#include "LowThrustException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_LOBATTO_4HSORDER


//------------------------------------------------------------------------------
// LobattoIIIA_4HSOrder()
//------------------------------------------------------------------------------
/**
 * default constructor
 *
 */
//------------------------------------------------------------------------------
LobattoIIIA_4HSOrder::LobattoIIIA_4HSOrder() :
   LobattoIIIASeparated()
{
   // Initialize of LobattoIIIA_4HSOrder
   LoadButcherTable();
   InitializeData();
   ComputeDependencies();
}

//------------------------------------------------------------------------------
// LobattoIIIA_4HSOrder(const LobattoIIIA_4HSOrder &copy)
//------------------------------------------------------------------------------
/**
 * copy constructor
 *
 */
//------------------------------------------------------------------------------
LobattoIIIA_4HSOrder::LobattoIIIA_4HSOrder(const LobattoIIIA_4HSOrder &copy) :
   LobattoIIIASeparated(copy)
{
   /// copying matrices is done in Implicit RK level.
};

//------------------------------------------------------------------------------
// operator=(const LobattoIIIA_4HSOrder &copy)
//------------------------------------------------------------------------------
/**
 * operator=
 *
 */
//------------------------------------------------------------------------------
LobattoIIIA_4HSOrder& LobattoIIIA_4HSOrder::operator=(
                                            const LobattoIIIA_4HSOrder &copy)
{
   if (&copy == this)
      return *this;

   /// copying matrices is done in Implicit RK level.
   LobattoIIIASeparated::operator=(copy);
   
   return *this;
};

//------------------------------------------------------------------------------
// ~LobattoIIIA_4HSOrder()
//------------------------------------------------------------------------------
/**
 * destructor
 *
 */
//------------------------------------------------------------------------------
LobattoIIIA_4HSOrder::~LobattoIIIA_4HSOrder()
{
   // do nothing
}

//------------------------------------------------------------------------------
// void InitializeData()
//------------------------------------------------------------------------------
/**
 * This method initializes the data
 *
 */
//------------------------------------------------------------------------------
void LobattoIIIA_4HSOrder::InitializeData()
{
   numDefectCons = 2;
   numPointsPerStep = 3;
   // Set up non - dimensional stage times
   stageTimes.SetSize(3);
   stageTimes(0) = rhoVec(0);
   stageTimes(1) = rhoVec(1);
   stageTimes(2) = rhoVec(2);
   // Set up the optimization parameter dependency array
   paramDepArray.SetSize(2, 3);
   paramDepArray(0, 0) = -1.0/2.0;
   paramDepArray(1, 0) = -1.0;

   paramDepArray(0, 1) = 1.0;
   paramDepArray(1, 1) = 0.0;

   paramDepArray(0, 2) = -1.0 / 2.0;
   paramDepArray(1, 2) = 1.0;


   // Set up the function dependency array
   funcConstArray.SetSize(2, 3);
   funcConstArray(0, 0) = -1.0 / 8.0;
   funcConstArray(0, 1) = 0.0;
   funcConstArray(0, 2) = 1.0 / 8.0;

   funcConstArray(1, 0) = -betaVec(0);
   funcConstArray(1, 1) = -betaVec(1);
   funcConstArray(1, 2) = -betaVec(2);

   numStagePointsPerMesh = 1;
   numStateStagePointsPerMesh = 1;
   numControlStagePointsPerMesh = 1;
};

//------------------------------------------------------------------------------
// void LoadButcherTable()
//------------------------------------------------------------------------------
/**
 * This method loads data into the butcher table
 *
 */
//------------------------------------------------------------------------------
void LobattoIIIA_4HSOrder::LoadButcherTable()
{
   // Populates the butcher array member data.Taken from table
   // labelled with(S = 3) in Betts.

   // The non - dimensionalized stage times
   rhoVec.SetSize(3);
   rhoVec(0) = 0.0;
   rhoVec(1) = 0.5;
   rhoVec(2) = 1.0;

   // The alpha vector
   betaVec.SetSize(3);
   betaVec(0) = 1.0 / 6.0;
   betaVec(1) = 4.0 / 6.0;
   betaVec(2) = 1.0 / 6.0;

   // The beta matrix
   sigmaMatrix.SetSize(3, 3);

   // Row 2
   sigmaMatrix(1, 0) = 5.0 / 24.0;
   sigmaMatrix(1, 1) = 1.0 / 3.0;
   sigmaMatrix(1, 2) = -1.0 / 24.0;
   // Row 3
   sigmaMatrix(2, 0) = 1.0 / 6.0;
   sigmaMatrix(2, 1) = 4.0 / 6.0;
   sigmaMatrix(2, 2) = 1.0 / 6.0;

   #ifdef DEBUG_LOBATTO_4HSORDER
      MessageInterface::ShowMessage("sigmaMatrix is given as follows:\n%s\n",
                                    sigmaMatrix.ToString(12).c_str());
   #endif
};

//------------------------------------------------------------------------------
//  ImplicitRungeKutta* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Asteroid.
 *
 * @return clone of the ImplicitRungeKutta.
 */
//------------------------------------------------------------------------------
ImplicitRungeKutta* LobattoIIIA_4HSOrder::Clone() const
{
   return (new LobattoIIIA_4HSOrder(*this));
}

