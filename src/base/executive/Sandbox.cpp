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
#include "FiniteThrust.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SANDBOX_OBJ 1
//#define DEBUG_SANDBOX_INIT 2
//#define DEBUG_SANDBOX_RUN 1

//------------------------------------------------------------------------------
// Sandbox::Sandbox(void)
//------------------------------------------------------------------------------
Sandbox::Sandbox() :
   solarSys        (NULL),
   internalCoordSys(NULL),
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
   Clear();
}
    
// Setup methods
//------------------------------------------------------------------------------
// bool AddObject(GmatBase *obj)
//------------------------------------------------------------------------------
bool Sandbox::AddObject(GmatBase *obj)
{
#if DEBUG_SANDBOX_OBJ
   MessageInterface::ShowMessage
      ("Sandbox::AddObject() objTypeName=%s, objName=%s\n",
       obj->GetTypeName().c_str(), obj->GetName().c_str());
#endif
   
   if (state == INITIALIZED)
      state = IDLE;

   std::string name = obj->GetName();
   if (name == "")
      return false;           // No unnamed objects in the Sandbox tables

   // Check to see if the object is already in the map
   if (objectMap.find(name) == objectMap.end())
   {
      // If not, store the new object pointer
      /// @todo Replace copy c'tor call with Clone() -- Build 3 issue
      if ((obj->GetType() == Gmat::SPACECRAFT) ||
          (obj->GetType() == Gmat::FORMATION))
      {
         objectMap[name] = obj->Clone();
         
         if(obj->GetType() == Gmat::SPACECRAFT)
         {
            if (solarSys)
               ((Spacecraft*)(obj))->SetSolarSystem(solarSys);
            // Finalize the state data -- this call moves the display state data
            // into the internal state.
            ((Spacecraft*)(obj))->SaveDisplay();
         }
      }
      else
         objectMap[name] = obj;
   }
   else
   {
      MessageInterface::ShowMessage("%s is already in the map\n", name.c_str());
   }
   
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
// bool SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
bool Sandbox::SetInternalCoordSystem(CoordinateSystem *cs)
{
   if (state == INITIALIZED)
      state = IDLE;
   
   if (!cs)
      return false;
   
   //internalCoordSys = (CoordinateSystem*)(cs->Clone());
   internalCoordSys = cs;
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


//loj: 3/2/05 Added default ObjectType
//------------------------------------------------------------------------------
// GmatBase* GetInternalObject(std::string name,
//                             Gmat::ObjectType type = Gmat::UNKNOWN_OBJECT)
//------------------------------------------------------------------------------
GmatBase* Sandbox::GetInternalObject(std::string name,
                                     Gmat::ObjectType type)
{
   GmatBase* obj = NULL;
    
   if (objectMap.find(name) != objectMap.end()) {
      obj = objectMap[name];
      if (type != Gmat::UNKNOWN_OBJECT)
      {
         if (obj->GetType() != type) {
            std::string errorStr = "GetInternalObject type mismatch for ";
            errorStr += name;
            throw SandboxException(errorStr);
         }
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
      
   transientForces.empty();

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
      for (omi = objectMap.begin(); omi != objectMap.end(); omi++)
      {
         #if DEBUG_SANDBOX_INIT
            MessageInterface::ShowMessage
               ("Sandbox::Initialize() objTypeName=%s, objName=%s\n",
                (omi->second)->GetTypeName().c_str(),
                (omi->second)->GetName().c_str());
         #endif
         if ((omi->second)->GetType() == Gmat::COORDINATE_SYSTEM)
         {
            CoordinateSystem *cs = (CoordinateSystem*)(omi->second);
            cs->SetSolarSystem(solarSys);
            cs->SetOrigin(solarSys->GetBody(cs->GetStringParameter("OriginName")));
            cs->SetJ2000Body(solarSys->GetBody(cs->GetStringParameter("J2000BodyName")));
            cs->Initialize();
         }
         else if ((omi->second)->GetType() == Gmat::PROP_SETUP)
         {
            ((PropSetup*)(omi->second))->GetForceModel()
               ->SetSolarSystem(solarSys);
         }
         else if((omi->second)->GetType() == Gmat::SPACECRAFT)
         {
            ((Spacecraft*)(omi->second))->SetSolarSystem(solarSys);
            // Finalize the state data -- this call moves the display state data
            // into the internal state.
            ((Spacecraft*)(omi->second))->SaveDisplay();
            
            // Build the spacecraft hardware and couple it together
            BuildAssociations(omi->second);
         }
         else if((omi->second)->GetType() == Gmat::PARAMETER)
         {
            Parameter *param = (Parameter*)(omi->second);

            // Set reference object for system parameters
            if (param->GetKey() == GmatParam::SYSTEM_PARAM)
            {
               // set Spacecraft
               std::string scName = param->GetRefObjectName(Gmat::SPACECRAFT);         
               #if DEBUG_SANDBOX_INIT > 1
               MessageInterface::ShowMessage
                  ("Sandbox::Initialize() Set SC <%s> pointer on parameter: \"%s\"\n",
                   scName.c_str(), param->GetName().c_str());
               #endif
               param->SetRefObject(GetSpacecraft(scName), Gmat::SPACECRAFT, scName);

                              
               // set Internal and Output CoordinateSystem
               if (param->NeedCoordSystem())
               {                  
                  param->SetInternalCoordSystem(internalCoordSys);
                  std::string csName;
                  
//                  try
//                  {
                     csName = param->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
//                  }
//                  catch(BaseException &e)
//                  {
//                     //----------------------------------------------------
//                     //loj: 1/26/05 temp. fix until ScriptInterpreter can
//                     //     handle CoordinateSystem
//                     //----------------------------------------------------
//                     #if DEBUG_SANDBOX_INIT
//                     MessageInterface::ShowMessage
//                        ("Sandbox::Initialize() ===>temp. fix for B3 script. setting "
//                         "EarthMJ2000Eq to %s \n", param->GetName().c_str());
//                     #endif
//                     csName = "EarthMJ2000Eq";
//                     param->SetRefObjectName(Gmat::COORDINATE_SYSTEM, csName);
//                  }

                  #if DEBUG_SANDBOX_INIT > 1
                  MessageInterface::ShowMessage
                     ("Sandbox::Initialize() Set CS <%s> pointer on parameter: \"%s\"\n",
                      csName.c_str(), param->GetName().c_str());
                  #endif
                  param->SetRefObject(GetInternalObject(csName, Gmat::COORDINATE_SYSTEM),
                                      Gmat::COORDINATE_SYSTEM, csName);
               }
               
               param->SetSolarSystem(solarSys);
               param->Initialize();
            }
         }
      }
   }
   else
      throw SandboxException("No solar system defined in the Sandbox!");

   
   #if DEBUG_SANDBOX_INIT
      MessageInterface::ShowMessage("Sandbox::Initialize() Initializing Variables...\n");
   #endif
   // Note: All system parameters need to be initialized first
   // Set reference object for user variables. Arrays don't have reference objects
   for (omi = objectMap.begin(); omi != objectMap.end(); omi++)
   {
      if((omi->second)->GetType() == Gmat::PARAMETER)
      {
         Parameter *param = (Parameter*)(omi->second);
         GmatBase *refParam;
         
         if (param->GetTypeName() == "Variable")
         {
            #if DEBUG_SANDBOX_INIT > 1
            MessageInterface::ShowMessage
               ("Sandbox::Initialize() userParamName=%s\n", param->GetName().c_str());
            #endif
            //StringArray refParamNames = param->GetStringArrayParameter("RefParams");
            StringArray refParamNames = param->GetRefObjectNameArray(Gmat::PARAMETER);
            for (unsigned int i=0; i<refParamNames.size(); i++)
            {
               refParam = GetInternalObject(refParamNames[i], Gmat::PARAMETER);
               param->SetRefObject(refParam, Gmat::PARAMETER, refParamNames[i]);
            }
            
            param->Initialize();
         }
      }
   }

   #if DEBUG_SANDBOX_INIT
      MessageInterface::ShowMessage("Sandbox::Initialize() Initializing Subscribers...\n");
   #endif

   // Initialize subscribers
   for (omi = objectMap.begin(); omi != objectMap.end(); omi++)
   {
      if((omi->second)->GetType() == Gmat::SUBSCRIBER)
      {
         Subscriber *sub = (Subscriber*)(omi->second);
         GmatBase *refParam;
         
         #if DEBUG_SANDBOX_INIT > 1
            MessageInterface::ShowMessage
               ("Sandbox::Initialize() subType=%s, subName=%s\n",
                sub->GetTypeName().c_str(), sub->GetName().c_str());
         #endif
         
         StringArray refParamNames = sub->GetRefObjectNameArray(Gmat::PARAMETER);
         for (unsigned int i=0; i<refParamNames.size(); i++)
         {
            refParam = GetInternalObject(refParamNames[i], Gmat::PARAMETER);
            sub->SetRefObject(refParam, Gmat::PARAMETER, refParamNames[i]);
         }
         
         if (sub->GetTypeName() == "OpenGLPlot")
         {
            // set SolarSystem and InternalCoordSystem first
            sub->SetRefObject(solarSys, Gmat::SOLAR_SYSTEM, "");
            sub->SetInternalCoordSystem(internalCoordSys);
            
            std::string csName = sub->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
            #if DEBUG_SANDBOX_INIT > 1
               MessageInterface::ShowMessage
                  ("Sandbox::Initialize() csName=%s\n", csName.c_str());
            #endif
            sub->SetRefObject(GetInternalObject(csName, Gmat::COORDINATE_SYSTEM),
                              Gmat::COORDINATE_SYSTEM, csName);
         }
         
         sub->Initialize();
      }
   }
   
   #if DEBUG_SANDBOX_INIT
      MessageInterface::ShowMessage("Sandbox::Initialize() Initializing Commands...\n");
   #endif
   // Initialize commands
   while (current)
   {
      #if DEBUG_SANDBOX_INIT
         MessageInterface::ShowMessage(
            "Initializing %s command\n", current->GetTypeName().c_str());
      #endif
      current->SetObjectMap(&objectMap);
      current->SetSolarSystem(solarSys);
      rv = current->Initialize();
      if (!rv)
         return false;

      current->SetTransientForces(&transientForces);
      current = current->GetNext();
   }
   
   #if DEBUG_SANDBOX_INIT
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

      #if DEBUG_SANDBOX_RUN
         MessageInterface::ShowMessage("Sandbox::Execution running %s\n",
                                       current->GetTypeName().c_str());
      #endif

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
         std::string str = "\"" + current->GetTypeName() +
            "\" Command failed to run to completion\nCommand Text is \"" +
            current->GetGeneratingString() + "\"";
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
    
   // Delete the clones (currently only Spacecraft and Formations get cloned)
   std::map<std::string, GmatBase *>::iterator omi;
   for (omi = objectMap.begin(); omi != objectMap.end(); omi++)
   {
//      if ((omi->second)->GetType() == Gmat::SUBSCRIBER)
//         publisher->Unsubscribe((Subscriber*)(omi->second));
      if (   ((omi->second)->GetType() == Gmat::SPACECRAFT)
          || ((omi->second)->GetType() == Gmat::FORMATION)
//          || ((omi->second)->GetType() == Gmat::SUBSCRIBER)
         )
         delete omi->second;
   }
   
   objectMap.clear();
   
   transientForces.clear();
}


//------------------------------------------------------------------------------
// bool AddSubscriber(Subscriber *sub)
//------------------------------------------------------------------------------
bool Sandbox::AddSubscriber(Subscriber *subsc)
{
   Subscriber *sub = (Subscriber *)(subsc->Clone());
#if DEBUG_SANDBOX_OBJ
   MessageInterface::ShowMessage
      ("Sandbox::AddSubscriber() name = %s\n",
       sub->GetName().c_str());
#endif
   publisher->Subscribe(sub);
   return  AddObject(sub);
}


//------------------------------------------------------------------------------
// void BuildAssociations()
//------------------------------------------------------------------------------
void Sandbox::BuildAssociations(GmatBase * obj)
{
   // Spacecraft receive clones of the associated hardware objects
   if (obj->GetType() == Gmat::SPACECRAFT) {
      StringArray hw = obj->GetRefObjectNameArray(Gmat::HARDWARE);
      for (StringArray::iterator i = hw.begin(); i < hw.end(); ++i) {
       
         #if DEBUG_SANDBOX
            MessageInterface::ShowMessage
               ("Sandbox::BuildAssociations() setting \"%s\" on \"%s\"\n",
                i->c_str(), obj->GetName().c_str());
         #endif
       
         if (objectMap.find(*i) == objectMap.end())
            throw SandboxException("Sandbox::BuildAssociations: Cannot find "
                                   "hardware element \"" + (*i) + "\"\n");
         GmatBase *el = objectMap[*i];
         GmatBase *newEl = el->Clone();
         #if DEBUG_SANDBOX
            MessageInterface::ShowMessage
               ("Sandbox::BuildAssociations() created clone \"%s\" of type \"%s\"\n", 
               newEl->GetName().c_str(), newEl->GetTypeName().c_str());
         #endif
         if (!obj->SetRefObject(newEl, newEl->GetType(), newEl->GetName()))
            MessageInterface::ShowMessage
               ("Sandbox::BuildAssociations() failed to set %s\n", 
               newEl->GetName().c_str());
         ;
      }
      
      obj->TakeAction("SetupHardware");
   }
}
