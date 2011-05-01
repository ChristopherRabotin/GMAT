//$Id: EstimationCommandFactory.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         EstimationCommandFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel Conway
// Created: 2009/08/03
//
/**
 *  This class is the factory class for estimator commands.
 */
//------------------------------------------------------------------------------
#ifndef EstimationCommandFactory_hpp
#define EstimationCommandFactory_hpp


#include "estimation_defs.hpp"
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "GmatCommand.hpp"

class ESTIMATION_API EstimationCommandFactory : public Factory
{
public:
   GmatCommand*  CreateCommand(const std::string &ofType,
                               const std::string &withName = "");

   // default constructor
   EstimationCommandFactory();
   // constructor
   EstimationCommandFactory(StringArray createList);
   // copy constructor
   EstimationCommandFactory(const EstimationCommandFactory& fact);
   // assignment operator
   EstimationCommandFactory& operator= (const EstimationCommandFactory& fact);

   // destructor
   ~EstimationCommandFactory();

protected:
   // protected data

private:
   // private data

};

#endif // EstimationCommandFactory_hpp
