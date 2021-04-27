//$Id$
//------------------------------------------------------------------------------
//                         RayleighControlStateConstraintPointObject
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Claire Conway
// Created: 2017.01.30
//
/**
 * Developed based on RayleighControlStateConstraintPointObject.m
 */
//------------------------------------------------------------------------------

#include "RayleighControlStateConstraintPointObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_RayleighControlStateConstraint_POINT

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

RayleighControlStateConstraintPointObject::RayleighControlStateConstraintPointObject() :
   UserPointFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
RayleighControlStateConstraintPointObject::RayleighControlStateConstraintPointObject(const RayleighControlStateConstraintPointObject &copy) :
   UserPointFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
RayleighControlStateConstraintPointObject& RayleighControlStateConstraintPointObject::operator=(const RayleighControlStateConstraintPointObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPointFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
RayleighControlStateConstraintPointObject::~RayleighControlStateConstraintPointObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void RayleighControlStateConstraintPointObject::EvaluateFunctions()
{
   Rvector stateInit     = GetInitialStateVector(0);
   Real    tInit         = GetInitialTime(0);
   Real    tFinal        = GetFinalTime(0);

   Rvector boundVector(4,tInit, tFinal, stateInit(0), stateInit(1));
   Rvector boundVecLower(4,0.0, 4.5, -5.0, -5.0);
   Rvector boundVecUpper(4,0.0, 4.5, -5.0, -5.0);
   SetFunctions(ALGEBRAIC, boundVector);
   SetFunctionBounds(ALGEBRAIC, LOWER, boundVecLower);
   SetFunctionBounds(ALGEBRAIC, UPPER, boundVecUpper);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void RayleighControlStateConstraintPointObject::EvaluateJacobians()
{
   // Currently does nothing
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
