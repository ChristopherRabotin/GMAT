//------------------------------------------------------------------------------
//                              ASin
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: LaMont Ruley
// Created: 2006/04/10
//
/**
 * Declares ASin class.
 */
//------------------------------------------------------------------------------
#ifndef ASin_hpp
#define ASin_hpp

#include "MathFunction.hpp"
#include "RealUtilities.hpp"

class GMAT_API ASin : public MathFunction
{
public:
   ASin(const std::string &nomme);
   virtual ~ASin();
   ASin(const ASin &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   virtual Real Evaluate();

protected:

  
private:
   
};

#endif // ASin_hpp
