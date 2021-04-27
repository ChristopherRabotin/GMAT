//------------------------------------------------------------------------------
//                              LobattoIIIA_4Order
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

#ifndef LobattoIIIA_4Order_hpp
#define LobattoIIIA_4Order_hpp

#include "LobattoIIIASeparated.hpp"

class LobattoIIIA_4Order : public LobattoIIIASeparated
{
public:
   /// default constructor
   LobattoIIIA_4Order();
   /// copy constructor
   LobattoIIIA_4Order(const LobattoIIIA_4Order &copy);
   /// operator=
   LobattoIIIA_4Order& operator=(const LobattoIIIA_4Order &copy);

   /// destructor
   ~LobattoIIIA_4Order();

   /// abstract methods inherited from ImplicitRungeKutta
   void InitializeData();

   void LoadButcherTable();

   virtual ImplicitRungeKutta* Clone() const;
};

#endif //LobattoIIIA_4Order_hpp
