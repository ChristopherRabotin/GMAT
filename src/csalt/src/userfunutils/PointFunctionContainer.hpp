//------------------------------------------------------------------------------
//                          PointFunctionContainer
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan / NASA
// Created: 2015.07.16
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 *
 *  PointFunctionContainer is a container class for data structures passed
 *  to UserPathFunctions and returned containing user data.  This class
 *  basically helps keep the user interfaces clean as the amount of I/O
 *  data increases by bundling all data into a single class.
 */
//------------------------------------------------------------------------------

#ifndef PointFunctionContainer_hpp
#define PointFunctionContainer_hpp

#include "csaltdefs.hpp"
#include "FunctionContainer.hpp"
#include "FunctionOutputData.hpp"

class PointFunctionContainer : public FunctionContainer
{
   
public:
   // class methods
   PointFunctionContainer();
   PointFunctionContainer(const PointFunctionContainer& copy);
   PointFunctionContainer& operator=(const PointFunctionContainer &copy);
   ~PointFunctionContainer();
   
   
   // Intialize the object
   virtual void Initialize();
   
protected:
   
};
#endif // FunctionContainer_hpp
