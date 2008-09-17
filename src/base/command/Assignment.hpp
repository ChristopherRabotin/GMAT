//$Id$
//------------------------------------------------------------------------------
//                                Assignment
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/03
//
/**
 * Definition of the Assignment command class.
 * 
 * The assignment class is the Command class that handles commands of the form
 * 
 *     GMAT object.parameter = value;
 *     GMAT variable = parameter;
 *     GMAT variable = equation;
 * 
 */
//------------------------------------------------------------------------------
#ifndef Assignment_hpp
#define Assignment_hpp

#include "gmatdefs.hpp"
#include "GmatCommand.hpp"
#include "MathTree.hpp"
#include "Function.hpp"

class GMAT_API Assignment : public GmatCommand
{
public:
   Assignment();
   virtual ~Assignment();
   Assignment(const Assignment& a);
   Assignment&          operator=(const Assignment& a);
   
   MathTree*            GetMathTree();
   virtual bool         HasAFunction();
   const StringArray&   GetGmatFunctionNames();
   void                 SetMathWrappers();
   
   std::string          GetLHS() { return lhs; }
   std::string          GetRHS() { return rhs; }
   
   virtual void         SetFunction(Function *function);
   virtual std::vector<Function*> GetFunctions() const;
   
   // inherited from GmatCommand
   virtual void         SetSolarSystem(SolarSystem *ss);
   virtual void         SetInternalCoordSystem(CoordinateSystem *cs);
   virtual void         SetTransientForces(std::vector<PhysicalModel*> *tf);
   virtual void         SetObjectMap(ObjectMap *map);
   virtual void         SetGlobalObjectMap(ObjectMap *map);
   
   virtual bool         InterpretAction();
   virtual bool         Initialize();
   virtual bool         Execute();
   virtual void         RunComplete();
   virtual bool         SkipInterrupt();
   virtual void         SetCallingFunction(FunctionManager *fm);
   
   virtual const StringArray& 
                        GetWrapperObjectNameArray();
   virtual bool         SetElementWrapper(ElementWrapper* toWrapper,
                                          const std::string &withName);
   virtual void         ClearWrappers();
   
   // inherited from GmatBase
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   virtual GmatBase*    Clone() const;
   
   virtual const std::string&
                        GetGeneratingString(
                           Gmat::WriteMode mode = Gmat::SCRIPTING,
                           const std::string &prefix = "",
                           const std::string &useName = "");

protected:
   
   /// string on the left side of the equals sign
   std::string          lhs;
   /// string on the right side of the equals sign
   std::string          rhs;
   /// ElementWrapper pointer for the lhs of the equals sign
   ElementWrapper*      lhsWrapper;
   /// ElementWrapper pointer for the rhs of the equals sign
   ElementWrapper*      rhsWrapper;
   /// MathNode pointer for RHS equation
   MathTree             *mathTree;
   /// Wrapper name and ElementWrapper pointer Map for RHS math element
   WrapperMap           mathWrapperMap;
   
   // methods
   ElementWrapper* RunMathTree(ElementWrapper *lhsWrapper);
};

#endif // Assignment_hpp
