//$Header$
//------------------------------------------------------------------------------
//                         CommandFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2003/10/09
//
/**
 *  This class is the factory class for commands.  
 */
//------------------------------------------------------------------------------
#ifndef CommandFactory_hpp
#define CommandFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "Command.hpp"

class GMAT_API CommandFactory : public Factory
{
public:
   GmatCommand*  CreateCommand(std::string ofType, std::string withName = "");

   // default constructor
   CommandFactory();
   // constructor
   CommandFactory(StringArray createList);
   // copy constructor
   CommandFactory(const CommandFactory& fact);
   // assignment operator
   CommandFactory& operator= (const CommandFactory& fact);

   // destructor
   virtual ~CommandFactory();

protected:
   // protected data

private:

   // private data


};

#endif // CommandFactory_hpp

