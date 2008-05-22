//$Id$
//------------------------------------------------------------------------------
//                              FunctionRunner
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
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
#include "RealUtilities.hpp"

class GMAT_API FunctionRunner : public MathFunction
{
public:
   FunctionRunner(const std::string &nomme);
   virtual ~FunctionRunner();
   FunctionRunner(const FunctionRunner &copy);
   
   void                 SetFunctionName(const std::string &fname);
   void                 SetFunction(Function *function);
   void                 AddFunctionInput(const std::string &name);
   void                 AddFunctionOutput(const std::string &name);
   void                 SetFunctionInputs();
   void                 SetFunctionOutputs();
   const StringArray&   GetInputs();
   
   // for string to object maps
   virtual void         SetObjectMap(ObjectMap *map);
   virtual void         SetGlobalObjectMap(ObjectMap *map);
   
   // inherited from MathFunction
   virtual void         GetOutputInfo(Integer &type, Integer &rowCount, 
                                      Integer &colCount);
   virtual bool         ValidateInputs(); 
   virtual Real         Evaluate();
   virtual Rmatrix      MatrixEvaluate();
   
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
   
   GmatBase* FindObject(const std::string &name);
};

#endif // FunctionRunner_hpp
