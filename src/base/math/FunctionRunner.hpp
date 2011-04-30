//$Id$
//------------------------------------------------------------------------------
//                              FunctionRunner
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CCA54C
//
// Author: Wendy Shoan
// Created: 2008.04.21
//
/**
 * Declares FunctionRunner class.
 */
//------------------------------------------------------------------------------
#ifndef FunctionRunner_hpp
#define FunctionRunner_hpp

#include "MathFunction.hpp"
#include "FunctionManager.hpp"
#include "SolarSystem.hpp"
#include "PhysicalModel.hpp"
#include "RealUtilities.hpp"

typedef std::vector<MathNode*> MathNodeArray;

class GMAT_API FunctionRunner : public MathFunction
{
public:
   FunctionRunner(const std::string &nomme);
   virtual ~FunctionRunner();
   FunctionRunner(const FunctionRunner &copy);

   // for Function
   void                 SetFunctionName(const std::string &fname);
   void                 SetFunction(Function *function);
   
   // for Function input
   void                 AddFunctionInput(const std::string &name);
   void                 SetFunctionInputs();
   const StringArray&   GetInputs();
   void                 AddInputNode(MathNode *node);
   const MathNodeArray& GetInputNodes();
   
   // for Function output
   void                 AddFunctionOutput(const std::string &name);
   void                 SetFunctionOutputs();
   
   // for calling function
   virtual void         SetCallingFunction(FunctionManager *fm);
   
   // for setting objects to FunctionManager
   virtual void         SetObjectMap(ObjectMap *map);
   virtual void         SetGlobalObjectMap(ObjectMap *map);
   void                 SetSolarSystem(SolarSystem *ss);
   void                 SetInternalCoordSystem(CoordinateSystem *cs);
   void                 SetTransientForces(std::vector<PhysicalModel*> *tf);
   void                 SetPublisher(Publisher *pub);
   
   // inherited from MathFunction
   virtual void         GetOutputInfo(Integer &type, Integer &rowCount, 
                                      Integer &colCount);
   virtual bool         ValidateInputs(); 
   virtual Real         Evaluate();
   virtual Rmatrix      MatrixEvaluate();
   virtual void         Finalize();
   
   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   
protected:
   
   FunctionManager theFunctionManager;
   ObjectMap       *theObjectMap;
   ObjectMap       *theGlobalObjectMap;
   std::string     theFunctionName;
   Function        *theFunction;
   StringArray     theInputNames;
   StringArray     theOutputNames;
   MathNodeArray   theInputNodes;
   FunctionManager *callingFunction;
   
   CoordinateSystem *internalCS;
   
   GmatBase* FindObject(const std::string &name);
   void      HandlePassingMathExp(Function *function);
};

#endif // FunctionRunner_hpp
