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
   in                  (fm.in),
   out                 (fm.out),
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
      in                  = fm.in;
      out                 = fm.out;
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
   fName = itsName;
}

std::string FunctionManager::GetFunctionName() const
{
   return fName;
}

void FunctionManager::SetFunction(Function *theFunction)
{
   f = theFunction;
}

Function* FunctionManager::GetFunction() const
{
   return f;
}

void FunctionManager::SetInputs(const StringArray &inputs)
{
   in = inputs;
}

void FunctionManager::SetOutputs(const StringArray &outputs)
{
   out = outputs;
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
   return f->Execute();
   return true;  // TBD
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

