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
#include "RealTypes.hpp"
#include "RealUtilities.hpp"

//#define DEBUG_FUNCTION_MANAGER
//#define DEBUG_FM_EXECUTE

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
   inputWrappers.clear();
   outputWrappers.clear();
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
   inputWrappers       (fm.inputWrappers),
   outputWrappers      (fm.outputWrappers), // is that right?
   firstExecution      (true)
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
      inputWrappers       = fm.inputWrappers; // is that right?
      outputWrappers      = fm.outputWrappers; // is that right?
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
   #ifdef DEBUG_FM_EXECUTE
      MessageInterface::ShowMessage("Entering FM::Execute for %s\n", fName.c_str());
   #endif
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
      #ifdef DEBUG_FM_EXECUTE
         MessageInterface::ShowMessage("in FM::Execute - firstExecution\n");
         if (ins.size() == 0) MessageInterface::ShowMessage("NOTE - ins is empty - is it supposed to be?\n");
         if (outs.size() == 0) MessageInterface::ShowMessage("NOTE - outs is empty - is it supposed to be?\n");
      #endif
      functionObjectStore.clear();
      inputWrappers.clear();
      outputWrappers.clear();
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
         functionObjectStore.insert(std::make_pair(objName,itsClone));
         #ifdef DEBUG_FM_EXECUTE // ------------------------------------------------- debug ---
            MessageInterface::ShowMessage("Adding object %s to the FOS\n", objName.c_str());
         #endif // -------------------------------------------------------------- end debug ---
         ElementWrapper *wrapper = validator.CreateElementWrapper(ins.at(ii));
         wrapper->SetRefObject(itsClone);
         inputWrappers.push_back(wrapper);
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
         ElementWrapper *outWrapper = validator.CreateElementWrapper(outs.at(jj));
         outWrapper->SetRefObject(obj);  // no clone here?  Assuming no number literals in output
         outputWrappers.push_back(outWrapper);
         #ifdef DEBUG_FM_EXECUTE // ------------------------------------------------- debug ---
            MessageInterface::ShowMessage("Output wrapper set for %s\n", (outs.at(jj)).c_str());
         #endif // -------------------------------------------------------------- end debug ---
      }
      firstExecution = false;
   }
   else // not the firstExecution
   {
      for (unsigned int ii=0; ii<ins.size(); ii++)
      {
         if (!(obj = FindObject(ins.at(ii))))
         {
            std::string errMsg = "Input \"" + ins.at(ii);
            errMsg += " not found for function \"" + fName + "\"";
            throw FunctionException(errMsg);
         }
         objName = f->GetStringParameter("Input", ii);
         if (functionObjectStore.find(objName) == functionObjectStore.end())
         {
            std::string errMsg = "FunctionManager error: input object \"" + objName;
            errMsg += "\"not found in Function Object Store.\n";
            throw FunctionException(errMsg);
         }
         GmatBase *fosObj = functionObjectStore[objName];
         fosObj->Copy(obj);
         f->SetInputElementWrapper(objName, inputWrappers.at(ii));
      }
   }
   // pass the FOS into the function
   f->SetObjectMap(&functionObjectStore);
   f->SetGlobalObjectMap(globalObjectStore);
   f->SetSolarSystem(solarSys);
   f->SetTransientForces(forces);
   // must re-initialize the function each time, as it may be called in more than
   // one place
   if (!(f->Initialize()))
   {
      std::string errMsg = "FunctionManager:: Error initializing function \"";
      errMsg += f->GetStringParameter("FunctionName") + "\"\n";
      throw FunctionException(errMsg);
   }
   // Now, eceute the function
   f->Execute();
   // Now get the outout data
   Real rval = -99999.999;
   Integer ival = -99999;
   bool bval = false;
   std::string sval;
   GmatBase *outObj = NULL;
   ElementWrapper *ew;
   Gmat::ParameterType rightType, leftType;
   for (unsigned int jj = 0; jj < outputWrappers.size(); jj++)
   {
      rval = -99999.999;
      ival = -99999;
      bval = false;
      sval = "";
      Rmatrix rmat;
      outObj = NULL;
      ew = f->GetOutputArgument(jj);
      // extract the data from the ElementWrapper, depending on the type
      rightType = ew->GetDataType();
      switch (rightType) 
      {
      case Gmat::BOOLEAN_TYPE:
         bval = ew->EvaluateBoolean();
         break;
      case Gmat::INTEGER_TYPE:
         ival = ew->EvaluateInteger();
         break;
      case Gmat::REAL_TYPE:
         rval = ew->EvaluateReal();
         break;
      case Gmat::RMATRIX_TYPE:
         rmat = ew->EvaluateArray();
         break;
      case Gmat::STRING_TYPE:
      case Gmat::ENUMERATION_TYPE:
         sval = ew->EvaluateString();
         sval = GmatStringUtil::RemoveEnclosingString(sval, "'");
         break;
      case Gmat::ON_OFF_TYPE:
         sval = ew->EvaluateOnOff();
         break;
      case Gmat::OBJECT_TYPE:
         outObj = ew->EvaluateObject();
         break;
      default:
         throw FunctionException("FunctionManager: Unknown output data type");
      }
      
      leftType = (outputWrappers.at(jj))->GetDataType();
      Gmat::WrapperDataType rightWrapperType = ew->GetWrapperType();
      Gmat::WrapperDataType leftWrapperType  = (outputWrappers.at(jj))->GetWrapperType();
      switch (leftType)
      {
      case Gmat::BOOLEAN_TYPE:
         (outputWrappers.at(jj))->SetBoolean(bval);
         break;
      case Gmat::INTEGER_TYPE:
         if (rightType == Gmat::INTEGER_TYPE)
         {
            (outputWrappers.at(jj))->SetInteger(ival);
         }
         else if (rightType == Gmat::REAL_TYPE)
         {
            Real tmpReal = GmatMathUtil::NearestInt(rval);
            if (GmatMathUtil::Abs(rval - tmpReal) > GmatRealConst::REAL_TOL)
               throw FunctionException("FunctionManager: Cannot get Integer from Real number.\n");
            (outputWrappers.at(jj))->SetInteger((Integer) rval);
         }
         break;
      case Gmat::REAL_TYPE:
         if (rval != -99999.999)
            (outputWrappers.at(jj))->SetReal(rval);
         else
            throw FunctionException("FunctionManager: Cannot set Non-Real value on Real");
         break;
      case Gmat::RMATRIX_TYPE:
         (outputWrappers.at(jj))->SetArray(rmat);
         break;
      case Gmat::STRING_TYPE:
      case Gmat::ENUMERATION_TYPE:
         // Object to String is needed for Remove for Formation
         if (outObj != NULL)
         {
            (outputWrappers.at(jj))->SetString(outObj->GetName());
         }
         else if ((rightType == Gmat::STRING_TYPE ||
                   rightType == Gmat::ENUMERATION_TYPE ||
                   rightType == Gmat::ON_OFF_TYPE))
         {
            (outputWrappers.at(jj))->SetString(sval);
         }
         // We don't want to allow VARIABLE to STRING assignment
         else if (rightType == Gmat::REAL_TYPE &&
                  rightWrapperType != Gmat::VARIABLE)
         {
            (outputWrappers.at(jj))->SetString(ew->GetDescription());
         }
         else
         {
            FunctionException fe;
            if (outObj != NULL)
               fe.SetDetails("FunctionManager: Cannot set object of type \"%s\" to an undefined "
                             "object \"%s\"", outObj->GetTypeName().c_str(), (outs.at(jj)).c_str());
            else if (leftWrapperType == Gmat::STRING_OBJECT &&
                     rightWrapperType == Gmat::VARIABLE)
               fe.SetDetails("FunctionManager: Cannot set objet of type \"Variable\" to object of "
                             "type \"String\"");
            else
               fe.SetDetails("FunctionManager: Cannot set value to an undefined object\n");
            throw fe;
         }
         break;
      case Gmat::ON_OFF_TYPE:
         (outputWrappers.at(jj))->SetOnOff(sval);
         break;
      case Gmat::OBJECT_TYPE:
         if (outObj == NULL)
            throw FunctionException("FunctionManager: Expected output object is NULL\n");
         (outputWrappers.at(jj))->SetObject(outObj);
         break;
      case Gmat::STRINGARRAY_TYPE:  //// @todo - revisit this
         if (outObj != NULL)
            (outputWrappers.at(jj))->SetString(outObj->GetName());
         else
            throw FunctionException("FunctionManager: Cannot set StringArray from output object.\n");
         break;
      case Gmat::OBJECTARRAY_TYPE:  
         // Object to String is needed for Add for Subscribers/Formation
         if (outObj != NULL)
            (outputWrappers.at(jj))->SetObject(outObj);
         else
         {
            //bool errorCond = true;
            //// @todo - not sure what to do here ...
          }
         break;
      default:
         throw FunctionException("FunctionManager: Unknown output data type");
      }
   }
   //// @todo -delete the clones here ??? (or not) ... where?
   
   return true;
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

