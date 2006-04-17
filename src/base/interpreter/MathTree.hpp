//$Header$
//------------------------------------------------------------------------------
//                                   MathTree
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Allison Greene
// Created: 2006/04/17
//
/**
 * Defines the MathTree base class used for Math in Scripts.
 */
//------------------------------------------------------------------------------
#ifndef MATHTREE_HPP_
#define MATHTREE_HPP_

#include "gmatdefs.hpp"
#include "MathNode.hpp"
#include <map>

class GMAT_API MathTree : public GmatBase
{
public:
   MathTree(const std::string &typeStr, const std::string &nomme);
   virtual ~MathTree();
   MathTree(const MathTree& mt);
   MathTree&         operator=(const MathTree& mt);
   
   // Inherited (GmatBase) methods
   virtual GmatBase*    Clone(void) const;
   
   Real              Evaluate();
   Rmatrix           MatrixEvaluate();
   bool              Initialize(std::map<std::string, GmatBase *> *objectMap);
   void              ReportOutputs(Integer &type, Integer &rowCount,
                            Integer &colCount);
   
protected:
   MathNode *topNode;
};

#endif /*MATHTREE_HPP_*/
