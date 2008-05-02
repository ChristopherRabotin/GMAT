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

void FunctionManager::SetGlobalObjectMap(std::map<std::string, GmatBase *> *map)
{
   globalObjectStore = map;
}

void FunctionManager::SetSolarSystem(SolarSystem *ss)
{
   solarSys = ss;
   if ((f) && (f->GetTypeName() == "GmatFunction"))   f->SetSolarSystem(ss);
}

void FunctionManager::SetTransientForces(std::vector<PhysicalModel*> &tf)
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
   if (f == NULL)
   {
      std::string errMsg = "FunctionManager:: Unable to execute Function """;
      errMsg += fName + """ - pointer is NULL\n";
      throw FunctionException(errMsg);
   }
   if (firstExecution)
   {
      // do a bunch of stuff here to set up FOS, etc.
      if (!(f->Initialize()))
      {
         std::string errMsg = "FunctionManager:: Error initializing function \"";
         errMsg += f->GetStringParameter("FunctionName") + "\"\n";
         throw FunctionException(errMsg);
      }
      firstExecution = false;
   }
   // do some stuff here to re-setup stuff
   return f->Execute();
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

