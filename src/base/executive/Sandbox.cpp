//$Header$
//------------------------------------------------------------------------------
//                                 Sandbox
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/10/08
//
/**
 * Implementation for the GMAT Sandbox class
 */
//------------------------------------------------------------------------------


#include "Sandbox.hpp"
#include "Moderator.hpp"
#include "SandboxException.hpp"
#include "Parameter.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SANDBOX 1

//------------------------------------------------------------------------------
// Sandbox::Sandbox(void)
//------------------------------------------------------------------------------
Sandbox::Sandbox() :
   solarSys        (NULL),
   publisher       (NULL),
   sequence        (NULL),
   current         (NULL),
   moderator       (NULL),
   state           (IDLE)
{
}


//------------------------------------------------------------------------------
// ~Sandbox(void)
//------------------------------------------------------------------------------
Sandbox::~Sandbox()
{
   if (solarSys)
      delete solarSys;
   if (sequence)
      delete sequence;
    
   // Delete the local objects
}
    
// Setup methods
//------------------------------------------------------------------------------
// bool AddObject(GmatBase *obj)
//------------------------------------------------------------------------------
bool Sandbox::AddObject(GmatBase *obj)
{
#if DEBUG_SANDBOX
   MessageInterface::ShowMessage
      ("Sandbox::AddObject() objType=%s, objName=%s\n",
       obj->GetTypeName().c_str(), obj->GetName().c_str());
#endif
   
   if (state == INITIALIZED)
      state = IDLE;

   std::string name = obj->GetName();
   if (name == "")
      return false;           // No unnamed objects in the Sandbox tables

   // Check to see if the object is already in the map
   if (objectMap.find(name) == objectMap.end())
      // If not, store the new object pointer
      /// @todo Replace copy c'tor call with Clone() -- Build 3 issue
      if ((obj->GetType() == Gmat::SPACECRAFT) || 
          (obj->GetType() == Gmat::FORMATION))
         objectMap[name] = obj->Clone(); // new Spacecraft(*((Spacecraft*)obj));
      else
         objectMap[name] = obj;
    
   return true;
}


//------------------------------------------------------------------------------
// bool AddCommand(GmatCommand *cmd)
//------------------------------------------------------------------------------
bool Sandbox::AddCommand(GmatCommand *cmd)
{
   if (state == INITIALIZED)
      state = IDLE;

   if (!cmd)
      return false;
        
   if (cmd == sequence)
      return true;
   // throw SandboxException("Adding command that is already in the Sandbox");
    
   if (sequence)
      return sequence->Append(cmd);

   sequence = cmd;
   return true;
}


//------------------------------------------------------------------------------
// bool AddSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
bool Sandbox::AddSolarSystem(SolarSystem *ss)
{
   if (state == INITIALIZED)
      state = IDLE;
   if (!ss)
      return false;
//   solarSys = (SolarSystem*)(ss->Clone());
   solarSys = ss;
   return true;
}


//------------------------------------------------------------------------------
// bool SetPublisher(Publisher *pub)
//------------------------------------------------------------------------------
bool Sandbox::SetPublisher(Publisher *pub)
{
   if (state == INITIALIZED)
      state = IDLE;

   if (pub) {
      publisher = pub;
      return true;
   }

   // Initialize off of the singleton
   //publisher = Publisher::Instance();
   if (!publisher)
      return false;
   return true;
}


//------------------------------------------------------------------------------
// GmatBase* GetInternalObject(std::string name, Gmat::ObjectType type)
//------------------------------------------------------------------------------
GmatBase* Sandbox::GetInternalObject(std::string name, Gmat::ObjectType type)
{
   GmatBase* obj = NULL;
    
   if (objectMap.find(name) != objectMap.end()) {
      obj = objectMap[name];
      if (obj->GetType() != type) {
         std::string errorStr = "GetInternalObject type mismatch for ";
         errorStr += name;
         throw SandboxException(errorStr);
      }
   }
   else {
      std::string errorStr = "Could not find ";
      errorStr += name;
      errorStr += " in the Sandbox.";
      throw SandboxException(errorStr);
   }
    
   return obj;
}


//------------------------------------------------------------------------------
// Spacecraft* GetSpacecraft(std::string name)
//------------------------------------------------------------------------------
Spacecraft* Sandbox::GetSpacecraft(std::string name)
{
   Spacecraft *sc = NULL;
   GmatBase* obj = GetInternalObject(name, Gmat::SPACECRAFT);
   
   if (obj)
      sc = (Spacecraft*)(obj);
    
   return sc;
}


// Execution methods
//------------------------------------------------------------------------------
// bool Initialize(void)
//------------------------------------------------------------------------------
bool Sandbox::Initialize()
{
   bool rv = false;
   
   if (moderator == NULL)
      moderator = Moderator::Instance();

   // Already initialized
   if (state == INITIALIZED)
      return true;

   current = sequence;
   if (!current)
      return false;
        
   std::map<std::string, GmatBase *>::iterator omi;
   // Set the solar system on each force model, spacecraft, parameter
   if (solarSys)
   {
      //std::map<std::string, GmatBase *>::iterator omi;
      for (omi = objectMap.begin(); omi != objectMap.end(); omi++)
      {
#if DEBUG_SANDBOX
         MessageInterface::ShowMessage
            ("Sandbox::Initialize() objType=%s, objName=%s\n",
             (omi->second)->GetTypeName().c_str(),
             (omi->second)->GetName().c_str());
#endif
         if ((omi->second)->GetType() == Gmat::PROP_SETUP)
         {
            ((PropSetup*)(omi->second))->GetForceModel()
               ->SetSolarSystem(solarSys);
            //((PropSetup*)(omi->second))->Initialize();
         }
         else if((omi->second)->GetType() == Gmat::SPACECRAFT)
         {
            ((Spacecraft*)(omi->second))->SetSolarSystem(solarSys);
         }
         else if((omi->second)->GetType() == Gmat::PARAMETER)
         {
            //loj: 9/13/04 moved code from Moderator::SetupRun()
            Parameter *param = (Parameter*)(omi->second);

            // Set reference object for system parameters (loj: 9/22/04)
            if (param->GetKey() == Parameter::SYSTEM_PARAM)
            {
               std::string scName = param->GetRefObjectName(Gmat::SPACECRAFT);            
               param->SetRefObject(GetSpacecraft(scName), Gmat::SPACECRAFT, scName);
               param->SetSolarSystem(solarSys);
               param->Initialize();
            }
         }
      }
   }
   else
      throw SandboxException("No solar system defined in the Sandbox!");


#if DEBUG_SANDBOX
   MessageInterface::ShowMessage("Sandbox::Initialize() Initializing Variables...\n");
#endif
   // Note: All system parameters need to be initialized first
   // Set reference object for user parameters (loj: 9/22/04)
   for (omi = objectMap.begin(); omi != objectMap.end(); omi++)
   {
      if((omi->second)->GetType() == Gmat::PARAMETER)
      {
         Parameter *param = (Parameter*)(omi->second);
         GmatBase *refParam;
                  
         if (param->GetKey() == Parameter::USER_PARAM)
         {
#if DEBUG_SANDBOX
            MessageInterface::ShowMessage
               ("Sandbox::Initialize() userParamName=%s\n", param->GetName().c_str());
#endif
            StringArray refParamNames = param->GetStringArrayParameter("RefParams");
            for (unsigned int i=0; i<refParamNames.size(); i++)
            {
               refParam = GetInternalObject(refParamNames[i], Gmat::PARAMETER);
               param->SetRefObject(refParam, Gmat::PARAMETER, refParamNames[i]);
               param->Initialize();
            }
         }
      }
   }

#if DEBUG_SANDBOX
   MessageInterface::ShowMessage("Sandbox::Initialize() Initializing Subscribers...\n");
#endif

   // Initialize subscribers (loj: 9/13/04 moved code from Moderator::SetupRun())
   //std::map<std::string, GmatBase *>::iterator omi;
   for (omi = objectMap.begin(); omi != objectMap.end(); omi++)
   {
      if((omi->second)->GetType() == Gmat::SUBSCRIBER)
      {
         ((Subscriber*)(omi->second))->Initialize();
      }
   }
   
#if DEBUG_SANDBOX
   MessageInterface::ShowMessage("Sandbox::Initialize() Initializing Commands...\n");
#endif
   // Initialize commands
   while (current)
   {
      current->SetObjectMap(&objectMap);
      current->SetSolarSystem(solarSys); //loj: 6/16/04 added
      rv = current->Initialize();
      if (!rv)
         return false;
      current = current->GetNext();
   }
   
#if DEBUG_SANDBOX
   MessageInterface::ShowMessage("Sandbox::Initialize() Successfully initialized\n");
#endif
   
   return rv;
}


//------------------------------------------------------------------------------
// bool Execute(void)
//------------------------------------------------------------------------------
bool Sandbox::Execute()
{
   
   bool rv = true;

   state = RUNNING;
   Gmat::RunState runState = Gmat::IDLE, currentState = Gmat::RUNNING;
    
   current = sequence;
   if (!current)
      return false;

   while (current) {
      // First check to see if the run should be interrupted
      if (Interrupt()) {
         MessageInterface::ShowMessage("Sandbox::Execution interrupted by Moderator\n");
         return rv;
      }
        
      // MessageInterface::ShowMessage("Sandbox::Execution running %s\n",
      //                               current->GetTypeName().c_str());

      if (current->GetTypeName() == "Target") {
         if (current->GetBooleanParameter(current->GetParameterID("TargeterConverged")))
            currentState = Gmat::RUNNING;
         else
            currentState = Gmat::TARGETING;
      }
      
      if (currentState != runState) {
         publisher->SetRunState(currentState);
         runState = currentState;
      }
        
      rv = current->Execute();

      // Possible fix for displaying the last iteration...
      if (current->GetTypeName() == "Target") {
         if (current->GetBooleanParameter(current->GetParameterID("TargeterConverged")))
            currentState = Gmat::RUNNING;
         else
            currentState = Gmat::TARGETING;
      }
      
      if (!rv) {
         std::string str = current->GetTypeName() +
            " Command failed to run to completion";
         throw SandboxException(str);
      }
      current = current->GetNext();
   }

   return rv;
}


//------------------------------------------------------------------------------
// bool Interrupt(void)
//------------------------------------------------------------------------------
bool Sandbox::Interrupt()
{
   // Ask the moderator for the current RunState:
   Gmat::RunState interruptType =  moderator->GetUserInterrupt();
    
   switch (interruptType) {
      case Gmat::PAUSED:   // Pause
         state = PAUSED;
         break;
    
      case Gmat::IDLE:     // Stop puts GMAT into the Idle state
         state = STOPPED;
         break;
    
      case Gmat::RUNNING:   // Pause
         state = RUNNING;
         break;
    
      default:
         break;
   }
   
   if ((state == PAUSED) || (state == STOPPED))
      return true;
      
   return false;
}


//------------------------------------------------------------------------------
// void Clear(void)
//------------------------------------------------------------------------------
void Sandbox::Clear()
{
//   if (solarSys)
//      delete solarSys;
   solarSys  = NULL;
   publisher = NULL;
   sequence  = NULL;
   current   = NULL;
   state     = IDLE;
    
   /// @todo The current Sandbox::Clear() method has a small memory leak
   ///       when spacecraft are removed from the onjectMap.  This needs to
   ///       be fixed.
   objectMap.clear();
}


//------------------------------------------------------------------------------
// bool AddSubscriber(Subscriber *sub)
//------------------------------------------------------------------------------
bool Sandbox::AddSubscriber(Subscriber *sub)
{
#if DEBUG_SANDBOX
   MessageInterface::ShowMessage
      ("Sandbox::AddSubscriber() name = %s\n",
       sub->GetName().c_str());
#endif
   publisher->Subscribe(sub); //loj: 3/9/04 added
   return  AddObject(sub);
}

