//$Id$
//------------------------------------------------------------------------------
//                           SolverState
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
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

#include "SolverState.hpp"

//------------------------------------------------------------------------------
// SolverState(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
SolverState::SolverState(const std::string &name, GmatBase *obj) :
   SolverReal(name, "SolverState", obj, "SolverState", "", Gmat::SOLVER,
         GmatParam::NO_DEP)
{
   mDepObjectName = "";
}

//------------------------------------------------------------------------------
// ~SolverState()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SolverState::~SolverState()
{
}

//------------------------------------------------------------------------------
// SolverState(const SolverState& ss)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ss The parameter that is copied to make this new one
 */
//------------------------------------------------------------------------------
SolverState::SolverState(const SolverState& ss) :
   SolverReal(ss)
{
}

//------------------------------------------------------------------------------
// SolverState& operator=(const SolverState& ss)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ss The parameter that is copied into this one
 *
 * @return Thos parameter, set to look like ss
 */
//------------------------------------------------------------------------------
SolverState& SolverState::operator =(const SolverState& ss)
{
   if (this != &ss)
   {
      SolverReal::operator=(ss);
   }

   return *this;
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates the parameter
 *
 * @return true is the evaluation succeeds, false if it fails
 */
//------------------------------------------------------------------------------
bool SolverState::Evaluate()
{
   bool retval = false;

   mRealValue = SolverData::GetSolverReal("State");

   if (mRealValue == SOLVER_REAL_UNDEFINED)
      return false;
   else
      return true;

   return retval;
}

//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* SolverState::Clone() const
{
   return new SolverState(*this);
}
