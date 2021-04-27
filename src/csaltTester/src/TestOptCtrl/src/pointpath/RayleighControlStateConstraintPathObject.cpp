//$Id$
//------------------------------------------------------------------------------
//                         RayleighControlStateConstraintPathObject
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Claire Conway
// Created: 2017.02.16
//
/**
 * Developed based on RayleighControlStateConstraintPathObject.m
 */
//------------------------------------------------------------------------------

#include "RayleighControlStateConstraintPathObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_RayleighControlStateConstraint_PATH

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------

RayleighControlStateConstraintPathObject::RayleighControlStateConstraintPathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
RayleighControlStateConstraintPathObject::RayleighControlStateConstraintPathObject(const RayleighControlStateConstraintPathObject &copy) :
   UserPathFunction(copy)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
RayleighControlStateConstraintPathObject& RayleighControlStateConstraintPathObject::operator=(const RayleighControlStateConstraintPathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
RayleighControlStateConstraintPathObject::~RayleighControlStateConstraintPathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void RayleighControlStateConstraintPathObject::EvaluateFunctions()
{
	// Define constants
	double p = 0.14;

	//Extract parameter data
	Rvector stateVec = GetStateVector();
	Rvector controlVec = GetControlVector();

	//Real y = stateVec(0);
   Real y2 = -(stateVec(0)) + (stateVec(1)*(1.4 - p*stateVec(1)*stateVec(1))) + 4*controlVec(0);

   Rvector dynFunctions(2, stateVec(1), y2);
   SetFunctions(DYNAMICS, dynFunctions);

   Real y3 = controlVec(0) + (stateVec(0)/6.0);

   Rvector algFunctions(1,y3);
   Rvector algFuncUpper(1, 0.0);
   Rvector algFuncLower(1,-1e16);
   SetFunctions(ALGEBRAIC, algFunctions);
   SetFunctionBounds(ALGEBRAIC, UPPER, algFuncUpper);
   SetFunctionBounds(ALGEBRAIC, LOWER, algFuncLower);

   Real y4 = stateVec(0)*stateVec(0) + controlVec(0)*controlVec(0);

   Rvector costFunction(1,y4);
   SetFunctions(COST, costFunction);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void RayleighControlStateConstraintPathObject::EvaluateJacobians()
{
   // Computes the user Jacobians
   //Does not exist for RayleighControlStateConstraintPathObject.m
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
