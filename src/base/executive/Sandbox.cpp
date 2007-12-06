//$Id$
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
#include "GmatFunction.hpp"
#include "CallFunction.hpp"
#include "SubscriberException.hpp"
#include "CommandUtil.hpp"         // for GetCommandSeqString()
#include "MessageInterface.hpp"

#include <algorithm>       // for find

//#define DISABLE_SOLAR_SYSTEM_CLONING

//#define DEBUG_SANDBOX_OBJ 1
//#define DEBUG_SANDBOX_INIT 1
//#define DEBUG_SANDBOX_INIT_CS 1
//#define DEBUG_SANDBOX_INIT_PARAM 1
//#define DEBUG_SANDBOX_RUN 1
//#define DEBUG_SANDBOX_OBJECT_MAPS
//#define DEBUG_MODERATOR_CALLBACK
//#define DEBUG_FM_INITIALIZATION


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
   state             (IDLE),
   interruptCount    (45),
   pollFrequency     (50)
{
   #ifdef DEBUG_SANDBOX_CLONING
      // List of the objects that can safely be cloned.  This list will be removed
      // when the cloning has been tested for all of GMAT's classes.
      clonable.push_back(Gmat::SPACECRAFT);
      clonable.push_back(Gmat::FORMATION);
      clonable.push_back(Gmat::SPACEOBJECT);
      clonable.push_back(Gmat::GROUND_STATION);
      clonable.push_back(Gmat::BURN);
      clonable.push_back(Gmat::IMPULSIVE_BURN);
      clonable.push_back(Gmat::FINITE_BURN);
      clonable.push_back(Gmat::COMMAND);
      clonable.push_back(Gmat::PROPAGATOR);
      clonable.push_back(Gmat::FORCE_MODEL);
      clonable.push_back(Gmat::PHYSICAL_MODEL);
      clonable.push_back(Gmat::TRANSIENT_FORCE);
      clonable.push_back(Gmat::INTERPOLATOR);
      clonable.push_back(Gmat::SPACE_POINT);
      clonable.push_back(Gmat::CELESTIAL_BODY);
      clonable.push_back(Gmat::CALCULATED_POINT);
      clonable.push_back(Gmat::LIBRATION_POINT);
      clonable.push_back(Gmat::BARYCENTER);
      clonable.push_back(Gmat::ATMOSPHERE);
      clonable.push_back(Gmat::PARAMETER);
      clonable.push_back(Gmat::STOP_CONDITION);
      clonable.push_back(Gmat::SOLVER);
      clonable.push_back(Gmat::SUBSCRIBER);
      clonable.push_back(Gmat::PROP_SETUP);
      clonable.push_back(Gmat::FUNCTION);
      clonable.push_back(Gmat::FUEL_TANK);
      clonable.push_back(Gmat::THRUSTER);
      clonable.push_back(Gmat::HARDWARE);
      clonable.push_back(Gmat::COORDINATE_SYSTEM);
      clonable.push_back(Gmat::AXIS_SYSTEM);
   #endif

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
   #ifndef DISABLE_SOLAR_SYSTEM_CLONING   
      if (solarSys)
         delete solarSys;
   #endif
   
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

   if ((state != INITIALIZED) && (state != STOPPED) && (state != IDLE))
          MessageInterface::ShowMessage(
             "Unexpected state transition in the Sandbox\n");

   state = IDLE;

   std::string name = obj->GetName();
   if (name == "")
      return false;           // No unnamed objects in the Sandbox tables


   // Check to see if the object is already in the map
   if (objectMap.find(name) == objectMap.end())
   {
      // If not, store the new object pointer
      #ifdef DEBUG_SANDBOX_CLONING
      if (find(clonable.begin(), clonable.end(), obj->GetType()) !=
          clonable.end())
      {
      #endif
         #ifdef DEBUG_SANDBOX_OBJECT_MAPS
            MessageInterface::ShowMessage(
               "Cloning object %s of type %s\n", obj->GetName().c_str(),
               obj->GetTypeName().c_str());
         #endif

         // Subscribers are already cloned in AddSubscriber()
         if (obj->GetType() == Gmat::SUBSCRIBER)
            objectMap[name] = obj;
         else
            objectMap[name] = obj->Clone();
         
         if (obj->GetType() == Gmat::SPACECRAFT)
         {
            if (solarSys)
               ((Spacecraft*)(obj))->SetSolarSystem(solarSys);
         }
      #ifdef DEBUG_SANDBOX_CLONING
      }
      else
         objectMap[name] = obj;
      #endif
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

   if ((state != INITIALIZED) && (state != STOPPED) && (state != IDLE))
          MessageInterface::ShowMessage(
             "Unexpected state transition in the Sandbox\n");

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
   if ((state != INITIALIZED) && (state != STOPPED) && (state != IDLE))
          MessageInterface::ShowMessage(
             "Unexpected state transition in the Sandbox\n");
   state = IDLE;

   if (!ss)
      return false;

#ifdef DISABLE_SOLAR_SYSTEM_CLONING
   solarSys = ss;
#else
   MessageInterface::ShowMessage("Cloning the solar system in the Sandbox\n");
   solarSys = (SolarSystem*)(ss->Clone());
#endif
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
   if ((state != INITIALIZED) && (state != STOPPED) && (state != IDLE))
          MessageInterface::ShowMessage(
             "Unexpected state transition in the Sandbox\n");

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

   if ((state != INITIALIZED) && (state != STOPPED) && (state != IDLE))
          MessageInterface::ShowMessage(
             "Unexpected state transition in the Sandbox\n");
   state = IDLE;


   if (pub) {
      publisher = pub;
      // Now publisher needs internal coordinate system
      publisher->SetInternalCoordSystem(internalCoordSys);
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
      std::string errorStr = "Sandbox::GetInternalObject(" + name +
                             "...) Could not find ";
      errorStr += name;
      errorStr += " in the Sandbox.";

      #ifdef DEBUG_SANDBOX_OBJECT_MAPS
         MessageInterface::ShowMessage("Here is the current object map:\n");
         for (std::map<std::string, GmatBase *>::iterator i = objectMap.begin();
              i != objectMap.end(); ++i)
            MessageInterface::ShowMessage("   %s\n", i->first.c_str());
      #endif

      throw SandboxException(errorStr);
   }

   return obj;
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
   #ifdef DEBUG_SANDBOX_INIT
      MessageInterface::ShowMessage("Initializing the Sandbox\n");
   #endif

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
   GmatBase *obj = NULL;
   std::string oName;
   std::string j2kName;


   // Set the solar system references
   if (solarSys == NULL)
      throw SandboxException("No solar system defined in the Sandbox!");


   // Initialize the solar system, internal coord system, etc.
   InitializeInternalObjects();

   // Set J2000 Body for all SpacePoint derivatives before anything else
   for (omi = objectMap.begin(); omi != objectMap.end(); ++omi)
   {
      obj = omi->second;
      if (obj->IsOfType(Gmat::SPACE_POINT))
      {
         #ifdef DEBUG_SANDBOX_INIT
            MessageInterface::ShowMessage(
               "Setting J2000 Body for %s in the Sandbox\n",
               obj->GetName().c_str());
         #endif
         SpacePoint *spObj = (SpacePoint *)obj;
         SpacePoint* j2k = FindSpacePoint(spObj->GetJ2000BodyName());
         if (j2k)
            spObj->SetJ2000Body(j2k);
         else
            throw SandboxException("Sandbox did not find the Spacepoint \"" +
               spObj->GetJ2000BodyName() + "\"");
      }
   }

   // The initialization order is:
   //
   //  1. CoordinateSystem
   //  2. Spacecraft
   //  3. PropSetup and others
   //  4. System Parameters
   //  5. Other Parameters
   //  6. Subscribers.

   // Set reference objects

   // Coordinate Systems
   for (omi = objectMap.begin(); omi != objectMap.end(); ++omi)
   {
      obj = omi->second;
      if (obj->GetType() == Gmat::COORDINATE_SYSTEM)
      {
         #ifdef DEBUG_SC_INIT_CS
            MessageInterface::ShowMessage("Initializing CS %s\n",
               obj->GetName().c_str());
         #endif
         obj->SetSolarSystem(solarSys);
         BuildReferences(obj);
         InitializeCoordinateSystem((CoordinateSystem *)obj);
         obj->Initialize();
      }
   }


   // Spacecraft
   for (omi = objectMap.begin(); omi != objectMap.end(); ++omi)
   {
      obj = omi->second;
      if (obj->GetType() == Gmat::SPACECRAFT)
      {
         obj->SetSolarSystem(solarSys);
         ((Spacecraft *)obj)->SetInternalCoordSystem(internalCoordSys);

         BuildReferences(obj);

         // Setup spacecraft hardware
         BuildAssociations(obj);

         obj->Initialize();
      }
   }


   // All others except Parameters and Subscribers
   // Spacecraft
   for (omi = objectMap.begin(); omi != objectMap.end(); ++omi)
   {
      obj = omi->second;
      if ((obj->GetType() != Gmat::COORDINATE_SYSTEM) &&
          (obj->GetType() != Gmat::SPACECRAFT) &&
          (obj->GetType() != Gmat::PARAMETER) &&
          (obj->GetType() != Gmat::SUBSCRIBER))
      {
         #ifdef DEBUG_SANDBOX_INIT
            MessageInterface::ShowMessage(
               "Sandbox::Initialize objTypeName = %s, objName = %s\n",
               obj->GetTypeName().c_str(), obj->GetName().c_str());
         #endif


         //*************************** TEMPORARY *******************************
         if (obj->GetType() != Gmat::PROP_SETUP)
         {
            obj->SetSolarSystem(solarSys);
            if (obj->IsOfType(Gmat::SPACE_POINT))
               BuildReferences(obj);
            continue;
         }
         #ifdef DEBUG_FM_INITIALIZATION
            else
               MessageInterface::ShowMessage("Initializing PropSetup '%s'\n",
                  obj->GetName().c_str());
         #endif
         //********************** END OF TEMPORARY *****************************


         BuildReferences(obj);
         // PropSetup initialization is handled by the commands, since the
         // state that is propagated may change as spacecraft are added or
         // removed.
         if (obj->GetType() != Gmat::PROP_SETUP)
            obj->Initialize();
      }
   }


   //MessageInterface::ShowMessage("=====> Initialize System Parameters\n");
   // System Parameters
   for (omi = objectMap.begin(); omi != objectMap.end(); ++omi)
   {
      obj = omi->second;


      // Treat parameters as a special case -- because system parameters have
      // to be initialized before other parameters.
      if (obj->GetType() == Gmat::PARAMETER)
      {
         Parameter *param = (Parameter *)obj;
         // Make sure system parameters are configured before others
         if (param->GetKey() == GmatParam::SYSTEM_PARAM)
         {
            #ifdef DEBUG_SANDBOX_INIT_PARAM
               MessageInterface::ShowMessage(
                  "Sandbox::Initialize objTypeName = %s, objName = %s\n",
                  obj->GetTypeName().c_str(), obj->GetName().c_str());
            #endif
            param->SetSolarSystem(solarSys);
            param->SetInternalCoordSystem(internalCoordSys);
            BuildReferences(obj);
            obj->Initialize();
         }
      }
   }


   //MessageInterface::ShowMessage("=====> Initialize remaining parameters\n");
   // Do all remaining Parameters next
   for (omi = objectMap.begin(); omi != objectMap.end(); ++omi)
   {
      obj = omi->second;
      if (obj->GetType() == Gmat::PARAMETER)
      {
         Parameter *param = (Parameter *)obj;
         // Make sure system parameters are configured before others
         #ifdef DEBUG_SANDBOX_INIT
            MessageInterface::ShowMessage(
               "Sandbox::Initialize objTypeName = %s, objName = %s\n",
               obj->GetTypeName().c_str(), obj->GetName().c_str());
         #endif

         BuildReferences(obj);
         param->Initialize();
      }
   }


   //MessageInterface::ShowMessage("=====> Initialize subscribers\n");
   // Now that the references are all set, handle the Subscribers
   for (omi = objectMap.begin(); omi != objectMap.end(); ++omi)
   {
      obj = omi->second;
      // Parameters were initialized above
      if (obj->GetType() == Gmat::SUBSCRIBER)
      {
         #ifdef DEBUG_SANDBOX_INIT
            MessageInterface::ShowMessage(
               "Sandbox::Initialize objTypeName = %s, objName = %s\n",
               obj->GetTypeName().c_str(), obj->GetName().c_str());
         #endif
            
         BuildReferences(obj);
         ((Subscriber*)obj)->SetInternalCoordSystem(internalCoordSys);
         ((Subscriber*)obj)->SetSolarSystem(solarSys);
         
         //if (obj->IsOfType("OpenGLPlot"))
         //   obj->SetRefObject(solarSys, Gmat::SOLAR_SYSTEM, "");
         obj->Initialize();
      }
   }


   #if DEBUG_SANDBOX_INIT
      MessageInterface::ShowMessage(
         "Sandbox::Initialize() Initializing Commands...\n");
   #endif


   //MessageInterface::ShowMessage("=====> Initialize commands\n");
   // Initialize commands
   while (current)
   {
      #if DEBUG_SANDBOX_INIT
         MessageInterface::ShowMessage(
            "Initializing %s command\n   \"%s\"\n",
            current->GetTypeName().c_str(),
            current->GetGeneratingString(Gmat::SCRIPTING, "", "").c_str());
      #endif
         
      current->SetObjectMap(&objectMap);
      current->SetSolarSystem(solarSys);
      current->SetTransientForces(&transientForces);

      // Handle GmatFunctions
      if (current->GetTypeName() == "CallFunction")
      {
         // Check to see if it is a GmatFunction
         std::string funName = current->GetStringParameter("FunctionName");
         
         if (funName == "")
            throw SandboxException
               ("CallFunction command has empty Function name in line:\n \"" +
                current->GetGeneratingString(Gmat::SCRIPTING) + "\"");
         
         if (objectMap.find(funName) == objectMap.end())
            throw SandboxException
               ("CallFunction command references undefined Function \"" + funName +
                "\" in line:\n \"" + current->GetGeneratingString(Gmat::SCRIPTING) + "\"");
         
         GmatFunction *fun = (GmatFunction*)objectMap[funName];
         if (fun->GetTypeName() == "GmatFunction")
         {
            /// @todo Make the GmatFunction file name handling more robust
            std::string pathAndName =
               fun->GetStringParameter(fun->GetParameterID("FunctionPath"));
            if (pathAndName == "")
               pathAndName = funName + ".gmf";

            GmatCommand *funStream =
               moderator->InterpretGmatFunction(pathAndName);
            if (!current->SetRefObject(funStream, Gmat::COMMAND, ""))
            {
               std::string errstr = "Error setting the GmatFunction commands "
                  "for the script line\n  '";
               throw SandboxException(errstr +
                  current->GetGeneratingString(Gmat::SCRIPTING) + "'");
            }
            ((CallFunction *)current)->SetInternalCoordSystem(internalCoordSys);
         }
      }

      //MessageInterface::ShowMessage
      //   ("=====> calling Initialize() on %s command\n", current->GetTypeName().c_str());
      
      rv = current->Initialize();
      if (!rv)
         return false;

      current = current->GetNext();
   }


   #if DEBUG_SANDBOX_INIT
      MessageInterface::ShowMessage(
         "Sandbox::Initialize() Successfully initialized\n");
   #endif

   state = INITIALIZED;
   
   //MessageInterface::ShowMessage("=====> Initialize successful\n");
   return rv;
}



//------------------------------------------------------------------------------
// void BuildReferences()
//------------------------------------------------------------------------------
/**
 *  Sets all reference objects for the input object.
 */
//------------------------------------------------------------------------------
void Sandbox::BuildReferences(GmatBase *obj)
{
   std::string oName;

   obj->SetSolarSystem(solarSys);
   // PropSetup probably should do this...
   if ((obj->GetType() == Gmat::PROP_SETUP) ||
       (obj->GetType() == Gmat::FORCE_MODEL))
   {
      ForceModel *fm = ((PropSetup *)obj)->GetForceModel();
      fm->SetSolarSystem(solarSys);
      
      // Handle the coordinate systems
      StringArray csList = fm->GetStringArrayParameter("CoordinateSystemList");
      
      #ifdef DEBUG_SANDBOX_INIT
         MessageInterface::ShowMessage("Coordinate system list for '%s':\n",
            fm->GetName().c_str());
         for (StringArray::iterator i = csList.begin(); i != csList.end(); ++i)
            MessageInterface::ShowMessage("   %s\n", i->c_str());
      #endif
      // Set CS's on the objects
      for (StringArray::iterator i = csList.begin(); i != csList.end(); ++i)
      {
         CoordinateSystem *fixedCS = NULL;
         
         if (objectMap.find(*i) != objectMap.end())
         {
            GmatBase *ref = objectMap[*i];
            if (ref->IsOfType("CoordinateSystem") == false)
               throw SandboxException("Object named " + (*i) + 
                  " was expected to be a Coordinate System, but it has type " +
                  ref->GetTypeName());
            fixedCS = (CoordinateSystem*)ref;
            fm->SetRefObject(fixedCS, fixedCS->GetType(), *i);         
         }
         else
         {
            //MessageInterface::ShowMessage("===> create BodyFixed here\n");
            fixedCS = moderator->CreateCoordinateSystem("", false);
            AxisSystem *axes = moderator->CreateAxisSystem("BodyFixed", "");
            fixedCS->SetName(*i);
            fixedCS->SetRefObject(axes, Gmat::AXIS_SYSTEM, "");
            fixedCS->SetOriginName(fm->GetStringParameter("CentralBody"));
            
            fm->SetRefObject(fixedCS, fixedCS->GetType(), *i);         

            fixedCS->SetSolarSystem(solarSys);
            BuildReferences(fixedCS); 
            InitializeCoordinateSystem(fixedCS);
            fixedCS->Initialize();
            
            objectMap[*i] = fixedCS;
            
            #ifdef DEBUG_SANDBOX_INIT
               MessageInterface::ShowMessage(
                  "Coordinate system %s has body %s\n",
                  fixedCS->GetName().c_str(), fixedCS->GetOriginName().c_str());
            #endif
         }
      }
      
      #ifdef DEBUG_FM_INITIALIZATION
         MessageInterface::ShowMessage(
            "Initializing force model references for '%s'\n",
            (fm->GetName() == "" ? obj->GetName().c_str() :
                                   fm->GetName().c_str()) );
      #endif

      try
      {
         StringArray fmRefs = fm->GetRefObjectNameArray(Gmat::UNKNOWN_OBJECT);
         for (StringArray::iterator i = fmRefs.begin();
              i != fmRefs.end(); ++i)
         {
            oName = *i;
            try
            {
               SetRefFromName(fm, oName);
            }
            catch (SandboxException &ex)
            {
               // Handle SandboxExceptions first.
               #ifdef DEBUG_SANDBOX_INIT
                  MessageInterface::ShowMessage(
                     "RefObjectName " + oName + " not found; ignoring " +
                     ex.GetFullMessage() + "\n");
               #endif
               //throw ex;
            }
            catch (BaseException &ex)
            {
               // Post a message and -- otherwise -- ignore the exceptions
               // Handle SandboxExceptions first.
               #ifdef DEBUG_SANDBOX_INIT
                  MessageInterface::ShowMessage(
                     "RefObjectName not found; ignoring " +
                     ex.GetFullMessage() + "\n");
               #endif
            }
         }
      }
      catch (SandboxException &ex)
      {
         // Handle SandboxExceptions first.
         #ifdef DEBUG_SANDBOX_INIT
            MessageInterface::ShowMessage(
               "RefObjectNameArray not found; ignoring " +
               ex.GetFullMessage() + "\n");
         #endif
         //throw ex;
      }
      catch (BaseException &ex) // Handles no refObject array
      {
         // Post a message and -- otherwise -- ignore the exceptions
         #ifdef DEBUG_SANDBOX_INIT
            MessageInterface::ShowMessage(
               "RefObjectNameArray not found; ignoring " +
               ex.GetFullMessage() + "\n");
         #endif
      }

      if (obj->GetType() == Gmat::FORCE_MODEL)
         return;
   }


   try
   {
      // First set the individual reference objects
      oName = obj->GetRefObjectName(Gmat::UNKNOWN_OBJECT);
      SetRefFromName(obj, oName);
   }
   catch (SubscriberException &ex)
   {
      throw ex;
   }
   catch (SandboxException &ex)
   {
      // Handle SandboxExceptions first.
      // For now, post a message and -- otherwise -- ignore exceptions
      #ifdef DEBUG_SANDBOX_INIT
         MessageInterface::ShowMessage("RefObjectName not found; ignoring " +
            ex.GetFullMessage() + "\n");
      #endif
      //throw ex;
   }
   catch (BaseException &ex)
   {
      // Post a message and -- otherwise -- ignore the exceptions
      #ifdef DEBUG_SANDBOX_INIT
         MessageInterface::ShowMessage("RefObjectName not found; ignoring " +
            ex.GetFullMessage() + "\n");
      #endif
   }


   // Next handle the array version
   try
   {
      StringArray oNameArray =
         obj->GetRefObjectNameArray(Gmat::UNKNOWN_OBJECT);
      for (StringArray::iterator i = oNameArray.begin();
           i != oNameArray.end(); ++i)
      {
         oName = *i;
         try
         {
            SetRefFromName(obj, oName);
         }
         catch (SandboxException &ex)
         {
            // Handle SandboxExceptions first.
            #ifdef DEBUG_SANDBOX_INIT
               MessageInterface::ShowMessage(
                  "RefObjectName " + oName + " not found; ignoring " +
                  ex.GetFullMessage() + "\n");
            #endif
            //throw ex;
         }
         catch (SubscriberException &ex)
         {
            throw ex;
         }
         catch (BaseException &ex)
         {
            // Post a message and -- otherwise -- ignore the exceptions
            // Handle SandboxExceptions first.
            #ifdef DEBUG_SANDBOX_INIT
               MessageInterface::ShowMessage(
                  "RefObjectName not found; ignoring " +
                  ex.GetFullMessage() + "\n");
            #endif
         }
      }
   }
   catch (SandboxException &ex)
   {
      // Handle SandboxExceptions first.
      #ifdef DEBUG_SANDBOX_INIT
         MessageInterface::ShowMessage(
            "RefObjectNameArray not found; ignoring " +
            ex.GetFullMessage() + "\n");
      #endif
      //throw ex;
   }
   catch (SubscriberException &ex)
   {
      throw ex;
   }
   catch (BaseException &ex) // Handles no refObject array
   {
      // Post a message and -- otherwise -- ignore the exceptions
      #ifdef DEBUG_SANDBOX_INIT
         MessageInterface::ShowMessage(
            "RefObjectNameArray not found; ignoring " +
            ex.GetFullMessage() + "\n");
      #endif
   }


   // Temporary calls that will be removed once GetRefObjectName methods are
   // completed.
//   if (obj->GetType() == Gmat::COORDINATE_SYSTEM)
//      InitializeCoordinateSystem((CoordinateSystem *)obj);


//   // Not sure if needed...
//   if (obj->GetType() == Gmat::SPACECRAFT)
//      ((Spacecraft *)obj)->SaveDisplay();


}



//------------------------------------------------------------------------------
// void InitializeInternalObjects()
//------------------------------------------------------------------------------
/**
 *  Initializes internal objects in the sandbox.
 */
//------------------------------------------------------------------------------
void Sandbox::InitializeInternalObjects()
{
   SpacePoint *sp, *j2kBod;
   std::string j2kName, oName;

   solarSys->Initialize();
   
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

   BuildReferences(internalCoordSys);

   // Set reference origin for internal coordinate system.
   oName = internalCoordSys->GetStringParameter("Origin");
   sp = FindSpacePoint(oName);
   if (sp == NULL)
      throw SandboxException("Cannot find SpacePoint named \"" +
         oName + "\" used for the internal coordinate system origin");
   internalCoordSys->SetRefObject(sp, Gmat::SPACE_POINT, oName);


   // Set J2000 body for internal coordinate system
   oName = internalCoordSys->GetStringParameter("J2000Body");
   sp = FindSpacePoint(oName);
   if (sp == NULL)
      throw SandboxException("Cannot find SpacePoint named \"" +
         oName + "\" used for the internal coordinate system J2000 body");
   internalCoordSys->SetRefObject(sp, Gmat::SPACE_POINT, oName);


   internalCoordSys->Initialize();
}


//*********************  TEMPORARY ******************************************************************
void Sandbox::InitializeCoordinateSystem(CoordinateSystem *cs)
{
   SpacePoint *sp;
   std::string oName;


   // Set the reference objects for the coordinate system
   BuildReferences(cs);


   oName = cs->GetStringParameter("Origin");


   GmatBase *axes = cs->GetOwnedObject(0);
   BuildReferences(axes);


   sp = FindSpacePoint(oName);
   if (sp == NULL)
      throw SandboxException("Cannot find SpacePoint named \"" +
         oName + "\" used for the coordinate system " +
         cs->GetName() + " origin");


   cs->SetRefObject(sp, Gmat::SPACE_POINT, oName);


   oName = cs->GetStringParameter("J2000Body");


   sp = FindSpacePoint(oName);
   if (sp == NULL)
      throw SandboxException("Cannot find SpacePoint named \"" +
         oName + "\" used for the coordinate system " +
         cs->GetName() + " J2000 body");

   cs->SetRefObject(sp, Gmat::SPACE_POINT, oName);
}


//------------------------------------------------------------------------------
// void SetRefFromName(GmatBase *obj, const std::string &oName)
//------------------------------------------------------------------------------
/**
 *  Sets a reference object on an object.
 *
 *  @param <obj>   The object that needs to set the reference.
 *  @param <oName> The name of the reference object.
 */
//------------------------------------------------------------------------------
void Sandbox::SetRefFromName(GmatBase *obj, const std::string &oName)
{
   #ifdef DEBUG_SANDBOX_INIT
      MessageInterface::ShowMessage("Setting reference '%s' on '%s'\n", 
         oName.c_str(), obj->GetName().c_str());
   #endif
   
   if (objectMap.find(oName) != objectMap.end())
   {
      GmatBase *refObj = objectMap[oName];
      obj->SetRefObject(refObj, refObj->GetType(), refObj->GetName());
   }
   else
   {
      // look SolarSystem
      GmatBase *refObj = FindSpacePoint(oName);

      if (refObj == NULL)
         throw SandboxException("Unknown object " + oName + " requested by " +
                                obj->GetName());

      obj->SetRefObject(refObj, refObj->GetType(), refObj->GetName());
   }
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 *  Runs the mission sequence.
 *
 *  This method walks through the command linked list, firing each GmatCommand
 *  as it is encountered by calling Execute() on the commands.  Between command
 *  executions, the method check with the Moderator to see if the user has
 *  requested that the sequence be paused or halted.
 *
 *  @return true if the mission sequence was executed, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::Execute()
{

   #if DEBUG_SANDBOX_RUN > 1
   MessageInterface::ShowMessage("Sandbox::Execute() Here is the current object map:\n");
   for (std::map<std::string, GmatBase *>::iterator i = objectMap.begin();
        i != objectMap.end(); ++i)
      MessageInterface::ShowMessage("   (%p) %s\n", i->second, i->first.c_str());

   MessageInterface::ShowMessage("Sandbox::Execute() Here is the mission sequence:\n");
   std::string seq = GmatCommandUtil::GetCommandSeqString(sequence);
   MessageInterface::ShowMessage(seq);
   #endif
   
   bool rv = true;

   state = RUNNING;
   Gmat::RunState runState = Gmat::IDLE, currentState = Gmat::RUNNING;

   current = sequence;
   if (!current)
      return false;

   try
   {
      while (current)
      {
         // First check to see if the run should be interrupted
         if (Interrupt())
         {
            #ifdef DEBUG_MODERATOR_CALLBACK
            MessageInterface::ShowMessage("   Interrupted in %s command\n",
                                          current->GetTypeName().c_str());
            #endif
            
            
            if (state == PAUSED)
            {
               continue;
            }
            else
            {
               //MessageInterface::ShowMessage("Sandbox::Execution interrupted.\n");
               sequence->RunComplete();
               
               // notify subscribers end of run
               currentState = Gmat::IDLE;
               publisher->SetRunState(currentState);
               publisher->NotifyEndOfRun();
               
               throw SandboxException("Execution interrupted");
               //return rv;
            }
         }
         
         #if DEBUG_SANDBOX_RUN
         MessageInterface::ShowMessage
            ("Sandbox::Execution running %s\n   %s\n", current->GetTypeName().c_str(),
             current->GetGeneratingString().c_str());
         #endif
         
         if (currentState != runState)
         {
            publisher->SetRunState(currentState);
            runState = currentState;
         }
         
         rv = current->Execute();
                  
         if (!rv)
         {
            std::string str = "\"" + current->GetTypeName() +
               "\" Command failed to run to completion\nCommand Text is \"" +
               current->GetGeneratingString() + "\"\n";
            throw SandboxException(str);
         }
         current = current->GetNext();
      }
   }
   catch (BaseException &e)
   {
      sequence->RunComplete();
      
      #if DEBUG_SANDBOX_RUN
      MessageInterface::ShowMessage
         ("   Sandbox rethrowing %s\n", e.GetFullMessage().c_str());
      #endif
      
      throw;
   }
   
   sequence->RunComplete();
   state = STOPPED;
   
   // notify subscribers end of run
   currentState = Gmat::IDLE;
   publisher->SetRunState(currentState);
   publisher->NotifyEndOfRun();
   
   return rv;
}



//------------------------------------------------------------------------------
// bool Interrupt()
//------------------------------------------------------------------------------
/**
 *  Tests to see if the mission sequence should be interrupted.
 *
 *  @return true if the Moderator wants to interrupt execution, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::Interrupt()
{
   // Ask the moderator for the current RunState; only check at fixed frequency
   if (++interruptCount == pollFrequency)
   {
      Gmat::RunState interruptType =  moderator->GetUserInterrupt();
   
      switch (interruptType)
      {
         case Gmat::PAUSED:   // Pause
            state = PAUSED;
            break;
   
         case Gmat::IDLE:     // Stop puts GMAT into the Idle state
            state = STOPPED;
            break;
   
         case Gmat::RUNNING:   // MCS is running
            state = RUNNING;
            break;
   
         default:
            break;
      }
      interruptCount = 0;
   }
   
   if ((state == PAUSED) || (state == STOPPED))
      return true;

   return false;
}



//------------------------------------------------------------------------------
// void Clear()
//------------------------------------------------------------------------------
/**
 *  Cleans up the local object store.
 */
//------------------------------------------------------------------------------
void Sandbox::Clear()
{
   sequence  = NULL;
   current   = NULL;

   // Delete the all cloned objects
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
      if ((omi->second)->GetType() == Gmat::SUBSCRIBER)
         publisher->Unsubscribe((Subscriber*)(omi->second));
      
      #ifdef DEBUG_SANDBOX_OBJECT_MAPS
         MessageInterface::ShowMessage("Sandbox clearing %s\n",
            (omi->first).c_str());
      #endif

      #ifdef DEBUG_SANDBOX_CLONING
         if (find(clonable.begin(), clonable.end(),
             (omi->second)->GetType()) != clonable.end())
      #endif
      {
         #ifdef DEBUG_SANDBOX_OBJECT_MAPS
            MessageInterface::ShowMessage("Deleting '%s'\n",
               (omi->second)->GetName().c_str());
         #endif
         delete omi->second;
         //objectMap.erase(omi);
      }
      /// @todo Subscribers are cloned; where are they deleted?
   }

   publisher = NULL;

#ifndef DISABLE_SOLAR_SYSTEM_CLONING
   if (solarSys != NULL)
   {
      MessageInterface::ShowMessage
         ("Deleting the solar system clone in the Sandbox\n");
      delete solarSys;
   }
   
   solarSys = NULL;
#endif

   objectMap.clear();
   transientForces.clear();

   // Update the sandbox state
   if ((state != STOPPED) && (state != IDLE))
          MessageInterface::ShowMessage(
             "Unexpected state transition in the Sandbox\n");

   state     = IDLE;
}



//------------------------------------------------------------------------------
// bool AddSubscriber(Subscriber *sub)
//------------------------------------------------------------------------------
/**
 *  Add Subcribers to the Sandbox and registers them with the Publisher.
 *
 *  @param <subsc> The subscriber.
 *
 *  @return true if the Subscriber was registered.
 */
//------------------------------------------------------------------------------
bool Sandbox::AddSubscriber(Subscriber *subsc)
{
   if ((state != STOPPED) && (state != INITIALIZED) && (state != IDLE))
          MessageInterface::ShowMessage(
             "Unexpected state transition in the Sandbox\n");
   state     = IDLE;

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
// void BuildAssociations(GmatBase * obj)
//------------------------------------------------------------------------------
/**
 *  Assigns clones of objects to their owners.
 *
 *  This method finds referenced objects that need to be associated with the
 *  input object through cloning, creates the clones, and hands the cloned
 *  object to the owner.
 *
 *  An example of the associations that are made here are hardware elements that
 *  get associated with spacecraft.  Users configure a single element -- for
 *  example, a tank, and then can assign that element to many different
 *  spacecraft.  In order to avoid multiple objects using the same instance of
 *  the tank, clones are made for each spacecraft that has the tank associated
 *  with it.
 *
 *  @param <obj> The owner of the clones.
 */
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
/**
 *  Finds a SpacePoint by name.
 *
 *  @param <spname> The name of the SpacePoint.
 *
 *  @return A pointer to the SpacePoint, or NULL if it does not exist in the
 *          Sandbox.
 */
//------------------------------------------------------------------------------
SpacePoint * Sandbox::FindSpacePoint(const std::string &spName)
{
   SpacePoint *sp = solarSys->GetBody(spName);


   if (sp == NULL)
   {
      if (objectMap.find(spName) != objectMap.end())
      {
         if (objectMap[spName]->IsOfType(Gmat::SPACE_POINT))
            sp = (SpacePoint*)(objectMap[spName]);
      }
   }


   return sp;
}
