//$Header$
//------------------------------------------------------------------------------
//                                  FunctionManager
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CCA54C
//
// Author: Wendy C. Shoan
// Created: 2008.03.19
//
/**
 * Definition for the FunctionManager class
 */
//------------------------------------------------------------------------------


#ifndef FunctionManager_hpp
#define FunctionManager_hpp


#include <map>              // for mapping between object names and types
#include <algorithm>        // for find()
#include "gmatdefs.hpp"

// Headers used by commands that override InterpretAction
#include "Function.hpp"
#include "StringUtil.hpp"
#include "TextParser.hpp"
#include "SolarSystem.hpp"
#include "PhysicalModel.hpp"
#include "Validator.hpp"


/**
 * GMAT FunctionManager Class, used for managing Function calls from a
 * CallFunction command or from a FunctionRunner Math Node.
 *
 */
class GMAT_API FunctionManager
{
public:
   // class constructor
   FunctionManager();
   // class destructor
   virtual ~FunctionManager();
   // Copy constructor
   FunctionManager(const FunctionManager &fm);
   // Assignment operator
   FunctionManager&         operator=(const FunctionManager &fm);
   
   
   virtual void         SetObjectMap(std::map<std::string, GmatBase *> *map);
   virtual void         SetGlobalObjectMap(std::map<std::string, GmatBase *> *map);
   virtual void         SetSolarSystem(SolarSystem *ss);
   virtual void         SetTransientForces(std::vector<PhysicalModel*> *tf);
   virtual void         SetFunctionName(const std::string &itsName);
   virtual std::string  GetFunctionName() const;
   virtual void         SetFunction(Function *theFunction);
   virtual Function*    GetFunction() const;
   virtual void         AddInput(const std::string &withName, Integer atIndex = -999);
   virtual void         AddOutput(const std::string &withName, Integer atIndex = -999);
   virtual void         SetInputs(const StringArray &inputs);
   virtual void         SetOutputs(const StringArray &outputs);
   
   // Sequence methods
   //virtual bool         Initialize();
   virtual bool         Execute();
   virtual Real         Evaluate();
   virtual Rmatrix      MatrixEvaluate();

protected:
   
   /// Object store for the Function 
   std::map<std::string, GmatBase *>
                        functionObjectStore;
   /// Object store obtained from the caller
   std::map<std::string, GmatBase *>
                        *localObjectStore;
   /// Object store obtained from the Sandbox
   std::map<std::string, GmatBase *>
                        *globalObjectStore;
   // Combined object store, used by Validator
   std::map<std::string, GmatBase *>
                        *combinedObjectStore;
   /// Solar System, set by the local Sandbox, to pass to the function
   SolarSystem          *solarSys;
   /// transient forces to pass to the function
   std::vector<PhysicalModel *> 
                        *forces;
   /// Name of the function this FunctionManager manages
   std::string          fName;
   /// the function that this FunctionManager manages
   Function             *f;
   /// the list of input strings for this call of the function
   StringArray          ins;
   /// the list of output strings for this call of the function
   StringArray          outs;
   /// Saved input wrappers - need to save in order to resend to function at each call
   /// (since the same function may be called from many places)
   std::vector<ElementWrapper *> 
                        inputWrappers;
   /// wrappers for the output objects
   std::vector<ElementWrapper *> 
                        outputWrappers;
   /// flag indicating whether or not its the first execution
   bool                 firstExecution;
   /// Output Objects
   //ObjectArray          outObjects; 
   // Validator used to create the ElementWrappers
   Validator            validator;
   
   //virtual bool         BuildFunctionObjectStore();
   //virtual bool         RefreshFunctionObjectStore();
   GmatBase* FindObject(const std::string &name);
   
};

#endif // FunctionManager_hpp
