//$Id$
//------------------------------------------------------------------------------
//                                  GmatFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
#include "Assignment.hpp"        // for Assignment::GetMathTree()
#include "FileManager.hpp"       // for GetGmatFunctionPath()
#include "FileUtil.hpp"          // for ParseFileName(), GetWorkingDirectory()
#include "StringUtil.hpp"        // for Trim()
#include "CommandUtil.hpp"       // for ClearCommandSeq()
#include "HardwareException.hpp" 
#include "MessageInterface.hpp"

//#define DEBUG_FUNCTION
//#define DEBUG_FUNCTION_SET
//#define DEBUG_FUNCTION_INIT
//#define DEBUG_FUNCTION_EXEC
//#define DEBUG_FUNCTION_FINALIZE
//#define DEBUG_UNUSED_GOL

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
   mIsNewFunction = false;
   unusedGlobalObjectList = NULL;
   
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
      // Use exception to remove Visual C++ warning
      e.GetMessageType();
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
         // Use exception to remove Visual C++ warning
         e.GetMessageType();
         #ifdef DEBUG_FUNCTION
         MessageInterface::ShowMessage(e.GetFullMessage());
         #endif
      }
   }
   
   objectTypeNames.push_back("GmatFunction");

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
      ("GmatFunction() destructor entered, this=<%p> '%s', fcs=<%p>\n", this,
       GetName().c_str(), fcs);
   #endif
   
   // delete function sequence including NoOp (loj: 2008.12.22)
   if (fcs != NULL)
      GmatCommandUtil::ClearCommandSeq(fcs, false);
   
   if (unusedGlobalObjectList != NULL)
      delete unusedGlobalObjectList;
   
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
   mIsNewFunction = false;
   unusedGlobalObjectList = NULL;
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
   mIsNewFunction = false;
   unusedGlobalObjectList = NULL;
   
   return *this;
}


//------------------------------------------------------------------------------
// bool IsNewFunction()
//------------------------------------------------------------------------------
/**
 * Return true if function was created but not saved to file.
 * FunctionSetupPanel uses this flag to open new editor or load existing function.
 */
//------------------------------------------------------------------------------
bool GmatFunction::IsNewFunction()
{
   return mIsNewFunction;
}


//------------------------------------------------------------------------------
// void SetNewFunction(bool flag)
//------------------------------------------------------------------------------
void GmatFunction::SetNewFunction(bool flag)
{
   mIsNewFunction = flag;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool GmatFunction::Initialize()
{
   #ifdef DEBUG_TRACE
   static Integer callCount = 0;
   callCount++;      
   clock_t t1 = clock();
   ShowTrace(callCount, t1, "GmatFunction::Initialize() entered");
   #endif
   
   #ifdef DEBUG_FUNCTION_INIT
      MessageInterface::ShowMessage
         ("======================================================================\n"
          "GmatFunction::Initialize() entered for function '%s'\n", functionName.c_str());
      MessageInterface::ShowMessage("   and FCS is %s set.\n", (fcs? "correctly" : "NOT"));
      MessageInterface::ShowMessage("   Pointer for FCS is %p\n", fcs);
      MessageInterface::ShowMessage("   First command in fcs is %s\n",
            (fcs->GetTypeName()).c_str());
      MessageInterface::ShowMessage("   internalCS is %p\n", internalCoordSys);
   #endif
   if (!fcs) return false;
   
   Function::Initialize();
   
   // Initialize the Validator - I think I need to do this each time - or do I?
   validator->SetFunction(this);
   validator->SetSolarSystem(solarSys);
   std::map<std::string, GmatBase *>::iterator omi;
   
   // add automatic objects such as sat.X to the FOS (well, actually, clones of them)
   for (omi = automaticObjectMap.begin(); omi != automaticObjectMap.end(); ++omi)
   {
      std::string autoObjName = omi->first;
      
      // if name not found, clone it and add to map (loj: 2008.12.15)
      if (objectStore->find(autoObjName) == objectStore->end())
      {
         GmatBase *autoObj = (omi->second)->Clone();
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (autoObj, autoObjName, "GmatFunction::Initialize()",
             "autoObj = (omi->second)->Clone()");
         #endif
         
         #ifdef DEBUG_FUNCTION_INIT
         try
         {
            MessageInterface::ShowMessage
               ("   autoObj->EvaluateReal() = %f\n", autoObj->GetRealParameter("Value"));
         }
         catch (BaseException &e)
         {
            MessageInterface::ShowMessage(e.GetFullMessage());             
         }
         #endif
         
         autoObj->SetIsLocal(true);
         objectStore->insert(std::make_pair(autoObjName, autoObj));
      }
   }
   
   // first, send all the commands the object store, solar system, etc
   GmatCommand *current = fcs;
   
   while (current)
   {
      #ifdef DEBUG_FUNCTION_INIT
         if (!current)  MessageInterface::ShowMessage("Current is NULL!!!\n");
         else MessageInterface::ShowMessage("   =====> Current command is %s <%s>\n",
                 (current->GetTypeName()).c_str(),
                 current->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      #endif
      current->SetObjectMap(objectStore);
      current->SetGlobalObjectMap(globalObjectStore);
      current->SetSolarSystem(solarSys);
      current->SetInternalCoordSystem(internalCoordSys);
      current->SetTransientForces(forces);
      #ifdef DEBUG_FUNCTION_INIT
         MessageInterface::ShowMessage
            ("   Now about to set object map of type %s to Validator\n",
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
      
      #ifdef DEBUG_FUNCTION_INIT
      MessageInterface::ShowMessage
         ("   Now about to call Validator->ValidateCommand() of type %s\n",
          current->GetTypeName().c_str());
      #endif
      
      // Let's try to ValidateCommand here, this will validate the command
      // and create wrappers also
      if (!validator->ValidateCommand(current, false, 2))
      {
         // get error message (loj: 2008.06.04)
         StringArray errList = validator->GetErrorList();
         std::string msg; // Check for empty errList (loj: 2009.03.17)
         if (errList.empty())
            msg = "Error occurred";
         else
            msg = errList[0];
         
         throw FunctionException(msg + " in the function \"" + functionPath + "\"");
      }
      
      #ifdef DEBUG_FUNCTION_INIT
      MessageInterface::ShowMessage
         ("   Now about to initialize command of type %s\n", current->GetTypeName().c_str());
      #endif
      
      // catch exception and add function name to message (loj: 2008.09.23)
      try
      {
         if (!(current->Initialize()))
         {
            #ifdef DEBUG_FUNCTION_INIT
            MessageInterface::ShowMessage
               ("Exiting  GmatFunction::Initialize for function '%s' with false\n",
                functionName.c_str());
            #endif
            return false;
         }
      }
      catch (BaseException &e)
      {
         throw FunctionException("Cannot continue due to " + e.GetFullMessage() +
                                 " in the function \"" + functionPath + "\"");
      }
      
      // Check to see if the command needs a server startup (loj: 2008.07.25)
      if (current->NeedsServerStartup())
         if (validator->StartMatlabServer(current) == false)
            throw FunctionException("Unable to start the server needed by the " +
                                   (current->GetTypeName()) + " command");
      
      current = current->GetNext();
   }
   
   // Get automatic global object list and check if they are used in the function
   // command sequence so that when any global object is declared in the main script
   // but not used in the function, they can be ignored during function local object
   // initialization. (LOJ: 2009.12.18)
   BuildUnusedGlobalObjectList();
   
   fcsFinalized = false;
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("GmatFunction::Initialize() exiting for function '%s' with true\n",
       functionName.c_str());
   #endif
   
   #ifdef DEBUG_TRACE
   ShowTrace(callCount, t1, "GmatFunction::Initialize() exiting", true);
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool GmatFunction::Execute(ObjectInitializer *objInit, bool reinitialize)
//------------------------------------------------------------------------------
bool GmatFunction::Execute(ObjectInitializer *objInit, bool reinitialize)
{
   if (!fcs) return false;
   if (!objInit) return false;
   
   #ifdef DEBUG_TRACE
   static Integer callCount = 0;
   callCount++;      
   clock_t t1 = clock();
   ShowTrace(callCount, t1, "GmatFunction::Execute() entered");
   #endif
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("======================================================================\n"
       "GmatFunction::Execute() entered for '%s'\n   internalCS is <%p>, "
       "reinitialize = %d\n", functionName.c_str(), internalCoordSys, reinitialize);
   #endif
   
   GmatCommand *current = fcs;
   GmatCommand *last = NULL;
   
   // We want to initialize local objects with new object map,
   // so do it everytime (loj: 2008.09.26)
   // This causes to slow down function execution, so initialize if necessary
   if (reinitialize)
      objectsInitialized = false;
   
   // Reinitialize CoordinateSystem to fix bug 1599 (LOJ: 2009.11.05)
   // Reinitialize Parameters to fix bug 1519 (LOJ: 2009.09.16)
   if (objectsInitialized)
   {
      if (!objInit->InitializeObjects(true, Gmat::COORDINATE_SYSTEM))
         throw FunctionException
            ("Failed to re-initialize Parameters in the \"" + functionName + "\"");
      if (!objInit->InitializeObjects(true, Gmat::PARAMETER))
         throw FunctionException
            ("Failed to re-initialize Parameters in the \"" + functionName + "\"");
   }
   
   // Go through each command in the sequence and execute.
   // Once it gets to a real command, initialize local and automatic objects.
   while (current)
   {
      // Call to IsNextAFunction is necessary for branch commands in particular
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("......Function executing <%p><%s> [%s]\n", current, current->GetTypeName().c_str(),
          current->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      MessageInterface::ShowMessage("      objectsInitialized=%d\n", objectsInitialized);
      #endif
      
      last = current;
      
      if (!objectsInitialized)
      {
         // Since we don't know where actual mission sequence starts, just check
         // for command that is not NoOp, Create, Global, and GMAT with equation.
         // Can we have simple command indicating beginning of the sequence,
         // such as BeginSequence? (loj: 2008.06.19)
         // @todo: Now we have BeginMissionSequence, but not all functions have it,
         // so check it first otherwise do in the old way. (loj: 2010.07.16)
         Function *func = current->GetCurrentFunction();
         bool isEquation = false;
         std::string cmdType = current->GetTypeName();
         if (func && cmdType == "GMAT")
            if (((Assignment*)current)->GetMathTree() != NULL)
               isEquation = true;
         
         if (cmdType != "NoOp" && cmdType != "Create" && cmdType != "Global")
         {
            bool beginInit = true;            
            if (cmdType == "GMAT" && !isEquation)
               beginInit = false;

            if (cmdType == "BeginMissionSequence" || cmdType == "BeginScript")
               beginInit = true;
            
            if (beginInit)
            {
               objectsInitialized = true;
               validator->HandleCcsdsEphemerisFile(objectStore, true);
               #ifdef DEBUG_FUNCTION_EXEC
               MessageInterface::ShowMessage
                  ("============================ Initializing LocalObjects at current\n"
                   "%s\n", current->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
               #endif
               InitializeLocalObjects(objInit, current, true);
            }
         }
      }
      
      // Now execute the function sequence
      try
      {
         #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
            ("Now calling <%p>[%s]->Execute()\n", current->GetTypeName().c_str(),
             current->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
         #endif
         
         if (!(current->Execute()))
            return false;
      }
      catch (BaseException &e)
      {
         // If it is user interrupt, rethrow (loj: 2008.10.16)
         // How can we tell if it is thrown by Stop command?
         // For now just find the phrase "interrupted by Stop command"
         std::string msg = e.GetFullMessage();
         if (msg.find("interrupted by Stop command") != msg.npos)
         {
            #ifdef DEBUG_FUNCTION_EXEC
            MessageInterface::ShowMessage
               ("*** Interrupted by Stop commaned, so re-throwing...\n");
            #endif
            throw;
         }
         
         if (e.IsFatal())
         {
            #ifdef DEBUG_FUNCTION_EXEC
            MessageInterface::ShowMessage
               ("*** The exception is fatal, so re-throwing...\n");
            #endif
            // Add command line to error message (LOJ: 2010.04.13)
            throw FunctionException
               ("In " + current->GetGeneratingString(Gmat::NO_COMMENTS) + ", " +
                e.GetFullMessage());
            //throw;
         }
         
         // Let's try initialzing local objects here again (2008.10.14)
         try
         {
            #ifdef DEBUG_FUNCTION_EXEC
            MessageInterface::ShowMessage
               ("============================ Reinitializing LocalObjects at current\n"
                "%s\n", current->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
            #endif
            
            InitializeLocalObjects(objInit, current, false);
            
            #ifdef DEBUG_FUNCTION_EXEC
            MessageInterface::ShowMessage
               ("......Function re-executing <%p><%s> [%s]\n", current,
                current->GetTypeName().c_str(),
                current->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
            #endif
            
            if (!(current->Execute()))
               return false;
         }
         catch (HardwareException &)
         {
            // Ignore for hardware exception since spacecraft is associated with Thruster
            // but Thruster binds with Tank later in the fcs
         }
         catch (BaseException &)
         {
            throw FunctionException
               ("During initialization of local objects before \"" +
                current->GetGeneratingString(Gmat::NO_COMMENTS) + "\", " +
                e.GetFullMessage());
         }
      }
      
      // If current command is BranchCommand and still executing, continue to next
      // command in the branch (LOJ: 2009.03.24)
      if (current->IsOfType("BranchCommand") && current->IsExecuting())
      {
         #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
            ("In Function '%s', still executing current command is <%p><%s>\n",
             functionName.c_str(), current, current ? current->GetTypeName().c_str() : "NULL");
         #endif
         
         continue;
      }
      
      current = current->GetNext();
      
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("In Function '%s', the next command is <%p><%s>\n", functionName.c_str(),
          current, current ? current->GetTypeName().c_str() : "NULL");
      #endif
   }
   
   // Set ObjectMap from the last command to Validator in order to create
   // valid output wrappers (loj: 2008.11.12)
   validator->SetObjectMap(last->GetObjectMap());
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("   Now about to create %d output wrapper(s) to set results, objectsInitialized=%d\n",
       outputNames.size(), objectsInitialized);
   #endif
   
   // create output wrappers and put into map
   GmatBase *obj;
   wrappersToDelete.clear();
   for (unsigned int jj = 0; jj < outputNames.size(); jj++)
   {
      if (!(obj = FindObject(outputNames.at(jj))))
      {
         std::string errMsg = "Function: Output \"" + outputNames.at(jj);
         errMsg += " not found for function \"" + functionName + "\"";
         throw FunctionException(errMsg);
      }
      std::string outName = outputNames.at(jj);
      ElementWrapper *outWrapper =
         validator->CreateElementWrapper(outName, false, false);
      #ifdef DEBUG_MORE_MEMORY
      MessageInterface::ShowMessage
         ("+++ GmatFunction::Execute() *outWrapper = validator->"
          "CreateElementWrapper(%s), <%p> '%s'\n", outName.c_str(), outWrapper,
          outWrapper->GetDescription().c_str());
      #endif
      
      outWrapper->SetRefObject(obj);
      
      // nested CallFunction crashes if old outWrappers are deleted here. (loj: 2008.11.24)
      // so collect here and delete when FunctionRunner completes.
      wrappersToDelete.push_back(outWrapper);         
      
      // Set new outWrapper
      outputArgMap[outName] = outWrapper;
      #ifdef DEBUG_FUNCTION_EXEC // --------------------------------------------------- debug ---
         MessageInterface::ShowMessage("GmatFunction: Output wrapper created for %s\n",
                                       (outputNames.at(jj)).c_str());
      #endif // -------------------------------------------------------------- end debug ---
   }
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("GmatFunction::Execute() exiting true for '%s'\n", functionName.c_str());
   #endif
   
   #ifdef DEBUG_TRACE
   ShowTrace(callCount, t1, "GmatFunction::Execute() exiting", true);
   #endif
   
   return true; 
}


//------------------------------------------------------------------------------
// void Finalize()
//------------------------------------------------------------------------------
void GmatFunction::Finalize()
{
   #ifdef DEBUG_TRACE
   static Integer callCount = 0;
   callCount++;      
   clock_t t1 = clock();
   ShowTrace(callCount, t1, "GmatFunction::Finalize() entered");
   #endif
   
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage
      ("======================================================================\n"
       "GmatFunction::Finalize() entered for '%s', FCS %s\n",
       functionName.c_str(), fcsFinalized ? "already finalized, so skp fcs" :
       "NOT finalized, so call fcs->RunComplete");
   #endif
   
   // Call RunComplete on each command in fcs
   if (!fcsFinalized)
   {
      fcsFinalized = true;
      GmatCommand *current = fcs;
      while (current)
      {
         #ifdef DEBUG_FUNCTION_FINALIZE
            if (!current)
               MessageInterface::ShowMessage
                  ("   GmatFunction:Finalize() Current is NULL!!!\n");
            else
               MessageInterface::ShowMessage
                  ("   GmatFunction:Finalize() Now about to finalize "
                   "(call RunComplete on) command %s\n",
                   (current->GetTypeName()).c_str());
         #endif
         current->RunComplete();
         current = current->GetNext();
      }
   }
   
   Function::Finalize();
   
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage("GmatFunction::Finalize() leaving\n");
   #endif
   
   #ifdef DEBUG_TRACE
   ShowTrace(callCount, t1, "GmatFunction::Finalize() exiting", true, true);
   #endif
   
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
            std::string currPath = fm->GetWorkingDirectory();
            
            #ifdef DEBUG_FUNCTION_SET
            MessageInterface::ShowMessage("   currPath=%s\n", currPath.c_str());
            #endif
            
            if (temp[1] != '.')
               functionPath = currPath + temp.substr(1);
            else
            {
               functionPath = currPath + '/';
               functionPath = functionPath + temp;
            }
         }
         else
         {
            functionPath = value;
         }
         
         // Add to GmatFunction path
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


//------------------------------------------------------------------------------
// void ShowTrace(Integer count, Integer t1, const std::string &label = "",
//                bool showMemoryTracks = false, bool addEol = false)
//------------------------------------------------------------------------------
void GmatFunction::ShowTrace(Integer count, Integer t1, const std::string &label,
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
          functionName.c_str(), count, (Real)(t2-t1)/CLOCKS_PER_SEC);
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
             label.c_str(), functionName.c_str());
      
      if (addEol)
         MessageInterface::ShowMessage("\n");
      #endif
   }
}


//------------------------------------------------------------------------------
// bool InitializeLocalObjects(ObjectInitializer *objInit,
//                             GmatCommand *current, bool ignoreException)
//------------------------------------------------------------------------------
bool GmatFunction::InitializeLocalObjects(ObjectInitializer *objInit,
                                          GmatCommand *current,
                                          bool ignoreException)
{
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("\n============================ Begin initialization of local objects in '%s'\n",
       functionName.c_str());
   MessageInterface::ShowMessage
      ("Now at command \"%s\"\n",
       current->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
   
   // Why internal coordinate system is empty in ObjectInitializer?
   // Set internal coordinate system (added (loj: 2008.10.07)
   objInit->SetInternalCoordinateSystem(internalCoordSys);
   
   // Let's try initializing local objects using ObjectInitializer (2008.06.19)
   // We need to add subscribers to publisher, so pass true
   try
   {
      if (!objInit->InitializeObjects(true, Gmat::UNKNOWN_OBJECT,
                                      unusedGlobalObjectList))
         // Should we throw an exception instead?
         return false;
   }
   catch (BaseException &e)
   {
      // We need to ignore exception thrown for the case Object is
      // created after it is used, such as
      // GMAT DefaultOpenGL.ViewPointReference = EarthSunL1;
      // Create LibrationPoint EarthSunL1;
      if (!ignoreException || (ignoreException && e.IsFatal()))
      {
         #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
            ("objInit->InitializeObjects() threw a fatal exception:\n'%s'\n"
             "   So rethrow...\n", e.GetFullMessage().c_str());
         #endif
         throw;
      }
      else
      {
         #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
            ("objInit->InitializeObjects() threw an exception:\n'%s'\n"
             "   So ignoring...\n", e.GetFullMessage().c_str());
         #endif
      }
   }
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("============================ End   initialization of local objects\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void BuildUnusedGlobalObjectList()
//------------------------------------------------------------------------------
/*
 * Builds unused global object list which is used in the ObjectInitializer
 * for ignoring undefined ref objects. For now it adds automatic global
 * CoordinateSystem if it's ref objects is Spacecraft and it is not used in
 * the function sequence. Since Spacecraft is not an automatic object it is
 * not automatically added to GOS.
 */
//------------------------------------------------------------------------------
void GmatFunction::BuildUnusedGlobalObjectList()
{
   #ifdef DEBUG_UNUSED_GOL
   MessageInterface::ShowMessage
      ("BuildUnusedGlobalObjectList() entered. There are %d global objects\n",
       globalObjectStore->size());
   #endif
   
   if (unusedGlobalObjectList != NULL)
      delete unusedGlobalObjectList;
   
   unusedGlobalObjectList = new StringArray;
   
   // Check global object store
   std::string cmdName;
   GmatCommand *cmdUsing = NULL;
   std::map<std::string, GmatBase *>::iterator omi;
   for (omi = globalObjectStore->begin(); omi != globalObjectStore->end(); ++omi)
   {
      GmatBase *obj = omi->second;
      if (!GmatCommandUtil::FindObject(fcs, (omi->second)->GetType(), omi->first,
                                       cmdName, &cmdUsing))
      {
         // Add unused global CoordinateSystem with Spacecraft origin,  primary,
         // or secondary, since Spacecraft is not an automatic global object and
         // we don't want to throw an exception for unexisting Spacecraft in the GOS.
         if (obj->IsOfType(Gmat::COORDINATE_SYSTEM))
         {
            GmatBase *origin = obj->GetRefObject(Gmat::SPACE_POINT, "_GFOrigin_");
            GmatBase *primary = obj->GetRefObject(Gmat::SPACE_POINT, "_GFPrimary_");
            GmatBase *secondary = obj->GetRefObject(Gmat::SPACE_POINT, "_GFSecondary_");
            
            if ((origin != NULL && origin->IsOfType(Gmat::SPACECRAFT)) ||
                (primary != NULL && primary->IsOfType(Gmat::SPACECRAFT)) ||
                (secondary != NULL && secondary->IsOfType(Gmat::SPACECRAFT)))
            {
               #ifdef DEBUG_UNUSED_GOL
               MessageInterface::ShowMessage
                  ("==> Adding '%s' to unusedGOL\n", (omi->first).c_str());
               #endif
               unusedGlobalObjectList->push_back(omi->first);
            }
         }
      }
   }
   #ifdef DEBUG_UNUSED_GOL
   MessageInterface::ShowMessage
      ("BuildUnusedGlobalObjectList() leaving, There are %d unused global objects\n",
       unusedGlobalObjectList->size());
   #endif
}
