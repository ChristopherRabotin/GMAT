//$Header$
//------------------------------------------------------------------------------
//                                   MathElement
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Waka Waktola
// Created: 2006/04/04
//
/**
 * Defines the Math elements class for Math in scripts.
 */
//------------------------------------------------------------------------------

#ifndef MathElement_hpp
#define MathElement_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "MathNode.hpp"
#include "MathException.hpp"
#include "Parameter.hpp"
#include "Array.hpp"

class MathElement : public MathNode
{
public:
   MathElement(const std::string &typeStr, const std::string &nomme);
   virtual ~MathElement();
   MathElement(const MathElement &me);
   MathElement& operator=(const MathElement &me);
   
   // Inherited (GmatBase) methods
   virtual GmatBase* Clone(void) const; 
   virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name);
   virtual bool      SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                  const std::string &name = "");
   virtual std::string 
                     GetRefObjectName(const Gmat::ObjectType type) const;
   virtual bool      SetRefObjectName(const Gmat::ObjectType type,
                                      const std::string &name);
   virtual const     StringArray& GetRefObjectNameArray(const Gmat::ObjectType type);
   
   // Inherited (MathNode) methods                                   
   virtual void SetMatrixValue(const Rmatrix &mat);
   virtual bool ValidateInputs(); 
   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   
   virtual bool SetChildren(MathNode *leftChild, MathNode *rightChild);
   virtual MathNode* GetLeft();
   virtual MathNode* GetRight();
   
   virtual Real Evaluate();
   virtual Rmatrix MatrixEvaluate();

protected:
   
   /// A pointer to the referenced object (i.e. the leaf node or element).  
   /// This pointer is set when the MathTree is initialized in the Sandbox.
   Parameter* refObject;
   
   /// Holds the name of the GMAT object that is accessed by this node
   std::string refObjectName; 
   
   std::string refObjectType;
      
};

#endif //MathElement_hpp
