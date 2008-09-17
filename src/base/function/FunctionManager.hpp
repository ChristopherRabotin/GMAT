//$Id$
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
#include <stack>            // for objectMap and caller stacks
#include <algorithm>        // for find()
#include "gmatdefs.hpp"

// Headers used by commands that override InterpretAction
#include "Function.hpp"
#include "StringUtil.hpp"
#include "TextParser.hpp"
#include "SolarSystem.hpp"
#include "PhysicalModel.hpp"
#include "Validator.hpp"
#include "ObjectInitializer.hpp"


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
   virtual StringArray  GetOutputs();
   virtual void         SetInternalCoordinateSystem(CoordinateSystem *internalCS);
   
   // Sequence methods
   virtual bool         Execute(FunctionManager *callingFM = NULL);
   virtual Real         Evaluate(FunctionManager *callingFM = NULL);
   virtual Rmatrix      MatrixEvaluate(FunctionManager *callingFM = NULL);
   virtual void         Finalize();

   //void                 SetCallingFunction(FunctionManager *fm);
   ObjectMap*           PushToStack();
   void                 PopFromStack(ObjectMap* cloned, const StringArray &outNames, 
                                     const StringArray &callingNames);

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
                        combinedObjectStore;
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
   /// wrappers for the input objects
   std::vector<ElementWrapper *> 
                        outputWrappers;
   // wrappers for the output objects
   std::map<std::string, ElementWrapper *> 
                        inputWrappers;
   /// flag indicating whether or not its the first execution
   bool                 firstExecution;
   // number of Variables created for the FOS
   Integer              numVarsCreated;
   /// Output Objects
   //ObjectArray          outObjects; 
   // Validator used to create the ElementWrappers
   Validator            *validator;
   // Created objects for string or numeric literal inputs
   std::map<std::string, GmatBase *>
                        createdLiterals;
   // Created objects that need to be reevaluated each time (i.e. they are based on
   // current values of some object (e.g. an array element))
   std::map<std::string, GmatBase *>
                        createdOthers;
   /// Real value output
   Real                 realResult;
   /// Rmatrix value output
   Rmatrix              matResult;
   /// Flag indicating whether or not there is one nameless result (i.e. called from FunctionRunner)
   bool                 blankResult;
   /// Which type of output was saved last - real or rmatrix?
   std::string          outputType;
   /// Object needed to initialize the FOS objects
   ObjectInitializer    *objInit;
   /// the internal coordinate system
   CoordinateSystem     *intCS;
   /// pointer to the function's function control sequence
   GmatCommand          *fcs;
   /// current command being executed
   GmatCommand          *current;
   /// Call stack of FOSs for nested/recursive function calls
   ObjectMapStack       callStack;
   /// Call stack of LOSs for nested/recursive function calls
   ObjectMapStack       losStack;
   /// pointers to the calling function of this function (needs to be a stack 
   // because we need to be able to handle nested and recursive functions)
   std::stack<FunctionManager*> callers;
   // pointer to the current calling function
   FunctionManager      *callingFunction;
   
   virtual bool         Initialize();
   GmatBase*            FindObject(const std::string &name, bool arrayElementsAllowed = false);
   GmatBase*            CreateObject(const std::string &fromString);
   void                 SaveLastResult();
   
   bool                 EmptyObjectMap(ObjectMap *om);  
   bool                 DeleteObjectMap(ObjectMap *om);
   bool                 IsOnStack(ObjectMap *om);
   
   void                 ShowObjectMap(ObjectMap *om, const std::string &mapID = "");
   void                 ShowStackContents(ObjectMapStack omStack, const std::string &stackID = "");
   
};

#endif // FunctionManager_hpp
