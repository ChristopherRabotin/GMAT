//------------------------------------------------------------------------------
//                              Add
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: LaMont Ruley
// Created: 2006/03/31
//
/**
 * Declares Add class.
 */
//------------------------------------------------------------------------------
#ifndef Add_hpp
#define Add_hpp

#include "MathFunction.hpp"

class GMAT_API Add : public MathFunction
{
public:
   Add(const std::string &typeStr, const std::string &nomme);
   virtual ~Add();
   Add(const Add &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   virtual Real Evaluate();

protected:

  
private:
   
};

#endif // Add_hpp
