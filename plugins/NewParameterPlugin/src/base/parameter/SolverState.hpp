//$Id$
//------------------------------------------------------------------------------
//                           SolverState
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Oct 9, 2013
/**
 * 
 */
//------------------------------------------------------------------------------

#ifndef SolverState_hpp
#define SolverState_hpp

#include "SolverReal.hpp"

class NEW_PARAMETER_API SolverState : public SolverReal
{
public:
   SolverState(const std::string &name = "", GmatBase *obj = NULL);
   virtual ~SolverState();
   SolverState(const SolverState& ss);
   SolverState& operator=(const SolverState& ss);

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;
};

#endif /* SolverState_hpp */
