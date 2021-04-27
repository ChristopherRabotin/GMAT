//------------------------------------------------------------------------------
//                              PathFuncProperties
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: S. Hughes / NASA
// Created: 2015.07.20
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#include "PathFuncProperties.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
PathFuncProperties::PathFuncProperties() :
   numFunctions  (0),
   hasStateVars  (false),
   hasControlVars(false)
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
PathFuncProperties::PathFuncProperties(const PathFuncProperties &copy) :
   stateJacPattern    (copy.stateJacPattern),
   timeJacPattern     (copy.timeJacPattern),
   controlJacPattern  (copy.controlJacPattern),
   numFunctions       (copy.numFunctions),
   hasStateVars       (copy.hasStateVars),
   hasControlVars     (copy.hasControlVars)
{

}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
PathFuncProperties& PathFuncProperties::operator=(
                                        const PathFuncProperties &copy)
{
   if (this != &copy)
   {
      stateJacPattern = copy.stateJacPattern;
      timeJacPattern = copy.timeJacPattern;
      controlJacPattern = copy.controlJacPattern;
      numFunctions = copy.numFunctions;
      hasStateVars = copy.hasStateVars;
      hasControlVars = copy.hasControlVars;
   }

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
PathFuncProperties::~PathFuncProperties()
{
   // nothing to do here at the moment
}

//------------------------------------------------------------------------------
// void Initialize(Integer numState, Integer numControl)
//------------------------------------------------------------------------------
//void PathFuncProperties::Initialize(Integer numState, Integer numControl)
//{
//   FunctionInputData::Initialize(numState);
//   numControlVars   = numControl;
//}

//------------------------------------------------------------------------------
// void  SetControlJacPattern(const Rmatrix jacPattern)
//------------------------------------------------------------------------------
/**
 * This method sets the control jacobian pattern
 *
 * @param <jacPattern>  input jacobian pattern
 *
 */
//------------------------------------------------------------------------------
void PathFuncProperties::SetControlJacPattern(const Rmatrix jacPattern)
{
   controlJacPattern = jacPattern;
}

//------------------------------------------------------------------------------
// void  SetStateJacPattern(const Rmatrix jacPattern)
//------------------------------------------------------------------------------
/**
 * This method sets the state jacobian pattern
 *
 * @param <jacPattern>  input jacobian pattern
 *
 */
//------------------------------------------------------------------------------
void PathFuncProperties::SetStateJacPattern(const Rmatrix jacPattern)
{
   stateJacPattern = jacPattern;
}

//------------------------------------------------------------------------------
// void  SetTimeJacPattern(const Rmatrix jacPattern)
//------------------------------------------------------------------------------
/**
 * This method sets the time jacobian pattern
 *
 * @param <jacPattern>  input jacobian pattern
 *
 */
//------------------------------------------------------------------------------
void PathFuncProperties::SetTimeJacPattern(const Rmatrix jacPattern)
{
   timeJacPattern = jacPattern;
}

//------------------------------------------------------------------------------
// void  SetNumFunctions(const Integer numFunctions)
//------------------------------------------------------------------------------
/**
 * This method sets the number of functions
 *
 * @param <numFunctions>  input number of functions
 *
 */
//------------------------------------------------------------------------------
void PathFuncProperties::SetNumFunctions(const Integer numFuncs)
{
   numFunctions = numFuncs;
}

//------------------------------------------------------------------------------
// void SetHasStateVars(const bool hasVars)
//------------------------------------------------------------------------------
/**
 * This method sets the flag indicating whether or not it has state variables
 *
 * @param <hasVars>  flag indicating if it has state vars
 *
 */
//------------------------------------------------------------------------------
void PathFuncProperties::SetHasStateVars(const bool hasVars)
{
   hasStateVars = hasVars;
}

//------------------------------------------------------------------------------
// void SetHasControlVars(const bool hasVars)
//------------------------------------------------------------------------------
/**
 * This method sets the flag indicating whether or not it has control variables
 *
 * @param <hasVars>  flag indicating if it has control vars
 *
 */
//------------------------------------------------------------------------------
void PathFuncProperties::SetHasControlVars(const bool hasVars)
{
   hasControlVars = hasVars;
}

