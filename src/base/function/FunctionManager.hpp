//$Id$
//------------------------------------------------------------------------------
//                                  FunctionManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
   FunctionManager&     operator=(const FunctionManager &fm);
      
   virtual void         SetPublisher(Publisher *pub);
   virtual Publisher*   GetPublisher();
   
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
   virtual void         SetInternalCoordinateSystem(CoordinateSystem *intCS);
   
   virtual bool         SetInputWrapper(Integer index, ElementWrapper *ew);
   virtual ElementWrapper* GetInputWrapper(Integer index);
   virtual bool         SetPassedInput(Integer index, GmatBase *obj, bool &inputAdded);
   virtual WrapperArray& GetWrappersToDelete();
   
   // Sequence methods
   bool                 PrepareObjectMap();
   virtual bool         Initialize();
   virtual bool         Execute(FunctionManager *callingFM = NULL);
   virtual Real         Evaluate(FunctionManager *callingFM = NULL);
   virtual Rmatrix      MatrixEvaluate(FunctionManager *callingFM = NULL);
   virtual void         Finalize();
   bool                 IsFinalized();
   
   //void                 SetCallingFunction(FunctionManager *fm);
   ObjectMap*           PushToStack();
   bool                 PopFromStack(ObjectMap* cloned, const StringArray &outNames, 
                                     const StringArray &callingNames);

protected:
   
   /// Publisher for data generation
   Publisher            *publisher;
   /// Object store for the Function 
   ObjectMap            *functionObjectStore;
   /// Object store obtained from the caller
   ObjectMap            *localObjectStore;
   /// Object store obtained from the Sandbox
   ObjectMap            *globalObjectStore;
   /// Combined object store, used by Validator
   ObjectMap            combinedObjectStore;
   /// Object stores cloned when push to stack
   std::vector<ObjectMap *> clonedObjectStores;
   /// Solar System, set by the local Sandbox, to pass to the function
   SolarSystem          *solarSys;
   /// transient forces to pass to the function
   std::vector<PhysicalModel *> 
                        *forces;
   /// Name of the function this FunctionManager manages
   std::string          fName;
   /// the function that this FunctionManager manages
   Function             *f;
   /// the list of passing input strings for this call of the function
   StringArray          passedIns;
   /// the list of passing output strings for this call of the function
   StringArray          passedOuts;
   /// wrappers for the outut objects
   WrapperArray         outputWrappers;
   // wrapper map for the input objects
   WrapperMap           inputWrapperMap;
   /// flag indicating whether or not it's the first execution
   bool                 firstExecution;
   /// flag indicating whether or not FunctionManager is finalized
   bool                 isFinalized;
   // number of Variables created for the FOS
   Integer              numVarsCreated;
   /// Output Objects
   //ObjectArray          outObjects; 
   // Validator used to create the ElementWrappers
   Validator            *validator;
   // Created objects for string or numeric literal inputs
   ObjectMap            createdLiterals;
   // Created objects that need to be reevaluated each time (i.e. they are based on
   // current values of some object (e.g. an array element))
   ObjectMap            createdOthers;
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
   CoordinateSystem     *internalCS;
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
   void                 PrepareExecution(FunctionManager *callingFM = NULL);
   bool                 ValidateFunctionArguments();
   bool                 CreatePassingArgWrappers();
   void                 RefreshFOS();
   void                 RefreshFormalInputObjects();
   GmatBase*            FindObject(const std::string &name, bool arrayElementsAllowed = false);
   GmatBase*            CreateObject(const std::string &fromString);
   void                 AssignResult();
   bool                 HandleCallStack();
   void                 SaveLastResult();
   
   void                 Cleanup();
   void                 UnsubscribeSubscribers(ObjectMap *om);
   bool                 EmptyObjectMap(ObjectMap *om, const std::string &mapID = "");  
   bool                 DeleteObjectMap(ObjectMap *om, const std::string &mapID = "");
   bool                 ClearInOutWrappers();
   bool                 CloneObjectMap(ObjectMap *orig, ObjectMap *cloned);
   bool                 CopyObjectMap(ObjectMap *from, ObjectMap *to);
   bool                 IsOnStack(ObjectMap *om);
   
   // for debug
   void                 ShowObjectMap(ObjectMap *om, const std::string &mapID = "");
   void                 ShowStackContents(ObjectMapStack omStack, const std::string &stackID = "");
   void                 ShowCallers(const std::string &label = "");
   void                 ShowTrace(Integer count, Integer t1, const std::string &label = "",
                                  bool showMemoryTracks = false, bool addEol = false);
};

#endif // FunctionManager_hpp
