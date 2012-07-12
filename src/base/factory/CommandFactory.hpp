//$Id$
//------------------------------------------------------------------------------
//                         CommandFactory
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
#include "GmatCommand.hpp"

class GMAT_API CommandFactory : public Factory
{
public:
   GmatCommand*  CreateCommand(const std::string &ofType,
                               const std::string &withName = "");

   // default constructor
   CommandFactory();
   // constructor
   CommandFactory(StringArray createList);
   // copy constructor
   CommandFactory(const CommandFactory& fact);
   // assignment operator
   CommandFactory& operator= (const CommandFactory& fact);

   // destructor
   ~CommandFactory();

   // method to return list of types of objects that this factory can create
   virtual StringArray      GetListOfCreatableObjects(
                                  const std::string &qualifier = "");

protected:
   // protected data
   /// List of commands that toggle from object mode to command mode
   StringArray              sequenceStarters;

private:

   // private data


};

#endif // CommandFactory_hpp

