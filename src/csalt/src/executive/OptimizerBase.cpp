//------------------------------------------------------------------------------
//                              OptimizerBase
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Youngkwang Kim / Yonsei University
// Created: 2017.09.28
//
/**
 * the base class for SNOPT/IPOPT optimizer classes
 */
//------------------------------------------------------------------------------

#include "OptimizerBase.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_OPTIMIZER

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
OptimizerBase::OptimizerBase(Trajectory* trajectory_in) :
	traj    (trajectory_in)
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
OptimizerBase::OptimizerBase(const OptimizerBase &copy) :
   traj    (copy.traj)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
OptimizerBase& OptimizerBase::operator=(const OptimizerBase &copy)
{
   if (&copy == this)
      return *this;
   
   traj    = copy.traj;

   return *this;   
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
OptimizerBase::~OptimizerBase()
{
   // nothing to do here 
}