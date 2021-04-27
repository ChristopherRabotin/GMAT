//------------------------------------------------------------------------------
//                           UserFunctionManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan / NASA
// Created: 2015.11.20
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef UserFunctionManager_hpp
#define UserFunctionManager_hpp

#include "csaltdefs.hpp"
#include <random>
#include "FunctionOutputData.hpp"
#include "UserPathFunction.hpp"
#include "Rvector.hpp"

class UserFunctionManager
{
public:
   
   UserFunctionManager();
   UserFunctionManager(const UserFunctionManager &copy);
   UserFunctionManager& operator=(const UserFunctionManager &copy);
   virtual ~UserFunctionManager();
   
   Rvector      GetRandomVector(const Rvector &lowerBound,
                                const Rvector &upperBound);
   
   virtual void SetIsInitializing(bool isInit);
   virtual bool IsInitializing();
   
   virtual bool HasCostFunction();
   
protected:
   
   /// Pointer to the FunctionOutputData
   UserPathFunction   *userData;
   /// The phase ID number
   Integer            phaseNum;
   /// Flag indicating whether the user provided a function
   bool               hasFunction;
   /// Number of state parameters
   Integer            numStateVars;
   /// Number of controls
   Integer            numControlVars;
   /// Number of time variables
   Integer            numTimeVars;
   /// Number of static parameters
   Integer            numStaticVars;
   /// Flag indicating testing of user-provided derivatives
   bool               forceFiniteDiff;
   /// Indicates if user has cost component
   bool               hasCostFunction;
   /// Flag to control some actions during initialization
   bool               isInitializing;
   
   /// Random number generator
   std::default_random_engine randGen;
   /// Distribution setting for the random number generator, will be 
   /// set to use real numbers between 0 and 1
   std::uniform_real_distribution<Real> realDistribution;
};
#endif // UserFunctionManager_hpp

