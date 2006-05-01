//$Header$
//------------------------------------------------------------------------------
//                              MathFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
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

#ifdef __UNIT_TEST__
class GMAT_API MathFactory
#else
#include "Factory.hpp"
class GMAT_API MathFactory : public Factory
#endif
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
   
   #ifdef __UNIT_TEST__
   StringArray creatables;
   bool isCaseSensitive;
   #endif
};

#endif // MathFactory_hpp
