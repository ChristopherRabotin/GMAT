//------------------------------------------------------------------------------
//                              PathFuncProperties
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: S. Hughes / NASA
// Created: 2015.07.20
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef PathFuncProperties_hpp
#define PathFuncProperties_hpp

#include "csaltdefs.hpp"
#include "Rmatrix.hpp"

class PathFuncProperties 
{

public:
   
   PathFuncProperties();
   PathFuncProperties(const PathFuncProperties &copy);
   PathFuncProperties& operator=(const PathFuncProperties &copy);
   virtual ~PathFuncProperties();
   
   void  SetControlJacPattern(const Rmatrix jacPattern);
   void  SetStateJacPattern(const Rmatrix jacPattern);
   void  SetTimeJacPattern(const Rmatrix jacPattern);
   void  SetNumFunctions(const Integer numFunctions);
   void  SetHasStateVars(const bool hasVars);
   void  SetHasControlVars(const bool hasVars);

protected:

   /// state Jacobian pattern
   Rmatrix stateJacPattern;
   /// time Jacobian pattern
   Rmatrix timeJacPattern;
   ///  control Jacobian pattern
   Rmatrix controlJacPattern;
   ///  number of functions
   Integer numFunctions;
   ///  indicates if problem has state variables
   bool hasStateVars;
   ///  indicates if problem has control variables
   bool hasControlVars;
   
};
#endif // PathFuncProperties_hpp
