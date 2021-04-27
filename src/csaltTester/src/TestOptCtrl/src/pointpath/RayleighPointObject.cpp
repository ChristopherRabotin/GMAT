//$Id$
//------------------------------------------------------------------------------
//                         RayleighPointObject
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
 * Developed based on RayleighPointObject.m
 */
//------------------------------------------------------------------------------

#include "RayleighPointObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_Rayleigh_POINT

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

RayleighPointObject::RayleighPointObject() :
   UserPointFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
RayleighPointObject::RayleighPointObject(const RayleighPointObject &copy) :
   UserPointFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
RayleighPointObject& RayleighPointObject::operator=(const RayleighPointObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPointFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
RayleighPointObject::~RayleighPointObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void RayleighPointObject::EvaluateFunctions()
{
   Rvector stateInit     = GetInitialStateVector(0);
   Rvector stateFinal    = GetFinalStateVector(0);
   Real    tInit         = GetInitialTime(0);
   Real    tFinal        = GetFinalTime(0);

   Rvector boundVector(6,tInit, tFinal, stateInit(0), stateInit(1),
         stateFinal(0), stateFinal(1));

   Rvector boundVecLower(6, 0.0, 4.5, -5.0, -5.0, 0.0, 0.0);
   Rvector boundVecUpper(6, 0.0, 4.5, -5.0, -5.0, 0.0, 0.0);

   SetFunctions(ALGEBRAIC, boundVector);
   SetFunctionBounds(ALGEBRAIC, LOWER, boundVecLower);
   SetFunctionBounds(ALGEBRAIC, UPPER, boundVecUpper);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void RayleighPointObject::EvaluateJacobians()
{
   // Currently does nothing
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
