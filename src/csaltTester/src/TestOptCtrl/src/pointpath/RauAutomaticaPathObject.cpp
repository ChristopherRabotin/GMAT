//$Id$
//------------------------------------------------------------------------------
//                         RauAutomaticaPathObject
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Youngkwang Kim
// Created: 2016.11.07
//
/**
 * Developed based on RauAutomaticaPathObject.m
 */
//------------------------------------------------------------------------------

#include "RauAutomaticaPathObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_RauAutomatica_PATH

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

RauAutomaticaPathObject::RauAutomaticaPathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
RauAutomaticaPathObject::RauAutomaticaPathObject(const RauAutomaticaPathObject &copy) :
   UserPathFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
RauAutomaticaPathObject& RauAutomaticaPathObject::operator=(const RauAutomaticaPathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
RauAutomaticaPathObject::~RauAutomaticaPathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void RauAutomaticaPathObject::EvaluateFunctions()
{     
   // Extract parameter data
   Rvector y          = GetStateVector();
   Rvector u          = GetControlVector();
   
   Rvector dynFunctions(1, 5.0 / 2.0*(-y(0) + y(0)*u(0) - u(0)*u(0)));
   SetFunctions(DYNAMICS, dynFunctions);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void RauAutomaticaPathObject::EvaluateJacobians()
{
   // Computes the user Jacobians   
   Rvector y          = GetStateVector();
   Rvector u          = GetControlVector();
   
   Rmatrix dynState(1, 1, 5.0 / 2.0*(-1.0 + u(0)));
   Rmatrix dynControl(1, 1, 5.0 / 2.0*(y(0) - 2.0*u(0)));
   Rmatrix dynTime(1,1, 0.0);
   
   // Dynamics Function Partials
   SetJacobian(DYNAMICS, STATE, dynState);
   SetJacobian(DYNAMICS, CONTROL, dynControl);
   SetJacobian(DYNAMICS, TIME, dynTime);
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
