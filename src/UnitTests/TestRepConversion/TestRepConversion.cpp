//$Header: /cygdrive/p/dev/cvs/test/TestRepConversion/TestRepConversion.cpp,v 1.2 2004/06/23 22:13:31 jgurgan Exp $
//------------------------------------------------------------------------------
//                              TestSpacecraft
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Joey Gurganus 
// Created: 2004/01/16
//
/**
 * Unit test program for spacraft including spherical. 
 */
//------------------------------------------------------------------------------


#include <iostream>
#include "StateConverter.hpp"
#include "Rvector6.hpp"
#include "SphericalOne.hpp"
#include "SphericalTwo.hpp"

int main(int argc, char **argv)
{
   std::cout << "************************************************\n"
             << "*** StateType Conversion Unit Test Program\n"
             << "************************************************\n\n";

   
   StateConverter stateConverter; 

   Rvector6 newState;
   Real state[6];

   state[0] = 7100;
   state[1] = 0.0;
   state[2] = 1300;
   state[3] = 0.0;
   state[4] = 7.35;
   state[5] = 1.0;

   std::cout << "\n--- Beginning with state ----";
   std::cout << std::setprecision(8);
   for (int i=0; i < 6; i++)
       std::cout << "\n[" << i << "]: " << state[i];

  
   std::cout << "\n--- Converting to Keplerian state ----";
   newState = stateConverter.Convert(state,"Cartesian","Keplerian"); 
   for (int i=0; i < 6; i++)
   {
       state[i] = newState.Get(i);
       std::cout << "\n[" << i << "]: " << state[i];
   }

   std::cout << "\n--- Converting to Cartesian state ----";
   newState = stateConverter.Convert(state,"Keplerian","Cartesian"); 
   for (int i=0; i < 6; i++)
   {
       state[i] = newState.Get(i);
       std::cout << "\n[" << i << "]: " << state[i];
   }

   std::cout << "\n********************* End of Testing ********************\n";
}
