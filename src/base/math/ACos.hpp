//------------------------------------------------------------------------------
//                              ACos
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
 * Declares ACos class.
 */
//------------------------------------------------------------------------------
#ifndef ACos_hpp
#define ACos_hpp

#include "MathFunction.hpp"
#include "RealUtilities.hpp"

class GMAT_API ACos : public MathFunction
{
public:
   ACos(const std::string &nomme);
   virtual ~ACos();
   ACos(const ACos &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   virtual Real Evaluate();

protected:

private:
   
};

#endif // ACos_hpp
