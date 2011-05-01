//$Id: DataFileFactory.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         DataFileFactory
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/06/20
//
/**
 * Factory used to create DataFile objects.
 */
//------------------------------------------------------------------------------


#ifndef DataFileFactory_hpp
#define DataFileFactory_hpp

#include "estimation_defs.hpp"
#include "Factory.hpp"
#include "DataFile.hpp"


class ESTIMATION_API DataFileFactory : public Factory
{
public:
   DataFileFactory();
   virtual ~DataFileFactory();

   DataFileFactory(StringArray createList);
   DataFileFactory(const DataFileFactory& fact);
   DataFileFactory& operator= (const DataFileFactory& fact);

   virtual DataFile *CreateDataFile(const std::string &ofType,
         const std::string &withName = "");
};

#endif /* DataFileFactory_hpp */
