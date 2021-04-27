//------------------------------------------------------------------------------
//                              LobattoIIIA_2Order
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

#ifndef LobattoIIIA_2Order_hpp
#define LobattoIIIA_2Order_hpp

#include "LobattoIIIASeparated.hpp"

class LobattoIIIA_2Order : public LobattoIIIASeparated
{
public:
   /// default constructor
   LobattoIIIA_2Order();
   /// copy constructor
   LobattoIIIA_2Order(const LobattoIIIA_2Order &copy);
   /// operator=
   LobattoIIIA_2Order& operator=(const LobattoIIIA_2Order &copy);

   /// destructor
   ~LobattoIIIA_2Order();

   /// abstract methods inherited from ImplicitRungeKutta
   void InitializeData();

   void LoadButcherTable();

   virtual ImplicitRungeKutta* Clone() const;
};

#endif //LobattoIIIA_2Order_hpp
