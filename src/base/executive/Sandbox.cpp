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

#include <algorithm>       // for find

//#define DEBUG_SANDBOX_OBJ 1
//#define DEBUG_SANDBOX_INIT 2
//#define DEBUG_SANDBOX_RUN 1
//#define DEBUG_SANDBOX_OBJECT_MAPS

//------------------------------------------------------------------------------
// Sandbox::Sandbox()
//------------------------------------------------------------------------------
/**
 *  Default constructor.
 */
//------------------------------------------------------------------------------
Sandbox::Sandbox() :
   solarSys          (NULL),
   internalCoordSys  (NULL),
   publisher         (NULL),
   sequence          (NULL),
   current           (NULL),
   moderator         (NULL),
   state             (IDLE)
{
   // List of the objects that can safely be cloned.  This list will be removed
   // when the cloning has been tested for all of GMAT's classes.
   clonable.push_back(Gmat::SPACECRAFT);
   clonable.push_back(Gmat::FORMATION);
//   clonable.push_back(Gmat::SPACEOBJECT);
//   clonable.push_back(Gmat::GROUND_STATION);
//   clonable.push_back(Gmat::BURN);
//   clonable.push_back(Gmat::COMMAND);
//   clonable.push_back(Gmat::PROPAGATOR);
//   clonable.push_back(Gmat::FORCE_MODEL);
//   clonable.push_back(Gmat::PHYSICAL_MODEL);
//   clonable.push_back(Gmat::TRANSIENT_FORCE);
//   clonable.push_back(Gmat::INTERPOLATOR);
//   clonable.push_back(Gmat::SPACE_POINT);
//   clonable.push_back(Gmat::CELESTIAL_BODY);
//   clonable.push_back(Gmat::CALCULATED_POINT);
//   clonable.push_back(Gmat::LIBRATION_POINT);
//   clonable.push_back(Gmat::BARYCENTER);
//   clonable.push_back(Gmat::ATMOSPHERE);
//   clonable.push_back(Gmat::PARAMETER);
//   clonable.push_back(Gmat::STOP_CONDITION);
//   clonable.push_back(Gmat::SOLVER);
//   clonable.push_back(Gmat::SUBSCRIBER);
//   clonable.push_back(Gmat::PROP_SETUP);
//   clonable.push_back(Gmat::REF_FRAME);
//   clonable.push_back(Gmat::FUNCTION);
//   clonable.push_back(Gmat::FUEL_TANK);
//   clonable.push_back(Gmat::THRUSTER);
//   clonable.push_back(Gmat::HARDWARE);
//   clonable.push_back(Gmat::COORDINATE_SYSTEM);
//   clonable.push_back(Gmat::AXIS_SYSTEM);

   // SolarSystem instances are handled separately from the other objects
   // clonable.push_back(Gmat::SOLAR_SYSTEM);
}


//------------------------------------------------------------------------------
// ~Sandbox()
//------------------------------------------------------------------------------
/**
 *  Destructor.
 */
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
    

//------------------------------------------------------------------------------
// Setup methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// bool AddObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 *  Adds an object to the Sandbox's object container.
 *
 *  Objects are added to the Sandbox by cloning the objects.  That way local
 *  copies can be manipulated without affecting the objects managed by the
 *  ConfigurationManager.
 *
 *  @note Cloning is not yet fully functional in GMAT.  Oncew cloning is
 *        completed, this method will be simplified to clone every object added
 *        to the Sandbox.
 *
 *  @param <obj> The object that needs to be included in the Sandbox.
 *
 *  @return true if the object was added to the Sandbox's container, false if
 *          it was not.
 */
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
      if (find(clonable.begin(), clonable.end(), obj->GetType()) !=
             clonable.end())
      {
         #ifdef DEBUG_SANDBOX_OBJECT_MAPS
            MessageInterface::ShowMessage(
               "Cloning object %s of type %s\n", obj->GetName().c_str(),
               obj->GetTypeName().c_str());
         #endif
         
         objectMap[name] = obj->Clone();
         
         if (obj->GetType() == Gmat::SPACECRAFT)
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
/**
 *  Adds a command to the Sandbox's command sequence.
 *
 *  Command are added to the command srquence by appending them ti the command
 *  list, using the GmatCommand::Append() method.
 *
 *  @param <cmd> The command that needs to be added to this Sandbox's sequence.
 *
 *  @return true if the command was added to the sequence, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::AddCommand(GmatCommand *cmd)
{
   if (state == INITIALIZED)
      state = IDLE;

   if (!cmd)
      return false;
        
   if (cmd == sequence)
      return true;
    
   if (sequence)
      return sequence->Append(cmd);

   sequence = cmd;
   return true;
}


//------------------------------------------------------------------------------
// bool AddSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 *  Sets the SolarSystem for this Sandbox by cloning the input solar system.
 *
 *  @param <ss> The SolarSystem this Sandbox's will use.
 *
 *  @return true if the solar system was added to the Sandbox, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::AddSolarSystem(SolarSystem *ss)
{
   if (state == INITIALIZED)
      state = IDLE;
   if (!ss)
      return false;

   solarSys = (SolarSystem*)(ss->Clone());
   return true;
}


//------------------------------------------------------------------------------
// bool SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 *  Sets the internal coordinate system used by the Sandbox.
 *
 *  @param <cs> The internal coordinate system.
 *
 *  @return true if the command was added to the sequence, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::SetInternalCoordSystem(CoordinateSystem *cs)
{
   if (state == INITIALIZED)
      state = IDLE;
   
   if (!cs)
      return false;
   
   /// @todo Check initialization and cloning for the internal CoordinateSystem.
   //internalCoordSys = (CoordinateSystem*)(cs->Clone());
   internalCoordSys = cs;
   return true;
}


//------------------------------------------------------------------------------
// bool SetPublisher(Publisher *pub)
//------------------------------------------------------------------------------
/**
 *  Sets the Publisher so the Sandbox can pipe data to the rest of GMAT.
 *
 *  @param <pub> The GMAT Publisher.
 *
 *  @return true if the command was added to the sequence, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::SetPublisher(Publisher *pub)
{
   if (state == INITIALIZED)
      state = IDLE;

   if (pub) {
      publisher = pub;
      return true;
   }

   if (!publisher)
      return false;

   return true;
}


//------------------------------------------------------------------------------
// GmatBase* GetInternalObject(std::string name, Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 *  Accesses objects managed by this Sandbox.
 *
 *  @param <name> The name of the object.
 *  @param <name> type of object requested.
 *
 *  @return The pointer to the object.
 */
//------------------------------------------------------------------------------
GmatBase* Sandbox::GetInternalObject(std::string name, Gmat::ObjectType type)
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
/**
 *  Accesses the local copy of a Spacecraft managed by this Sandbox.
 *
 *  @param <name> The name of the Spacecraft.
 *
 *  @return The pointer to the Spacecraft.
 */
//------------------------------------------------------------------------------
Spacecraft* Sandbox::GetSpacecraft(std::string name)
{
   Spacecraft *sc = NULL;
   GmatBase* obj = GetInternalObject(name, Gmat::SPACECRAFT);
   
   if (obj)
      sc = (Spacecraft*)(obj);
    
   return sc;
}


//------------------------------------------------------------------------------
// Execution methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 *  Established the internal linkages between objects needed prior to running a
 *  mission sequence.
 *
 *  @return true if everything was connected properly, false if not.
 */
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
      throw SandboxException("No mission sequence defined in the Sandbox!");

   if (!internalCoordSys)
      throw SandboxException(
         "No reference (internal) coordinate system defined in the Sandbox!");

   std::map<std::string, GmatBase *>::iterator omi;

   // Set the solar system on each force model, spacecraft, parameter
   if (solarSys)
   {
      std::string objName;
      SpacePoint *sp, *j2kBod;
      std::string j2kName;
      
      // Set J2000 bodies for solar system objects -- should this happen here?
      const StringArray biu = solarSys->GetBodiesInUse();
      for (StringArray::const_iterator i = biu.begin(); i != biu.end(); ++i)
      {
         sp = solarSys->GetBody(*i);
         j2kName = sp->GetStringParameter("J2000BodyName");
         j2kBod = FindSpacePoint(j2kName);
         sp->SetJ2000Body(j2kBod);
      }
         

      // set ref object for internal coordinate system
      internalCoordSys->SetSolarSystem(solarSys);

      // Set refence origin for internal coordinate system.
      objName = internalCoordSys->GetStringParameter("OriginName");
      sp = FindSpacePoint(objName);
      if (sp == NULL)
         throw SandboxException("Cannot find SpacePoint named \"" +
            objName + "\" used for the internal coordinate system origin");
      internalCoordSys->SetRefObject(sp, Gmat::SPACE_POINT, objName);

      // Set J2000 body for internal coordinate system
      objName = internalCoordSys->GetStringParameter("J2000BodyName");
      sp = FindSpacePoint(objName);
      if (sp == NULL)
         throw SandboxException("Cannot find SpacePoint named \"" +
            objName + "\" used for the internal coordinate system J2000 body");
      internalCoordSys->SetRefObject(sp, Gmat::SPACE_POINT, objName);
      
      internalCoordSys->Initialize();
      
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

            // Set the reference objects for the coordinate system
            objName = cs->GetStringParameter("OriginName");
            sp = FindSpacePoint(objName);
            if (sp == NULL)
               throw SandboxException("Cannot find SpacePoint named \"" +
                  objName + "\" used for the coordinate system " +
                  cs->GetName() + " origin");
            cs->SetRefObject(sp, Gmat::SPACE_POINT, objName);
            
            objName = cs->GetStringParameter("J2000BodyName");
            sp = FindSpacePoint(objName);
            if (sp == NULL)
               throw SandboxException("Cannot find SpacePoint named \"" +
                  objName + "\" used for the coordinate system " +
                  cs->GetName() + " J2000 body");
            cs->SetRefObject(sp, Gmat::SPACE_POINT, objName);

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
                     ("Sandbox::Initialize() Set SC <%s> pointer on parameter: "
                      "\"%s\"\n", scName.c_str(), param->GetName().c_str());
               #endif
               param->SetRefObject(GetSpacecraft(scName), Gmat::SPACECRAFT,
                                   scName);

               // set Internal and Output CoordinateSystem
               if (param->NeedCoordSystem())
               {                  
                  param->SetInternalCoordSystem(internalCoordSys);
                  std::string csName;
                  
                  csName = param->GetRefObjectName(Gmat::COORDINATE_SYSTEM);

                  #if DEBUG_SANDBOX_INIT > 1
                  MessageInterface::ShowMessage
                     ("Sandbox::Initialize() Set CS <%s> pointer on parameter: \"%s\"\n",
                      csName.c_str(), param->GetName().c_str());
                  #endif
                  param->SetRefObject(GetInternalObject(csName, Gmat::COORDINATE_SYSTEM),
                                      Gmat::COORDINATE_SYSTEM, csName);
               }
               
               // set reference body
               if (param->IsOriginDependent())
               {
                  std::string origName;

                  origName = param->GetRefObjectName(Gmat::SPACE_POINT);

                  #if DEBUG_SANDBOX_INIT > 1
                     MessageInterface::ShowMessage
                        ("Sandbox::Initialize() Set SpacePoint <%s> pointer on "
                        "parameter: \"%s\"\n", origName.c_str(),
                        param->GetName().c_str());
                  #endif
                  
                  SpacePoint *sp = FindSpacePoint(origName);
                  param->SetRefObject(sp, Gmat::SPACE_POINT, origName);
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
// bool Execute()
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
// bool Interrupt()
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
// void Clear()
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
   
   #ifdef DEBUG_SANDBOX_OBJECT_MAPS
      MessageInterface::ShowMessage("Sandbox OMI List\n");
      for (omi = objectMap.begin(); omi != objectMap.end(); omi++)
      {
         MessageInterface::ShowMessage("   %s", (omi->first).c_str());
         MessageInterface::ShowMessage(" of type %s\n",
            (omi->second)->GetTypeName().c_str());
      }
   #endif

   for (omi = objectMap.begin(); omi != objectMap.end(); omi++)
   {
//      if ((omi->second)->GetType() == Gmat::SUBSCRIBER)
//         publisher->Unsubscribe((Subscriber*)(omi->second));
      #ifdef DEBUG_SANDBOX_OBJECT_MAPS
         MessageInterface::ShowMessage("Sandbox clearing %s\n",
            (omi->first).c_str());
      #endif
      
      if (find(clonable.begin(), clonable.end(),
          (omi->second)->GetType()) != clonable.end())
      {
         delete omi->second;
      }
   }
   
   if (solarSys != NULL)
      delete solarSys;
   
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


//------------------------------------------------------------------------------
// SpacePoint* FindSpacePoint(const std::string &spname)
//------------------------------------------------------------------------------
SpacePoint * Sandbox::FindSpacePoint(const std::string &spName)
{
   SpacePoint *sp = solarSys->GetBody(spName);

   if (sp == NULL)
   {
      /// @todo If sp is not solar system object, look for a configured point
      ;
   }

   return sp;
}
