//------------------------------------------------------------------------------
//                              DummyPathFunction2
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan / NASA
// Created: 2015.11.02
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef DummyPathFunction2_hpp
#define DummyPathFunction2_hpp

#include "csaltdefs.hpp"
#include "UserPathFunction.hpp"

class DummyPathFunction2 : public UserPathFunction
{
public:
   
   DummyPathFunction2();
   DummyPathFunction2(const DummyPathFunction2 &copy);
   DummyPathFunction2& operator=(const DummyPathFunction2 &copy);
   virtual ~DummyPathFunction2();
   
   virtual void           EvaluateFunctions();
   virtual void           EvaluateJacobians();

protected:
   
};
#endif // DummyPathFunction2_hpp
