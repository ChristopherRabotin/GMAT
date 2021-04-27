//------------------------------------------------------------------------------
//                              LobattoIIIA_4HSOrder
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

#ifndef LobattoIIIA_4HSOrder_hpp
#define LobattoIIIA_4HSOrder_hpp

#include "LobattoIIIASeparated.hpp"

class LobattoIIIA_4HSOrder : public LobattoIIIASeparated
{
public:
   /// default constructor
   LobattoIIIA_4HSOrder();
   /// copy constructor
   LobattoIIIA_4HSOrder(const LobattoIIIA_4HSOrder &copy);
   /// operator=
   LobattoIIIA_4HSOrder& operator=(const LobattoIIIA_4HSOrder &copy);
   
   /// destructor
   ~LobattoIIIA_4HSOrder();

   /// abstract methods inherited from ImplicitRungeKutta
   void InitializeData();

   void LoadButcherTable();

   virtual ImplicitRungeKutta* Clone() const;
};

#endif //LobattoIIIA_4HSOrder_hpp
