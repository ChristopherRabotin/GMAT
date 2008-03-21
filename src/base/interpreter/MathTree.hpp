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
#include "ElementWrapper.hpp"
#include <map>

class GMAT_API MathTree : public GmatBase
{
public:
   
   MathTree(const std::string &typeStr, const std::string &nomme);
   virtual ~MathTree();
   MathTree(const MathTree& mt);
   MathTree& operator=(const MathTree& mt);
   
   MathNode*         GetTopNode() { return theTopNode; }
   void              SetTopNode(MathNode *node) { theTopNode = node; }
   void              SetMathWrappers(std::map<std::string, ElementWrapper*> *wrapperMap);
   
   Real              Evaluate();
   Rmatrix           MatrixEvaluate();
   bool              Initialize(std::map<std::string, GmatBase *> *objectMap,
                                std::map<std::string, GmatBase *> *globalObjectMap);
   void              GetOutputInfo(Integer &type, Integer &rowCount,
                                   Integer &colCount);
   
   // Inherited (GmatBase) methods
   virtual GmatBase* Clone(void) const;
   
   virtual bool      RenameRefObject(const Gmat::ObjectType type,
                                  const std::string &oldName,
                                  const std::string &newName);
   virtual const StringArray&
                     GetRefObjectNameArray(const Gmat::ObjectType type);
protected:
   
   /// Top node of the math tree
   MathNode *theTopNode;
   
   /// Object store obtained from the Sandbox
   std::map<std::string, GmatBase *> *theObjectMap;
   /// Global object store obtained from the Sandbox
   std::map<std::string, GmatBase *> *theGlobalObjectMap;
   /// Wrapper name and ElementWrapper pointer Map for RHS math element
   std::map<std::string, ElementWrapper*> *theWrapperMap;
   
   /// All parameter name array
   StringArray theParamArray;
   StringArray theAllParamArray;
   
   bool InitializeParameter(MathNode *node);
   void SetMathElementWrappers(MathNode *node);
   bool RenameParameter(MathNode *node, const Gmat::ObjectType type,
                        const std::string &oldName, const std::string &newName);
   void CreateParameterNameArray(MathNode *node);
   
};

#endif /*MATHTREE_HPP_*/
