//------------------------------------------------------------------------------
//                              ATan
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
 * Declares ATan class.
 */
//------------------------------------------------------------------------------
#ifndef ATan_hpp
#define ATan_hpp

#include "MathFunction.hpp"
#include "RealUtilities.hpp"

class GMAT_API ATan : public MathFunction
{
public:
   ATan(const std::string &nomme);
   virtual ~ATan();
   ATan(const ATan &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   virtual Real Evaluate();

protected:
 
private:
   
};

#endif // ATan_hpp
