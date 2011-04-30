//$Id$
//------------------------------------------------------------------------------
//                         CalculatedPointFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy Shoan
// Created: 2005/04/06
//
/**
*  This class is the factory class for CalculatedPoints.
 */
//------------------------------------------------------------------------------
#ifndef CalculatedPointFactory_hpp
#define CalculatedPointFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "CalculatedPoint.hpp"

class GMAT_API CalculatedPointFactory : public Factory
{
public:
   CalculatedPoint*  CreateCalculatedPoint(const std::string &ofType,
                                           const std::string &withName = "");

   // default constructor
   CalculatedPointFactory();
   // constructor
   CalculatedPointFactory(const StringArray &createList);
   // copy constructor
   CalculatedPointFactory(const CalculatedPointFactory &fact);
   // assignment operator
   CalculatedPointFactory& operator= (const CalculatedPointFactory &fact);

   // destructor
   ~CalculatedPointFactory();

protected:
      // protected data

private:
      // private data


};

#endif // CalculatedPointFactory_hpp




