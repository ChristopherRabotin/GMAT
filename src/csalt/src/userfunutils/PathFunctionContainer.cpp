//------------------------------------------------------------------------------
//                          PathFunctionContainer
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan / NASA
// Created: 2015.07.16
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 *
 *  POINTFUNCTIONCONTAINER is a container class for data structures passed
 *  to UserPointFunctions and returned containing user data.  This class
 *  basically helps keep the user interfaces clean as the amount of I/O
 *  data increases by bundling all data into a single class.
 */
//------------------------------------------------------------------------------

#include "PathFunctionContainer.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// class methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
PathFunctionContainer::PathFunctionContainer() :
   FunctionContainer(),
   dynData  (NULL)
{
}


//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
PathFunctionContainer::PathFunctionContainer(
                       const PathFunctionContainer& copy) :
   FunctionContainer(copy),
   dynData   (NULL)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
PathFunctionContainer& PathFunctionContainer::operator=(
                       const PathFunctionContainer &copy)
{
   
   if (&copy == this)
      return *this;
   
   FunctionContainer::operator=(copy);
   
//   if (dynData) delete dynData;
   dynData   = copy.dynData;  // or CLONE this???
   
   return *this;
   
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
PathFunctionContainer::~PathFunctionContainer()
{
   if (dynData)  delete dynData; // don't want to delete it if it was set and
                                   // not created here
}

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
/**
 * This method initializes the container
 *
 */
//------------------------------------------------------------------------------
void PathFunctionContainer::Initialize()
{
   FunctionContainer::Initialize();
   if (!dynData)  dynData  = new FunctionOutputData();
}


//------------------------------------------------------------------------------
// UserPathFunctionData* GetDynData()
//------------------------------------------------------------------------------
/**
 * This method returns a pointer to the dyn data
 *
 * @return a pointer to the dyn data
 *
 */
//------------------------------------------------------------------------------
FunctionOutputData* PathFunctionContainer::GetDynData()
{
   return dynData;
}



//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
