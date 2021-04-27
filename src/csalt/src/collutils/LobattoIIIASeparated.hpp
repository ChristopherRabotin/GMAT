// ------------------------------------------------------------------------------
//                              LobattoIIIASeparated
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

#ifndef LobattoIIIA_Separated_hpp
#define LobattoIIIA_Separated_hpp

#include "ImplicitRungeKutta.hpp"

class LobattoIIIASeparated : public ImplicitRungeKutta
{
public:
   /// default constructor
   LobattoIIIASeparated();
   /// copy constructor
   LobattoIIIASeparated(const LobattoIIIASeparated &copy);
   /// operator =
   LobattoIIIASeparated& operator=(const LobattoIIIASeparated &copy);

   /// destructor
   virtual ~LobattoIIIASeparated();
   
   bool         GetIsSeparated();


protected:
   /// flag indicating whether or not the ??? is separated
   bool isSeparated;
   
   Rvector GetFuncVecFromArray(Rmatrix funcArray, Integer numStates, Real deltaT);
};

#endif //LobattoIIIA_Separated_hpp
