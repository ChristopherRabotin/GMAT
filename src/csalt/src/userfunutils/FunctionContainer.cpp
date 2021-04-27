//------------------------------------------------------------------------------
//                              FunctionContainer
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
 *  FUNCTIONCONTAINER is a container class for data structures passed
 *  to UserFunctions and returned containing user data.  This class
 *  basically helps keep the user interfaces clean as the amount of I/O
 *  data increases by bundling all data into a single class.
 */
//------------------------------------------------------------------------------
#include "FunctionContainer.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// class methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
FunctionContainer::FunctionContainer() :
   costData  (NULL),
   algData   (NULL)
{
}


//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
FunctionContainer::FunctionContainer(const FunctionContainer& copy) :
   costData   (NULL),
   algData    (NULL)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
FunctionContainer& FunctionContainer::operator=(const FunctionContainer &copy)
{
   
   if (&copy == this)
      return *this;
   
//   if (costData) delete costData;
//   if (algData)  delete algData;

   costData   = copy.costData;  // or CLONE this???
   algData    = copy.algData;   // or CLONE this???
   
   return *this;

}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
FunctionContainer::~FunctionContainer()
{
   if (costData)  delete costData;  // ???
   if (algData)   delete algData;
}

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
/**
 * This method initializes the FunctionContainer
 *
 *
 */
//------------------------------------------------------------------------------
void FunctionContainer::Initialize()
{
   // @todo should these be done in constructor?
   if (!costData) costData = new FunctionOutputData();
   if (!algData)  algData  = new FunctionOutputData();
}


//------------------------------------------------------------------------------
// FunctionOutputData* GetCostData()
//------------------------------------------------------------------------------
/**
 * This method returns the Cost data
 *
 * @return the cost data
 *
 */
//------------------------------------------------------------------------------
FunctionOutputData* FunctionContainer::GetCostData()
{
   return costData;
}

//------------------------------------------------------------------------------
// FunctionOutputData* GetAlgData()
//------------------------------------------------------------------------------
/**
 * This method returns the Alg data
 *
 * @return the alg data
 *
 */
//------------------------------------------------------------------------------
FunctionOutputData* FunctionContainer::GetAlgData()
{
   return algData;
}

