//$Header$
//------------------------------------------------------------------------------
//                                  GmatFunction
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2004/12/16
//
/**
 * Definition for the GmatFunction class.
 */
//------------------------------------------------------------------------------


#ifndef GmatFunction_hpp
#define GmatFunction_hpp

#include "Function.hpp"

class GmatFunction : public Function
{
public:
   GmatFunction(const std::string &nomme);

   virtual ~GmatFunction(void);

   GmatFunction(const GmatFunction &);
   GmatFunction& operator=(const GmatFunction&);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;

};

#endif
