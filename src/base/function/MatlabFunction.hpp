//$Header$
//------------------------------------------------------------------------------
//                                  MatlabFunction
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2004/09/22
//
/**
 * Definition for the MatlabFunction class.
 */
//------------------------------------------------------------------------------


#ifndef MatlabFunction_hpp
#define MatlabFunction_hpp

#include "Function.hpp"

class MatlabFunction : public Function
{
public:
   MatlabFunction(const std::string &nomme);

   virtual ~MatlabFunction(void);

   MatlabFunction(const MatlabFunction &);
   MatlabFunction& operator=(const MatlabFunction&);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;

};

#endif
