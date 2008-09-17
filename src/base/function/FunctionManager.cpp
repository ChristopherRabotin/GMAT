//$Id$
//------------------------------------------------------------------------------
//                                 FunctionManager
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
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
#include "RealTypes.hpp"
#include "RealUtilities.hpp"
#include "Variable.hpp"
#include "StringVar.hpp"

#define DO_NOT_EXECUTE_NESTED_GMAT_FUNCTIONS

//#define DEBUG_FUNCTION_MANAGER
//#define DEBUG_FM_INIT
//#define DEBUG_FM_EXECUTE
//#define DEBUG_FM_FINALIZE
//#define DEBUG_FM_STACK

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
   globalObjectStore (NULL),
   solarSys          (NULL),
   fName             (""),
   f                 (NULL),
   firstExecution    (true),
   numVarsCreated    (0),
   validator         (NULL),
   realResult        (-999.99),
   blankResult       (false),
   outputType        (""),
   objInit           (NULL),
   intCS             (NULL), 
   fcs               (NULL),
   current           (NULL)
{
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
   functionObjectStore.clear();
   //inputWrappers.clear();
   outputWrappers.clear();
   if (objInit) delete objInit;
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
   functionObjectStore (fm.functionObjectStore),
   globalObjectStore   (fm.globalObjectStore),
   solarSys            (fm.solarSys),
   forces              (fm.forces),
   fName               (fm.fName),
   f                   (fm.f), // copy the pointer here
   ins                 (fm.ins),
   outs                (fm.outs),
   //inputWrappers       (fm.inputWrappers),
   outputWrappers      (fm.outputWrappers), // is that right?
   firstExecution      (true),
   numVarsCreated      (fm.numVarsCreated),
   validator           (NULL),
   realResult          (fm.realResult),
   matResult           (fm.matResult),
   blankResult         (fm.blankResult),
   outputType          (fm.outputType),
   objInit             (NULL),
   intCS               (fm.intCS), 
   fcs                 (NULL),
   current             (NULL)
{
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
      functionObjectStore = fm.functionObjectStore;
      globalObjectStore   = fm.globalObjectStore; // is that right?
      solarSys            = fm.solarSys;
      forces              = fm.forces;
      fName               = fm.fName;
      f                   = fm.f;  // copy the pointer here
      ins                 = fm.ins;
      outs                = fm.outs;
      firstExecution      = true;
      numVarsCreated      = fm.numVarsCreated;
      validator           = NULL;
      //inputWrappers       = fm.inputWrappers; // is that right?
      outputWrappers      = fm.outputWrappers; // is that right?
      realResult          = fm.realResult;
      matResult           = fm.matResult;
      blankResult         = fm.blankResult;
      outputType          = fm.outputType;
      objInit             = NULL;  
      intCS               = fm.intCS;  // right?
      fcs                 = NULL;
      current             = NULL;
   }
   
   return *this;
}

void FunctionManager::SetObjectMap(std::map<std::string, GmatBase *> *map)
{
   localObjectStore = map;
}

void FunctionManager::SetGlobalObjectMap(std::map<std::string, GmatBase *> *map)
{
   globalObjectStore = map;
}

void FunctionManager::SetSolarSystem(SolarSystem *ss)
{
   solarSys = ss;
   if ((f) && (f->GetTypeName() == "GmatFunction"))   f->SetSolarSystem(ss);
}

void FunctionManager::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
   forces = tf;
   if ((f) && (f->GetTypeName() == "GmatFunction"))   f->SetTransientForces(tf);
}

void FunctionManager::SetFunctionName(const std::string &itsName)
{
   #ifdef DEBUG_FUNCTION_MANAGER
      MessageInterface::ShowMessage("entering FM::SetFunctionName with name = %s\n", itsName.c_str());
   #endif
   fName = itsName;
   if ((f) && (f->GetTypeName() == "GmatFunction"))  
   {
      #ifdef DEBUG_FUNCTION_MANAGER
         MessageInterface::ShowMessage(" ... and setting name to %s on the function\n", itsName.c_str());
      #endif
      f->SetStringParameter("FunctionName", itsName);
   }
}

std::string FunctionManager::GetFunctionName() const
{
   if (f)   
   {
      std::string theFunctionName = f->GetStringParameter("FunctionName");
      #ifdef DEBUG_FUNCTION_MANAGER
         MessageInterface::ShowMessage(" ... and getting name of %s from the function\n", theFunctionName.c_str());
      #endif
      //return f->GetStringParameter("FunctionName");
   }
   return fName; // why not return theFunctionName here?
}

void FunctionManager::SetFunction(Function *theFunction)
{
   #ifdef DEBUG_FUNCTION_MANAGER
   MessageInterface::ShowMessage
      ("FunctionManager::SetFunction() fName='%s', theFunction=<%p>\n", fName.c_str(), theFunction);
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

Function* FunctionManager::GetFunction() const
{
   return f;
}
void FunctionManager::AddInput(const std::string &withName, Integer atIndex)
{
   // -999 means to put it at the end of the list (default value/behavior)
   if ((atIndex == -999) || (atIndex == (Integer) ins.size()))
   {
      ins.push_back(withName);
      return;
   }
   
   if ((atIndex < 0) || (atIndex > (Integer) ins.size()))
      throw FunctionException("FunctionManager:: input index out of range - unable to set.\n");
   // replace an entry
   ins.at(atIndex) = withName;   
}

void FunctionManager::AddOutput(const std::string &withName, Integer atIndex)
{
   // -999 means to put it at the end of the list (default value/behavior)
   if ((atIndex == -999) || (atIndex == (Integer) outs.size()))
   {
      outs.push_back(withName);
      return;
   }
   
   if ((atIndex < 0) || (atIndex > (Integer) outs.size()))
      throw FunctionException("FunctionManager:: input index out of range - unable to set.\n");
   // replace an entry
   outs.at(atIndex) = withName;   
}

void FunctionManager::SetInputs(const StringArray &inputs)
{
   ins = inputs;
}

void FunctionManager::SetOutputs(const StringArray &outputs)
{
   outs = outputs;
}

StringArray  FunctionManager::GetOutputs()
{
   return outs;
}

void FunctionManager::SetInternalCoordinateSystem(CoordinateSystem *internalCS)
{
   intCS = internalCS;
   if ((f) && (f->GetTypeName() == "GmatFunction"))   f->SetInternalCoordSystem(intCS);
}


// Sequence methods

bool FunctionManager::Execute(FunctionManager *callingFM)
{
   #ifdef DEBUG_FM_EXECUTE
      MessageInterface::ShowMessage("Entering FM::Execute for %s\n", fName.c_str());
      MessageInterface::ShowMessage("   and the calling function is %s NULL\n",
            callingFM? "NOT": "");
      if (callingFM != NULL)
         MessageInterface::ShowMessage("The calling function is %s\n",
               (callingFM->GetFunctionName()).c_str());
   #endif
   if (f == NULL)
   {
      std::string errMsg = "FunctionManager:: Unable to execute Function """;
      errMsg += fName + """ - pointer is NULL\n";
      throw FunctionException(errMsg);
   }
   GmatBase *obj;
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
   validator->SetObjectMap(&combinedObjectStore);
   #ifdef DEBUG_FUNCTION_MANAGER
      MessageInterface::ShowMessage("in FM::Execute - just set Validator's object map\n");
   #endif
   
   StringArray inNames = f->GetStringArrayParameter(f->GetParameterID("Input"));
   #ifdef DEBUG_FM_EXECUTE
      MessageInterface::ShowMessage("in FM::Execute \n");
      for (unsigned int rr = 0; rr < inNames.size(); rr++)
         MessageInterface::ShowMessage("  inNames[%d] = %s\n", rr, (inNames.at(rr)).c_str());
      if (ins.size() == 0) MessageInterface::ShowMessage("NOTE - ins is empty - is it supposed to be?\n");
      else
         for (unsigned int qq = 0; qq < ins.size(); qq++)
            MessageInterface::ShowMessage("  ins[%d] = %s\n", qq, (ins.at(qq)).c_str());
      if (outs.size() == 0) MessageInterface::ShowMessage("NOTE - outs is empty - is it supposed to be?\n");
      else
         for (unsigned int qq = 0; qq < outs.size(); qq++)
            MessageInterface::ShowMessage("  outs[%d] = %s\n", qq, (outs.at(qq)).c_str());
   #endif

   callers.push(callingFunction);
   callingFunction  = callingFM;
   if (callingFunction != NULL)
   {
      #ifdef DO_NOT_EXECUTE_NESTED_GMAT_FUNCTIONS
         Finalize(); // memory usage keep growing so try Finalize here (loj: 2008.09.11)
         std::string noNested = "FunctionManager (";
         noNested += fName + ") - nested functions not yet supported.\n";
         throw FunctionException(noNested);
      #else 
      losStack.push(localObjectStore);
      localObjectStore = callingFunction->PushToStack();
      firstExecution   = true; // so that it will initialize
      #endif
   }
      
   if (firstExecution)
   {
      Initialize();
      firstExecution = false;
   }
   else
   {
      // Need to delete all items in the FOS that are not inputs (so that they can 
      // properly be created again in the FCS 
      StringArray toDelete;
      bool        isInput = false;
      for (omi = functionObjectStore.begin(); omi != functionObjectStore.end(); ++omi)
      {
         isInput = false;
         for (unsigned int qq = 0; qq < inNames.size(); qq++)
            if (omi->first == inNames.at(qq))
            {
               isInput = true;
               break;
            }
         if (!isInput) 
         {  
            if (omi->second != NULL)
            {
               delete omi->second;
               omi->second = NULL;
            }
            toDelete.push_back(omi->first); 
         }
      }
      for (unsigned int kk = 0; kk < toDelete.size(); kk++)
      {
         functionObjectStore.erase(toDelete.at(kk));
      }
      // Find and/or evaluate the input objects
      for (unsigned int ii=0; ii<ins.size(); ii++)
      {
         // Get the object from the object store first 
         //objName = f->GetStringParameter("Input", ii);
         objName = inNames.at(ii);
         if (functionObjectStore.find(objName) == functionObjectStore.end())
         {
            std::string errMsg = "FunctionManager error: input object \"" + objName;
            errMsg += "\"not found in Function Object Store.\n";
            throw FunctionException(errMsg);
         }
         GmatBase *fosObj = functionObjectStore[objName];
         // Now find the corresponding input object
         if (!(obj = FindObject(ins.at(ii))))
         {
            if (createdLiterals.find(ins.at(ii)) == createdLiterals.end())
            {
               if (createdOthers.find(ins.at(ii)) == createdOthers.end())
               {
                  std::string errMsg = "Input \"" + ins.at(ii);
                  errMsg += " not found for function \"" + fName + "\"";
                  throw FunctionException(errMsg);
               }
               GmatBase *tmpObj  = fosObj;
               GmatBase *tmpObj2 = createdOthers[ins.at(ii)];
               obj               = CreateObject(ins.at(ii));
               if (!obj)
               {
                  std::string errMsg2 = "FunctionManager: Object not found or created for input string \"";
                  errMsg2 += ins.at(ii) + "\" for function \"";
                  errMsg2 += fName + "\"\n";
                  throw FunctionException(errMsg2);
               }
               createdOthers[ins.at(ii)] = obj;
               fosObj = obj->Clone();
               fosObj->SetName(objName);
               delete tmpObj;
               delete tmpObj2;
            }
            else
               obj = createdLiterals[ins.at(ii)];
         }
         // Update the object in the object store with the current/reset data
         fosObj->Copy(obj);
         (inputWrappers[objName])->SetRefObject(fosObj);  // is this necessary? I think so
      }
   } // end if not first time through
   
   // pass the FOS into the function
   f->SetObjectMap(&functionObjectStore);
   f->SetGlobalObjectMap(globalObjectStore);
   f->SetSolarSystem(solarSys);
   f->SetTransientForces(forces);
   // send all input element wrappers to the function
   std::map<std::string, ElementWrapper *>::iterator ewi;
   for (ewi = inputWrappers.begin(); ewi != inputWrappers.end(); ++ewi)
      f->SetInputElementWrapper(ewi->first, ewi->second);
   // must re-initialize the function each time, as it may be called in more than
   // one place
   if (!(f->Initialize()))
   {
      std::string errMsg = "FunctionManager:: Error initializing function \"";
      errMsg += f->GetStringParameter("FunctionName") + "\"\n";
      throw FunctionException(errMsg);
   }
   // -- ****************************************************************************************
   // @todo - this probably needs to be moved to the right spot!!!! 
//   if (!objInit)  
//      objInit = new ObjectInitializer(solarSys, &functionObjectStore, globalObjectStore, intCS, true);
//   else
//   {
//      objInit->SetObjectMap(&functionObjectStore);
//      objInit->SetCoordinateSystem(intCS);
//   }
   if (objInit) delete objInit;
   objInit = new ObjectInitializer(solarSys, &functionObjectStore, globalObjectStore, intCS, true);
   // tell the fcs that this is the calling function
   #ifdef DEBUG_FM_EXECUTE
      MessageInterface::ShowMessage(
            "in FM::Execute (%s), about to set calling function on commands\n",
            fName.c_str());
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
      cmd = cmd->GetNext();
   }
   // Now, execute the function
   f->Execute(objInit);
   // -- ****************************************************************************************
   // @todo - here is the placeholder for calling the sequence from the FunctionManager ...
//   bool isFunction = false;
//   current = fcs;
//   while (current)
//   {
//      isFunction = current->HasAFunction();
//      #ifdef DEBUG_FM_EXECUTE
//            MessageInterface::ShowMessage("In FunctionManager execute loop and next command (%s) %s a function call.\n",
//                  (current->GetTypeName()).c_str(), isFunction? "HAS" : "DOES NOT HAVE");
//      #endif
//      if (isFunction)
//      {
//         ; // @todo - call stack stuff or function-cloning stuff goes here
//      }
//      if (!(current->Execute()))
//         return false;
//      if (isFunction)
//      {
//         ; // @todo - call stack stuff or function-cloning stuff goes here
//      }
//      current = current->GetNext();
//   }
   // @todo - need to figure out how to get output data if calling the sequence from here ...
   // -- ****************************************************************************************
   // Now get the output data
   
   ElementWrapper *ew;
   
   if (blankResult)  SaveLastResult();
   else
   {
      for (unsigned int jj = 0; jj < outputWrappers.size(); jj++)
      {
         bool retval = false;
         ew = f->GetOutputArgument(jj);
         retval = ElementWrapper::SetValue(outputWrappers.at(jj), ew, solarSys,
                                           &functionObjectStore, globalObjectStore);
         if (!retval)
            throw FunctionException
               ("FunctionManager::Execute() failed to assign results to function output");
      }
   }
   // deal with the calling function here
   if (callingFunction != NULL)
   {
      StringArray outNames = f->GetStringArrayParameter(f->GetParameterID("Output"));
      // call the caller to pop its FOS back of the stack
      callingFunction->PopFromStack(&functionObjectStore, outNames, outs);
      // restore the localObjectStore
      localObjectStore = losStack.top();
      losStack.pop();
      // remove the caller from the stack of callers
      //callers.pop(); 
      firstExecution = true;   // to make sure it is reinitialized next time ???
   }
   callingFunction = callers.top();
   callers.pop();
   
   return true;
}

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
      ("==> FunctionManager::Evaluate() f=<%p><%s>\n", f, f->GetName().c_str());
   #endif
   
   Execute(callingFM);
   if (outputType != "Real")
      throw FunctionException("FunctionManager: invalid output type - should be real\n");
   return realResult;
   //return f->Evaluate();
}

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
      ("==> FunctionManager::MatrixEvaluate() f=<%p><%s>\n", f, f->GetName().c_str());
   #endif
   
   Execute(callingFM);
   if (outputType != "Rmatrix")
      throw FunctionException("FunctionManager: invalid output type - should be Rmatrix\n");
   return matResult;
   //return f->MatrixEvaluate();
}

void FunctionManager::Finalize()
{
   std::map<std::string, GmatBase *>::iterator omi;
   #ifdef DEBUG_FM_FINALIZE
      MessageInterface::ShowMessage("Entering FM::Finalize ...\n");
      for (omi = functionObjectStore.begin(); omi != functionObjectStore.end(); ++omi)
         MessageInterface::ShowMessage("   entry for \"%s\" is %p\n",
               (omi->first).c_str(), omi->second);
   #endif
   if (f != NULL)
      if (f->IsOfType("GmatFunction")) //loj: added check for GmatFunction
         f->Finalize();
   // now delete all of the items/entries in the FOS - we can do this since they 
   // are all either locally-created or clones of reference objects or automatic objects
   //EmptyObjectMap(&functionObjectStore); // do I need to do this?????
   firstExecution = true;
}

//---------------------------------
// protected methods
//---------------------------------

bool FunctionManager::Initialize()
{
   #ifdef DEBUG_FM_INIT
      MessageInterface::ShowMessage("Entering FM::Initialize for %s\n", fName.c_str());
   #endif
   if (!firstExecution) return true;
   
   GmatBase *obj, *objFOS;
   std::string objName;
   
   StringArray inNames = f->GetStringArrayParameter(f->GetParameterID("Input"));
   StringArray outNames = f->GetStringArrayParameter(f->GetParameterID("Output"));
   
   #ifdef DEBUG_FM_INIT
   MessageInterface::ShowMessage
      ("   inNames.size=%d, ins.size=%d\n", inNames.size(), ins.size());
   MessageInterface::ShowMessage
      ("   outNames.size=%d, outs.size=%d\n", outNames.size(), outs.size());
   #endif
   
   // check input arguments
   // Number of inputs cannot be more than number of formal input arguments,
   // but can we have less than formal input arguments?
   // Can we have default input value?
   // Currently if number of input is less than formal, it is created as String,
   // so number of inputs must match for now.
   if (ins.size() != inNames.size())
   {
      FunctionException ex;
      ex.SetDetails("The function '%s' expecting %d input argument(s), but called "
                    "with %d argument(s)", f->GetFunctionPathAndName().c_str(),
                    inNames.size(), ins.size());
      throw ex;
   }
   
   // check output arguments   
   // Number of outputs cannot be more than number of formal output arguments,
   // but can we have less than formal output arguments?
   // Let's just check for more than formal output for now.
   if (outs.size() > outNames.size())
   {
      FunctionException ex;
      ex.SetDetails("The function '%s' expecting %d output argument(s), but called "
                    "with %d argument(s)", f->GetFunctionPathAndName().c_str(),
                    outNames.size(), outs.size());
      throw ex;
   }
   
   validator->SetObjectMap(&combinedObjectStore);
   validator->SetSolarSystem(solarSys);
   
   if (!(IsOnStack(&functionObjectStore))) // ********** ???? *************
      EmptyObjectMap(&functionObjectStore);
   functionObjectStore.clear();
   //inputWrappers.clear();
   outputWrappers.clear();
   numVarsCreated = 0;
   // set up the FOS with the input objects
   for (unsigned int ii=0; ii<ins.size(); ii++)
   {
      // if the input string does not refer to an object that can be located in the LOS or
      // GOS, we must try to create an object for it, if it makes sense (e.g. numeric literal, 
      // string literal, array element)
      if (!(obj = FindObject(ins.at(ii))))
      {
         obj = CreateObject(ins.at(ii));
         if (!obj)
         {
            std::string errMsg = "FunctionManager: Object not found or created for input string \"";
            errMsg += ins.at(ii) + "\" for function \"";
            errMsg += fName + "\"\n";
            throw FunctionException(errMsg);
         }
      }
      #ifdef DEBUG_FM_INIT
         MessageInterface::ShowMessage(
               "in FM::Initialize: object \"%s\" of type \"%s\" found in LOS/GOS \n",
               (ins.at(ii)).c_str(), (obj->GetTypeName()).c_str());
      #endif
      // Clone the object, and insert it into the FOS
      objFOS = obj->Clone();
      objName = inNames.at(ii);
      objFOS->SetName(objName);
      functionObjectStore.insert(std::make_pair(objName,objFOS));
      #ifdef DEBUG_FM_INIT // ------------------------------------------------- debug ---
         MessageInterface::ShowMessage("Adding object %s to the FOS\n", objName.c_str());
      #endif // -------------------------------------------------------------- end debug ---
      // create an element wrapper for the input
      validator->SetObjectMap(&combinedObjectStore);
      validator->SetSolarSystem(solarSys);
      ElementWrapper *inWrapper = validator->CreateElementWrapper(ins.at(ii), false, false);
      inWrapper->SetRefObject(objFOS);
      inputWrappers.insert(std::make_pair(objName, inWrapper));
      #ifdef DEBUG_FM_INIT // ------------------------------------------------- debug ---
         MessageInterface::ShowMessage("Created element wrapper of type %d for \"%s\"",
               inWrapper->GetWrapperType(), (ins.at(ii)).c_str());
      #endif // -------------------------------------------------------------- end debug ---
   }
   // Outputs cannot be numeric or string literals or array elements, etc.
   // They must be found in the object store; and we do not need to clone them
   // Handle the case with one blank input (called from FunctionRunner) first
   outputWrappers.clear();
   if ((outs.size() == 1) && (outs.at(0) == ""))
      blankResult = true;
   else
   {
      for (unsigned int jj = 0; jj < outs.size(); jj++)
      {
         if (!(obj = FindObject(outs.at(jj))))
         {
            std::string errMsg = "Output \"" + outs.at(jj);
            errMsg += " not found for function \"" + fName + "\"";
            throw FunctionException(errMsg);
         }
         validator->SetObjectMap(&combinedObjectStore);
         validator->SetSolarSystem(solarSys);
         ElementWrapper *outWrapper = validator->CreateElementWrapper(outs.at(jj));
         outWrapper->SetRefObject(obj); 
         outputWrappers.push_back(outWrapper);
         #ifdef DEBUG_FM_INIT // ------------------------------------------------- debug ---
            MessageInterface::ShowMessage("Output wrapper set for %s\n", (outs.at(jj)).c_str());
         #endif // -------------------------------------------------------------- end debug ---
      }
   }
   // get a pointer to the function's function control sequence
   return true;
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
      else return (*globalObjectStore)[newName];
   }
   else
      return (*localObjectStore)[newName];
   return NULL; // should never get to this point
}

GmatBase* FunctionManager::CreateObject(const std::string &fromString)
{
   
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
      v->SetReal(rval);
      obj = (GmatBase*) v;
      createdLiterals.insert(std::make_pair(str,obj));
   }
   // Now check to see if it is a string literal, enclosed with single quotes
   else if ((str[0] == '\'') && (str[sz-1] == '\''))
   {
      sval = str.substr(1, sz-2);
      StringVar *sv = new StringVar(str);
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
      ElementWrapper *ew = validator->CreateElementWrapper(fromString);
      if (ew)
      {
         Gmat::WrapperDataType wType = ew->GetWrapperType();
         switch (wType)
         {
            case Gmat::ARRAY :
            {
               Array *array   = new Array(str);
               Rmatrix matVal = ew->EvaluateArray();
               array->SetRmatrix(matVal);
               obj = (GmatBase*) array;
               break;
            }
            case Gmat::ARRAY_ELEMENT :
            {
               // get the reference object first ...
               refObj = FindObject(fromString, true);
               if (!refObj)
               {
                  std::string errMsg = "FunctionManager: Error getting reference object for array element ";
                  errMsg += fromString + "\n";
                  throw FunctionException(errMsg);
               }
               ew->SetRefObject(refObj);
               rval = ew->EvaluateReal(); 
               v = new Variable(str);
               v->SetReal(rval);
               obj = (GmatBase*) v;
               break;
            }
            case Gmat::NUMBER :
            case Gmat::VARIABLE :
            case Gmat::INTEGER :       // what is this anyway?
            {
               rval = ew->EvaluateReal(); 
               v = new Variable(str);
               v->SetReal(rval);
               obj = (GmatBase*) v;
               break;
            }
            default:
            {
               std::string errMsg = "FunctionManager: error using string \"";
               errMsg += fromString + "\" to determine an object\n";
               throw FunctionException(errMsg);
               break;
            }
         }
      }
      if (obj) createdOthers.insert(std::make_pair(str, obj));
   }
   if (obj) numVarsCreated++;
   return obj;
}

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
   switch (outType) 
   {
   case Gmat::INTEGER_TYPE:
      realResult = (Real) ew->EvaluateInteger();
      outputType = "Real";
      break;
   case Gmat::REAL_TYPE:
      realResult = ew->EvaluateReal();
      outputType = "Real";
      break;
   case Gmat::RMATRIX_TYPE:
      matResult = ew->EvaluateArray();
      outputType = "Rmatrix";
      break;
   default:
      throw FunctionException("FunctionManager: Unknown or invalid output data type");
   }
}

//void FunctionManager::SetCallingFunction(FunctionManager *fm)
//{
//   callers.push(fm);
//}

ObjectMap* FunctionManager::PushToStack()
{
   #ifdef DEBUG_FM_STACK
      MessageInterface::ShowMessage(
            "PushToStack::Entering PushToStack for function %s\n",
            fName.c_str());
      ShowStackContents(callStack, "Stack contents at beg. of PushToStack");
   #endif
   // Clone the FOS
   ObjectMap *cloned = new ObjectMap();
   std::map<std::string, GmatBase *>::iterator omi;
   GmatBase *objInMap;
   std::string strInMap;
   #ifdef DEBUG_FM_STACK
      ShowObjectMap(&functionObjectStore, "FOS at beg. of PushToStack");
   #endif
   for (omi = functionObjectStore.begin(); omi != functionObjectStore.end(); ++omi)
   {
      strInMap = omi->first;
      objInMap = omi->second;
      
      if (strInMap == "")
         throw FunctionException
            ("PushToStack:: Cannot insert blank object name to object map\n");
      
      #ifdef DEBUG_FM_STACK
         MessageInterface::ShowMessage(
            "PushToStack::Inserting clone of object \"%s\" into cloned object map\n",
            strInMap.c_str());
      #endif
      cloned->insert(std::make_pair(strInMap, objInMap->Clone()));
   }
   // Put the FOS onto the stack
   callStack.push(&functionObjectStore);
   #ifdef DEBUG_FM_STACK
      MessageInterface::ShowMessage(
            "PushToStack::Exiting PushToStack for function %s\n",
            fName.c_str());
      ShowObjectMap(cloned, "Cloned map at end of PushToStack");
      ShowStackContents(callStack, "Stack contents at end of PushToStack");
   #endif
   // Return the clone, to be used as the LOS for the FM that called this method
   return cloned;
}

void FunctionManager::PopFromStack(ObjectMap* cloned, const StringArray &outNames,
                                   const StringArray &callingNames)
{
   #ifdef DEBUG_FM_STACK
      MessageInterface::ShowMessage(
            "PopFromStack::Entering PopFromStack for function %s\n",
            fName.c_str());
      ShowStackContents(callStack, "Stack contents at beg. of PopFromStack");
   #endif
   if (callStack.empty())
      throw FunctionException(
            "ERROR popping object store from call stack - stack is empty\n");
   ObjectMap *topMap = callStack.top();
   callStack.pop();
   functionObjectStore = *topMap;
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
      ShowObjectMap(&functionObjectStore, "FOS");
   #endif
   for (unsigned int jj = 0; jj < outNames.size(); jj++)
   {
      GmatBase *clonedObj = (*cloned)[outNames.at(jj)];
      GmatBase *fosObj    = functionObjectStore[callingNames.at(jj)];
      //GmatBase *fosObj    = functionObjectStore[outNames.at(jj)];
      if (clonedObj == NULL)
      {
         std::string errMsg = "PopFromStack::Error getting output named ";
         errMsg += outNames.at(jj) + " from cloned map in function \"";
         errMsg += fName + "\"\n";
         throw FunctionException(errMsg);
      }
      if (fosObj == NULL)
      {
         std::string errMsg = "PopFromStack::Error setting output named \"";
         errMsg += callingNames.at(jj) + "\" from nested function on function \"";
         errMsg += fName + "\"\n";
         throw FunctionException(errMsg);
      }
      #ifdef DEBUG_FM_STACK
         MessageInterface::ShowMessage(
               "PopFromStack::Copying value of %s into popped stack element %s\n",
               (outNames.at(jj)).c_str(), (callingNames.at(jj)).c_str());
      #endif
      fosObj->Copy(clonedObj);
   }
   DeleteObjectMap(cloned);
   //firstExecution = true;
   //Initialize();
   #ifdef DEBUG_FM_STACK
      MessageInterface::ShowMessage(
            "PopFromStack::reset object map to the one popped from the stack\n");
      MessageInterface::ShowMessage(
            "PopFromStack::now about to re-initialize the function\n");
   #endif
   f->SetObjectMap(&functionObjectStore);
   f->Initialize();
   #ifdef DEBUG_FM_STACK
      MessageInterface::ShowMessage(
            "PopFromStack::Exiting PopFromStack for function %s\n",
            fName.c_str());
      ShowStackContents(callStack, "Stack contents at end of PopFromStack");
   #endif
}

bool FunctionManager::EmptyObjectMap(ObjectMap *om)
{   
   StringArray toDelete;
   std::map<std::string, GmatBase *>::iterator omi;
   for (omi = om->begin(); omi != om->end(); ++omi)
   {
      if (omi->second != NULL)
      {
         #ifdef DEBUG_FUNCTION_MANAGER
         GmatBase *obj = omi->second;
         MessageInterface::ShowMessage
            ("   Deleting <%p> <%s> '%s'\n", obj, obj->GetTypeName().c_str(),
             obj->GetName().c_str());
         #endif
         // for now, don't delete subscribers as the Publisher still points to them and
         // bad things happen at the end of the run if they disappear
         if (!((omi->second)->IsOfType("Subscriber")))
            delete omi->second;
         omi->second = NULL;
      }
      toDelete.push_back(omi->first); 
   }
   for (unsigned int kk = 0; kk < toDelete.size(); kk++)
   {
      #ifdef DEBUG_FUNCTION_MANAGER
      MessageInterface::ShowMessage
         ("   Erasing element with name '%s'\n", (toDelete.at(kk)).c_str());
      #endif
      om->erase(toDelete.at(kk));
   }
   om->clear();
   return true;
}

bool FunctionManager::DeleteObjectMap(ObjectMap *om)
{  
   EmptyObjectMap(om);
   #ifdef DEBUG_FUNCTION_MANAGER
   MessageInterface::ShowMessage
      ("   Deleting object map\n");
   #endif

   delete om;
   return true;
}

bool FunctionManager::IsOnStack(ObjectMap *om)
{
   if (callStack.empty())  return false;

   ObjectMap *tmp = callStack.top();
   //if (&tmp == &om) return true;  // is om the top element on the LIFO stack?
   if (tmp == om) return true;  // is om the top element on the LIFO stack?
   else           return false;
}

void FunctionManager::ShowObjectMap(ObjectMap *om, const std::string &mapID)
{
   std::string itsID = mapID;
   if (itsID == "") itsID = "unknown name";
   if (om)
   {
//      MessageInterface::ShowMessage
//         ("Object map (%s) contents:\n", itsID.c_str());
      MessageInterface::ShowMessage("Object Map contains:\n");
      for (std::map<std::string, GmatBase *>::iterator i = om->begin();
           i != om->end(); ++i)
         MessageInterface::ShowMessage
            ("   name: %30s ...... pointer: %p...... object type: %s\n", i->first.c_str(), i->second,
             i->second == NULL ? "NULL" : (i->second)->GetTypeName().c_str());
   }
}

void FunctionManager::ShowStackContents(ObjectMapStack omStack, const std::string &stackID)
{
   MessageInterface::ShowMessage("Showing stack contents: %s\n", stackID.c_str());
   ObjectMapStack temp;
   ObjectMap      *om;
   Integer        idx = 0;
   std::string    mapID;
   while (!(omStack.empty()))
   {
      mapID = "Object map " + idx;
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

