//$Id$
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
#include "GmatBase.hpp"
#include <map>

// Forward references for GMAT core objects
class MathNode;
class ElementWrapper;
class Function;
class FunctionManager;
class SolarSystem;
class PhysicalModel;
class CoordinateSystem;

class GMAT_API MathTree : public GmatBase
{
public:
   
   MathTree(const std::string &typeStr, const std::string &nomme);
   virtual ~MathTree();
   MathTree(const MathTree& mt);
   MathTree& operator=(const MathTree& mt);
   
   const StringArray&   GetGmatFunctionNames();
   void                 SetGmatFunctionNames(StringArray funcList);
   
   std::vector<Function*> GetFunctions() const;
   void                 SetFunction(Function *function);
   
   virtual void         SetCallingFunction(FunctionManager *fm);
   
   MathNode*            GetTopNode();
   void                 SetTopNode(MathNode *node);
   void                 SetMathWrappers(WrapperMap *wrapperMap);
   
   Real                 Evaluate();
   Rmatrix              MatrixEvaluate();
   bool                 Initialize(ObjectMap *objectMap,
                                   ObjectMap *globalObjectMap);
   void                 Finalize();
   void                 GetOutputInfo(Integer &type, Integer &rowCount,
                                      Integer &colCount);
   
   // for setting objects to FunctionRunner
   void                 SetObjectMap(ObjectMap *map);
   void                 SetGlobalObjectMap(ObjectMap *map);
   void                 SetSolarSystem(SolarSystem *ss);
   void                 SetInternalCoordSystem(CoordinateSystem *cs);
   void                 SetTransientForces(std::vector<PhysicalModel*> *tf);
   
   // Inherited (GmatBase) methods
   virtual GmatBase*    Clone(void) const;
   
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
protected:
   
   /// Top node of the math tree
   MathNode   *theTopNode;
   
   /// Object store obtained from the Sandbox
   ObjectMap  *theObjectMap;
   /// Global object store obtained from the Sandbox
   ObjectMap  *theGlobalObjectMap;
   /// Wrapper name and ElementWrapper pointer Map for RHS math element
   WrapperMap *theWrapperMap;
   
   /// All parameter name array
   StringArray theParamArray;
   StringArray theAllParamArray;
   StringArray theGmatFunctionNames;
   std::vector<Function*> theFunctions;
   
   bool InitializeParameter(MathNode *node);
   void FinalizeFunctionRunner(MathNode *node);
   void SetMathElementWrappers(MathNode *node);
   void SetFunctionToRunner(MathNode *node, Function *function);
   void SetCallingFunctionToRunner(MathNode *node, FunctionManager *fm);
   void SetObjectMapToRunner(MathNode *node, ObjectMap *map);
   void SetGlobalObjectMapToRunner(MathNode *node, ObjectMap *map);
   void SetSolarSystemToRunner(MathNode *node, SolarSystem *ss);
   void SetInternalCoordSystemToRunner(MathNode *node, CoordinateSystem *cs);
   void SetTransientForcesToRunner(MathNode *node, std::vector<PhysicalModel*> *tf);
   bool RenameParameter(MathNode *node, const Gmat::ObjectType type,
                        const std::string &oldName, const std::string &newName);
   void CreateParameterNameArray(MathNode *node);
   
};

#endif /*MATHTREE_HPP_*/
