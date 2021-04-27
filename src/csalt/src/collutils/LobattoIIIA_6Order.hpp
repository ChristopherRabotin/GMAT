//------------------------------------------------------------------------------
//                              LobattoIIIA_6Order
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

#ifndef LobattoIIIA_6Order_hpp
#define LobattoIIIA_6Order_hpp

#include "LobattoIIIASeparated.hpp"

class LobattoIIIA_6Order : public LobattoIIIASeparated
{
public:
   /// default constructor
   LobattoIIIA_6Order();
   /// copy constructor
   LobattoIIIA_6Order(const LobattoIIIA_6Order &copy);
   /// operator=
   LobattoIIIA_6Order& operator=(const LobattoIIIA_6Order &copy);

   /// destructor
   ~LobattoIIIA_6Order();

   /// abstract methods inherited from ImplicitRungeKutta
   void InitializeData();

   void LoadButcherTable();

   virtual ImplicitRungeKutta* Clone() const;
};

#endif //LobattoIIIA_6Order_hpp
