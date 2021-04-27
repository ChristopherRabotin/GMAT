//------------------------------------------------------------------------------
//                              FunctionInputData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan / NASA
// Created: 2015.07.15
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef FunctionInputData_hpp
#define FunctionInputData_hpp

#include "csaltdefs.hpp"
#include "Rvector.hpp"

class FunctionInputData
{
public:
   
   FunctionInputData();
   FunctionInputData(const FunctionInputData &copy);
   FunctionInputData& operator=(const FunctionInputData &copy);
   virtual ~FunctionInputData();
   
   virtual void           Initialize(Integer numState, Integer numControl, 
                                     Integer numStatic = 0);

   virtual void           SetStateVector(const Rvector &toState);
   virtual void           SetControlVector(const Rvector& toControl);
   
   virtual void           SetPhaseNum(Integer toNum);
   virtual void           SetIsPerturbing(bool isPerturb);
   virtual void           SetIsSparsity(bool isSparse);
   virtual void           SetTime(Real toTime);
   
   virtual const Rvector& GetStateVector();
   virtual const Rvector& GetControlVector();
   virtual Integer        GetNumStateVars();
   virtual Integer        GetNumControlVars();

   virtual Integer        GetPhaseNum();
   virtual bool           IsPerturbing();
   virtual bool           IsSparsity();
   virtual Real           GetTime();

   // YK mod static params
   virtual void           SetStaticVector(const Rvector& toStatic);
   virtual const Rvector& GetStaticVector();
   virtual Integer        GetNumStaticVars();

protected:
   /// number of state variables
   Integer     numStateVars;
   /// state vector
   Rvector     state;
   /// number of control variables
   Integer     numControlVars;
   /// state vector
   Rvector     control;
   /// the time
   Real        time;
   /// phase number
   Integer     phaseNum;
   /// indicates if perturbation call
   bool        isPerturbing;
   /// Indicates whether or not it is a sparsity call
   bool        isSparsity;
   
   // YK mod static params
   /// number of state variables
   Integer     numStaticVars;
   /// state vector
   Rvector     staticVars;

};
#endif // FunctionInputData_hpp
