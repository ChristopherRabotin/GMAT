//$Header$
//------------------------------------------------------------------------------
//                         PhysicalModelFactory
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
*  This class is the factory class for PhysicalModels.
 */
//------------------------------------------------------------------------------
#ifndef PhysicalModelFactory_hpp
#define PhysicalModelFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "PhysicalModel.hpp"

class GMAT_API PhysicalModelFactory : public Factory
{
public:
   PhysicalModel*  CreatePhysicalModel(std::string ofType,
                                       std::string withName = "");

   // default constructor
   PhysicalModelFactory();
   // constructor
   PhysicalModelFactory(StringArray createList);
   // copy constructor
   PhysicalModelFactory(const PhysicalModelFactory& fact);
   // assignment operator
   PhysicalModelFactory& operator= (const PhysicalModelFactory& fact);

   // destructor
   ~PhysicalModelFactory();

protected:
      // protected data

private:
      // private data


};

#endif // PhysicalModelFactory_hpp




