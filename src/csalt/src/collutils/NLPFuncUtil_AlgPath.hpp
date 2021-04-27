//$Id$
//------------------------------------------------------------------------------
//                         NLPFuncUtil_AlgPath
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Youngkwang Kim
// Created: 2015/11/30 (mods 2016.05.20 WCS)
//
/**
 * Developed based on NLPFuncUtil_AlgPath.m
 */
//------------------------------------------------------------------------------
#ifndef NLPFuncUtil_AlgPath_hpp
#define NLPFuncUtil_AlgPath_hpp

#include "csaltdefs.hpp"
#include "NLPFuncUtil_Path.hpp"
#include "NLPFunctionData.hpp"
#include "UserFunctionProperties.hpp"
#include "FunctionOutputData.hpp"

/**
 * NLPFuncUtil_AlgPath class
 */
class NLPFuncUtil_AlgPath : public NLPFuncUtil_Path
{
public:
   
   /// default constructor
   NLPFuncUtil_AlgPath();
   /// copy constructor
   NLPFuncUtil_AlgPath(const NLPFuncUtil_AlgPath &copy);
   /// operator=
   NLPFuncUtil_AlgPath& operator=(const NLPFuncUtil_AlgPath &copy);
   /// default destructor
   virtual ~NLPFuncUtil_AlgPath();
   
   
   /// Initialize NLPFuncUtil_AlgPath
   void Initialize(UserFunctionProperties *funcProps,
                   const std::vector<FunctionOutputData*> &funcData,
                   Integer                numDecisionParams,
                   Integer                numFuncPoints,
                   const Rvector          &dTi,
                   const Rvector          &dTf);
   
   /// Get the number of non-zero elements in the matrix
   IntegerArray GetMatrixNumNonZeros();
   
   /// Compute the function(s) and Jacobian
   void ComputeFuncAndJac(const std::vector<FunctionOutputData*> &funcData,
                          Rvector    &funcValues,
                          RSMatrix   &jacArray);
   
   /// Compute the sparsity pattern
   RSMatrix* ComputeSparsity();
  
   
protected:
   // Parameters
   
   /// NLPFunctionData
   NLPFunctionData dataFuncUtil;
   /// Number of functions at each discretization point
   Integer         numFunctionsPerPoint;
   /// Number of points with path functions (length of function data vec)
   Integer         numFunctionPoints;
   /// Number of constraint functions
   Integer         numFunctions;
   /// Number of decision parameters
   /// added by YK
   Integer         numDecisionParams;

   /// Indicates if problem has state variables
   bool            hasStateVars;
   /// Indicates if problem has control variables
   bool            hasControlVars;
   
   /// Partial of time at each point w/r/t to initial time
   Rvector         dTimedTi;
   /// Partial of time at each point w/r/t to final time
   Rvector         dTimedTf;
   
   /// Update arrays that are not constant (q, and dq/dz)
   void FillUserNLPMatrices(const std::vector<FunctionOutputData*> &funcData,
                            Rvector                  &funcValues,
                            RSMatrix                 &jacArray);
   
   // added by YK
   void InitializeConstantNLPMatrices(UserFunctionProperties *funcProps,
                  const std::vector<FunctionOutputData*> &funcData);
};

#endif // NLPFuncUtil_AlgPath_hpp