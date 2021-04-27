//$Id$
//------------------------------------------------------------------------------
//                         BreakwellPathObject
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Claire Conway
// Created: 2017.02.13
//
/**
 * Developed based on BreakwellPathObject.m
 */
//------------------------------------------------------------------------------

#include "BreakwellPathObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_Breakwell_PATH

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

BreakwellPathObject::BreakwellPathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
BreakwellPathObject::BreakwellPathObject(const BreakwellPathObject &copy) :
   UserPathFunction(copy)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
BreakwellPathObject& BreakwellPathObject::operator=(const BreakwellPathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
BreakwellPathObject::~BreakwellPathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void BreakwellPathObject::EvaluateFunctions()
{

   //Extract parameter data
   Rvector stateVec = GetStateVector();
   Rvector controlVec = GetControlVector();
   Real time = GetTime();

   Real x = stateVec(0);
   Real v = stateVec(1);
   Real u = controlVec(0);

   Rvector costFunction(1,0.5*u*u);
   Rvector dynFunction(2,v, u);
   SetCostFunction(costFunction);
   SetDynFunctions(dynFunction);

   Rvector algFunction(1,x);
   Rvector upperBound(1,0.1);
   Rvector lowerBound(1,-10);
   SetAlgFunctions(algFunction);
   SetAlgUpperBounds(upperBound);
   SetAlgLowerBounds(lowerBound);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void BreakwellPathObject::EvaluateJacobians()
{
   //Extract parameter data
   Rvector controlVec = GetControlVector();
   u = controlVec(0);

   Rmatrix costStateJac(1,2);
   costStateJac(0,0) = 0.0;
   costStateJac(0,1) = 0.0;
   Rmatrix costControlJac(1,1);
   costControlJac(0,0) = u;
   Rmatrix costTimeJac(1,1);
   costTimeJac(0,0) = 0.0;
   SetCostStateJacobian(costStateJac);
   SetCostControlJacobian(costControlJac);
   SetCostTimeJacobian(costTimeJac);

   Rmatrix algStateJac(1,2);
   algStateJac(0,0) = 1.0;
   algStateJac(0,1) = 0.0;
   Rmatrix algControlJac(1,1);
   algControlJac(0,0) = 0.0;
   Rmatrix algTimeJac(1,1);
   algTimeJac(0,0) = 0.0;
   SetAlgStateJacobian(algStateJac);
   SetAlgControlJacobian(algControlJac);
   SetAlgTimeJacobian(algTimeJac);

   Rmatrix dynStateJac(2,2);
   dynStateJac(0,0) = 0.0;
   dynStateJac(0,1) = 1.0;
   dynStateJac(1,0) = 0.0;
   dynStateJac(1,1) = 0.0;
   Rmatrix dynControlJac(1,2);
   dynControlJac(0,0) = 0.0;
   dynControlJac(0,1) = 1.0;
   Rmatrix dynTimeJac(1,2);
   dynTimeJac(0,0) = 0.0;
   dynTimeJac(0,1) = 0.0;
   SetDynStateJacobian(dynStateJac);
   SetDynControlJacobian(dynControlJac);
   SetDynTimeJacobian(dynTimeJac);

}
//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
