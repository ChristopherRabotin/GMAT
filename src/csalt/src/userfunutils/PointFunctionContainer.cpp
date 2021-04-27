//------------------------------------------------------------------------------
//                          PointFunctionContainer
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

#include "PointFunctionContainer.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// class methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
PointFunctionContainer::PointFunctionContainer() :
   FunctionContainer()
{
}


//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
PointFunctionContainer::PointFunctionContainer(
                        const PointFunctionContainer& copy) :
   FunctionContainer(copy)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
PointFunctionContainer& PointFunctionContainer::operator=(
                        const PointFunctionContainer &copy)
{
   
   if (&copy == this)
      return *this;
   
   FunctionContainer::operator=(copy);
   
   return *this;
   
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
PointFunctionContainer::~PointFunctionContainer()
{
   // nothing to do here
}

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
/**
 * This method initializes the container
 *
 */
//------------------------------------------------------------------------------
void PointFunctionContainer::Initialize()
{
   FunctionContainer::Initialize();
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
