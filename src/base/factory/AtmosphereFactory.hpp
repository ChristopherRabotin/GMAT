//$Header$
//------------------------------------------------------------------------------
//                         AtmosphereFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2004/08/12
//
/**
*  This class is the factory class for Atmospheres.
 */
//------------------------------------------------------------------------------
#ifndef AtmosphereFactory_hpp
#define AtmosphereFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "AtmosphereModel.hpp"

class GMAT_API AtmosphereFactory : public Factory
{
public:
   AtmosphereModel*  CreateAtmosphereModel(const std::string &ofType,
                                           const std::string &withName = "",
                                           const std::string &forBody = "Earth");

   // default constructor
   AtmosphereFactory();
   // constructor
   AtmosphereFactory(StringArray createList);
   // copy constructor
   AtmosphereFactory(const AtmosphereFactory& fact);
   // assignment operator
   AtmosphereFactory& operator= (const AtmosphereFactory& fact);

   // destructor
   ~AtmosphereFactory();

protected:
      // protected data

private:


};

#endif // AtmosphereFactory_hpp




