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
   firstExecution    (true),
   numVarsCreated    (0),
   realResult        (-999.99),
   blankResult       (false),
   outputType        ("")
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
   //inputWrappers       (fm.inputWrappers),
   outputWrappers      (fm.outputWrappers), // is that right?
   firstExecution      (true),
   numVarsCreated      (fm.numVarsCreated),
   realResult          (fm.realResult),
   matResult           (fm.matResult),
   blankResult         (fm.blankResult),
   outputType          (fm.outputType)
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
      numVarsCreated      = fm.numVarsCreated;
      //inputWrappers       = fm.inputWrappers; // is that right?
      outputWrappers      = fm.outputWrappers; // is that right?
      realResult          = fm.realResult;
      matResult           = fm.matResult;
      blankResult         = fm.blankResult;
      outputType          = fm.outputType;
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
   #ifdef DEBUG_FUNCTION_MANAGER
   MessageInterface::ShowMessage
      ("FunctionManager::SetFunction() fName='%s', theFunction=<%p>\n", fName.c_str(), theFunction);
   #endif
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
   GmatBase *obj, *objFOS;
   std::string objName;
   if (f == NULL)
   {
      std::string errMsg = "FunctionManager:: Unable to execute Function """;
      errMsg += fName + """ - pointer is NULL\n";
      throw FunctionException(errMsg);
   }
   // Initialize the Validator - I think I need to do this each time - or do I?
   validator.SetSolarSystem(solarSys);
   std::map<std::string, GmatBase *>::iterator omi;
   for (omi = localObjectStore->begin(); omi != localObjectStore->end(); ++omi)
      combinedObjectStore.insert(std::make_pair(omi->first, omi->second));
   for (omi = globalObjectStore->begin(); omi != globalObjectStore->end(); ++omi)
      combinedObjectStore.insert(std::make_pair(omi->first, omi->second));
   validator.SetObjectMap(&combinedObjectStore);
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
   if (firstExecution)
   {
      #ifdef DEBUG_FM_EXECUTE
         MessageInterface::ShowMessage("in FM::Execute - firstExecution\n");
      #endif
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
         #ifdef DEBUG_FM_EXECUTE
            MessageInterface::ShowMessage(
                  "in FM::Execute: object \"%s\" of type \"%s\" found in LOS/GOS \n",
                  (ins.at(ii)).c_str(), (obj->GetTypeName()).c_str());
         #endif
         // Clone the object, and insert it into the FOS
         objFOS = obj->Clone();
         objName = inNames.at(ii);
         objFOS->SetName(objName);
         functionObjectStore.insert(std::make_pair(objName,objFOS));
         #ifdef DEBUG_FM_EXECUTE // ------------------------------------------------- debug ---
            MessageInterface::ShowMessage("Adding object %s to the FOS\n", objName.c_str());
         #endif // -------------------------------------------------------------- end debug ---
         // create an element wrapper for the input
         ElementWrapper *inWrapper = validator.CreateElementWrapper(ins.at(ii), false, false);
         inWrapper->SetRefObject(objFOS);
         inputWrappers.insert(std::make_pair(objName, inWrapper));
         #ifdef DEBUG_FM_EXECUTE // ------------------------------------------------- debug ---
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
            ElementWrapper *outWrapper = validator.CreateElementWrapper(outs.at(jj));
            outWrapper->SetRefObject(obj); 
            outputWrappers.push_back(outWrapper);
            #ifdef DEBUG_FM_EXECUTE // ------------------------------------------------- debug ---
               MessageInterface::ShowMessage("Output wrapper set for %s\n", (outs.at(jj)).c_str());
            #endif // -------------------------------------------------------------- end debug ---
         }
      }
      firstExecution = false;
   }
   else // not the firstExecution
   {
      // Need to delete all items in the FOS that are not inputs (so that they can 
      // properly be created again in the FCS 
      StringArray toDelete;
      bool        isInput = false;
      for (omi = functionObjectStore.begin(); omi != functionObjectStore.end(); ++omi)
      {
         isInput = false;
         for (unsigned int qq = 0; qq < inNames.size(); qq++)
            if (omi->first == inNames.at(qq))  isInput = true;
         if (!isInput) toDelete.push_back(omi->first);                                     
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
   }
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
   // Now, execute the function
   f->Execute();
   // Now get the output data
   Real rval = -99999.999;
   Integer ival = -99999;
   bool bval = false;
   std::string sval;
   GmatBase *outObj = NULL;
   ElementWrapper *ew;
   Gmat::ParameterType rightType, leftType;
   outputType = "";
   if (blankResult)  SaveLastResult();
   else
   {
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
   
   #ifdef DEBUG_FM_EVAL
   MessageInterface::ShowMessage
      ("==> FunctionManager::Evaluate() f=<%p><%s>\n", f, f->GetName().c_str());
   #endif
   
   Execute();
   if (outputType != "Real")
      throw FunctionException("FunctionManager: invalid output type - should be real\n");
   return realResult;
   //return f->Evaluate();
}

Rmatrix FunctionManager::MatrixEvaluate()
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
   
   Execute();
   if (outputType != "Rmatrix")
      throw FunctionException("FunctionManager: invalid output type - should be Rmatrix\n");
   return matResult;
   //return f->MatrixEvaluate();
}

void FunctionManager::Finalize()
{
   firstExecution = true;
   
   ; // @todo - call function to call RunComplete on FCS here?
     // delete all contents of FOS here?
   if (f != NULL && f->IsOfType("GmatFunction")) //loj: added check for GmatFunction
      f->Finalize();
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
      //v = new Variable(newName);
      v = new Variable(str);
      v->SetReal(rval);
      obj = (GmatBase*) v;
      createdLiterals.insert(std::make_pair(str,obj));
   }
   // Now check to see if it is a string literal, enclosed with single quotes
   else if ((str[0] == '\'') && (str[sz-1] == '\''))
   {
      sval = str.substr(1, sz-2);
      //StringVar *sv = new StringVar(newName);
      StringVar *sv = new StringVar(str);
      sv->SetStringParameter("Value", sval);
      obj = (GmatBase*) sv;
      createdLiterals.insert(std::make_pair(str,obj));
   }
   else
   {
      // otherwise, we assume it is something else, like array element.
      // NOTE that we are only allowing Real and Array here 
      ElementWrapper *ew = validator.CreateElementWrapper(fromString);
      if (ew)
      {
         Gmat::WrapperDataType wType = ew->GetWrapperType();
         switch (wType)
         {
            case Gmat::ARRAY :
            {
               //Array *array   = new Array(newName);
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
               //v = new Variable(newName);
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
               //v = new Variable(newName);
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