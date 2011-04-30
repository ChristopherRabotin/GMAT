//$Id$
//------------------------------------------------------------------------------
//                                 FunctionManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy C. Shoan
// FunctionManagerd: 2008.03.24
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CCA54C
//
/**
 * Class implementation for the FunctionManager command
 */
//------------------------------------------------------------------------------
#include <stack>

#include "FunctionManager.hpp"
#include "MessageInterface.hpp"
#include "FunctionException.hpp"
#include "StringUtil.hpp"
#include "Array.hpp"
#include "ElementWrapper.hpp"
#include "NumberWrapper.hpp"
#include "RealUtilities.hpp"
#include "Variable.hpp"
#include "StringVar.hpp"

//#define DO_NOT_EXECUTE_NESTED_GMAT_FUNCTIONS

// Deleting subscribers needs more testing but try
//#define __DO_NOT_DELETE_SUBSCRIBERS__

//#define DEBUG_FUNCTION_MANAGER
//#define DEBUG_FM_SET
//#define DEBUG_FM_INIT
//#define DEBUG_FM_REFRESH
//#define DEBUG_FM_EVAL
//#define DEBUG_FM_EXECUTE
//#define DEBUG_FM_RESULT
//#define DEBUG_FM_FINALIZE
//#define DEBUG_FM_STACK
//#define DEBUG_OBJECT_MAP
//#define DEBUG_WRAPPERS
//#define DEBUG_CLEANUP

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif
//#ifndef DEBUG_TRACE
//#define DEBUG_TRACE
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif
#ifdef DEBUG_TRACE
#include <ctime>                 // for clock()
#endif

//---------------------------------
// static data
//---------------------------------

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// FunctionManager()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
FunctionManager::FunctionManager() :
   publisher           (NULL),
   functionObjectStore (NULL),
   localObjectStore    (NULL),
   globalObjectStore   (NULL),
   solarSys            (NULL),
   fName               (""),
   f                   (NULL),
   firstExecution      (true),
   isFinalized         (false),
   numVarsCreated      (0),
   validator           (NULL),
   realResult          (-999.99),
   blankResult         (false),
   outputType          (""),
   objInit             (NULL),
   internalCS          (NULL), 
   fcs                 (NULL),
   current             (NULL),
   callingFunction     (NULL)
{
   #ifdef DEBUG_FUNCTION_MANAGER
   MessageInterface::ShowMessage("FM default constructor entered, this=<%p>\n", this);
   #endif
}


//------------------------------------------------------------------------------
// ~FunctionManager()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
FunctionManager::~FunctionManager()
{
   #ifdef DEBUG_FUNCTION_MANAGER
   MessageInterface::ShowMessage("FM destructor entered, this=<%p>\n", this);
   #endif
   
   if (objInit)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (objInit, "objInit", "FunctionManager::~FunctionManager()");
      #endif
      delete objInit;
      objInit = NULL;
   }
   
   DeleteObjectMap(functionObjectStore, "FOS in Destructor");
   functionObjectStore = NULL;
   ClearInOutWrappers();
}


//------------------------------------------------------------------------------
// FunctionManager(const FunctionManager &fm)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param <fm> The instance that gets copied.
 */
//------------------------------------------------------------------------------
FunctionManager::FunctionManager(const FunctionManager &fm) :
   publisher           (fm.publisher),
   functionObjectStore (NULL),
   localObjectStore    (NULL),
   globalObjectStore   (fm.globalObjectStore),
   solarSys            (fm.solarSys),
   forces              (fm.forces),
   fName               (fm.fName),
   f                   (fm.f), // copy the pointer here
   passedIns           (fm.passedIns),
   passedOuts          (fm.passedOuts),
   //inputWrapperMap       (fm.inputWrapperMap),
   outputWrappers      (fm.outputWrappers), // is that right?
   firstExecution      (true),
   isFinalized         (false),
   numVarsCreated      (fm.numVarsCreated),
   validator           (NULL),
   realResult          (fm.realResult),
   matResult           (fm.matResult),
   blankResult         (fm.blankResult),
   outputType          (fm.outputType),
   objInit             (NULL),
   internalCS          (fm.internalCS), 
   fcs                 (NULL),
   current             (NULL),
   callingFunction     (NULL)
{
   #ifdef DEBUG_FUNCTION_MANAGER
   MessageInterface::ShowMessage("FM copy constructor entered, this=<%p>\n", this);
   #endif
}


//------------------------------------------------------------------------------
// FunctionManager& operator=(const FunctionManager &fm)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 * 
 * @param <fm> The command that gets copied.
 * 
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
FunctionManager& FunctionManager::operator=(const FunctionManager &fm)
{
   if (&fm != this)
   {
      publisher           = fm.publisher;
      functionObjectStore = NULL;
      globalObjectStore   = fm.globalObjectStore; // is that right?
      solarSys            = fm.solarSys;
      forces              = fm.forces;
      fName               = fm.fName;
      f                   = fm.f;  // copy the pointer here
      passedIns           = fm.passedIns;
      passedOuts          = fm.passedOuts;
      firstExecution      = true;
      isFinalized         = false;
      numVarsCreated      = fm.numVarsCreated;
      validator           = NULL;
      //inputWrapperMap       = fm.inputWrapperMap; // is that right?
      outputWrappers      = fm.outputWrappers; // is that right?
      realResult          = fm.realResult;
      matResult           = fm.matResult;
      blankResult         = fm.blankResult;
      outputType          = fm.outputType;
      objInit             = NULL;  
      internalCS          = fm.internalCS;  // right?
      fcs                 = NULL;
      current             = NULL;
      callingFunction     = NULL;
   }
   return *this;
}


//------------------------------------------------------------------------------
// virtual void SetPublisher(Publisher *pub)
//------------------------------------------------------------------------------
void FunctionManager::SetPublisher(Publisher *pub)
{
   #ifdef DEBUG_PUBLISHER
   MessageInterface::ShowMessage
      ("FunctionManager::SetPublisher() '%s' setting publisher <%p>\n",
       fName.c_str(), pub);
   #endif
   publisher = pub;
}


//------------------------------------------------------------------------------
// virtual Publisher* GetPublisher()
//------------------------------------------------------------------------------
Publisher* FunctionManager::GetPublisher()
{
   return publisher;
}


//------------------------------------------------------------------------------
// void SetObjectMap(std::map<std::string, GmatBase *> *map)
//------------------------------------------------------------------------------
void FunctionManager::SetObjectMap(std::map<std::string, GmatBase *> *map)
{
   localObjectStore = map;
   #ifdef DEBUG_OBJECT_MAP
   MessageInterface::ShowMessage
      ("FunctionManager::SetObjectMap() localObjectStore=<%p>\n", localObjectStore);
   ShowObjectMap(localObjectStore, "SetObjectMap()");
   #endif
}

//------------------------------------------------------------------------------
// void SetGlobalObjectMap(std::map<std::string, GmatBase *> *map)
//------------------------------------------------------------------------------
void FunctionManager::SetGlobalObjectMap(std::map<std::string, GmatBase *> *map)
{
   globalObjectStore = map;
}

//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
void FunctionManager::SetSolarSystem(SolarSystem *ss)
{
   #ifdef DEBUG_FM_SET
   MessageInterface::ShowMessage("FunctionManager::SetSolarSystem() ss=<%p>\n", ss);
   #endif
   solarSys = ss;
   if ((f) && (f->GetTypeName() == "GmatFunction"))   f->SetSolarSystem(ss);
}

//------------------------------------------------------------------------------
// void SetTransientForces(std::vector<PhysicalModel*> *tf)
//------------------------------------------------------------------------------
void FunctionManager::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
   forces = tf;
   if ((f) && (f->GetTypeName() == "GmatFunction"))   f->SetTransientForces(tf);
}

//------------------------------------------------------------------------------
// void SetFunctionName(const std::string &itsName)
//------------------------------------------------------------------------------
void FunctionManager::SetFunctionName(const std::string &itsName)
{
   #ifdef DEBUG_FUNCTION_MANAGER
      MessageInterface::ShowMessage("Entering FM::SetFunctionName with name = %s\n",
                                    itsName.c_str());
   #endif
   fName = itsName;
   if ((f) && (f->GetTypeName() == "GmatFunction"))  
   {
      #ifdef DEBUG_FUNCTION_MANAGER
         MessageInterface::ShowMessage("   and setting name to %s on the function\n",
                                       itsName.c_str());
      #endif
      f->SetStringParameter("FunctionName", itsName);
   }
}

//------------------------------------------------------------------------------
// std::string GetFunctionName() const
//------------------------------------------------------------------------------
std::string FunctionManager::GetFunctionName() const
{
   if (f)
   {
      std::string theFunctionName = f->GetStringParameter("FunctionName");
      #ifdef DEBUG_FM_GET
      MessageInterface::ShowMessage
         ("in FM::GetFunctionName, <%p> function name = '%s', "
          "fName = '%s'\n", f, theFunctionName.c_str(), fName.c_str());
      #endif
      //return f->GetStringParameter("FunctionName");
   }
   return fName; // why not return theFunctionName here?
}

//------------------------------------------------------------------------------
// void SetFunction(Function *theFunction)
//------------------------------------------------------------------------------
void FunctionManager::SetFunction(Function *theFunction)
{
   #ifdef DEBUG_FUNCTION_MANAGER
   MessageInterface::ShowMessage
      ("FunctionManager::SetFunction() fName='%s', theFunction=<%p>\n",
       fName.c_str(), theFunction);
   #endif
   f = theFunction;
   f->SetStringParameter("FunctionName", fName);
   if (f->IsOfType("GmatFunction"))
      fcs = f->GetFunctionControlSequence();
   else
   {
      std::string errMsg = "Function passed to FunctionManager \"";
      errMsg += fName + "\" is of wrong type; must be a GmatFunction\n"; // other types in the future?
      throw FunctionException(errMsg);
   }

}

//------------------------------------------------------------------------------
// Function* GetFunction() const
//------------------------------------------------------------------------------
Function* FunctionManager::GetFunction() const
{
   return f;
}

//------------------------------------------------------------------------------
// void AddInput(const std::string &withName, Integer atIndex)
//------------------------------------------------------------------------------
void FunctionManager::AddInput(const std::string &withName, Integer atIndex)
{
   // -999 means to put it at the end of the list (default value/behavior)
   if ((atIndex == -999) || (atIndex == (Integer) passedIns.size()))
   {
      passedIns.push_back(withName);
      return;
   }
   
   if ((atIndex < 0) || (atIndex > (Integer) passedIns.size()))
      throw FunctionException("FunctionManager:: input index out of range - unable to set.\n");
   // replace an entry
   passedIns.at(atIndex) = withName;   
}

//------------------------------------------------------------------------------
// void AddOutput(const std::string &withName, Integer atIndex)
//------------------------------------------------------------------------------
void FunctionManager::AddOutput(const std::string &withName, Integer atIndex)
{
   // -999 means to put it at the end of the list (default value/behavior)
   if ((atIndex == -999) || (atIndex == (Integer) passedOuts.size()))
   {
      passedOuts.push_back(withName);
      return;
   }
   
   if ((atIndex < 0) || (atIndex > (Integer) passedOuts.size()))
      throw FunctionException("FunctionManager:: input index out of range - unable to set.\n");
   // replace an entry
   passedOuts.at(atIndex) = withName;   
}

//------------------------------------------------------------------------------
// void SetInputs(const StringArray &inputs)
//------------------------------------------------------------------------------
void FunctionManager::SetInputs(const StringArray &inputs)
{
   passedIns = inputs;
}

//------------------------------------------------------------------------------
// void SetOutputs(const StringArray &outputs)
//------------------------------------------------------------------------------
void FunctionManager::SetOutputs(const StringArray &outputs)
{
   passedOuts = outputs;
}

//------------------------------------------------------------------------------
// StringArray GetOutputs()
//------------------------------------------------------------------------------
StringArray  FunctionManager::GetOutputs()
{
   return passedOuts;
}

//------------------------------------------------------------------------------
// void SetInternalCoordinateSystem(CoordinateSystem *intCS)
//------------------------------------------------------------------------------
void FunctionManager::SetInternalCoordinateSystem(CoordinateSystem *intCS)
{
   #ifdef DEBUG_FM_SET
   MessageInterface::ShowMessage
      ("in FM:SetInternalCoordinateSystem(), intCS=<%p>\n", intCS);
   #endif
   
   // if cs is NULL, just return (loj: 2008.10.07)
   if (intCS == NULL)
      return;
   
   internalCS = intCS;
   if ((f) && (f->GetTypeName() == "GmatFunction"))
      f->SetInternalCoordSystem(internalCS);
}


//------------------------------------------------------------------------------
// virtual bool SetInputWrapper(Integer index, ElementWrapper *ew)
//------------------------------------------------------------------------------
bool FunctionManager::SetInputWrapper(Integer index, ElementWrapper *ew)
{
   if (ew == NULL)
      return false;
   
   Integer numWrappers = inputWrapperMap.size();
   std::string formalInput = f->GetStringParameter("Input", index);
   
   #ifdef DEBUG_INPUT
   MessageInterface::ShowMessage
      ("FunctionManager::SetInputWrapper() entered, index=%d, numWrappers=%d, "
       "formalInput='%s'\n", index, numWrappers, formalInput.c_str());
   #endif
   
   if (index < 0 && index >= numWrappers)
   {
      #ifdef DEBUG_INPUT
      MessageInterface::ShowMessage
         ("FunctionManager::SetInputWrapper() returning false, "
          "index is out-of-bounds of inputWrapper size\n");
      #endif
      return false;
   }
   
   #ifdef DEBUG_INPUT
   MessageInterface::ShowMessage
      ("FunctionManager::SetInputWrapper() passed input string=<%s>\n",
       passedIns[index].c_str());
   #endif
   
   ElementWrapper *oldEw = inputWrapperMap[formalInput];
   inputWrapperMap[formalInput] = ew;
   if (oldEw)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (oldEw, oldEw->GetDescription(), "FunctionManager::SetInputWrapper()");
      #endif
      delete oldEw;
      oldEw = NULL;
   }
   
   #ifdef DEBUG_INPUT
   MessageInterface::ShowMessage
      ("FunctionManager::SetInputWrapper() returning true\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// virtual ElementWrapper* GetInputWrapper(Integer index)
//------------------------------------------------------------------------------
ElementWrapper* FunctionManager::GetInputWrapper(Integer index)
{
   Integer numWrappers = inputWrapperMap.size();
   std::string formalInput = f->GetStringParameter("Input", index);

   #ifdef DEBUG_INPUT
   MessageInterface::ShowMessage
      ("FunctionManager::GetInputWrapper() entered, index=%d, numWrappers=%d, "
       "formalInput='%s'\n", index, numWrappers, formalInput.c_str());
   #endif
   
   if (index < 0 && index >= numWrappers)
   {
      #ifdef DEBUG_INPUT
      MessageInterface::ShowMessage
         ("FunctionManager::GetInputWrapper() returning NULL, "
          "index is out-of-bounds\n");
      #endif
      return NULL;
   }
   
   #ifdef DEBUG_INPUT
   MessageInterface::ShowMessage
      ("FunctionManager::GetInputWrapper() passed input string=<%s>\n",
       passedIns[index].c_str());
   #endif
   
   if (inputWrapperMap.find(formalInput) == inputWrapperMap.end())
   {
      #ifdef DEBUG_INPUT
      MessageInterface::ShowMessage
         ("FunctionManager::GetInputWrapper() returning NULL, "
          "formal input name '%s' not found\n", formalInput.c_str());
      #endif
      return NULL;
   }
   
   ElementWrapper *ew = inputWrapperMap[formalInput];

   #ifdef DEBUG_INPUT
   MessageInterface::ShowMessage
      ("FunctionManager::GetInputWrapper() returning <%p><%s>, wrapperType=%d\n",
       ew, ew ? ew->GetDescription().c_str() : "NULL", ew->GetWrapperType());
   #endif
   
   if (ew->GetWrapperType() == Gmat::NUMBER_WT)
   {      
      #ifdef DEBUG_INPUT
      NumberWrapper *nw = (NumberWrapper*)ew;
      MessageInterface::ShowMessage
         ("FunctionManager::GetInputWrapper() ew value=%f\n", nw->EvaluateReal());
      #endif
   }
   
   return ew;
}


//------------------------------------------------------------------------------
// virtual bool SetPassedInput(Integer index, GmatBase *obj, bool &inputAdded)
//------------------------------------------------------------------------------
bool FunctionManager::SetPassedInput(Integer index, GmatBase *obj, bool &inputAdded)
{
   inputAdded = false;
   
   if (obj == NULL)
      return false;
   
   #ifdef DEBUG_INPUT
   MessageInterface::ShowMessage
      ("FunctionManager::SetPassedInput() entered, index=%d, obj=<%p><%s>'%s'\n",
       index, obj, obj->GetTypeName().c_str(), obj->GetName().c_str());
   #endif
   
   if (obj->GetTypeName() == "Variable")
   {
      #ifdef DEBUG_INPUT
      MessageInterface::ShowMessage("   value=%f\n", ((Variable*)obj)->GetReal());
      #endif
   }
   
   std::string formalInput = f->GetStringParameter("Input", index);
   
   #ifdef DEBUG_INPUT
   Integer numOthers = createdOthers.size();
   MessageInterface::ShowMessage
      ("   numOthers=%d, formalInput='%s'\n", numOthers, formalInput.c_str());
   #endif
   
   Integer numInput = passedIns.size();
   
   if (index < 0 && index >= numInput)
   {
      #ifdef DEBUG_INPUT
      MessageInterface::ShowMessage
         ("FunctionManager::SetPassedInput() returning false, "
          "index is out-of-bounds of input list\n");
      #endif
      return false;
   }
   
   std::string passedInput = passedIns[index];
   obj->SetName(passedInput);
   
   #ifdef DEBUG_INPUT
   MessageInterface::ShowMessage
      ("   passed input string='%s'\n", passedInput.c_str());
   #endif
   
   // if passed input string is in createdLiterals, set new object
   if (createdLiterals.find(passedInput) != createdLiterals.end())
   {
      #ifdef DEBUG_INPUT
      MessageInterface::ShowMessage
         ("      '%s' found, so set new object to createdLiterals\n", passedInput.c_str());
      #endif
      
      GmatBase *oldObj = createdLiterals[passedInput];
      createdOthers[passedInput] = obj;
      if (oldObj)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (oldObj, oldObj->GetName(), "FunctionManager::SetPassedInput()",
             "deleting oldObj passedInput");
         #endif
         delete oldObj;
      }
      inputAdded = false;
      
      #ifdef DEBUG_INPUT
      MessageInterface::ShowMessage
         ("FunctionManager::SetPassedInput() returning true\n");
      #endif
      return true;
   }
   
   // if passed input string is in createdOthers, set new object
   // else add new object
   if (createdOthers.find(passedInput) != createdOthers.end())
   {
      #ifdef DEBUG_INPUT
      MessageInterface::ShowMessage
         ("      '%s' found, so set new object to createdOthers\n", passedInput.c_str());
      #endif
      GmatBase *oldObj = createdOthers[passedInput];
      createdOthers[passedInput] = obj;
      if (oldObj)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (oldObj, oldObj->GetName(), "FunctionManager::SetPassedInput()",
             "deleting oldObj passedInput");
         #endif
         delete oldObj;
      }
   }
   else
   {
      #ifdef DEBUG_INPUT
      MessageInterface::ShowMessage
         ("      '%s' not found, so added to createdOthers\n", passedInput.c_str());
      #endif
      createdOthers.insert(std::make_pair(passedInput, obj));
      inputAdded = true;
      
      #ifdef DEBUG_INPUT
      std::map<std::string, GmatBase *>::iterator omi;
      for (omi = createdOthers.begin(); omi != createdOthers.end(); ++omi)
         MessageInterface::ShowMessage
            ("   '%s', <%p>'%s'\n", (omi->first).c_str(), (omi->second),
             (omi->second)->GetName().c_str());
      #endif
   }
   
   #ifdef DEBUG_INPUT
   MessageInterface::ShowMessage
      ("   adding formal input '%s' to function object store\n", formalInput.c_str());
   #endif
   
   // Add formalInput to function object store
   GmatBase *objFOS = obj->Clone();   
   objFOS->SetName(formalInput);
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (objFOS, formalInput, "FunctionManager::SetPassedInput()",
       "objFOS = obj->Clone()");
   #endif
   
   objFOS->SetIsLocal(true);
   functionObjectStore->insert(std::make_pair(formalInput, objFOS));
   
   #ifdef DEBUG_INPUT
   MessageInterface::ShowMessage("FunctionManager::SetPassedInput() returning true\n");
   #endif
   return true;
}

//------------------------------------------------------------------------------
// WrapperArray& GetWrappersToDelete()
//------------------------------------------------------------------------------
WrapperArray& FunctionManager::GetWrappersToDelete()
{
   return f->GetWrappersToDelete();
}

// Sequence methods

//------------------------------------------------------------------------------
// bool PrepareObjectMap()
//------------------------------------------------------------------------------
bool FunctionManager::PrepareObjectMap()
{
   if (f == NULL)
   {
      std::string errMsg = "FunctionManager:: Unable to execute Function """;
      errMsg += fName + """ - pointer is NULL\n";
      throw FunctionException(errMsg);
   }
   
   std::string objName;
   
   // Initialize the Validator - I think I need to do this each time - or do I?
   //validator.SetSolarSystem(solarSys);
   if (validator == NULL)
      validator = Validator::Instance();
   
   combinedObjectStore.clear();
   std::map<std::string, GmatBase *>::iterator omi;
   for (omi = localObjectStore->begin(); omi != localObjectStore->end(); ++omi)
      combinedObjectStore.insert(std::make_pair(omi->first, omi->second));
   for (omi = globalObjectStore->begin(); omi != globalObjectStore->end(); ++omi)
      combinedObjectStore.insert(std::make_pair(omi->first, omi->second));
   
   #ifdef DEBUG_FM_EXECUTE
   MessageInterface::ShowMessage
      ("in FM::PrepareObjectMap(), Validator's object map was just set to "
       "combinedObjectStore\n");
   #ifdef DEBUG_OBJECT_MAP
   ShowObjectMap(&combinedObjectStore, "CombinedObjectStore");
   #endif
   #endif
   
   validator->SetObjectMap(&combinedObjectStore);
   
   return true;
}

//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool FunctionManager::Initialize()
{
   #ifdef DEBUG_TRACE
   static Integer callCount = 0;
   callCount++;
   clock_t t1 = clock();
   ShowTrace(callCount, t1, "FunctionManager::Initialize() entered");
   #endif
   
   #ifdef DEBUG_FM_INIT
   MessageInterface::ShowMessage
      ("=======================================================================\n"
       "Entering FM::Initialize() this FM is <%p>, current function is '%s'\n   "
       "calling FM is <%p>'%s'\n", this, fName.c_str(), callingFunction,
       callingFunction ? callingFunction->GetFunctionName().c_str() : "NULL");
   MessageInterface::ShowMessage
      ("FunctionManager::Initialize() clonedObjectStores.size()=%d\n",
       clonedObjectStores.size());
   #endif
   
   // clonedObjectStores should not be cleared for recursive call to work
   //clonedObjectStores.clear();
   functionObjectStore = new ObjectMap;
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (functionObjectStore, "functionObjectStore", "FunctionManager::Initialize()",
       "functionObjectStore = new ObjectMap");
   #endif
   
   #ifdef DEBUG_FM_INIT
   MessageInterface::ShowMessage
      ("   new functionObjectStore <%p> created\n", functionObjectStore);
   #endif
   
   if (!ValidateFunctionArguments())
      return false;
   
   if (!CreatePassingArgWrappers())
      return false;
   
   #ifdef DEBUG_FM_INIT
   MessageInterface::ShowMessage("Exiting  FM::Initialize() firstExecution set to false\n");
   #endif
   
   firstExecution = false;
   isFinalized = false;
   
   #ifdef DEBUG_TRACE
   ShowTrace(callCount, t1, "FunctionManager::Initialize() exiting", true);
   #endif
   
   return true;
}

//------------------------------------------------------------------------------
// bool Execute(FunctionManager *callingFM)
//------------------------------------------------------------------------------
bool FunctionManager::Execute(FunctionManager *callingFM)
{
   if (f == NULL)
      throw FunctionException("FunctionManager: Function pointer is NULL");
   
   #ifdef DEBUG_TRACE
   static Integer callCount = 0;
   callCount++;      
   clock_t t1 = clock();
   ShowTrace(callCount, t1, "FunctionManager::Execute() entered");
   //MessageInterface::ShowMessage("   === firstExecution=%d\n", firstExecution);
   #endif
   
   #ifdef DEBUG_FM_EXECUTE
   MessageInterface::ShowMessage
      ("=======================================================================\n"
       "Entering FM::Execute(), current function is '%s'\n   calling FM is '%s'\n"
       "   solarSys is %p, internalCS is %p, forces is %p\n",
       fName.c_str(), callingFM ? callingFM->GetFunctionName().c_str() : "NULL",
       solarSys, internalCS, forces);
   #endif
   
   PrepareObjectMap();
   PrepareExecution(callingFM);
   
   if (firstExecution)
   {
      #ifdef DEBUG_FM_EXECUTE
      MessageInterface::ShowMessage("   First execution, so calling Initialize()\n");
      #endif
      Initialize();
   }
   else
   {
      #ifdef DEBUG_FM_EXECUTE
      MessageInterface::ShowMessage
         ("   NOT First execution, so calling RefreshFOS()\n");
      #endif
      RefreshFOS();
      RefreshFormalInputObjects();
   } // end if not first time through
   
   #ifdef DEBUG_FM_EXECUTE
   MessageInterface::ShowMessage
      ("   Now pass FOS/GOS into the function <%p>'%s'\n", f, GetFunctionName().c_str());
   ShowObjectMap(functionObjectStore, "FOS to pass to function");
   ShowObjectMap(globalObjectStore, "GOS to pass to function");
   #endif
   
   // pass the FOS/GOS and other objects into the function
   f->SetObjectMap(functionObjectStore);
   f->SetGlobalObjectMap(globalObjectStore);
   f->SetSolarSystem(solarSys);
   f->SetTransientForces(forces);
   
   // send all input element wrappers to the function
   std::map<std::string, ElementWrapper *>::iterator ewi;
   for (ewi = inputWrapperMap.begin(); ewi != inputWrapperMap.end(); ++ewi)
   {
      #ifdef DEBUG_FM_EXECUTE
      MessageInterface::ShowMessage
         ("   Calling f->InputElementWrapper(%s, %p)\n", ewi->first.c_str(),
          ewi->second);
      #endif
      f->SetInputElementWrapper(ewi->first, ewi->second);
   }
   
   // must re-initialize the function each time, as it may be called in more than
   // one place
   if (!(f->Initialize()))
   {
      std::string errMsg = "FunctionManager:: Error initializing function \"";
      errMsg += f->GetStringParameter("FunctionName") + "\"\n";
      throw FunctionException(errMsg);
   }
   
   // create new ObjectInitializer   
   #ifdef DEBUG_FM_EXECUTE
   MessageInterface::ShowMessage
      ("in FM::Execute (%s), about to create new ObjectInitializer, objInit=<%p>, "
       "solarSys=<%p>, FOS=<%p>, GOS=<%p>, internalCS=<%p>, and true for useGOS\n",
       fName.c_str(), objInit, solarSys, functionObjectStore, globalObjectStore, internalCS);
   #endif
   
   if (objInit)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (objInit, "objInit", "FunctionManager::Execute()");
      #endif
      delete objInit;
   }
   
   objInit = new ObjectInitializer(solarSys, functionObjectStore,
                                   globalObjectStore, internalCS, true, true);   
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (objInit, "objInit", "FunctionManager::Execute()", "objInit = new ObjectInitializer");
   #endif
   
   // tell the fcs that this is the calling function
   #ifdef DEBUG_FM_EXECUTE
      MessageInterface::ShowMessage("   new objInit=<%p> created\n", objInit);
      MessageInterface::ShowMessage(
         "in FM::Execute (%s), about to set calling function <%p> '%s' on commands\n",
         fName.c_str(), callingFunction, callingFunction ?
         callingFunction->GetFunctionName().c_str() : "NULL");
   #endif
   GmatCommand *cmd = f->GetFunctionControlSequence();
   while (cmd) 
   {
      #ifdef DEBUG_FM_EXECUTE
         MessageInterface::ShowMessage(
               "in FM::Execute, about to set calling function on command %s\n",
               (cmd->GetTypeName()).c_str());
      #endif
      cmd->SetCallingFunction(this);
      cmd->SetInternalCoordSystem(internalCS); //added (loj: 2008.10.07)
      cmd = cmd->GetNext();
   }
   
   #ifdef DEBUG_FM_EXECUTE
   MessageInterface::ShowMessage("in FM::Execute, Now calling function->Execute()\n");
   MessageInterface::ShowMessage
      ("   Create and Global command may have updated FOS/GOS in the function <%p>'%s'\n",
       f, GetFunctionName().c_str());
   ShowObjectMap(functionObjectStore, "FOS to be used in the function");
   ShowObjectMap(globalObjectStore, "GOS to be used in the function");
   MessageInterface::ShowMessage
      ("======================================================= Calling f->Execute()\n");
   #endif
   
   // Now, execute the function
   bool reinitialize = false;
   if (callingFunction != NULL)
      reinitialize = true;

   // If function sequence failed or threw an exception, finalize the function
   try
   {
      if (!f->Execute(objInit, reinitialize))
      {
         MessageInterface::ShowMessage
            ("*** ERROR *** FunctionManager \"%s\" finalizing... due to false returned "
             "from f->Execute()\n", fName.c_str());
         f->Finalize();
         if (publisher)
            publisher->ClearPublishedData();
         
         Cleanup();
         return false;
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("*** ERROR *** FunctionManager \"%s\" finalizing... due to \n%s\n", fName.c_str(),
          e.GetFullMessage().c_str());
      f->Finalize();
      if (publisher)
         publisher->ClearPublishedData();
      
      Cleanup();
      return false;
   }
   
   #ifdef DEBUG_FM_EXECUTE
   MessageInterface::ShowMessage
      ("======================================================= f->Execute() DONE\n");
   #endif
   
   // Now assign result
   AssignResult();
   
   // Now deal with the calling function here
   if (!HandleCallStack())
   {
      Cleanup();
      return false;
   }
   
   #ifdef DEBUG_TRACE
   ShowTrace(callCount, t1, "FunctionManager::Execute() exiting", true);
   #endif
   
   return true;
}

//------------------------------------------------------------------------------
// Real FunctionManager::Evaluate(FunctionManager *callingFM)
//------------------------------------------------------------------------------
Real FunctionManager::Evaluate(FunctionManager *callingFM)
{
   if (f == NULL)
   {
      std::string errMsg = "FunctionManager:: Unable to return Real value from Function """;
      errMsg += fName + """ - pointer is NULL\n";
      throw FunctionException(errMsg);
   }
   
   #ifdef DEBUG_FM_EVAL
   MessageInterface::ShowMessage
      ("Entering FunctionManager::Evaluate() f=<%p><%s>\n", f, f->GetName().c_str());
   #endif
   
   Execute(callingFM);
   
   if (outputType != "Real")
      throw FunctionException("FunctionManager: invalid output type - should be real\n");
   
   #ifdef DEBUG_FM_EVAL
   MessageInterface::ShowMessage
      ("Exiting  FunctionManager::Evaluate() with %f\n", realResult);
   #endif
   
   return realResult;
}

//------------------------------------------------------------------------------
// Rmatrix FunctionManager::MatrixEvaluate(FunctionManager *callingFM)
//------------------------------------------------------------------------------
Rmatrix FunctionManager::MatrixEvaluate(FunctionManager *callingFM)
{
   if (f == NULL)
   {
      std::string errMsg = "FunctionManager:: Unable to return Rmatrix value from Function """;
      errMsg += fName + """ - pointer is NULL\n";
      throw FunctionException(errMsg);
   }

   #ifdef DEBUG_FM_EVAL
   MessageInterface::ShowMessage
      ("Entering FunctionManager::MatrixEvaluate() f=<%p><%s>\n", f, f->GetName().c_str());
   #endif
   
   Execute(callingFM);
   if (outputType != "Rmatrix")
      throw FunctionException("FunctionManager: invalid output type - should be Rmatrix\n");
   
   #ifdef DEBUG_FM_EVAL
   MessageInterface::ShowMessage
      ("Exiting  FunctionManager::MatrixEvaluate() with %s\n", matResult.ToString().c_str());
   #endif
   
   return matResult;
}

//------------------------------------------------------------------------------
// void FunctionManager::Finalize()
//------------------------------------------------------------------------------
void FunctionManager::Finalize()
{
   #ifdef DEBUG_TRACE
   static Integer callCount = 0;
   callCount++;      
   clock_t t1 = clock();
   ShowTrace(callCount, t1, "FunctionManager::Finalize() entered");
   #endif
   
   #ifdef DEBUG_FM_FINALIZE
   MessageInterface::ShowMessage
      ("Entering FM::Finalize, this FM is <%p>, current function is '%s', "
       " calling FM is <%p> '%s'\n", this, fName.c_str(), callingFunction,
       callingFunction ? callingFunction->GetFunctionName().c_str() : "NULL");
   MessageInterface::ShowMessage
       ("   functionObjectStore=<%p>, localObjectStore=<%p>, clonedObjectStores.size()=%d\n",
        functionObjectStore, localObjectStore, clonedObjectStores.size());
   #ifdef DEBUG_OBJECT_MAP
   ShowObjectMap(functionObjectStore, "FOS in Finalize");
   ShowObjectMap(localObjectStore, "LOS in Finalize");
   //ShowObjectMap(clonedObjectStore, "clonedOS in Finalize");
   #endif
   #endif
   
   if (!isFinalized)
   {
      Cleanup();
   
      firstExecution = true;
      isFinalized = true;
   }
   
   #ifdef DEBUG_TRACE
   ShowTrace(callCount, t1, "FunctionManager::Finalize() exiting", true, true);
   #endif
}

//------------------------------------------------------------------------------
// bool IsFinalized()
//------------------------------------------------------------------------------
bool FunctionManager::IsFinalized()
{
   return isFinalized;
//    if (f)
//       return f->IsFcsFinalized();
//    return true;
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// void PrepareExecution(FunctionManager *callingFM = NULL)
//------------------------------------------------------------------------------
void FunctionManager::PrepareExecution(FunctionManager *callingFM)
{
   #ifdef DEBUG_FM_EXECUTE
   MessageInterface::ShowMessage
      ("Entering FM::PrepareExecution for '%s'\n", fName.c_str());
   MessageInterface::ShowMessage
      ("   and the calling FM is '%s'\n",
       callingFM? (callingFM->GetFunctionName()).c_str() : "NULL");
   #endif
   
   callers.push(callingFunction);
   callingFunction  = callingFM;
   
   #ifdef DEBUG_FM_EXECUTE
   ShowCallers("After push");
   #endif
   
   if (callingFunction != NULL)
   {
      #ifdef DO_NOT_EXECUTE_NESTED_GMAT_FUNCTIONS
         Finalize(); // Do we need Finalize here? (loj: 2008.09.11)
         std::string noNested = "FunctionManager (";
         noNested += fName + ") - nested functions not yet supported";
         throw FunctionException(noNested);
      #else
         #ifdef DEBUG_FM_EXECUTE
         ShowObjectMap(localObjectStore, "=====> LOS in FM::PrepareExecution() before push");
         #endif
         losStack.push(localObjectStore);
         localObjectStore = callingFunction->PushToStack();
         #ifdef DEBUG_FM_EXECUTE
         ShowObjectMap(localObjectStore, "=====> LOS in FM::PrepareExecution() after  push");
         #endif
         firstExecution   = true; // so that it will initialize
      #endif
   }
   
   #ifdef DEBUG_FM_EXECUTE
   MessageInterface::ShowMessage("Exiting  FM::PrepareExecution() for '%s'\n", fName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// bool ValidateFunctionArguments()
//------------------------------------------------------------------------------
/**
 * Checks function input/output arguments against function's formal parameters.
 */
//------------------------------------------------------------------------------
bool FunctionManager::ValidateFunctionArguments()
{
   #ifdef DEBUG_FM_INIT
   MessageInterface::ShowMessage
      ("Entering FM::ValidateFunctionArguments for '%s'\n", fName.c_str());
   #endif
   if (!firstExecution) return true;
   
   StringArray inFormalNames = f->GetStringArrayParameter(f->GetParameterID("Input"));
   StringArray outFormalNames = f->GetStringArrayParameter(f->GetParameterID("Output"));
   
   #ifdef DEBUG_FM_INIT
   MessageInterface::ShowMessage
      ("    inFormalNames.size=%d,  passedIns.size=%d\n", inFormalNames.size(), passedIns.size());
   MessageInterface::ShowMessage
      ("   outFormalNames.size=%d, passedOuts.size=%d\n", outFormalNames.size(), passedOuts.size());
   for (unsigned int rr = 0; rr < inFormalNames.size(); rr++)
      MessageInterface::ShowMessage
         ("    inFormalNames[%d] = %s\n", rr, (inFormalNames.at(rr)).c_str());
   if (passedIns.size() == 0)
      MessageInterface::ShowMessage("NOTE - passedIns is empty - is it supposed to be?\n");
   else
      for (unsigned int qq = 0; qq < passedIns.size(); qq++)
         MessageInterface::ShowMessage("        passedIns[%d] = %s\n", qq, (passedIns.at(qq)).c_str());
   for (unsigned int rr = 0; rr < outFormalNames.size(); rr++)
      MessageInterface::ShowMessage
         ("   outFormalNames[%d] = %s\n", rr, (outFormalNames.at(rr)).c_str());
   if (passedOuts.size() == 0)
      MessageInterface::ShowMessage("NOTE - passedOuts is empty - is it supposed to be?\n");
   else
      for (unsigned int qq = 0; qq < passedOuts.size(); qq++)
         MessageInterface::ShowMessage("       passedOuts[%d] = %s\n", qq, (passedOuts.at(qq)).c_str());
   #endif
   
   
   // check input arguments
   // Number of inputs cannot be more than number of formal input arguments,
   // but can we have less than formal input arguments?
   // Can we have default input value?
   // Currently if number of input is less than formal, it is created as String,
   // so number of inputs must match for now.
   if (passedIns.size() != inFormalNames.size())
   {
      FunctionException ex;
      ex.SetDetails("The function '%s' expecting %d input argument(s), but called "
                    "with %d argument(s)", f->GetFunctionPathAndName().c_str(),
                    inFormalNames.size(), passedIns.size());
      throw ex;
   }
   
   // check output arguments   
   // Number of outputs cannot be more than number of formal output arguments,
   // but can we have less than formal output arguments?
   // Let's just check for more than formal output for now.
   if (passedOuts.size() > outFormalNames.size())
   {
      FunctionException ex;
      ex.SetDetails("The function '%s' expecting %d output argument(s), but called "
                    "with %d argument(s)", f->GetFunctionPathAndName().c_str(),
                    outFormalNames.size(), passedOuts.size());
      throw ex;
   }
   
   #ifdef DEBUG_FM_INIT
   MessageInterface::ShowMessage
      ("Exiting  FM::ValidateFunctionArguments with true for '%s'\n", fName.c_str());
   #endif
   
   return true;
}

//------------------------------------------------------------------------------
// bool CreatePassingArgWrappers()
//------------------------------------------------------------------------------
/**
 * Creates passing input and output argument wrappers.
 *
 * It finds input from LOS or GOS and clones it and adds it to input wrapper array.
 * If the input string does not refer to an object that can be located in the LOS
 * or GOS, it will try to create an object for it, e.g. numeric literal, string
 * literal. Since Outputs cannot be numeric or string literals or array elements, etc.
 * they must be found in the object store; and we do not need to clone them.
 */
//------------------------------------------------------------------------------
bool FunctionManager::CreatePassingArgWrappers()
{
   #ifdef DEBUG_FM_INIT
   MessageInterface::ShowMessage
      ("Entering FM::CreatePassingArgWrappers() for '%s'\n", fName.c_str());
   #endif
   
   GmatBase *obj, *objFOS;
   std::string formalName, passedName;
   
   StringArray inFormalNames = f->GetStringArrayParameter(f->GetParameterID("Input"));
   StringArray outFormalNames = f->GetStringArrayParameter(f->GetParameterID("Output"));
   
   validator->SetObjectMap(&combinedObjectStore);
   validator->SetSolarSystem(solarSys);
   
   // empty object map first (loj: 2008.10.31)
   if (!(IsOnStack(functionObjectStore))) // ********** ???? *************
      EmptyObjectMap(functionObjectStore, "FOS in CreatePassingArgWrappers");
   
   // clear input and out wrappers first
   ClearInOutWrappers();
   numVarsCreated = 0;
   
   // set up the FOS with the input objects
   for (unsigned int ii=0; ii<passedIns.size(); ii++)
   {
      // if the passing input string does not refer to an object that can be
      // located in the LOS or GOS, we must try to create an object for it,
      // if it makes sense (e.g. numeric literal, string literal, array element)
      formalName = inFormalNames.at(ii);
      passedName = passedIns.at(ii);
      if (!(obj = FindObject(passedName)))
      {
         #ifdef DEBUG_FM_INIT
         MessageInterface::ShowMessage
            ("==========> '%s' not found so creating...\n", passedName.c_str());
         #endif
         obj = CreateObject(passedName);
         if (!obj)
         {
            std::string errMsg = "FunctionManager: Object not found or created for input string \"";
            errMsg += passedIns.at(ii) + "\" for function \"";
            errMsg += fName + "\"\n";
            throw FunctionException(errMsg);
         }
         objFOS = obj; // do not clone obj, just set to objFos (loj: 2008.12.10)
         ////objFOS = obj->Clone();      
         objFOS->SetName(formalName);
      }
      // if object found clone it (loj: 2008.12.10)
      else
      {
         #ifdef DEBUG_FM_INIT
         MessageInterface::ShowMessage(
            "   passed input object \"%s\" of type \"%s\" found in LOS/GOS \n",
            (passedIns.at(ii)).c_str(), (obj->GetTypeName()).c_str());
         #endif
         objFOS = obj->Clone();
         objFOS->SetName(formalName);
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (objFOS, formalName, "FunctionManager::CreatePassingArgWrappers()",
             "objFOS = obj->Clone()");
         #endif
      }
      
      // Flag it as local object
      objFOS->SetIsLocal(true);
      
      functionObjectStore->insert(std::make_pair(formalName,objFOS));
      
      #ifdef DEBUG_FM_INIT // ------------------------------------------------- debug ---
         MessageInterface::ShowMessage("   Adding object '%s' to the FOS\n", formalName.c_str());
      #endif // -------------------------------------------------------------- end debug ---
      // create an element wrapper for the input
      // Do we neet to set this inside the loop? commented out (loj: 2008.11.21)
      //validator->SetObjectMap(&combinedObjectStore);
      //validator->SetSolarSystem(solarSys);
      std::string inName = passedIns.at(ii);      
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
         ("==========> FM:CreatePassingArgWrappers() '%s' Creating "
          "ElementWrapper for '%s'\n", fName.c_str(), inName.c_str());
      #endif
      
      ElementWrapper *inWrapper = validator->CreateElementWrapper(inName, false, false);
      
      #ifdef DEBUG_MORE_MEMORY
      MessageInterface::ShowMessage
         ("+++ FunctionManager::CreatePassingArgWrappers() *inWrapper = validator->"
          "CreateElementWrapper(%s), <%p> '%s'\n", inName.c_str(), inWrapper,
          inWrapper->GetDescription().c_str());
      #endif
      inWrapper->SetRefObject(objFOS);
      inputWrapperMap.insert(std::make_pair(formalName, inWrapper));
      #ifdef DEBUG_FM_INIT // ------------------------------------------------- debug ---
         MessageInterface::ShowMessage("   Created element wrapper of type %d for \"%s\"\n",
               inWrapper->GetWrapperType(), inName.c_str());
      #endif // -------------------------------------------------------------- end debug ---
   }
   
   // Outputs cannot be numeric or string literals or array elements, etc.
   // They must be found in the object store; and we do not need to clone them
   // Handle the case with one blank output (called from FunctionRunner) first   
   if ((passedOuts.size() == 1) && (passedOuts.at(0) == ""))
   {
      #ifdef DEBUG_FM_INIT
      MessageInterface::ShowMessage("   Has 1 output and it's name is blank\n");
      #endif
      blankResult = true;
   }
   else
   {
      for (unsigned int jj = 0; jj < passedOuts.size(); jj++)
      {
         if (!(obj = FindObject(passedOuts.at(jj))))
         {
            std::string errMsg = "Output \"" + passedOuts.at(jj);
            errMsg += " not found for function \"" + fName + "\"";
            throw FunctionException(errMsg);
         }
         // Do we neet to set this inside the loop? commented out (loj: 2008.11.21)
         //validator->SetObjectMap(&combinedObjectStore);
         //validator->SetSolarSystem(solarSys);
         std::string outName = passedOuts.at(jj);         
         #ifdef DEBUG_WRAPPERS
         MessageInterface::ShowMessage
            ("==========> FM:CreatePassingArgWrappers() '%s' Creating "
             "ElementWrapper for '%s'\n", fName.c_str(), outName.c_str());
         #endif
         
         ElementWrapper *outWrapper = validator->CreateElementWrapper(outName);
         
         #ifdef DEBUG_MORE_MEMORY
         MessageInterface::ShowMessage
            ("+++ FunctionManager::CreatePassingArgWrappers() *outWrapper = validator->"
             "CreateElementWrapper(%s), <%p> '%s'\n", outName.c_str(), outWrapper,
             outWrapper->GetDescription().c_str());
         #endif
         outWrapper->SetRefObject(obj); 
         outputWrappers.push_back(outWrapper);
         #ifdef DEBUG_FM_INIT // ------------------------------------------------- debug ---
            MessageInterface::ShowMessage("   Output wrapper created for %s\n", outName.c_str());
         #endif // -------------------------------------------------------------- end debug ---
      }
   }
   
   #ifdef DEBUG_FM_INIT
   MessageInterface::ShowMessage
      ("Exiting  FM::CreatePassingArgWrappers() for '%s'\n", fName.c_str());
   ShowObjectMap(functionObjectStore, "in CreatePassingArgWrappers()\n");
   #endif
   
   return true;
}

//------------------------------------------------------------------------------
// void RefreshFOS()
//------------------------------------------------------------------------------
/*
 * Deletes all objects in the function object store except formal inputs
 */
//------------------------------------------------------------------------------
void FunctionManager::RefreshFOS()
{
   #ifdef DEBUG_FM_REFRESH
   MessageInterface::ShowMessage
      ("FM:RefreshFOS() entered for '%s', current FOS = <%p>, "
       "has %d objects\n", fName.c_str(), functionObjectStore, functionObjectStore->size());
   #endif
   
   if (functionObjectStore == NULL)
      throw FunctionException
         ("FunctionManager::RefreshFOS() function pointer is NULL");
   
   // Need to delete all items in the FOS that are not inputs (so that they can 
   // properly be created again in the FCS 
   StringArray toDelete;
   bool        isInput = false;
   std::map<std::string, GmatBase *>::iterator omi;
   StringArray inFormalNames = f->GetStringArrayParameter(f->GetParameterID("Input"));
   
   #ifdef DEBUG_FM_REFRESH
   MessageInterface::ShowMessage
      ("   Function '%s' has %d inputs and FOS <%p> has %d objects\n",
       fName.c_str(), inFormalNames.size(), functionObjectStore, functionObjectStore->size());
   #endif
   
   for (omi = functionObjectStore->begin(); omi != functionObjectStore->end(); ++omi)
   {
      isInput = false;
      for (unsigned int qq = 0; qq < inFormalNames.size(); qq++)
         if (omi->first == inFormalNames.at(qq))
         {
            isInput = true;
            break;
         }
      if (!isInput) 
      {  
         if (omi->second != NULL)
         {
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (omi->second, (omi->second)->GetName(), "FunctionManager::RefreshFOS()");
            #endif
            delete omi->second;
            omi->second = NULL;
         }
         toDelete.push_back(omi->first); 
      }
   }
   for (unsigned int kk = 0; kk < toDelete.size(); kk++)
   {
      #ifdef DEBUG_FM_REFRESH
      MessageInterface::ShowMessage
         ("   erasing %s from FOS <%p>\n", toDelete.at(kk).c_str());
      #endif
      functionObjectStore->erase(toDelete.at(kk));
   }
   
   #ifdef DEBUG_FM_REFRESH
   MessageInterface::ShowMessage
      ("FM:RefreshFOS() leaving for '%s', has %d objects in FOS\n",
       fName.c_str(), functionObjectStore->size());
   #endif
}


//------------------------------------------------------------------------------
// void RefreshFormalInputObjects()
//------------------------------------------------------------------------------
/*
 * Finds object with passing input argument name from LOS or GOS or createdLiterals
 * or createdOthers and set to object in the FOS.
 */
//------------------------------------------------------------------------------
void FunctionManager::RefreshFormalInputObjects()
{
   #ifdef DEBUG_FM_EXECUTE
   MessageInterface::ShowMessage("FM:RefreshFormalInputObjects() entered\n");
   #endif
   
   std::string formalName, passedName;
   // Get function formal input argument names (parameters)
   StringArray inFormalNames = f->GetStringArrayParameter(f->GetParameterID("Input"));
   
   // Find and/or evaluate the input objects
   for (unsigned int ii=0; ii<passedIns.size(); ii++)
   {
      // Get the object from the object store first 
      formalName = inFormalNames.at(ii);
      passedName = passedIns.at(ii);
      
      #ifdef DEBUG_FM_EXECUTE
      MessageInterface::ShowMessage
         ("   passedIns[%d]='%s', formalName='%s'\n", ii, passedName.c_str(),
          formalName.c_str());
      #endif
      
      if (functionObjectStore->find(formalName) == functionObjectStore->end())
      {
         std::string errMsg = "FunctionManager error: input object \"" + formalName;
         errMsg += "\"not found in Function Object Store.\n";
         throw FunctionException(errMsg);
      }
      
      #ifdef DEBUG_FM_EXECUTE
      MessageInterface::ShowMessage
         ("   formalName='%s' found from the function object store\n", formalName.c_str());
      #endif
      
      GmatBase *obj = NULL;
      GmatBase *fosObj = (*functionObjectStore)[formalName];
      
      // Now find the corresponding input object
      // if passed name not found in LOS or GOS, it may be a number, string literal,
      // array element, or automatic object such as sat.X
      if (!(obj = FindObject(passedName)))
      {
         #ifdef DEBUG_FM_EXECUTE
         ShowObjectMap(&createdLiterals, "createdLiterals map in RefreshFormalInputObjects");
         #endif
         
         // if passed name not found in created literals
         if (createdLiterals.find(passedName) == createdLiterals.end())
         {
            #ifdef DEBUG_FM_EXECUTE
            ShowObjectMap(&createdLiterals, "createdOthers map in RefreshFormalInputObjects");
            #endif            
            
            if (createdOthers.find(passedName) == createdOthers.end())
            {
               std::string errMsg = "Input \"" + passedName;
               errMsg += " not found for function \"" + fName + "\"";
               throw FunctionException(errMsg);
            }
            
            #ifdef DEBUG_FM_EXECUTE
            MessageInterface::ShowMessage
               ("   '%s' found from the createdOthers\n", passedName.c_str());
            #endif
            
            GmatBase *oldFosObj = fosObj;
            GmatBase *oldObj = createdOthers[passedName];
            obj = CreateObject(passedName);
            if (!obj)
            {
               std::string errMsg2 =
                  "FunctionManager: Object not found or created for input string \"";
               errMsg2 += passedName + "\" for function \"";
               errMsg2 += fName + "\"\n";
               throw FunctionException(errMsg2);
            }
            createdOthers[passedName] = obj;
            fosObj = obj->Clone();
            fosObj->SetName(formalName);
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Add
               (fosObj, formalName, "FunctionManager::RefreshFormalInputObjects()",
                "obj->Clone()");
            #endif
            
            if (oldFosObj)
            {
               #ifdef DEBUG_MEMORY
               MemoryTracker::Instance()->Remove
                  (oldFosObj, oldFosObj->GetName(),
                   "FunctionManager::RefreshFormalInputObjects()", "deleting oldFosObj");
               #endif
               delete oldFosObj;
               oldFosObj = NULL;
            }
            
            if (oldObj)
            {
               #ifdef DEBUG_MEMORY
               MemoryTracker::Instance()->Remove
                  (oldObj, oldObj->GetName(), "FunctionManager::RefreshFormalInputObjects()",
                   "deleting oldObj");
               #endif
               delete oldObj;
               oldObj = NULL;
            }
         }
         else
         {
            #ifdef DEBUG_FM_EXECUTE
            MessageInterface::ShowMessage
               ("   '%s' found from the createdLiterals\n", passedName.c_str());
            #endif
            obj = createdLiterals[passedName];
         }
      }
      
      #ifdef DEBUG_FM_EXECUTE
      MessageInterface::ShowMessage
         ("   Update the object in the object store with the current object\n");
      #endif
      
      // Update the object in the object store with the current/reset data
      fosObj->Copy(obj);
      (inputWrapperMap[formalName])->SetRefObject(fosObj);  // is this necessary? I think so
      
   }
   
   #ifdef DEBUG_FM_EXECUTE
   MessageInterface::ShowMessage("FM:RefreshFormalInputObjects() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// GmatBase* FunctionManager::FindObject(const std::string &name, bool arrayElementsAllowed = false)
//------------------------------------------------------------------------------
GmatBase* FunctionManager::FindObject(const std::string &name, bool arrayElementsAllowed)
{
   std::string newName = name;
   
   // Ignore array indexing of Array
   std::string::size_type index = name.find('(');
   if (index != name.npos)
   {
      if (!arrayElementsAllowed) return NULL; // we deal with array elements separately
      else                       newName = name.substr(0, index);
   // Check for the object in the Local Object Store (LOS) first
   }
   if (localObjectStore->find(newName) == localObjectStore->end())
   {
     // If not found in the LOS, check the Global Object Store (GOS)
      if (globalObjectStore->find(newName) == globalObjectStore->end())
         return NULL;
      else
      {
         #ifdef DEBUG_FIND_OBJ
         MessageInterface::ShowMessage("   '%s' found in GOS\n", newName.c_str());
         #endif
         return (*globalObjectStore)[newName];
      }
   }
   else
   {
      #ifdef DEBUG_FIND_OBJ
      MessageInterface::ShowMessage("   '%s' found in LOS\n", newName.c_str());
      #endif
      return (*localObjectStore)[newName];
   }
   return NULL; // should never get to this point
}

//------------------------------------------------------------------------------
// GmatBase* CreateObject(const std::string &fromString)
//------------------------------------------------------------------------------
/*
 * Creates an object from the input string name. If input string is numeric string
 * it creates a Variable, and StringVar if it is string literal. All other string
 * values are handled by first creating an element wrapper and create a proper object
 * based on the wrapper type. The local element wrapper is deleted at last.
 *
 * @param fromString input name of the object to be created
 *
 * @return newly created object
 */
//------------------------------------------------------------------------------
GmatBase* FunctionManager::CreateObject(const std::string &fromString)
{
   #ifdef DEBUG_CREATE_OBJ
   MessageInterface::ShowMessage
      ("FunctionManager::CreateObject() entered, fromString='%s'\n", fromString.c_str());
   #endif
   
   GmatBase    *obj = NULL, *refObj;
   Real        rval;
   std::string sval;
   Variable    *v = NULL;
   if (GmatStringUtil::IsBlank(fromString)) return obj;
   // determine the name of the next object to be created (if needed)
   //std::string newName = "FMvar" + numVarsCreated;
   std::string str     = GmatStringUtil::Trim(fromString, GmatStringUtil::BOTH);
   Integer     sz      = (Integer) str.size();
   
   // Check first to see if the string is a numeric literal (and assume we want a real)
   if (GmatStringUtil::ToReal(fromString, &rval))
   {
      v = new Variable(str);      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (v, str, "FunctionManager::CreateObject()", "v = new Variable()");
      #endif
      
      v->SetReal(rval);
      obj = (GmatBase*) v;
      createdLiterals.insert(std::make_pair(str,obj));
   }
   // Now check to see if it is a string literal, enclosed with single quotes
   else if ((str[0] == '\'') && (str[sz-1] == '\''))
   {
      sval = str.substr(1, sz-2);
      StringVar *sv = new StringVar(str);      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (v, str, "FunctionManager::CreateObject()", "sv = new StringVar()");
      #endif
      
      sv->SetStringParameter("Value", sval);
      obj = (GmatBase*) sv;
      createdLiterals.insert(std::make_pair(str,obj));
   }
   else
   {
      // otherwise, we assume it is something else, like array element.
      // NOTE that we are only allowing Real and Array here 
      validator->SetObjectMap(&combinedObjectStore);
      validator->SetSolarSystem(solarSys);
      
      // We should pass manage=2 as Function here so that new objects created
      // can be stored in the function object store(LOJ: 2009.05.14)
      ElementWrapper *ew = validator->CreateElementWrapper(fromString, false, 2);
      if (ew)
      {
         #ifdef DEBUG_MORE_MEMORY
         MessageInterface::ShowMessage
            ("+++ FunctionManager::CreateObject() *ew = validator->"
             "CreateElementWrapper(%s), <%p> '%s'\n", fromString.c_str(), ew,
             ew->GetDescription().c_str());
         #endif
         
         Gmat::WrapperDataType wType = ew->GetWrapperType();
         switch (wType)
         {
            case Gmat::ARRAY_WT :
            {
               Array *array   = new Array(str);               
               #ifdef DEBUG_MEMORY
               MemoryTracker::Instance()->Add
                  (array, str, "FunctionManager::CreateObject()", "array = new Array()");
               #endif
               
               Rmatrix matVal = ew->EvaluateArray();
               array->SetRmatrix(matVal);
               obj = (GmatBase*) array;
               break;
            }
            case Gmat::ARRAY_ELEMENT_WT :
            {
               // get the reference object first ...
               refObj = FindObject(fromString, true);
               if (!refObj)
               {
                  std::string errMsg = "FunctionManager: Error getting reference"
                     " object for array element ";
                  errMsg += fromString + "\n";
                  throw FunctionException(errMsg);
               }
               ew->SetRefObject(refObj);
               rval = ew->EvaluateReal(); 
               v = new Variable(str);               
               #ifdef DEBUG_MEMORY
               MemoryTracker::Instance()->Add
                  (v, str, "FunctionManager::CreateObject()", "v = new Variable()");
               #endif
               
               v->SetReal(rval);
               obj = (GmatBase*) v;
               break;
            }
            case Gmat::NUMBER_WT :
            case Gmat::VARIABLE_WT :
            case Gmat::INTEGER_WT :       // what is this anyway?
            {
               rval = ew->EvaluateReal(); 
               v = new Variable(str);
               #ifdef DEBUG_MEMORY
               MemoryTracker::Instance()->Add
                  (v, str, "FunctionManager::CreateObject()", "v = new Variable()");
               #endif
               
               v->SetReal(rval);
               obj = (GmatBase*) v;
               break;
            }
            default:
            {
               // If unknown create variable (loj: 2008.09.12)
               v = new Variable(str);
               obj = (GmatBase*) v;               
               #ifdef DEBUG_MEMORY
               MemoryTracker::Instance()->Add
                  (v, str, "FunctionManager::CreateObject()", "v = new Variable()");
               #endif
               
               std::string errMsg = "FunctionManager: error using string \"";
               errMsg += fromString + "\" to determine an object\n";
               MessageInterface::ShowMessage(errMsg + " so ignoring\n");
               ////throw FunctionException(errMsg);
               break;
            }
         }
         
         // We need to delete newly created element wrapper here
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (ew, ew->GetDescription(), "FunctionManager::CreateObject()",
             "deleting unused wrapper");
         #endif
         delete ew;
      }
      if (obj) createdOthers.insert(std::make_pair(str, obj));
   }
   if (obj) numVarsCreated++;
   
   #ifdef DEBUG_CREATE_OBJ
   MessageInterface::ShowMessage
      ("FunctionManager::CreateObject() returning <%p><%s>'%s'\n", obj,
       obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL");
   #endif
   return obj;
} // CreateObject()


//------------------------------------------------------------------------------
// void AssignResult()
//------------------------------------------------------------------------------
/*
 * Sets result to output wrapper
 */
//------------------------------------------------------------------------------
void FunctionManager::AssignResult()
{
   if (blankResult)
   {
      #ifdef DEBUG_FM_RESULT
      MessageInterface::ShowMessage
         ("in FM::AssignResult, It has a blank result, so saving the last result\n");
      #endif
      SaveLastResult();
   }
   else
   {
      #ifdef DEBUG_FM_RESULT
      MessageInterface::ShowMessage
         ("in FM::AssignResult, It has NO blank result, so assigning the result\n");
      MessageInterface::ShowMessage
         ("It has %d output wrapper(s)\n", outputWrappers.size());
      #endif
      
      for (unsigned int jj = 0; jj < outputWrappers.size(); jj++)
      {
         bool retval = false;
         ElementWrapper *ew = NULL;
         ew = f->GetOutputArgument(jj);
         
         if (ew == NULL)
            throw FunctionException
               ("FunctionManager::AssignResult() failed to assign results to function output");
         
         #ifdef DEBUG_FM_RESULT
         MessageInterface::ShowMessage
            ("Now setting result to output wrapper <%s>\n", ew->GetDescription().c_str());
         MessageInterface::ShowMessage
            ("   outputWrapper(%d)='%s'\n", jj, outputWrappers.at(jj)->ToString().c_str());
         #endif
         
         retval = ElementWrapper::SetValue(outputWrappers.at(jj), ew, solarSys,
                                           functionObjectStore, globalObjectStore);
         if (!retval)
            throw FunctionException
               ("FunctionManager::AssignResult() failed to assign results to function output");
         
         // Delete output wrappers here (loj: 2008.11.12)
         // Do we need to delete it here? (loj: 2008.11.21)
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (ew, ew->GetDescription(), "FunctionManager::AssignResult()",
             "deleting output wrapper");
         #endif
         delete ew;
      }
   }
}


//------------------------------------------------------------------------------
// bool HandleCallStack()
//------------------------------------------------------------------------------
/*
 * Handles callstack.
 */
//------------------------------------------------------------------------------
bool FunctionManager::HandleCallStack()
{
   #ifdef DEBUG_FM_EXECUTE
   MessageInterface::ShowMessage
      ("Entering FM::HandleCallStack for function '%s'\n", fName.c_str());
   //ShowStackContents(callStack, "Stack contents at beg. of PushToStack");
   //ShowObjectMap(functionObjectStore, "FOS at beg. of PushToStack");
   #endif
   
   if (callingFunction != NULL)
   {
      #ifdef DEBUG_FM_EXECUTE
      MessageInterface::ShowMessage
         ("Calling function is not NULL, so PopFromStack() with functionObjectStore=<%p>\n",
          functionObjectStore);
      #endif
      
      StringArray outFormalNames = f->GetStringArrayParameter(f->GetParameterID("Output"));
      
      // call the caller to pop its FOS back of the stack
      if (!callingFunction->PopFromStack(functionObjectStore, outFormalNames, passedOuts))
         return false;
      
      // restore the localObjectStore
      localObjectStore = losStack.top();
      
      #ifdef DEBUG_FM_STACK
      MessageInterface::ShowMessage("===> after losStack.top(), functionObjectStore=<%p>\n",
                                    functionObjectStore);
      MessageInterface::ShowMessage("===> after losStack.top(), localObjectStore=<%p>\n",
                                    localObjectStore);
      #endif
      
      losStack.pop();
      
      // functionObjectStore is deleted from PopFromStack()
      // reset functionObjectStore to NULL so that it won't use stale pointer (loj: 2008.10.07)
      if (losStack.empty())
         functionObjectStore = NULL;
      else
         functionObjectStore = localObjectStore;
      
      // Should this be deleted here? (loj: 2008.12.12)
      //f->ClearClonedObjects();
      firstExecution = true;   // to make sure it is reinitialized next time ???
   }
   else
   {
      // We need to finalize all commands in FCS here (loj: 2008.10.08)
      // Do not call Finalize() it will delete LOS and nested function will not work
      #ifdef DEBUG_FM_EXECUTE
      MessageInterface::ShowMessage
         ("   calling function <%p>'%s'->Finalize()\n", f, f->GetName().c_str());
      #endif
      f->Finalize();
      
      // Unsubscribe subscribers since function run is completed
      UnsubscribeSubscribers(functionObjectStore);
      
      // delete cloned object store
      Integer numClones = clonedObjectStores.size();
      for (Integer i=0; i<numClones; i++)
      {
         DeleteObjectMap(clonedObjectStores[i], "clonedOS in Finalize");
         clonedObjectStores[i] = NULL;
      }
      clonedObjectStores.clear();
   }
   
   if (!callers.empty())
   {
      // remove the caller from the stack of callers      
      callingFunction = callers.top();
      callers.pop();
      #ifdef DEBUG_FM_EXECUTE
      ShowCallers("After pop");
      #endif
   }
   
   #ifdef DEBUG_FM_EXECUTE
   MessageInterface::ShowMessage("Exiting  FM::HandleCallStack()\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void SaveLastResult()
//------------------------------------------------------------------------------
void FunctionManager::SaveLastResult()
{
   ElementWrapper *ew = f->GetOutputArgument(0);
   if (!ew) 
   {
      std::string errMsg = "FunctionManager: missing output argument from function \"";
      errMsg += fName + "\"\n";
      throw FunctionException(errMsg);
   }
   
   Gmat::ParameterType outType = ew->GetDataType();
   
   #ifdef DEBUG_FM_EXECUTE
   MessageInterface::ShowMessage
      ("FunctionManager::SaveLastResult() entered, ew='%s', outType=%d\n",
       ew->GetDescription().c_str(), outType);
   #endif
   
   switch (outType) 
   {
   case Gmat::INTEGER_TYPE:
      realResult = (Real) ew->EvaluateInteger();      
      outputType = "Real";
      break;
   case Gmat::REAL_TYPE:
      realResult = ew->EvaluateReal();
      outputType = "Real";
      #ifdef DEBUG_FM_RESULT
      MessageInterface::ShowMessage("   saved realResult=%f\n", realResult);
      #endif
      break;
   case Gmat::RMATRIX_TYPE:
      matResult = ew->EvaluateArray();
      outputType = "Rmatrix";
      #ifdef DEBUG_FM_RESULT
      MessageInterface::ShowMessage
         ("   saved matResult=%s\n", matResult.ToString().c_str());
      #endif
      break;
   default:
      throw FunctionException("FunctionManager: Unknown or invalid output data type");
   }
   
   #ifdef DEBUG_FM_EXECUTE
   MessageInterface::ShowMessage
      ("FunctionManager::SaveLastResult() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// ObjectMap* PushToStack()
//------------------------------------------------------------------------------
ObjectMap* FunctionManager::PushToStack()
{
   #ifdef DEBUG_FM_STACK
   MessageInterface::ShowMessage
      ("Entering FM::PushToStack for function '%s'\n", fName.c_str());
   ShowStackContents(callStack, "Stack contents at beg. of PushToStack");
   ShowObjectMap(functionObjectStore, "FOS at beg. of PushToStack");
   #endif
   
   // Clone the FOS
   ObjectMap *clonedObjMap = new ObjectMap();
   clonedObjectStores.push_back(clonedObjMap);
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (clonedObjMap, "clonedObjMap", fName + ":FunctionManager::PushToStack()",
       "*clonedObjMap = new ObjectMap");
   #endif
   CloneObjectMap(functionObjectStore, clonedObjMap);
   
   // Put the FOS onto the stack
   callStack.push(functionObjectStore);
   
   #ifdef DEBUG_FM_STACK
      MessageInterface::ShowMessage(
            "Exiting  PushToStack for function '%s'\n", fName.c_str());
      ShowObjectMap(clonedObjMap, "Cloned map at end of PushToStack");
      ShowStackContents(callStack, "Stack contents at end of PushToStack");
   #endif
   // Return the clone, to be used as the LOS for the FM that called this method
   return clonedObjMap;
}

//------------------------------------------------------------------------------
// bool PopFromStack(ObjectMap* cloned, const StringArray &outNames, ...)
//------------------------------------------------------------------------------
/**
 * Pops callstack and sets output to caller's output
 */
//------------------------------------------------------------------------------
bool FunctionManager::PopFromStack(ObjectMap* cloned, const StringArray &outNames,
                                   const StringArray &callingNames)
{
   #ifdef DEBUG_FM_STACK
   MessageInterface::ShowMessage
      ("Entering FM::PopFromStack for function '%s', cloned=<%p>\n", fName.c_str(), cloned);
   ShowStackContents(callStack, "Stack contents at beg. of PopFromStack");
   #endif
   if (callStack.empty())
      throw FunctionException(
            "ERROR popping object store from call stack - stack is empty\n");
   ObjectMap *topMap = callStack.top();
   callStack.pop();   
   functionObjectStore = topMap;
   
   #ifdef DEBUG_FM_STACK
      MessageInterface::ShowMessage("PopFromStack::outNames are: \n");
      for (unsigned int ii=0; ii<outNames.size(); ii++)
         MessageInterface::ShowMessage(" %d)   %s\n", ii, (outNames.at(ii)).c_str());
      MessageInterface::ShowMessage("PopFromStack::callingNames are: \n");
      for (unsigned int ii=0; ii<callingNames.size(); ii++)
         MessageInterface::ShowMessage(" %d)   %s\n", ii, (callingNames.at(ii)).c_str());
      MessageInterface::ShowMessage("PopFromStack::cloned stack entries are: \n");
      ShowObjectMap(cloned, "cloned");
      MessageInterface::ShowMessage("PopFromStack::popped stack entries are: \n");
      ShowObjectMap(functionObjectStore, "FOS");
   #endif
   
   // Assign output
   for (unsigned int jj = 0; jj < outNames.size(); jj++)
   {
      // Find output name from cloned object store
      // If name found then assign to object pointer since [] operator
      // inserts the item if it doesn't exist (LOJ: 2010.05.19)
      GmatBase *clonedObj = NULL;
      if (cloned->find(outNames.at(jj)) != cloned->end())
          clonedObj = (*cloned)[outNames.at(jj)];
      
      // Find output name from popped object store
      // If name found then assign to object pointer since [] operator
      // inserts the item if it doesn't exist (LOJ: 2010.05.19)
      GmatBase *fosObj = NULL;
      if (functionObjectStore->find(callingNames.at(jj)) != functionObjectStore->end())
         fosObj = (*functionObjectStore)[callingNames.at(jj)];
      
      if (clonedObj == NULL)
      {
         std::string errMsg = "PopFromStack::Error getting output named ";
         errMsg += outNames.at(jj) + " from cloned map in function \"";
         errMsg += fName + "\"\n";
         throw FunctionException(errMsg);
      }
      if (fosObj == NULL)
      {
         // We don't want to throw exception here (loj: 2008.09.12)
         //std::string errMsg = "PopFromStack::Error setting output named \"";
         //errMsg += callingNames.at(jj) + "\" from nested function on function \"";
         //errMsg += fName + "\"\n";
         ////throw FunctionException(errMsg);
         #ifdef DEBUG_FM_STACK
         MessageInterface::ShowMessage
            ("PopFromStack:: Found blank output name, realResult=%f\n", realResult);
         #endif
      }
      else
      {
         #ifdef DEBUG_FM_STACK
            MessageInterface::ShowMessage(
               "PopFromStack::Copying value of %s into popped stack element %s\n",
               (outNames.at(jj)).c_str(), (callingNames.at(jj)).c_str());
         #endif
         fosObj->Copy(clonedObj);
      }
   }
   DeleteObjectMap(cloned, "cloned in PropFromStack");
   cloned = NULL;
   
   #ifdef DEBUG_FM_STACK
      MessageInterface::ShowMessage(
            "PopFromStack::reset object map to the one popped from the stack\n");
      MessageInterface::ShowMessage(
            "PopFromStack::now about to re-initialize the function '%s'\n",
            fName.c_str());
   #endif
   
   // Set popped FOS to function and re-initialize fcs
   f->SetObjectMap(functionObjectStore);
   bool retval = f->Initialize();
   
   #ifdef DEBUG_FM_STACK
      MessageInterface::ShowMessage(
         "Exiting  FM::PopFromStack for function %s with %d\n", fName.c_str(), retval);
      ShowStackContents(callStack, "Stack contents at end of PopFromStack");
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// void Cleanup()
//------------------------------------------------------------------------------
void FunctionManager::Cleanup()
{
   #ifdef DEBUG_CLEANUP
   MessageInterface::ShowMessage("==> FunctionManager::Cleanup() entered, f=<%p>\n", f);
   #endif
   
   if (f != NULL)
   {
      if (f->IsOfType("GmatFunction"))
      {
         ////Edwin's MMS script failed, so commented out (loj: 2008.12.03)
         ////We need to delete this somewhere though.
         ////f->ClearAutomaticObjects();
         #ifdef DEBUG_CLEANUP
         MessageInterface::ShowMessage("   Calling f->Finalize()\n");
         #endif
         
         f->Finalize();
      }
   }
   
   // now delete all of the items/entries in the FOS - we can do this since they 
   // are all either locally-created or clones of reference objects or automatic objects
   if (functionObjectStore)
   {
      #ifdef DEBUG_CLEANUP
      MessageInterface::ShowMessage
         ("   Deleting functionObjectStore <%p>\n", functionObjectStore);
      #endif
      DeleteObjectMap(functionObjectStore, "FOS in Finalize");
      functionObjectStore = NULL;
   }
   
   Integer numClones = clonedObjectStores.size();
   for (Integer i=0; i<numClones; i++)
   {
      #ifdef DEBUG_CLEANUP
      MessageInterface::ShowMessage
         ("   Deleting clonedObjectStores[%d] <%p>\n", i, clonedObjectStores[i]);
      #endif
      DeleteObjectMap(clonedObjectStores[i], "clonedOS in Finalize");
      clonedObjectStores[i] = NULL;
   }
   clonedObjectStores.clear();
   
   #ifdef DEBUG_CLEANUP
   MessageInterface::ShowMessage("==> FunctionManager::Cleanup() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void UnsubscribeSubscribers(ObjectMap *om)
//------------------------------------------------------------------------------
void FunctionManager::UnsubscribeSubscribers(ObjectMap *om)
{
   std::map<std::string, GmatBase *>::iterator omi;
   for (omi = om->begin(); omi != om->end(); ++omi)
   {
      if (omi->second != NULL)
      {
         if ((omi->second)->IsOfType(Gmat::SUBSCRIBER))
         {
            // Finalize subscriber
            Subscriber *sub = (Subscriber*)omi->second;
            sub->TakeAction("Finalize");
            
            #ifdef DEBUG_FM_SUBSCRIBER
            MessageInterface::ShowMessage
               ("   '%s' Unsubscribe <%p>'%s' from the publisher <%p>\n",
                fName.c_str(), sub, (omi->second)->GetName().c_str(), publisher);
            #endif
            
            // Unsubscribe subscriber
            if (publisher)
            {
               publisher->Unsubscribe(sub);
            }
            else
            {
               #ifdef DEBUG_FM_SUBSCRIBER
               MessageInterface::ShowMessage
                  ("   '%s' Cannot unsubscribe, the publisher is NULL\n", fName.c_str());
               #endif
            }
         }
      }
   }
}


//------------------------------------------------------------------------------
// bool EmptyObjectMap(ObjectMap *om, const std::string &mapID)
//------------------------------------------------------------------------------
bool FunctionManager::EmptyObjectMap(ObjectMap *om, const std::string &mapID)
{   
   if (om == NULL)
   {
      #ifdef DEBUG_OBJECT_MAP
      MessageInterface::ShowMessage
         ("in FM::EmptyObjectMap(), object map '%s' is NULL\n", mapID.c_str());
      #endif
      return true;
   }
   
   #ifdef DEBUG_CLEANUP
   MessageInterface::ShowMessage
      ("in FM::EmptyObjectMap(), object map '%s' <%p> had %u objects\n", mapID.c_str(),
       om, om->size());
   #endif
   
   StringArray toDelete;
   std::map<std::string, GmatBase *>::iterator omi;
   for (omi = om->begin(); omi != om->end(); ++omi)
   {
      if (omi->second != NULL)
      {
         if ((omi->second)->IsOfType(Gmat::SUBSCRIBER))
         {
            //=============================================================
            #ifdef __DO_NOT_DELETE_SUBSCRIBERS__
            //=============================================================
            
            // for now, don't delete subscribers as the Publisher still points to them and
            // bad things happen at the end of the run if they disappear
            #ifdef DEBUG_MEMORY
            GmatBase *obj = omi->second;
            MessageInterface::ShowMessage
               ("*** FunctionManager::EmptyObjectMap() should delete <%p> <%s> '%s'\n",
                obj, obj->GetTypeName().c_str(), obj->GetName().c_str());
            MessageInterface::ShowMessage
               ("*** For now, don't delete subscribers as the Publisher still points "
                "to them and bad things happen at the end of the run if they disappear.\n");
            #endif
            
            //=============================================================
            #else
            //=============================================================
            
            // Unsubscribe subscriber before deleting (LOJ: 2009.04.07)
            Subscriber *sub = (Subscriber*)omi->second;
            // Instead of deleting OpenGL plot from the OpenGlPlot destructor
            // call TakeAction() to delete it (LOJ:2009.04.22)
            sub->TakeAction("Finalize");
            //@todo This causes Func_AssigningWholeObjects crash
            // in Subscriber::ClearWrappers() due to stale wrapper pointers
            //sub->ClearWrappers();
            
            #ifdef DEBUG_CLEANUP
            MessageInterface::ShowMessage
               ("   '%s' Unsubscribe <%p>'%s' from the publisher <%p>\n",
                fName.c_str(), sub, (omi->second)->GetName().c_str(), publisher);
            #endif
            
            if (publisher)
            {
               publisher->Unsubscribe(sub);
            }
            else
            {
               #ifdef DEBUG_CLEANUP
               MessageInterface::ShowMessage
                  ("   '%s' Cannot unsubscribe, the publisher is NULL\n", fName.c_str());
               #endif
            }
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (omi->second, (omi->second)->GetName(), "FunctionManager::EmptyObjectMap()",
                "deleting subscriber from ObjectMap");
            #endif
            delete omi->second;
            omi->second = NULL;
            
            //=============================================================
            #endif
            //=============================================================
         }
         else
         {
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (omi->second, (omi->second)->GetName(), "FunctionManager::EmptyObjectMap()",
                "deleting obj from ObjectMap");
            #endif
            delete omi->second;
            omi->second = NULL;
         }
      }
      toDelete.push_back(omi->first); 
   }
   for (unsigned int kk = 0; kk < toDelete.size(); kk++)
   {
      #ifdef DEBUG_CLEANUP
      MessageInterface::ShowMessage
         ("   Erasing element with name '%s'\n", (toDelete.at(kk)).c_str());
      #endif
      om->erase(toDelete.at(kk));
   }
   om->clear();
   return true;
}

//------------------------------------------------------------------------------
// bool DeleteObjectMap(ObjectMap *om, const std::string &mapID)
//------------------------------------------------------------------------------
bool FunctionManager::DeleteObjectMap(ObjectMap *om, const std::string &mapID)
{
   if (om == NULL)
   {
      #ifdef DEBUG_CLEANUP
      MessageInterface::ShowMessage
         ("in FM::DeleteObjectMap(), object map %s is NULL\n", mapID.c_str());
      #endif
      return true;
   }
   
   #ifdef DEBUG_CLEANUP
   MessageInterface::ShowMessage
      ("in FM::DeleteObjectMap(), object map %s <%p> had %u objects\n", mapID.c_str(),
       om, om->size());
   #endif
   
   EmptyObjectMap(om, mapID);
   
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Remove
      (om, "om", "FunctionManager::DeleteObjectMap()", "deleting ObjectMap");
   #endif
   delete om;
   om = NULL;
   return true;
}

//------------------------------------------------------------------------------
// bool ClearInOutWrappers()
//------------------------------------------------------------------------------
bool FunctionManager::ClearInOutWrappers()
{
   // input wrappers map
   std::map<std::string, ElementWrapper *>::iterator ewi;
   for (ewi = inputWrapperMap.begin(); ewi != inputWrapperMap.end(); ++ewi)
   {
      if (ewi->second)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (ewi->second, (ewi->second)->GetDescription(),
             "FunctionManager::ClearInOutWrappers()", "deleting inputWrapper");
         #endif
         delete ewi->second;
         ewi->second = NULL;
      }
   }
   inputWrapperMap.clear();
   
   // output wrappers
   for (UnsignedInt i=0; i<outputWrappers.size(); i++)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (outputWrappers[i], outputWrappers[i]->GetDescription(),
          "FunctionManager::ClearInOutWrappers()", "deleting outputWrapper");
      #endif
      delete outputWrappers[i];
      outputWrappers[i] = NULL;
   }
   outputWrappers.clear();
   
   return true;
}


//------------------------------------------------------------------------------
// bool IsOnStack(ObjectMap *om)
//------------------------------------------------------------------------------
bool FunctionManager::IsOnStack(ObjectMap *om)
{
   if (callStack.empty())  return false;

   ObjectMap *tmp = callStack.top();
   //if (&tmp == &om) return true;  // is om the top element on the LIFO stack?
   if (tmp == om) return true;  // is om the top element on the LIFO stack?
   else           return false;
}

//------------------------------------------------------------------------------
// bool CloneObjectMap(ObjectMap *orig, ObjectMap *cloned)
//------------------------------------------------------------------------------
bool FunctionManager::CloneObjectMap(ObjectMap *orig, ObjectMap *cloned)
{
   if (orig == NULL || cloned == NULL)
      return false;

   ObjectMap::iterator omi;
   GmatBase *objInMap;
   std::string strInMap;
   
   cloned->clear();
   for (omi = orig->begin(); omi != orig->end(); ++omi)
   {
      strInMap = omi->first;
      objInMap = omi->second;
      
      // We don't want to insert blank name, so just commented out (loj: 2008.09.22)
      if (strInMap == "")
      {
         #ifdef DEBUG_FM_STACK
         MessageInterface::ShowMessage("PushToStack:: Found blank output name\n");
         #endif
         continue;
         //throw FunctionException
         //   ("PushToStack:: Cannot insert blank object name to object map\n");
      }
      
      #ifdef DEBUG_FM_STACK
         MessageInterface::ShowMessage(
            "PushToStack::Inserting clone of object \"%s\" into cloned object map\n",
            strInMap.c_str());
      #endif
      
      GmatBase *clonedObj = objInMap->Clone();
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (clonedObj, strInMap, "FunctionManager::CloneObjectMap()",
          "clonedObj = objInMap->Clone()");
      #endif
      cloned->insert(std::make_pair(strInMap, clonedObj));
   }
   
   return true;
}

//------------------------------------------------------------------------------
// bool CopyObjectMap(ObjectMap *from, ObjectMap *to)
//------------------------------------------------------------------------------
bool FunctionManager::CopyObjectMap(ObjectMap *from, ObjectMap *to)
{
   if (from == NULL || from == NULL)
      return false;
   
   ObjectMap::iterator omi;   
   to->clear();
   for (omi = from->begin(); omi != from->end(); ++omi)
      to->insert(std::make_pair(omi->first, omi->second));
   
   return true;
}

//------------------------------------------------------------------------------
// void ShowObjectMap(ObjectMap *om, const std::string &mapID)
//------------------------------------------------------------------------------
void FunctionManager::ShowObjectMap(ObjectMap *om, const std::string &mapID)
{
   std::string itsID = mapID;
   if (itsID == "") itsID = "unknown name";
   if (om)
   {
      MessageInterface::ShowMessage
         ("Object Map <%p> '%s' contains %u objects:\n", om, itsID.c_str(), om->size());
      if (om->size() > 0)
      {
         for (std::map<std::string, GmatBase *>::iterator i = om->begin();
              i != om->end(); ++i)
            MessageInterface::ShowMessage
               ("   name: %40s ...... pointer: %p...... object type: %s\n", i->first.c_str(),
                i->second, i->second == NULL ? "NULL" : (i->second)->GetTypeName().c_str());
      }
   }
   else
      MessageInterface::ShowMessage("Object Map '%s' is NULL\n", itsID.c_str());
}

//------------------------------------------------------------------------------
// void ShowStackContents(ObjectMapStack omStack, const std::string &stackID)
//------------------------------------------------------------------------------
void FunctionManager::ShowStackContents(ObjectMapStack omStack, const std::string &stackID)
{
   MessageInterface::ShowMessage("Showing stack contents: %s\n", stackID.c_str());
   ObjectMapStack temp;
   ObjectMap      *om;
   Integer        idx = 0;
   std::string    mapID;
   while (!(omStack.empty()))
   {
      mapID = GmatStringUtil::ToString(idx, 3);
      om = omStack.top();
      ShowObjectMap(om, mapID);
      temp.push(om);
      omStack.pop();
      idx++;
      mapID = "";
   }
   // put them back on the stack again
   while (!(temp.empty()))
   {
      om = temp.top();
      omStack.push(om);
      temp.pop();
   }
}

//------------------------------------------------------------------------------
// void ShowCallers(const std::string &label)
//------------------------------------------------------------------------------
/*
 * Shows contents of callers
 */
//------------------------------------------------------------------------------
void FunctionManager::ShowCallers(const std::string &label)
{
   std::stack<FunctionManager*> temp;
   FunctionManager *fm;
   Integer idx = 0;
   
   MessageInterface::ShowMessage("==================== %s\n", label.c_str());
   MessageInterface::ShowMessage
      ("   Call stack for this=<%p>'%s', there are %d FunctionManagers in stack\n",
       this, fName.c_str(), callers.size());
   
   while (!(callers.empty()))
   {
      fm = callers.top();
      MessageInterface::ShowMessage
         ("   caller %d, FM = <%p> '%s'\n", idx, fm,
          fm ? fm->GetFunctionName().c_str() : "NULL");
      temp.push(fm);
      callers.pop();
      idx++;
   }
   
   // put them back on the stack again
   while (!(temp.empty()))
   {
      fm = temp.top();
      callers.push(fm);
      temp.pop();
   }
   MessageInterface::ShowMessage("===============================\n");
}


//------------------------------------------------------------------------------
// void ShowTrace(Integer count, Integer t1, const std::string &label = "",
//                bool showMemoryTracks = false, bool addEol = false)
//------------------------------------------------------------------------------
void FunctionManager::ShowTrace(Integer count, Integer t1, const std::string &label,
                                bool showMemoryTracks, bool addEol)
{
   // To locally control debug output
   bool showTrace = false;
   bool showTracks = true;
   
   showTracks = showTracks & showMemoryTracks;
   
   if (showTrace)
   {
      #ifdef DEBUG_TRACE
      clock_t t2 = clock();
      MessageInterface::ShowMessage
         ("=== %s, '%s' Count = %d, elapsed time: %f sec\n", label.c_str(),
          fName.c_str(), count, (Real)(t2-t1)/CLOCKS_PER_SEC);
      #endif
   }
   
   if (showTracks)
   {
      #ifdef DEBUG_MEMORY
      StringArray tracks = MemoryTracker::Instance()->GetTracks(false, false);
      if (showTrace)
         MessageInterface::ShowMessage
            ("    ==> There are %d memory tracks\n", tracks.size());
      else
         MessageInterface::ShowMessage
            ("=== There are %d memory tracks when %s, '%s'\n", tracks.size(),
             label.c_str(), fName.c_str());
      
      if (addEol)
         MessageInterface::ShowMessage("\n");
      #endif
   }
}

