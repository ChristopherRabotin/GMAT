//$Header$
//------------------------------------------------------------------------------
//                                 Moderator
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/08/25
//
/**
 * Implements operations of the GMAT executive.
 */
//------------------------------------------------------------------------------

#include "Moderator.hpp"
#include "NoOp.hpp"
#include "GravityField.hpp"
#include "TimeSystemConverter.hpp" // for SetLeapSecsFileReader(), SetEopFile()
#include "BodyFixedAxes.hpp"       // for SetEopFile(), SetCoefficientsFile()
#include "ObjectReferencedAxes.hpp"
#include "MessageInterface.hpp"
#include "CommandUtil.hpp"         // for GetCommandSeq()
#include "StringTokenizer.hpp"
#include <ctime>                   // for clock()

//loj: 1/18/06 debug
//#define DEBUG_INIT 1
//#define DEBUG_RUN 1
//#define DEBUG_CREATE_RESOURCE 1
//#define DEBUG_RENAME 1
//#define DEBUG_DEFAULT_MISSION 1
//#define DEBUG_PLANETARY_FILE 1
//#define DEBUG_MULTI_STOP 2
//#define DEBUG_USER_INTERRUPT 1
//#define DEBUG_ACTION_REMOVE 1
//#define DEBUG_LOOKUP_RESOURCE 1
//#define DEBUG_SEQUENCE_CLEARING 1
//#define DEBUG_CONFIG 1
//#define DEBUG_FINALIZE 1

//#define DEBUG_CREATE_VAR 1
//#define DEBUG_CREATE_BURN_PARAM 1

//#define __SHOW_FINAL_STATE__

//---------------------------------
// static data
//---------------------------------
Moderator* Moderator::instance = NULL;
GuiInterpreter* Moderator::theGuiInterpreter = NULL;
ScriptInterpreter* Moderator::theScriptInterpreter = NULL;

const std::string
Moderator::PLANETARY_SOURCE_STRING[PlanetarySourceCount] =
{
   "Analytic",
   "SLP",
   "DE200",
   "DE405",
};

const std::string
Moderator::ANALYTIC_MODEL_STRING[AnalyticModelCount] =
{
   "Low Fidelity",
};

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// Moderator* Instance()
//------------------------------------------------------------------------------
Moderator* Moderator::Instance()
{
   if (instance == NULL)
      instance = new Moderator;
    
   return instance;
}


//------------------------------------------------------------------------------
// bool Initialize(bool fromGui = false)
//------------------------------------------------------------------------------
bool Moderator::Initialize(bool fromGui)
{
   isFromGui = fromGui;
   
   try
   {
      // Read startup file, Set Log file
      theFileManager = FileManager::Instance();
      theFileManager->ReadStartupFile();
      
      MessageInterface::ShowMessage("Moderator is creating core engine...\n");
      
      // Create interpreters and managers
      theGuiInterpreter = GuiInterpreter::Instance();
      theScriptInterpreter = ScriptInterpreter::Instance();
      theFactoryManager = FactoryManager::Instance();
      theConfigManager = ConfigManager::Instance();
      
      // Create publisher
      thePublisher = Publisher::Instance();
      
      // Create factories
      theAtmosphereFactory = new AtmosphereFactory();
      theAxisSystemFactory = new AxisSystemFactory();
      theBurnFactory = new BurnFactory();
      theCalculatedPointFactory = new CalculatedPointFactory();
      theCommandFactory = new CommandFactory();
      theCoordinateSystemFactory = new CoordinateSystemFactory();
      theForceModelFactory = new ForceModelFactory();
      theFunctionFactory = new FunctionFactory();
      theHardwareFactory = new HardwareFactory();
      theParameterFactory = new ParameterFactory();
      thePhysicalModelFactory = new PhysicalModelFactory();
      thePropagatorFactory = new PropagatorFactory();
      thePropSetupFactory = new PropSetupFactory();
      theSolverFactory = new SolverFactory();
      theSpacecraftFactory = new SpacecraftFactory();
      theStopConditionFactory = new StopConditionFactory();
      theSubscriberFactory = new SubscriberFactory();
      
      // Register factories
      theFactoryManager->RegisterFactory(theAtmosphereFactory);
      theFactoryManager->RegisterFactory(theAxisSystemFactory);
      theFactoryManager->RegisterFactory(theBurnFactory);
      theFactoryManager->RegisterFactory(theCalculatedPointFactory);
      theFactoryManager->RegisterFactory(theCommandFactory);
      theFactoryManager->RegisterFactory(theCoordinateSystemFactory);
      theFactoryManager->RegisterFactory(theForceModelFactory);
      theFactoryManager->RegisterFactory(theFunctionFactory);
      theFactoryManager->RegisterFactory(theHardwareFactory);
      theFactoryManager->RegisterFactory(theParameterFactory);
      theFactoryManager->RegisterFactory(thePhysicalModelFactory);
      theFactoryManager->RegisterFactory(thePropagatorFactory);
      theFactoryManager->RegisterFactory(thePropSetupFactory);
      theFactoryManager->RegisterFactory(theSolverFactory);
      theFactoryManager->RegisterFactory(theSpacecraftFactory);
      theFactoryManager->RegisterFactory(theStopConditionFactory);
      theFactoryManager->RegisterFactory(theSubscriberFactory);
      
      // Create default SolarSystem
      /// @note: If the solar system can be configured by name, add it to the
      ///        ConfigManager by calling CreateSolarSystem().
      ///        Until then, just use solar system name as "SolarSystem"
      
      theDefaultSolarSystem = new SolarSystem("SolarSystem");
      theConfigManager->SetDefaultSolarSystem(theDefaultSolarSystem);
      theSolarSystemInUse = new SolarSystem("SolarSystem");
      
      StringArray bodies = theSolarSystemInUse->GetBodiesInUse();
      SpacePoint *sp;
      SpacePoint *earth = theSolarSystemInUse->GetBody("Earth");
      
      // Set J2000Body to SolarSystem bodies
      for (UnsignedInt i=0; i<bodies.size(); i++)
      {
         sp = theSolarSystemInUse->GetBody(bodies[i]);
         sp->SetJ2000Body(earth);
      }
      
      //MessageInterface::ShowMessage
      //   ("Moderator::Initialize() theDefaultSolarSystem created\n");
      
      // Create internal CoordinateSystem with noname, so that it will not
      // be configured
      theInternalCoordSystem = CreateCoordinateSystem("", true);
      theInternalCoordSystem->SetName("EarthMJ2000Eq");
      
      // Set object pointers so that it can be used in CS conversion in the GUI
      SpacePoint *origin =
         (SpacePoint*)GetConfiguredItem(theInternalCoordSystem->GetOriginName());
      origin->SetJ2000Body(earth);
      SpacePoint *j2000body =
         (SpacePoint*)GetConfiguredItem(theInternalCoordSystem->GetJ2000BodyName());
      j2000body->SetJ2000Body(earth);
      theInternalCoordSystem->SetOrigin(origin);
      theInternalCoordSystem->SetJ2000Body(j2000body);
      theInternalCoordSystem->SetSolarSystem(theSolarSystemInUse);
      theInternalCoordSystem->Initialize();
      
      InitializePlanetarySource();
      InitializePlanetaryCoeffFile();
      InitializeTimeFile();
      
      if (fromGui)
      {
         CreateDefaultMission();
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::WARNING_,
                                     "Error occured during initialization: " +
                                     e.GetMessage());
      return false;
   }
   catch (...)
   {
      MessageInterface::PopupMessage(Gmat::WARNING_,
                                     "Unknown Error occured during initialization");
      return false;
   }
   
   MessageInterface::ShowMessage("Moderator successfully created core engine\n");
   return true;;
}


//------------------------------------------------------------------------------
// void Finalize()
//------------------------------------------------------------------------------
/*
 * Fininalizes the system by closing all opened files by deleting objects.
 */
//------------------------------------------------------------------------------
void Moderator::Finalize()
{
   MessageInterface::ShowMessage("Moderator is deleting core engine...\n");
   
   #if DEBUG_FINALIZE
   MessageInterface::ShowMessage("deleting files\n");
   #endif
   
   delete theFileManager;
   delete theDefaultSlpFile;
   delete theDefaultDeFile;
   delete theEopFile;
   delete theItrfFile;
   delete theLeapSecsFile;
   
   //clear resource and command sequence
   ClearResource();
   ClearCommandSeq();
   
   //MessageInterface::ShowMessage("deleting factories\n");
   //delete theStopConditionFactory;
   //delete theCalculatedPointFactory;
   //delete theFunctionFactory;
   //delete theAtmosphereFactory;
   //delete theHardwareFactory;
   //delete theSpacecraftFactory;
   
   //MessageInterface::ShowMessage("deleting theCommandFactory\n");
   //if (theCommandFactory)
   //   delete theCommandFactory;
   //MessageInterface::ShowMessage("deleting theSubscriberFactory\n");
   //if (theSubscriberFactory)
   //   delete theSubscriberFactory;
   //MessageInterface::ShowMessage("deleting theForceModelFactory\n");
   //if (theForceModelFactory)
   //   delete theForceModelFactory;
   //MessageInterface::ShowMessage("deleting theParameterFactory\n");
   //if (theParameterFactory)
   //   delete theParameterFactory;
   //MessageInterface::ShowMessage("deleting thePhysicalModelFactory\n");
   //if (thePhysicalModelFactory)
   //   delete thePhysicalModelFactory;
   //MessageInterface::ShowMessage("deleting thePropSetupFactory\n");
   //if (thePropSetupFactory)
   //   delete thePropSetupFactory;
   //MessageInterface::ShowMessage("deleting thePropagatorFactory\n");
   //if (thePropagatorFactory)
   //   delete thePropagatorFactory;
   //MessageInterface::ShowMessage("deleting theAxisSystemFactory\n");
   //if (theAxisSystemFactory)
   //   delete theAxisSystemFactory;
   //MessageInterface::ShowMessage("deleting theCoordinateSystemFactory\n");
   //if (theCoordinateSystemFactory)
   //   delete theCoordinateSystemFactory;

   //MessageInterface::ShowMessage("deleting theGuiInterpreter");
   //if (theConfigManager)
   //   delete theConfigManager;
   //std::cout << "deleting theFactoryManager" << std::endl;
   //if (theFactoryManager)
   //   delete theFactoryManager;
   //MessageInterface::ShowMessage("deleting theGuiInterpreter");
   //if (theGuiInterpreter)
   //   delete theGuiInterpreter;
   //if (theScriptInterpreter)
   //   delete theScriptInterpreter;
   //if (thePublisher)
   //   delete thePublisher;
   
   //MessageInterface::ShowMessage("deleting internal objects\n");
   //delete theDefaultSolarSystem;
   //delete theInternalCoordSystem;
   
   //if (instance)
   //   delete instance;
   
   #if DEBUG_FINALIZE
   MessageInterface::ShowMessage("Moderator::Finalize() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void SetRunReady(bool flag = false)
//------------------------------------------------------------------------------
void Moderator::SetRunReady(bool flag)
{
   isRunReady = flag;
}

//----- ObjectType
//------------------------------------------------------------------------------
// std::string GetObjectTypeString(Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Returns object type name of given object type.
 *
 * @param <type> object type
 *
 * @return object type name
 */
//------------------------------------------------------------------------------
std::string Moderator::GetObjectTypeString(Gmat::ObjectType type)
{
   if (type >= Gmat::SPACECRAFT && type <= Gmat::PROP_SETUP)
      return GmatBase::OBJECT_TYPE_STRING[type - Gmat::SPACECRAFT];
   else
      return "UnknownObject";
}

//----- interpreter
//------------------------------------------------------------------------------
// GuiInterpreter* GetGuiInterpreter()
//------------------------------------------------------------------------------
/**
 * Returns GuiInterpreter pointer.
 *
 * @return GuiInterpreter pointer
 */
//------------------------------------------------------------------------------
GuiInterpreter* Moderator::GetGuiInterpreter()
{
   return theGuiInterpreter;
}

//------------------------------------------------------------------------------
// ScriptInterpreter* GetScriptInterpreter()
//------------------------------------------------------------------------------
/**
 * Returns ScriptInterpreter pointer.
 *
 * @return ScriptInterpreter pointer
 */
//------------------------------------------------------------------------------
ScriptInterpreter* Moderator::GetScriptInterpreter()
{
   return theScriptInterpreter;
}

//------------------------------------------------------------------------------
// void SetGuiInterpreter(GuiInterpreter *guiInterp)
//------------------------------------------------------------------------------
/**
 * Sets GuiInterpreter pointer.
 */
//------------------------------------------------------------------------------
void Moderator::SetGuiInterpreter(GuiInterpreter *guiInterp)
{
   //loj: allow setting only for the first time?
   if (theGuiInterpreter == NULL)
      theGuiInterpreter = guiInterp;
}

//------------------------------------------------------------------------------
// void SetScriptInterpreter(ScriptInterpreter *scriptInterp)
//------------------------------------------------------------------------------
/**
 * Sets ScriptInterpreter pointer.
 */
//------------------------------------------------------------------------------
void Moderator::SetScriptInterpreter(ScriptInterpreter *scriptInterp)
{
   //loj: allow setting only for the first time?
   if (theScriptInterpreter == NULL)
      theScriptInterpreter = scriptInterp;
}

//----- factory
//------------------------------------------------------------------------------
// StringArray GetListOfFactoryItems(Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Returns names of all configurable items of object type.
 *
 * @param <type> object type
 *
 * @return array of configurable item names; return empty array if none
 */
//------------------------------------------------------------------------------
StringArray Moderator::GetListOfFactoryItems(Gmat::ObjectType type)
{
   return theFactoryManager->GetListOfItems(type);
}

//----- configuration
//------------------------------------------------------------------------------
// StringArray& GetListOfConfiguredItems(Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Returns names of all configured items of object type.
 *
 * @param <type> object type
 *
 * @return array of configured item names; return empty array if none
 */
//------------------------------------------------------------------------------
StringArray& Moderator::GetListOfConfiguredItems(Gmat::ObjectType type)
{
   if (type == Gmat::CELESTIAL_BODY || type == Gmat::SPACE_POINT)
   {
      theSpacePointList.clear();
      
      if (type == Gmat::CELESTIAL_BODY)
      {
         // add bodies to the list
         theSpacePointList = theSolarSystemInUse->GetBodiesInUse();
      }
      else if (type == Gmat::SPACE_POINT)
      {
         // add Spacecraft to the list
         theSpacePointList = theConfigManager->GetListOfItems(Gmat::SPACECRAFT);
         
         // add bodies to the list
         StringArray bodyList = theSolarSystemInUse->GetBodiesInUse();
         for (UnsignedInt i=0; i<bodyList.size(); i++)
            theSpacePointList.push_back(bodyList[i]);
         
         // add CalculatedPoint to the list
         StringArray calptList =
            theConfigManager->GetListOfItems(Gmat::CALCULATED_POINT);
         for (UnsignedInt i=0; i<calptList.size(); i++)
            theSpacePointList.push_back(calptList[i]);
         
      }
      
      return theSpacePointList;
   }
   
   return theConfigManager->GetListOfItems(type);
}


//------------------------------------------------------------------------------
// GmatBase* GetConfiguredItem(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* Moderator::GetConfiguredItem(const std::string &name)
{
   #if DEBUG_CONFIG
   MessageInterface::ShowMessage("Moderator::GetConfiguredItem() entered: "
                                 "name = " + name + "\n");
   #endif

   GmatBase *obj = theConfigManager->GetItem(name);

   if (obj == NULL)
   {
      // try SolarSystem
      return theSolarSystemInUse->GetBody(name);
   }
   
   return obj;
}


//------------------------------------------------------------------------------
// bool RenameConfiguredItem(Gmat::ObjectType type, const std::string &oldName
//                           const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames configured item
 *
 * @param <type> object type
 * @param <oldName>  old object name
 * @param <newName>  new object name
 *
 * @return true if the item has been removed; false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::RenameConfiguredItem(Gmat::ObjectType type, const std::string &oldName,
                                     const std::string &newName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Moderator::RenameConfiguredItem() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   bool renamed = theConfigManager->RenameItem(type, oldName, newName);
   
   //--------------------------------------------------
   // rename object name used in mission sequence
   //--------------------------------------------------
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Moderator::RenameConfiguredItem() ===> Change Command ref object names\n");
   #endif
   
   int sandboxIndex = 0; //handles one sandbox for now
   GmatCommand *cmd = commands[sandboxIndex]->GetNext();
   GmatCommand *child;
   std::string typeName;
   
   while (renamed && cmd != NULL)
   {
      typeName = cmd->GetTypeName();
      #if DEBUG_RENAME
      MessageInterface::ShowMessage("--typeName=%s\n", typeName.c_str());
      #endif
      
      renamed = cmd->RenameRefObject(type, oldName, newName);
      child = cmd->GetChildCommand(0);

      while (renamed && (child != NULL) && (child != cmd))
      {
         typeName = child->GetTypeName();
         #if DEBUG_RENAME
         MessageInterface::ShowMessage("----typeName=%s\n", typeName.c_str());
         #endif
         if (typeName.find("End") == typeName.npos)
            renamed = child->RenameRefObject(type, oldName, newName);
         
         child = child->GetNext();
      }
      
      cmd = cmd->GetNext();
   }
   
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Moderator::RenameConfiguredItem() rename status=%d\n", renamed);
   #endif

   return renamed;
}

//------------------------------------------------------------------------------
// bool RemoveConfiguredItem(Gmat::ObjectType type, const std::string &name,
//                           bool delIfNotUsed)
//------------------------------------------------------------------------------
/**
 * Removes item from the configured list.
 *
 * @param <type> object type
 * @param <name> object name
 * @param <delIfNotUse> flag indicating delete if item is not used in the mission
 *                      sequence
 *
 * @return true if the item has been removed; false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::RemoveConfiguredItem(Gmat::ObjectType type, const std::string &name,
                                     bool delOnlyIfNotUsed)
{
   if (!delOnlyIfNotUsed)
   {
      return theConfigManager->RemoveItem(type, name);
   }
   else
   {
      bool nameUsed = false;
      
      //--------------------------------------------------------------
      // check configure items
      //--------------------------------------------------------------
      
      StringArray itemList = theConfigManager->GetListOfItemsHas(type, name, false);
      
      if (itemList.size() > 0)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** Cannot remove %s. The first object it is used: %s\n",
             name.c_str(), itemList[0].c_str());
         
         return false;
      }
      
      //--------------------------------------------------------------
      // check mission sequence
      //--------------------------------------------------------------
      
      std::string::size_type pos;
      int sandboxIndex = 0; //handles one sandbox for now
      GmatCommand *cmd = commands[sandboxIndex]->GetNext();
      GmatCommand *child;
      std::string cmdString;
      std::string typeName;
      
      while (cmd != NULL)
      {
         typeName = cmd->GetTypeName();
         #if DEBUG_REMOVE
         MessageInterface::ShowMessage("--typeName=%s\n", typeName.c_str());
         #endif
         
         cmdString = cmd->GetGeneratingString();
         pos = cmdString.find(name);
         if (pos != cmdString.npos)
         {
            nameUsed = true;
            break;
         }
         
         child = cmd->GetChildCommand(0);
         
         while ((child != NULL) && (child != cmd))
         {
            typeName = child->GetTypeName();
            #if DEBUG_REMOVE
            MessageInterface::ShowMessage("----typeName=%s\n", typeName.c_str());
            #endif
            if (typeName.find("End") == typeName.npos)
            {
               cmdString = child->GetGeneratingString();
               pos = cmdString.find(name);
               if (pos != cmdString.npos)
               {
                  nameUsed = true;
                  break;
               }
            }
            
            child = child->GetNext();
         }
         
         cmd = cmd->GetNext();
      } // end While
      
      if (nameUsed)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** Cannot remove %s. It is used in the %s\n",
             name.c_str(), cmd->GetTypeName().c_str());
         return false;
      }
      else
      {
         return theConfigManager->RemoveItem(type, name);
      }
   }
}


//------------------------------------------------------------------------------
// bool HasConfigurationChanged(Integer sandboxNum = 1)
//------------------------------------------------------------------------------
bool Moderator::HasConfigurationChanged(Integer sandboxNum)
{
   bool rsrcChanged = theConfigManager->HasConfigurationChanged();
   bool cmdsChanged = commands[sandboxNum-1]->HasConfigurationChanged();
   
   #if DEBUG_CONFIG
   MessageInterface::ShowMessage
      ("Moderator::HasConfigurationChanged() rsrcChanged=%d, "
       "cmdsChanged=%d\n", rsrcChanged, cmdsChanged);
   #endif
   
   return (rsrcChanged || cmdsChanged);
}


//------------------------------------------------------------------------------
// void ConfigurationChanged(GmatBase *obj, bool tf)
//------------------------------------------------------------------------------
void Moderator::ConfigurationChanged(GmatBase *obj, bool tf)
{
   //loj: 1/17/06 debug
   #if DEBUG_CONFIG
   MessageInterface::ShowMessage("Moderator::ConfigurationChanged() called\n");
   #endif
   
   if (obj != NULL)
   {
      if (obj->IsOfType(Gmat::COMMAND))
         ((GmatCommand*)obj)->ConfigurationChanged(true);
      else
         theConfigManager->ConfigurationChanged(true);
   }
}


//------------------------------------------------------------------------------
// void ResetConfigurationChanged(bool resetResource = true,
//                                bool resetCommands = true,
//                                Integer sandboxNum = 1)
//------------------------------------------------------------------------------
void Moderator::ResetConfigurationChanged(bool resetResource, bool resetCommands,
                                          Integer sandboxNum)
{
   #if DEBUG_CONFIG
   MessageInterface::ShowMessage
      ("Moderator::ResetConfigurationChanged() entered\n");
   #endif
   
   if (resetResource)
      theConfigManager->ConfigurationChanged(false);
   
   if (resetCommands)
      SetCommandsUnchanged(sandboxNum-1);
}


// SolarSystem
//------------------------------------------------------------------------------
// SolarSystem* GetDefaultSolarSystem()
//------------------------------------------------------------------------------
/**
 * Retrieves a default solar system object pointer.
 *
 * @return a default solar system object pointer
 */
//------------------------------------------------------------------------------
SolarSystem* Moderator::GetDefaultSolarSystem()
{
   return theConfigManager->GetDefaultSolarSystem();
   //loj: 2/8/06 return theDefaultSolarSystem;
}


//------------------------------------------------------------------------------
// SolarSystem* CreateSolarSystem(const std::string &name)
//------------------------------------------------------------------------------
SolarSystem* Moderator::CreateSolarSystem(const std::string &name)
{
   // comment this out until solar system can be managed by name
   //SolarSystem *solarSys = theFactoryManager->CreateSolarSystem(name);
   //theConfigManager->AddSolarSystem(solarSys);
   //return solarSys;
   throw GmatBaseException
      ("Moderator::CreateSolarSystem() has not been implemented\n");
}


//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystemInUse()
//------------------------------------------------------------------------------
SolarSystem* Moderator::GetSolarSystemInUse()
{
   return theConfigManager->GetSolarSystemInUse();
   //loj: 2/8/06 return theDefaultSolarSystem;
}


//------------------------------------------------------------------------------
// void SetSolarSystemInUse(SolarSystem *ss)
//------------------------------------------------------------------------------
void Moderator::SetSolarSystemInUse(SolarSystem *ss)
{
   if (ss != NULL)
      theConfigManager->SetSolarSystemInUse(ss);
   else
      throw GmatBaseException
         ("Moderator::SetSolarSystemInUse() cannot set NULL SolarSystem\n");
}


//------------------------------------------------------------------------------
// bool SetSolarSystemInUse(const std::string &name)
//------------------------------------------------------------------------------
bool Moderator::SetSolarSystemInUse(const std::string &name)
{
   return theConfigManager->SetSolarSystemInUse(name);
}


// CalculatedPoint
//------------------------------------------------------------------------------
// CalculatedPoint* CreateCalculatedPoint(const std::string &type,
//                                        const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a calculated point object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a CalculatedPoint object pointer
 */
//------------------------------------------------------------------------------
CalculatedPoint* Moderator::CreateCalculatedPoint(const std::string &type,
                                                  const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateCalculatedPoint() type = %s, name = %s\n",
       type.c_str(), name.c_str());
   #endif
   
   if (GetCalculatedPoint(name) == NULL)
   {
      CalculatedPoint *cp = theFactoryManager->CreateCalculatedPoint(type, name);
      
      if (cp == NULL)
      {
         //MessageInterface::PopupMessage
         //   (Gmat::ERROR_, "Cannot create a CreateCalculatedPoint type: %s.\n"
         //    "Make sure %s is correct type and registered to CalculatedPointFactory.\n",
         //    type.c_str(), type.c_str());
         
         //return NULL;
         throw GmatBaseException("Error Creating CalculatedPoint: " + type + "\n");
      }
      
      // add default bodies
      if (type == "LibrationPoint")
      {
         // first create default Earth-Moon Barycenter
         CalculatedPoint *defBc = GetCalculatedPoint("DefaultBC");
         
         if (defBc == NULL)
         {
            defBc = CreateCalculatedPoint("Barycenter", "DefaultBC");
         }
         
         cp->SetStringParameter("Primary", "Sun");
         cp->SetStringParameter("Point", "L1");
         cp->SetStringParameter("Secondary", "DefaultBC");
         
         // Set body and J2000Body pointer, so that GUI can create LibrationPoint
         // and use it in Coord.System conversion
         SpacePoint *sun = (SpacePoint*)GetConfiguredItem("Sun");
         SpacePoint *earth = (SpacePoint*)GetConfiguredItem("Earth");
         if (sun->GetJ2000Body() == NULL)
            sun->SetJ2000Body(earth);
         
         cp->SetRefObject(sun, Gmat::SPACE_POINT, "Sun");
         cp->SetRefObject(defBc, Gmat::SPACE_POINT, "DefaultBC");         
      }
      else if (type == "Barycenter")
      {
         cp->SetStringParameter("BodyNames", "Earth");
         cp->SetStringParameter("BodyNames", "Luna");

         // Set body and J2000Body pointer, so that GUI can create LibrationPoint
         // and use it in Coord.System conversion
         SpacePoint *earth = (SpacePoint*)GetConfiguredItem("Earth");
         SpacePoint *luna = (SpacePoint*)GetConfiguredItem("Luna");
         cp->SetRefObject(earth, Gmat::SPACE_POINT, "Earth");
         if (luna->GetJ2000Body() == NULL)
            luna->SetJ2000Body(earth);
         cp->SetRefObject(luna, Gmat::SPACE_POINT, "Luna");
      }
      
      // Manage it if it is a named calculated point
      try
      {
         if (cp->GetName() != "")
            theConfigManager->AddCalculatedPoint(cp);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateCalculatedPoint()\n" +
                                       e.GetMessage());
      }
    
      return cp;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateCalculatedPoint() Unable to create CalculatedPoint "
          "name: %s already exist\n", name.c_str());
      #endif
      return GetCalculatedPoint(name);
   }
}


//------------------------------------------------------------------------------
// CalculatedPoint* GetCalculatedPoint(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a calclulated point object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a CalculatedPoint object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
CalculatedPoint* Moderator::GetCalculatedPoint(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetCalculatedPoint(name);
}


// CelestialBody
//------------------------------------------------------------------------------
// CelestialBody* CreateCelestialBody(const std::string &type,
//                                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a celestial body object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a CelestialBody object pointer
 */
//------------------------------------------------------------------------------
CelestialBody* Moderator::CreateCelestialBody(const std::string &type,
                                              const std::string &name)
{
   if (GetCelestialBody(name) == NULL)
   {
      CelestialBody *body = theFactoryManager->CreateCelestialBody(type, name);
    
      if (body == NULL)
      {
         //MessageInterface::ShowMessage
         //   ("Moderator::CreateCelestialBody() Error Creating %s.  Make sure "
         //    "CelestialBodyFactory is registered and has correct type. \n",
         //    type.c_str());
         
         //return NULL;
         throw GmatBaseException("Error Creating CelestialBody: " + type + "\n");
      }
      
      // Manage it if it is a named body
      //loj: Do we want to add bodies that are not in the default solar system?
      try
      {
         if (body->GetName() != "")
            theConfigManager->AddCelestialBody(body);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateCelestialBody()\n" +
                                       e.GetMessage());
      }
    
      return body;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateCelestialBody() Unable to create CelestialBody "
          "name: %s already exist\n", name.c_str());
      #endif
      return GetCelestialBody(name);
   }
}


//------------------------------------------------------------------------------
// CelestialBody* GetCelestialBody(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a celestial body object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a CelestialBody object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
CelestialBody* Moderator::GetCelestialBody(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetCelestialBody(name);
}


// Spacecraft
//------------------------------------------------------------------------------
// SpaceObject* CreateSpacecraft(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a spacecraft object by given name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return spacecraft object pointer
 */
//------------------------------------------------------------------------------
SpaceObject* Moderator::CreateSpacecraft(const std::string &type,
                                         const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateSpacecraft() type = %s, name = %s\n",
       type.c_str(), name.c_str());
   #endif
   
   if (GetSpacecraft(name) == NULL)
   {
      //SpaceObject *sc = theFactoryManager->CreateSpacecraft(type, name);
      Spacecraft *sc = (Spacecraft*)(theFactoryManager->CreateSpacecraft(type, name));

      if (sc == NULL)
      {
         //MessageInterface::PopupMessage
         //   (Gmat::ERROR_, "Cannot create a Spacecraft type: %s.\n"
         //    "Make sure %s is correct type and registered to SpacecraftFactory.\n",
         //    type.c_str(), type.c_str());
         
         //return NULL;
         throw GmatBaseException("Error Creating Spacecraft: " + type + "\n");
      }
      
      if (type == "Spacecraft")
      {
         // Set internal and default CoordinateSystem
         sc->SetInternalCoordSystem(theInternalCoordSystem);
         sc->SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
      }
      
      // Manage it if it is a named Spacecraft
      try
      {
         if (sc->GetName() != "")
            theConfigManager->AddSpacecraft(sc);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateSpacecraft()\n" +
                                       e.GetMessage());
      }

      return sc;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateSpacecraft() Unable to create Spacecraft "
          "name: %s already exist\n", name.c_str());
      #endif
      return GetSpacecraft(name);
   }
}

//------------------------------------------------------------------------------
// SpaceObject* GetSpacecraft(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a spacecraft object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a SpaceObject object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
SpaceObject* Moderator::GetSpacecraft(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetSpacecraft(name);
}


// Hardware
//------------------------------------------------------------------------------
// Hardware* CreateHardware(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a Hardware object by given name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return Hardware object pointer
 */
//------------------------------------------------------------------------------
Hardware* Moderator::CreateHardware(const std::string &type, const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateHardware() type = %s, name = %s\n",
       type.c_str(), name.c_str());
   #endif
   
   if (GetHardware(name) == NULL)
   {
      Hardware *hw = theFactoryManager->CreateHardware(type, name);
      
      if (hw == NULL)
      {
         //MessageInterface::PopupMessage
         //   (Gmat::ERROR_, "Cannot create a Hardware type: %s.\n"
         //    "Make sure %s is correct type and registered to HardwareFactory.\n",
         //    type.c_str(), type.c_str());
         
         //return NULL;
         throw GmatBaseException("Error Creating Hardware: " + type + "\n");
      }
      
      // Manage it if it is a named Hardware
      try
      {
         if (hw->GetName() != "")
            theConfigManager->AddHardware(hw);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateHardware()\n" +
                                       e.GetMessage());
      }
      
      return hw;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateHardware() Unable to create Hardware "
          "name: \"%s\" already exists\n", name.c_str());
      #endif
      return GetHardware(name);
   }
}


//------------------------------------------------------------------------------
// Hardware* GetHardware(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Hardware object pointer by given name and add to configuration.
 *
 * @param <name> object name
 *
 * @return a Hardware object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Hardware* Moderator::GetHardware(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetHardware(name);
}


// Propagator
//------------------------------------------------------------------------------
// Propagator* CreatePropagator(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a propagator object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a propagator object pointer
 */
//------------------------------------------------------------------------------
Propagator* Moderator::CreatePropagator(const std::string &type,
                                        const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreatePropagator() type = %s, name = %s\n",
       type.c_str(), name.c_str());
   #endif
   
   if (GetPropagator(name) == NULL)
   {
      Propagator *prop = theFactoryManager->CreatePropagator(type, name);
    
      if (prop ==  NULL)
      {
         //MessageInterface::PopupMessage
         //   (Gmat::ERROR_, "Cannot create a Propagator type: %s.\n"
         //    "Make sure %s is correct type and registered to PropagatorFactory.\n",
         //   type.c_str(), type.c_str());
         
         //return NULL;
         throw GmatBaseException("Error Creating Propagator: " + type + "\n");
      }
      
      // Manage it if it is a named Propagator
      try
      {
         if (prop->GetName() != "")
            theConfigManager->AddPropagator(prop);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreatePropagator()\n" +
                                       e.GetMessage());
      }
        
      return prop;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreatePropagator() Unable to create Propagator "
          "name: %s already exist\n", name.c_str());
      #endif
      return GetPropagator(name);
   }
}

//------------------------------------------------------------------------------
// Propagator* GetPropagator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a propagator object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a propagator object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Propagator* Moderator::GetPropagator(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetPropagator(name);
}

// PhysicalModel
//------------------------------------------------------------------------------
// PhysicalModel* CreatePhysicalModel(const std::string &type,
//                                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a physical model object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a physical model object pointer
 */
//------------------------------------------------------------------------------
PhysicalModel* Moderator::CreatePhysicalModel(const std::string &type,
                                              const std::string &name)
{
   if (GetPhysicalModel(name) == NULL)
   {
      PhysicalModel *physicalModel =
         theFactoryManager->CreatePhysicalModel(type, name);
    
      if (physicalModel ==  NULL)
      {
         //MessageInterface::PopupMessage
         //   (Gmat::ERROR_, "Cannot create a PhysicalModel type: %s.\n"
         //    "Make sure %s is correct type and registered to PhysicalModelFactory.\n",
         //    type.c_str(), type.c_str());
         
         //return NULL;         
         throw GmatBaseException("Error Creating PhysicalModel: " + type + "\n");
      }
      
      // Manage it if it is a named PhysicalModel
      try
      {
         if (physicalModel->GetName() != "")
            theConfigManager->AddPhysicalModel(physicalModel);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreatePhysicalModel()\n" +
                                       e.GetMessage());
      }
        
      return physicalModel;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreatePhysicalModel() Unable to create PhysicalModel "
          "name: %s already exist\n", name.c_str());
      #endif
      return GetPhysicalModel(name);
   }
}

//------------------------------------------------------------------------------
// PhysicalModel* GetPhysicalModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a physical model object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a physical model object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
PhysicalModel* Moderator::GetPhysicalModel(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetPhysicalModel(name);
}

// AtmosphereModel
//------------------------------------------------------------------------------
// AtmosphereModel* CreateAtmosphereModel(const std::string &type,
//                                        const std::string &name,
//                                        const std::string &body = "Earth")
//------------------------------------------------------------------------------
/**
 * Creates an atmosphere model object by given type and name and add to
 * configuration.
 *
 * @param <type> object type
 * @param <name> object name
 * @param <body> the body for which the atmosphere model is requested
 *
 * @return a atmosphereModel object pointer
 */
//------------------------------------------------------------------------------
AtmosphereModel* Moderator::CreateAtmosphereModel(const std::string &type,
                                                  const std::string &name,
                                                  const std::string &body)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateAtmosphereModel() entered: type = " +
       type + ", name = " + name + "\n");
   #endif

   // if AtmosphereModel name doesn't exist, create AtmosphereModel
   if (GetAtmosphereModel(name) == NULL)
   {
      AtmosphereModel *atmosphereModel =
         theFactoryManager->CreateAtmosphereModel(type, body, name);

      if (atmosphereModel ==  NULL)
      {
         //MessageInterface::PopupMessage
         //   (Gmat::ERROR_, "Cannot create a AtmosphereModel type: %s.\n"
         //    "Make sure %s is correct type and registered to AtmosphereFactory.\n",
         //    type.c_str(), type.c_str());
         
         //return NULL;
         throw GmatBaseException("Error Creating AtmosphereModel: " + type + "\n");
      }
      
      // Manage it if it is a named AtmosphereModel
      try
      {
         if (atmosphereModel->GetName() != "")
            theConfigManager->AddAtmosphereModel(atmosphereModel);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage
            ("Moderator::CreateAtmosphereModel()\n" + e.GetMessage());
      }
    
      return atmosphereModel;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateAtmosphereModel() Unable to create AtmosphereModel "
          "name: %s already exist\n", name.c_str());
      #endif
      return GetAtmosphereModel(name);
   }
}

//------------------------------------------------------------------------------
// AtmosphereModel* GetAtmosphereModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves an atmosphere model object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a AtmosphereModel pointer, return null if name not found
 */
//------------------------------------------------------------------------------
AtmosphereModel* Moderator::GetAtmosphereModel(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetAtmosphereModel(name);
}

// Burn
//------------------------------------------------------------------------------
// Burn* CreateBurn(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a burn object by given type and name and add to configuration.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a burn object pointer
 */
//------------------------------------------------------------------------------
Burn* Moderator::CreateBurn(const std::string &type,
                            const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("Moderator::CreateBurn() entered: type = " +
                                 type + ", name = " + name + "\n");
   #endif

   // if Burn name doesn't exist, create Burn
   if (GetBurn(name) == NULL)
   {
      Burn *burn = theFactoryManager->CreateBurn(type, name);

      if (burn ==  NULL)
      {
         //MessageInterface::PopupMessage
         //   (Gmat::ERROR_, "Cannot create a Burn type: %s.\n"
         //    "Make sure %s is correct type and registered to BurnFactory.\n",
         //    type.c_str(), type.c_str());
         
         //return NULL;
         throw GmatBaseException("Error Creating Burn: " + type + "\n");
      }
    
      // Manage it if it is a named burn
      try
      {
         if (burn->GetName() != "")
            theConfigManager->AddBurn(burn);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateBurn()\n" +
                                       e.GetMessage());
      }
    
      return burn;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateBurn() Unable to create Burn "
          "name: %s already exist\n", name.c_str());
      #endif
      return GetBurn(name);
   }
}

//------------------------------------------------------------------------------
// Burn* GetBurn(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a burn object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a burn pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Burn* Moderator::GetBurn(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetBurn(name);
}

// Parameter
//------------------------------------------------------------------------------
// bool Moderator::IsParameter(const std::string &type)
//------------------------------------------------------------------------------
/**
 * Checks to see if a given type is a Parameter.
 *
 * @param <type> object type
 *
 * @return true if the type is a registered parameter, false if not.
 */
//------------------------------------------------------------------------------
bool Moderator::IsParameter(const std::string &type)
{
   StringArray sar = theFactoryManager->GetListOfItems(Gmat::PARAMETER);
      
   if (find(sar.begin(), sar.end(), type) != sar.end()) {
      #ifdef DEBUG_LOOKUP_RESOURCE
      MessageInterface::ShowMessage("Found parameter \"%s\"\n", type.c_str());
      #endif
      return true;
   }

   #ifdef DEBUG_LOOKUP_RESOURCE
   MessageInterface::ShowMessage("Could not find parameter \"%s\"\n",
                                 type.c_str());
   #endif

   return false;   
}


//------------------------------------------------------------------------------
// Parameter* CreateParameter(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a parameter object by given type and name and add to configuration.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a parameter object pointer
 */
//------------------------------------------------------------------------------
Parameter* Moderator::CreateParameter(const std::string &type,
                                      const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("Moderator::CreateParameter() type = %s, "
                                 "name = %s\n", type.c_str(), name.c_str());
   #endif
   
   // if Parameter name doesn't exist, create Parameter
   if (GetParameter(name) == NULL)
   {
      Parameter *parameter = theFactoryManager->CreateParameter(type, name);

      // create parameter dependent coordinate system (loj: 1/27/06)
      if (type == "Longitude" || type == "Latitude" || type == "Altitude" ||
          type == "MHA" || type == "LST")
      {
         // need body-fixed CS
         StringTokenizer st(name, ".");
         StringArray tokens = st.GetAllTokens();
         
         if (tokens.size() == 2 || (tokens.size() == 3 && tokens[1] == "Earth"))
         {
            //MessageInterface::ShowMessage("==> Create EarthFixed\n");
            
            // default EarthFixed
            CreateCoordinateSystem("EarthFixed");
            parameter->SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthFixed");
         }
         else if (tokens.size() == 3)
         {
            std::string origin = tokens[1];
            std::string axisName = origin + "Fixed";
            
            //MessageInterface::ShowMessage("==> Create %s\n", axisName.c_str());
            
            CoordinateSystem *cs = CreateCoordinateSystem(axisName);
            
            // create BodyFixedAxis with origin
            AxisSystem *axis = CreateAxisSystem("BodyFixed", "");
            cs->SetStringParameter("Origin", origin);
            cs->SetRefObject(GetConfiguredItem(origin), Gmat::SPACE_POINT, origin);
            cs->SetRefObject(axis, Gmat::AXIS_SYSTEM, axis->GetName());
            //cs->SetStringParameter("J2000Body", "Earth"); // Default is Earth
            cs->SetRefObject(GetConfiguredItem("Earth"), Gmat::SPACE_POINT, "Earth");
            cs->SetSolarSystem(theSolarSystemInUse);
            cs->Initialize();
            
            parameter->SetRefObjectName(Gmat::COORDINATE_SYSTEM, axisName);
         }
         else
         {
            MessageInterface::ShowMessage("===> Invalid parameter name: %s\n",
                                          name.c_str());
         }
      }
      
      if (parameter == NULL)
      {
        throw GmatBaseException("Error Creating Parameter: " + type + "\n");
      }
      
      // Manage it if it is a named parameter
      try
      {
         if (parameter->GetName() != "")
            theConfigManager->AddParameter(parameter);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateParameter()\n" +
                                       e.GetMessage());
      }
      
      return parameter;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateParameter() Unable to create Parameter "
          "name: %s already exist\n", name.c_str());
      #endif
      return GetParameter(name);
   }
}

//------------------------------------------------------------------------------
// Parameter* GetParameter(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a parameter object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a parameter object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Parameter* Moderator::GetParameter(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetParameter(name);
}

// ForceModel
//------------------------------------------------------------------------------
// ForceModel* CreateForceModel(const std::string &name)
//------------------------------------------------------------------------------
ForceModel* Moderator::CreateForceModel(const std::string &name)
{
   ForceModel *fm = theFactoryManager->CreateForceModel(name);
   
   if (fm == NULL)
   {
      //MessageInterface::PopupMessage
      //   (Gmat::ERROR_, "Cannot create a ForceModel.\n"
      //    "Make sure ForceModel is correct type and registered to ForceModelFactory.\n");
      
      //return NULL;
      throw GmatBaseException("Error Creating ForceModel\n");
   }
   
   // Manage it if it is a named ForceModel
   try
   {
      if (fm->GetName() != "")
         theConfigManager->AddForceModel(fm);
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage("Moderator::CreateForceModel()\n" +
                                    e.GetMessage() + "\n");
   }
   
   return fm;
}

//------------------------------------------------------------------------------
// ForceModel* GetForceModel(const std::string &name)
//------------------------------------------------------------------------------
ForceModel* Moderator::GetForceModel(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetForceModel(name);
}

//------------------------------------------------------------------------------
// bool AddToForceModel(const std::string &forceModelName,
//                      const std::string &forceName)
//------------------------------------------------------------------------------
bool Moderator::AddToForceModel(const std::string &forceModelName,
                                const std::string &forceName)
{
   bool status = true;
   ForceModel *fm = theConfigManager->GetForceModel(forceModelName);
   PhysicalModel *physicalModel = theConfigManager->GetPhysicalModel(forceName);
   fm->AddForce(physicalModel);
   return status;
}

// StopCondition
//------------------------------------------------------------------------------
// StopCondition* CreateStopCondition(const std::string &type,
//                                    const std::string &name)
//------------------------------------------------------------------------------
StopCondition* Moderator::CreateStopCondition(const std::string &type,
                                              const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("Moderator::CreateStopCondition() type = %s, "
                                 "name = %s\n", type.c_str(), name.c_str());
   #endif
   
   StopCondition *stopCond = theFactoryManager->CreateStopCondition(type, name);
   
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateStopCondition() *stopCond type = %s\n",
       stopCond->GetTypeName().c_str());
   #endif
    
   if (stopCond ==  NULL)
   {
      //MessageInterface::PopupMessage
      //   (Gmat::ERROR_, "Cannot create a StopCondition type: %s.\n"
      //    "Make sure %s is correct type and registered to StopConditionFactory.\n",
      //    type.c_str(), type.c_str());
      
      //return NULL;
      throw GmatBaseException("Error Creating StopCondition: " + type + "\n");
   }
   
   return stopCond;
}

//------------------------------------------------------------------------------
// StopCondition* GetStopCondition(const std::string &name)
//------------------------------------------------------------------------------
StopCondition* Moderator::GetStopCondition(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetStopCondition(name);
}

// Solver
//------------------------------------------------------------------------------
// Solver* CreateSolver(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a solver object by given type and name and add to configuration.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a solver object pointer
 */
//------------------------------------------------------------------------------
Solver* Moderator::CreateSolver(const std::string &type, const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("Moderator::CreateSolver() type = %s, "
                                 "name = %s\n", type.c_str(), name.c_str());
   #endif

   if (GetSolver(name) == NULL)
   {
      Solver *solver = theFactoryManager->CreateSolver(type, name);

      if (solver == NULL)
      {
         //MessageInterface::PopupMessage
         //   (Gmat::ERROR_, "Cannot create a Solver type: %s.\n"
         //    "Make sure %s is correct type and registered to SolverFactory.\n",
         //    type.c_str(), type.c_str());
         
         //return NULL;
         throw GmatBaseException("Error Creating Solver: " + type + "\n");
      }
      
      // Manage it if it is a named solver
      try
      {
         if (solver->GetName() != "")
            theConfigManager->AddSolver(solver);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateSolver()\n" +
                                       e.GetMessage());
      }
      
      return solver;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateSolver() Unable to create Solver "
          "name: %s already exist\n", name.c_str());
      #endif
      return GetSolver(name);
   }
}


//------------------------------------------------------------------------------
// Solver* GetSolver(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a solver object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a solver object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Solver* Moderator::GetSolver(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetSolver(name);
}

// PropSetup
//------------------------------------------------------------------------------
// PropSetup* CreateDefaultPropSetup(const std::string &name)
//------------------------------------------------------------------------------
PropSetup* Moderator::CreateDefaultPropSetup(const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("====================\n");
   MessageInterface::ShowMessage("Moderator::CreateDefaultPropSetup() name=%s\n",
                                 name.c_str());
   #endif
   
   // PropSetup creates default Integrator(RungeKutta89)
   // and default force (PointMassForce body=Earth)
   PropSetup *propSetup = CreatePropSetup(name);

   ForceModel *oldfm= propSetup->GetForceModel();
   if (oldfm->GetName() == "")
      delete oldfm;
   
   // create default force model with Earth primary body
   ForceModel *newfm= CreateForceModel("");
   GravityField *gravForce = new GravityField("", "Earth");
   gravForce->SetStringParameter("PotentialFile", GetFileName("JGM2_FILE"));
   newfm->AddForce(gravForce);
   propSetup->SetForceModel(newfm);
   
   return propSetup;
}

//------------------------------------------------------------------------------
// PropSetup* CreatePropSetup(const std::string &name,
//                            const std::string &propagatorName = "",
//                            const std::string &forceModelName = "")
//------------------------------------------------------------------------------
PropSetup* Moderator::CreatePropSetup(const std::string &name,
                                      const std::string &propagatorName,
                                      const std::string &forceModelName)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("====================\n");
   MessageInterface::ShowMessage("Moderator::CreatePropSetup() name=%s\n",
                                 name.c_str());
   #endif

   if (GetPropSetup(name) == NULL)
   {
      Propagator *prop = theConfigManager->GetPropagator(propagatorName);
      ForceModel *fm = theConfigManager->GetForceModel(forceModelName);
      
      PropSetup *propSetup = theFactoryManager->CreatePropSetup(name);
      
      if (prop)
         propSetup->SetPropagator(prop);
   
      if (fm)
         propSetup->SetForceModel(fm);

      if (name != "")
         theConfigManager->AddPropSetup(propSetup);
   
      return propSetup;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreatePropSetup() Unable to create PropSetup "
          "name: %s already exist\n", name.c_str());
      #endif
      return GetPropSetup(name);
   }
}

//------------------------------------------------------------------------------
// PropSetup* GetPropSetup(const std::string &name)
//------------------------------------------------------------------------------
PropSetup* Moderator::GetPropSetup(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetPropSetup(name);
}


//------------------------------------------------------------------------------
// Interpolator* CreateInterpolator(const std::string &type,
//                                  const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a Interpolator object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a Interpolator object pointer
 */
//------------------------------------------------------------------------------
Interpolator* Moderator::CreateInterpolator(const std::string &type,
                                            const std::string &name)
{
   //loj: 3/22/04 theFactoryManager->CreateInterpolator() not implemented
   return NULL;
}

//------------------------------------------------------------------------------
// Interpolator* GetInterpolator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Interpolator object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a Interpolator object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Interpolator* Moderator::GetInterpolator(const std::string &name)
{
   return NULL;
}

// CoordinateSystem
//------------------------------------------------------------------------------
// CoordinateSystem* CreateCoordinateSystem(const std::string &name,
//                                          bool createDefault = false)
//------------------------------------------------------------------------------
CoordinateSystem* Moderator::CreateCoordinateSystem(const std::string &name,
                                                    bool createDefault)
{
   CoordinateSystem *cs = theFactoryManager->CreateCoordinateSystem(name);

   if (cs == NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Cannot create a CoordinateSystem.\n"
          "Make sure CoordinateSystem is correct type and registered to "
          "CoordinateSystemFactory.\n");
      
      return NULL;
      //throw GmatBaseException("Error Creating CoordinateSystem");
   }
   
   // Manage it if it is a named CoordinateSystem
   try
   {
      if (cs->GetName() != "")
         theConfigManager->AddCoordinateSystem(cs);
      
      if (createDefault)
      {
         // create MJ2000Eq AxisSystem with Earth as origin
         AxisSystem *axis = CreateAxisSystem("MJ2000Eq", "");
         cs->SetStringParameter("Origin", "Earth");
         cs->SetRefObject(GetConfiguredItem("Earth"), Gmat::SPACE_POINT, "Earth");
         //cs->SetStringParameter("J2000Body", "Earth"); // Default is Earth
         cs->SetRefObject(axis, Gmat::AXIS_SYSTEM, axis->GetName());
         cs->SetSolarSystem(theSolarSystemInUse);
         cs->Initialize();
      }
   }
   catch (BaseException &e)
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage("Moderator::CreateCoordinateSystem() %s\n",
                                    e.GetMessage().c_str());
      #endif
   }
   
   return cs;
}


//------------------------------------------------------------------------------
// CoordinateSystem* GetCoordinateSystem(const std::string &name)
//------------------------------------------------------------------------------
CoordinateSystem* Moderator::GetCoordinateSystem(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetCoordinateSystem(name);
}


//------------------------------------------------------------------------------
// AxisSystem* CreateAxisSystem(const std::string &type,
//                              const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a AxisSystem object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a AxisSystem object pointer
 */
//------------------------------------------------------------------------------
AxisSystem* Moderator::CreateAxisSystem(const std::string &type,
                                        const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("Moderator::CreateAxisSystem() type = %s, "
                                 "name = %s\n", type.c_str(), name.c_str());
   #endif
   
   AxisSystem *axisSystem = theFactoryManager->CreateAxisSystem(type, name);
   
   if (axisSystem == NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Cannot create a AxisSystem type: %s.\n"
          "Make sure %s is correct type and registered to AxisSystemFactory.\n",
          type.c_str(), type.c_str());
      
      return NULL;
      //throw GmatBaseException("Error Creating AxisSystem: " + type);
   }

   // set origin and j2000body
   axisSystem->SetOrigin((SpacePoint*)GetConfiguredItem(axisSystem->GetOriginName()));
   axisSystem->SetJ2000Body((SpacePoint*)GetConfiguredItem(axisSystem->GetJ2000BodyName()));
   
   // Notes: AxisSystem is not configured. It is local to CoordinateSystem
   // and gets deleted when CoordinateSystem is deleted.
   
   // DJC added 5/11/05.  The ScriptInterpreter does not have the parms needed
   // to set these references, so defaults are set here.  This might need to be
   // fixed later.

   /// @todo Evaluate how the AxixSystem file usage really should be set
   
   // Set required internal references if they are used
   if (axisSystem->UsesEopFile() == GmatCoordinate::REQUIRED)
      axisSystem->SetEopFile(theEopFile);
   if (axisSystem->UsesItrfFile() == GmatCoordinate::REQUIRED)
      axisSystem->SetCoefficientsFile(theItrfFile);

   return axisSystem;
}

// Subscriber
//------------------------------------------------------------------------------
// Subscriber* CreateSubscriber(const std::string &type, const std::string &name,
//                              const std::string &fileName = "",
//                              bool createDefault = false)
//------------------------------------------------------------------------------
/**
 * Creates a subscriber object by given type and name if not already created.
 *
 * @param <type> object type
 * @param <name> object name
 * @param <fileName> file name if used
 * @param <createDefalut> create default Subscriber by setting ref. object if true
 *
 * @return a subscriber object pointer
 */
//------------------------------------------------------------------------------
Subscriber* Moderator::CreateSubscriber(const std::string &type,
                                        const std::string &name,
                                        const std::string &fileName,
                                        bool createDefault)
{
   //loj: 12/16/05 added fileName
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateSubscriber() entered: type=%s, name=%s, fileName=%s\n"
       "createDefault=%d\n", type.c_str(), name.c_str(), fileName.c_str(),
       createDefault);
   #endif
   
   if (GetSubscriber(name) == NULL)
   {      
      Subscriber *sub = theFactoryManager->CreateSubscriber(type, name, fileName);
      
      if (sub == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Cannot create a Subscriber type: %s.\n"
             "Make sure %s is correct type and registered to SubscriberFactory.\n",
             type.c_str(), type.c_str());
         
         return NULL;
         //throw GmatBaseException("Error Creating Subscriber:" + type);
      }
      
      try
      {
         if (sub->GetName() != "")
            theConfigManager->AddSubscriber(sub);
      
         #if DEBUG_CREATE_RESOURCE
         MessageInterface::ShowMessage
            ("Moderator::CreateSubscriber() Creating default subscriber...\n");
         #endif
         
         if (createDefault)
         {
            if (type == "OpenGLPlot")
            {
               // add default spacecraft and coordinate system
               sub->SetStringParameter("Add", GetDefaultSpacecraft()->GetName());
               sub->SetStringParameter("CoordinateSystem", "EarthMJ2000Eq");
            }
            else if (type == "XYPlot")
            {
               // add default x,y parameter to XYPlot
               sub->SetStringParameter("IndVar", GetDefaultX()->GetName());
               sub->SetStringParameter("Add", GetDefaultY()->GetName(), 0);
               sub->Activate(true);
            }
            else if (type == "ReportFile")
            {
               // add default parameters to ReportFile
               sub->SetStringParameter(sub->GetParameterID("Filename"),
                                       name + ".txt");
               sub->SetStringParameter("Add", GetDefaultX()->GetName());
               sub->SetStringParameter("Add", GetDefaultY()->GetName());
               sub->Activate(true);
            }
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateSubscriber()\n" +
                                       e.GetMessage());
      }
   
      return sub;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateSubscriber() Unable to create Subscriber "
          "name: %s already exist\n", name.c_str());
      #endif
      
      return GetSubscriber(name);
   }
}

//------------------------------------------------------------------------------
// Subscriber* GetSubscriber(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a subscriber object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a subscriber object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Subscriber* Moderator::GetSubscriber(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetSubscriber(name);
}

// Function
//------------------------------------------------------------------------------
// Function* CreateFunction(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a function object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a Function object pointer
 */
//------------------------------------------------------------------------------
Function* Moderator::CreateFunction(const std::string &type,
                                    const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("Moderator::CreateFunction() entered: type = " +
                                 type + ", name = " + name + "\n");
   #endif
   
   if (GetFunction(name) == NULL)
   {
      Function *function = theFactoryManager->CreateFunction(type, name);
      
      if (function == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Cannot create a Function type: %s.\n"
             "Make sure %s is correct type and registered to FunctionFactory.\n",
             type.c_str(), type.c_str());
         
         return NULL;
         //throw GmatBaseException("Error Creating Function:" + type);
      }
      
      // Manage it if it is a named function
      try
      {
         if (function->GetName() != "")
            theConfigManager->AddFunction(function);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateFunction()\n" +
                                       e.GetMessage());
      }
   
      return function;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateFunction() Unable to create Function "
          "name: %s already exist\n", name.c_str());
      #endif
      
      return GetFunction(name);
   }
}

//------------------------------------------------------------------------------
// Function* GetFunction(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a function object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a Function object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Function* Moderator::GetFunction(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetFunction(name);
}


//------------------------------------------------------------------------------
// GmatCommand* InterpretGmatFunction(const std::string functionFilename)
//------------------------------------------------------------------------------
/**
 * Retrieves a function object pointer by given name.
 *
 * @param <functionFilename>  Full path and name of the GmatFunction file.
 *
 * @return A command list that is executed to run the function.
 */
//------------------------------------------------------------------------------
GmatCommand* Moderator::InterpretGmatFunction(const std::string &functionFilename)
{
   if (functionFilename == "")
      return NULL;
   else
      return theScriptInterpreter->InterpretGMATFunction(functionFilename);
}


// GmatCommand
//------------------------------------------------------------------------------
// GmatCommand* CreateCommand(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
GmatCommand* Moderator::CreateCommand(const std::string &type,
                                      const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("Moderator::CreateCommand() entered: type = " +
                                 type + ", name = " + name + "\n");
   #endif

   GmatCommand *cmd = theFactoryManager->CreateCommand(type, name);
   
   if (cmd == NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Cannot create a Command type: %s.\n"
          "Make sure %s is correct type and registered to Commandactory.\n",
          type.c_str(), type.c_str());
   }
   
   return cmd;
}


//------------------------------------------------------------------------------
// GmatCommand* CreateDefaultCommand(const std::string &type,
//                                   const std::string &name)
//------------------------------------------------------------------------------
GmatCommand* Moderator::CreateDefaultCommand(const std::string &type,
                                             const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateDefaultCommand() entered: type = " +
       type + ", name = " + name + "\n");
   #endif
   
   GmatCommand *cmd = theFactoryManager->CreateCommand(type, name);
   
   if (cmd == NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Cannot create a Command type: %s.\n"
          "Make sure %s is correct type and registered to Commandactory.\n",
          type.c_str(), type.c_str());

      return NULL;
   }
   
   Integer id;
   
   if (type == "Toggle")
   {
      cmd->SetStringParameter(cmd->GetParameterID("Subscriber"),
                              GetDefaultSubscriber()->GetName());
   }
   else if (type == "Propagate")
   {

      cmd->SetObject(GetDefaultPropSetup()->GetName(), Gmat::PROP_SETUP);

      StringArray &formList = GetListOfConfiguredItems(Gmat::FORMATION);
      
      // if formation exist, set first formation to command
      if (formList.size() > 0)
         cmd->SetObject(formList[0], Gmat::SPACECRAFT);
      else
         cmd->SetObject(GetDefaultSpacecraft()->GetName(), Gmat::SPACECRAFT);
      
      cmd->SetRefObject(CreateDefaultStopCondition(), Gmat::STOP_CONDITION, "", 0);
      cmd->SetSolarSystem(theSolarSystemInUse);
   }
   else if (type == "Maneuver")
   {
      // set burn
      id = cmd->GetParameterID("Burn");
      cmd->SetStringParameter(id, GetDefaultBurn()->GetName());
      
      // set spacecraft
      id = cmd->GetParameterID("Spacecraft");
      cmd->SetStringParameter(id, GetDefaultSpacecraft()->GetName());
   }
   else if (type == "Target")
   {
      // set solver
      Solver *solver = CreateSolver("DifferentialCorrector",
                                    GetDefaultSolver()->GetName());
      id = cmd->GetParameterID("Targeter");
      cmd->SetStringParameter(id, solver->GetName());
   }
   else if (type == "Vary")
   {
      // set solver
      Solver *solver = CreateSolver("DifferentialCorrector",
                                    GetDefaultSolver()->GetName());
      id = cmd->GetParameterID("TargeterName");
      cmd->SetStringParameter(id, solver->GetName());
      
      // set variable parameter
      id = cmd->GetParameterID("Variable");
      //----------------------------------------------------
      //@note loj: 10/6/04
      // I should create a burn parameter of V.
      // But use temp code for now.
      //----------------------------------------------------
      
      cmd->SetStringParameter(id, GetDefaultBurn()->GetName() + ".V");
      //cmd->SetStringParameter(id, GetDefaultBurn()->GetName() + ".N");
      
      id = cmd->GetParameterID("InitialValue");
      cmd->SetRealParameter(id, 0.5);
      //cmd->SetRealParameter(id, 0.5);
      
      id = cmd->GetParameterID("Perturbation");
      cmd->SetRealParameter(id, 0.000001);
      //cmd->SetRealParameter(id, 0.00001);

      id = cmd->GetParameterID("MinimumValue");
      cmd->SetRealParameter(id, -9.000e30);
      //cmd->SetRealParameter(id, -9.000e30);
      
      id = cmd->GetParameterID("MaximumValue");
      cmd->SetRealParameter(id, 9.000e30);
      //cmd->SetRealParameter(id, 9.000e30);
      
      id = cmd->GetParameterID("MaximumChange");
      cmd->SetRealParameter(id, 9.000e30);
      //cmd->SetRealParameter(id, 9.000e30);
   }
   else if (type == "Achieve")
   {
      // set solver
      Solver *solver = CreateSolver("DifferentialCorrector",
                                    GetDefaultSolver()->GetName());

      id = cmd->GetParameterID("TargeterName");
      cmd->SetStringParameter(id, solver->GetName());
      
      // set goal parameter
      id = cmd->GetParameterID("Goal");
      cmd->SetStringParameter(id, GetDefaultSpacecraft()->GetName() + ".SMA");
      //cmd->SetStringParameter(id, GetDefaultSpacecraft()->GetName() + ".INC");
      
      id = cmd->GetParameterID("GoalValue");
      cmd->SetStringParameter(id, "8500.0"); 
      //cmd->SetStringParameter(id, "30.0");
      
      id = cmd->GetParameterID("Tolerance");
      cmd->SetRealParameter(id, 0.1);
      //cmd->SetRealParameter(id, 0.1);
   }
   
   return cmd;
}


//------------------------------------------------------------------------------
// void SetCommandsUnchanged(Integer whichList)
//------------------------------------------------------------------------------
/**
 * Resets the command list to the unchanged state.
 * 
 * @param <whichList>   index indicating which command list gets updated
 */
//------------------------------------------------------------------------------
void Moderator::SetCommandsUnchanged(Integer whichList)
{
   commands[whichList]->ConfigurationChanged(false, true);
}


// CoordinateSystem
//------------------------------------------------------------------------------
// CoordinateSystem* GetInternalCoordinateSystem()
//------------------------------------------------------------------------------
/**
 * @return internal CoordinateSystem.
 */
//------------------------------------------------------------------------------
CoordinateSystem* Moderator::GetInternalCoordinateSystem()
{
   return theInternalCoordSystem;
}

//Planetary files
//------------------------------------------------------------------------------
// StringArray& GetPlanetarySourceTypes()
//------------------------------------------------------------------------------
/**
 * @return a planetary source types
 */
//------------------------------------------------------------------------------
StringArray& Moderator::GetPlanetarySourceTypes()
{
   return thePlanetarySourceTypes;
}

//------------------------------------------------------------------------------
// StringArray& GetPlanetarySourceNames()
//------------------------------------------------------------------------------
/**
 * @return a planetary source file names
 */
//------------------------------------------------------------------------------
StringArray& Moderator::GetPlanetarySourceNames()
{
   return thePlanetarySourceNames;
}

//------------------------------------------------------------------------------
// StringArray& GetPlanetarySourceTypesInUse()
//------------------------------------------------------------------------------
/**
 * @return a planetary source types in use
 */
//------------------------------------------------------------------------------
StringArray& Moderator::GetPlanetarySourceTypesInUse()
{
   theTempFileList.clear();
   for (unsigned int i=0; i<thePlanetarySourceTypesInUse.size(); i++)
      theTempFileList.push_back(thePlanetarySourceTypesInUse[i]);

   return theTempFileList;
}


//------------------------------------------------------------------------------
// StringArray& GetAnalyticModelNames()
//------------------------------------------------------------------------------
/**
 * @return available planetary analytic model names.
 */
//------------------------------------------------------------------------------
StringArray& Moderator::GetAnalyticModelNames()
{
   return theAnalyticModelNames;
}


//------------------------------------------------------------------------------
// bool SetAnalyticModelToUse(const std::string &modelName)
//------------------------------------------------------------------------------
bool Moderator::SetAnalyticModelToUse(const std::string &modelName)
{
   int modelId = 0;
   for (int i=0; i<AnalyticModelCount; i++)
   {
      if (ANALYTIC_MODEL_STRING[i] == modelName)
      {
         modelId = i;
         break;
      }
   }
   
   // because NO_ANALYTIC_METHOD = 0 in CelestialBody.hpp
   theAnalyticMethod = Gmat::AnalyticMethod(modelId + 1);
   
   #if DEBUG_PLANETARY_FILE
   MessageInterface::ShowMessage
      ("Moderator::SetAnalyticModelToUse() theAnalyticMethod=%d\n",
       theAnalyticMethod);
   #endif
   
   return theSolarSystemInUse->SetAnalyticMethod(theAnalyticMethod);
}


//------------------------------------------------------------------------------
// bool SetPlanetarySourceName(const std::string &sourceType,
//                           const std::string &fileName)
//------------------------------------------------------------------------------
bool Moderator::SetPlanetarySourceName(const std::string &sourceType,
                                     const std::string &fileName)
{
   bool status = false;
   Integer id = GetPlanetarySourceId(sourceType);

   if (id >= 0)
   {
      thePlanetarySourceNames[id] = fileName;
      status = true;
   }
    
   return status;
}

//------------------------------------------------------------------------------
// std::string GetPlanetarySourceName(const std::string &sourceType)
//------------------------------------------------------------------------------
std::string Moderator::GetPlanetarySourceName(const std::string &sourceType)
{
   Integer id = GetPlanetarySourceId(sourceType);

   if (id >= 0)
      return thePlanetarySourceNames[id];
   else
      return "Unknown Source Type";
}


//------------------------------------------------------------------------------
// Integer SetPlanetarySourceTypesInUse(const StringArray &sourceTypes)
//------------------------------------------------------------------------------
/*
 * @param <sourceTypes> list of file type in the priority order of use
 *
 * @return 0, if error setting any of planetary file in the list.
 *         1, if error setting first planetary file in the list, but set to
 *            next available file.
 *         2, if successfuly set to first planetary file in the list
 */
//------------------------------------------------------------------------------
Integer Moderator::SetPlanetarySourceTypesInUse(const StringArray &sourceTypes)
{
   #if DEBUG_PLANETARY_FILE
   MessageInterface::
      ShowMessage("Moderator::SetPlanetarySourceTypesInUse() num source types=%d\n",
                  sourceTypes.size());
   #endif
   
   bool status = false;
   Integer sourceTypeInUse = -1;
   Integer retCode = 0;
   
   // update planetary file types
   if (&thePlanetarySourceTypesInUse != &sourceTypes)
   {
      #if DEBUG_PLANETARY_FILE
      MessageInterface::ShowMessage
         ("Moderator::SetPlanetarySourceTypesInUse() updating planetary source\n");
      #endif
      
      thePlanetarySourceTypesInUse.clear();
    
      for (unsigned int i=0; i<sourceTypes.size(); i++)
      {
         thePlanetarySourceTypesInUse.push_back(sourceTypes[i]);
      }
   }

   theTempFileList.clear();
   for (unsigned int i=0; i<thePlanetarySourceTypesInUse.size(); i++)
      theTempFileList.push_back(thePlanetarySourceTypesInUse[i]);
   
   // create planetary ephem file if non-analytic
   for (unsigned int i=0; i<thePlanetarySourceTypesInUse.size(); i++)
   {
      if (thePlanetarySourceTypesInUse[i] == PLANETARY_SOURCE_STRING[ANALYTIC])
      {
         thePlanetarySourcePriority[ANALYTIC] = 0;
         status = theSolarSystemInUse->SetAnalyticMethod(theAnalyticMethod);
         if (status)
         {
            thePlanetarySourcePriority[ANALYTIC] = HIGHEST_PRIORITY - i;
            isPlanetarySourceInUse[ANALYTIC] = true;
            sourceTypeInUse = ANALYTIC;
            break;
         }
         else
         {
            MessageInterface::ShowMessage
               ("*** Error *** Failed to Set AnalyticMethod: %d\n",
                theAnalyticMethod);
         }
      }
      else if (thePlanetarySourceTypesInUse[i] == PLANETARY_SOURCE_STRING[SLP])
      {
         thePlanetarySourcePriority[SLP] = 0;
         status = CreateSlpFile(thePlanetarySourceNames[SLP]);
         if (status)
         {
            thePlanetarySourcePriority[SLP] = HIGHEST_PRIORITY - i;
            isPlanetarySourceInUse[SLP] = true;
            sourceTypeInUse = SLP;
            break;
         }
      }
      else if (thePlanetarySourceTypesInUse[i] == PLANETARY_SOURCE_STRING[DE200])
      {
         #if DEBUG_PLANETARY_FILE
         MessageInterface::
            ShowMessage("Moderator::SetPlanetarySourceTypesInUse() create DE200\n");
         #endif
         
         thePlanetarySourcePriority[DE200] = 0;
         status = CreateDeFile(DE200, thePlanetarySourceNames[DE200]);
         if (status)
         {
            thePlanetarySourcePriority[DE200] = HIGHEST_PRIORITY - i;
            isPlanetarySourceInUse[DE200] = true;
            sourceTypeInUse = DE200;
            break;
         }
      }
      else if (thePlanetarySourceTypesInUse[i] == PLANETARY_SOURCE_STRING[DE405])
      {
         #if DEBUG_PLANETARY_FILE
         MessageInterface::
            ShowMessage("Moderator::SetPlanetarySourceTypesInUse() create DE405\n");
         #endif
         
         thePlanetarySourcePriority[DE405] = 0;
         status = CreateDeFile(DE405, thePlanetarySourceNames[DE405]);
         if (status)
         {
            thePlanetarySourcePriority[DE405] = HIGHEST_PRIORITY - i;
            isPlanetarySourceInUse[DE405] = true;
            sourceTypeInUse = DE405;
            break;
         }
      }
   }

   // set SolarSystem to use the file
   if (sourceTypeInUse == -1)
   {
      MessageInterface::ShowMessage("Moderator::SetPlanetarySourceTypesInUse() NO "
                                    "Planetary file is set to use \n");
      retCode = 0;
   }
   else
   {
      #if DEBUG_PLANETARY_FILE
      MessageInterface::
         ShowMessage("Moderator::SetPlanetarySourceTypesInUse() "
                     "Set Planetary Source to use:%d\n", sourceTypeInUse);
      #endif
      switch (sourceTypeInUse)
      {
      case ANALYTIC:
         if (theSolarSystemInUse->SetSource(Gmat::ANALYTIC))
            if (theSolarSystemInUse->SetAnalyticMethod(theAnalyticMethod))
               retCode = 1;
         break;
      case SLP:
         if (theSolarSystemInUse->SetSource(Gmat::SLP))
            if (theSolarSystemInUse->SetSourceFile(theDefaultSlpFile))
               retCode = 1;
         break;
      case DE200:
         if (theSolarSystemInUse->SetSource(Gmat::DE_200))
            if (theSolarSystemInUse->SetSourceFile(theDefaultDeFile))
               retCode = 1;
         break;
      case DE405:
         if (theSolarSystemInUse->SetSource(Gmat::DE_405))
            if (theSolarSystemInUse->SetSourceFile(theDefaultDeFile))
               retCode = 1;
         break;
      default:
         break;
      }
   }

   // if planetary file is set to first type in the list
   if (retCode == 1 && PLANETARY_SOURCE_STRING[sourceTypeInUse] == sourceTypes[0])
      retCode = 2;


   // if error setting given planetary file, re-arrange planetary file list
   if (retCode == 1)
   {      
      thePlanetarySourceTypesInUse.clear();

      for (unsigned int i=0; i<theTempFileList.size(); i++)
      {            
         if (theTempFileList[i] == PLANETARY_SOURCE_STRING[ANALYTIC])
         {
            if (thePlanetarySourcePriority[ANALYTIC] > 0)
               thePlanetarySourceTypesInUse.push_back(PLANETARY_SOURCE_STRING[ANALYTIC]);
         }
         else if (theTempFileList[i] == PLANETARY_SOURCE_STRING[SLP])
         {
            if (thePlanetarySourcePriority[SLP] > 0)
               thePlanetarySourceTypesInUse.push_back(PLANETARY_SOURCE_STRING[SLP]);
         }
         else if (theTempFileList[i] == PLANETARY_SOURCE_STRING[DE200])
         {
            if (thePlanetarySourcePriority[DE200] > 0)
               thePlanetarySourceTypesInUse.push_back(PLANETARY_SOURCE_STRING[DE200]);
         }
         else if (theTempFileList[i] == PLANETARY_SOURCE_STRING[DE405])
         {
            if (thePlanetarySourcePriority[DE405] > 0)
               thePlanetarySourceTypesInUse.push_back(PLANETARY_SOURCE_STRING[DE405]);
         }
      }
      
      #if DEBUG_PLANETARY_FILE
      for (unsigned int i=0; i<thePlanetarySourceTypesInUse.size(); i++)
      {
         MessageInterface::ShowMessage
            ("thePlanetarySourceTypesInUse[%d]=%s\n", i,
             thePlanetarySourceTypesInUse[i].c_str());
      }
      #endif
   }
   
   if (retCode > 0)
      MessageInterface::ShowMessage
         ("Successfully set Planetary Source to use: %s\n",
          PLANETARY_SOURCE_STRING[sourceTypeInUse].c_str());
   return retCode;
}

//------------------------------------------------------------------------------
// Integer GetPlanetarySourceId(const std::string &sourceType)
//------------------------------------------------------------------------------
Integer Moderator::GetPlanetarySourceId(const std::string &sourceType)
{
   for (int i=0; i<PlanetarySourceCount; i++)
   {
      if (sourceType == PLANETARY_SOURCE_STRING[i])
         return i;
   }
   
   return -1;
}

// Potential field files
//------------------------------------------------------------------------------
// std::string GetPotentialFileName(const std::string &fileType)
//------------------------------------------------------------------------------
std::string Moderator::GetPotentialFileName(const std::string &fileType)
{
   if (fileType == "JGM2")
      return theFileManager->GetFullPathname("JGM2_FILE");
   else if (fileType == "JGM3")
      return theFileManager->GetFullPathname("JGM3_FILE");
   else
      return "Unknown Potential File Type:" + fileType;
}


//------------------------------------------------------------------------------
// std::string GetFileName(const std::string &fileType)
//------------------------------------------------------------------------------
std::string Moderator::GetFileName(const std::string &fileType)
{
   return theFileManager->GetFullPathname(fileType);
}


// Mission
//------------------------------------------------------------------------------
// bool LoadDefaultMission()
//------------------------------------------------------------------------------
bool Moderator::LoadDefaultMission()
{
   CreateDefaultMission();
    
   return true;
}

// Resource
//------------------------------------------------------------------------------
// bool ClearResource()
//------------------------------------------------------------------------------
bool Moderator::ClearResource()
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage("Moderator::ClearResource() entered\n");
   #endif
   
   theConfigManager->RemoveAllItems();
   ClearAllSandboxes();
   
   return true;
}

// Command Sequence
//------------------------------------------------------------------------------
// bool ClearCommandSeq(Integer sandboxNum = 1)
//------------------------------------------------------------------------------
bool Moderator::ClearCommandSeq(Integer sandboxNum)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage("Moderator::ClearCommandSeq() entered\n");
   #endif
   
   //djc: Maybe set to NULL if you plan to do something completely different from
   // the way GMAT acts from a script?  I think you want to do this, though:
   // commands[sandboxNum-1] = NULL;
   GmatCommand *cmd = commands[sandboxNum-1], *oldcmd;
   
   // Be sure we're in an idle state first
   cmd->RunComplete();
   
   oldcmd = cmd->GetNext();
   DeleteCommand(cmd);
   if (oldcmd) {
      #ifdef DEBUG_SEQUENCE_CLEARING
         GmatCommand *current = oldcmd;
         MessageInterface::ShowMessage("Clearing this command list:\n   ");
         while (current) {
            MessageInterface::ShowMessage("%s\n   ",
                                          current->GetTypeName().c_str());
            current = current->GetNext();
         }
         MessageInterface::ShowMessage("\n");
      #endif
      delete oldcmd;
   }
   
   // djc: if you plan on adding the gui commands to the sandbox next, using 
   // the same approach used when running a script.
   cmd = new NoOp; 
    
   return true;
}

//------------------------------------------------------------------------------
// bool AppendCommand(GmatCommand *cmd, Integer sandboxNum)
//------------------------------------------------------------------------------
bool Moderator::AppendCommand(GmatCommand *cmd, Integer sandboxNum)
{
   return commands[sandboxNum-1]->Append(cmd);
}

//------------------------------------------------------------------------------
// GmatCommand* AppendCommand(const std::string &type, const std::string &name,
//                        Integer sandboxNum)
//------------------------------------------------------------------------------
GmatCommand* Moderator::AppendCommand(const std::string &type,
                                      const std::string &name,
                                      Integer sandboxNum)
{
   bool status;
   GmatCommand *cmd = theFactoryManager->CreateCommand(type, name);

   if (cmd != NULL)
   {
      if (name != "")
         theConfigManager->AddCommand(cmd);
    
      status = commands[sandboxNum-1]->Append(cmd);
   }
    
   return cmd;
}


//------------------------------------------------------------------------------
// bool InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd, Integer sandboxNum)
//------------------------------------------------------------------------------
bool Moderator::InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd,
                              Integer sandboxNum)
{
   return commands[sandboxNum-1]->Insert(cmd, prevCmd);
}


//------------------------------------------------------------------------------
// GmatCommand* DeleteCommand(GmatCommand *cmd, Integer sandboxNum)
//------------------------------------------------------------------------------
GmatCommand* Moderator::DeleteCommand(GmatCommand *cmd, Integer sandboxNum)
{
   return commands[sandboxNum-1]->Remove(cmd);
}


//------------------------------------------------------------------------------
// GmatCommand* GetNextCommand(Integer sandboxNum)
//------------------------------------------------------------------------------
GmatCommand* Moderator::GetNextCommand(Integer sandboxNum)
{
   return commands[sandboxNum-1];
}


// sandbox
//------------------------------------------------------------------------------
// void ClearAllSandboxes()
//------------------------------------------------------------------------------
void Moderator::ClearAllSandboxes()
{
   for (UnsignedInt i=0; i<sandboxes.size(); i++)
      if (sandboxes[i])
         sandboxes[i]->Clear();
}


//------------------------------------------------------------------------------
// GmatBase* GetInternalObject(const std::string &name, Integer sandboxNum = 1)
//------------------------------------------------------------------------------
GmatBase* Moderator::GetInternalObject(const std::string &name, Integer sandboxNum)
{
   return sandboxes[sandboxNum-1]->GetInternalObject(name);
}


//------------------------------------------------------------------------------
// Integer RunMission(Integer sandboxNum)
//------------------------------------------------------------------------------
Integer Moderator::RunMission(Integer sandboxNum)
{
   //MessageInterface::ShowMessage("\n========================================\n");
   //MessageInterface::ShowMessage("Moderator::RunMission() entered\n");
   MessageInterface::ShowMessage("Running mission...\n");
   Integer status = 0;

   clock_t t1 = clock(); // Should I clock after initilization?
   
   if (isRunReady)
   {
      // clear sandbox
      if (sandboxNum > 0 && sandboxNum <= Gmat::MAX_SANDBOX)
      {
         sandboxes[sandboxNum-1]->Clear();
         #if DEBUG_RUN
         MessageInterface::ShowMessage
            ("Moderator::RunMission() after sandboxes[%d]->Clear()\n", sandboxNum-1);
         #endif
      }
      else
      {
         status = -1;
         MessageInterface::PopupMessage(Gmat::ERROR_,
                                        "Invalid Sandbox number" + sandboxNum);
         return status;
      }

      try
      {
         // add objects to sandbox and initialize
         AddSolarSystemToSandbox(sandboxNum-1);
         AddInternalCoordSystemToSandbox(sandboxNum-1);
         AddPublisherToSandbox(sandboxNum-1);        
         AddCoordSystemToSandbox(sandboxNum-1);
         AddSpacecraftToSandbox(sandboxNum-1);
         AddFormationToSandbox(sandboxNum-1);
         AddForceModelToSandbox(sandboxNum-1);
         AddPropagatorToSandbox(sandboxNum-1);
         AddPropSetupToSandbox(sandboxNum-1);
         AddBurnToSandbox(sandboxNum-1);        
         AddSolverToSandbox(sandboxNum-1);

         // Note:
         // Add Subscriber after Publisher.
         // AddPublisherToSandbox() clears subscribers
         AddSubscriberToSandbox(sandboxNum-1); 
         AddParameterToSandbox(sandboxNum-1);
         AddFunctionToSandbox(sandboxNum-1);
         AddCommandToSandbox(sandboxNum-1);
         
         #if DEBUG_RUN
         MessageInterface::ShowMessage
            ("Moderator::RunMission() after AddCommandToSandbox()\n");
         #endif
         
         InitializeSandbox(sandboxNum-1);
         
         #if DEBUG_RUN
         MessageInterface::ShowMessage
            ("Moderator::RunMission() after InitializeSandbox()\n");
         #endif

         // execute sandbox
         runState = Gmat::RUNNING;
         ExecuteSandbox(sandboxNum-1);
         
         #if DEBUG_RUN
         MessageInterface::ShowMessage
            ("Moderator::RunMission() after ExecuteSandbox()\n");
         #endif
      
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage
            ("Moderator::RunMission() %s\n", e.GetMessage().c_str());
         MessageInterface::PopupMessage(Gmat::ERROR_, e.GetMessage());
         // assign status
         status = -2;
      }
      catch (...)
      {
         MessageInterface::ShowMessage
            ("Moderator::RunMission() Unknown error occurred.\n");
         throw;
      }
   }
   else
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Mission not Complete. Cannot Run Mission.\n");
      status = -3;
   }
   
   runState = Gmat::IDLE;
   theGuiInterpreter->NotifyRunCompleted();
   
   if (status == 0)
      MessageInterface::ShowMessage("Mission run completed.\n");
   else
      MessageInterface::ShowMessage("*** Mission run failed.\n");
   
   clock_t t2 = clock();
   MessageInterface::ShowMessage
      ("===> Total Run Time: %f seconds\n", (Real)(t2-t1)/CLOCKS_PER_SEC);

   // show final state
   #ifdef __SHOW_FINAL_STATE__
   showFinalState = true;
   #endif
   
   if (showFinalState)
   {
      GmatCommand *cmd = GetNextCommand();
      //MessageInterface::ShowMessage(GmatCommandUtil::GetCommandSeqString(cmd));
      GmatCommand *lastCmd = GmatCommandUtil::GetLastCommand(cmd);
      
      MessageInterface::ShowMessage("\n========== Final State ==========\n");
      MessageInterface::ShowMessage(lastCmd->GetStringParameter("MissionSummary"));
      MessageInterface::ShowMessage("\n\n");      
   }
   else
   {
      MessageInterface::ShowMessage("\n========================================\n");
   }
   
   return status;
}


//------------------------------------------------------------------------------
// Integer ChangeRunState(const std::string &state, Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Changes run state.
 *
 * @param <state> run state string ("Stop", "Pause", "Resume")
 * @param <snadobxNum> sandbox number
 *
 * @return a status code
 *    0 = successful, <0 = error (tbd)
 */
//------------------------------------------------------------------------------
Integer Moderator::ChangeRunState(const std::string &state, Integer sandboxNum)
{
   #if DEBUG_USER_INTERRUPT
   MessageInterface::ShowMessage("Moderator::ChangeRunState() entered\n");
   #endif
   
   if (state == "Stop")   
      runState = Gmat::IDLE;

   else if (state == "Pause")
      runState = Gmat::PAUSED;
   
   else if (state == "Resume")
      runState = Gmat::RUNNING;
   
   else
      ; // no action
   
   return 0;
}


//------------------------------------------------------------------------------
// Gmat::RunState GetUserInterrupt()
//------------------------------------------------------------------------------
/**
 * Checks to see if the user has requested that the run stop or pause.
 * 
 * This method is called by the Sandbox periodically during a run to determine
 * if the user has requested that the run terminate before the mission sequence
 * has finished executing.
 * 
 * @return The expected state of the system (RUNNING, PAUSED, or IDLE).
 */
//------------------------------------------------------------------------------
Gmat::RunState Moderator::GetUserInterrupt()
{
   #if DEBUG_USER_INTERRUPT
   MessageInterface::ShowMessage("Moderator::GetUserInterrupt() entered\n");
   #endif
   
   // give MainFrame input focus
   theGuiInterpreter->SetInputFocus();
   return runState;
}


//------------------------------------------------------------------------------
// Gmat::RunState GetRunState()
//------------------------------------------------------------------------------
/**
 * @return the state of the system (Gmat::RUNNING, Gmat::PAUSED, Gmat::IDLE)
 */
//------------------------------------------------------------------------------
Gmat::RunState Moderator::GetRunState()
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::GetRunsState() isRunReady=%d, endOfInterpreter=%d\n",
       isRunReady, endOfInterpreter);
   #endif
   
   // return RUNNING so that Matlab can wait for building objects
   if (!isRunReady && !endOfInterpreter)
      return Gmat::RUNNING;
   
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::GetRunsState() runState=%d\n", runState);
   #endif

   return runState;
}


// Script
//------------------------------------------------------------------------------
// bool InterpretScript(const std::string &scriptFileName)
//------------------------------------------------------------------------------
/**
 * Creates objects from script file.
 *
 * @param <scriptFileName> input script file name
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::InterpretScript(const std::string &scriptFileName)
{
   bool status = false;
   isRunReady = false;
   endOfInterpreter = false;
   runState = Gmat::IDLE;
   
   //MessageInterface::ShowMessage("========================================\n");
   //MessageInterface::ShowMessage("Moderator::InterpretScript() entered\n"
   //                              "***** file: " + scriptFileName + "\n");
   MessageInterface::ShowMessage
      ("Interpreting scripts from the file.\n***** file: " + scriptFileName + "\n");
   
   //clear both resource and command sequence
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::InterpretScript() clearing both resource and command sequence...\n");
   #endif
   ClearResource();
   ClearCommandSeq();
   
   try
   {
      CreateSolarSystemInUse();
      
      // Need default CS's in case they are used in the script
      CreateDefaultCoordSystems();

      status = theScriptInterpreter->Interpret(scriptFileName);
      if (status)
      {
         #if DEBUG_RUN
         MessageInterface::ShowMessage
             ("Moderator::InterpretScript() creating Default Coordinate "
              "System...\n");
         #endif

         #if DEBUG_RUN
         MessageInterface::ShowMessage
            ("Moderator::InterpretScript() successfully interpreted the script\n");
         #endif
         
         isRunReady = true;
      }
      else
      {
         MessageInterface::ShowMessage("\n========================================\n");
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetMessage());
      //MessageInterface::ShowMessage(e.GetMessage());
      ResetConfigurationChanged(); //loj: 1/12/06 added
      isRunReady = false;
   }
   
   endOfInterpreter = true;

   return status;
}

//------------------------------------------------------------------------------
// bool InterpretScript(std::istringstream *ss, bool clearObjs)
//------------------------------------------------------------------------------
/**
 * Creates objects from stringstream
 *
 * @param <ss> input istringstream
 * @param <clearObjs> clears objects and mission sequence if true
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::InterpretScript(std::istringstream *ss, bool clearObjs)
{
   bool status = false;
   isRunReady = false;
   endOfInterpreter = false;
   runState = Gmat::IDLE;
   
   //MessageInterface::ShowMessage("========================================\n");
   //MessageInterface::ShowMessage("Moderator::InterpretScript(ss) entered\n");
   MessageInterface::ShowMessage
      ("Interpreting scripts from the input stream\n");
   
   //clear both resource and command sequence
   if (clearObjs)
   {
      ClearResource();
      ClearCommandSeq();
   }
   
   try
   {
      CreateSolarSystemInUse();
      CreateDefaultCoordSystems();
      
      theScriptInterpreter->SetInStream(ss);
      status = theScriptInterpreter->Interpret();
      if (status)
      {
         #if DEBUG_RUN
         MessageInterface::ShowMessage
            ("Moderator::InterpretScript(ss) creating Default Coordinate "
             "System...\n");
         #endif
         
         //CreateDefaultCoordSystems();

         #if DEBUG_RUN
         MessageInterface::ShowMessage
            ("Moderator::InterpretScript() successfully interpreted the script\n");
         #endif
         
         isRunReady = true;
      }
      else
      {
         MessageInterface::ShowMessage("\n========================================\n");
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetMessage());
      //MessageInterface::ShowMessage(e.GetMessage());
      ResetConfigurationChanged(); //loj: 1/12/06 added
      isRunReady = false;
   }

   endOfInterpreter = true;

   return status;
}

//------------------------------------------------------------------------------
// bool SaveScript(const std::string &scriptFileName)
//------------------------------------------------------------------------------
/**
 * Builds scripts from objects and write to a file.
 *
 * @param <scriptFileName> output script file name
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::SaveScript(const std::string &scriptFileName)
{
   //MessageInterface::ShowMessage("Moderator::SaveScript() entered\n"
   //                              "file: " + scriptFileName + "\n");
   //   MessageInterface::PopupMessage(Gmat::INFO_, "The Script is saved to " +
   //                                  scriptFileName);
   
   MessageInterface::ShowMessage("The Script is saved to " +
                                 scriptFileName + "\n");
   
   try
   {
      return theScriptInterpreter->Build(scriptFileName);
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetMessage());
      return false;
   }
}

//------------------------------------------------------------------------------
// Integer RunScript(Integer sandboxNum = 1)
//------------------------------------------------------------------------------
/**
 * Executes commands built from the script file.
 *
 * @param <sandboxNum> sandbox number
 *
 * @return a status code
 *    0 = successful, <0 = error (tbd)
 */
//------------------------------------------------------------------------------
Integer Moderator::RunScript(Integer sandboxNum)
{
   MessageInterface::ShowMessage("Moderator::RunScript() entered\n");
   return RunMission(sandboxNum);
}

//---------------------------------
//  private
//---------------------------------

// initialization
//------------------------------------------------------------------------------
// void InitializePlanetarySource()
//------------------------------------------------------------------------------
void Moderator::InitializePlanetarySource()
{
   #if DEBUG_INIT
   //MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("Moderator initializing planetary source...\n");
   #endif
   
   // initialize planetary source
   for (int i=0; i<PlanetarySourceCount; i++)
   {
      thePlanetarySourcePriority[i] = 0;
      isPlanetarySourceInUse[i] = false;
      
      thePlanetarySourceTypes.push_back(PLANETARY_SOURCE_STRING[i]);
   }
   
   for (int i=0; i<AnalyticModelCount; i++)
   {
      theAnalyticModelNames.push_back(ANALYTIC_MODEL_STRING[i]);
   }

   theAnalyticMethod = Gmat::LOW_FIDELITY;
   
   //initialize file names
   thePlanetarySourceNames.push_back("NA");
   thePlanetarySourceNames.push_back(theFileManager->
                                     GetFullPathname("SLP_FILE"));
   thePlanetarySourceNames.push_back(theFileManager->
                                     GetFullPathname("DE200_FILE"));
   thePlanetarySourceNames.push_back(theFileManager->
                                     GetFullPathname("DE405_FILE"));
   
   // initialize planetary file types/names in use
   // Set DE405 as default
   SetDefaultPlanetarySource();
   
//    thePlanetarySourceTypesInUse.push_back(PLANETARY_SOURCE_STRING[DE405]);
//    thePlanetarySourceTypesInUse.push_back(PLANETARY_SOURCE_STRING[ANALYTIC]); 
//    thePlanetarySourceTypesInUse.push_back(PLANETARY_SOURCE_STRING[SLP]);
   
//    SetPlanetarySourceTypesInUse(thePlanetarySourceTypesInUse);
}


//------------------------------------------------------------------------------
// void InitializePlanetaryCoeffFile()
//------------------------------------------------------------------------------
void Moderator::InitializePlanetaryCoeffFile()
{
   #if DEBUG_INIT
   //MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("Moderator initializing planetary coeff. file...\n");
   #endif
   
   std::string nutFileName =
      theFileManager->GetFullPathname("NUTATION_COEFF_FILE");
   MessageInterface::ShowMessage("Setting nutation file to %s\n",
                                 nutFileName.c_str());
   std::string planFileName =
      theFileManager->GetFullPathname("PLANETARY_COEFF_FILE");
   MessageInterface::ShowMessage("Setting planetary coeff. file to %s\n",
                                 planFileName.c_str());
   
   theItrfFile = new ItrfCoefficientsFile(nutFileName, planFileName);
   theItrfFile->Initialize();
}


//------------------------------------------------------------------------------
// void InitializeTimeFile()
//------------------------------------------------------------------------------
void Moderator::InitializeTimeFile()
{
   #if DEBUG_INIT
   //MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("Moderator initializing time file...\n");
   #endif
   
   std::string filename = theFileManager->GetFullPathname("LEAP_SECS_FILE");
   MessageInterface::ShowMessage("Setting leap seconds file to %s\n",
                                 filename.c_str());
   theLeapSecsFile = new LeapSecsFileReader(filename);
   theLeapSecsFile->Initialize();
   
   filename = theFileManager->GetFullPathname("EOP_FILE");
   theEopFile = new EopFile(filename);
   theEopFile->Initialize();
   
   TimeConverterUtil::SetLeapSecsFileReader(theLeapSecsFile);
   TimeConverterUtil::SetEopFile(theEopFile);
}


//------------------------------------------------------------------------------
// void SetDefaultPlanetarySource()
//------------------------------------------------------------------------------
void Moderator::SetDefaultPlanetarySource()
{
   #if DEBUG_INIT
   MessageInterface::ShowMessage("Moderator setting default planetary source...\n");
   #endif
         
   // initialize planetary file types/names in use
   // Set DE405 as default
   thePlanetarySourceTypesInUse.clear();
   thePlanetarySourceTypesInUse.push_back(PLANETARY_SOURCE_STRING[DE405]);
   thePlanetarySourceTypesInUse.push_back(PLANETARY_SOURCE_STRING[ANALYTIC]); 
   thePlanetarySourceTypesInUse.push_back(PLANETARY_SOURCE_STRING[SLP]);
   
   SetPlanetarySourceTypesInUse(thePlanetarySourceTypesInUse);
}


//------------------------------------------------------------------------------
// void CreateSolarSystemInUse()
//------------------------------------------------------------------------------
void Moderator::CreateSolarSystemInUse()
{
   // Create SolarSystem in use (loj: 2/8/06)
   // Note: GMAT crashes when I delete solarSystemInUse in ConfigManager
   // So just set to default values for now.

   // We can Clone theDefaultSolarSystem when we can delete solarSystemInUse 
   //delete theSolarSystemInUse;
   //theSolarSystemInUse = theDefaultSolarSystem->Clone();

   // Until then we just set defaults
   theSolarSystemInUse->ResetToDefaults();
   theConfigManager->SetSolarSystemInUse(theSolarSystemInUse);
   SetDefaultPlanetarySource();
}


//------------------------------------------------------------------------------
// void CreateDefaultCoordSystems()
//------------------------------------------------------------------------------
void Moderator::CreateDefaultCoordSystems()
{
   #if DEBUG_INIT
   //MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("Moderator creating default coordinate systems...\n");
   #endif
   
   try
   {
      StringArray csNames =
            theConfigManager->GetListOfItems(Gmat::COORDINATE_SYSTEM);
      
      SpacePoint *earth = (SpacePoint*)GetConfiguredItem("Earth");
      
      // EarthMJ2000Eq
      if (find(csNames.begin(), csNames.end(), "EarthMJ2000Eq") == csNames.end())
         CreateCoordinateSystem("EarthMJ2000Eq", true);
      
      // EarthMJ2000Ec
      if (find(csNames.begin(), csNames.end(), "EarthMJ2000Ec") == csNames.end())
      {
         CoordinateSystem *eccs = CreateCoordinateSystem("EarthMJ2000Ec", false);
         AxisSystem *ecAxis = CreateAxisSystem("MJ2000Ec", "EarthMJ2000Ec");
         eccs->SetStringParameter("Origin", "Earth");
         eccs->SetStringParameter("J2000Body", "Earth");
         eccs->SetRefObject(ecAxis, Gmat::AXIS_SYSTEM, ecAxis->GetName());
         eccs->SetOrigin(earth);
         eccs->SetJ2000Body(earth);
         eccs->SetSolarSystem(theSolarSystemInUse);
         eccs->Initialize();
      }
      
      // EarthFixed
      if (find(csNames.begin(), csNames.end(), "EarthFixed") == csNames.end())
      {
         CoordinateSystem *bfcs = CreateCoordinateSystem("EarthFixed", false);
         BodyFixedAxes *bfecAxis =
            (BodyFixedAxes*)CreateAxisSystem("BodyFixed", "EarthFixed");
         bfecAxis->SetEopFile(theEopFile);
         bfecAxis->SetCoefficientsFile(theItrfFile);
         bfcs->SetStringParameter("Origin", "Earth");
         bfcs->SetStringParameter("J2000Body", "Earth");
         bfcs->SetRefObject(bfecAxis, Gmat::AXIS_SYSTEM, bfecAxis->GetName());
         bfcs->SetOrigin(earth);
         bfcs->SetJ2000Body(earth);
         bfcs->SetSolarSystem(theSolarSystemInUse);
         bfcs->Initialize();
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_,
          "Moderator::CreateDefaultCoordSystems() Error occurred during default "
          "coordinate system creation. " +  e.GetMessage());
   }
}


//------------------------------------------------------------------------------
// void CreateDefaultMission()
//------------------------------------------------------------------------------
void Moderator::CreateDefaultMission()
{
   #if DEBUG_INIT
   //MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("Moderator creating default mission...\n");
   #endif
   
   try
   {
      //----------------------------------------------------
      // Create default resource
      //----------------------------------------------------

      // Create solar system in use
      CreateSolarSystemInUse();
      
      // Create default coordinate systems
      CreateDefaultCoordSystems();
      
      // Spacecraft
      Spacecraft *sc = (Spacecraft*)CreateSpacecraft("Spacecraft", "DefaultSC");
      sc->SetInternalCoordSystem(theInternalCoordSystem);
      sc->SetRefObject(GetCoordinateSystem("EarthMJ2000Eq"),
                       Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
      
      #if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default Spacecraft created\n");
      #endif
      
      // PropSetup
      CreateDefaultPropSetup("DefaultProp");
      
      #if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default PropSetup created\n");
      #endif

      //--------------------------------------------------------------
      // test Burn Parameter
      //--------------------------------------------------------------
      #ifdef DEBUG_CREATE_BURN_PARAM
      // Hardware 
      CreateHardware("FuelTank", "DefaultFuelTank");
      CreateHardware("Thruster", "DefaultThruster");
      // Burn
      GetDefaultBurn();
      
      // Create VNB CoordinateSystem
      CoordinateSystem *vnb = CreateCoordinateSystem("VNB", false);
      ObjectReferencedAxes *orAxis =
         (ObjectReferencedAxes*)CreateAxisSystem("ObjectReferenced",
                                                 "ObjectReferenced");
      orAxis->SetEopFile(theEopFile);
      orAxis->SetCoefficientsFile(theItrfFile);
      orAxis->SetStringParameter("XAxis", "V");
      orAxis->SetStringParameter("YAxis", "N");
      orAxis->SetStringParameter("Primary", "Earth");
      orAxis->SetStringParameter("Secondary", "DefaultSC");
      vnb->SetStringParameter("Origin", "Earth");
      //      vnb->SetStringParameter("J2000Body", "Earth");
      vnb->SetRefObject(orAxis, Gmat::AXIS_SYSTEM, orAxis->GetName());
      
      // Burn parameters
      CreateParameter("DeltaV1", "DefaultIB.VNB.DeltaV1");
      CreateParameter("DeltaV2", "DefaultIB.VNB.DeltaV2");
      CreateParameter("DeltaV3", "DefaultIB.VNB.DeltaV3");
      #endif
      //--------------------------------------------------------------
      
      // Time parameters
      CreateParameter("ElapsedSecs", "DefaultSC.ElapsedSecs");
      CreateParameter("ElapsedDays", "DefaultSC.ElapsedDays");
      
      CreateParameter("CurrA1MJD", "DefaultSC.CurrA1MJD");
      CreateParameter("A1ModJulian", "DefaultSC.A1ModJulian");
      CreateParameter("A1Gregorian", "DefaultSC.A1Gregorian");
      CreateParameter("TAIModJulian", "DefaultSC.TAIModJulian");
      CreateParameter("TAIGregorian", "DefaultSC.TAIGregorian");
      CreateParameter("TTModJulian", "DefaultSC.TTModJulian");
      CreateParameter("TTGregorian", "DefaultSC.TTGregorian");
      CreateParameter("TDBModJulian", "DefaultSC.TDBModJulian");
      CreateParameter("TDBGregorian", "DefaultSC.TDBGregorian");
      CreateParameter("TCBModJulian", "DefaultSC.TCBModJulian");
      CreateParameter("TCBGregorian", "DefaultSC.TCBGregorian");
      CreateParameter("UTCModJulian", "DefaultSC.UTCModJulian");
      CreateParameter("UTCGregorian", "DefaultSC.UTCGregorian");
      
      // Cartesian parameters
      CreateParameter("X", "DefaultSC.EarthMJ2000Eq.X");
      CreateParameter("Y", "DefaultSC.EarthMJ2000Eq.Y");
      CreateParameter("Z", "DefaultSC.EarthMJ2000Eq.Z");
      CreateParameter("VX", "DefaultSC.EarthMJ2000Eq.VX");
      CreateParameter("VY", "DefaultSC.EarthMJ2000Eq.VY");
      CreateParameter("VZ", "DefaultSC.EarthMJ2000Eq.VZ");
      
      // Keplerian parameters
      CreateParameter("SMA", "DefaultSC.Earth.SMA");
      CreateParameter("ECC", "DefaultSC.Earth.ECC");
      CreateParameter("INC", "DefaultSC.Earth.INC");
      CreateParameter("RAAN", "DefaultSC.Earth.RAAN");
      CreateParameter("AOP", "DefaultSC.EarthMJ2000Eq.AOP");
      CreateParameter("TA", "DefaultSC.Earth.TA");
      CreateParameter("MA", "DefaultSC.Earth.MA");
      CreateParameter("MM", "DefaultSC.Earth.MM");
      
      // Orbital parameters
      CreateParameter("VelApoapsis", "DefaultSC.Earth.VelApoapsis");
      CreateParameter("VelPeriapsis", "DefaultSC.Earth.VelPeriapsis");
      CreateParameter("Apoapsis", "DefaultSC.Earth.Apoapsis");
      CreateParameter("Periapsis", "DefaultSC.Earth.Periapsis");
      CreateParameter("OrbitPeriod", "DefaultSC.Earth.OrbitPeriod");
      CreateParameter("RadApo", "DefaultSC.Earth.RadApo");
      CreateParameter("RadPer", "DefaultSC.Earth.RadPer");
      CreateParameter("C3Energy", "DefaultSC.Earth.C3Energy");
      CreateParameter("Energy", "DefaultSC.Earth.Energy");
      
      // Spherical parameters
      CreateParameter("RMAG", "DefaultSC.Earth.RMAG");
      CreateParameter("RA", "DefaultSC.Earth.RA");
      CreateParameter("DEC", "DefaultSC.EarthMJ2000Eq.DEC");
      CreateParameter("VMAG", "DefaultSC.EarthMJ2000Eq.VMAG");
      CreateParameter("RAV", "DefaultSC.EarthMJ2000Eq.RAV");
      CreateParameter("DECV", "DefaultSC.EarthMJ2000Eq.DECV");
      
      // Angular parameters
      CreateParameter("SemilatusRectum", "DefaultSC.Earth.SemilatusRectum");
      CreateParameter("HMAG", "DefaultSC.HMAG");
      CreateParameter("HX", "DefaultSC.EarthMJ2000Eq.HX");
      CreateParameter("HY", "DefaultSC.EarthMJ2000Eq.HY");
      CreateParameter("HZ", "DefaultSC.EarthMJ2000Eq.HZ");
      
      // Environmental parameters
      CreateParameter("AtmosDensity", "DefaultSC.Earth.AtmosDensity");
      
      // Planet parameters
      CreateParameter("Altitude", "DefaultSC.Earth.Altitude");
      CreateParameter("MHA", "DefaultSC.Earth.MHA");
      CreateParameter("Longitude", "DefaultSC.Earth.Longitude");
      CreateParameter("Latitude", "DefaultSC.Earth.Latitude");
      CreateParameter("LST", "DefaultSC.Earth.LST");
      CreateParameter("BetaAngle", "DefaultSC.BetaAngle");
      
      // B-Plane parameters
      CreateParameter("BdotT", "DefaultSC.Earth.BdotT");
      CreateParameter("BdotR", "DefaultSC.Earth.BdotR");
      CreateParameter("BVectorMag", "DefaultSC.Earth.BVectorMag");
      CreateParameter("BVectorAngle", "DefaultSC.Earth.BVectorAngle");
      
      #ifdef DEBUG_CREATE_VAR
      // User variable
      Parameter *var = CreateParameter("Variable", "DefaultSC_EarthMJ2000Eq_Xx2");
      var->SetStringParameter("Expression", "DefaultSC.EarthMJ2000Eq.X * 2.0");
      var->SetRefObjectName(Gmat::PARAMETER, "DefaultSC.EarthMJ2000Eq.X");
      #endif
      
      #if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default parameters created\n");
      #endif
      
      // Set parameter description and object name
      StringArray &params = GetListOfConfiguredItems(Gmat::PARAMETER);
      Parameter *param;
    
      for (unsigned int i=0; i<params.size(); i++)
      {
         param = GetParameter(params[i]);

         // need spacecraft if system parameter
         if (param->GetKey() == GmatParam::SYSTEM_PARAM)
         {
            //MessageInterface::ShowMessage("name = %s\n", param->GetName().c_str());
            param->SetStringParameter("Expression", param->GetName());
            param->SetRefObjectName(Gmat::SPACECRAFT, "DefaultSC");

            if (param->NeedCoordSystem())
            {
               param->SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
               if (param->IsOriginDependent())
                  param->SetStringParameter("DepObject", "Earth");
               else if (param->IsCoordSysDependent())
                  param->SetStringParameter("DepObject", "EarthMJ2000Eq");
            }
         }
      }
      
      #if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->ref. object to parameters are set\n");
      #endif   

      // StopCondition
      StopCondition *stopOnElapsedSecs =
         CreateStopCondition("StopCondition", "StopOnDefaultSC.ElapsedSecs");
      stopOnElapsedSecs->SetStringParameter("EpochVar", "DefaultSC.A1ModJulian");
      stopOnElapsedSecs->SetStringParameter("StopVar", "DefaultSC.ElapsedSecs");
      stopOnElapsedSecs->SetStringParameter("Goal", "8640.0");
      
      #if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default StopCondition created\n");
      #endif
      
      // Subscribers
      // OpenGLPlot
      Subscriber *sub;
      sub = CreateSubscriber("OpenGLPlot", "DefaultOpenGL");
      sub->SetStringParameter("Add", "DefaultSC");
      sub->SetStringParameter("Add", "Earth");
      sub->SetStringParameter("CoordinateSystem", "EarthMJ2000Eq");
      
      #if DEBUG_ACTION_REMOVE
         sub->SetStringParameter("Add", "Spacecraft1");
         sub->TakeAction("Remove", "Spacecraft1");
      #endif
         
      sub->Activate(true);
      
      #if DEBUG_ACTION_REMOVE
      // XYPlot
      sub = CreateSubscriber("XYPlot", "DefaultXYPlot"); 
      sub->SetStringParameter("IndVar", "DefaultSC.A1ModJulian");
      sub->SetStringParameter("Add", "DefaultSC.EarthMJ2000Eq.X", 0);
      
      sub->SetStringParameter("Add", "DefaultSC.EarthMJ2000Eq.Y", 1);
      sub->SetStringParameter("Add", "DefaultSC.EarthMJ2000Eq.Z", 2);
      sub->TakeAction("Remove", "DefaultSC.EarthMJ2000Eq.Y");
      sub->Activate(true);
      #endif
      
      #if DEBUG_DEFAULT_MISSION
         MessageInterface::ShowMessage("-->default Subscribers created\n");
      #endif
      
      //----------------------------------------------------
      // Create default mission sequence
      //----------------------------------------------------
      
      // Propagate Command
      GmatCommand *propCommand = CreateCommand("Propagate");
      propCommand->SetObject("DefaultProp", Gmat::PROP_SETUP);
      propCommand->SetObject("DefaultSC", Gmat::SPACECRAFT);
      propCommand->SetRefObject(stopOnElapsedSecs, Gmat::STOP_CONDITION, "", 0);
      
      propCommand->SetSolarSystem(theSolarSystemInUse);
      
      #if DEBUG_MULTI_STOP
      //----------------------------------------------------
      //just for testing multiple stopping condition
      //----- StopCondition 2
      StopCondition *stopOnX =
         CreateStopCondition("StopCondition", "StopOnDefaultSC.EarthMJ2000Eq.X");
      stopOnX->SetStringParameter("EpochVar", "DefaultSC.A1ModJulian");
      stopOnX->SetStringParameter("StopVar", "DefaultSC.EarthMJ2000Eq.X");
      //stopOnX->SetRealParameter("Goal", 5000.0);
      stopOnX->SetStringParameter("Goal", "5000.0");
      propCommand->SetRefObject(stopOnX, Gmat::STOP_CONDITION, "", 1);
      #endif
      
      #if DEBUG_MULTI_STOP > 1
      StopCondition *stopOnPeriapsis =
         CreateStopCondition("StopCondition", "StopOnDefaultSC.Earth.Periapsis");
      stopOnPeriapsis->SetStringParameter("EpochVar", "DefaultSC.A1ModJulian");
      stopOnPeriapsis->SetStringParameter("StopVar", "DefaultSC.Earth.Periapsis");
      propCommand->SetRefObject(stopOnPeriapsis, Gmat::STOP_CONDITION, "", 2);
      //----------------------------------------------------
      #endif

      #if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default Propagate command created\n");
      #endif
      
      // Add propagate command
      AppendCommand(propCommand);

      isRunReady = true;
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_,
          "Moderator::CreateDefaultMission() Error occurred during default "
          "mission creation. Default mission will not run.\n Message: " +
          e.GetMessage());
   }
}


//------------------------------------------------------------------------------
// bool CreateSlpFile(const std::string &fileName)
//------------------------------------------------------------------------------
bool Moderator::CreateSlpFile(const std::string &fileName)
{
   //MessageInterface::ShowMessage("Moderator::CreateSlpFile() fileName=%s\n",
   //                              fileName.c_str());
    
   bool status = false;
    
   if (isPlanetarySourceInUse[SLP])
   {
      MessageInterface::ShowMessage
         ("Moderator::CreateSlpFile() SlpFile already created\n");
      status = true;
   }
   else
   {
      theDefaultSlpFile = new SlpFile(fileName);
      //MessageInterface::ShowMessage("Moderator::CreateSlpFile() SlpFile created\n");
        
      if (theDefaultSlpFile != NULL)
         status = true;
   }

   return status;
}


//------------------------------------------------------------------------------
// bool CreateDeFile(const Integer id, const std::string &fileName,
//                   Gmat::DeFileFormat format = Gmat::DE_BINARY)
//------------------------------------------------------------------------------
bool Moderator::CreateDeFile(Integer id, const std::string &fileName,
                             Gmat::DeFileFormat format)
{
   bool status = false;
   Gmat::DeFileType deFileType;
    
   if (isPlanetarySourceInUse[id])
   {
      MessageInterface::ShowMessage
         ("Moderator::CreateDeFile() DeFile already created\n");
      status = true;
   }
   else
   {
      switch (id)
      {
      case DE200:
         deFileType = Gmat::DE200;
         break;
      case DE405:
         deFileType = Gmat::DE405;
         break;
      default:
         MessageInterface::PopupMessage
            (Gmat::WARNING_,
             "Moderator::CreateDeFile() unsupported DE file type");
         return false;
      }
      
      #if DEBUG_PLANETARY_FILE
      MessageInterface::ShowMessage
         ("Moderator::CreateDeFile() creating DeFile. type=%d, "
          "fileName=%s, format=%d\n", deFileType, fileName.c_str(),
          format);
      #endif

      FILE *defile = fopen(fileName.c_str(), "rb");
      if (defile == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_,
             "Error opening DE file:%s. \n"
             "Please check file path. "
             "The next filetype in the list will be used.\n", fileName.c_str());
      }
      else
      {
         fclose(defile);
         
         try
         {
            theDefaultDeFile = new DeFile(deFileType, fileName, format);
        
            if (theDefaultDeFile != NULL)
               status = true;
         }
         catch (...)
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_,
                "Moderator::CreateDeFile() Error creating %s. "
                "The next filetype in the list will "
                "be created.\n", fileName.c_str());
         }
      }
   }
   return status;
}

// default objects
//------------------------------------------------------------------------------
// Spacecraft* GetDefaultSpacecraft()
//------------------------------------------------------------------------------
Spacecraft* Moderator::GetDefaultSpacecraft()
{
   StringArray &soConfigList = GetListOfConfiguredItems(Gmat::SPACECRAFT);
  
   if (soConfigList.size() > 0)
   {
      // return 1st Spacecraft
      SpaceObject *so = GetSpacecraft(soConfigList[0]);
      return (Spacecraft*)so;
   }
   else
   {
      // create Spacecraft
      return (Spacecraft*)CreateSpacecraft("Spacecraft", "DefaultSC");
   }
}

//------------------------------------------------------------------------------
// PropSetup* GetDefaultPropSetup()
//------------------------------------------------------------------------------
PropSetup* Moderator::GetDefaultPropSetup()
{
   StringArray &configList = GetListOfConfiguredItems(Gmat::PROP_SETUP);
   
   if (configList.size() > 0)
   {
      // return 1st PropSetup from the list
      return GetPropSetup(configList[0]);
   }
   else
   {
      // create PropSetup
      return CreateDefaultPropSetup("DefaultProp");
   }
}


//------------------------------------------------------------------------------
// Burn* GetDefaultBurn()
//------------------------------------------------------------------------------
Burn* Moderator::GetDefaultBurn()
{
   StringArray &configList = GetListOfConfiguredItems(Gmat::BURN);

   if (configList.size() > 0)
   {
      for (UnsignedInt i=0; i<configList.size(); i++)
         if (GetBurn(configList[i])->IsOfType("ImpulsiveBurn"))
            return GetBurn(configList[i]);
   }
   
   // create ImpulsiveBurn
   return CreateBurn("ImpulsiveBurn", "DefaultIB");
}


//------------------------------------------------------------------------------
// Subscriber* GetDefaultSubscriber()
//------------------------------------------------------------------------------
Subscriber* Moderator::GetDefaultSubscriber()
{
   StringArray &configList = GetListOfConfiguredItems(Gmat::SUBSCRIBER);
   
   if (configList.size() > 0)
   {
      // return 1st Subscriber from the list
      return GetSubscriber(configList[0]);
   }
   else
   {
      // create default ReportFile
      Subscriber *sub = CreateSubscriber("ReportFile", "DefaultReportFile");
      std::string scName = GetDefaultSpacecraft()->GetName();
      sub->SetStringParameter(sub->GetParameterID("Filename"),
                              "DefaultReportFile.txt");
      sub->SetStringParameter("Add", scName + ".A1ModJulian");
      sub->SetStringParameter("Add", scName + ".EarthMJ2000Eq.X");
      sub->SetStringParameter("Add", scName + ".EarthMJ2000Eq.Y");
      sub->SetStringParameter("Add", scName + ".EarthMJ2000Eq.Z");
      sub->SetStringParameter("Add", scName + ".EarthMJ2000Eq.VX");
      sub->SetStringParameter("Add", scName + ".EarthMJ2000Eq.VY");
      sub->SetStringParameter("Add", scName + ".EarthMJ2000Eq.VZ");
      sub->Activate(true);
      return sub;
   }
}

//------------------------------------------------------------------------------
// Solver* GetDefaultSolver()
//------------------------------------------------------------------------------
Solver* Moderator::GetDefaultSolver()
{
   StringArray &configList = GetListOfConfiguredItems(Gmat::SOLVER);
   
   if (configList.size() > 0)
   {
      // return 1st Burn from the list
      return GetSolver(configList[0]);
   }
   else
   {
      // create Solver
      return CreateSolver("DifferentialCorrector", "DefaultDC");
   }
}

//------------------------------------------------------------------------------
// StopCondition* CreateDefaultStopCondition()
//------------------------------------------------------------------------------
StopCondition* Moderator::CreateDefaultStopCondition()
{
   StopCondition *stopCond = NULL;
   Parameter *param;
   
   Spacecraft *sc = GetDefaultSpacecraft();
   std::string scName = sc->GetName();
   
   std::string epochVar = scName + ".A1ModJulian";
   std::string stopVar = scName + ".ElapsedSecs";

   #ifdef DEBUG_DEFAULT_MISSION
   MessageInterface::ShowMessage
      ("Moderator::CreateDefaultStopCondition() scName=%s, epochVar=%s, "
       "stopVar=%s\n", scName.c_str(), epochVar.c_str(), stopVar.c_str());
   #endif
   
   if (GetParameter(epochVar) == NULL)
   {
      param = CreateParameter("A1ModJulian", epochVar);
      param->SetRefObjectName(Gmat::SPACECRAFT, scName);
   }
   
   if (GetParameter(stopVar) == NULL)
   {
      param = CreateParameter("ElapsedSecs", stopVar);
      param->SetRefObjectName(Gmat::SPACECRAFT, scName);
   }
   
   std::string stopCondName = "StopOn" + stopVar;
   
   stopCond = CreateStopCondition("StopCondition", "StopOn" + stopVar);
   
   stopCond->SetStringParameter("EpochVar", epochVar);
   stopCond->SetStringParameter("StopVar", stopVar);
   stopCond->SetStringParameter("Goal", "8640.0");
   return stopCond;
}


//------------------------------------------------------------------------------
// Parameter* GetDefaultX()
//------------------------------------------------------------------------------
Parameter* Moderator::GetDefaultX()
{
   Spacecraft *sc = GetDefaultSpacecraft();
   Parameter* param = GetParameter(sc->GetName() + ".A1ModJulian");

   if (param == NULL)
   {
      param = CreateParameter("A1ModJulian", sc->GetName() + ".A1ModJulian");
      param->SetRefObjectName(Gmat::SPACECRAFT, sc->GetName());
   }
   
   return param;
}


//------------------------------------------------------------------------------
// Parameter* GetDefaultY()
//------------------------------------------------------------------------------
Parameter* Moderator::GetDefaultY()
{
   Spacecraft *sc = GetDefaultSpacecraft();
   Parameter* param = GetParameter(sc->GetName() + ".EarthMJ2000Eq.X");
   
   if (param == NULL)
   {
      param = CreateParameter("X", sc->GetName() + ".EarthMJ2000Eq.X");
      param->SetRefObjectName(Gmat::SPACECRAFT, sc->GetName());
   }
   
   return param;
}


// sandbox
//------------------------------------------------------------------------------
// void AddSolarSystemToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddSolarSystemToSandbox(Integer index)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddSolarSystemToSandbox() entered\n");
   #endif
   
   //SolarSystem *solarSys = theConfigManager->GetSolarSystemInUse();
   //sandboxes[index]->AddSolarSystem(solarSys);
   sandboxes[index]->AddSolarSystem(theSolarSystemInUse);

   // Add LibrationPoint and Barycenter objects
   StringArray cpNames = theConfigManager->GetListOfItems(Gmat::CALCULATED_POINT);

   CalculatedPoint *cp;
   for (Integer i=0; i<(Integer)cpNames.size(); i++)
   {
      cp = theConfigManager->GetCalculatedPoint(cpNames[i]);
      sandboxes[index]->AddObject(cp);
   }
}


//------------------------------------------------------------------------------
// void AddInternalCoordSystemToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddInternalCoordSystemToSandbox(Integer index)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddInternalCoordSystemToSandbox() entered.\n");
   #endif
   
   sandboxes[index]->SetInternalCoordSystem(theInternalCoordSystem);
   
}

//------------------------------------------------------------------------------
// void AddPublisherToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddPublisherToSandbox(Integer index)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddPublisherToSandbox() entered.\n");
   #endif
   
   thePublisher->UnsubscribeAll();
   sandboxes[index]->SetPublisher(thePublisher);
}

//------------------------------------------------------------------------------
// void AddCoordSystemToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddCoordSystemToSandbox(Integer index)
{
   CoordinateSystem *cs;
   StringArray csNames = theConfigManager->GetListOfItems(Gmat::COORDINATE_SYSTEM);
   
   for (Integer i=0; i<(Integer)csNames.size(); i++)
   {
      cs = theConfigManager->GetCoordinateSystem(csNames[i]);
      sandboxes[index]->AddObject(cs);
   }
}

//------------------------------------------------------------------------------
// void AddSpacecraftToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddSpacecraftToSandbox(Integer index)
{
   Spacecraft *sc;
   StringArray scNames = theConfigManager->GetListOfItems(Gmat::SPACECRAFT);

   for (Integer i=0; i<(Integer)scNames.size(); i++)
   {
      sc = (Spacecraft*)theConfigManager->GetSpacecraft(scNames[i]);
      #ifdef DEBUG_RUN
      MessageInterface::ShowMessage
         ("Moderator::AddSpacecraftToSandbox() sc[%d] = %s\n", i,
          sc->GetName().c_str());
      #endif
      sandboxes[index]->AddObject(sc);
   }

   Hardware *hw;
   StringArray hwNames = theConfigManager->GetListOfItems(Gmat::HARDWARE);

   for (Integer i=0; i<(Integer)hwNames.size(); i++)
   {
      hw = (Hardware*)theConfigManager->GetHardware(hwNames[i]);
      sandboxes[index]->AddObject(hw);
   }
}

//------------------------------------------------------------------------------
// void AddFormationToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddFormationToSandbox(Integer index)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddFormationToSandbox() entered\n");
   #endif
   
   Formation *form;
   StringArray formNames = theConfigManager->GetListOfItems(Gmat::FORMATION);

   for (Integer i=0; i<(Integer)formNames.size(); i++)
   {
      form = (Formation*)theConfigManager->GetSpacecraft(formNames[i]);
      sandboxes[index]->AddObject(form);
   }
}

//------------------------------------------------------------------------------
// void AddPropSetupToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddPropSetupToSandbox(Integer index)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddPropSetupToSandbox() entered\n");
   #endif
   
   PropSetup *propSetup;
   StringArray propSetupNames = theConfigManager->GetListOfItems(Gmat::PROP_SETUP);
    
   for (Integer i=0; i<(Integer)propSetupNames.size(); i++)
   {
      propSetup = theConfigManager->GetPropSetup(propSetupNames[i]);
      sandboxes[index]->AddObject(propSetup);
   }
}

//------------------------------------------------------------------------------
// void AddPropagatorToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddPropagatorToSandbox(Integer index)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddPropagatorToSandbox() entered\n");
   #endif
   
   Propagator *prop;
   StringArray propNames = theConfigManager->GetListOfItems(Gmat::PROPAGATOR);
    
   for (Integer i=0; i<(Integer)propNames.size(); i++)
   {
      prop = theConfigManager->GetPropagator(propNames[i]);
      sandboxes[index]->AddObject(prop);
   }
}

//------------------------------------------------------------------------------
// void AddForceModelToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddForceModelToSandbox(Integer index)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddPropagatorToSandbox() entered\n");
   #endif
   
   ForceModel *fm;
   StringArray fmNames = theConfigManager->GetListOfItems(Gmat::FORCE_MODEL);
    
   for (Integer i=0; i<(Integer)fmNames.size(); i++)
   {
      fm = theConfigManager->GetForceModel(fmNames[i]);
      sandboxes[index]->AddObject(fm);
   }
}

//------------------------------------------------------------------------------
// void AddBurnToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddBurnToSandbox(Integer index)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddBurnToSandbox() entered\n");
   #endif
   
   Burn *burn;
   StringArray burnNames = theConfigManager->GetListOfItems(Gmat::BURN);
    
   for (Integer i=0; i<(Integer)burnNames.size(); i++)
   {
      burn = theConfigManager->GetBurn(burnNames[i]);
      sandboxes[index]->AddObject(burn);
   }
}

//------------------------------------------------------------------------------
// void AddSolverToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddSolverToSandbox(Integer index)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddSolverToSandbox() entered\n");
   #endif
   
   Solver *solver;
   StringArray solverNames = theConfigManager->GetListOfItems(Gmat::SOLVER);
    
   for (Integer i=0; i<(Integer)solverNames.size(); i++)
   {
      solver = theConfigManager->GetSolver(solverNames[i]);
      sandboxes[index]->AddObject(solver);
   }
}

//------------------------------------------------------------------------------
// void AddSuscriberToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddSubscriberToSandbox(Integer index)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddSubscriberToSandbox() entered\n");
   #endif
   
   Subscriber *sub;
   StringArray subNames = theConfigManager->GetListOfItems(Gmat::SUBSCRIBER);
   for (Integer i=0; i<(Integer)subNames.size(); i++)
   {
      sub = theConfigManager->GetSubscriber(subNames[i]);
      sandboxes[index]->AddSubscriber(sub);
   }
}

//------------------------------------------------------------------------------
// void AddParameterToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddParameterToSandbox(Integer index)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddParameterToSandbox() entered\n");
   #endif
   
   Parameter *param;
   StringArray paramNames = theConfigManager->GetListOfItems(Gmat::PARAMETER);
   for (Integer i=0; i<(Integer)paramNames.size(); i++)
   {
      param = theConfigManager->GetParameter(paramNames[i]);
      sandboxes[index]->AddObject(param);
   }
}

//------------------------------------------------------------------------------
// void AddFunctionToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddFunctionToSandbox(Integer index)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddFunctionToSandbox() entered\n");
   #endif
   
   Function *fn;
   StringArray functionNames = theConfigManager->GetListOfItems(Gmat::FUNCTION);
   for (Integer i=0; i<(Integer)functionNames.size(); i++)
   {
      fn = theConfigManager->GetFunction(functionNames[i]);
      sandboxes[index]->AddObject(fn);
   }
}

//------------------------------------------------------------------------------
// void AddCommandToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddCommandToSandbox(Integer index)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddCommandToSandbox() entered\n");
   #endif
   
   GmatCommand *cmd = commands[index]->GetNext();

   if (cmd != NULL)
   {
      sandboxes[index]->AddCommand(cmd);
   }
}

//------------------------------------------------------------------------------
// void InitializeSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::InitializeSandbox(Integer index)
{
   sandboxes[index]->Initialize();
}

//------------------------------------------------------------------------------
// void ExecuteSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::ExecuteSandbox(Integer index)
{
   sandboxes[index]->Execute();
}

//---------------------------------
// private
//---------------------------------

//------------------------------------------------------------------------------
// Moderator()
//------------------------------------------------------------------------------
/*
 * Constructor
 */
//------------------------------------------------------------------------------
Moderator::Moderator()
{
//    isInitialized = false;
   isRunReady = false;
   showFinalState = false;
   theDefaultSolarSystem = NULL;
   theSolarSystemInUse = NULL;
   theInternalCoordSystem = NULL;
   theDefaultSlpFile = NULL;
   theDefaultDeFile = NULL;
   runState = Gmat::IDLE;
   
   sandboxes.reserve(Gmat::MAX_SANDBOX);
   commands.reserve(Gmat::MAX_SANDBOX);

   // create at least 1 Sandbox and Command
   sandboxes.push_back(new Sandbox());
   commands.push_back(new NoOp());
}

//------------------------------------------------------------------------------
// ~Moderator()
//------------------------------------------------------------------------------
Moderator::~Moderator()
{
}

