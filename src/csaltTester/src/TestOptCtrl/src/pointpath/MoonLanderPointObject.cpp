//$Id$
//------------------------------------------------------------------------------
//                         MoonLanderPointObject
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
 * Developed based on MoonLanderPointObject.m
 */
//------------------------------------------------------------------------------

#include "MoonLanderPointObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_MoonLander_POINT

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

MoonLanderPointObject::MoonLanderPointObject() :
   UserPointFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
MoonLanderPointObject::MoonLanderPointObject(const MoonLanderPointObject &copy) :
   UserPointFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
MoonLanderPointObject& MoonLanderPointObject::operator=(const MoonLanderPointObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPointFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
MoonLanderPointObject::~MoonLanderPointObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void MoonLanderPointObject::EvaluateFunctions()
{
   Rvector stateInit     = GetInitialStateVector(0);
   Rvector stateFin      = GetFinalStateVector(0);
   Real    tInit         = GetInitialTime(0);
   //Real    tFinal        = GetFinalTime(0);

   Real hi = stateInit(0);
   Real vi = stateInit(1);
   Real mi = stateInit(2);
   Real hf = stateFin(0);
   Real vf = stateFin(1);

   Rvector boundVector(6, tInit, hi, vi, mi, hf, vf);
   Rvector boundVecLower(6, 0.0, 1.0, -0.783, 1.0, 0.0, 0.0);
   Rvector boundVecUpper(6, 0.0, 1.0, -0.783, 1.0, 0.0, 0.0);

   SetFunctions(ALGEBRAIC, boundVector);
   SetFunctionBounds(ALGEBRAIC, LOWER, boundVecLower);
   SetFunctionBounds(ALGEBRAIC, UPPER, boundVecUpper);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void MoonLanderPointObject::EvaluateJacobians()
{
   // Currently does nothing
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
