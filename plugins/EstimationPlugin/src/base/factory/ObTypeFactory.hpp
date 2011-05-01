//$Id: ObTypeFactory.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         ObTypeFactory
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
// Created: 2009/06/19
//
/**
 * Factory used to create ObType objects
 */
//------------------------------------------------------------------------------


#ifndef OBTYPEFACTORY_HPP_
#define OBTYPEFACTORY_HPP_

#include "estimation_defs.hpp"
#include "Factory.hpp"
#include "ObType.hpp"

/// Descriptor here
class ESTIMATION_API ObTypeFactory : public Factory
{
public:
   ObTypeFactory();
   virtual ~ObTypeFactory();

   ObTypeFactory(StringArray createList);
   ObTypeFactory(const ObTypeFactory& fact);
   ObTypeFactory& operator= (const ObTypeFactory& fact);

   virtual ObType *CreateObType(const std::string &ofType,
         const std::string &withName = "");
};

#endif /* OBTYPEFACTORY_HPP_ */
