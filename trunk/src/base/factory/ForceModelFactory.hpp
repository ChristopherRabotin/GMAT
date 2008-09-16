//$Header$
//------------------------------------------------------------------------------
//                         ForceModelFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2003/10/14
//
/**
 *  This class is the factory class for ForceModels.  
 */
//------------------------------------------------------------------------------
#ifndef ForceModelFactory_hpp
#define ForceModelFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "ForceModel.hpp"

class GMAT_API ForceModelFactory : public Factory
{
public:
   ForceModel*  CreateForceModel(const std::string &ofType,
                                 const std::string &withName = "");

   // default constructor
   ForceModelFactory();
   // constructor
   ForceModelFactory(StringArray createList);
   // copy constructor
   ForceModelFactory(const ForceModelFactory& fact);
   // assignment operator
   ForceModelFactory& operator= (const ForceModelFactory& fact);

   // destructor
   ~ForceModelFactory();

protected:
   // protected data

private:
   // private data


};

#endif // ForceModelFactory_hpp




