//$Header$
//------------------------------------------------------------------------------
//                         AxisSystemFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G and MOMS Task order 124
//
// Author: Wendy Shoan
// Created: 2004/12/23
//
/**
 *  This class is the factory class for AxisSystem objects.
 *
 */
//------------------------------------------------------------------------------
#ifndef AxisSystemFactory_hpp
#define AxisSystemFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "AxisSystem.hpp"

class GMAT_API AxisSystemFactory : public Factory
{
public:
   AxisSystem*  CreateAxisSystem(const std::string &ofType,
                                 const std::string &withName = "");
 
   // default constructor
   AxisSystemFactory();
   // constructor
   AxisSystemFactory(StringArray createList);
   // copy constructor
   AxisSystemFactory(const AxisSystemFactory& fact);
   // assignment operator
   AxisSystemFactory& operator= (const AxisSystemFactory& fact);

   // destructor
   ~AxisSystemFactory();

protected:
      // protected data

private:


};

#endif // AxisSystemFactory_hpp




