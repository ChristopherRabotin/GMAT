//$Id$
//------------------------------------------------------------------------------
//                                  ObjectInitializer
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CCA54C
//
// Author: Wendy Shoan
// Created: 2008.06.11
//
// Original code from the Sandbox.
// Author: Darrel J. Conway
//
/**
 * Implementation for the ObjectInitializer class.
 * This class initializes objects of the specified LocalObjectStore and ,
 * on option, GlobalObjectStore.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Moderator.hpp"
#include "ObjectInitializer.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "GmatBaseException.hpp"  // need a specific one here?  ObjectExecption?
#include "SubscriberException.hpp"
#include "Publisher.hpp"

//#define DEBUG_OBJECT_INITIALIZER
//#define DEBUG_SUBSCRIBER

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
ObjectInitializer::ObjectInitializer(SolarSystem *solSys, ObjectMap *objMap,
                                     ObjectMap *globalObjMap, CoordinateSystem *internalCS, 
                                     bool useGOS) :

   ss         (solSys),
   LOS        (objMap),
   GOS        (globalObjMap),
   mod        (NULL),
   cs         (internalCS),
   includeGOS (useGOS)
{
   mod = Moderator::Instance();
   publisher = Publisher::Instance();
}

ObjectInitializer::ObjectInitializer(const ObjectInitializer &objInit) :
   ss         (objInit.ss),
   LOS        (objInit.LOS),
   GOS        (objInit.GOS),
   mod        (NULL),
   cs         (objInit.cs),
   includeGOS (objInit.includeGOS)
{
   mod = Moderator::Instance();
   publisher = Publisher::Instance();
}

ObjectInitializer& ObjectInitializer::operator= (const ObjectInitializer &objInit)
{
   if (&objInit != this)
   {
      ss         = objInit.ss;
      LOS        = objInit.LOS;
      GOS        = objInit.GOS;
      mod        = objInit.mod;
      cs         = objInit.cs;
      includeGOS = objInit.includeGOS;
      publisher  = objInit.publisher;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
ObjectInitializer::~ObjectInitializer()
{
}

void ObjectInitializer::SetSolarSystem(SolarSystem *solSys)
{
   ss = solSys;
}

void ObjectInitializer::SetObjectMap(ObjectMap *objMap)
{
   LOS = objMap;
}

void ObjectInitializer::SetCoordinateSystem(CoordinateSystem* internalCS)
{
   cs = internalCS;
}

bool ObjectInitializer::InitializeObjects(bool registerSubs)
{
   std::map<std::string, GmatBase *>::iterator omIter;
   std::map<std::string, GmatBase *>::iterator omi;
   GmatBase *obj = NULL;
   std::string oName;
   std::string j2kName;

   #ifdef DEBUG_OBJECT_INITIALIZER
       MessageInterface::ShowMessage("About to Initialize Internal Objects ...\n");
    #endif
    InitializeInternalObjects();
    #ifdef DEBUG_OBJECT_INITIALIZER
       MessageInterface::ShowMessage("Internal Objects Initialized ...\n");
    #endif
   // Set J2000 Body for all SpacePoint derivatives before anything else
   // NOTE - at this point, everything should be in the SandboxObjectMap,
   // and the GlobalObjectMap should be empty
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage("--- Right before setting the J2000 body ---\n");
      MessageInterface::ShowMessage("The Object Map contains:\n");
      for (omIter = LOS->begin(); omIter != LOS->end(); ++omIter)
         MessageInterface::ShowMessage("   %s of type %s\n",
               (omIter->first).c_str(), ((omIter->second)->GetTypeName()).c_str());
      MessageInterface::ShowMessage("The Global Object Map contains:\n");
      for (omIter = GOS->begin(); omIter != GOS->end(); ++omIter)
         MessageInterface::ShowMessage("   %s of type %s\n",
               (omIter->first).c_str(), ((omIter->second)->GetTypeName()).c_str());
   #endif
   for (omi = LOS->begin(); omi != LOS->end(); ++omi)
   {
      obj = omi->second;
      if (obj->IsOfType(Gmat::SPACE_POINT))
      {
         #ifdef DEBUG_OBJECT_INITIALIZER
            MessageInterface::ShowMessage(
               "Setting J2000 Body for %s in the ObjectInitializer\n",
               obj->GetName().c_str());
         #endif
         SpacePoint *spObj = (SpacePoint *)obj;
         SpacePoint* j2k = FindSpacePoint(spObj->GetJ2000BodyName());
         if (j2k)
            spObj->SetJ2000Body(j2k);
         else
            throw GmatBaseException("ObjectInitializer did not find the Spacepoint \"" +
               spObj->GetJ2000BodyName() + "\"");
      }
   }
   if (includeGOS)
   {
      for (omi = GOS->begin(); omi != GOS->end(); ++omi)
      {
         obj = omi->second;
         if (obj->IsOfType(Gmat::SPACE_POINT))
         {
            #ifdef DEBUG_OBJECT_INITIALIZER
               MessageInterface::ShowMessage(
                  "Setting J2000 Body for %s in the ObjectInitializer\n",
                  obj->GetName().c_str());
            #endif
            SpacePoint *spObj = (SpacePoint *)obj;
            SpacePoint* j2k = FindSpacePoint(spObj->GetJ2000BodyName());
            if (j2k)
               spObj->SetJ2000Body(j2k);
            else
               throw GmatBaseException("ObjectInitializer did not find the Spacepoint \"" +
                  spObj->GetJ2000BodyName() + "\"");
         }
      }
   }
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage("J2000 Body set ...\n");
   #endif

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
   for (omi = LOS->begin(); omi != LOS->end(); ++omi)
   {
      obj = omi->second;
      if (obj->GetType() == Gmat::COORDINATE_SYSTEM)
      {
         #ifdef DEBUG_OBJECT_INITIALIZER
            MessageInterface::ShowMessage("Initializing CS %s\n",
               obj->GetName().c_str());
         #endif
         obj->SetSolarSystem(ss);
         BuildReferences(obj);
         InitializeCoordinateSystem((CoordinateSystem *)obj);
         obj->Initialize();
      }
   }
   
   if (includeGOS)
   {
      for (omi = GOS->begin(); omi != GOS->end(); ++omi)
      {
         obj = omi->second;
         if (obj->GetType() == Gmat::COORDINATE_SYSTEM)
         {
            #ifdef DEBUG_OBJECT_INITIALIZER
               MessageInterface::ShowMessage("Initializing CS %s\n",
                  obj->GetName().c_str());
            #endif
            obj->SetSolarSystem(ss);
            BuildReferences(obj);
            InitializeCoordinateSystem((CoordinateSystem *)obj);
            obj->Initialize();
         }
      }
   }

   // Spacecraft
   for (omi = LOS->begin(); omi != LOS->end(); ++omi)
   {
      obj = omi->second;
      if (obj->GetType() == Gmat::SPACECRAFT)
      {
         obj->SetSolarSystem(ss);
         ((Spacecraft *)obj)->SetInternalCoordSystem(cs);

         BuildReferences(obj);

         // Setup spacecraft hardware
         BuildAssociations(obj);

         obj->Initialize();
      }
   }
   
   if (includeGOS)
   {
      for (omi = GOS->begin(); omi != GOS->end(); ++omi)
      {
         obj = omi->second;
         if (obj->GetType() == Gmat::SPACECRAFT)
         {
            obj->SetSolarSystem(ss);
            ((Spacecraft *)obj)->SetInternalCoordSystem(cs);

            BuildReferences(obj);

            // Setup spacecraft hardware
            BuildAssociations(obj);

            obj->Initialize();
         }
      }
   }


   // All others except Parameters and Subscribers
   // Spacecraft
   for (omi = LOS->begin(); omi != LOS->end(); ++omi)
   {
      obj = omi->second;
      if ((obj->GetType() != Gmat::COORDINATE_SYSTEM) &&
          (obj->GetType() != Gmat::SPACECRAFT) &&
          (obj->GetType() != Gmat::PARAMETER) &&
          (obj->GetType() != Gmat::SUBSCRIBER))
      {
         #ifdef DEBUG_OBJECT_INITIALIZER
            MessageInterface::ShowMessage(
               "ObjectInitializer::Initialize objTypeName = %s, objName = %s\n",
               obj->GetTypeName().c_str(), obj->GetName().c_str());
         #endif


         //*************************** TEMPORARY *******************************
         if (obj->GetType() != Gmat::PROP_SETUP)
         {
            obj->SetSolarSystem(ss);
            if (obj->IsOfType(Gmat::SPACE_POINT))
               BuildReferences(obj);
            continue;
         }
         #ifdef DEBUG_OBJECT_INITIALIZER
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
   
   if (includeGOS)
   {
      for (omi = GOS->begin(); omi != GOS->end(); ++omi)
      {
         obj = omi->second;
         if ((obj->GetType() != Gmat::COORDINATE_SYSTEM) &&
             (obj->GetType() != Gmat::SPACECRAFT) &&
             (obj->GetType() != Gmat::PARAMETER) &&
             (obj->GetType() != Gmat::SUBSCRIBER))
         {
            #ifdef DEBUG_OBJECT_INITIALIZER
               MessageInterface::ShowMessage(
                  "ObjectInitializer::Initialize objTypeName = %s, objName = %s\n",
                  obj->GetTypeName().c_str(), obj->GetName().c_str());
            #endif


            //*************************** TEMPORARY *******************************
            if (obj->GetType() != Gmat::PROP_SETUP)
            {
               obj->SetSolarSystem(ss);
               if (obj->IsOfType(Gmat::SPACE_POINT))
                  BuildReferences(obj);
               continue;
            }
            #ifdef DEBUG_OBJECT_INITIALIZER
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
   }


   //MessageInterface::ShowMessage("=====> Initialize System Parameters\n");
   // System Parameters
   for (omi = LOS->begin(); omi != LOS->end(); ++omi)
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
            #ifdef DEBUG_OBJECT_INITIALIZER
               MessageInterface::ShowMessage(
                  "ObjectInitializer::Initialize objTypeName = %s, objName = %s\n",
                  obj->GetTypeName().c_str(), obj->GetName().c_str());
            #endif
            param->SetSolarSystem(ss);
            param->SetInternalCoordSystem(cs);
            BuildReferences(obj);
            obj->Initialize();
         }
      }
   }
   
   if (includeGOS)
   {
      for (omi = GOS->begin(); omi != GOS->end(); ++omi)
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
               #ifdef DEBUG_OBJECT_INITIALIZER
                  MessageInterface::ShowMessage(
                     "ObjectInitializer::Initialize objTypeName = %s, objName = %s\n",
                     obj->GetTypeName().c_str(), obj->GetName().c_str());
               #endif
               param->SetSolarSystem(ss);
               param->SetInternalCoordSystem(cs);
               BuildReferences(obj);
               obj->Initialize();
            }
         }
      }
   }


   //MessageInterface::ShowMessage("=====> Initialize remaining parameters\n");
   // Do all remaining Parameters next
   for (omi = LOS->begin(); omi != LOS->end(); ++omi)
   {
      obj = omi->second;
      if (obj->GetType() == Gmat::PARAMETER)
      {
         Parameter *param = (Parameter *)obj;
         // Make sure system parameters are configured before others
         #ifdef DEBUG_OBJECT_INITIALIZER
            MessageInterface::ShowMessage(
               "ObjectInitializer::Initialize objTypeName = %s, objName = %s\n",
               obj->GetTypeName().c_str(), obj->GetName().c_str());
         #endif
         
         BuildReferences(obj);
         param->Initialize();
      }
   }
   
   if (includeGOS)
   {
      for (omi = GOS->begin(); omi != GOS->end(); ++omi)
      {
         obj = omi->second;
         if (obj->GetType() == Gmat::PARAMETER)
         {
            Parameter *param = (Parameter *)obj;
            // Make sure system parameters are configured before others
            #ifdef DEBUG_OBJECT_INITIALIZER
               MessageInterface::ShowMessage(
                  "ObjectInitializer::Initialize objTypeName = %s, objName = %s\n",
                  obj->GetTypeName().c_str(), obj->GetName().c_str());
            #endif
            
            BuildReferences(obj);
            param->Initialize();
         }
      }
   }
   
   
   //MessageInterface::ShowMessage("=====> Initialize subscribers\n");
   // Now that the references are all set, handle the Subscribers
   for (omi = LOS->begin(); omi != LOS->end(); ++omi)
   {
      obj = omi->second;
      // Parameters were initialized above
      if (obj->GetType() == Gmat::SUBSCRIBER)
      {
         #ifdef DEBUG_OBJECT_INITIALIZER
            MessageInterface::ShowMessage(
               "ObjectInitializer::Initialize objTypeName = %s, objName = %s\n",
               obj->GetTypeName().c_str(), obj->GetName().c_str());
         #endif
         
         BuildReferences(obj);
         ((Subscriber*)obj)->SetInternalCoordSystem(cs);
         ((Subscriber*)obj)->SetSolarSystem(ss);
         obj->Initialize();
         
         // Check if we need to register subscribers to the publisher (loj:2008.06.19)
         if (registerSubs)
         {
            #ifdef DEBUG_SUBSCRIBER
            MessageInterface::ShowMessage
               ("ObjectInitializer registering local subscriber '%s' of type '%s' "
                "to publisher\n", obj->GetName().c_str(), obj->GetTypeName().c_str());
            #endif
            publisher->Subscribe((Subscriber*)obj);
         }
      }
   }
   
   if (includeGOS)
   {
      for (omi = GOS->begin(); omi != GOS->end(); ++omi)
      {
         obj = omi->second;
         // Parameters were initialized above
         if (obj->GetType() == Gmat::SUBSCRIBER)
         {
            #ifdef DEBUG_OBJECT_INITIALIZER
               MessageInterface::ShowMessage(
                  "ObjectInitializer::Initialize objTypeName = %s, objName = %s\n",
                  obj->GetTypeName().c_str(), obj->GetName().c_str());
            #endif
            
            BuildReferences(obj);
            ((Subscriber*)obj)->SetInternalCoordSystem(cs);
            ((Subscriber*)obj)->SetSolarSystem(ss);
            obj->Initialize();
            
            // Check if we need to register subscribers to the publisher (loj:2008.06.19)
            if (registerSubs)
            {
               #ifdef DEBUG_SUBSCRIBER
               MessageInterface::ShowMessage
                  ("ObjectInitializer registering global subscriber '%s' of type '%s' "
                   "to publisher\n", obj->GetName().c_str(), obj->GetTypeName().c_str());
               #endif
               publisher->Subscribe((Subscriber*)obj);
            }
         }
      }
   }
   
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage("ObjectInitializer: Objects Initialized ...\n");
   #endif
      
   return true;
   
}

//------------------------------------------------------------------------------
// void InitializeInternalObjects()
//------------------------------------------------------------------------------
/**
 *  Initializes internal objects in the sandbox.
 */
//------------------------------------------------------------------------------
void ObjectInitializer::InitializeInternalObjects()
{
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage("Entering InitializeInternalObjects ...\n");
   #endif
   SpacePoint *sp, *j2kBod;
   std::string j2kName, oName;
   
   if (ss == NULL)
      throw GmatBaseException("ObjectInitializer::InitializeInternalObjects() "
                              "The Solar System pointer is NULL");
   
   if (cs == NULL)
      throw GmatBaseException("ObjectInitializer::InitializeInternalObjects() "
                              "The Internal Coordinate System pointer is NULL");
   
   //#ifdef DEBUG_OBJECT_INITIALIZER
   //if (!ss)
   //   MessageInterface::ShowMessage("Solar System pointer is NULL!!!!!! ...\n");
   //#endif
   ss->Initialize();
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage(" ... and solar system is initialized  ...\n");
   #endif
   
   // Set J2000 bodies for solar system objects -- should this happen here?
   const StringArray biu = ss->GetBodiesInUse();
   for (StringArray::const_iterator i = biu.begin(); i != biu.end(); ++i)
   {
      #ifdef DEBUG_OBJECT_INITIALIZER
         MessageInterface::ShowMessage(" ... initializing body %s ...\n", (*i).c_str());
      #endif
      sp = ss->GetBody(*i);
      j2kName = sp->GetStringParameter("J2000BodyName");
      j2kBod = FindSpacePoint(j2kName);
      sp->SetJ2000Body(j2kBod);
   }

   // set ref object for internal coordinate system
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage(" ... solar system about to be set on coordinate system  ...\n");
      //if (!cs) MessageInterface::ShowMessage(" but solar system is NULL!!!!!!\n");
   #endif
   
   cs->SetSolarSystem(ss);
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage(" ... and solar system is set on coordinate system  ...\n");
      MessageInterface::ShowMessage(" ... about to call BuildReferences  ...\n");
   #endif

   BuildReferences(cs);

   // Set reference origin for internal coordinate system.
   oName = cs->GetStringParameter("Origin");
   sp = FindSpacePoint(oName);
   if (sp == NULL)
      throw GmatBaseException("Cannot find SpacePoint named \"" +
         oName + "\" used for the internal coordinate system origin");
   cs->SetRefObject(sp, Gmat::SPACE_POINT, oName);


   // Set J2000 body for internal coordinate system
   oName = cs->GetStringParameter("J2000Body");
   sp = FindSpacePoint(oName);
   if (sp == NULL)
      throw GmatBaseException("Cannot find SpacePoint named \"" +
         oName + "\" used for the internal coordinate system J2000 body");
   cs->SetRefObject(sp, Gmat::SPACE_POINT, oName);

   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage(" ... about to call Initialize on cs  ...\n");
   #endif

   cs->Initialize();
}


//*********************  TEMPORARY ******************************************************************
void ObjectInitializer::InitializeCoordinateSystem(CoordinateSystem *cs)
{
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage("Entering ObjectInitializer::InitializeCoordinateSystem ...\n");
   #endif
   SpacePoint *sp;
   std::string oName;


   // Set the reference objects for the coordinate system
   BuildReferences(cs);


   oName = cs->GetStringParameter("Origin");


   GmatBase *axes = cs->GetOwnedObject(0);
   BuildReferences(axes);


   sp = FindSpacePoint(oName);
   if (sp == NULL)
      throw GmatBaseException("Cannot find SpacePoint named \"" +
         oName + "\" used for the coordinate system " +
         cs->GetName() + " origin");


   cs->SetRefObject(sp, Gmat::SPACE_POINT, oName);


   oName = cs->GetStringParameter("J2000Body");


   sp = FindSpacePoint(oName);
   if (sp == NULL)
      throw GmatBaseException("Cannot find SpacePoint named \"" +
         oName + "\" used for the coordinate system " +
         cs->GetName() + " J2000 body");

   cs->SetRefObject(sp, Gmat::SPACE_POINT, oName);
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage("Exiting ObjectInitializer::InitializeCoordinateSystem ...\n");
   #endif
}



//------------------------------------------------------------------------------
// void BuildReferences()
//------------------------------------------------------------------------------
/**
 *  Sets all reference objects for the input object.
 */
//------------------------------------------------------------------------------
void ObjectInitializer::BuildReferences(GmatBase *obj)
{
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage("Entering ObjectInitializer::BuildReferences ...\n");
   #endif
   std::string oName;

   obj->SetSolarSystem(ss);
   // PropSetup probably should do this...
   if ((obj->GetType() == Gmat::PROP_SETUP) ||
       (obj->GetType() == Gmat::FORCE_MODEL))
   {
      #ifdef DEBUG_OBJECT_INITIALIZER
         MessageInterface::ShowMessage("--- it is a PropSetup or ForceModel ...\n");
      #endif
      ForceModel *fm = ((PropSetup *)obj)->GetForceModel();
      fm->SetSolarSystem(ss);
      
      // Handle the coordinate systems
      StringArray csList = fm->GetStringArrayParameter("CoordinateSystemList");
      
      #ifdef DEBUG_OBJECT_INITIALIZER
         MessageInterface::ShowMessage("Coordinate system list for '%s':\n",
            fm->GetName().c_str());
         for (StringArray::iterator i = csList.begin(); i != csList.end(); ++i)
            MessageInterface::ShowMessage("   %s\n", i->c_str());
      #endif
      // Set CS's on the objects
      for (StringArray::iterator i = csList.begin(); i != csList.end(); ++i)
      {
         CoordinateSystem *fixedCS = NULL;

         if (LOS->find(*i) != LOS->end())
         {
            GmatBase *ref = (*LOS)[*i];
            if (ref->IsOfType("CoordinateSystem") == false)
               throw GmatBaseException("Object named " + (*i) + 
                  " was expected to be a Coordinate System, but it has type " +
                  ref->GetTypeName());
            fixedCS = (CoordinateSystem*)ref;
            fm->SetRefObject(fixedCS, fixedCS->GetType(), *i); 
         }
         else if (GOS->find(*i) != GOS->end())
         {
            GmatBase *ref = (*GOS)[*i];
            if (ref->IsOfType("CoordinateSystem") == false)
               throw GmatBaseException("Object named " + (*i) + 
                  " was expected to be a Coordinate System, but it has type " +
                  ref->GetTypeName());
            fixedCS = (CoordinateSystem*)ref;
            fm->SetRefObject(fixedCS, fixedCS->GetType(), *i); 
         }
         else
         {
            //MessageInterface::ShowMessage("===> create BodyFixed here\n");
            fixedCS = mod->CreateCoordinateSystem("", false);
            AxisSystem *axes = mod->CreateAxisSystem("BodyFixed", "");
            fixedCS->SetName(*i);
            fixedCS->SetRefObject(axes, Gmat::AXIS_SYSTEM, "");
            fixedCS->SetOriginName(fm->GetStringParameter("CentralBody"));
            
            fm->SetRefObject(fixedCS, fixedCS->GetType(), *i);         

            fixedCS->SetSolarSystem(ss);
            BuildReferences(fixedCS); 
            InitializeCoordinateSystem(fixedCS);
            fixedCS->Initialize();
            
            // if things have already been moved to the globalObjectStore, put it there
            if ((GOS->size() > 0) && (fixedCS->GetIsGlobal()))
               (*GOS)[*i] = fixedCS;
            // otherwise, put it in the Sandbox object map - it will be moved to the GOS later
            else
               (*LOS)[*i] = fixedCS;
            
            #ifdef DEBUG_OBJECT_INITIALIZER
               MessageInterface::ShowMessage(
                  "Coordinate system %s has body %s\n",
                  fixedCS->GetName().c_str(), fixedCS->GetOriginName().c_str());
            #endif
         }
      }
      
      #ifdef DEBUG_OBJECT_INITIALIZER
         MessageInterface::ShowMessage(
            "Initializing force model references for '%s'\n",
            (fm->GetName() == "" ? obj->GetName().c_str() :
                                   fm->GetName().c_str()) );
      #endif

      try
      {
         StringArray fmRefs = fm->GetRefObjectNameArray(Gmat::UNKNOWN_OBJECT);
         #ifdef DEBUG_OBJECT_INITIALIZER
            MessageInterface::ShowMessage("fmRefs are:\n");
            for (unsigned int ii=0; ii < fmRefs.size(); ii++)
               MessageInterface::ShowMessage(" --- %s\n", (fmRefs.at(ii)).c_str());
         #endif
         for (StringArray::iterator i = fmRefs.begin();
              i != fmRefs.end(); ++i)
         {
            oName = *i;
            try
            {
               SetRefFromName(fm, oName);
            }
            catch (GmatBaseException &ex)  // *******
            {
               // Handle SandboxExceptions first.
               #ifdef DEBUG_OBJECT_INITIALIZER
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
               #ifdef DEBUG_OBJECT_INITIALIZER
                  MessageInterface::ShowMessage(
                     "RefObjectName not found; ignoring " +
                     ex.GetFullMessage() + "\n");
               #endif
            }
         }
      }
      catch (GmatBaseException &ex) // **********
      {
         // Handle SandboxExceptions first.
         #ifdef DEBUG_OBJECT_INITIALIZER
            MessageInterface::ShowMessage(
               "RefObjectNameArray not found; ignoring " +
               ex.GetFullMessage() + "\n");
         #endif
         //throw ex;
      }
      catch (BaseException &ex) // Handles no refObject array
      {
         // Post a message and -- otherwise -- ignore the exceptions
         #ifdef DEBUG_OBJECT_INITIALIZER
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
      #ifdef DEBUG_OBJECT_INITIALIZER
         MessageInterface::ShowMessage(
               "Attempting to set individual reference objects ...\n");
      #endif
      oName = obj->GetRefObjectName(Gmat::UNKNOWN_OBJECT);
      #ifdef DEBUG_OBJECT_INITIALIZER
         MessageInterface::ShowMessage(
               "Attempting to set individual reference objects with name = %s...\n",
               oName.c_str());
      #endif
      SetRefFromName(obj, oName);
   }
   catch (SubscriberException &ex)
   {
      throw ex;
   }
   catch (GmatBaseException &ex) // ************
   {
      // Handle SandboxExceptions first.
      // For now, post a message and -- otherwise -- ignore exceptions
      #ifdef DEBUG_OBJECT_INITIALIZER
         MessageInterface::ShowMessage("RefObjectName not found; ignoring " +
            ex.GetFullMessage() + "\n");
      #endif
      //throw ex;
   }
   catch (BaseException &ex)
   {
      // Post a message and -- otherwise -- ignore the exceptions
      #ifdef DEBUG_OBJECT_INITIALIZER
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
         #ifdef DEBUG_OBJECT_INITIALIZER
            MessageInterface::ShowMessage(
                  "Attempting to set ARRAY object with name = %s...\n",
                  oName.c_str());
         #endif
         try
         {
            SetRefFromName(obj, oName);
         }
         catch (GmatBaseException &ex) // ***********
         {
            // Handle SandboxExceptions first.
            #ifdef DEBUG_OBJECT_INITIALIZER
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
            #ifdef DEBUG_OBJECT_INITIALIZER
               MessageInterface::ShowMessage(
                  "RefObjectName not found; ignoring " +
                  ex.GetFullMessage() + "\n");
            #endif
         }
      }
   }
   catch (GmatBaseException &ex)  // ***********
   {
      // Handle SandboxExceptions first.
      #ifdef DEBUG_OBJECT_INITIALIZER
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
      #ifdef DEBUG_OBJECT_INITIALIZER
         MessageInterface::ShowMessage(
            "RefObjectNameArray not found; ignoring " +
            ex.GetFullMessage() + "\n");
      #endif
   }
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage("Exiting BuildReferences ...\n");
   #endif
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
void ObjectInitializer::SetRefFromName(GmatBase *obj, const std::string &oName)
{
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage("Setting reference '%s' on '%s'\n", 
         oName.c_str(), obj->GetName().c_str());
   #endif
   
   GmatBase *refObj = NULL;

   if ((refObj = FindObject(oName)) != NULL)
      obj->SetRefObject(refObj, refObj->GetType(), refObj->GetName());
   else
   {
      // look in the SolarSystem
      refObj = FindSpacePoint(oName);

      if (refObj == NULL)
         throw GmatBaseException("Unknown object " + oName + " requested by " +
                                  obj->GetName());

      obj->SetRefObject(refObj, refObj->GetType(), refObj->GetName());
   }
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
void ObjectInitializer::BuildAssociations(GmatBase * obj)
{
   // Spacecraft receive clones of the associated hardware objects
   if (obj->GetType() == Gmat::SPACECRAFT) 
   {
      StringArray hw = obj->GetRefObjectNameArray(Gmat::HARDWARE);
      for (StringArray::iterator i = hw.begin(); i < hw.end(); ++i) {

         #ifdef DEBUG_OBJECT_INITIALIZER
            MessageInterface::ShowMessage
               ("ObjectInitializer::BuildAssociations() setting \"%s\" on \"%s\"\n",
                i->c_str(), obj->GetName().c_str());
         #endif

         GmatBase *el = NULL;
         if ((el = FindObject(*i)) == NULL)
            throw GmatBaseException("ObjectInitializer::BuildAssociations: Cannot find "
                                    "hardware element \"" + (*i) + "\"\n");
         //GmatBase *el = objectMap[*i];
         GmatBase *newEl = el->Clone();
         #ifdef DEBUG_OBJECT_INITIALIZER
            MessageInterface::ShowMessage
               ("ObjectInitializer::BuildAssociations() created clone \"%s\" of type \"%s\"\n",
               newEl->GetName().c_str(), newEl->GetTypeName().c_str());
         #endif
         if (!obj->SetRefObject(newEl, newEl->GetType(), newEl->GetName()))
            MessageInterface::ShowMessage
               ("ObjectInitializer::BuildAssociations() failed to set %s\n",
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
 *          stores.
 */
//------------------------------------------------------------------------------
SpacePoint * ObjectInitializer::FindSpacePoint(const std::string &spName)
{
   SpacePoint *sp = ss->GetBody(spName);


   if (sp == NULL)
   {
      GmatBase *spObj;
      if ((spObj = FindObject(spName)) != NULL)
      {
         if (spObj->IsOfType(Gmat::SPACE_POINT))
            sp = (SpacePoint*)(spObj);
      }
   }

   return sp;
}

//------------------------------------------------------------------------------
// GmatBase* ObjectInitializer::FindObject(const std::string &name)
//------------------------------------------------------------------------------
/**
 *  Finds an object by name, searching through the LOS first,
 *  then the GOS
 *
 *  @param <name> The name of the object.
 *
 *  @return A pointer to the object, or NULL if it does not exist in the
 *          object stores.
 */
//------------------------------------------------------------------------------
GmatBase* ObjectInitializer::FindObject(const std::string &name)
{
   if (LOS->find(name) == LOS->end())
   {
     // If not found in the LOS, check the Global Object Store (GOS)
      if (includeGOS)
      {
         if (GOS->find(name) == GOS->end())
            return NULL;
         else return (*GOS)[name];
      }
      else return NULL;
   }
   else
      return (*LOS)[name];
}

ObjectInitializer::ObjectInitializer() :
   ss         (NULL),
   LOS        (NULL),
   GOS        (NULL),
   mod        (NULL),
   cs         (NULL),
   includeGOS (false)
{
}

