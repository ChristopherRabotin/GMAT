//$Id$
//------------------------------------------------------------------------------
//                                  GmatFunction
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2004/12/16
//
/**
 * Implementation for the GmatFunction class.
 */
//------------------------------------------------------------------------------

#include "GmatFunction.hpp"
#include "FileManager.hpp"       // for GetGmatFunctionPath()
#include "FileUtil.hpp"          // for ParseFileName()
#include "StringUtil.hpp"        // for Trim()
#include "MessageInterface.hpp"


//#define DEBUG_FUNCTION
//#define DEBUG_FUNCTION_SET
//#define DEBUG_FUNCTION_INIT
//#define DEBUG_FUNCTION_EXEC
//#define DEBUG_FUNCTION_FINALIZE

//---------------------------------
// static data
//---------------------------------
//const std::string
//GmatFunction::PARAMETER_TEXT[GmatFunctionParamCount - FunctionParamCount] =
//{
//};
//
//const Gmat::ParameterType
//GmatFunction::PARAMETER_TYPE[GmatFunctionParamCount - FunctionParamCount] =
//{
//};




//------------------------------------------------------------------------------
// GmatFunction(std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> function name
 */
//------------------------------------------------------------------------------
GmatFunction::GmatFunction(const std::string &name) :
   Function("GmatFunction", name)
{
   // for initial function path, use FileManager
   FileManager *fm = FileManager::Instance();
   std::string pathname;
   
   try
   {
      // if there is a function name, try to locate it
      if (name != "")
      {
         // Get path of first it is located
         pathname = fm->GetGmatFunctionPath(name + ".gmf");
         
         // gmat function uses whole path name
         pathname = pathname + name + ".gmf";         
         functionPath = pathname;
         functionName = GmatFileUtil::ParseFileName(functionPath);
         
         // Remove path and .gmf
         functionName = GmatFileUtil::ParseFileName(functionPath);
         std::string::size_type dotIndex = functionName.find(".gmf");
         functionName = functionName.substr(0, dotIndex);
      }
      else
      {
         // gmat function uses whole path name
         functionPath = fm->GetFullPathname("GMAT_FUNCTION_PATH");    
      }
   }
   catch (GmatBaseException &e)
   {
      #ifdef DEBUG_FUNCTION
      MessageInterface::ShowMessage(e.GetFullMessage());
      #endif
      
      // see if there is FUNCTION_PATH
      try
      {
         pathname = fm->GetFullPathname("FUNCTION_PATH");
         functionPath = pathname;
      }
      catch (GmatBaseException &e)
      {
         #ifdef DEBUG_FUNCTION
         MessageInterface::ShowMessage(e.GetFullMessage());
         #endif
      }
   }
   
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("   Gmat functionPath=<%s>\n", functionPath.c_str());
   MessageInterface::ShowMessage
      ("   Gmat functionName=<%s>\n", functionName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// ~GmatFunction()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
GmatFunction::~GmatFunction()
{
   #ifdef DEBUG_GMATFUNCTION
   MessageInterface::ShowMessage
      ("GmatFunction() destructor entered, this=<%p>'%s'\n", this, GetName().c_str());
   #endif
   StringArray toDelete;
   std::map<std::string, GmatBase *>::iterator omi;
   for (omi = automaticObjects.begin(); omi != automaticObjects.end(); ++omi)
   {
      if (omi->second != NULL)
      {
         #ifdef DEBUG_GMATFUNCTION
         GmatBase *obj = omi->second;
         MessageInterface::ShowMessage
            ("   Deleting <%p><%s>'%s'\n", obj, obj->GetTypeName().c_str(),
             obj->GetName().c_str());
         #endif
         delete omi->second;
         omi->second = NULL;
      }
      toDelete.push_back(omi->first); 
   }
   for (unsigned int kk = 0; kk < toDelete.size(); kk++)
   {
      automaticObjects.erase(toDelete.at(kk));
   }
   #ifdef DEBUG_GMATFUNCTION
   MessageInterface::ShowMessage("GmatFunction() destructor exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// GmatFunction(const GmatFunction &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> object being copied
 */
//------------------------------------------------------------------------------
GmatFunction::GmatFunction(const GmatFunction &copy) :
   Function(copy)
{
}


//------------------------------------------------------------------------------
// GmatFunction& GmatFunction::operator=(const GmatFunction& right)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 *
 * @param <right> object being copied
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
GmatFunction& GmatFunction::operator=(const GmatFunction& right)
{
   if (this == &right)
      return *this;
   
   Function::operator=(right);
   
   return *this;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool GmatFunction::Initialize()
{
   #ifdef DEBUG_FUNCTION_INIT
      MessageInterface::ShowMessage("Entering GmatFunction::Initialize for function %s\n",
            functionName.c_str());
      MessageInterface::ShowMessage("   and FCS is %s set.\n", (fcs? "correctly" : "NOT"));
      MessageInterface::ShowMessage("   Pointer for FCS is %p\n", fcs);
      MessageInterface::ShowMessage("   First command in fcs is %s\n",
            (fcs->GetTypeName()).c_str());
   #endif
   if (!fcs) return false;
   
   Function::Initialize();
   
   // Initialize the Validator - I think I need to do this each time - or do I?
   validator->SetFunction(this);
   validator->SetSolarSystem(solarSys);
   std::map<std::string, GmatBase *>::iterator omi;
      
   // add automatic objects to the FOS (well, actually, clones of them)
   for (omi = automaticObjects.begin(); omi != automaticObjects.end(); ++omi)
   {
      std::string objName = omi->first;
      GmatBase    *objPtr = (omi->second)->Clone();
      if (objectStore->find(omi->first) == objectStore->end())
         objectStore->insert(std::make_pair(objName, objPtr));
   }
   // first, send all the commands the input wrappers
   
   GmatCommand *current = fcs;
   
   while (current)
   {
      #ifdef DEBUG_FUNCTION_INIT
         if (!current)  MessageInterface::ShowMessage("Current is NULL!!!\n");
         else MessageInterface::ShowMessage("   Now about to initialize command of type %s\n   %s\n",
                 (current->GetTypeName()).c_str(), current->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      #endif
      current->SetObjectMap(objectStore);
      current->SetGlobalObjectMap(globalObjectStore);
      current->SetSolarSystem(solarSys);
      current->SetInternalCoordSystem(internalCoordSys);
      current->SetTransientForces(forces);
      #ifdef DEBUG_FUNCTION_INIT
         MessageInterface::ShowMessage("   Now about to send required wrappers to command of type %s\n",
               (current->GetTypeName()).c_str());         
      #endif
      // (Re)set object map on Validator (necessary because objects may have been added to the 
      // Local Object Store or Global Object Store during initialization of previous commands)
      validatorStore.clear();
      for (omi = objectStore->begin(); omi != objectStore->end(); ++omi)
         validatorStore.insert(std::make_pair(omi->first, omi->second));
      for (omi = globalObjectStore->begin(); omi != globalObjectStore->end(); ++omi)
         validatorStore.insert(std::make_pair(omi->first, omi->second));
      validator->SetObjectMap(&validatorStore);
      
      // Let's try to ValidateCommand here, this will validate the comand
      // and create wrappers also
      if (!validator->ValidateCommand(current, false, 2))
      {
         // get error message (loj: 2008.06.04)
         StringArray errList = validator->GetErrorList();
         throw FunctionException(errList[0] + " in the function \"" + functionPath + "\"");
      }
      
      if (!(current->Initialize()))
         return false;
      
      // Check to see if the command needs a server startup (loj: 2008.07.25)
      if (current->NeedsServerStartup())
         if (validator->StartServer(current) == false)
            throw FunctionException("Unable to start the server needed by the " +
                                   (current->GetTypeName()) + " command");
      
      current = current->GetNext();
   }
   fcsFinalized = false;
   return true;
}


//------------------------------------------------------------------------------
// bool GmatFunction::Execute(ObjectInitializer *objInit)
//------------------------------------------------------------------------------
bool GmatFunction::Execute(ObjectInitializer *objInit)
{
   if (!fcs) return false;
   if (!objInit) return false;
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("GmatFunction::Execute() entered for '%s';  objectsInitialized=%d\n",
       functionName.c_str(), objectsInitialized);
   #endif
   
   GmatCommand *current = fcs;
   
   while (current)
   {
      // Call to IsNextAFunction is necessary for branch commands in particular
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("......Function executing <%p><%s><%s>\n", current, current->GetTypeName().c_str(),
          current->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      #endif
      
      // Since we don't know where actual mission squence start, just check for command
      // that is not NoOp, Create, and GMAT.
      // Can we have simple command indicating beginning of the sequence,
      // such as BeginMission? (loj: 2008.06.19)
      if (!objectsInitialized)
      {
         if (current->GetTypeName() != "NoOp" && current->GetTypeName() != "Create" &&
             current->GetTypeName() != "GMAT")
         {
            objectsInitialized = true;
            
            #ifdef DEBUG_FUNCTION_EXEC
            MessageInterface::ShowMessage("Now at command \"%s\"\n\n", 
                  (current->GetGeneratingString()).c_str());
            MessageInterface::ShowMessage
               ("\n============================ Begin initialization of local objects\n");
            #endif
            // Let's try initialzing local objects using ObjectInitializer (2008.06.19)
            // We need to add subscribers to publisher, so pass true
            if (!objInit->InitializeObjects(true))
            {
               // Should we throw an exception instead?
               return false;
            }
            #ifdef DEBUG_FUNCTION_EXEC
            MessageInterface::ShowMessage
               ("============================ End  initialization of local objects\n");
            #endif
         }
      }
      
      if (!(current->Execute()))
         return false;
      current = current->GetNext();
   }
   // create output wrappers and put into map
   GmatBase *obj;
   for (unsigned int jj = 0; jj < outputNames.size(); jj++)
   {
      if (!(obj = FindObject(outputNames.at(jj))))
      {
         std::string errMsg = "Function: Output \"" + outputNames.at(jj);
         errMsg += " not found for function \"" + functionName + "\"";
         throw FunctionException(errMsg);
      }
      ElementWrapper *outWrapper = validator->CreateElementWrapper(outputNames.at(jj), false, false);
      outWrapper->SetRefObject(obj); 
      outputArgMap[outputNames.at(jj)] = outWrapper;
      #ifdef DEBUG_FUNCTION_EXEC // --------------------------------------------------- debug ---
         MessageInterface::ShowMessage("GmatFunction: Output wrapper created for %s\n", (outputNames.at(jj)).c_str());
      #endif // -------------------------------------------------------------- end debug ---
   }
   return true; 
}


//------------------------------------------------------------------------------
// void Finalize()
//------------------------------------------------------------------------------
void GmatFunction::Finalize()
{
   ; // @todo - finalize anything else that needs it as well
   if (!fcsFinalized)
   {
      GmatCommand *current = fcs;
      while (current)
      {
         #ifdef DEBUG_FUNCTION_FINALIZE
            if (!current)  MessageInterface::ShowMessage("Current is NULL!!!\n");
            else MessageInterface::ShowMessage("   Now about to finalize (call RunComplete on) command %s\n",
                  (current->GetTypeName()).c_str());         
         #endif
         current->RunComplete();
         current = current->GetNext();
      }
   }
   fcsFinalized = true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the GmatFunction.
 *
 * @return clone of the GmatFunction.
 *
 */
//------------------------------------------------------------------------------
GmatBase* GmatFunction::Clone() const
{
   return (new GmatFunction(*this));
}


//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void GmatFunction::Copy(const GmatBase* orig)
{
   operator=(*((GmatFunction *)(orig)));
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * 
 * @return If value of the parameter was set.
 */
//------------------------------------------------------------------------------
bool GmatFunction::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_FUNCTION_SET
   MessageInterface::ShowMessage
      ("GmatFunction::SetStringParameter() entered, id=%d, value=%s\n", id, value.c_str());
   #endif
   
   switch (id)
   {
   case FUNCTION_PATH:
      {
         FileManager *fm = FileManager::Instance();
         
         // Compose full path if it has relative path.
         // Assuming if first char has '.', it has relative path.
         std::string temp = GmatStringUtil::Trim(value);
         if (temp[0] == '.')
         {
            std::string currPath = fm->GetCurrentPath();
            
            #ifdef DEBUG_FUNCTION_SET
            MessageInterface::ShowMessage("   currPath=%s\n", currPath.c_str());
            #endif
            
            functionPath = currPath + temp.substr(1);
         }
         else
         {
            functionPath = value;
         }
         
         // add to GmatFunction path (loj: 2008.04.18)
         fm->AddGmatFunctionPath(functionPath);
         
         // Remove path
         functionName = GmatFileUtil::ParseFileName(functionPath);
         
         // Remove .gmf if GmatGmatFunction
         std::string::size_type dotIndex = functionName.find(".gmf");
         functionName = functionName.substr(0, dotIndex);
         
         #ifdef DEBUG_FUNCTION_SET
         MessageInterface::ShowMessage
            ("   functionPath=<%s>\n", functionPath.c_str());
         MessageInterface::ShowMessage
            ("   functionName=<%s>\n", functionName.c_str());
         #endif
         
         return true;
      }
   case FUNCTION_NAME:
      {
         // Remove path if it has one
         functionName = GmatFileUtil::ParseFileName(functionPath);
         
         // Remove .gmf if GmatGmatFunction
         std::string::size_type dotIndex = functionName.find(".gmf");
         functionName = functionName.substr(0, dotIndex);
         
         return true;
      }
   default:
      return Function::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool GmatFunction::SetStringParameter(const std::string &label,
                                      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

