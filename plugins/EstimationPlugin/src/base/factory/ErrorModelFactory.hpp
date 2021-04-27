//$Id: ErrorModelFactory.hpp 1398 2015-01-07 20:39:37Z tdnguyen $
//------------------------------------------------------------------------------
//                         ErrorModelFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen;  NASA/GSFC.
// Created: 2015/01/07
//
/**
 * Factory used to create ErrorModel objects.
 */
//------------------------------------------------------------------------------


#ifndef ErrorModelFactory_hpp
#define ErrorModelFactory_hpp

#include "estimation_defs.hpp"
#include "Factory.hpp"
#include "ErrorModel.hpp"


class ESTIMATION_API ErrorModelFactory : public Factory
//class GMAT_API ErrorModelFactory : public Factory
{
public:
   ErrorModelFactory();
   virtual ~ErrorModelFactory();

   ErrorModelFactory(StringArray createList);
   ErrorModelFactory(const ErrorModelFactory& fact);
   ErrorModelFactory& operator= (const ErrorModelFactory& fact);

   virtual GmatBase *CreateObject(const std::string &ofType,
                                const std::string &withName = "");

   virtual ErrorModel *CreateErrorModel(const std::string &ofType,
         const std::string &withName = "");
};

#endif /* ErrorModelFactory_hpp */
