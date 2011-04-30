//$Id$
//------------------------------------------------------------------------------
//                         StopConditionFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2003/10/22
//
/**
*  This class is the factory class for StopConditions.
 */
//------------------------------------------------------------------------------
#ifndef StopConditionFactory_hpp
#define StopConditionFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "StopCondition.hpp"

class GMAT_API StopConditionFactory : public Factory
{
public:
   StopCondition*  CreateStopCondition(const std::string &ofType,
                                       const std::string &withName = "");

   // default constructor
   StopConditionFactory();
   // constructor
   StopConditionFactory(StringArray createList);
   // copy constructor
   StopConditionFactory(const StopConditionFactory& fact);
   // assignment operator
   StopConditionFactory& operator= (const StopConditionFactory& fact);

   // destructor
   ~StopConditionFactory();

protected:
      // protected data

private:
      // private data


};

#endif // StopConditionFactory_hpp




