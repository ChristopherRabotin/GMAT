//$Header$
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


#include "FunctionManager.hpp"
#include "MessageInterface.hpp"
#include "FunctionException.hpp"
#include "StringUtil.hpp"
#include "Array.hpp"
#include "ElementWrapper.hpp"

//#define DEBUG_FUNCTION_MANAGER

//---------------------------------
// static data
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
   firstExecution    (true)
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
   globalObjectStore   (NULL),
   solarSys            (fm.solarSys),
   forces              (fm.forces),
   fName               (fm.fName),
   f                   (NULL),  // is that right?
   ins                 (fm.ins),
   outs                (fm.outs),
   firstExecution      (true),
   outObjects          (fm.outObjects) // is that right?
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
      f                   = NULL;  // is that right?
      ins                 = fm.ins;
      outs                = fm.outs;
      firstExecution      = true;
      outObjects          = fm.outObjects; // is that right?
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
   return fName;
}

void FunctionManager::SetFunction(Function *theFunction)
{
   f = theFunction;
   f->SetStringParameter("FunctionName", fName);
   // need to check to see if it's a GmatFunction here?  why?
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

// Sequence methods
//virtual bool         Initialize();
bool FunctionManager::Execute()
{
   GmatBase *obj, *itsClone;
   std::string objName;
   if (f == NULL)
   {
      std::string errMsg = "FunctionManager:: Unable to execute Function """;
      errMsg += fName + """ - pointer is NULL\n";
      throw FunctionException(errMsg);
   }
   // Initialize the Validator - I think I need to do this each time - or do I?
   validator.SetSolarSystem(solarSys);
   combinedObjectStore = localObjectStore;
   std::map<std::string, GmatBase *>::iterator omi;
   for (omi = globalObjectStore->begin(); omi != globalObjectStore->end(); ++omi)
      combinedObjectStore->insert(std::make_pair(omi->first, omi->second));
   validator.SetObjectMap(combinedObjectStore);
   #ifdef DEBUG_FUNCTION_MANAGER
      MessageInterface::ShowMessage("in FM::Execute - just set Validator's object map\n");
   #endif
   
   if (firstExecution)
   {
      #ifdef DEBUG_FUNCTION_MANAGER
         MessageInterface::ShowMessage("in FM::Execute - firstExecution\n");
      #endif
      functionObjectStore.clear();
      outObjects.clear();
      // set up the FOS with the input objects
      for (unsigned int ii=0; ii<ins.size(); ii++)
      {
         if (!(obj = FindObject(ins.at(ii))))
         {
            std::string errMsg = "Input \"" + ins.at(ii);
            errMsg += " not found for function \"" + fName + "\"";
            throw FunctionException(errMsg);
         }
         itsClone = obj->Clone();
         objName = f->GetStringParameter("Input", ii);
         functionObjectStore.insert(std::make_pair(objName,obj));
         ElementWrapper *wrapper = validator.CreateElementWrapper(objName);
         bool inputOK = f->SetInputElementWrapper(objName, wrapper);
         if (!inputOK)
         {
            std::string errMsg = "Error setting ElementWrapper on function \"" + fName;
            errMsg += "\" for input \"" + objName + "\"\n";
            throw FunctionException(errMsg);
         }
      }
      for (unsigned int jj = 0; jj < outs.size(); jj++)
      {
         if (!(obj = FindObject(outs.at(jj))))
         {
            std::string errMsg = "Output \"" + outs.at(jj);
            errMsg += " not found for function \"" + fName + "\"";
            throw FunctionException(errMsg);
         }
         outObjects.push_back(obj);
      }
      f->SetGlobalObjectMap(globalObjectStore);
      f->SetSolarSystem(solarSys);
      f->SetTransientForces(forces);
      firstExecution = false;
   }
   else
   {
      ; // do stuff here to re-setup the FOS, etc.
        // TBD
   }
   // pass the FOS into the function
   f->SetObjectMap(&functionObjectStore);
   // must re-initialize the function each time, as it may be called in more than
   // one place
   if (!(f->Initialize()))
   {
      std::string errMsg = "FunctionManager:: Error initializing function \"";
      errMsg += f->GetStringParameter("FunctionName") + "\"\n";
      throw FunctionException(errMsg);
   }
   return f->Execute();
   // retrieve the output data here ................
   // TBD
   // delete the clones here ...
   // TBD
}

Real FunctionManager::Evaluate()
{
   if (f == NULL)
   {
      std::string errMsg = "FunctionManager:: Unable to return Real value from Function """;
      errMsg += fName + """ - pointer is NULL\n";
      throw FunctionException(errMsg);
   }
   return f->Evaluate();
}

Rmatrix FunctionManager::MatrixEvaluate()
{
   {
      if (f == NULL)
      {
         std::string errMsg = "FunctionManager:: Unable to return Rmatrix value from Function """;
         errMsg += fName + """ - pointer is NULL\n";
         throw FunctionException(errMsg);
      }
      return f->MatrixEvaluate();
   }
}

//------------------------------------------------------------------------------
// GmatBase* FunctionManager::FindObject(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* FunctionManager::FindObject(const std::string &name)
{
   std::string newName = name;
   
   // Ignore array indexing of Array
   std::string::size_type index = name.find('(');
   if (index != name.npos)
      newName = name.substr(0, index);
   // Check for the object in the Local Object Store (LOS) first
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

