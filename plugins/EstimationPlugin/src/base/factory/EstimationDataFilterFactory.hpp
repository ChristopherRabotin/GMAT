//$Id: EstimationDataFilterFactory.hpp 1398 2015-03-03 15:37:37Z tdnguyen $
//------------------------------------------------------------------------------
//                         EstimationDataFilterFactory
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
// Author: Tuan Dang Nguyen, NASA/GSFC.
// Created: 2015/03/03
//
/**
 * Factory used to create EstimationDataFilter objects.
 */
//------------------------------------------------------------------------------


#ifndef EstimationDataFilterFactory_hpp
#define EstimationDataFilterFactory_hpp

#include "estimation_defs.hpp"
#include "Factory.hpp"


class ESTIMATION_API EstimationDataFilterFactory : public Factory
{
public:
   EstimationDataFilterFactory();
   virtual ~EstimationDataFilterFactory();

   EstimationDataFilterFactory(StringArray createList);
   EstimationDataFilterFactory(const EstimationDataFilterFactory& fact);
   EstimationDataFilterFactory& operator= (const EstimationDataFilterFactory& fact);

   virtual GmatBase* CreateObject(const std::string &ofType,
         const std::string &withName = "");

   virtual DataFilter *CreateDataFilter(const std::string &ofType,
         const std::string &withName = "");
};

#endif /* EstimationDataFilterFactory_hpp */
