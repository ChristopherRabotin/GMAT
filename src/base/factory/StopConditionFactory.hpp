//$Header$
//------------------------------------------------------------------------------
//                         StopConditionFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
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
#define StopConditiontFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "StopCondition.hpp"

class GMAT_API StopConditionFactory : public Factory
{
public:
   StopCondition*  CreateStopCondition(std::string ofType,
                                       std::string withName = "");

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




