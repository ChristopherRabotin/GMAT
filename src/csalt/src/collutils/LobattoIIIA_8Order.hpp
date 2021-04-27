//------------------------------------------------------------------------------
//                              LobattoIIIA_8Order
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

#ifndef LobattoIIIA_8Order_hpp
#define LobattoIIIA_8Order_hpp

#include "LobattoIIIASeparated.hpp"

class LobattoIIIA_8Order : public LobattoIIIASeparated
{
public:
   /// default constructor
   LobattoIIIA_8Order();
   /// copy constructor
   LobattoIIIA_8Order(const LobattoIIIA_8Order &copy);
   /// operator=
   LobattoIIIA_8Order& operator=(const LobattoIIIA_8Order &copy);
   /// destructor
   ~LobattoIIIA_8Order();

   /// abstract methods inherited from ImplicitRungeKutta
   void InitializeData();

   void LoadButcherTable();

   virtual ImplicitRungeKutta* Clone() const;
};

#endif //LobattoIIIA_8Order_hpp
