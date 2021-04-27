//------------------------------------------------------------------------------
//                              DummyPathFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan / NASA
// Created: 2015.08.15
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef DummyPathFunction_hpp
#define DummyPathFunction_hpp

#include "csaltdefs.hpp"
#include "UserPathFunction.hpp"

class DummyPathFunction : public UserPathFunction
{
public:
   
   DummyPathFunction();
   DummyPathFunction(const DummyPathFunction &copy);
   DummyPathFunction& operator=(const DummyPathFunction &copy);
   virtual ~DummyPathFunction();
   
   virtual void           EvaluateFunctions();
   virtual void           EvaluateJacobians();

protected:
   
};
#endif // DummyPathFunction_hpp
