//$Id$
//------------------------------------------------------------------------------
//                              MathFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Linda Jun, NASA/GSFC
// Created: 2006/04/05
//
/**
 * Implementation code for the MathFactory class, responsible for creating 
 * MathNode objects.
 */
//------------------------------------------------------------------------------
#ifndef MathFactory_hpp
#define MathFactory_hpp

#include "MathNode.hpp"
#include "Factory.hpp"

class GMAT_API MathFactory : public Factory
{
public:
   MathNode* CreateMathNode(const std::string &ofType,
                            const std::string &withName = "");
   
   MathFactory();
   virtual ~MathFactory();
   MathFactory(StringArray createList);
   MathFactory(const MathFactory &fact);
   MathFactory& operator= (const MathFactory &fact);
   
protected:
   void BuildCreatables();
   
};

#endif // MathFactory_hpp
