//$Id$
//------------------------------------------------------------------------------
//                                 Moderator
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
// Developed further jointly by NASA/GSFC, Thinking Systems, Inc., and 
// Schafer Corp., under AFRL NOVA Contract #FA945104D03990003
//
// Author: Linda Jun
// Created: 2003/08/25
// Modified:  Dunn Idle (added MRPs)
// Date:      2010/08/24
//
/**
 * Implements operations of the GMAT executive.  It is a singleton class -
 * only one instance of this class can be created.
 */
//------------------------------------------------------------------------------

#include "Moderator.hpp"

// factories
#include "AssetFactory.hpp"
#include "AtmosphereFactory.hpp"
#include "AttitudeFactory.hpp"
#include "AxisSystemFactory.hpp"
#include "BurnFactory.hpp"
#include "CelestialBodyFactory.hpp"
#include "CommandFactory.hpp"
#include "CoordinateSystemFactory.hpp"
#include "ODEModelFactory.hpp"
#include "FunctionFactory.hpp"
#include "HardwareFactory.hpp"
#include "ParameterFactory.hpp"
#include "PhysicalModelFactory.hpp"
#include "PropagatorFactory.hpp"
#include "PropSetupFactory.hpp"
#include "SolverFactory.hpp"
#include "SpacecraftFactory.hpp"
#include "StopConditionFactory.hpp"
#include "SubscriberFactory.hpp"
#include "CalculatedPointFactory.hpp"
#include "MathFactory.hpp"
#include "Interface.hpp"
#include "XyPlot.hpp"

#include "NoOp.hpp"
#include "GravityField.hpp"
#include "CalculatedPoint.hpp"
#include "TimeSystemConverter.hpp"  // for SetLeapSecsFileReader(), SetEopFile()
#include "BodyFixedAxes.hpp"        // for SetEopFile(), SetCoefficientsFile()
#include "ObjectReferencedAxes.hpp"
#include "MessageInterface.hpp"
#include "CommandUtil.hpp"          // for GetCommandSeq()
#include "StringTokenizer.hpp"      // for StringTokenizer
#include "StringUtil.hpp"           // for GmatStringUtil::
#include "FileUtil.hpp"             // for GmatFileUtil::
#include <algorithm>                // for sort(), set_difference()
#include <ctime>                    // for clock()



//#define DEBUG_INITIALIZE 1
//#define DEBUG_FINALIZE 1
//#define DEBUG_INTERPRET 1
//#define DEBUG_RUN 1
//#define DEBUG_CREATE_COORDSYS 1
//#define DEBUG_CREATE_RESOURCE 2
//#define DEBUG_CREATE_CALC_POINT
//#define DEBUG_CREATE_PARAMETER 1
//#define DEBUG_CREATE_EPHEMFILE 1
//#define DEBUG_PARAMETER_REF_OBJ 1
//#define DEBUG_DEFAULT_COMMAND 1
//#define DEBUG_COMMAND_APPEND 1
//#define DEBUG_COMMAND_DELETE 1
//#define DEBUG_RENAME 1
//#define DEBUG_REMOVE 1
//#define DEBUG_DEFAULT_MISSION 2
//#define DEBUG_MULTI_STOP 2
//#define DEBUG_USER_INTERRUPT 1
//#define DEBUG_LOOKUP_RESOURCE 1
//#define DEBUG_SEQUENCE_CLEARING 1
//#define DEBUG_CONFIG 1
//#define DEBUG_CREATE_VAR 1
//#define DEBUG_GMAT_FUNCTION 2
//#define DEBUG_OBJECT_MAP 1
//#define DEBUG_FIND_OBJECT 1
//#define DEBUG_ADD_OBJECT 1
//#define DEBUG_SOLAR_SYSTEM 1
//#define DEBUG_SOLAR_SYSTEM_IN_USE 1
//#define DEBUG_CREATE_BODY
//#define DEBUG_PLUGIN_REGISTRATION
//#define DEBUG_MATLAB
//#define DEBUG_CCSDS_EPHEMERIS

// Currently we can't use DataFile for 2011a release so commented out
// Actually we want to put this flag in BuildEnv.mk but it is very close to
// release so added it here and EphemerisFile.cpp
//#define __USE_DATAFILE__

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

// @note If this is enabled, Bug1430-Func_GFuncInsideCFlow.script leaves
// no memory tracks but AssigningWholeObjects.script crashes when exiting
//#define __ENABLE_CLEAR_UNMANAGED_FUNCTIONS__
//#define __CREATE_DEFAULT_BC__
//#define __SHOW_FINAL_STATE__
//#define __DISABLE_SOLAR_SYSTEM_CLONING__

//---------------------------------
// static data
//---------------------------------
Moderator* Moderator::instance = NULL;
ScriptInterpreter* Moderator::theUiInterpreter = NULL;
ScriptInterpreter* Moderator::theScriptInterpreter = NULL;


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
// bool Initialize(const std::string &startupFile, bool fromGui = false)
//------------------------------------------------------------------------------
bool Moderator::Initialize(const std::string &startupFile, bool fromGui)
{
   #if DEBUG_INITIALIZE
   MessageInterface::ShowMessage
      ("Moderator::Initialize() entered, startupFile='%s', fromGui=%d\n",
       startupFile.c_str(), fromGui);
   #endif
   
   isFromGui = fromGui;
   
   try
   {
      // We don't want to write before startup file is read so commented out
      //MessageInterface::ShowMessage("Moderator is reading startup file...\n");
      
      // Read startup file, Set Log file
      theFileManager = FileManager::Instance();
      theFileManager->ReadStartupFile(startupFile);
      
      MessageInterface::ShowMessage("Moderator is creating core engine...\n");
      
      // Set trace flag globally
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->SetShowTrace(false);
      #endif
      
      // Create Managers
      theFactoryManager = FactoryManager::Instance();
      
      #if DEBUG_INITIALIZE
      MessageInterface::ShowMessage
         (".....created  (%p)theFactoryManager\n", theFactoryManager);
      #endif
      
      theConfigManager = ConfigManager::Instance();
      
      #if DEBUG_INITIALIZE
      MessageInterface::ShowMessage
         (".....created  (%p)theConfigManager\n", theConfigManager);
      #endif
      
      // Register factories
      theFactoryManager->RegisterFactory(new AtmosphereFactory());
      theFactoryManager->RegisterFactory(new AttitudeFactory());
      theFactoryManager->RegisterFactory(new AxisSystemFactory());
      theFactoryManager->RegisterFactory(new BurnFactory());
      theFactoryManager->RegisterFactory(new CalculatedPointFactory());
      theFactoryManager->RegisterFactory(new CommandFactory());
      theFactoryManager->RegisterFactory(new CoordinateSystemFactory());
      theFactoryManager->RegisterFactory(new ODEModelFactory());
      theFactoryManager->RegisterFactory(new FunctionFactory());
      theFactoryManager->RegisterFactory(new HardwareFactory());
      theFactoryManager->RegisterFactory(new MathFactory());
      theFactoryManager->RegisterFactory(new ParameterFactory());
      theFactoryManager->RegisterFactory(new PhysicalModelFactory());
      theFactoryManager->RegisterFactory(new PropagatorFactory());
      theFactoryManager->RegisterFactory(new PropSetupFactory());
      theFactoryManager->RegisterFactory(new SolverFactory());
      theFactoryManager->RegisterFactory(new SpacecraftFactory());
      theFactoryManager->RegisterFactory(new StopConditionFactory());
      theFactoryManager->RegisterFactory(new SubscriberFactory());

      theFactoryManager->RegisterFactory(new CelestialBodyFactory());
      theFactoryManager->RegisterFactory(new AssetFactory());
            
      // Create publisher
      thePublisher = Publisher::Instance();
      
      #if DEBUG_INITIALIZE
      MessageInterface::ShowMessage
         (".....created  (%p)thePublisher...\n", thePublisher);
      #endif
      
      // Create script interpreter
      theScriptInterpreter = ScriptInterpreter::Instance();      
      
      #if DEBUG_INITIALIZE
      MessageInterface::ShowMessage
         (".....created  (%p)theScriptInterpreter\n", theScriptInterpreter);
      #endif
      
      LoadPlugins();
      
      // Create default SolarSystem
      theDefaultSolarSystem = CreateSolarSystem("DefaultSolarSystem");
      
      #if DEBUG_INITIALIZE
      MessageInterface::ShowMessage
         (".....created  (%p)theDefaultSolarSystem\n", theDefaultSolarSystem);
      #endif
      
      theConfigManager->SetDefaultSolarSystem(theDefaultSolarSystem);
      
      // Create solar system in use
      /// @note: If the solar system can be configured by name, add it to the
      ///        ConfigManager by calling CreateSolarSystem().
      ///        Until then, just use solar system name as "SolarSystem"
      
      CreateSolarSystemInUse();
      
      // Create other files in use
      CreatePlanetaryCoeffFile();
      CreateTimeFile();
      
      // Create at least 1 Sandbox and NoOp Command
      Sandbox *sandbox = new Sandbox();
      GmatCommand *noOp = new NoOp();
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add(sandbox, "Sandbox", "Moderator::Initialize()", "");
      MemoryTracker::Instance()->Add(noOp, "NoOP", "Moderator::Initialize()", "");
      #endif
      
      sandboxes.push_back(sandbox);
      commands.push_back(noOp);
      
      #if DEBUG_INITIALIZE
      MessageInterface::ShowMessage
         (".....created  (%p)Sandbox 1\n", sandboxes[0]);
      MessageInterface::ShowMessage
         (".....created  (%p)%s\n", commands[0], commands[0]->GetTypeName().c_str());
      #endif
      
      // set objectMapInUse (loj: 2008.05.23)
      objectMapInUse = theConfigManager->GetObjectMap();
      
      #ifdef DEBUG_OBJECT_MAP
      MessageInterface::ShowMessage
         ("Moderator::Initialize() objectMapInUse was set to the "
          "configuration map <%p>\n", objectMapInUse);
      #endif
      
      if (isFromGui)
         CreateDefaultMission();
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "Error occurred during initialization: " +
          e.GetFullMessage());
      return false;
   }
   catch (...)
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "Unknown Error occurred during initialization");
      return false;
   }
   
   // Let's put current time out
   time_t rawtime;
   struct tm * timeinfo;
   
   time(&rawtime);
   timeinfo = localtime(&rawtime);
   char timestr[80];
   strftime(timestr, 80, "%Y-%m-%d %H:%M:%S",timeinfo);
   
   MessageInterface::ShowMessage
      ("%s GMAT Moderator successfully created core engine\n", timestr);
   
   // Check if MatlabInterface is required
   if (GmatGlobal::Instance()->GetMatlabMode() == GmatGlobal::NO_MATLAB)
   {
      MessageInterface::ShowMessage
         ("*** Use of MATLAB is disabled from the gmat_startup_file\n");
   }
   else
   {
      try
      {
         theMatlabInterface = theFactoryManager->CreateInterface("MatlabInterface", "MI");
         #ifdef DEBUG_MATLAB
         MessageInterface::ShowMessage
            ("Moderator::Initialize() theMatlabInterface=<%p>\n", theMatlabInterface);
         #endif
         // Check if MATLAB is installed
         // Do not override matlab setting in the startup file since
         // GmatFileUtil::IsAppInstalled() not implemented for all platforms
         std::string appLoc;
         bool hasMatlab = GmatFileUtil::IsAppInstalled("MATLAB", appLoc);
         // Since GmatFileUtil::IsAppInstalled() is not complete for all platforms,
         // assume there is MATLAB for now. (LOJ: 2010.04.07)
         hasMatlab = true;
         if (hasMatlab)
         {
            #ifdef DEBUG_MATLAB
            MessageInterface::ShowMessage
               ("*** MATLAB is installed in '%s'\n", appLoc.c_str());
            #endif
            GmatGlobal::Instance()->SetMatlabAvailable(true);
         }
         else
         {
            #ifdef DEBUG_MATLAB
            MessageInterface::ShowMessage("*** MATLAB is not installed\n");
            #endif
         }
      }
      catch (BaseException &be)
      {
         MessageInterface::ShowMessage(be.GetFullMessage());
      }
   }

   // Set MatlabInterface run mode, that is SINGLE_USE or SHARED MATLAB engine.
   if (theMatlabInterface == NULL)
      GmatGlobal::Instance()->SetMatlabMode(GmatGlobal::NO_MATLAB);
   else
      theMatlabInterface->
         SetIntegerParameter("MatlabMode",
                             GmatGlobal::Instance()->GetMatlabMode());
   #if DEBUG_INITIALIZE
   MessageInterface::ShowMessage("Moderator::Initialize() returning true\n");
   #endif
   
   return true;;
} // Initialize()



//------------------------------------------------------------------------------
// void Finalize()
//------------------------------------------------------------------------------
/*
 * Finalizes the system by closing all opened files by deleting objects.
 */
//------------------------------------------------------------------------------
void Moderator::Finalize()
{
   MessageInterface::ShowMessage("Moderator is deleting core engine...\n");
   
   #if DEBUG_FINALIZE > 0
   MessageInterface::ShowMessage("Moderator::Finalize() entered\n");
   #endif
   
   #if DEBUG_FINALIZE > 1
   //Notes: Do not use %s for command string, it may crash when it encounters
   // comment with % in the scripts
   GmatCommand *cmd = GetFirstCommand();
   MessageInterface::ShowMessage(GmatCommandUtil::GetCommandSeqString(cmd));
   MessageInterface::ShowMessage(GetScript());
   #endif
   
   #if DEBUG_FINALIZE > 0
   MessageInterface::ShowMessage
      (".....Moderator::Finalize() deleting (%p)theFileManager\n", theFileManager);
   MessageInterface::ShowMessage
      (".....Moderator::Finalize() deleting (%p)theEopFile\n", theEopFile);
   MessageInterface::ShowMessage
      (".....Moderator::Finalize() deleting (%p)theItrfFile\n", theItrfFile);
   MessageInterface::ShowMessage
      (".....Moderator::Finalize() deleting (%p)theLeapSecsFile\n", theLeapSecsFile);
   #endif
   
   delete theFileManager;
   delete theEopFile;
   delete theItrfFile;
   delete theLeapSecsFile;
   if (theMatlabInterface != NULL)
      delete theMatlabInterface;
   
   theFileManager = NULL;
   theEopFile = NULL;
   theItrfFile = NULL;
   theLeapSecsFile = NULL;
   
   //clear resource and command sequence
   try
   {
      #if DEBUG_FINALIZE > 0
      MessageInterface::ShowMessage(".....clearing resource\n");
      MessageInterface::ShowMessage(".....clearing command sequence\n");
      #endif
      
      // Clear command sequence before resource (loj: 2008.07.10)
      // only 1 sandbox for now
      ClearCommandSeq(false, false);
      ClearResource();
      
      // Delete the plugin resource data
      for (UnsignedInt i = 0; i < userResources.size(); ++i)
         delete userResources[i];
      userResources.clear();

      // Close out the plug-in libraries
      std::map<std::string, DynamicLibrary*>::iterator i;
      for (i = userLibraries.begin(); i != userLibraries.end(); ++i)
      {
         delete i->second;
         i->second = NULL;
         #ifdef DEBUG_PLUGINS
            MessageInterface::ShowMessage("Closed %s.\n", i->first.c_str());
         #endif
      }

      
//       #if DEBUG_FINALIZE > 0
//       MessageInterface::ShowMessage
//          (".....deleting (%p)theGuiInterpreter\n", theGuiInterpreter);
//       #endif
//       delete theGuiInterpreter;
//       theGuiInterpreter = NULL;
      
      //delete theFactoryManager; (private destructor)
      //delete theConfigManager; (private destructor)
      //delete theScriptInterpreter; (private destructor)
      //delete theGuiInterpreter; (private destructor)
      #if DEBUG_FINALIZE > 0
      MessageInterface::ShowMessage
         (".....Moderator::Finalize() deleting (%p)thePublisher\n",
          thePublisher);
      #endif      
      delete thePublisher;
      
      // delete solar systems
      #if DEBUG_FINALIZE > 0
      MessageInterface::ShowMessage
         (".....Moderator::Finalize() deleting (%p)theDefaultSolarSystem\n",
          theDefaultSolarSystem);
      #endif      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (theDefaultSolarSystem, theDefaultSolarSystem->GetName(),
          "Moderator::Finalize()");
      #endif
      delete theDefaultSolarSystem;
      theDefaultSolarSystem = NULL;
      
      if (theSolarSystemInUse != NULL)
      {
         #if DEBUG_FINALIZE > 0
         MessageInterface::ShowMessage
            (".....Moderator::Finalize() deleting (%p)theSolarSystemInUse\n",
             theSolarSystemInUse);
         #endif
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (theSolarSystemInUse, theSolarSystemInUse->GetName(),
             "Moderator::Finalize()");
         #endif
         delete theSolarSystemInUse;
         theSolarSystemInUse = NULL;
      }
      
      // delete internal coordinate system
      if (theInternalCoordSystem != NULL)
      {
         #if DEBUG_FINALIZE > 0
         MessageInterface::ShowMessage
            (".....Moderator::Finalize() deleting (%p)theInternalCoordSystem\n",
             theInternalCoordSystem);
         #endif
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (theInternalCoordSystem, theInternalCoordSystem->GetName(),
             "Moderator::Finalize()");
         #endif
         delete theInternalCoordSystem;
         theInternalCoordSystem = NULL;
      }
      
      // Delete unmanaged functions (LOJ: 2009.03.24)
      // This causes crash on Func_MatlabObjectPassingCheck.script
      // so disable until it is fully tested (LOJ: 2009.04.08)
      #ifdef __ENABLE_CLEAR_UNMANAGED_FUNCTIONS__
      #if DEBUG_FINALIZE > 0
      MessageInterface::ShowMessage
         (".....Moderator::Finalize() deleting %d unmanaged functions\n",
          unmanagedFunctions.size());
      #endif
      for (UnsignedInt i=0; i<unmanagedFunctions.size(); i++)
      {
         GmatBase *func = (GmatBase*)(unmanagedFunctions[i]);
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (func, func->GetName(), "Moderator::Finalize()",
             "deleting unmanaged function");
         #endif
         delete func;
         func = NULL;
      }
      unmanagedFunctions.clear();
      #endif
      
      // delete Sandbox (only 1 Sandbox for now)
      #if DEBUG_FINALIZE > 0
      MessageInterface::ShowMessage
         (".....Moderator::Finalize() deleting (%p)sandbox 1\n", sandboxes[0]);
      #endif
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (sandboxes[0], "Sandbox", "Moderator::Finalize()");
      #endif
      delete sandboxes[0];
      commands[0] = NULL;
      sandboxes[0] = NULL;
      commands.clear();
      sandboxes.clear();
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   #ifdef DEBUG_MEMORY
   StringArray tracks = MemoryTracker::Instance()->GetTracks(true, false);
   MessageInterface::ShowMessage
      ("===> There are %d memory tracks after Finalize\n", tracks.size());
   for (UnsignedInt i=0; i<tracks.size(); i++)
      MessageInterface::ShowMessage("%s\n", tracks[i].c_str());
   #endif
   
   #if DEBUG_FINALIZE > 0
   MessageInterface::ShowMessage("Moderator::Finalize() exiting\n");
   #endif
} // Finalize()


//------------------------------------------------------------------------------
// void SetRunReady(bool flag = false)
//------------------------------------------------------------------------------
void Moderator::SetRunReady(bool flag)
{
   isRunReady = flag;
}


//------------------------------------------------------------------------------
// Interface* GetMatlabInterface()
//------------------------------------------------------------------------------
/**
 * Returns MatlabInterface pointer.
 */ 
//------------------------------------------------------------------------------
Interface* Moderator::GetMatlabInterface()
{
   return theMatlabInterface;
}


//------------------------------------------------------------------------------
// bool OpenMatlabEngine()
//------------------------------------------------------------------------------
bool Moderator::OpenMatlabEngine()
{
   #ifdef DEBUG_MATLAB
   MessageInterface::ShowMessage
      ("Moderator::OpenMatlabEngine() theMatlabInterface=<%p>\n",
       theMatlabInterface);
   #endif
   
   if (theMatlabInterface != NULL)
   {
      #ifdef DEBUG_MATLAB
      MessageInterface::ShowMessage
         ("Moderator::OpenMatlabEngine() calling theMatlabInterface->Open()\n");
      #endif
      
      if (theMatlabInterface->Open() == 1)
         return true;
      else
         return false;
   }
   else
   {
      #ifdef DEBUG_MATLAB
      MessageInterface::ShowMessage
         ("Moderator::OpenMatlabEngine() theMatlabInterface is NULL, so returning false\n");
      #endif
      return false;
   }
}


//------------------------------------------------------------------------------
// bool CloseMatlabEngine()
//------------------------------------------------------------------------------
bool Moderator::CloseMatlabEngine()
{
   #ifdef DEBUG_MATLAB
   MessageInterface::ShowMessage
      ("Moderator::CloseMatlabEngine() theMatlabInterface=<%p>\n",
       theMatlabInterface);
   #endif
   
   if (theMatlabInterface != NULL)
   {
      #ifdef DEBUG_MATLAB
      MessageInterface::ShowMessage
         ("Moderator::CloseMatlabEngine() calling theMatlabInterface->Close()\n");
      #endif
      
      if (theMatlabInterface->Close() == 1)
         return true;
      else
         return false;
   }
   else
   {
      #ifdef DEBUG_MATLAB
      MessageInterface::ShowMessage
         ("Moderator::OpenMatlabEngine() theMatlabInterface is NULL, so returning false\n");
      #endif
      return false;
   }
}


//---------------------------- Plug-in modules ---------------------------------

//------------------------------------------------------------------------------
// void LoadPlugins()
//------------------------------------------------------------------------------
/**
 * Method that loads in the plug-in libraries listed in a user's startup file.
 * 
 * The GMAT startup file may list one or more plug-in libraries by name.  This 
 * method retrieves the list of libraries, and loads them into GMAT.
 * 
 * @note The current code looks for exactly one library -- the VF13ad library --
 *       and loads it into GMAT if found.  The generic updates for any user 
 *       library will be added in a later build.
 */ 
//------------------------------------------------------------------------------
void Moderator::LoadPlugins()
{
   StringArray pluginList = theFileManager->GetPluginList();

   // This is done for all plugins in the startup file
   for (StringArray::const_iterator i = pluginList.begin(); 
         i != pluginList.end(); ++i)
   {

      #ifndef __WIN32__
   
         #ifdef DEBUG_PLUGIN_REGISTRATION
            MessageInterface::ShowMessage(
                  "*** Loading dynamic library \"%s\": ", i->c_str());
         #endif
         LoadAPlugin(*i);

      #else
         
         #ifdef DEBUG_PLUGIN_REGISTRATION
            MessageInterface::ShowMessage(
                  "*** Loading dynamic library \"%s\": ", i->c_str());
         #endif
         LoadAPlugin(*i);
        
      #endif
   }
   
   if (theUiInterpreter != NULL)
      theUiInterpreter->BuildCreatableObjectMaps();
   theScriptInterpreter->BuildCreatableObjectMaps();
}

//------------------------------------------------------------------------------
// void LoadAPlugin(std::string pluginName)
//------------------------------------------------------------------------------
/**
 * Method that loads a plug-in library into memory.
 * 
 * This method loads a plug-in library into memory, and retrieves and registers 
 * any Factories contained in that plug-in.  If the library is not found, this 
 * method just returns.
 * 
 * @param pluginName The file name for the plug-in library.  The name should not 
 *                   include the file extension (e.g. ".ddl" or ".so").
 */ 
//------------------------------------------------------------------------------
void Moderator::LoadAPlugin(std::string pluginName)
{
   // Set platform specific slash style
   #ifdef DEBUG_PLUGIN_REGISTRATION
      MessageInterface::ShowMessage("Input plugin name: \"%s\"\n", pluginName.c_str());
   #endif

   char fSlash = '/';
   char bSlash = '\\';
   char osSlash = '\\';       // Default to Windows, but change if *nix

   #ifndef _WIN32
      osSlash = '/';          // Mac or Linux
   #endif

   #ifdef DEBUG_PLUGIN_REGISTRATION
      MessageInterface::ShowMessage("OS slash is \"%c\"\n", osSlash);
   #endif

   for (UnsignedInt i = 0; i < pluginName.length(); ++i)
   {
      if ((pluginName[i] == fSlash) || (pluginName[i] == bSlash))
         pluginName[i] = osSlash;
   }

   #ifdef DEBUG_PLUGIN_REGISTRATION
      MessageInterface::ShowMessage("Used plugin name:  \"%s\"\n", pluginName.c_str());
   #endif

   DynamicLibrary *theLib = LoadLibrary(pluginName);

   if (theLib != NULL)
   {
      Integer fc = theLib->GetFactoryCount();

      if (fc > 0)
      {
         // Do the GMAT factory dance
         #ifdef DEBUG_PLUGIN_REGISTRATION
            MessageInterface::ShowMessage(
               "Library %s contains %d %s.\n", pluginName.c_str(), fc,
               ( fc==1 ? "factory" : "factories"));
         #endif
            
         // Now pass factories to the FactoryManager
         Factory *newFactory = NULL;
         for (Integer i = 0; i < fc; ++i)
         {
            newFactory = theLib->GetGmatFactory(i);
            if (newFactory != NULL)
            {
               if (theFactoryManager->RegisterFactory(newFactory) == false)
                  MessageInterface::ShowMessage(
                        "Factory %d in library %s failed to register with the "
                        "Factory Manager.\n", i, pluginName.c_str());
               else
               {
                  #ifdef DEBUG_PLUGIN_REGISTRATION
                     MessageInterface::ShowMessage(
                        "Factory %d in library %s is now registered with the "
                        "Factory Manager!\n", i, pluginName.c_str());

                     StringArray facts = newFactory->GetListOfCreatableObjects();

                     MessageInterface::ShowMessage(
                           "The new factory creates these objects types:\n");

                     for (UnsignedInt f = 0; f < facts.size(); ++f)
                        MessageInterface::ShowMessage("   %s\n",
                              facts[f].c_str());
                  #endif
               }
            } 
            else
               MessageInterface::ShowMessage(
                     "Factory %d in library %s was not constructed; a NULL "
                     "pointer was returned instead.\n", i, pluginName.c_str());
         }
      }
      else
         MessageInterface::PutMessage(
            "*** Library \"" + pluginName + "\" does not contain a factory\n");
      
      // Test to see if there might be TriggerManagers
      Integer triggerCount = theLib->GetTriggerManagerCount();
      #ifdef DEBUG_PLUGIN_REGISTRATION
         MessageInterface::ShowMessage(
            "Library %s contains %d %s.\n", pluginName.c_str(), triggerCount,
            ( triggerCount == 1 ? "TriggerManager" : "TriggerManagers"));
      #endif

      for (Integer i = 0; i < triggerCount; ++i)
      {
         TriggerManager *tm = theLib->GetTriggerManager(i);
         triggerManagers.push_back(tm);

         #ifdef DEBUG_PLUGIN_REGISTRATION
            MessageInterface::ShowMessage(
               "   TriggerManager %d of type %s is now registered.\n", i,
               tm->GetTriggerTypeString().c_str());
         #endif
      }

      // Check for new GUI elements
      Integer menuCount = theLib->GetMenuEntryCount();
      #ifdef DEBUG_PLUGIN_REGISTRATION
         MessageInterface::ShowMessage(
            "Library %s contains %d %s.\n", pluginName.c_str(), menuCount,
            ( menuCount == 1 ? "menu entry" : "menu entries"));
      #endif

      for (Integer i = 0; i < menuCount; ++i)
      {
         Gmat::PluginResource* res = theLib->GetMenuEntry(i);
         if (res != NULL)
         {
            #ifdef DEBUG_PLUGIN_REGISTRATION
               MessageInterface::ShowMessage("Adding user resource node:\n"
                     "   Name: %s\n   Parent: %s\n   type: %d\n"
                     "   subtype: %s\n", res->nodeName.c_str(),
                     res->parentNodeName.c_str(), res->type,
                     res->subtype.c_str());
            #endif
            userResources.push_back(res);
         }

         #ifdef DEBUG_PLUGIN_REGISTRATION
            MessageInterface::ShowMessage(
               "   Menu entry %d for node %s is now registered.\n", i,
               res->nodeName.c_str());
         #endif
      }
   }
   else
      MessageInterface::PutMessage(
         "*** Unable to load the dynamic library \"" + pluginName + "\"\n");
}

//------------------------------------------------------------------------------
// Dynamic library specific code
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DynamicLibrary *LoadLibrary(const std::string &libraryName)
//------------------------------------------------------------------------------
/**
 * Loads a dynamic library into memory.
 * 
 * Creates a DynamicLibrary object and uses that instance to provide the 
 * interfaces GMAT uses to load a dynamic library into memory.  If the library 
 * is not found or if it could not be loaded, a message is written out stating 
 * that the library did not open.
 * 
 * @param libraryName The file name for the plug-in library.  The name should 
 *                    not include the file extension.
 * 
 * @return The DynamicLibrary object that supplies the library interfaces, or a
 *         NULL pointer if the library did not load.
 */ 
//------------------------------------------------------------------------------
DynamicLibrary *Moderator::LoadLibrary(const std::string &libraryName)
{
   DynamicLibrary *theLib = new DynamicLibrary(libraryName);
   if (theLib->LoadDynamicLibrary())
   {
      userLibraries[libraryName] = theLib;
   }
   else
   {
      MessageInterface::ShowMessage("*** Library \"%s\" did not open.\n",
            libraryName.c_str());
      delete theLib;
      theLib = NULL;
   }
   
   return theLib;
}

//------------------------------------------------------------------------------
// bool IsLibraryLoaded(const std::string &libName)
//------------------------------------------------------------------------------
/**
 * Method that checks to see if a specified library has been loaded.
 * 
 * @param libName The name of the library.
 * 
 * @return true if the library has been loaded, false if not.
 */ 
//------------------------------------------------------------------------------
bool Moderator::IsLibraryLoaded(const std::string &libName)
{
   bool retval = false;
   if (userLibraries.find(libName) != userLibraries.end())
      retval = true;
   
   return retval;
}

//------------------------------------------------------------------------------
// void (*GetDynamicFunction(const std::string &funName, 
//                           const std::string &libraryName))()
//------------------------------------------------------------------------------
/**
 * Retrieves a specified function from a specified library.
 * 
 * @param funName The name of the requested function.
 * @param libraryName Teh name of the library that contains the function.
 * 
 * @return A function pointer for the specified function, or NULL if the 
 *         function is not found.  The returned function pointer has the 
 *         signature
 *             void (*)()
 *         and should be cast to the correct signature.
 */ 
//------------------------------------------------------------------------------
void (*Moderator::GetDynamicFunction(const std::string &funName, 
      const std::string &libraryName))()
{
   void (*theFunction)() = NULL;
   if (IsLibraryLoaded(libraryName))
      theFunction = userLibraries[libraryName]->GetFunction(funName);
   return theFunction;
}

//------------------------ End of Plug-in Code ---------------------------------


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
ScriptInterpreter* Moderator::GetUiInterpreter()
{
   return theUiInterpreter;
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
void Moderator::SetUiInterpreter(ScriptInterpreter *uiInterp)
{
   theUiInterpreter = uiInterp;
   theUiInterpreter->BuildCreatableObjectMaps();
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


//------------------------------------------------------------------------------
// void SetInterpreterMapAndSS(Interpreter *interp)
//------------------------------------------------------------------------------
/**
 * Sets Interpreter ObjectMap and SolarSystem to current pointers in use.
 * 
 * @param interp The Interpreter that is setup. 
 */
//------------------------------------------------------------------------------
void Moderator::SetInterpreterMapAndSS(Interpreter *interp)
{
   interp->SetObjectMap(objectMapInUse, true);
   interp->SetSolarSystemInUse(theSolarSystemInUse);
}

//----- object finding
//------------------------------------------------------------------------------
// void SetObjectMap(ObjectMap *objMap)
//------------------------------------------------------------------------------
void Moderator::SetObjectMap(ObjectMap *objMap)
{
   if (objMap != NULL)
   {
      objectMapInUse = objMap;
      
      #ifdef DEBUG_OBJECT_MAP
      MessageInterface::ShowMessage
         ("Moderator::SetObjectMap() objectMapInUse was set to input objMap <%p>\n",
          objMap);
      ShowObjectMap("Moderator::SetObjectMap() Here is the object map in use");
      #endif
   }
}


//------------------------------------------------------------------------------
// void SetObjectManageOption(Integer option)
//------------------------------------------------------------------------------
/*
 * Sets object manage option. Usually objects created inside GmatFunction uses
 * object maps passed to Moderator. All objects created in the main sequence and
 * through the GUI are managed through Configuration Manager.
 *
 * @param <option>  0, if object is not managed
 *                  1, if configuration object map is used for finding objects
 *                     and adding new objects (default)
 *                  2, if function object map is used for finding objects and
 *                     adding new objects including automatic objects
 */
//------------------------------------------------------------------------------
void Moderator::SetObjectManageOption(Integer option)
{
   #ifdef DEBUG_OBJECT_MANAGE
   MessageInterface::ShowMessage
      ("Moderator::SetObjectManageOption() option = %d\n", option);
   #endif
   objectManageOption = option;
}


//------------------------------------------------------------------------------
// Integer GetObjectManageOption()
//------------------------------------------------------------------------------
/*
 * returns object manage option.
 */
//------------------------------------------------------------------------------
Integer Moderator::GetObjectManageOption()
{
   return objectManageOption;
}


//------------------------------------------------------------------------------
// bool ResetObjectPointer(GmatBase *newObj, const std::string &name)
//------------------------------------------------------------------------------
/*
 * Sets configured object pointer with new pointer.
 *
 * @param  newObj  New object pointer
 * @param  name  Name of the configured object to be reset
 *
 * @return  true if pointer was reset, false otherwise
 */
//------------------------------------------------------------------------------
void Moderator::ResetObjectPointer(ObjectMap *objMap, GmatBase *newObj,
                                   const std::string &name)
{
   #if DEBUG_RESET_OBJECT
   MessageInterface::ShowMessage
      ("Moderator::ResetObjectPointer() entered, objMap=<%p>, newObj=<%p>, "
       "name='%s'\n", objMap, obj, newObj, name.c_str());
   #endif
   
   if (objMap->find(name) != objMap->end())
   {
      GmatBase *mapObj = (*objMap)[name];
      if (mapObj->GetName() == name)
      {
         // We want to replace if it has the same sub type
         if (newObj->IsOfType(mapObj->GetTypeName()))
         {
            #if DEBUG_RESET_OBJECT
            MessageInterface::ShowMessage
               ("   Replacing mapObj=<%p> with newObj=<%p>\n", mapObj, newObj);
            #endif
            (*objMap)[name] = newObj;
         }
      }
   }
   
   #if DEBUG_RESET_OBJECT
   MessageInterface::ShowMessage("Moderator::ResetObjectPointer() leaving\n");
   #endif
}


//----- factory
//------------------------------------------------------------------------------
// const StringArray& GetListOfFactoryItems(Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Returns names of all configurable items of object type.
 *
 * @param <type> object type
 *
 * @return array of configurable item names; return empty array if none
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetListOfFactoryItems(Gmat::ObjectType type)
{
   return theFactoryManager->GetListOfItems(type);
}

//------------------------------------------------------------------------------
// const StringArray& GetListOfAllFactoryItems()
//------------------------------------------------------------------------------
/**
 * Return a list of all items that can be created.
 *
 * @return list of all creatable items.
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetListOfAllFactoryItems()
{
   return theFactoryManager->GetListOfAllItems();
}

//------------------------------------------------------------------------------
// const StringArray& GetListOfAllFactoryItemsExcept(const ObjectTypeArray &types)
//------------------------------------------------------------------------------
/**
 * Returns names of all configurable items of object type except given type
 *
 * @param <type> object types to be excluded
 *
 * @return array of configurable item names excluding types;
 *         return empty array if none
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetListOfAllFactoryItemsExcept(const ObjectTypeArray &types)
{
   return theFactoryManager->GetListOfAllItemsExcept(types);
}

//------------------------------------------------------------------------------
// const StringArray& GetListOfViewableItems(Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Return a list of all viewable objets via the GUI
 *
 * @param  type  The enumerated ObjectType defined in gmatdef.hpp
 *
 * @return list of all viewable objects
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetListOfViewableItems(Gmat::ObjectType type)
{
   return theFactoryManager->GetListOfViewableItems(type);
}

//------------------------------------------------------------------------------
// const StringArray& GetListOfUnviewableItems(Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Return a list of all unviewable objects via the GUI
 *
 * @return list of all viewable objects
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetListOfUnviewableItems(Gmat::ObjectType type)
{
   return theFactoryManager->GetListOfUnviewableItems(type);
}


//------------------------------------------------------------------------------
// bool DoesObjectTypeMatchSubtype(const std::string &theType,
//       const std::string &theSubtype)
//------------------------------------------------------------------------------
/**
 * Checks if a creatable object type matches a subtype.
 *
 * @param coreType The ObjectType of the candidate object
 * @param theType The script identifier for the object type
 * @param theSubtype The subtype being checked
 *
 * @return true if the scripted type and subtype match, false if the type does
 *         not match the subtype
 */
//------------------------------------------------------------------------------
bool Moderator::DoesObjectTypeMatchSubtype(const Gmat::ObjectType coreType,
      const std::string &theType, const std::string &theSubtype)
{
   return theFactoryManager->DoesObjectTypeMatchSubtype(coreType, theType,
         theSubtype);
}


//----- configuration
//------------------------------------------------------------------------------
// ObjectMap* GetConfiguredObjectMap()
//------------------------------------------------------------------------------
/*
 * Returns configured object map
 */
//------------------------------------------------------------------------------
ObjectMap* Moderator::GetConfiguredObjectMap()
{
   return theConfigManager->GetObjectMap();
}


//------------------------------------------------------------------------------
// const StringArray& GetListOfObjects(Gmat::ObjectType type,
//                                     bool excludeDefaultObjects)
//------------------------------------------------------------------------------
/**
 * Returns names of all configured items of object type.
 *
 * @param <type> object type
 * @param <excludeDefaultObjects> set this flag to true if default objects
 *           should be execluded, such as  default coordinate systems
 *
 * @return array of configured item names of the type; return empty array if none
 *  return all configured item if type is UNKNOWN_OBJECT
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetListOfObjects(Gmat::ObjectType type,
                                               bool excludeDefaultObjects)
{
   tempObjectNames.clear();
   
   if (type == Gmat::UNKNOWN_OBJECT)
      return theConfigManager->GetListOfAllItems();
   
   if (type == Gmat::CELESTIAL_BODY || type == Gmat::SPACE_POINT)
   {
      tempObjectNames.clear();
      
      if (theSolarSystemInUse == NULL)
         return tempObjectNames;
      
      if (type == Gmat::CELESTIAL_BODY)
      {
         // add bodies to the list
         tempObjectNames = theSolarSystemInUse->GetBodiesInUse();
      }
      else if (type == Gmat::SPACE_POINT)
      {
         // add Spacecraft to the list
         tempObjectNames = theConfigManager->GetListOfItems(Gmat::SPACECRAFT);
         
         // add bodies to the list
         StringArray bodyList = theSolarSystemInUse->GetBodiesInUse();
         for (UnsignedInt i=0; i<bodyList.size(); i++)
            tempObjectNames.push_back(bodyList[i]);
         
         // add CalculatedPoint to the list
         StringArray calptList =
            theConfigManager->GetListOfItems(Gmat::CALCULATED_POINT);
         for (UnsignedInt i=0; i<calptList.size(); i++)
            tempObjectNames.push_back(calptList[i]);
         
         StringArray osptList =
            theConfigManager->GetListOfItems(Gmat::SPACE_POINT);
         for (UnsignedInt i=0; i<osptList.size(); i++)
         {
            // do not add the same object name
            if (find(tempObjectNames.begin(), tempObjectNames.end(), osptList[i])
                == tempObjectNames.end())
               tempObjectNames.push_back(osptList[i]);
         }
      }
      
      return tempObjectNames;
   }
   
   // Do not add default coordinate systems on option
   if (type == Gmat::COORDINATE_SYSTEM && excludeDefaultObjects)
   {
      tempObjectNames.clear();
      StringArray csObjNames = theConfigManager->GetListOfItems(type);
      for (UnsignedInt i=0; i<csObjNames.size(); i++)
         if (csObjNames[i] != "EarthMJ2000Eq" && csObjNames[i] != "EarthMJ2000Ec" &&
             csObjNames[i] != "EarthFixed")
            tempObjectNames.push_back(csObjNames[i]);
      return tempObjectNames;
   }
   
   return theConfigManager->GetListOfItems(type);
}


//------------------------------------------------------------------------------
// const StringArray& GetListOfObjects(const std::string &typeName,
//                                     bool excludeDefaultObjects)
//------------------------------------------------------------------------------
/**
 * Returns names of all configured items of object type name
 *
 * @param <typeName> object type name
 * @param <excludeDefaultObjects> set this flag to true if default objects
 *           should be execluded, such as  default coordinate systems
 *
 * @return array of configured item names of the type; return empty array if none
 *  return all configured item if type is UNKNOWN_OBJECT
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetListOfObjects(const std::string &typeName,
                                               bool excludeDefaultObjects)
{
   if (typeName == "UnknownObject")
      return theConfigManager->GetListOfAllItems();
   
   if (typeName == "CelestialBody" || typeName == "SpacePoint")
   {
      tempObjectNames.clear();
      
      if (theSolarSystemInUse == NULL)
         return tempObjectNames;
      
      if (typeName == "CelestialBody")
      {
         // add bodies to the list
         tempObjectNames = theSolarSystemInUse->GetBodiesInUse();
      }
      else if (typeName == "SpacePoint")
      {
         // add Spacecraft to the list
         tempObjectNames = theConfigManager->GetListOfItems(Gmat::SPACECRAFT);
         
         // add bodies to the list
         StringArray bodyList = theSolarSystemInUse->GetBodiesInUse();
         for (UnsignedInt i=0; i<bodyList.size(); i++)
            tempObjectNames.push_back(bodyList[i]);
         
         // add CalculatedPoint to the list
         StringArray calptList =
            theConfigManager->GetListOfItems(Gmat::CALCULATED_POINT);
         for (UnsignedInt i=0; i<calptList.size(); i++)
            tempObjectNames.push_back(calptList[i]);
         
         StringArray osptList =
            theConfigManager->GetListOfItems(Gmat::SPACE_POINT);
         for (UnsignedInt i=0; i<osptList.size(); i++)
            tempObjectNames.push_back(osptList[i]);
      }
      
      return tempObjectNames;
   }
   
   // Do not add default coordinate systems on option
   if (typeName == "CoordinateSystem" && excludeDefaultObjects)
   {
      tempObjectNames.clear();
      StringArray csObjNames = theConfigManager->GetListOfItems(typeName);
      for (UnsignedInt i=0; i<csObjNames.size(); i++)
         if (csObjNames[i] != "EarthMJ2000Eq" && csObjNames[i] != "EarthMJ2000Ec" &&
             csObjNames[i] != "EarthFixed")
            tempObjectNames.push_back(csObjNames[i]);
      return tempObjectNames;
   }
   
   return theConfigManager->GetListOfItems(typeName);
}


//------------------------------------------------------------------------------
// GmatBase* GetConfiguredObject(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* Moderator::GetConfiguredObject(const std::string &name)
{
   #if DEBUG_CONFIG
   MessageInterface::ShowMessage
      ("Moderator::GetConfiguredObject() entered: name=%s\n", name.c_str());
   #endif
   
   std::string newName = name;
   
   // check for SolarSystem first until SolarSystem can be configured(LOJ: 2009.02.19)
   if (name == "SolarSystem" || name == "Solar System")
      return theSolarSystemInUse;
   
   // Ignore array indexing of Array
   std::string::size_type index = name.find_first_of("([");
   if (index != name.npos)
   {
      newName = name.substr(0, index);
      
      #if DEBUG_CONFIG
      MessageInterface::ShowMessage
         ("Moderator::GetConfiguredObject() entered: newName=%s\n", newName.c_str());
      #endif
   }
   
   GmatBase *obj = theConfigManager->GetItem(newName);
   
   if (obj == NULL)
   {
      #if DEBUG_CONFIG
      MessageInterface::ShowMessage
         ("   Trying SolarSystem, theSolarSystemInUse=<%p>\n", theSolarSystemInUse);
      #endif
      
      // try SolarSystem
      if (theSolarSystemInUse)
         obj = (GmatBase*)(theSolarSystemInUse->GetBody(newName));
   }
   
   #if DEBUG_CONFIG
   if (obj)
   {
      MessageInterface::ShowMessage
         ("Moderator::GetConfiguredObject() Found object: name=%s, type=%s, "
          "addr=%p\n", obj->GetName().c_str(), obj->GetTypeName().c_str(), obj);
   }
   else
   {
      MessageInterface::ShowMessage
         ("Moderator::GetConfiguredObject() Cannot find object: name=%s\n",
          newName.c_str());
   }
   #endif
   
   return obj;
}


//------------------------------------------------------------------------------
// bool ReconfigureItem(GmatBase *newobj, const std::string &name)
//------------------------------------------------------------------------------
/*
 * Sets configured object pointer with new pointer.
 *
 * @param  newobj  New object pointer
 * @param  name  Name of the configured object to be reset
 *
 * @return  true if pointer was reset, false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::ReconfigureItem(GmatBase *newobj, const std::string &name)
{
   // Reconfigure item only if name found in the configuration.
   // Changed due to GmatFunction implementation (loj: 2008.06.25)
   if (GetConfiguredObject(name))
      return theConfigManager->ReconfigureItem(newobj, name);
   else
      return true;
}


//------------------------------------------------------------------------------
// std::string GetNewName(const std::string &name, Integer startCount)
//------------------------------------------------------------------------------
/*
 * It gives new name by adding counter to the input name.
 *
 * @param <name> Base name to used to generate new name
 * @param <startCount> Starting counter
 * @return new name
 */
//------------------------------------------------------------------------------
std::string Moderator::GetNewName(const std::string &name, Integer startCount)
{
   if (name == "")
      return "";
   
   return theConfigManager->GetNewName(name, startCount);
}


//------------------------------------------------------------------------------
// std::string AddClone(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Adds the clone of the named object to configuration.
 * It gives new name by adding counter to the name to be cloned.
 *
 * return new name if object was cloned and added to configuration, blank otherwise
 */
//------------------------------------------------------------------------------
std::string Moderator::AddClone(const std::string &name)
{
   if (name == "")
      return "";

   return theConfigManager->AddClone(name);
}


//------------------------------------------------------------------------------
// bool RenameObject(Gmat::ObjectType type, const std::string &oldName
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
bool Moderator::RenameObject(Gmat::ObjectType type, const std::string &oldName,
                                     const std::string &newName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Moderator::RenameObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   // let's check to make sure it is a valid name
   if (!GmatStringUtil::IsValidName(newName, true))
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "'%s' is not a valid object name.\nPlease enter a different name.\n",
         newName.c_str());
      return false;
   }
   
   // check to make sure it is not a command type
   StringArray commandNames = GetListOfFactoryItems(Gmat::COMMAND);
   for (Integer i=0; i<(Integer)commandNames.size(); i++)
   {
      if (commandNames[i] == newName)
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "'%s' is not a valid object name.\nPlease enter a different name.\n",
            newName.c_str());
         return false;
      }
   }
   
   #if DEBUG_RENAME
   MessageInterface::ShowMessage("   Calling theConfigManager->RenameItem()\n");
   #endif
   bool renamed = theConfigManager->RenameItem(type, oldName, newName);
   
   //--------------------------------------------------
   // rename object name used in mission sequence
   //--------------------------------------------------
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Moderator::RenameObject() ===> Change Command ref object names\n");
   #endif
   
   int sandboxIndex = 0; //handles one sandbox for now
   GmatCommand *cmd = commands[sandboxIndex]->GetNext();
   GmatCommand *child;
   std::string typeName;
   
   while (renamed && cmd != NULL)
   {
      typeName = cmd->GetTypeName();
      #if DEBUG_RENAME
      MessageInterface::ShowMessage
         ("...typeName=%12s, '%s'\n", typeName.c_str(),
          cmd->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      #endif
      
      renamed = cmd->RenameRefObject(type, oldName, newName);
      child = cmd->GetChildCommand(0);
      
      while (renamed && (child != NULL) && (child != cmd))
      {
         typeName = child->GetTypeName();
         #if DEBUG_RENAME
         MessageInterface::ShowMessage
            ("......typeName=%12s, '%s'\n", typeName.c_str(),
             cmd->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
         #endif
         if (typeName.find("End") == typeName.npos)
            renamed = child->RenameRefObject(type, oldName, newName);
         
         child = child->GetNext();
      }
      
      cmd = cmd->GetNext();
   }
   
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Moderator::RenameObject() rename status=%d\n", renamed);
   #endif

   return renamed;
}

//------------------------------------------------------------------------------
// bool RemoveObject(Gmat::ObjectType type, const std::string &name,
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
bool Moderator::RemoveObject(Gmat::ObjectType type, const std::string &name,
                             bool delOnlyIfNotUsed)
{
   GmatCommand *cmd = GetFirstCommand();
   
   #if DEBUG_REMOVE
   MessageInterface::ShowMessage
      ("Moderator::RemoveObject() type=%d, name=%s, delOnlyIfNotUsed=%d\n",
       type, name.c_str(), delOnlyIfNotUsed);
   MessageInterface::ShowMessage(GmatCommandUtil::GetCommandSeqString(cmd));
   #endif
   
   if (!delOnlyIfNotUsed)
   {
      return theConfigManager->RemoveItem(type, name);
   }
   else
   {
      // remove if object is not used in other resource
      #if DEBUG_REMOVE
      MessageInterface::ShowMessage("   Checking if '%s' is used in resource\n", name.c_str());
      #endif
      GmatBase *obj = theConfigManager->GetFirstItemUsing(type, name);
      if (obj != NULL)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** Cannot remove \"%s.\"  It is used in the %s "
             "object named \"%s\"\n", name.c_str(), obj->GetTypeName().c_str(),
             obj->GetName().c_str());
         return false;
      }
      else
      {
         #if DEBUG_REMOVE
         MessageInterface::ShowMessage
            ("   '%s' is not used in resource, checking command\n", name.c_str());
         #endif
         // remove if object is not used in the command sequence
         std::string cmdName;
         if (GmatCommandUtil::FindObject(cmd, type, name, cmdName))
         {
            MessageInterface::ShowMessage
               ("*** WARNING *** Cannot remove \"%s.\"  It is used in the %s "
                "command.\n",  name.c_str(), cmdName.c_str());
            return false;
         }
         else
         {
            bool retval = theConfigManager->RemoveItem(type, name);
            #if DEBUG_REMOVE
            MessageInterface::ShowMessage
               ("Moderator::RemoveObject() returning %d from "
                "theConfigManager->RemoveItem()\n", retval);
            #endif
            return retval;
         }
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
   #if DEBUG_CONFIG
   MessageInterface::ShowMessage
      ("Moderator::ConfigurationChanged() obj type=%s, name='%s', changed=%d\n",
       obj->GetTypeName().c_str(), obj->GetName().c_str(), tf);
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
}


//------------------------------------------------------------------------------
// SolarSystem* CreateSolarSystem(const std::string &name)
//------------------------------------------------------------------------------
SolarSystem* Moderator::CreateSolarSystem(const std::string &name)
{
   #if DEBUG_SOLAR_SYSTEM
   MessageInterface::ShowMessage
      ("Moderator::CreateSolarSystem() creating '%s'\n", name.c_str());
   #endif
   
   // There is no factory to create SolarSystem so just create by new
   // SolarSystem constructor Creates available planetary ephem sorce list.
   // Also set to DE405 as a default planetary ephem source
   SolarSystem *ss = new SolarSystem(name);
   
   #ifdef DEBUG_MEMORY
   std::string funcName;
   funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
   MemoryTracker::Instance()->Add
      (ss, name, "Moderator::CreateSolarSystem()", funcName);
   #endif
   
   #if DEBUG_SOLAR_SYSTEM
   MessageInterface::ShowMessage("Moderator::CreateSolarSystem() returning %p\n", ss);
   #endif
   
   return ss;
}


//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystemInUse(Integer manage = 1)
//------------------------------------------------------------------------------
/*
 * Returns SolarSystem in use from configuration or object map in use
 *
 * @param  manage  If value is 1 it will return SolarSystem from the configuration
 *                 if value is 2 it will return SolarSystem from the object map in use
 */
//------------------------------------------------------------------------------
SolarSystem* Moderator::GetSolarSystemInUse(Integer manage)
{
   #if DEBUG_SOLAR_SYSTEM_IN_USE
   MessageInterface::ShowMessage
      ("Moderator::GetSolarSystemInUse() entered, manage=%d, objectMapInUse=<%p>, "
       "theInternalSolarSystem=<%p>\n", manage, objectMapInUse, theInternalSolarSystem);
   #endif
   
   SolarSystem *ss = NULL;
   if (manage == 1)
   {
      ss = theConfigManager->GetSolarSystemInUse();
      #if DEBUG_SOLAR_SYSTEM_IN_USE
      MessageInterface::ShowMessage("   Using SolarSystem from configuration <%p>\n", ss);
      #endif
   }
   else
   {
      ObjectMap::iterator pos = objectMapInUse->find("SolarSystem");
      if (pos != objectMapInUse->end())
      {
         ss = (SolarSystem*)pos->second;
         #if DEBUG_SOLAR_SYSTEM_IN_USE
         MessageInterface::ShowMessage("   Using SolarSystem from objectMapInUse <%p>\n", ss);
         #endif
      }
      
      if (ss == NULL)
      {
         ss = theInternalSolarSystem;
         #if DEBUG_SOLAR_SYSTEM_IN_USE
         MessageInterface::ShowMessage("   Using Internal SolarSystem <%p>\n", ss);
         #endif
      }      
   }
   
   // if SolarSystem is NULL, there is some problem
   if (ss == NULL)
      throw GmatBaseException
         ("Moderator::GetSolarSystemInUse() The SolarSystem in use is UNSET.\n");
   
   #if DEBUG_SOLAR_SYSTEM_IN_USE
   MessageInterface::ShowMessage
      ("Moderator::GetSolarSystemInUse() returning <%p>\n", ss);
   #endif
   
   return ss;
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
// void SetInternalSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/*
 * Sets the internal solar system. The internal solar system is initially set to
 * theSolarSystemInUse for creating main objects and commands. When creating
 * objects and commands for GmatFunction, it will use the solar system cloned in
 * the Sandbox during the Sandbox initialization. Setting this internal solar
 * system happens during the GmatFunction initialization.
 */
//------------------------------------------------------------------------------
void Moderator::SetInternalSolarSystem(SolarSystem *ss)
{
   if (ss != NULL)
   {
      #if DEBUG_SOLAR_SYSTEM_IN_USE
      MessageInterface::ShowMessage
         ("Moderator::SetInternalSolarSystem() entered, ss=<%p>'%s'\n",
          ss, ss->GetName().c_str());
      #endif
      
      theInternalSolarSystem = ss;
   }
}


//------------------------------------------------------------------------------
// bool SetSolarSystemInUse(const std::string &name)
//------------------------------------------------------------------------------
bool Moderator::SetSolarSystemInUse(const std::string &name)
{
   return theConfigManager->SetSolarSystemInUse(name);
}


// Create object
//------------------------------------------------------------------------------
// GmatBase* CreateOtherObject(Gmat::ObjectType objType, const std::string &type,
//                             const std::string &name, bool createDefault)
//------------------------------------------------------------------------------
GmatBase* Moderator::CreateOtherObject(Gmat::ObjectType objType, const std::string &type,
                                       const std::string &name, bool createDefault)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateOtherObject() objType=%d, type='%s', name='%s', createDefault=%d, "
       "objectManageOption=%d\n", objType, type.c_str(), name.c_str(), createDefault,
       objectManageOption);
   #endif
   
   GmatBase *obj = NULL;
   if (FindObject(name) == NULL)
   {
      obj = theFactoryManager->CreateObject(objType, type, name);
      
      if (obj == NULL)
      {
         throw GmatBaseException
            ("The Moderator cannot create an object of type \"" + type + "\"\n");
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateOtherObject()", funcName);
      }
      #endif
      
      // Manage it if it is a named object
      try
      {
         if (name != "" && objectManageOption == 1)
         {
            theConfigManager->AddObject(objType, obj);
            #if DEBUG_CREATE_RESOURCE
            MessageInterface::ShowMessage("   ==> '%s' added to configuration\n", name.c_str());
            #endif
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateOtherObject()\n" +
                                       e.GetFullMessage());
      }
      
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateOtherObject() returning <%p>\n", obj);
      #endif
      return obj;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateOtherObject() Unable to create an object "
          "name: %s already exist\n", name.c_str());
      #endif
      return FindObject(name);
   }
}


// CalculatedPoint
//------------------------------------------------------------------------------
// CalculatedPoint* CreateCalculatedPoint(const std::string &type,
//                                        const std::string &name,
//                                        bool addDefaultBodies = true)
//------------------------------------------------------------------------------
/**
 * Creates a calculated point object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 * @param <addDefaultBodies> true if add default bodies requested (true)
 *
 * @return a CalculatedPoint object pointer
 */
//------------------------------------------------------------------------------
CalculatedPoint* Moderator::CreateCalculatedPoint(const std::string &type,
                                                  const std::string &name,
                                                  bool addDefaultBodies)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateCalculatedPoint() type='%s', name='%s', "
       "addDefaultBodies=%d, objectManageOption=%d\n", type.c_str(), name.c_str(),
       addDefaultBodies, objectManageOption);
   #endif
   
   if (GetCalculatedPoint(name) == NULL)
   {
      #ifdef DEBUG_CREATE_CALC_POINT
         MessageInterface::ShowMessage("Moderator::Creating new %s named %s\n",
               type.c_str(), name.c_str());
      #endif
      CalculatedPoint *obj = theFactoryManager->CreateCalculatedPoint(type, name);
      
      if (obj == NULL)
      {
         throw GmatBaseException
            ("The Moderator cannot create a CalculatedPoint type \"" + type + "\"\n");
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateCalculatedPoint()", funcName);
      }
      #endif
      
      // add default bodies
      if (type == "LibrationPoint")
      {
         if (addDefaultBodies)
         {
            obj->SetStringParameter("Primary", "Sun");
            obj->SetStringParameter("Point", "L1");
            obj->SetStringParameter("Secondary", "Earth");
            
            #ifdef __CREATE_DEFAULT_BC__
            // first create default Earth-Moon Barycenter
            CalculatedPoint *defBc = GetCalculatedPoint("DefaultBC");
            
            if (defBc == NULL)
               defBc = CreateCalculatedPoint("Barycenter", "DefaultBC");
            
            obj->SetStringParameter("Secondary", "DefaultBC");
            obj->SetRefObject(defBc, Gmat::SPACE_POINT, "DefaultBC");
            #endif
            
            // Set body and J2000Body pointer, so that GUI can create LibrationPoint
            // and use it in Coord.System conversion
            SpacePoint *sun = (SpacePoint*)FindObject("Sun");
            SpacePoint *earth = (SpacePoint*)FindObject("Earth");
            
            if (sun->GetJ2000Body() == NULL)
               sun->SetJ2000Body(earth);
            
            #if DEBUG_CREATE_RESOURCE
            MessageInterface::ShowMessage
               ("Moderator::Setting sun <%p> and earth <%p> to LibrationPoint %s\n", sun, earth, name.c_str());
            #endif
            
            obj->SetRefObject(sun, Gmat::SPACE_POINT, "Sun");
            obj->SetRefObject(earth, Gmat::SPACE_POINT, "Earth");
         }
      }
      else if (type == "Barycenter")
      {
         if (addDefaultBodies)
         {
//            obj->SetStringParameter("BodyNames", "Earth");
            ((CalculatedPoint*) obj)->SetDefaultBody("Earth");
            ((CalculatedPoint*) obj)->SetDefaultBody("Luna");

            // Set body and J2000Body pointer, so that GUI can create LibrationPoint
            // and use it in Coord.System conversion
//            SpacePoint *earth = (SpacePoint*)FindObject("Earth");
//            obj->SetRefObject(earth, Gmat::SPACE_POINT, "Earth");

            // obj->SetStringParameter("BodyNames", "Luna");
            // SpacePoint *luna = (SpacePoint*)FindObject("Luna");
            // if (luna->GetJ2000Body() == NULL)
            //    luna->SetJ2000Body(earth);
            // obj->SetRefObject(luna, Gmat::SPACE_POINT, "Luna");
         }
      }
      
      // Manage it if it is a named CalculatedPoint
      try
      {
         if (name != "" && objectManageOption == 1)
            theConfigManager->AddCalculatedPoint(obj);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateCalculatedPoint()\n" +
                                       e.GetFullMessage());
      }
      
      return obj;
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
      return (CalculatedPoint*)FindObject(name);
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
   #ifdef DEBUG_CREATE_BODY
   MessageInterface::ShowMessage
      ("Moderator::CreateCelestialBody() called with type = '%s' and name = '%s'\n",
       type.c_str(), name.c_str());
   if (GetCelestialBody(name) != NULL) 
      MessageInterface::ShowMessage("... that body alreday exists\n");
   #endif
   if (GetCelestialBody(name) == NULL)
   {
      CelestialBody *obj = theFactoryManager->CreateCelestialBody(type, name);
      
      if (obj == NULL)
         throw GmatBaseException
            ("The Moderator cannot create a CelestialBody type \"" + type + "\"\n");
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateCelestialBody()", funcName);
      }
      #endif
      
      // Add new celestial body to solar syatem in use
      Integer manage = 0; // Just set other than 1 here
      SolarSystem *ss = GetSolarSystemInUse(manage);
      obj->SetUserDefined(true);
      obj->SetSolarSystem(ss);
      obj->SetUpBody(); //Added so that it works inside a GmatFunction (LOJ: 2010.04.30)
      ss->AddBody(obj);
      #ifdef DEBUG_CREATE_BODY
      MessageInterface::ShowMessage
         ("Moderator::CreateCelestialBody() Created CelestialBody <%p> "
          "\"%s\" and added to Solar System <%p>\n", obj, name.c_str(), ss);
      #endif
      
      // Manually set configuration changed to true here since
      // SolarSystem is not configured yet
      theConfigManager->ConfigurationChanged(true);
      
      return obj;
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
      return (CelestialBody*)FindObject(name);
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
      ("Moderator::CreateSpacecraft() type = '%s', name = '%s'\n",
       type.c_str(), name.c_str());
   #endif
   
   if (GetSpacecraft(name) == NULL)
   {
      Spacecraft *obj = (Spacecraft*)(theFactoryManager->CreateSpacecraft(type, name));
      
      if (obj == NULL)
         throw GmatBaseException
            ("The Moderator cannot create a Spacecraft type \"" + type + "\"\n");
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateSpacecraft()", funcName);
      }
      #endif
      
      // Create internal and default CoordinateSystems if they do not exist.
      // This code will make new mission to work after script errors occur.
      // This change was made while looking at Bug 1532 (LOJ: 2009.11.13)
      if (theInternalCoordSystem == NULL)
         CreateInternalCoordSystem();
      CreateDefaultCoordSystems();
      
      if (type == "Spacecraft")
      {
         // Set internal and default CoordinateSystem
         obj->SetInternalCoordSystem(theInternalCoordSystem);
         obj->SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
      }
      
      // Manage it if it is a named Spacecraft
      try
      {
         if (name != "" && objectManageOption == 1)
            theConfigManager->AddSpacecraft(obj);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateSpacecraft()\n" +
                                       e.GetFullMessage());
      }
      
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateSpacecraft() returning <%p>'%s'\n", obj,
          obj->GetName().c_str());
      #endif
      return obj;
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
      return (SpaceObject*)FindObject(name);
}


//------------------------------------------------------------------------------
// std::string GetSpacecraftNotInFormation()
//------------------------------------------------------------------------------
/**
 * This method finds the first spacecraft name sorted by ascending order not
 * in any formation.
 *
 * @return  The first spacecraft name not in any formation.
 *          return "" if such spacecraft is not found.
 */
//------------------------------------------------------------------------------
std::string Moderator::GetSpacecraftNotInFormation()
{
   StringArray scList = GetListOfObjects(Gmat::SPACECRAFT);
   StringArray fmList = GetListOfObjects(Gmat::FORMATION);
   int numSc = scList.size(), numFm = fmList.size();
   
   if (numSc == 0 && numFm == 0)
      return "";
   
   if (numSc > 0 && numFm == 0)
      return GetDefaultSpacecraft()->GetName();
   
   // formation exists
   StringArray fmscListAll;
   
   //------------------------------------------
   // Merge spacecrafts in Formation
   //------------------------------------------
   for (int i=0; i<numFm; i++)
   {
      GmatBase *fm = GetConfiguredObject(fmList[i]);
      StringArray fmscList = fm->GetStringArrayParameter(fm->GetParameterID("Add"));
      fmscListAll.insert(fmscListAll.begin(), fmscList.begin(), fmscList.end());
   }
   
   // sort the lists in order to  set_difference()
   sort(scList.begin(), scList.end());
   sort(fmscListAll.begin(), fmscListAll.end());
   
   //------------------------------------------
   // Make list of spacecrafts not in Formation
   //------------------------------------------
   StringArray scsNotInForms;
   
   // The set_difference() algorithm produces a sequence that contains the
   // difference between the two ordered sets.
   set_difference(scList.begin(), scList.end(), fmscListAll.begin(),
                  fmscListAll.end(), back_inserter(scsNotInForms));
   
   if (scsNotInForms.size() > 0)
      return scsNotInForms[0];
   else
      return "";
}


// SpacePoints
//------------------------------------------------------------------------------
// SpacePoint* CreateSpacePoint(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a spacepoint object by given name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return SpacePoint object pointer
 */
//------------------------------------------------------------------------------
SpacePoint* Moderator::CreateSpacePoint(const std::string &type,
                                        const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateSpacePoint() type = '%s', name = '%s'\n",
       type.c_str(), name.c_str());
   #endif
   
   if (GetSpacePoint(name) == NULL)
   {
      SpacePoint *obj = (SpacePoint*)(theFactoryManager->CreateSpacePoint(type, name));
      
      if (obj == NULL)
      {
         throw GmatBaseException
            ("The Moderator cannot create a SpacePoint type \"" + type + "\"\n");
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateSpacePoint()", funcName);
      }
      #endif
            
      // Manage it if it is a named SpacePoint
      try
      {
         if (name != "" && objectManageOption == 1)
            theConfigManager->AddSpacePoint(obj);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateSpacePoint()\n" +
                                       e.GetFullMessage());
      }
   
      return obj;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateSpacePoint() Unable to create SpacePoint "
          "name: %s already exist\n", name.c_str());
      #endif
      return GetSpacePoint(name);
   }
}

//------------------------------------------------------------------------------
// SpacePoint* GetSpacePoint(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a SpacePoint object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a SpacePoint object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
SpacePoint* Moderator::GetSpacePoint(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (SpacePoint*)FindObject(name);
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
      ("Moderator::CreateHardware() type = '%s', name = '%s'\n",
       type.c_str(), name.c_str());
   #endif
   
   if (GetHardware(name) == NULL)
   {
      Hardware *obj = theFactoryManager->CreateHardware(type, name);
      
      if (obj == NULL)
      {
         throw GmatBaseException
            ("The Moderator cannot create a Hardware type \"" + type + "\"\n");
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateHardware()", funcName);
      }
      #endif
      
      // Manage it if it is a named Hardware
      try
      {
         if (name != "" && objectManageOption == 1)
            theConfigManager->AddHardware(obj);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateHardware()\n" +
                                       e.GetFullMessage());
      }
      
      return obj;
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
      return (Hardware*)FindObject(name);
}


// Propagator
//------------------------------------------------------------------------------
// Propagator* CreatePropagator(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a propagator object by given type and name. Actually this creates
 * Integrator.
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
      ("Moderator::CreatePropagator() type = '%s', name = '%s'\n",
       type.c_str(), name.c_str());
   #endif
   
   // GMAT doesn't name propagators, so we don't need to check the configuration
   // for them.  PropSetups are the only things that get named for propagation.
   Propagator *obj = theFactoryManager->CreatePropagator(type, name);
      
   if (obj ==  NULL)
      throw GmatBaseException
         ("The Moderator cannot create a Propagator type \"" + type + "\"\n");
      
   #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreatePropagator()", funcName);
      }
   #endif
      
   #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreatePropagator() returning new Propagator <%p>'%s'\n",
          obj, obj->GetName().c_str());
      #endif
      
      return obj;
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
      return (Propagator*)FindObject(name);
}

// PhysicalModel
//------------------------------------------------------------------------------
// PhysicalModel* CreateDefaultPhysicalModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a default physical model of full Earth gravity force with JGM2 file.
 *
 * @param <name> object name
 *
 * @return a physical model object pointer
 */
//------------------------------------------------------------------------------
PhysicalModel* Moderator::CreateDefaultPhysicalModel(const std::string &name)
{
   std::string type = "GravityField";
   
   if (GetPhysicalModel(name) == NULL)
   {
      PhysicalModel *obj =
         theFactoryManager->CreatePhysicalModel(type, name);
      
      if (obj ==  NULL)
         throw GmatBaseException
               ("The Moderator cannot create a PhysicalModel type \"" + type +
                "\"\n");
      
      #ifdef DEBUG_MEMORY
      std::string funcName;
      funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
      MemoryTracker::Instance()->Add
         (obj, name, "Moderator::CreateDefaultPhysicalModel()", funcName);
      #endif
      
      SolarSystem *ss = GetSolarSystemInUse(objectManageOption);
      obj->SetName("Earth");
      obj->SetSolarSystem(ss);
      obj->SetBody("Earth");
      obj->SetBodyName("Earth");
      
      if (type == "GravityField")
         obj->SetStringParameter("PotentialFile", GetFileName("JGM2_FILE"));
      
      // Manage it if it is a named PhysicalModel
      try
      {
         if (name != "" && objectManageOption == 1)
            theConfigManager->AddPhysicalModel(obj);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreatePhysicalModel()\n" +
                                       e.GetFullMessage());
      }
      
      return obj;
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
      PhysicalModel *obj =
         theFactoryManager->CreatePhysicalModel(type, name);
      
      if (obj ==  NULL)
         throw GmatBaseException
            ("The Moderator cannot create a PhysicalModel type \"" + type + "\"\n");
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreatePhysicalModel()", funcName);
      }
      #endif
      
      // Manage it if it is a named PhysicalModel
      try
      {
         if (name != "" && objectManageOption == 1)
            theConfigManager->AddPhysicalModel(obj);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreatePhysicalModel()\n" +
                                       e.GetFullMessage());
      }
      
      return obj;
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
      return (PhysicalModel*)FindObject(name);
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
      ("Moderator::CreateAtmosphereModel() type = '%s', name = '%s', body = '%s'\n",
       type.c_str(), name.c_str(), body.c_str());
   #endif
   
   // if AtmosphereModel name doesn't exist, create AtmosphereModel
   if (GetAtmosphereModel(name) == NULL)
   {
      AtmosphereModel *obj =
         theFactoryManager->CreateAtmosphereModel(type, name, body);
      
      if (obj ==  NULL)
         throw GmatBaseException
            ("The Moderator cannot create an AtmosphereModel type \"" + type + "\"\n");
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateAtmosphereModel()", funcName);
      }
      #endif
      
      // Manage it if it is a named AtmosphereModel
      try
      {
         if (name != "" && objectManageOption == 1)
            theConfigManager->AddAtmosphereModel(obj);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage
            ("Moderator::CreateAtmosphereModel()\n" + e.GetFullMessage());
      }
    
      return obj;
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
      return (AtmosphereModel*)FindObject(name);
}

// Burn
//------------------------------------------------------------------------------
// Burn* CreateBurn(const std::string &type, const std::string &name,
//                  bool createDefault)
//------------------------------------------------------------------------------
/**
 * Creates a burn object by given type and name and add to configuration.
 * If createDefault is true, it will create "Local" coordinate system with
 * "VNB" axes. Usually this flag is set to true if ImpulsiveBurn object is
 * created from the GUI.
 *
 * @param <type> object type
 * @param <name> object name
 * @param <createDefault> set to true if default burn to be created (false)
 *
 * @return a burn object pointer
 */
//------------------------------------------------------------------------------
Burn* Moderator::CreateBurn(const std::string &type,
                            const std::string &name, bool createDefault)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateBurn() type = '%s, name = '%s'\n", type.c_str(),
       name.c_str());
   #endif
   
   // if Burn name doesn't exist, create Burn
   if (GetBurn(name) == NULL)
   {
      Burn *obj = theFactoryManager->CreateBurn(type, name);
      
      if (obj ==  NULL)
         throw GmatBaseException
            ("The Moderator cannot create Burn type \"" + type + "\"\n");
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateBurn()", funcName);
      }
      #endif
      
      // Set default Axes to VNB
      if (createDefault)
      {
         obj->SetStringParameter(obj->GetParameterID("CoordinateSystem"), "Local");
         obj->SetStringParameter(obj->GetParameterID("Axes"), "VNB");
      }
      
      // Manage it if it is a named Burn
      try
      {
         if (name != "" && objectManageOption == 1)
            theConfigManager->AddBurn(obj);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateBurn()\n" +
                                       e.GetFullMessage());
      }
      
      return obj;
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
      return (Burn*)FindObject(name);
}

// Parameter
//------------------------------------------------------------------------------
// bool Moderator::IsParameter(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Checks to see if a given type is a Parameter. If str has '.', it parses
 * string to get type before checking.
 *
 * @param <str> object type string
 *
 * @return true if the type is a registered parameter, false if not.
 */
//------------------------------------------------------------------------------
bool Moderator::IsParameter(const std::string &str)
{
   if (str == "") return false;
   StringArray sar = theFactoryManager->GetListOfItems(Gmat::PARAMETER);
   std::string type;
   
   if (str.find(".") == str.npos)
   {
      type = str;
   }
   else
   {
      std::string ownerName, depObj;
      GmatStringUtil::ParseParameter(str, type, ownerName, depObj);
   }
   
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
// Parameter* CreateAutoParameter(const std::string &type, const std::string &name
//                            const std::string &ownerName,
//                            const std::string &depName, bool manage)
//------------------------------------------------------------------------------
/**
 * Creates a parameter object by given type and name and add to configuration.
 *
 * @param <type> parameter type
 * @param <name> parameter name
 * @param <ownerName> parameter owner name ("")
 * @param <depName> dependent object name ("")
 * @param <manage>  0, if parameter is not managed
 *                  1, if parameter is added to configuration (default)
 *                  2, if Parameter is added to function object map
 *
 * @return a parameter object pointer
 */
//------------------------------------------------------------------------------
Parameter* Moderator::CreateAutoParameter(const std::string &type,
                                          const std::string &name,
                                          bool &alreadyManaged,
                                          const std::string &ownerName,
                                          const std::string &depName,
                                          Integer manage)
{
   #if DEBUG_CREATE_PARAMETER
   MessageInterface::ShowMessage
      ("Moderator::CreateAutoParameter() type='%s', name='%s', ownerName='%s', "
       "depName='%s', manage=%d\n", type.c_str(), name.c_str(), ownerName.c_str(),
       depName.c_str(), manage);
   #endif
   
   // if managing and Parameter already exist, give warning and return existing
   // Parameter
   alreadyManaged = false;
   Parameter *param = GetParameter(name);
   #if DEBUG_CREATE_PARAMETER
   MessageInterface::ShowMessage
      ("   managed param = <%p> '%s'\n", param, param ? param->GetName().c_str() : "NULL");
   #endif
   
   // if Parameter was created during GmatFunction parsing, just set reference object
   if (param != NULL && manage != 0)
   {
      #if DEBUG_CREATE_PARAMETER
      MessageInterface::ShowMessage
         ("*** WARNING *** Moderator::CreateAutoParameter() Unable to create "
          "Parameter name: %s already exist\n", name.c_str());
      MessageInterface::ShowMessage
         ("Moderator::CreateAutoParameter() returning <%s><%p>\n", param->GetName().c_str(),
          param);
      #endif
      
      // set Parameter reference object
      SetParameterRefObject(param, type, name, ownerName, depName, manage);
      
      // if Parameter is managed in the function object map, add it (loj: 2008.12.16)
      // so that we won't create multiple Parameters. FindObject() finds object from
      // objectMapInUse which can be object map from configuration or passed function
      // object map
      if (manage == 2)
         AddObject(param);
      
      alreadyManaged = true;
      return param;
   }
   
   return CreateParameter(type, name, ownerName, depName, manage);
}


//------------------------------------------------------------------------------
// Parameter* CreateParameter(const std::string &type, const std::string &name
//                            const std::string &ownerName = "",
//                            const std::string &depName = "", bool manage = 1)
//------------------------------------------------------------------------------
/**
 * Creates a parameter object by given type and name and add to configuration.
 *
 * @param <type> parameter type
 * @param <name> parameter name
 * @param <ownerName> parameter owner name ("")
 * @param <depName> dependent object name ("")
 * @param <manage>  0, if parameter is not managed
 *                  1, if parameter is added to configuration (default)
 *                  2, if Parameter is added to function object map
 *
 * @return a parameter object pointer
 *
 * @note manage option will override member data objectManageOption so do not
 * use objectManageOption here.
 */
//------------------------------------------------------------------------------
Parameter* Moderator::CreateParameter(const std::string &type,
                                      const std::string &name,
                                      const std::string &ownerName,
                                      const std::string &depName,
                                      Integer manage)
{   
   #if DEBUG_CREATE_PARAMETER
   MessageInterface::ShowMessage
      ("Moderator::CreateParameter() type='%s', name='%s', ownerName='%s', "
       "depName='%s', manage=%d\n", type.c_str(), name.c_str(), ownerName.c_str(),
       depName.c_str(), manage);
   MessageInterface::ShowMessage("   objectManageOption=%d\n", objectManageOption);
   #endif
   
   // if managing and Parameter already exist, give warning and return existing
   // Parameter
   Parameter *param = GetParameter(name);
   #if DEBUG_CREATE_PARAMETER
   MessageInterface::ShowMessage
      ("   managed param = <%p> '%s'\n", param, param ? param->GetName().c_str() : "NULL");
   #endif
   
   // if Parameter was created during GmatFunction parsing, just set reference object
   if (param != NULL && manage != 0)
   {
      #if DEBUG_CREATE_PARAMETER
      MessageInterface::ShowMessage
         ("*** WARNING *** Moderator::CreateParameter() Unable to create "
          "Parameter name: %s already exist\n", name.c_str());
      MessageInterface::ShowMessage
         ("Moderator::CreateParameter() returning <%s><%p>\n", param->GetName().c_str(),
          param);
      #endif
      
      // set Parameter reference object
      SetParameterRefObject(param, type, name, ownerName, depName, manage);
      
      // if Parameter is managed in the function object map, add it (loj: 2008.12.16)
      // so that we won't create multiple Parameters. FindObject() finds object from
      // objectMapInUse which can be object map from configuration or passed function
      // object map
      if (manage == 2)
         AddObject(param);
      
      return param;
   }
   
   // Check for deprecated Element* on Thruster, new Parameters are ThrustDirection*
   std::string newType = type;
   if (type == "Element1" || type == "Element2" || type == "Element3")
   {
      Integer numDots = GmatStringUtil::NumberOfOccurrences(name, '.');
      if (numDots > 1)
      {
         newType = GmatStringUtil::Replace(newType, "Element", "ThrustDirection");
         #if DEBUG_CREATE_PARAMETER
         MessageInterface::ShowMessage
            ("   Parameter type '%s' in '%s' changed to '%s'\n", type.c_str(),
             name.c_str(), newType.c_str());
         #endif
      }
   }
   
   // Ceate new Parameter
   param = theFactoryManager->CreateParameter(newType, name);
   
   #ifdef DEBUG_MEMORY
   if (param)
   {
      std::string funcName;
      funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
      MemoryTracker::Instance()->Add
         (param, name, "Moderator::CreateParameter()", funcName);
   }
   #endif
   
   #if DEBUG_CREATE_PARAMETER
   MessageInterface::ShowMessage
      ("   new param = <%p><%s>\n", param, param ? param->GetName().c_str() : "NULL");
   #endif
   
   if (param == NULL)
   {
      throw GmatBaseException
         ("The Moderator cannot create a Parameter type \"" + newType +
          "\" named \"" + name + "\"\n");
   }
   
   // We don't know the owner type the parameter before create,
   // so validate owner type after create.
   if (ownerName != "" && manage != 0)
      CheckParameterType(param, newType, ownerName);
   
   // set Parameter reference object
   SetParameterRefObject(param, newType, name, ownerName, depName, manage);
   
   // Add to configuration if manage flag is true and it is a named parameter
   try
   {
      // check if object is to be managed in configuration(loj: 2008.03.18)
      // @note Do not use objectManageOption here since manage flag overrides
      // this option for automatic objects such as Paramters
      if (manage == 1)
      {
         bool oldFlag = theConfigManager->HasConfigurationChanged();
         
         if (param->GetName() != "")
            theConfigManager->AddParameter(param);
         
         // if system paramter, set configuration changed to old flag.
         if (param->GetKey() == GmatParam::SYSTEM_PARAM)
         {
            #if DEBUG_CREATE_PARAMETER
            MessageInterface::ShowMessage
               ("   Resetting configuration changed flag for SystemParameter "
                "'%s' to %d\n", param->GetName().c_str(), oldFlag);
            #endif
            
            theConfigManager->ConfigurationChanged(oldFlag);
         }
      }
      else if (manage == 2) //LOJ: Add object if managed in FOS(2009.03.09)
      {
         AddObject(param);
      }
   }
   catch (BaseException &e)
   {
      // Use exception to remove Visual C++ warning
      e.GetMessageType();
      #if DEBUG_CREATE_PARAMETER
      MessageInterface::ShowMessage("Moderator::CreateParameter()\n" +
                                    e.GetFullMessage() + "\n");
      #endif
   }
   
   #if DEBUG_CREATE_PARAMETER
   MessageInterface::ShowMessage
      ("Moderator::CreateParameter() returning <%s><%p>\n", param->GetName().c_str(),
       param);
   #endif
   
   return param;
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
   Parameter *obj = NULL;
   
   if (name != "")
   {
      // find Parameter from the current object map in use (loj: 2008.05.23)
      GmatBase* obj = FindObject(name);
      if (obj != NULL && obj->IsOfType(Gmat::PARAMETER))
         return (Parameter*)obj;
   }
   
   return obj;
}

// ODEModel
//------------------------------------------------------------------------------
// ODEModel* CreateODEModel(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Creates ODEModel with given name
 */
//------------------------------------------------------------------------------
ODEModel* Moderator::CreateODEModel(const std::string &type,
                                    const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateODEModel() name='%s', objectManageOption=%d\n",
       name.c_str(), objectManageOption);
   #endif
   
   ODEModel *obj = GetODEModel(name);
   
   if (obj == NULL)
   {
      obj = theFactoryManager->CreateODEModel(type, name);
      
      if (obj == NULL)
      {
         MessageInterface::ShowMessage("No fm\n");
         throw GmatBaseException
            ("The Moderator cannot create ODEModel named \"" + name + "\"\n");
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateODEModel()", funcName);
      }
      #endif
      
      // Create default physical model
      PhysicalModel *pm = CreateDefaultPhysicalModel("");
      pm->SetName("_DefaultInternalForce_");
      obj->AddForce(pm);
      
      // Manage it if it is a named ODEModel
      try
      {
         if (obj->GetName() != "" && objectManageOption == 1)
            theConfigManager->AddODEModel(obj);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateODEModel()\n" +
                                       e.GetFullMessage() + "\n");
      }
      
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateODEModel() returning new ODEModel, <%p> '%s'\n",
          obj, obj->GetName().c_str());
      #endif
      return obj;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateODEModel() Unable to create ODEModel "
          "name: %s already exist <%p>\n", name.c_str(), obj);
      #endif
      return obj;
   }
}


//------------------------------------------------------------------------------
// ODEModel* GetODEModel(const std::string &name)
//------------------------------------------------------------------------------
ODEModel* Moderator::GetODEModel(const std::string &name)
{
   ODEModel *fm = NULL;
   
   if (name != "")
   {
      // Find ODEModel from the current object map in use (loj: 2008.06.20)
      GmatBase* obj = FindObject(name);
      if (obj != NULL && obj->IsOfType(Gmat::ODE_MODEL))
      {
         fm = (ODEModel*)obj;
         
         #if DEBUG_CREATE_RESOURCE
         MessageInterface::ShowMessage
            ("Moderator::GetODEModel() name='%s', returning <%p>\n", name.c_str(), fm);
         #endif
         
         return fm;
      }
   }
   
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::GetODEModel() name='%s', returning <%p>\n", name.c_str(), fm);
   #endif
   
   return fm;
}


//------------------------------------------------------------------------------
// bool AddToODEModel(const std::string &ODEModelName,
//                      const std::string &forceName)
//------------------------------------------------------------------------------
bool Moderator::AddToODEModel(const std::string &odeModelName,
                                const std::string &forceName)
{
   bool status = true;
   ODEModel *fm = theConfigManager->GetODEModel(odeModelName);
   PhysicalModel *physicalModel = theConfigManager->GetPhysicalModel(forceName);
   fm->AddForce(physicalModel);
   return status;
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
   MessageInterface::ShowMessage
      ("Moderator::CreateSolver() type = '%s', name = '%s'\n", type.c_str(),
       name.c_str());
   #endif
   
   if (GetSolver(name) == NULL)
   {
      Solver *obj = theFactoryManager->CreateSolver(type, name);

      if (obj == NULL)
      {
         throw GmatBaseException
            ("The Moderator cannot create Solver type \"" + type + "\"\n");
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateSolver()", funcName);
      }
      #endif
      
      // Manage it if it is a named Solver
      try
      {
         if (obj->GetName() != "" && objectManageOption == 1)
            theConfigManager->AddSolver(obj);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateSolver()\n" +
                                       e.GetFullMessage());
      }
      
      return obj;
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
      return (Solver*)FindObject(name);
}

// PropSetup
//------------------------------------------------------------------------------
// PropSetup* CreateDefaultPropSetup(const std::string &name)
//------------------------------------------------------------------------------
PropSetup* Moderator::CreateDefaultPropSetup(const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("====================\n");
   MessageInterface::ShowMessage("Moderator::CreateDefaultPropSetup() name='%s'\n",
                                 name.c_str());
   #endif
   
   // create PropSetup, PropSetup constructor creates default RungeKutta89 Integrator
   // and Earth PointMassForce
   PropSetup *propSetup = CreatePropSetup(name);
   
   // create default force model with Earth primary body with JGM2
   // Why unnamed FM? Changed back to named FM, since it causes error in
   // Interpreter::FinalPass() when parsing ScriptEvent (Begin/EndScript) from GUI.
   // The error is about undefined DefaultProp_ForceModel(LOJ: 2011.01.12)
   ODEModel *fm= CreateODEModel("ForceModel", name + "_ForceModel");
   // Create unnamed ODEModel when creating default PropSetup (LOJ: 2009.11.23)
   // and delete fm after setting it to PropSetup (see below)
   //ODEModel *fm = CreateODEModel("ForceModel", "");
   //fm->SetName(name + "_ForceModel");
   
   //=======================================================
   #if 0
   //=======================================================
   GravityField *gravForce = new GravityField("", "Earth");
   
   #ifdef DEBUG_MEMORY
   std::string funcName;
   funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
   MemoryTracker::Instance()->Add
      (gravForce, gravForce->GetName(),
       "Moderator::CreateDefaultPropSetup(), *gravForce = new GravityField", funcName);
   #endif
   
   gravForce->SetName("Earth");
   gravForce->SetSolarSystem(theSolarSystemInUse);
   gravForce->SetBody("Earth");
   gravForce->SetBodyName("Earth");
   gravForce->SetStringParameter("PotentialFile", GetFileName("JGM2_FILE"));
   //=======================================================
   #endif
   //=======================================================
   
   propSetup->SetODEModel(fm);
   
   // Why unnamed FM? Commented out (LOJ: 2011.01.12)
   // PropSetup::SetODEModel() clones the ODEModel, so delete it from here (LOJ: 2009.11.23)
   //#ifdef DEBUG_MEMORY
   //std::string funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
   //MemoryTracker::Instance()->Remove
   //   (fm, fm->GetName(), "Moderator::CreateDefaultPropSetup()"
   //    "deleting fm", funcName);
   //#endif
   //delete fm;
   
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreatePropSetup() returning new DefaultPropSetup <%p>\n", propSetup);
   #endif
   
   return propSetup;
}

//------------------------------------------------------------------------------
// PropSetup* CreatePropSetup(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Creates PropSetup which contains Integrator and ODEModel.
 */
//------------------------------------------------------------------------------
PropSetup* Moderator::CreatePropSetup(const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("====================\n");
   MessageInterface::ShowMessage("Moderator::CreatePropSetup() name='%s'\n",
                                 name.c_str());
   #endif
   
   if (GetPropSetup(name) == NULL)
   {
      PropSetup *propSetup = theFactoryManager->CreatePropSetup(name);
      
      if (propSetup == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "The Moderator cannot create a PropSetup.\n"
             "Make sure PropSetup is correct type and registered to "
             "PropSetupFactory.\n");
         return NULL;
      }
      
      #ifdef DEBUG_MEMORY
      if (propSetup)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (propSetup, name, "Moderator::CreatePropSetup()", funcName);
      }
      #endif
      
      // PropSetup creates default Integrator(RungeKutta89)
      // and default ODEModel (PointMassForce body=Earth)
      
      if (name != "" && objectManageOption == 1)
         theConfigManager->AddPropSetup(propSetup);
      
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreatePropSetup() returning new PropSetup <%p>\n", propSetup);
      #endif
      
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
      return (PropSetup*)FindObject(name);
}


// MeasurementModel
//------------------------------------------------------------------------------
// MeasurementModel* CreateMeasurementModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a new named MeasurementModel and adds it to the configuration
 *
 * @param name The name of the new MeasurementModel
 *
 * @return The new MeasurementModel
 */
//------------------------------------------------------------------------------
MeasurementModel* Moderator::CreateMeasurementModel(const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("====================\n");
   MessageInterface::ShowMessage("Moderator::GetMeasurementModel() name='%s'\n",
                                 name.c_str());
   #endif

   if (GetMeasurementModel(name) == NULL)
   {
      MeasurementModel *obj = theFactoryManager->CreateMeasurementModel(name);

      if (obj == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "The Moderator cannot create a MeasurementModel.\n"
             "Make sure MeasurementModel is correct type and registered to "
             "MeasurementModelFactory.\n");
         return NULL;
      }

      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateMeasurementModel()", funcName);
      }
      #endif
      
      if (name != "" && objectManageOption == 1)
         theConfigManager->AddMeasurementModel(obj);
      
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateMeasurementModel() returning new MeasurementModel "
               "<%p>\n", obj);
      #endif

      return obj;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateMeasurementModel() Unable to create "
          "MeasurementModel name: %s already exist\n", name.c_str());
      #endif
      return GetMeasurementModel(name);
   }
}

//------------------------------------------------------------------------------
// MeasurementModel* GetMeasurementModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a measurement model from the configuration
 *
 * @param name The name of the MeasurementModel object
 *
 * @return The named MeasurementModel
 */
//------------------------------------------------------------------------------
MeasurementModel* Moderator::GetMeasurementModel(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (MeasurementModel*)FindObject(name);
}

// TrackingSystem
//------------------------------------------------------------------------------
// TrackingSystem* CreateTrackingSystem(const std::string &type,
//       const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a new named TrackingSystem and adds it to the configuration
 *
 * @param type The type of the new TrackingSystem
 * @param name The name of the new TrackingSystem
 *
 * @return The new TrackingSystem
 */
//------------------------------------------------------------------------------
TrackingSystem* Moderator::CreateTrackingSystem(const std::string &type,
         const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("====================\n");
   MessageInterface::ShowMessage("Moderator::CreateTrackingSystem() type=%s, "
            "name='%s'\n", type.c_str(), name.c_str());
   #endif

   if (GetTrackingSystem(name) == NULL)
   {
      TrackingSystem *obj = theFactoryManager->CreateTrackingSystem(type, name);

      if (obj == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "The Moderator cannot create a TrackingSystem.\n"
             "Make sure TrackingSystem is correct type and registered to "
             "TrackingSystemFactory.\n");
         return NULL;
      }

      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateTrackingSystem()", funcName);
      }
      #endif
      
      if (name != "" && objectManageOption == 1)
         theConfigManager->AddTrackingSystem(obj);
      
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateTrackingSystem() returning new TrackingSystem "
               "<%p>\n", obj);
      #endif

      return obj;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateTrackingSystem() Unable to create "
          "TrackingSystem name: %s already exists\n", name.c_str());
      #endif
      return GetTrackingSystem(name);
   }
}

//------------------------------------------------------------------------------
// TrackingSystem* GetTrackingSystem(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a TrackingSystem from the configuration
 *
 * @param name The name of the TrackingSystem object
 *
 * @return The named TrackingSystem
 */
//------------------------------------------------------------------------------
TrackingSystem* Moderator::GetTrackingSystem(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (TrackingSystem*)FindObject(name);
}

// TrackingData
//------------------------------------------------------------------------------
// TrackingData* CreateTrackingData(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a new named TrackingData object and adds it to the configuration
 *
 * @param name The name of the new TrackingData object
 *
 * @return The new TrackingData object
 */
//------------------------------------------------------------------------------
TrackingData* Moderator::CreateTrackingData(const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("====================\n");
   MessageInterface::ShowMessage("Moderator::CreateTrackingData() name='%s'\n",
                                 name.c_str());
   #endif

   if (GetTrackingData(name) == NULL)
   {
      TrackingData *obj = theFactoryManager->CreateTrackingData(name);

      if (obj == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "The Moderator cannot create a TrackingData object."
             "\nMake sure TrackingData is correct type and registered to "
             "TrackingDataFactory.\n");
         return NULL;
      }

      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateTrackingData()", funcName);
      }
      #endif
      
      if (name != "" && objectManageOption == 1)
         theConfigManager->AddTrackingData(obj);
      
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateTrackingData() returning new TrackingData "
               "<%p>\n", obj);
      #endif

      return obj;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateTrackingData() Unable to create "
          "TrackingData name: %s already exists\n", name.c_str());
      #endif
      return GetTrackingData(name);
   }
}

//------------------------------------------------------------------------------
// TrackingData* GetTrackingData(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a TrackingData object from the configuration
 *
 * @param name The name of the TrackingData object
 *
 * @return The named TrackingData object
 */
//------------------------------------------------------------------------------
TrackingData* Moderator::GetTrackingData(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (TrackingData*)FindObject(name);
}


//------------------------------------------------------------------------------
// CoreMeasurement* CreateMeasurement(const std::string &type,
//       const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method calls the FactoryManager to create a new CoreMeasurement object
 *
 * @param type The type of measurement object needed
 * @param name The new object's name, is it is to be configured
 *
 * @return The new object's pointer
 */
//------------------------------------------------------------------------------
CoreMeasurement* Moderator::CreateMeasurement(const std::string &type,
      const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("====================\n");
   MessageInterface::ShowMessage("Moderator::CreateMeasurement() name='%s'\n",
                                 name.c_str());
   #endif

   if (GetMeasurement(type, name) == NULL)
   {
      CoreMeasurement *obj = theFactoryManager->CreateMeasurement(type, name);

      if (obj == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "The Moderator cannot create a Measurement.\n"
             "Make sure Measurement %s is correct type and registered to "
             "MeasurementFactory.\n", type.c_str());
         return NULL;
      }

      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateMeasurementModel()", funcName);
      }
      #endif

      if (name != "" && objectManageOption == 1)
         theConfigManager->AddMeasurement(obj);

      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateMeasurement() returning new Measurement "
               "<%p>\n", obj);
      #endif

      return obj;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateMeasurement() Unable to create "
          "Measurement name: %s already exist\n", name.c_str());
      #endif
      return GetMeasurement(type, name);
   }
}

//------------------------------------------------------------------------------
// CoreMeasurement* GetMeasurement(const std::string &type,
//       const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method finds a configured CoreMeasurement
 *
 * @param type The type of measurement object needed
 * @param name The new object's name, is it is to be configured
 *
 * @return The object's pointer; NULL if the object is not in the configuration
 */
//------------------------------------------------------------------------------
CoreMeasurement* Moderator::GetMeasurement(const std::string &type,
      const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (CoreMeasurement*)FindObject(name);
}


// DataFile
//------------------------------------------------------------------------------
// DataFile* CreateDataFile(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a new named DataFile and adds it to the configuration
 *
 * @param type The type of the DataFile object
 * @param name The name of the new DataFile
 *
 * @return The new DataFile
 */
//------------------------------------------------------------------------------
DataFile* Moderator::CreateDataFile(const std::string &type,
                                    const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("====================\n");
   MessageInterface::ShowMessage
      ("Moderator::CreateDataFile() type='%s', name='%s'\n", type.c_str(),
       name.c_str());
   #endif
   
   if (GetDataFile(name) == NULL)
   {
      DataFile *df = theFactoryManager->CreateDataFile(type, name);
      
      if (df == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "The Moderator cannot create a DataFile.\n"
             "Make sure DataFile is correct type and registered to "
             "DataFileFactory.\n");
         return NULL;
      }
      
      #ifdef DEBUG_MEMORY
      if (df)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (df, name, "Moderator::CreateDataFile()", funcName);
      }
      #endif
      
      if (name != "" && objectManageOption == 1)
         theConfigManager->AddDataFile(df);
      
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateDataFile() returning new DataFile "
               "<%p>\n", df);
      #endif
      
      return df;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateDataFile() Unable to create "
          "DataFile name: %s already exists\n", name.c_str());
      #endif
      return GetDataFile(name);
   }
}


//------------------------------------------------------------------------------
// DataFile* GetDataFile(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a measurement DataFile from the configuration
 *
 * @param name The name of the DataFile object
 *
 * @return The named DataFile
 */
//------------------------------------------------------------------------------
DataFile* Moderator::GetDataFile(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (DataFile*)FindObject(name);
}


// ObType
//------------------------------------------------------------------------------
// ObType* CreateObType(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
ObType* Moderator::CreateObType(const std::string &type, const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("====================\n");
   MessageInterface::ShowMessage("Moderator::CreateObType() name='%s'\n",
                                 name.c_str());
   #endif

   if (GetObType(name) == NULL)
   {
      ObType *ot = theFactoryManager->CreateObType(type, name);

      if (ot == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "The Moderator cannot create a ObType.\n"
             "Make sure ObType is correct type and registered to a "
             "ObTypeFactory.\n");
         return NULL;
      }

      #ifdef DEBUG_MEMORY
      if (ot)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (ot, name, "Moderator::CreateObType()", funcName);
      }
      #endif

      if (name != "" && objectManageOption == 1)
         theConfigManager->AddObType(ot);
      
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateObType() returning new ObType "
               "<%p>\n", ot);
      #endif

      return ot;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateDataFile() Unable to create "
          "DataFile name: %s already exists\n", name.c_str());
      #endif
      return GetObType(name);
   }
}

//------------------------------------------------------------------------------
// ObType* GetObType(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a ObType from the configuration
 *
 * @param name The name of the ObType object
 *
 * @return The named ObType  (Should always return NULL)
 */
//------------------------------------------------------------------------------
ObType* Moderator::GetObType(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (ObType*)FindObject(name);
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
//                   bool createDefault = false, bool internal = false,
//                   Integer manage = 1)
//------------------------------------------------------------------------------
/**
 * Creates coordinate system
 *
 * @param  name  Name of the coordinate system to create
 * @param  createDefault  If this flag is set, it will create MJ2000Eq system
 * @param  internal  If this flag is set, it will not configure the CS
 * @param  manage  The value to use for managing the newly created CS
 *                 0 = do not add to configuration
 *                 1 = add to configuration
 */
//------------------------------------------------------------------------------
CoordinateSystem* Moderator::CreateCoordinateSystem(const std::string &name,
                                                    bool createDefault,
                                                    bool internal, Integer manage)
{
   #if DEBUG_CREATE_COORDSYS
   MessageInterface::ShowMessage
      ("Moderator::CreateCoordinateSystem() name='%s', createDefault=%d, "
       "internal=%d, manage=%d, theSolarSystemInUse=%p\n", name.c_str(), createDefault,
       internal, manage, theSolarSystemInUse);
   #endif
   
   CoordinateSystem *obj = GetCoordinateSystem(name);
   
   if (obj == NULL)
   {
      #if DEBUG_CREATE_COORDSYS
      MessageInterface::ShowMessage
         ("   Cannot find CoordinateSystem named '%s', so create\n", name.c_str());
      #endif
      
      obj = theFactoryManager->CreateCoordinateSystem(name);
      
      if (obj == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "The Moderator cannot create a CoordinateSystem.\n"
             "Make sure CoordinateSystem is correct type and registered to "
             "CoordinateSystemFactory.\n");
         
         return NULL;
         
         //throw GmatBaseException
         //   ("The Moderator cannot create a CoordinateSystem type \"" + type + "\"\n");
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateCoordinateSystem()", funcName);
      }
      #endif
      
      // Manage it if it is a named CoordinateSystem
      try
      {
         if (name != "" && !internal && manage != 0)
         {
            if (manage == 1)
               theConfigManager->AddCoordinateSystem(obj);
            else
               // Do we really want to add a new CoordinateSystem to the
               // function object map? (loj: 2008.06.27)
               AddObject(obj);
         }
         
         // Call GetSolarSystemInUse() to get SolarSystem from configuration
         // or object map in use (loj: 2008.06.27)
         SolarSystem *ss = GetSolarSystemInUse(manage);
         CelestialBody *earth = ss->GetBody("Earth");
         #if DEBUG_CREATE_COORDSYS
            MessageInterface::ShowMessage
               ("Mod::CreateCS = SolarSystem found at <%p>\n", ss);
            MessageInterface::ShowMessage
               ("Mod::CreateCS = Earth found at <%p>\n", earth);
         #endif
         
         // Set J2000Body and SolarSystem
         obj->SetStringParameter("J2000Body", "Earth");
         obj->SetRefObject(earth, Gmat::SPACE_POINT, "Earth");
         obj->SetSolarSystem(ss);
         obj->Initialize();
         
         if (createDefault)
         {
            // create MJ2000Eq AxisSystem with Earth as origin
            AxisSystem *axis = CreateAxisSystem("MJ2000Eq", "MJ2000Eq_Earth");
            obj->SetStringParameter("J2000Body", "Earth");
            obj->SetStringParameter("Origin", "Earth");
            obj->SetRefObject(earth, Gmat::SPACE_POINT, "Earth");
            obj->SetRefObject(axis, Gmat::AXIS_SYSTEM, axis->GetName());
            obj->SetSolarSystem(ss);
            obj->Initialize();
            
            // Since CoordinateSystem clones AxisSystem, delete it from here
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (axis, "localAxes", "Moderator::CreateCoordinateSystem()",
                "deleting localAxes");
            #endif
            delete axis;
         }
      }
      catch (BaseException &e)
      {
         // Use exception to remove Visual C++ warning
         e.GetMessageType();
         #if DEBUG_CREATE_COORDSYS
         MessageInterface::ShowMessage("Moderator::CreateCoordinateSystem() %s\n",
                                       e.GetFullMessage().c_str());
         #endif
      }
      
      #if DEBUG_CREATE_COORDSYS
      MessageInterface::ShowMessage
         ("Moderator::CreateCoordinateSystem() returning new %p\n", obj);
      #endif
      
      return obj;
   }
   else
   {
      #if DEBUG_CREATE_COORDSYS
      MessageInterface::ShowMessage
         ("Moderator::CreateCoordinateSystem() Unable to create CoordinateSystem "
          "name: %s already exist\n", name.c_str());      
      MessageInterface::ShowMessage
         ("Moderator::CreateCoordinateSystem() returning existing %p\n", obj);
      #endif
      
      return obj;
   }
}


//------------------------------------------------------------------------------
// CoordinateSystem* GetCoordinateSystem(const std::string &name, Integer manage = 1)
//------------------------------------------------------------------------------
CoordinateSystem* Moderator::GetCoordinateSystem(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (CoordinateSystem*)FindObject(name);
}


//------------------------------------------------------------------------------
// const StringArray& GetDefaultCoordinateSystemNames()
//------------------------------------------------------------------------------
const StringArray& Moderator::GetDefaultCoordinateSystemNames()
{
   return defaultCoordSystemNames;
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
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateSubscriber() type='%s', name='%s', fileName='%s'\n"
       "   createDefault=%d\n", type.c_str(), name.c_str(), fileName.c_str(),
       createDefault);
   MessageInterface::ShowMessage
      ("   currentFunction = <%p>'%s', objectManageOption=%d\n", currentFunction,
       currentFunction ? currentFunction->GetName().c_str() : "NULL",
       objectManageOption);
   #endif
   
   if (GetSubscriber(name) == NULL)
   {      
      Subscriber *obj = theFactoryManager->CreateSubscriber(type, name, fileName);
      
      if (obj == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Cannot create a Subscriber type: %s.\n"
             "Make sure %s is correct type and registered to SubscriberFactory.\n",
             type.c_str(), type.c_str());
         
         return NULL;
         
         //throw GmatBaseException
         //   ("The Moderator cannot create a Subscriber type\"" + type + "\"\n");
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateSubscriber()", funcName);
      }
      #endif
      
      // Manage it if it is a named Subscriber
      try
      {
         if (obj->GetName() != "" && objectManageOption == 1)
            theConfigManager->AddSubscriber(obj);
         
         #if DEBUG_CREATE_RESOURCE
         MessageInterface::ShowMessage
            ("Moderator::CreateSubscriber() Creating default subscriber...\n");
         #endif
         
         if (createDefault)
         {
            if (type == "OrbitView")
            {
               // add default spacecraft and coordinate system
               obj->SetStringParameter("Add", GetDefaultSpacecraft()->GetName());
               obj->SetStringParameter("Add", "Earth");
               obj->SetStringParameter("CoordinateSystem", "EarthMJ2000Eq");
            }
            else if (type == "GroundTrackPlot")
            {
               // add default spacecraft and earth
               obj->SetStringParameter("Add", GetDefaultSpacecraft()->GetName());
               obj->SetStringParameter("Add", "Earth");
            }
            else if (type == "XYPlot")
            {
               // add default x,y parameter to XYPlot
               obj->SetStringParameter(XyPlot::XVARIABLE, GetDefaultX()->GetName());
               obj->SetStringParameter(XyPlot::YVARIABLES, GetDefaultY()->GetName(), 0);
               obj->Activate(true);
            }
            else if (type == "ReportFile")
            {
               // add default parameters to ReportFile
               obj->SetStringParameter(obj->GetParameterID("Filename"),
                                       name + ".txt");
               obj->SetStringParameter("Add", GetDefaultX()->GetName());
               obj->SetStringParameter("Add", GetDefaultY()->GetName());
               obj->Activate(true);
               
               // To validate and create element wrappers
               theScriptInterpreter->ValidateSubscriber(obj);
            }
            else if (type == "EphemerisFile")
            {
               // add default spacecraft and coordinate system
               obj->SetStringParameter("Spacecraft", GetDefaultSpacecraft()->GetName());
            }
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateSubscriber()\n" +
                                       e.GetFullMessage());
      }
      
      return obj;
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
      return (Subscriber*)FindObject(name);
}

//------------------------------------------------------------------------------
// Subscriber* CreateEphemerisFile(const std::string &type,
//                                 const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a subscriber object by given type and name if not already created.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a subscriber object pointer
 */
//------------------------------------------------------------------------------
Subscriber* Moderator::CreateEphemerisFile(const std::string &type,
                                           const std::string &name)
{
   #if DEBUG_CREATE_EPHEMFILE
   MessageInterface::ShowMessage
      ("Moderator::CreateEphemerisFile() type='%s', name='%s'\n",
       type.c_str(), name.c_str());
   MessageInterface::ShowMessage
      ("   in function = <%p>'%s'\n", currentFunction,
       currentFunction ? currentFunction->GetName().c_str() : "NULL");
   #endif
   
   if (GetEphemerisFile(name) == NULL)
   {      
      Subscriber *obj = (Subscriber*)(theFactoryManager->CreateEphemerisFile(type, name));
      
      if (obj == NULL)
      {
         #ifdef __USE_DATAFILE__
            if (type == "CcsdsEphemerisFile")
            {
               MessageInterface::PopupMessage
                  (Gmat::ERROR_, "**** ERROR **** Cannot create an EphemerisFile type: %s.\n"
                   "Make sure to specify PLUGIN = libDataFile and PLUGIN = libCcsdsEphemerisFile\n"
                   "in the gmat_start_file and make sure such dlls exist.  "
                   "Make sure that libpcre-0 and libpcrecpp-0 also exist in the path\n",
                   type.c_str(), type.c_str());

               return NULL;
            }
         #else
            #ifdef DEBUG_CREATE_EPHEMFILE
            MessageInterface::ShowMessage
               ("CreateEphemerisFile() Creating a subscriber of type EphemerisFile\n");
            #endif
            // Try again with "EphemerisFile" type
            obj = (Subscriber*)(theFactoryManager->CreateSubscriber("EphemerisFile", name));
         #endif
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateEphemerisFile()", funcName);
      }
      #endif
      
      // Manage it if it is a named EphemerisFile
      try
      {
         if (name != "" && objectManageOption == 1)
            theConfigManager->AddSubscriber(obj);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateEphemerisFile()\n" +
                                       e.GetFullMessage());
      }
      
      #if DEBUG_CREATE_EPHEMFILE
      MessageInterface::ShowMessage
         ("Moderator::CreateEphemerisFile() returning <%p>\n", obj);
      #endif
      
      return obj;
   }
   else
   {
      #if DEBUG_CREATE_EPHEMFILE
      MessageInterface::ShowMessage
         ("Moderator::CreateEphemerisFile() Unable to create EphemerisFile "
          "name: %s already exist\n", name.c_str());
      #endif
      
      return GetEphemerisFile(name);
   }
}

//------------------------------------------------------------------------------
// Subscriber* GetEphemerisFile(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a subscriber object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a subscriber object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Subscriber* Moderator::GetEphemerisFile(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (Subscriber*)FindObject(name);
}

// Function
//------------------------------------------------------------------------------
// Function* CreateFunction(const std::string &type, const std::string &name,
//                          Integer manage = 1)
//------------------------------------------------------------------------------
/**
 * Creates a function object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 * @param <manage> if 0, new function is not managed
 *                    1, new function is added to configuration
 * @return a Function object pointer
 */
//------------------------------------------------------------------------------
Function* Moderator::CreateFunction(const std::string &type,
                                    const std::string &name,
                                    Integer manage)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateFunction() type = '%s', name = '%s', manage=%d\n",
       type.c_str(), name.c_str(), manage);
   MessageInterface::ShowMessage
      ("   in function = <%p>'%s'\n", currentFunction,
       currentFunction ? currentFunction->GetName().c_str() : "NULL");
   #endif
   
   if (GetFunction(name) == NULL)
   {
      Function *obj = theFactoryManager->CreateFunction(type, name);
      
      if (obj == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Cannot create a Function type: %s.\n"
             "Make sure %s is correct type and registered to FunctionFactory.\n",
             type.c_str(), type.c_str());
         
         return NULL;
         
         //throw GmatBaseException
         //   ("The Moderator cannot create a Function type \"" + type + "\"\n");
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateFunction()", funcName);
      }
      #endif
      
      // Manage it if it is a named Function
      try
      {
         if (name != "" && manage == 1)
            theConfigManager->AddFunction(obj);
         else if (currentFunction != NULL && manage == 0)
            unmanagedFunctions.push_back(obj);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateFunction()\n" +
                                       e.GetFullMessage());
      }
      
      return obj;
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


//----- Non-configurable Items

// StopCondition
//------------------------------------------------------------------------------
// StopCondition* CreateStopCondition(const std::string &type,
//                                    const std::string &name)
//------------------------------------------------------------------------------
StopCondition* Moderator::CreateStopCondition(const std::string &type,
                                              const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("Moderator::CreateStopCondition() type = '%s', "
                                 "name = '%s'\n", type.c_str(), name.c_str());
   #endif
   
   StopCondition *stopCond = theFactoryManager->CreateStopCondition(type, name);
   
   if (stopCond ==  NULL)
   {
      throw GmatBaseException
         ("The Moderator cannot create StopCondition type \"" + type + "\"\n");
   }
   
   #ifdef DEBUG_MEMORY
   if (stopCond)
   {
      std::string funcName;
      funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
      MemoryTracker::Instance()->Add
         (stopCond, name, "Moderator::CreateStopCondition()", funcName);
   }
   #endif
   
   return stopCond;
}


//------------------------------------------------------------------------------
// AxisSystem* CreateAxisSystem(const std::string &type,
//                              const std::string &name, Integer manage)
//------------------------------------------------------------------------------
/**
 * Creates a AxisSystem object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 * @param <manage>  0, if parameter is not managed
 *                  1, if parameter is added to configuration (default)
 *                  2, if Parameter is added to function object map
 *
 * @return a AxisSystem object pointer
 */
//------------------------------------------------------------------------------
AxisSystem* Moderator::CreateAxisSystem(const std::string &type,
                                        const std::string &name, Integer manage)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateAxisSystem() type = '%s', name = '%s', manage=%d\n",
       type.c_str(), name.c_str(), manage);
   #endif
   
   AxisSystem *axisSystem = theFactoryManager->CreateAxisSystem(type, name);
   
   if (axisSystem == NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Cannot create a AxisSystem type: %s.\n"
          "Make sure %s is correct type and registered to AxisSystemFactory.\n",
          type.c_str(), type.c_str());
      
      return NULL;
      
      //throw GmatBaseException
      //   ("The Moderator cannot create AxisSystem type \"" + type + "\"\n");
   }
   
   #ifdef DEBUG_MEMORY
   if (axisSystem)
   {
      std::string funcName;
      funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
      MemoryTracker::Instance()->Add
         (axisSystem, name, "Moderator::CreateAxisSystem()", funcName);
   }
   #endif
   
   // set origin and j2000body
   axisSystem->SetOrigin((SpacePoint*)FindObject(axisSystem->GetOriginName()));
   axisSystem->SetJ2000Body((SpacePoint*)FindObject(axisSystem->GetJ2000BodyName()));
   
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
   
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateAxisSystem() returning <%p>\n", axisSystem);
   #endif
   
   return axisSystem;
}

// MathNode
//------------------------------------------------------------------------------
// MathNode* CreateMathNode(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a MathNode object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a MathNode object pointer
 */
//------------------------------------------------------------------------------
MathNode* Moderator::CreateMathNode(const std::string &type,
                                    const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("Moderator::CreateMathNode() type = '%s', "
                                 "name = '%s'\n", type.c_str(), name.c_str());
   #endif
   
   MathNode *mathNode = theFactoryManager->CreateMathNode(type, name);
   
   if (mathNode ==  NULL)
   {
      throw GmatBaseException
         ("The Moderator cannot create MathNode type \"" + type + "\"\n");
   }
   
   #ifdef DEBUG_MEMORY
   if (mathNode)
   {
      std::string funcName;
      funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
      MemoryTracker::Instance()->Add
         (mathNode, name, "Moderator::CreateMathNode()", funcName);
   }
   #endif
   
   return mathNode;
}

//------------------------------------------------------------------------------
// Attitude* CreateAttitude(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates an Attitude object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return an Attitude object pointer
 */
//------------------------------------------------------------------------------
Attitude* Moderator::CreateAttitude(const std::string &type,
                                    const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("Moderator::CreateAttitude() type = '%s', "
                                 "name = '%s'\n", type.c_str(), name.c_str());
   #endif
   
   Attitude *att = theFactoryManager->CreateAttitude(type, name);
   
   if (att == NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Cannot create an Attitude type: %s.\n"
          "Make sure %s is correct type and registered to AttitudeFactory.\n",
          type.c_str(), type.c_str());
      
      return NULL;
   }
   
   #ifdef DEBUG_MEMORY
   if (att)
   {
      std::string funcName;
      funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
      MemoryTracker::Instance()->Add
         (att, name, "Moderator::CreateAttitude()", funcName);
   }
   #endif
   
   return att;
}


//GmatCommand
//------------------------------------------------------------------------------
// GmatCommand* InterpretGmatFunction(const std::string fileName)
//------------------------------------------------------------------------------
/**
 * Retrieves a function object pointer by given name.
 *
 * @param <fileName>  Full path and name of the GmatFunction file.
 *
 * @return A command list that is executed to run the function.
 */
//------------------------------------------------------------------------------
GmatCommand* Moderator::InterpretGmatFunction(const std::string &fileName)
{
   #if DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("Moderator::InterpretGmatFunction() fileName='%s'\n", fileName.c_str());
   #endif
   
   GmatCommand *cmd = NULL;
   if (fileName != "")
      cmd =  theScriptInterpreter->InterpretGmatFunction(fileName);
   
   ResetConfigurationChanged();
   
   #if DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("Moderator::InterpretGmatFunction() returning <%p><%s>\n", cmd,
       cmd ? cmd->GetTypeName().c_str() : "NULL");
   #endif
   
   return cmd;
}


//------------------------------------------------------------------------------
// GmatCommand* InterpretGmatFunction(Function *funct, ObjectMap *objMap,
//                                    SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Retrieves a function object pointer by given name.
 *
 * @param <funct>  The GmatFunction pointer
 * @param <objMap> The object map pointer to be used for finding objects
 * @param <ss>     The solar system to be used
 *
 * @return A command list that is executed to run the function.
 */
//------------------------------------------------------------------------------
GmatCommand* Moderator::InterpretGmatFunction(Function *funct, ObjectMap *objMap,
                                              SolarSystem *ss)
{
   #if DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("Moderator::InterpretGmatFunction() function=<%p>, objMap=<%p>\n",
       funct, objMap);
   #endif
   
   currentFunction = funct;
   
   // If input objMap is NULL, use configured objects
   // use input object map otherwise   
   if (objMap == NULL)
   {
      objectMapInUse = theConfigManager->GetObjectMap();
      #ifdef DEBUG_OBJECT_MAP
      MessageInterface::ShowMessage
         ("Moderator::InterpretGmatFunction() objectMapInUse was set to the "
          "configuration map <%p>\n", objectMapInUse);
      #endif
   }
   else
   {
      objectMapInUse = objMap;
      #ifdef DEBUG_OBJECT_MAP
      MessageInterface::ShowMessage
         ("Moderator::InterpretGmatFunction() objectMapInUse was set to "
          "input objMap <%p>\n", objectMapInUse);
      #endif
   }
   
   // If input SolarSystem is NULL, use default SolarSystemInUse
   // use input SolarSystem otherwise      
   SolarSystem *solarSystemInUse = GetSolarSystemInUse();
   if (ss != NULL)
   {
      solarSystemInUse = ss;
      theInternalSolarSystem = ss;
   }
   
   #if DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("   Setting objectMapInUse<%p> to theScriptInterpreter\n"
       "   Setting theSolarSystemInUse<%p> to theScriptInterpreter\n",
       objectMapInUse, solarSystemInUse);
   #endif
   
   #if DEBUG_GMAT_FUNCTION > 1
   ShowObjectMap("Moderator::InterpretGmatFunction() Here is the object map in use");
   #endif
   
   // Set solar system in use and object map for GmatFunction
   SetSolarSystemAndObjectMap(solarSystemInUse, objectMapInUse, true,
                              "InterpretGmatFunction()");
   
   GmatCommand *cmd = NULL;
   cmd = theScriptInterpreter->InterpretGmatFunction(funct);
   
   // reset current function to NULL
   currentFunction = NULL;
   
   #if DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("Moderator::InterpretGmatFunction() returning <%p><%s>\n", cmd,
       cmd ? cmd->GetTypeName().c_str() : "NULL");
   #endif
   
   return cmd;
} // InterpretGmatFunction()


//------------------------------------------------------------------------------
// GmatCommand* CreateCommand(const std::string &type, const std::string &name
//                            bool &retFlag)
//------------------------------------------------------------------------------
/**
 * Creates command from the factory
 *
 * @param  type  The command type name such as Propagate, Maneuver
 * @param  name  The name of the command, usally blank
 * @param  retFlag  The return flag
 */
//------------------------------------------------------------------------------
GmatCommand* Moderator::CreateCommand(const std::string &type,
                                      const std::string &name, bool &retFlag)
{
   #if DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("Moderator::CreateCommand() entered: type = '%s', name = '%s'\n",
       type.c_str(), name.c_str());
   #endif
   
   GmatCommand *cmd = theFactoryManager->CreateCommand(type, name);
   
   if (cmd == NULL)
   {
      //MessageInterface::PopupMessage
      //   (Gmat::ERROR_, "Cannot create a Command type: %s.\n"
      //    "Make sure %s is correct type and registered to Commandactory.\n",
      //    type.c_str(), type.c_str());
      
      throw GmatBaseException
         ("The Moderator cannot create a Command type \"" + type + "\"\n");
   }
   
   #ifdef DEBUG_MEMORY
   if (cmd)
   {
      std::string funcName;
      funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
      MemoryTracker::Instance()->Add
         (cmd, type, "Moderator::CreateCommand()", funcName);
   }
   #endif
   
   #if DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("Moderator::CreateCommand() returning <%p><%s>, retFlag=%d\n", cmd,
       cmd->GetTypeName().c_str(), retFlag);
   #endif
   
   retFlag = true;
   return cmd;
}


//------------------------------------------------------------------------------
// GmatCommand* CreateDefaultCommand(const std::string &type,
//                                   const std::string &name,
//                                   GmatCommand *refCmd = NULL)
//------------------------------------------------------------------------------
/*
 * Creates a command with default settings. The input refCmd is only used for
 * EndFiniteBurn to match with BeginFiniteBurn. This method is usually called
 * from the GUI.
 *
 * @param  type  Command type
 * @param  name  Command name
 * @param  refCmd  Referenced command name
 *
 * @return  New command pointer
 */
//------------------------------------------------------------------------------
GmatCommand* Moderator::CreateDefaultCommand(const std::string &type,
                                             const std::string &name,
                                             GmatCommand *refCmd)
{
   #if DEBUG_DEFAULT_COMMAND
   MessageInterface::ShowMessage
      ("Moderator::CreateDefaultCommand() entered: type = " +
       type + ", name = " + name + "\n");
   #endif
   
   GmatCommand *cmd = theFactoryManager->CreateCommand(type, name);
   
   if (cmd == NULL)
      throw GmatBaseException
         ("The Moderator cannot create a Command type \"" + type + "\"\n");
   
   #ifdef DEBUG_MEMORY
   if (cmd)
   {
      std::string funcName;
      funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
      MemoryTracker::Instance()->Add
         (cmd, type, "Moderator::CreateDefaultCommand()", funcName);
   }
   #endif
   
   Integer id;
   
   try
   {
      if (type == "If" || type == "While")
      {
         std::string str = GetDefaultSpacecraft()->GetName() + ".ElapsedDays";
         cmd->SetCondition(str, "<", "1.0");
      }
      else if (type == "For")
      {
         CreateParameter("Variable", "I");
         cmd->SetStringParameter("IndexName", "I");
         cmd->SetStringParameter("StartName", "1");
         cmd->SetStringParameter("EndName", "10");
      }
      else if (type == "Save")
      {
         cmd->SetRefObjectName(Gmat::SPACECRAFT,
                               GetDefaultSpacecraft()->GetName());
      }
      else if (type == "Toggle")
      {
         cmd->SetStringParameter(cmd->GetParameterID("Subscriber"),
                                 GetDefaultSubscriber("OrbitView")->GetName());
      }
      else if (type == "Report")
      {
         Subscriber *sub = GetDefaultSubscriber("ReportFile", false);
         Parameter *param = GetDefaultX();
         cmd->SetStringParameter("ReportFile", sub->GetName());
         cmd->SetStringParameter("Add", param->GetName());
         cmd->SetRefObject(sub, Gmat::SUBSCRIBER, sub->GetName(), 0);
         cmd->SetRefObject(param, Gmat::PARAMETER, param->GetName(), 0);
      }
      else if (type == "Propagate")
      {
         cmd->SetObject(GetDefaultPropSetup()->GetName(), Gmat::PROP_SETUP);
         
         StringArray formList = GetListOfObjects(Gmat::FORMATION);
         
         if (formList.size() == 0)
         {
            cmd->SetObject(GetDefaultSpacecraft()->GetName(), Gmat::SPACECRAFT);
         }
         else
         {
            // Get first spacecraft name not in formation
            std::string scName = GetSpacecraftNotInFormation();
            if (scName != "")
               cmd->SetObject(scName, Gmat::SPACECRAFT);
            else
               cmd->SetObject(formList[0], Gmat::SPACECRAFT);
         }
         
         cmd->SetRefObject(CreateDefaultStopCondition(), Gmat::STOP_CONDITION, "", 0);
         cmd->SetSolarSystem(theSolarSystemInUse);
      }
      else if (type == "Maneuver")
      {
         // set burn
         id = cmd->GetParameterID("Burn");
         cmd->SetStringParameter(id, GetDefaultBurn("ImpulsiveBurn")->GetName());
         
         // set spacecraft
         id = cmd->GetParameterID("Spacecraft");
         cmd->SetStringParameter(id, GetDefaultSpacecraft()->GetName());
      }
      else if (type == "BeginFiniteBurn")
      {
         // set burn
         cmd->SetRefObjectName(Gmat::FINITE_BURN, GetDefaultBurn("FiniteBurn")->GetName());
         
         // set spacecraft
         cmd->SetRefObjectName(Gmat::SPACECRAFT, GetDefaultSpacecraft()->GetName());
      }
      else if (type == "EndFiniteBurn")
      {
         // get burn name of BeginFiniteBurn
         if (refCmd)
         {
            // set burn
            cmd->SetRefObjectName(Gmat::FINITE_BURN,
                                  refCmd->GetRefObjectName(Gmat::FINITE_BURN));
            
            // set spacecraft
            StringArray scNames = refCmd->GetRefObjectNameArray(Gmat::SPACECRAFT);
            for (UnsignedInt i=0; i<scNames.size(); i++)
               cmd->SetRefObjectName(Gmat::SPACECRAFT, scNames[i]);
         }
         else
         {
            // set burn
            cmd->SetRefObjectName(Gmat::FINITE_BURN, GetDefaultBurn("FiniteBurn")->GetName());
         
            // set spacecraft
            cmd->SetRefObjectName(Gmat::SPACECRAFT, GetDefaultSpacecraft()->GetName());
         }
      }
      else if (type == "Target")
      {
         // set solver
         Solver *solver = CreateSolver("DifferentialCorrector",
                                       GetDefaultSolver()->GetName());
         id = cmd->GetParameterID("Targeter");
         cmd->SetStringParameter(id, solver->GetName());
      }
      else if (type == "Optimize")
      {
         // set solver
         Solver *solver = CreateSolver("DifferentialCorrector",
                                       GetDefaultSolver()->GetName());
         id = cmd->GetParameterID("OptimizerName");
         cmd->SetStringParameter(id, solver->GetName());
      }
      else if (type == "Vary")
      {
         // set solver
         Solver *solver = CreateSolver("DifferentialCorrector",
                                       GetDefaultSolver()->GetName());
         id = cmd->GetParameterID("SolverName");
         cmd->SetStringParameter(id, solver->GetName());
         
         // set sover pointer, so that GetGeneratingString() can write correctly
         cmd->SetRefObject(solver, Gmat::SOLVER);
         
         // set variable parameter
         id = cmd->GetParameterID("Variable");
         cmd->SetStringParameter(id, GetDefaultBurn("ImpulsiveBurn")->GetName() + ".Element1");
         
         id = cmd->GetParameterID("InitialValue");
         cmd->SetStringParameter(id, "0.5");
         
         id = cmd->GetParameterID("Perturbation");
         cmd->SetStringParameter(id, "0.0001");
         
         id = cmd->GetParameterID("Lower");
         cmd->SetStringParameter(id, "0.0");
         
         id = cmd->GetParameterID("Upper");
         std::stringstream ss("");
         ss << GmatMathConstants::PI;
         cmd->SetStringParameter(id, ss.str());
      
         id = cmd->GetParameterID("MaxStep");
         cmd->SetStringParameter(id, "0.2");
         
      }
      else if (type == "Achieve")
      {
         // Get default solver
         Solver *solver = GetDefaultSolver();

         #if DEBUG_DEFAULT_COMMAND
         MessageInterface::ShowMessage
            ("Moderator::CreateDefaultCommand() cmd=%s, solver=%s\n",
             cmd->GetTypeName().c_str(), solver->GetTypeName().c_str());
         #endif
         
         id = cmd->GetParameterID("TargeterName");
         cmd->SetStringParameter(id, solver->GetName());
         
         // set goal parameter
         id = cmd->GetParameterID("Goal");
         cmd->SetStringParameter(id, GetDefaultSpacecraft()->GetName() + ".Earth.RMAG");
         
         id = cmd->GetParameterID("GoalValue");
         cmd->SetStringParameter(id, "42165.0"); 
         
         id = cmd->GetParameterID("Tolerance");
         cmd->SetStringParameter(id, "0.1");
      }
      else
      {
         // We need to set actual command string so that it can be saved to script
         std::string typeName = cmd->GetTypeName();
         std::string genStr = cmd->GetGeneratingString();
         // If there is comment only, prepend command string
         if (GmatStringUtil::StartsWith(genStr, "%"))
            cmd->SetGeneratingString(typeName + "; " + genStr);
      }
      
      // for creating ElementWrapper
      #if DEBUG_DEFAULT_COMMAND
      MessageInterface::ShowMessage
         ("   Moderator calling theScriptInterpreter->ValidateCommand()\n");
      #endif
      theScriptInterpreter->ValidateCommand(cmd);
      
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage(e.GetFullMessage());
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   #if DEBUG_DEFAULT_COMMAND
   MessageInterface::ShowMessage
      ("Moderator::CreateDefaultCommand() returning cmd=<%p><%s>\n", cmd,
       cmd->GetTypeName().c_str());
   #endif
   
   return cmd;
}


//------------------------------------------------------------------------------
// bool AppendCommand(GmatCommand *cmd, Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Appends command to last command
 *
 * @param  cmd  The command pointer to be appended
 * @param  sandboxNum  The sandbox number
 * @return  Return true if command successfully appended
 */
//------------------------------------------------------------------------------
bool Moderator::AppendCommand(GmatCommand *cmd, Integer sandboxNum)
{
   #if DEBUG_RESOURCE_COMMAND
   MessageInterface::ShowMessage
      ("==========> Moderator::AppendCommand() cmd=(%p)%s\n",
       cmd, cmd->GetTypeName().c_str());
   #endif
   
   // Get last command and append
   GmatCommand *lastCmd = GmatCommandUtil::GetLastCommand(commands[sandboxNum-1]);
   
   #if DEBUG_RESOURCE_COMMAND
   MessageInterface::ShowMessage
      ("     lastCmd=(%p)%s\n", lastCmd, lastCmd->GetTypeName().c_str());
   #endif
   
   if (lastCmd != NULL)
      return lastCmd->Append(cmd);
   else
      return commands[sandboxNum-1]->Append(cmd);
}


//------------------------------------------------------------------------------
// GmatCommand* AppendCommand(const std::string &type, const std::string &name,
//                           bool &retFlag, Integer sandboxNum)
//------------------------------------------------------------------------------
GmatCommand* Moderator::AppendCommand(const std::string &type,
                                      const std::string &name, bool &retFlag,
                                      Integer sandboxNum)
{
   #if DEBUG_RESOURCE_COMMAND
   MessageInterface::ShowMessage
      ("==========> Moderator::AppendCommand() type='%s', name='%s'\n",
       type.c_str(), name.c_str());
   #endif
   
   GmatCommand *cmd = theFactoryManager->CreateCommand(type, name);
   
   if (cmd != NULL)
   {
      #ifdef DEBUG_MEMORY
      if (cmd)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (cmd, type, "Moderator::AppendCommand()", funcName);
      }
      #endif
      
      retFlag = AppendCommand(cmd, sandboxNum);
   }
   else
   {
      throw GmatBaseException
         ("The Moderator cannot create a Command type \"" + type + "\"\n");
   }
   
   #if DEBUG_RESOURCE_COMMAND
   MessageInterface::ShowMessage
      ("==========> Moderator::AppendCommand() returning <%p>, retFlag=%d\n",
       cmd, retFlag);
   #endif
   
   return cmd;
}


//------------------------------------------------------------------------------
// bool InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd, Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Inserts command into the command sequence after previous command
 * 
 * @param cmd  Pointer to GmatCommand that is inserted
 * @param prev Pointer to GmatCommand preceding this GmatCommand
 *
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool Moderator::InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd,
                              Integer sandboxNum)
{
   #if DEBUG_COMMAND_INSERT
   MessageInterface::ShowMessage("==========> Moderator::InsertCommand() entered\n");
   ShowCommand("     inserting cmd = ", cmd, " after prevCmd = ", prevCmd);
   #endif
   
   return commands[sandboxNum-1]->Insert(cmd, prevCmd);
}


//------------------------------------------------------------------------------
// GmatCommand* DeleteCommand(GmatCommand *cmd, Integer sandboxNum)
//------------------------------------------------------------------------------
/*
 * Removes a command from the command sequence. The caller has to delete the command.
 *
 * If deleting branch command,
 * it will remove and delete all children from the branch. If deleting ScriptEvent,
 * it will remove and delete all commands including EndScrpt between BeginScrint
 * and EndScript.
 */
//------------------------------------------------------------------------------
GmatCommand* Moderator::DeleteCommand(GmatCommand *cmd, Integer sandboxNum)
{
   #if DEBUG_COMMAND_DELETE
   ShowCommand("==========> Moderator::DeleteCommand() cmd = ", cmd);
   #endif
   
   if (cmd == NULL)
      return NULL;
   
   GmatCommand *remvCmd;
   if (cmd->GetTypeName() != "BeginScript")
   {
      GmatCommand *remvCmd = commands[sandboxNum-1]->Remove(cmd);
      
      #if DEBUG_COMMAND_DELETE
      ShowCommand("   Removed = ", remvCmd);
      #endif
      
      #if DEBUG_COMMAND_DELETE
      ShowCommand("==========> Moderator::DeleteCommand() Returning ", remvCmd);
      #endif
      
      return remvCmd;
   }
   
   //-------------------------------------------------------
   // Remove commands inside Begin/EndScript block
   //-------------------------------------------------------

   // Check for previous command, it should not be NULL,
   // since "NoOp" is the first command
   
   GmatCommand *prevCmd = cmd->GetPrevious();
   if (prevCmd == NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Moderator::DeleteCommand() *** INTERNAL ERROR *** \n"
          "The previous command cannot be NULL.\n");
      return NULL;
   }
   
   GmatCommand *first = GetFirstCommand();
   
   #if DEBUG_COMMAND_DELETE
   std::string cmdString1 = GmatCommandUtil::GetCommandSeqString(first);
   MessageInterface::ShowMessage("     ==> Current sequence:");
   MessageInterface::ShowMessage(cmdString1);
   #endif
   
   GmatCommand *current = cmd->GetNext();
   
   #if DEBUG_COMMAND_DELETE
   GmatCommand *nextCmd = GmatCommandUtil::GetNextCommand(cmd);
   ShowCommand("     prevCmd = ", prevCmd, " nextCmd = ", nextCmd);
   #endif
   
   // Get matching EndScript for BeginScript
   GmatCommand *endScript = GmatCommandUtil::GetMatchingEnd(cmd);
   
   #if DEBUG_COMMAND_DELETE
   ShowCommand("     endScript = ", endScript);
   #endif
   
   GmatCommand* next;
   while (current != NULL)
   {
      #if DEBUG_COMMAND_DELETE
      ShowCommand("     current = ", current);
      #endif
      
      if (current == endScript)
         break;
      
      next = current->GetNext();
      
      #if DEBUG_COMMAND_DELETE
      ShowCommand("     removing and deleting ", current);
      #endif
      
      remvCmd = cmd->Remove(current);
      
      // check remvCmd first
      if (remvCmd != NULL)
      {
         remvCmd->ForceSetNext(NULL);
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (remvCmd, remvCmd->GetTypeName(), "Moderator::DeleteCommand()");
         #endif
         delete remvCmd;
      }
      
      current = next;
   }
   
   //-------------------------------------------------------
   // Remove and delete EndScript
   //-------------------------------------------------------
   #if DEBUG_COMMAND_DELETE
   ShowCommand("     removing and deleting ", current);
   #endif
   
   remvCmd = cmd->Remove(current);
   remvCmd->ForceSetNext(NULL);
   
   if (remvCmd != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (remvCmd, remvCmd->GetTypeName(), "Moderator::DeleteCommand()");
      #endif
      delete remvCmd;
      remvCmd = NULL;
   }
   
   next = cmd->GetNext();
   
   #if DEBUG_COMMAND_DELETE
   ShowCommand("     next    = ", next, " nextCmd = ", nextCmd);
   #endif
   
   //-------------------------------------------------------
   // Remove and delete BeginScript
   //-------------------------------------------------------
   #if DEBUG_COMMAND_DELETE
   ShowCommand("     removing and deleting ", cmd);
   #endif
   
   // Remove BeginScript
   remvCmd = first->Remove(cmd);
   
   // Set next command NULL
   cmd->ForceSetNext(NULL);
   if (cmd != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (cmd, cmd->GetTypeName(), "Moderator::DeleteCommand()");
      #endif
      delete cmd;
      cmd = NULL;
   }
   
   #if DEBUG_COMMAND_DELETE
   std::string cmdString2 = GmatCommandUtil::GetCommandSeqString(first);
   MessageInterface::ShowMessage("     ==> sequence after delete:");
   MessageInterface::ShowMessage(cmdString2);
   ShowCommand("==========> Moderator::DeleteCommand() Returning cmd = ", cmd);
   #endif
   
   // Just return cmd, it should be deleted by the caller.
   return cmd;
}


//------------------------------------------------------------------------------
// GmatCommand* GetFirstCommand(Integer sandboxNum)
//------------------------------------------------------------------------------
GmatCommand* Moderator::GetFirstCommand(Integer sandboxNum)
{
   return commands[sandboxNum-1];
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


//------------------------------------------------------------------------------
// void ValidateCommand(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Validates the command.
 */
//------------------------------------------------------------------------------
void Moderator::ValidateCommand(GmatCommand *cmd)
{
   theScriptInterpreter->ValidateCommand(cmd);
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
// const StringArray& GetPlanetarySourceTypes()
//------------------------------------------------------------------------------
/**
 * @return a planetary source types
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetPlanetarySourceTypes()
{
   return theSolarSystemInUse->GetPlanetarySourceTypes();
}


//------------------------------------------------------------------------------
// const StringArray& GetPlanetarySourceNames()
//------------------------------------------------------------------------------
/**
 * @return a planetary source file names
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetPlanetarySourceNames()
{
   return theSolarSystemInUse->GetPlanetarySourceNames();
}


//------------------------------------------------------------------------------
// const StringArray& GetPlanetarySourceTypesInUse()
//------------------------------------------------------------------------------
/**
 * @return a planetary source types in use
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetPlanetarySourceTypesInUse()
{
   return theSolarSystemInUse->GetPlanetarySourceTypesInUse();
}


////------------------------------------------------------------------------------
//// StringArray& GetAnalyticModelNames()
////------------------------------------------------------------------------------
///**
// * @return available planetary analytic model names.
// */
////------------------------------------------------------------------------------
//const StringArray& Moderator::GetAnalyticModelNames()
//{
//   return theSolarSystemInUse->GetAnalyticModelNames();
//}
//
//
////------------------------------------------------------------------------------
//// bool SetAnalyticModelToUse(const std::string &modelName)
////------------------------------------------------------------------------------
//bool Moderator::SetAnalyticModelToUse(const std::string &modelName)
//{
//   return theSolarSystemInUse->SetAnalyticModelToUse(modelName);
//}


//------------------------------------------------------------------------------
// bool SetPlanetarySourceName(const std::string &sourceType,
//                           const std::string &fileName)
//------------------------------------------------------------------------------
bool Moderator::SetPlanetarySourceName(const std::string &sourceType,
                                       const std::string &fileName)
{
   return theSolarSystemInUse->SetPlanetarySourceName(sourceType, fileName);
}


//------------------------------------------------------------------------------
// std::string GetPlanetarySourceName(const std::string &sourceType)
//------------------------------------------------------------------------------
std::string Moderator::GetPlanetarySourceName(const std::string &sourceType)
{
   return theSolarSystemInUse->GetPlanetarySourceName(sourceType);
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
   return theSolarSystemInUse->SetPlanetarySourceTypesInUse(sourceTypes);
}


//------------------------------------------------------------------------------
// Integer GetPlanetarySourceId(const std::string &sourceType)
//------------------------------------------------------------------------------
Integer Moderator::GetPlanetarySourceId(const std::string &sourceType)
{
   return theSolarSystemInUse->GetPlanetarySourceId(sourceType);
}


//------------------------------------------------------------------------------
// std::string GetPlanetarySourceNameInUse()
//------------------------------------------------------------------------------
std::string Moderator::GetCurrentPlanetarySource()
{
   return theSolarSystemInUse->GetCurrentPlanetarySource();
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
   else if (fileType == "EGM96")
      return theFileManager->GetFullPathname("EGM96_FILE");
   else if (fileType == "LP165P")
      return theFileManager->GetFullPathname("LP165P_FILE");
   else if (fileType == "MGNP180U")
      return theFileManager->GetFullPathname("MGNP180U_FILE");
   else if (fileType == "MARS50C")
      return theFileManager->GetFullPathname("MARS50C_FILE");
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
   #if DEBUG_DEFAULT_MISSION
   MessageInterface::ShowMessage("Moderator::LoadDefaultMission() entered\n");
   #endif
   
   theScriptInterpreter->SetHeaderComment("");
   theScriptInterpreter->SetFooterComment("");
   
   ClearCommandSeq(true, true);
   ClearResource();
   
   // Set object manage option to configuration
   objectManageOption = 1;
   
   CreateDefaultMission();
   
   #if DEBUG_DEFAULT_MISSION
   MessageInterface::ShowMessage("Moderator::LoadDefaultMission() leaving\n");
   #endif
   
   return true;
}

// Resource
//------------------------------------------------------------------------------
// bool ClearResource()
//------------------------------------------------------------------------------
bool Moderator::ClearResource()
{
   #if DEBUG_SEQUENCE_CLEARING
   MessageInterface::ShowMessage("Moderator::ClearResource() entered\n");
   MessageInterface::ShowMessage("   Removing configured objects...\n");
   #endif
   
   theConfigManager->RemoveAllItems();
   
   #if DEBUG_SEQUENCE_CLEARING
   MessageInterface::ShowMessage("   Clearing Sandbox objects...\n");
   #endif
   
   ClearAllSandboxes();
   
   // Delete solar system in use. We want to begin with default solar system
   // before creating default mission or new script is read.
   #ifndef __DISABLE_SOLAR_SYSTEM_CLONING__
   // Do not delete SolarSystem in case user wants to create a new mission from
   // the GUI after getting errors in script build. This will fix Bug 1532 (LOJ: 2009.11.13)
   if (!isRunReady && endOfInterpreter)
   {
      #if DEBUG_SEQUENCE_CLEARING | DEBUG_FINALIZE > 0
      MessageInterface::ShowMessage
         (".....Mod::ClearResource - <%p>theSolarSystemInUse was not deleted since "
          "there was script errors\n", theSolarSystemInUse);
      #endif
   }
   else
   {
      if (theSolarSystemInUse != NULL)
      {
         #if DEBUG_SEQUENCE_CLEARING | DEBUG_FINALIZE > 0
         MessageInterface::ShowMessage
            (".....Mod::ClearResource - deleting (%p)theSolarSystemInUse\n", theSolarSystemInUse);
         #endif
         if (theInternalSolarSystem == theSolarSystemInUse) theInternalSolarSystem = NULL;
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (theSolarSystemInUse, theSolarSystemInUse->GetName(),
             "deleting theSolarSystemInUse in Moderator::ClearResource()");
         #endif
         
         delete theSolarSystemInUse;
         theSolarSystemInUse = NULL;
      }
   }
   #endif
   
   #if DEBUG_SEQUENCE_CLEARING
   MessageInterface::ShowMessage("Moderator::ClearResource() returning true\n");
   #endif
   
   return true;
}

// Command Sequence
//------------------------------------------------------------------------------
// bool ClearCommandSeq(bool leaveFirstCmd, bool callRunComplete,
//                     Integer sandboxNum)
//------------------------------------------------------------------------------
/*
 * Deletes whole command sequence.
 *
 * @param  leaveFirstCmd  Set this flag to true if the first command should be
 *                        left undeleted (true)
 * @param  callRunComplete  Set this flag to true if RunComplete() should be
 *                        called for all commands (true)
 * @param  sandBoxNum  The sandobx number (1)
 */
//------------------------------------------------------------------------------
bool Moderator::ClearCommandSeq(bool leaveFirstCmd, bool callRunComplete,
                                Integer sandboxNum)
{
   #if DEBUG_SEQUENCE_CLEARING
   MessageInterface::ShowMessage("Moderator::ClearCommandSeq() entered\n");
   #endif
   
   if (commands.empty())
   {
      #if DEBUG_SEQUENCE_CLEARING
      MessageInterface::ShowMessage
         ("Moderator::ClearCommandSeq() exiting, command array is empty\n");
      #endif
      return true;
   }
   
   GmatCommand *cmd = commands[sandboxNum-1];
   bool retval =
      GmatCommandUtil::ClearCommandSeq(cmd, leaveFirstCmd, callRunComplete);
   
   #if DEBUG_SEQUENCE_CLEARING
   MessageInterface::ShowMessage
      ("Moderator::ClearCommandSeq() returning %s\n", retval ? "true" : "false");
   #endif
   
   return retval;
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
   
   #ifdef DEBUG_MEMORY
   StringArray tracks = MemoryTracker::Instance()->GetTracks(false, false);
   MessageInterface::ShowMessage
      ("===> There are %d memory tracks after Sandbox clear\n", tracks.size());
   #endif
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
/*
 * Adds configured objects to sandbox and execute. The number of sandbox created
 * is declared in ther header as Gmat::MAX_SANDBOX. But currently only 1 sandbox
 * is used for running the mission.
 *
 * @param  sandboxNum  The sandbox number (1 to Gmat::MAX_SANDBOX)
 *
 * @return  1 if run was successful
 *         -1 if sandbox number is invalid
 *         -2 if execution interrupted by user
 *         -3 if exception thrown during the run
 *         -4 if unknown error occurred
 */
//------------------------------------------------------------------------------
Integer Moderator::RunMission(Integer sandboxNum)
{
   //MessageInterface::ShowMessage("\n========================================\n");
   //MessageInterface::ShowMessage("Moderator::RunMission() entered\n");
   MessageInterface::ShowMessage("Running mission...\n");
   Integer status = 1;
   // Set to 1 to always run the mission and get the sandbox error message
   // Changed this code while looking at Bug 1532 (LOJ: 2009.11.13)
   isRunReady = 1;
   
   #if DEBUG_CONFIG
   MessageInterface::ShowMessage
      ("Moderator::RunMission() HasConfigurationChanged()=%d\n", HasConfigurationChanged());
   #endif
   
   clock_t t1 = clock(); // Should I clock after initilization?
   
   if (isRunReady)
   {
      // clear sandbox
      if (sandboxNum > 0 && sandboxNum <= Gmat::MAX_SANDBOX)
      {
         #if DEBUG_RUN
         MessageInterface::ShowMessage
            ("Moderator::RunMission() before sandboxes[%d]->Clear()\n", sandboxNum-1);
         #endif
         
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
         
         // add objects to sandbox
         AddSolarSystemToSandbox(sandboxNum-1);
         AddTriggerManagersToSandbox(sandboxNum-1);
         AddInternalCoordSystemToSandbox(sandboxNum-1);
         AddPublisherToSandbox(sandboxNum-1);
         AddSubscriberToSandbox(sandboxNum-1);
         AddOtherObjectsToSandbox(sandboxNum-1);
         
         // add command sequence to sandbox
         AddCommandToSandbox(sandboxNum-1);
         
         #if DEBUG_RUN
         MessageInterface::ShowMessage
            ("Moderator::RunMission() after AddCommandToSandbox()\n");
         #endif
         
         // initialize Sandbox
         InitializeSandbox(sandboxNum-1);
         
         #if DEBUG_RUN
         MessageInterface::ShowMessage
            ("Moderator::RunMission() after InitializeSandbox()\n");
         #endif
         
         // reset user interrupt flag
         GmatGlobal::Instance()->SetRunInterrupted(false);
         
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
         std::string msg = e.GetFullMessage();
         
         // assign status
         // Look for "interrupted" (loj: 2008.02.05)
         //if (msg.find("Execution interrupted") != msg.npos)
         if (msg.find("interrupted") != msg.npos)
         {
            status = -2;
            MessageInterface::ShowMessage("GMAT execution stopped by user.\n");      
         }
         else
         {
            status = -3;
//            msg = "**** ERROR **** " + msg;
            // Dunn would like to note that this is the popup message we were
            // getting that only said "ERROR" and did not provide a message.
            // We might want to debug that some day.
            MessageInterface::PopupMessage(Gmat::ERROR_, msg + "\n");
         }
      }
      catch (...)
      {
         MessageInterface::ShowMessage
            ("Moderator::RunMission() Unknown error occurred.\n");
         status = -4;
         //throw; // LOJ: We want to finish up the clearing process below
      }
   }
   else
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Cannot Run Mission. No mission sequence defined.\n");
      status = -4;
   }
   
   runState = Gmat::IDLE;
   thePublisher->SetRunState(runState);
   thePublisher->NotifyEndOfRun();
   if (theUiInterpreter != NULL)
      theUiInterpreter->NotifyRunCompleted();
   
   #if DEBUG_RUN > 1
   MessageInterface::ShowMessage("===> status=%d\n", status);
   #endif
   
   if (status == 1)
      MessageInterface::ShowMessage("Mission run completed.\n");
   else if (status == -2)
      MessageInterface::ShowMessage("*** Mission run interrupted.\n");
   else
      MessageInterface::ShowMessage("*** Mission run failed.\n");
   
   clock_t t2 = clock();
   MessageInterface::ShowMessage
      ("===> Total Run Time: %f seconds\n", (Real)(t2-t1)/CLOCKS_PER_SEC);
   
   #ifdef DEBUG_MEMORY
   StringArray tracks = MemoryTracker::Instance()->GetTracks(false, false);
   MessageInterface::ShowMessage
      ("===> There are %d memory tracks after MissionRun\n", tracks.size());
   #endif
   
   // show final state
   #ifdef __SHOW_FINAL_STATE__
   showFinalState = true;
   #endif
   
   if (showFinalState)
   {
      GmatCommand *cmd = GetFirstCommand();
      MessageInterface::ShowMessage(GmatCommandUtil::GetCommandSeqString(cmd));
      GmatCommand *lastCmd = GmatCommandUtil::GetLastCommand(cmd);
      
      MessageInterface::ShowMessage("\n========== Final State ==========\n");
      MessageInterface::ShowMessage(lastCmd->GetStringParameter("MissionSummary"));
      MessageInterface::ShowMessage("\n\n");      
   }
   else
   {
      MessageInterface::ShowMessage("\n========================================\n");
   }
   
   // Reset solar system in use and object map (LOJ: 2009.03.19)
   // So that users can create new objects from the GUI after GmatFunction run.
   objectMapInUse = theConfigManager->GetObjectMap();
   SetSolarSystemAndObjectMap(theSolarSystemInUse, objectMapInUse, false,
                              "RunMission()");
   
   return status;
} // RunMission()


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
   MessageInterface::ShowMessage
      ("Moderator::ChangeRunState(%s) entered\n", state.c_str());
   #endif
   
   if (state == "Stop")
   {
      runState = Gmat::IDLE;
      GmatGlobal::Instance()->SetRunInterrupted(true);
   }
   
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
   if (theUiInterpreter != NULL)
      theUiInterpreter->SetInputFocus();
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
// bool InterpretScript(const std::string &filename, bool readBack = false,
//                      const std::string &newPath = "")
//------------------------------------------------------------------------------
/**
 * Creates objects from script file. If readBack is true, it will save to
 * to new directory and interpret from it.  If newPath is blank "", then
 * it will create default directory "AutoSaved".
 *
 * @param <filename> input script file name
 * @param <readBack> true will read scripts, save, and read back in
 * @param <newPath> new path to be used for saving scripts
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::InterpretScript(const std::string &filename, bool readBack,
                                const std::string &newPath)
{
   bool status = false;
   isRunReady = false;
   endOfInterpreter = false;
   runState = Gmat::IDLE;
   
   //MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage
      ("\nInterpreting scripts from the file.\n***** file: " + filename + "\n");
   
   try
   {
      PrepareNextScriptReading();
      status = theScriptInterpreter->Interpret(filename);
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->SetScript(filename);
      #endif
      
      if (readBack)
      {
         #if DEBUG_INTERPRET
         MessageInterface::ShowMessage("===> newPath=%s\n", newPath.c_str());
         #endif
         
         std::string newpath = newPath;
         std::string sep = theFileManager->GetPathSeparator();
         UnsignedInt index = filename.find_last_of("/\\");
         std::string fname = filename.substr(index+1);
         
         if (newpath == "")
            newpath = filename.substr(0, index) + sep + "AutoSave" + sep;
         
         std::string newfile = newpath + fname;
         
         #if DEBUG_INTERPRET
         MessageInterface::ShowMessage
            ("===> newpath=%s\n===> newfile=%s\n", newpath.c_str(), newfile.c_str());
         #endif
         
         if (!theFileManager->DoesDirectoryExist(newpath))
         {
            std::string cmd = "mkdir " + newpath;
            
            int status = system(cmd.c_str());
            if (status != 0)
            {
               #if DEBUG_INTERPRET
               MessageInterface::ShowMessage
                  ("===> cmd=%s, status=%d\n", cmd.c_str(), status);
               #endif
            }
         }
         
         SaveScript(newfile);
         InterpretScript(newfile);
      }
      
      if (status)
      {
         #if DEBUG_INTERPRET
         MessageInterface::ShowMessage
             ("Moderator::InterpretScript() creating Default Coordinate "
              "System...\n");
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
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      isRunReady = false;
   }
   
   ResetConfigurationChanged();
   endOfInterpreter = true;
   
   // Append BeginMissionSequence command if not there (New since 2010.07.09)
   GmatCommand *first = GetFirstCommand();
   GmatCommand *second = first->GetNext();
   
   #if DEBUG_INTERPRET
   ShowCommand("first cmd = ", first, " second cmd = ", second);
   #endif
   
   if (second != NULL && second->GetTypeName() != "BeginMissionSequence")
   {
      // Show warning message for now (LOJ: 2010.07.15)
      std::string firstCmdStr = "The first command detected is \n'";
      firstCmdStr = firstCmdStr + second->GetGeneratingString(Gmat::NO_COMMENTS) + "'";
      //firstCmdStr = firstCmdStr + second->GetGeneratingString() + "'";
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "*** WARNING *** BeginMissionSequence is missing. "
          "It will be required in future builds.\n" + firstCmdStr);
      
      #if DEBUG_INTERPRET
      MessageInterface::ShowMessage
         ("==> Inserting 'BeginMissionSequence' after '%s'\n",
          first->GetTypeName().c_str());
      #endif
      bool retval;
      GmatCommand *bms = CreateCommand("BeginMissionSequence", "", retval);
      InsertCommand(bms, first);
   }
   
   #if DEBUG_INTERPRET > 1
   MessageInterface::ShowMessage(GetScript());
   #endif
   
   #if DEBUG_INTERPRET > 0
   GmatCommand *cmd = GetFirstCommand();
   MessageInterface::ShowMessage(GmatCommandUtil::GetCommandSeqString(cmd));
   MessageInterface::ShowMessage("Moderator::InterpretScript() returning %d\n", status);
   #endif
   
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
   MessageInterface::ShowMessage
      ("\nInterpreting scripts from the input stream\n");
   
   try
   {
      PrepareNextScriptReading(clearObjs);
      
      // Set istream and Interpret
      theScriptInterpreter->SetInStream(ss);
      status = theScriptInterpreter->Interpret();
      
      if (status)
      {
         #if DEBUG_INTERPRET
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
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      isRunReady = false;
   }
   
   ResetConfigurationChanged();
   endOfInterpreter = true;
   
   #if DEBUG_INTERPRET
   GmatCommand *cmd = GetFirstCommand();
   MessageInterface::ShowMessage(GmatCommandUtil::GetCommandSeqString(cmd));
   MessageInterface::ShowMessage(GetScript());
   #endif
   
   return status;
}


//------------------------------------------------------------------------------
// bool SaveScript(const std::string &filename,
//                 Gmat::WriteMode mode = Gmat::SCRIPTING)
//------------------------------------------------------------------------------
/**
 * Builds scripts from objects and write to a file.
 *
 * @param <filename> output script file name
 * @param <writeMode> write mode object(one of Gmat::SCRIPTING, Gmat::MATLAB_STRUCT)
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::SaveScript(const std::string &filename, Gmat::WriteMode mode)
{
   #ifdef DEBUG_SAVE_SCRIPT
   MessageInterface::ShowMessage
      ("Moderator::SaveScript() entered\n   file: %s, mode: %d\n",
       filename.c_str(), mode);
   MessageInterface::ShowMessage("The Script is saved to " + filename + "\n");
   #endif
   
   bool status = false;
   
   try
   {
      status = theScriptInterpreter->Build(filename, mode);
      if (status)
         ResetConfigurationChanged();
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   return status;
}


//------------------------------------------------------------------------------
// std::string GetScript(Gmat::WriteMode mode = Gmat::SCRIPTING)
//------------------------------------------------------------------------------
/**
 * Returns built scripts from objects
 *
 * @param <writeMode> write mode object(one of Gmat::SCRIPTING, Gmat::MATLAB_STRUCT)
 *
 * @return built scripts from objects
 */
//------------------------------------------------------------------------------
std::string Moderator::GetScript(Gmat::WriteMode mode)
{
   //MessageInterface::ShowMessage("Moderator::GetScript() mode: %d\n", mode);
   
   try
   {
      std::stringstream os;
      theScriptInterpreter->SetOutStream(&os);
      
      if (theScriptInterpreter->Build(mode))
      {
         return os.str();
      }
      else
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Unable to build script from objects\n");
         return "";
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage() + "\n");
      return "";
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

//------------------------------------------------------------------------------
// bool StartMatlabServer()
//------------------------------------------------------------------------------
/**
 * Interface used to tell an Interpreter to start the MATLAB server.
 */
//------------------------------------------------------------------------------
bool Moderator::StartMatlabServer()
{
   if (theUiInterpreter != NULL)
   {
      theUiInterpreter->StartMatlabServer();
      return true;
   }
   return false;
}

//------------------------------------------------------------------------------
// std::vector<Gmat::PluginResources*> *Moderator::GetPluginResourceList()
//------------------------------------------------------------------------------
/**
 * Passes the list of plugin resources to the GUI
 *
 * @return The resource list parsed during initialization
 */
//------------------------------------------------------------------------------
std::vector<Gmat::PluginResource*> *Moderator::GetPluginResourceList()
{
   return &userResources;
}

//---------------------------------
//  private
//---------------------------------

// initialization
//------------------------------------------------------------------------------
// void CreatePlanetaryCoeffFile()
//------------------------------------------------------------------------------
void Moderator::CreatePlanetaryCoeffFile()
{
   #if DEBUG_INITIALIZE
   //MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("Moderator initializing planetary coefficient file...\n");
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
   GmatGlobal::Instance()->SetItrfCoefficientsFile(theItrfFile);
}


//------------------------------------------------------------------------------
// void CreateTimeFile()
//------------------------------------------------------------------------------
void Moderator::CreateTimeFile()
{
   #if DEBUG_INITIALIZE
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
   GmatGlobal::Instance()->SetEopFile(theEopFile);
}


// prepare next script reading
//------------------------------------------------------------------------------
// void PrepareNextScriptReading(bool clearObjs = true)
//------------------------------------------------------------------------------
/*
 * This method prepares for next script reading by clearing commands and resource,
 * resetting object pointers
 *
 * @param <clearObjs> set to true if clearing commands and resource (true)
 */
//------------------------------------------------------------------------------
void Moderator::PrepareNextScriptReading(bool clearObjs)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::PrepareNextScriptReading() entered, %sclearing objects\n%s\n",
       clearObjs ? "" : "Not",
       "======================================================================");
   #endif
   
   // Set object manage option to configuration
   objectManageOption = 1;
   
   // Clear command sequence before resource (loj: 2008.07.10)
   if (clearObjs)
   {
      //clear both resource and command sequence
      #if DEBUG_RUN
      MessageInterface::ShowMessage(".....Clearing both resource and command sequence...\n");
      #endif
      
      ClearCommandSeq(true, true);
      ClearResource();
   }
   
   // Set object map in use
   objectMapInUse = theConfigManager->GetObjectMap();
   
   #ifdef DEBUG_OBJECT_MAP
   MessageInterface::ShowMessage
      ("ObjectMapInUse was set to the configuration map <%p>\n", objectMapInUse);
   #endif
   
   #if DEBUG_RUN
   MessageInterface::ShowMessage(".....Creating SolarSystem in use...\n");
   #endif
   CreateSolarSystemInUse();
   
   // Need default CS's in case they are used in the script
   #if DEBUG_RUN
   MessageInterface::ShowMessage(".....Creating Default CoordinateSystem...\n");
   #endif
   CreateDefaultCoordSystems();
   
   #if DEBUG_OBJECT_MAP > 1
   ShowObjectMap("   Moderator::PrepareNextScriptReading() Here is the configured object map");
   #endif
   
   // Set solar system in use and object map (loj: 2008.03.31)
   #if DEBUG_RUN
   MessageInterface::ShowMessage(".....Setting SolarSystem and ObjectMap to Interpreter...\n");
   #endif
   
   // Reset initial solar system in use and object map 
   SetSolarSystemAndObjectMap(theSolarSystemInUse, objectMapInUse, false,
                              "PrepareNextScriptReading()");
   currentFunction = NULL;
   
   // Delete unmanaged functions (LOJ: 2009.03.24)
   // This causes crash on Func_AssignmentTest after running
   // Func_MatlabObjectPassingCheck.script
   // so disable until it is fully tested (LOJ: 2009.04.08)
   #ifdef __ENABLE_CLEAR_UNMANAGED_FUNCTIONS__
   #if DEBUG_RUN > 0
   MessageInterface::ShowMessage
      (".....Moderator::PrepareNextScriptReading() deleting %d unmanaged functions\n",
       unmanagedFunctions.size());
   #endif
   for (UnsignedInt i=0; i<unmanagedFunctions.size(); i++)
   {
      GmatBase *func = (GmatBase*)(unmanagedFunctions[i]);
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (func, func->GetName(), "Moderator::PrepareNextScriptReading()",
          "deleting unmanaged function");
      #endif
      delete func;
      func = NULL;
   }
   unmanagedFunctions.clear();
   #endif
   
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::PrepareNextScriptReading() exiting\n"
       "======================================================================\n");
   #endif
} // PrepareNextScriptReading


//------------------------------------------------------------------------------
// void CreateSolarSystemInUse()
//------------------------------------------------------------------------------
/**
 * Creates SolarSystem in use by cloning the default SolarSystem if cloning
 * SolarSystem is enabled, else it just copies value of the default SolarSystem
 * to SolarSystem in use.  It also creates internal CoordinateSystem in use.
 */
//------------------------------------------------------------------------------
void Moderator::CreateSolarSystemInUse()
{
   //-----------------------------------------------------------------
   #ifndef __DISABLE_SOLAR_SYSTEM_CLONING__
   //-----------------------------------------------------------------   
      // delete old SolarSystem in use and create new from default
      if (theSolarSystemInUse != NULL)
      {
         #if DEBUG_INITIALIZE
         MessageInterface::ShowMessage
            ("Moderator deleting old solar system in use <%p>\n", theSolarSystemInUse);
         #endif
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (theSolarSystemInUse, theSolarSystemInUse->GetName(),
             "Moderator::CreateSolarSystemInUse()");
         #endif
         delete theSolarSystemInUse;
      }
      
      theSolarSystemInUse = NULL;
      
      #if DEBUG_SOLAR_SYSTEM_IN_USE
      MessageInterface::ShowMessage
         (".....Setting theSolarSystemInUse to clone of theDefaultSolarSystem <%p>...\n",
          theDefaultSolarSystem);
      #endif
      
      theSolarSystemInUse = theDefaultSolarSystem->Clone();
      theSolarSystemInUse->SetName("SolarSystem");
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (theSolarSystemInUse, theSolarSystemInUse->GetName(),
          "Moderator::CreateSolarSystemInUse()",
          "theSolarSystemInUse = theDefaultSolarSystem->Clone()");
      #endif
      
      #if DEBUG_SOLAR_SYSTEM_IN_USE
      MessageInterface::ShowMessage
         (".....Setting SolarSystemInUse to theInternalSolarSystem...\n");
      #endif
      theInternalSolarSystem = theSolarSystemInUse;
      
      // set solar system in use
      SetSolarSystemInUse(theSolarSystemInUse);
      
      #if DEBUG_INITIALIZE
      MessageInterface::ShowMessage
         ("Moderator created new solar system in use: %p\n", theSolarSystemInUse);
      #endif
      
   //-----------------------------------------------------------------
   #else
   //-----------------------------------------------------------------
      if (theSolarSystemInUse == NULL)
      {
         theSolarSystemInUse = theDefaultSolarSystem->Clone();
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (theSolarSystemInUse, theSolarSystemInUse->GetName(),
             "Moderator::CreateSolarSystemInUse()",
             "theSolarSystemInUse = theDefaultSolarSystem->Clone()");
         #endif
         theSolarSystemInUse->SetName("SolarSystem");
         SetSolarSystemInUse(theSolarSystemInUse);
      }
      else
      {
         theSolarSystemInUse->Copy(theDefaultSolarSystem);
         theSolarSystemInUse->SetName("SolarSystem");
      }
      
      theInternalSolarSystem = theSolarSystemInUse;
      
      #if DEBUG_SOLAR_SYSTEM_IN_USE
      MessageInterface::ShowMessage
         ("Moderator::CreateSolarSystemInUse() theSolarSystemInUse=<%p>, "
          "theDefaultSolarSystem=<%p>\n", theSolarSystemInUse,  theDefaultSolarSystem);
      #endif
   //-----------------------------------------------------------------
   #endif
   //-----------------------------------------------------------------
   
   // delete old theInternalCoordSystem and create new one
   if (theInternalCoordSystem)
   {
      #if DEBUG_SOLAR_SYSTEM_IN_USE
      MessageInterface::ShowMessage
         (".....deleting (%p)theInternalCoordSystem\n", theInternalCoordSystem);
      #endif
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (theInternalCoordSystem, theInternalCoordSystem->GetName(),
          "Moderator::CreateSolarSystemInUse()");
      #endif
      delete theInternalCoordSystem;
      theInternalCoordSystem = NULL;
   }
   
   CreateInternalCoordSystem();   
}


//------------------------------------------------------------------------------
// void CreateInternalCoordSystem()
//------------------------------------------------------------------------------
/*
 * Creates the internal coordinate system. This coordinate system is used for
 * publishing data for OpenGL plot.
 * Currently it is EarthMJ2000Eq system.
 */
//------------------------------------------------------------------------------
void Moderator::CreateInternalCoordSystem()
{
   #if DEBUG_INITIALIZE
   //MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("Moderator creating internal coordinate system...\n");
   #endif
   
   if (theInternalCoordSystem != NULL)
   {
      #if DEBUG_INITIALIZE
      MessageInterface::ShowMessage
         ("..... <%p>theInternalCoordSystem already exists\n",theInternalCoordSystem);
      #endif
   }
   else
   {
      // Create internal CoordinateSystem with no name, since we don't want
      // it to be configured.
      theInternalCoordSystem =
         CreateCoordinateSystem("InternalEarthMJ2000Eq", true, true);
      
      #if DEBUG_INITIALIZE
      MessageInterface::ShowMessage
         (".....created  <%p>theInternalCoordSystem\n", theInternalCoordSystem);
      #endif
   }
}


//------------------------------------------------------------------------------
// void CreateDefaultCoordSystems()
//------------------------------------------------------------------------------
void Moderator::CreateDefaultCoordSystems()
{
   #if DEBUG_INITIALIZE
   MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage
      ("Moderator checking if default coordinate systems should be created...\n");
   #endif
   
   defaultCoordSystemNames.clear();
   
   try
   {
      SpacePoint *earth = (SpacePoint*)GetConfiguredObject("Earth");
      SolarSystem *ss = GetSolarSystemInUse();
      
      // EarthMJ2000Eq
      CoordinateSystem *eqcs = GetCoordinateSystem("EarthMJ2000Eq");
      defaultCoordSystemNames.push_back("EarthMJ2000Eq");
      if (eqcs == NULL)
      {
         eqcs = CreateCoordinateSystem("EarthMJ2000Eq", true);
         
         #if DEBUG_INITIALIZE
         MessageInterface::ShowMessage
            (".....created <%p>'%s'\n", eqcs, eqcs->GetName().c_str());
         #endif
      }
      else
      {
         #if DEBUG_INITIALIZE
         MessageInterface::ShowMessage
            (".....found <%p>'%s'\n", eqcs, eqcs->GetName().c_str());
         #endif
         eqcs->SetSolarSystem(ss);
         eqcs->Initialize();
      }
      
      // EarthMJ2000Ec
      CoordinateSystem *eccs = GetCoordinateSystem("EarthMJ2000Ec");
      defaultCoordSystemNames.push_back("EarthMJ2000Ec");
      if (eccs == NULL)
      {
         eccs = CreateCoordinateSystem("EarthMJ2000Ec", false);
         #if DEBUG_INITIALIZE
         MessageInterface::ShowMessage
            (".....created <%p>'%s'\n", eccs, eccs->GetName().c_str());
         #endif
         AxisSystem *ecAxis = CreateAxisSystem("MJ2000Ec", "MJ2000Ec_Earth");
         eccs->SetStringParameter("Origin", "Earth");
         eccs->SetStringParameter("J2000Body", "Earth");
         eccs->SetRefObject(ecAxis, Gmat::AXIS_SYSTEM, ecAxis->GetName());
         eccs->SetOrigin(earth);
         eccs->SetJ2000Body(earth);
         eccs->SetSolarSystem(ss);
         eccs->Initialize();
         
         // Since CoordinateSystem clones AxisSystem, delete it from here
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (ecAxis, "localAxes", "Moderator::CreateDefaultCoordSystems()",
             "deleting localAxes");
         #endif
         delete ecAxis;
      }
      else
      {
         #if DEBUG_INITIALIZE
         MessageInterface::ShowMessage
            (".....found <%p>'%s'\n", eccs, eccs->GetName().c_str());
         #endif
         eccs->SetSolarSystem(ss);
         eccs->Initialize();
      }
      
      // EarthFixed
      CoordinateSystem *bfcs = GetCoordinateSystem("EarthFixed");
      defaultCoordSystemNames.push_back("EarthFixed");
      if (bfcs == NULL)
      {
         bfcs = CreateCoordinateSystem("EarthFixed", false);
         #if DEBUG_INITIALIZE
         MessageInterface::ShowMessage
            (".....created <%p>'%s'\n", bfcs, bfcs->GetName().c_str());
         #endif
         BodyFixedAxes *bfecAxis =
            (BodyFixedAxes*)CreateAxisSystem("BodyFixed", "BodyFixed_Earth");
         bfecAxis->SetEopFile(theEopFile);
         bfecAxis->SetCoefficientsFile(theItrfFile);
         bfcs->SetStringParameter("Origin", "Earth");
         bfcs->SetStringParameter("J2000Body", "Earth");
         bfcs->SetRefObject(bfecAxis, Gmat::AXIS_SYSTEM, bfecAxis->GetName());
         bfcs->SetOrigin(earth);
         bfcs->SetJ2000Body(earth);
         bfcs->SetSolarSystem(ss);
         bfcs->Initialize();
         
         // Since CoordinateSystem clones AxisSystem, delete it from here
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (bfecAxis, "localAxes", "Moderator::CreateDefaultCoordSystems()",
             "deleting localAxes");
         #endif
         delete bfecAxis;
      }
      else
      {
         #if DEBUG_INITIALIZE
         MessageInterface::ShowMessage
            (".....found <%p>'%s'\n", bfcs, bfcs->GetName().c_str());
         #endif
         bfcs->SetSolarSystem(ss);
         bfcs->Initialize();
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_,
          "Moderator::CreateDefaultCoordSystems() Error occurred during default "
          "coordinate system creation. " +  e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void CreateDefaultMission()
//------------------------------------------------------------------------------
void Moderator::CreateDefaultMission()
{
   #if DEBUG_INITIALIZE
   MessageInterface::ShowMessage("========================================\n");
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
      
      #ifdef __CREATE_HARDWARE__
      // Hardware 
      CreateHardware("FuelTank", "DefaultFuelTank");
      CreateHardware("Thruster", "DefaultThruster");
      
      #if DEBUG_DEFAULT_MISSION > 0
      MessageInterface::ShowMessage("-->default hardware created\n");
      #endif
      #endif
      
      #ifdef __CREATE_VNB_COORD__
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
      vnb->SetRefObject(orAxis, Gmat::AXIS_SYSTEM, orAxis->GetName());
      
      // Since CoordinateSystem clones AxisSystem, delete it from here
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (orAxis, "localAxes", "Moderator::CreateDefaultMission()",
          "deleting localAxes");
      #endif
      delete orAxis;
      
      #if DEBUG_DEFAULT_MISSION > 0
      MessageInterface::ShowMessage("-->default vnb coordinate system created\n");
      #endif
      #endif
      
      // ImpulsiveBurn
      GetDefaultBurn("ImpulsiveBurn");
      #if DEBUG_DEFAULT_MISSION > 0
      MessageInterface::ShowMessage("-->default impulsive burn created\n");
      #endif
      
      // ImpulsiveBurn parameters
      CreateParameter("Element1", "DefaultIB.Element1");
      CreateParameter("Element2", "DefaultIB.Element2");
      CreateParameter("Element3", "DefaultIB.Element3");
      CreateParameter("V", "DefaultIB.V");
      CreateParameter("N", "DefaultIB.N");
      CreateParameter("B", "DefaultIB.B");
      #if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default impulsive burn parameters created\n");
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
      #if DEBUG_DEFAULT_MISSION > 1
      MessageInterface::ShowMessage("-->default time parameters  created\n");
      #endif
      
      // Cartesian parameters
      CreateParameter("X", "DefaultSC.EarthMJ2000Eq.X");
      CreateParameter("Y", "DefaultSC.EarthMJ2000Eq.Y");
      CreateParameter("Z", "DefaultSC.EarthMJ2000Eq.Z");
      CreateParameter("VX", "DefaultSC.EarthMJ2000Eq.VX");
      CreateParameter("VY", "DefaultSC.EarthMJ2000Eq.VY");
      CreateParameter("VZ", "DefaultSC.EarthMJ2000Eq.VZ");
      #if DEBUG_DEFAULT_MISSION > 1
      MessageInterface::ShowMessage("-->default cartesian parameters created\n");
      #endif
      
      // Keplerian parameters
      CreateParameter("SMA", "DefaultSC.Earth.SMA");
      CreateParameter("ECC", "DefaultSC.Earth.ECC");
      CreateParameter("INC", "DefaultSC.Earth.INC");
      CreateParameter("RAAN", "DefaultSC.Earth.RAAN");
      CreateParameter("AOP", "DefaultSC.EarthMJ2000Eq.AOP");
      CreateParameter("TA", "DefaultSC.Earth.TA");
      CreateParameter("MA", "DefaultSC.Earth.MA");
      CreateParameter("EA", "DefaultSC.Earth.EA");
      CreateParameter("HA", "DefaultSC.Earth.HA");
      CreateParameter("MM", "DefaultSC.Earth.MM");
      #if DEBUG_DEFAULT_MISSION > 1
      MessageInterface::ShowMessage("-->default keplerian parameters created\n");
      #endif
      
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
      #if DEBUG_DEFAULT_MISSION > 1
      MessageInterface::ShowMessage("-->default orbital parameters created\n");
      #endif
      
      // Spherical parameters
      CreateParameter("RMAG", "DefaultSC.Earth.RMAG");
      CreateParameter("RA", "DefaultSC.Earth.RA");
      CreateParameter("DEC", "DefaultSC.EarthMJ2000Eq.DEC");
      CreateParameter("VMAG", "DefaultSC.EarthMJ2000Eq.VMAG");
      CreateParameter("RAV", "DefaultSC.EarthMJ2000Eq.RAV");
      CreateParameter("DECV", "DefaultSC.EarthMJ2000Eq.DECV");
      CreateParameter("AZI", "DefaultSC.EarthMJ2000Eq.AZI");
      CreateParameter("FPA", "DefaultSC.EarthMJ2000Eq.FPA");
      #if DEBUG_DEFAULT_MISSION > 1
      MessageInterface::ShowMessage("-->default spherical parameters created\n");
      #endif
      
      // Angular parameters
      CreateParameter("SemilatusRectum", "DefaultSC.Earth.SemilatusRectum");
      CreateParameter("HMAG", "DefaultSC.HMAG");
      CreateParameter("HX", "DefaultSC.EarthMJ2000Eq.HX");
      CreateParameter("HY", "DefaultSC.EarthMJ2000Eq.HY");
      CreateParameter("HZ", "DefaultSC.EarthMJ2000Eq.HZ");
      #if DEBUG_DEFAULT_MISSION > 1
      MessageInterface::ShowMessage("-->default angular parameters created\n");
      #endif

      #ifdef __ENABLE_ATMOS_DENSITY__
      // Environmental parameters
      CreateParameter("AtmosDensity", "DefaultSC.Earth.AtmosDensity");
      #if DEBUG_DEFAULT_MISSION > 1
      MessageInterface::ShowMessage("-->default env parameters created\n");
      #endif
      #endif
      
      // Planet parameters
      CreateParameter("Altitude", "DefaultSC.Earth.Altitude");
      CreateParameter("MHA", "DefaultSC.Earth.MHA");
      CreateParameter("Longitude", "DefaultSC.Earth.Longitude");
      CreateParameter("Latitude", "DefaultSC.Earth.Latitude");
      CreateParameter("LST", "DefaultSC.Earth.LST");
      CreateParameter("BetaAngle", "DefaultSC.Earth.BetaAngle");
      #if DEBUG_DEFAULT_MISSION > 1
      MessageInterface::ShowMessage("-->default planet parameters created\n");
      #endif
      
      // B-Plane parameters
      CreateParameter("BdotT", "DefaultSC.Earth.BdotT");
      CreateParameter("BdotR", "DefaultSC.Earth.BdotR");
      CreateParameter("BVectorMag", "DefaultSC.Earth.BVectorMag");
      CreateParameter("BVectorAngle", "DefaultSC.Earth.BVectorAngle");
      #if DEBUG_DEFAULT_MISSION > 1
      MessageInterface::ShowMessage("-->default b-plane parameters created\n");
      #endif
      
      // Attitude parameters
      CreateParameter("DCM11", "DefaultSC.DCM11");
      CreateParameter("DCM12", "DefaultSC.DCM12");
      CreateParameter("DCM13", "DefaultSC.DCM13");
      CreateParameter("DCM21", "DefaultSC.DCM21");
      CreateParameter("DCM22", "DefaultSC.DCM22");
      CreateParameter("DCM23", "DefaultSC.DCM23");
      CreateParameter("DCM31", "DefaultSC.DCM31");
      CreateParameter("DCM32", "DefaultSC.DCM32");
      CreateParameter("DCM33", "DefaultSC.DCM33");
      CreateParameter("EulerAngle1", "DefaultSC.EulerAngle1");
      CreateParameter("EulerAngle2", "DefaultSC.EulerAngle2");
      CreateParameter("EulerAngle3", "DefaultSC.EulerAngle3");
      CreateParameter("MRP1", "DefaultSC.MRP1");  // Dunn Added
      CreateParameter("MRP2", "DefaultSC.MRP2");  // Dunn Added
      CreateParameter("MRP3", "DefaultSC.MRP3");  // Dunn Added
      CreateParameter("Q1", "DefaultSC.Q1");
      CreateParameter("Q2", "DefaultSC.Q2");
      CreateParameter("Q3", "DefaultSC.Q3");
      CreateParameter("Q4", "DefaultSC.Q4");
      CreateParameter("AngularVelocityX", "DefaultSC.AngularVelocityX");
      CreateParameter("AngularVelocityY", "DefaultSC.AngularVelocityY");
      CreateParameter("AngularVelocityZ", "DefaultSC.AngularVelocityZ");
      CreateParameter("EulerAngleRate1", "DefaultSC.EulerAngleRate1");
      CreateParameter("EulerAngleRate2", "DefaultSC.EulerAngleRate2");
      CreateParameter("EulerAngleRate3", "DefaultSC.EulerAngleRate3");
      #if DEBUG_DEFAULT_MISSION > 1
      MessageInterface::ShowMessage("-->default attitude parameters created\n");
      #endif
      
      // Ballistic/Mass parameters
      CreateParameter("DryMass", "DefaultSC.DryMass");
      CreateParameter("Cd", "DefaultSC.Cd");
      CreateParameter("Cr", "DefaultSC.Cr");
      CreateParameter("DragArea", "DefaultSC.DragArea");
      CreateParameter("SRPArea", "DefaultSC.SRPArea");
      CreateParameter("TotalMass", "DefaultSC.TotalMass");
      #if DEBUG_DEFAULT_MISSION > 1
      MessageInterface::ShowMessage("-->default ballistic/mass parameters created\n");
      #endif
      
      // STM and A-Matrix parameters
      CreateParameter("OrbitSTM", "DefaultSC.OrbitSTM");
      CreateParameter("OrbitSTMA", "DefaultSC.OrbitSTMA");
      CreateParameter("OrbitSTMB", "DefaultSC.OrbitSTMB");
      CreateParameter("OrbitSTMC", "DefaultSC.OrbitSTMC");
      CreateParameter("OrbitSTMD", "DefaultSC.OrbitSTMD");
      CreateParameter("OrbitAMatrix", "DefaultSC.OrbitSTM");
      CreateParameter("OrbitAMatrixA", "DefaultSC.OrbitSTMA");
      CreateParameter("OrbitAMatrixB", "DefaultSC.OrbitSTMB");
      CreateParameter("OrbitAMatrixC", "DefaultSC.OrbitSTMC");
      CreateParameter("OrbitAMatrixD", "DefaultSC.OrbitSTMD");
      #if DEBUG_DEFAULT_MISSION > 1
      MessageInterface::ShowMessage("-->default STM parameters created\n");
      #endif
      
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
      StringArray params = GetListOfObjects(Gmat::PARAMETER);
      Parameter *param;
      
      for (unsigned int i=0; i<params.size(); i++)
      {
         param = GetParameter(params[i]);

         // need spacecraft if system parameter
         if (param->GetKey() == GmatParam::SYSTEM_PARAM)
         {
            if (param->GetOwnerType() == Gmat::SPACECRAFT)
            {
               //MessageInterface::ShowMessage("name = '%s'\n", param->GetName().c_str());
               //param->SetStringParameter("Expression", param->GetName());
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
            else if (param->GetOwnerType() == Gmat::IMPULSIVE_BURN)
            {
               //MessageInterface::ShowMessage("name = '%s'\n", param->GetName().c_str());
               param->SetRefObjectName(Gmat::IMPULSIVE_BURN, "DefaultIB");
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
      // Dunn changed ElapsedSecs for default mission to 12000.0 so the spacecraft
      // icon will stop on the near side of the earth where we can see it.  This
      // was required in two locations, so look for it again below.
      stopOnElapsedSecs->SetStringParameter("Goal", "12000.0");
      //stopOnElapsedSecs->SetStringParameter("Goal", "8640.0");
      
      #if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default StopCondition created\n");
      #endif
      
      // Subscribers
      // OrbitView
      GetDefaultSubscriber("OrbitView");
      
      #if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default Subscribers created\n");
      #endif
      
      //----------------------------------------------------
      // Create default mission sequence
      //----------------------------------------------------
      bool retval;
      
      // Append BeginMissionSequence command (New since 2010.07.09)
      //GmatCommand *cmd = AppendCommand("BeginMissionSequence", "", retval);
      AppendCommand("BeginMissionSequence", "", retval);
      
      // Propagate Command
      GmatCommand *propCommand = CreateCommand("Propagate", "", retval);
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
      
      // Append Propagate command
      AppendCommand(propCommand);
      
      #if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage
         ("-->Setting SolarSystem <%p> and ObjectMap <%p> to theScriptInterpreter\n",
          theSolarSystemInUse, theConfigManager->GetObjectMap());
      #endif
      
      // Reset initial solar system in use and object map
      objectMapInUse = theConfigManager->GetObjectMap();
      SetSolarSystemAndObjectMap(theSolarSystemInUse, objectMapInUse, false,
                                 "CreateDefaultMission()");
      
      isRunReady = true;
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_,
          "*** Error occurred during default mission creation.\n    The default "
          "mission will not run.\n    Message: " + e.GetFullMessage());
   }
   
   #if DEBUG_INITIALIZE
   MessageInterface::ShowMessage("Moderator successfully created default mission\n");
   #endif
} // CreateDefaultMission()


// Parameter reference object setting
//------------------------------------------------------------------------------
// void CheckParameterType(Parameter *param, const std::string &type,
//                         const std::string &ownerName)
//------------------------------------------------------------------------------
void Moderator::CheckParameterType(Parameter *param, const std::string &type,
                                   const std::string &ownerName)
{
   GmatBase *obj = FindObject(ownerName);
   if (obj)
   {
      #if DEBUG_CREATE_PARAMETER
      MessageInterface::ShowMessage
         ("   Found owner object, name='%s', addr=<%p>\n", obj->GetName().c_str(), obj);
      #endif
      
      if (param->GetOwnerType() != obj->GetType())
      {
         std::string paramOwnerType =
            GmatBase::GetObjectTypeString(param->GetOwnerType());
         
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (param, param->GetName(), "Moderator::CheckParameterType()");
         #endif
         delete param;
         param = NULL;
         
         if (paramOwnerType == "")
            throw GmatBaseException
               ("Cannot find the object type which has \"" + type +
                "\" as a Parameter type");
         else
            throw GmatBaseException
               ("Parameter type: " + type + " should be property of " +
                paramOwnerType);
      }
   }
}


//------------------------------------------------------------------------------
// void SetParameterRefObject(Parameter *param, const std::string &type, ...)
//------------------------------------------------------------------------------
/**
 * Sets parameter reference object
 *
 * @param <param> parameter pointer
 * @param <type> parameter type
 * @param <name> parameter name
 * @param <ownerName> parameter owner name
 * @param <depName> dependent object name
 */
//------------------------------------------------------------------------------
void Moderator::SetParameterRefObject(Parameter *param, const std::string &type,
                                      const std::string &name,
                                      const std::string &ownerName,
                                      const std::string &depName, Integer manage)
{
   #if DEBUG_PARAMETER_REF_OBJ
   MessageInterface::ShowMessage
      ("Moderator::SetParameterRefObject() param=<%p>, type=<%s>, name=<%s>, "
       "owner=<%s>, dep=<%s>, manage=%d\n",  param, type.c_str(), name.c_str(),
       ownerName.c_str(), depName.c_str(), manage);
   #endif
   
   // Why do we need to set expression for non-Variable? (loj: 2008.08.09)
   // Expression is set in the ScriptInterpreter or ParameterCreateDialog for
   // new Parameters
   // If type is Variable, don't set expression
   //if (type != "Variable")
   //   param->SetStringParameter("Expression", name);
   
   // Set parameter owner and dependent object
   if (ownerName != "")
   {
      param->SetRefObjectName(param->GetOwnerType(), ownerName);
      param->AddRefObject((Parameter*)FindObject(ownerName));
   }
   
   std::string newDep = depName;
   
   // Set dependent object name if depName is not blank,
   // if depName is blank get default dep name
   if (depName != "")
      param->SetStringParameter("DepObject", depName);
   else
      newDep = param->GetStringParameter("DepObject");
   
   // Set SolarSystem
   param->SetSolarSystem(theSolarSystemInUse);
   param->SetInternalCoordSystem(theInternalCoordSystem);
   
   #if DEBUG_PARAMETER_REF_OBJ
   MessageInterface::ShowMessage
      ("   name='%s', newDep='%s'\n", name.c_str(), newDep.c_str());
   #endif
   
   if (newDep != "")
      param->AddRefObject(FindObject(newDep));
   
   // I'm not sure if we always use EarthMJ2000Eq (loj: 2008.06.03)
   if (param->NeedCoordSystem())
      param->AddRefObject(FindObject("EarthMJ2000Eq"));
   
   // create parameter dependent coordinate system
   if (type == "Longitude" || type == "Latitude" || type == "Altitude" ||
       type == "MHA" || type == "LST")
   {
      // need body-fixed CS
      StringTokenizer st(name, ".");
      StringArray tokens = st.GetAllTokens();
      
      if (tokens.size() == 2 || (tokens.size() == 3 && tokens[1] == "Earth"))
      {
         #if DEBUG_PARAMETER_REF_OBJ
         MessageInterface::ShowMessage("   Creating 'EarthFixed' CoordinateSystem\n");
         #endif
         
         // default EarthFixed
         CoordinateSystem *cs = CreateCoordinateSystem("EarthFixed", false, false, manage);
         param->SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthFixed");
         // Set CoordinateSystem to param (2008.06.26)
         // It will work without setting CS pointer since EarthFixed is a default
         // CoordinateSyste, but for consistency just set here
         param->SetRefObject(cs, Gmat::COORDINATE_SYSTEM, "EarthFixed");
      }
      else if (tokens.size() == 3)
      {
         std::string origin = tokens[1];
         std::string axisName = origin + "Fixed";
         
         #if DEBUG_PARAMETER_REF_OBJ
         MessageInterface::ShowMessage
            ("   Creating '%s' CoordinateSystem\n", axisName.c_str());
         #endif
         
         CoordinateSystem *cs = CreateCoordinateSystem(axisName, false, false, manage);
         
         // create BodyFixedAxis with origin
         AxisSystem *axis = CreateAxisSystem("BodyFixed", "BodyFixed_Earth", manage);
         cs->SetStringParameter("Origin", origin);
         cs->SetRefObject(FindObject(origin), Gmat::SPACE_POINT, origin);
         cs->SetRefObject(axis, Gmat::AXIS_SYSTEM, axis->GetName());
         cs->SetStringParameter("J2000Body", "Earth");
         cs->SetRefObject(FindObject("Earth"), Gmat::SPACE_POINT, "Earth");
         cs->SetSolarSystem(theSolarSystemInUse);
         cs->Initialize();
         
         // Since CoordinateSystem clones AxisSystem, delete it from here
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (axis, "localAxes", "Moderator::SetParameterRefObject()",
             "deleting localAxes");
         #endif
         delete axis;
         
         param->SetRefObjectName(Gmat::COORDINATE_SYSTEM, axisName);
         // Set CoordinateSystem to param (2008.06.26)
         // This will fix problem with NULL output CS pointer if Paremeter is
         // used in the GmatFunction
         param->SetRefObject(cs, Gmat::COORDINATE_SYSTEM, axisName);
      }
      else
      {
         MessageInterface::ShowMessage("===> Invalid parameter name: %s\n",
                                       name.c_str());
      }
   }
}


// object map
//------------------------------------------------------------------------------
// GmatBase* FindObject(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Finds object from the object map in use by name base on objectManageOption.
 * if objectManageOption is 
 *    0, if object is not managed
 *    1, if configuration object map is used for finding objects and adding new
 *          objects
 *    2, if function object map is used for finding objects and adding new
 *          objects including automatic objects
 */
//------------------------------------------------------------------------------
GmatBase* Moderator::FindObject(const std::string &name)
{
   #if DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage
      ("Moderator::FindObject() entered, name='%s', objectMapInUse=<%p>\n",
       name.c_str(), objectMapInUse);
   #endif
   
   if (name == "")
   {
      #if DEBUG_FIND_OBJECT
      MessageInterface::ShowMessage
         ("Moderator::FindObject() name is blank, so just returning NULL\n");
      #endif
      return NULL;
   }
   
   if (objectMapInUse == NULL)
   {
      #if DEBUG_FIND_OBJECT
      MessageInterface::ShowMessage
         ("Moderator::FindObject() objectMapInUse is NULL, so just returning NULL\n");
      #endif
      return NULL;
   }
   
   // Ignore array indexing of Array
   std::string newName = name;
   std::string::size_type index = name.find_first_of("([");
   if (index != name.npos)
   {
      newName = name.substr(0, index);
      
      #if DEBUG_FIND_OBJECT
      MessageInterface::ShowMessage("   newName=%s\n", newName.c_str());
      #endif
   }
   
   #if DEBUG_FIND_OBJECT > 1
   ShowObjectMap("Moderator::FindObject() Here is the object map in use");
   #endif
   
   GmatBase *obj = NULL;
   
   if (objectMapInUse->find(newName) != objectMapInUse->end())
      obj = (*objectMapInUse)[newName];
   
   // check objectManageOption
   Integer manage = 1;
   if (objectManageOption != 1)
      manage = 2;
   
   // If object not found, try SolarSystem
   if (obj == NULL)
   {
      SolarSystem *ss = GetSolarSystemInUse(manage);
      
      if (ss)
      {
         obj = (GmatBase*)(ss->GetBody(newName));
         #if DEBUG_FIND_OBJECT
         if (obj)
         {
            MessageInterface::ShowMessage
               ("   Found '%s' from the SolarSystem <%p> in the map\n",
                name.c_str(), ss);
         }
         #endif
      }
   }
   
   #if DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage
      ("Moderator::FindObject() returning <%p><%s>'%s'\n", obj, 
       obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL");
   #endif
   
   return obj;
}


//------------------------------------------------------------------------------
// bool AddObject(GmatBase *obj)
//------------------------------------------------------------------------------
/*
 * Adds object to the objectMapInUse
 *
 * @param  obj  object to add to object map
 *
 * @return true if successfully added, false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::AddObject(GmatBase *obj)
{
   #if DEBUG_ADD_OBJECT
   MessageInterface::ShowMessage
      ("Moderator::AddObject() entered, obj=<%p><%s><%s>, objectMapInUse=<%p>\n",
       obj, obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL", objectMapInUse);
   #endif
   
   if (obj == NULL || (obj && obj->GetName() == ""))
   {
      #if DEBUG_ADD_OBJECT
      MessageInterface::ShowMessage
         ("Moderator::AddObject() returning false, has NULL obj or name is blank\n");
      #endif
      
      return false;
   }
   
   if (objectMapInUse == NULL)
      throw GmatBaseException
         ("Moderator::AddObject() cannot add object named \"" + obj->GetName() +
          "\" to unset object map in use");
   
   #ifdef DEBUG_OBJECT_MAP
   MessageInterface::ShowMessage
      ("Moderator::Moderator::AddObject() Adding <%p><%s>'%s' to objectMapInUse <%p>\n",
       obj, obj->GetTypeName().c_str(), obj->GetName().c_str(), objectMapInUse);
   #endif
   
   // if name not found in the object map, then insert (loj: 2008.12.16)
   if (objectMapInUse->find(obj->GetName()) == objectMapInUse->end())
      objectMapInUse->insert(std::make_pair(obj->GetName(), obj));
   else
   {
      #if DEBUG_ADD_OBJECT
      MessageInterface::ShowMessage
         ("*** WARNING *** Moderator::AddObject() '%s' is already in the object map, so ignored\n",
          obj->GetName().c_str());
      #endif
   }
   
   #if DEBUG_ADD_OBJECT
   MessageInterface::ShowMessage("Moderator::AddObject() returning true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void SetSolarSystemAndObjectMap(SolarSystem *ss, ObjectMap *objMap,
//         bool forFunction, const std::string &callFrom)
//------------------------------------------------------------------------------
/*
 * Sets the solar system in use and configured object map to interpreters.
 *
 * @param <ss> Pointer to the solar system
 * @param <objMap> Pointer to the object map
 * @param <forFunction> True if setting object map for function (false)
 */
//------------------------------------------------------------------------------
void Moderator::SetSolarSystemAndObjectMap(SolarSystem *ss, ObjectMap *objMap,
                                           bool forFunction,
                                           const std::string &callFrom)
{
   #if DEBUG_OBJECT_MAP
   MessageInterface::ShowMessage
      ("=====> Moderator::%s setting solarSystemInUse=<%p>, "
       "objectMapInUse=<%p> %s\n", callFrom.c_str(), ss, objMap,
       forFunction ? "for function" : "");
   #endif
   
   // Set solar system in use and object map 
   theScriptInterpreter->SetSolarSystemInUse(ss);
   theScriptInterpreter->SetObjectMap(objMap);
   if (!forFunction)
      theScriptInterpreter->SetFunction(NULL);
   if (theUiInterpreter != NULL)
   {
      theUiInterpreter->SetSolarSystemInUse(ss);
      theUiInterpreter->SetObjectMap(objMap);
      if (!forFunction)
         theUiInterpreter->SetFunction(NULL);
   }
}


// default objects
//------------------------------------------------------------------------------
// Spacecraft* GetDefaultSpacecraft()
//------------------------------------------------------------------------------
/*
 * Returns first spacecraft not in the Formation
 */
//------------------------------------------------------------------------------
Spacecraft* Moderator::GetDefaultSpacecraft()
{
   StringArray soConfigList = GetListOfObjects(Gmat::SPACECRAFT);
   
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
   StringArray configList = GetListOfObjects(Gmat::PROP_SETUP);
   
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
// Burn* GetDefaultBurn(const std::string &type)
//------------------------------------------------------------------------------
Burn* Moderator::GetDefaultBurn(const std::string &type)
{
   StringArray configList = GetListOfObjects(Gmat::BURN);

   if (configList.size() > 0)
   {
      for (UnsignedInt i=0; i<configList.size(); i++)
         if (GetBurn(configList[i])->IsOfType(type))
            return GetBurn(configList[i]);
   }
   
   Burn *burn = NULL;
   
   if (type == "ImpulsiveBurn")
      burn = CreateBurn("ImpulsiveBurn", "DefaultIB");
   else if (type == "FiniteBurn")
      burn = CreateBurn("FiniteBurn", "DefaultFB");
   
   return burn;
}


//------------------------------------------------------------------------------
// Hardware* GetDefaultHardware(const std::string &type)
//------------------------------------------------------------------------------
Hardware* Moderator::GetDefaultHardware(const std::string &type)
{
   StringArray configList = GetListOfObjects(Gmat::HARDWARE);

   if (configList.size() > 0)
   {
      for (UnsignedInt i=0; i<configList.size(); i++)
         if (GetHardware(configList[i])->IsOfType(type))
            return GetHardware(configList[i]);
   }
   
   Hardware *hw = NULL;
   
   if (type == "FuelTank")
      hw = CreateHardware("FuelTank", "DefaultFuelTank");
   else if (type == "Thruster")
      hw = CreateHardware("Thruster", "DefaultThruster");
   
   return hw;
}


//------------------------------------------------------------------------------
// Subscriber* GetDefaultSubscriber(const std::string &type, bool addObjects = true)
//------------------------------------------------------------------------------
Subscriber* Moderator::GetDefaultSubscriber(const std::string &type, bool addObjects)
{
   StringArray configList = GetListOfObjects(Gmat::SUBSCRIBER);
   int subSize = configList.size();
   Subscriber *sub = NULL;
   
   for (int i=0; i<subSize; i++)
   {
      sub = (Subscriber*)GetConfiguredObject(configList[i]);
      if (sub->GetTypeName() == type)
         return sub;
   }
   
   if (type == "OrbitView")
   {
      // create default OrbitView
      sub = CreateSubscriber("OrbitView", "DefaultOrbitView");
      sub->SetStringParameter("Add", "DefaultSC");
      sub->SetStringParameter("Add", "Earth");
      sub->SetStringParameter("CoordinateSystem", "EarthMJ2000Eq");
      sub->SetStringParameter("ViewPointVector", "[30000 0 0]");
      sub->Activate(true);
   }
   else if (type == "GroundTrackPlot")
   {
      // create default GroundTrackPlot
      sub = CreateSubscriber("GroundTrackPlot", "DefaultGroundTrackPlot");
      sub->SetStringParameter("Add", "DefaultSC");
      sub->SetStringParameter("Add", "Earth");
      sub->Activate(true);
   }
   else if (type == "XYPlot")
   {
      // create default XYPlot
      sub = CreateSubscriber("XYPlot", "DefaultXYPlot");
      sub->SetStringParameter("IndVar", "DefaultSC.A1ModJulian");
      sub->SetStringParameter("Add", "DefaultSC.EarthMJ2000Eq.X", 0);      
      sub->SetStringParameter("Add", "DefaultSC.EarthMJ2000Eq.Y", 1);
      sub->SetStringParameter("Add", "DefaultSC.EarthMJ2000Eq.Z", 2);
      sub->Activate(true);
   }
   else if (type == "ReportFile")
   {
      // create default ReportFile
      sub = CreateSubscriber("ReportFile", "DefaultReportFile");
      std::string scName = GetDefaultSpacecraft()->GetName();
      sub->SetStringParameter(sub->GetParameterID("Filename"),
                              "DefaultReportFile.txt");
      
      if (addObjects)
      {
         sub->SetStringParameter("Add", scName + ".A1ModJulian");
         sub->SetStringParameter("Add", scName + ".EarthMJ2000Eq.X");
         sub->SetStringParameter("Add", scName + ".EarthMJ2000Eq.Y");
         sub->SetStringParameter("Add", scName + ".EarthMJ2000Eq.Z");
         sub->SetStringParameter("Add", scName + ".EarthMJ2000Eq.VX");
         sub->SetStringParameter("Add", scName + ".EarthMJ2000Eq.VY");
         sub->SetStringParameter("Add", scName + ".EarthMJ2000Eq.VZ");
      }
      sub->Activate(true);
      
      // To validate and create element wrappers
      theScriptInterpreter->ValidateSubscriber(sub);
   }
   else
   {
      MessageInterface::ShowMessage
         ("*** ERROR *** GetDefaultSubscriber() Undefined subscriber type: %s\n",
          type.c_str());
   }

   return sub;
}

//------------------------------------------------------------------------------
// Solver* GetDefaultSolver()
//------------------------------------------------------------------------------
Solver* Moderator::GetDefaultSolver()
{
   StringArray configList = GetListOfObjects(Gmat::SOLVER);
   
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
   // Dunn changed ElapsedSecs for default mission to 12000.0 so the spacecraft
   // icon will stop on the near side of the earth where we can see it.
   stopCond->SetStringParameter("Goal", "12000.0");
   //stopCond->SetStringParameter("Goal", "8640.0");
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
   MessageInterface::ShowMessage
      ("   Adding theSolarSystemInUse<%p> to Sandbox\n", theSolarSystemInUse);
   #endif
   
   //If we are ready to configure SolarSystem by name
   // Script will have something like, "Create SolarSystem anotherSS;"
   #ifdef __USE_CONFIGURED_SOLAR_SYSTEM__
      SolarSystem *solarSys = theConfigManager->GetSolarSystemInUse(name);
      sandboxes[index]->AddSolarSystem(solarSys);
   #else
      sandboxes[index]->AddSolarSystem(theSolarSystemInUse);
   #endif
}


//------------------------------------------------------------------------------
// void Moderator::AddTriggerManagersToSandbox(Integer index)
//------------------------------------------------------------------------------
/**
 * Passes TriggerManager array to a Sandbox so the Sandbox can clone managers
 *
 * @param index The index of the sandbox getting the trigger managers
 */
//------------------------------------------------------------------------------
void Moderator::AddTriggerManagersToSandbox(Integer index)
{
   sandboxes[index]->AddTriggerManagers(&triggerManagers);
}


//------------------------------------------------------------------------------
// void AddInternalCoordSystemToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddInternalCoordSystemToSandbox(Integer index)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddInternalCoordSystemToSandbox() entered.\n");
   MessageInterface::ShowMessage
      ("   Adding theInternalCoordSystem<%p> to Sandbox\n", theInternalCoordSystem);
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
   MessageInterface::ShowMessage
      ("   Adding thePublisher<%p> to Sandbox\n", thePublisher);
   #endif
   
   thePublisher->UnsubscribeAll();
   sandboxes[index]->SetPublisher(thePublisher);
   
}


//------------------------------------------------------------------------------
// void HandleCcsdsEphemerisFile(ObjectMap *objMap, bool deleteOld = false)
//------------------------------------------------------------------------------
/**
 * Creates plug-in CcsdsEphemerisFile object if EphemerisFile type is CCSDS
 * and replaces the old one.
 */
//------------------------------------------------------------------------------
void Moderator::HandleCcsdsEphemerisFile(ObjectMap *objMap, bool deleteOld)
{
   #ifdef DEBUG_CCSDS_EPHEMERIS
   MessageInterface::ShowMessage
      ("Moderator::HandleCcsdsEphemerisFile() entered, objMap=<%p>\n", objMap);
   #endif
   
   GmatBase *obj;
   
   for (ObjectMap::iterator i = objMap->begin(); i != objMap->end(); ++i)
   {
      obj = i->second;
      
      //==============================================================
      // Special handling for CcsdsEphemerisFile plug-in
      //==============================================================
      // This is needed since we create EphemerisFile object first
      // from the script "Create EphemerisFile" and then create
      // CcsdsEphemerisFile if file format contains CCSDS.
      // It will create CcsdsEphemerisFile object via
      // plug-in factory and replace the object pointer.
      //==============================================================
      // Create CcsdsEphemerisFile if file format is CCSDS
      if (obj->IsOfType(Gmat::EPHEMERIS_FILE))
      {
         std::string name = obj->GetName();
         std::string format = obj->GetStringParameter("FileFormat");
         
         #ifdef DEBUG_CCSDS_EPHEMERIS
         MessageInterface::ShowMessage
            ("   Format of the object<%p><%s>'%s' is '%s'\n",
             obj, obj->GetTypeName().c_str(), name.c_str(), format.c_str());
         #endif
         
         if (format.find("CCSDS") != format.npos)
         {
            // Check type name to avoid recreating a CcsdsEphemerisFile object for re-runs
            if (obj->GetTypeName() != "CcsdsEphemerisFile")
            {
               #ifdef DEBUG_CCSDS_EPHEMERIS
               MessageInterface::ShowMessage("   About to create new CcsdsEphemerisFile\n");
               #endif
               
               // Create unnamed CcsdsEphemerisFile
               GmatBase *newObj = CreateEphemerisFile("CcsdsEphemerisFile", "");
               if (newObj == NULL)
               {
                  throw GmatBaseException
                     ("Moderator::AddSubscriberToSandbox() Cannot continue due to missing "
                      "CcsdsEphemerisFile plugin dll\n");
               }
               
               newObj->SetName(name);
               ResetObjectPointer(objMap, newObj, name);
               ResetObjectPointer(objectMapInUse, newObj, name);
               newObj->Copy(obj);
               newObj->TakeAction("ChangeTypeName", "CcsdsEphemerisFile");
               
               #ifdef DEBUG_CCSDS_EPHEMERIS
               MessageInterface::ShowMessage
                  ("   New object <%p><%s>'%s' created\n", newObj, newObj->GetTypeName().c_str(),
                   name.c_str());
               #endif
               
               GmatBase *oldObj = obj;
               obj = newObj;
               
               // Delete old object on option
               if (deleteOld)
               {
                  #ifdef DEBUG_CCSDS_EPHEMERIS
                  MessageInterface::ShowMessage
                     ("   Deleting old object <%p><%s>'%s'\n", oldObj, oldObj->GetTypeName().c_str(),
                      name.c_str());
                  #endif
                  #ifdef DEBUG_MEMORY
                  MemoryTracker::Instance()->Remove
                     (oldObj, oldObj->GetName(), "Moderator::HandleCcsdsEphemerisFile()");
                  #endif
                  delete oldObj;
               }
            }
         }
      }
   }
   
   #ifdef DEBUG_CCSDS_EPHEMERIS
   ShowObjectMap("In Moderator::HandleCcsdsEphemerisFile()", objMap);
   MessageInterface::ShowMessage
      ("Moderator::HandleCcsdsEphemerisFile() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// void AddSuscriberToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddSubscriberToSandbox(Integer index)
{
   #ifdef __USE_DATAFILE__
   // Handle CcsdsEphemerisFile which uses DataFile plugin
   HandleCcsdsEphemerisFile(objectMapInUse, false);
   #endif
   
   Subscriber *obj;
   StringArray names = theConfigManager->GetListOfItems(Gmat::SUBSCRIBER);
   
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddSubscriberToSandbox() count = %d\n", names.size());
   #endif
   
   for (Integer i=0; i<(Integer)names.size(); i++)
   {
      obj = theConfigManager->GetSubscriber(names[i]);
      sandboxes[index]->AddSubscriber(obj);
      
      #if DEBUG_RUN > 1
      MessageInterface::ShowMessage
         ("   Adding <%p><%s>'%s'\n", obj, obj->GetTypeName().c_str(), 
          obj->GetName().c_str());
      #endif
   }
}


//------------------------------------------------------------------------------
// void AddOtherObjectsToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddOtherObjectsToSandbox(Integer index)
{
   GmatBase *obj;
   StringArray names = theConfigManager->GetListOfAllItems();
   
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddOtherObjectsToSandbox() count = %d\n", names.size());
   #endif
   
   for (Integer i=0; i<(Integer)names.size(); i++)
   {
      obj = theConfigManager->GetItem(names[i]);

      // Skip subscribers since those are handled separately
      if (obj->IsOfType(Gmat::SUBSCRIBER))
         continue;
      
      #ifdef DEBUG_RUN
      MessageInterface::ShowMessage
         ("   Adding <%p><%s>'%s'\n", obj, obj->GetTypeName().c_str(),
          obj->GetName().c_str());
      #endif
      sandboxes[index]->AddObject(obj);
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
      #if DEBUG_RUN > 1
      MessageInterface::ShowMessage
         ("   Adding <%p><%s>\n", cmd, cmd->GetTypeName().c_str());
      #endif
      
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
// void ShowCommand(const std::string &title1, GmatCommand *cmd1,
//                  const std::string &title2, GmatCommand *cmd2)
//------------------------------------------------------------------------------
void Moderator::ShowCommand(const std::string &title1, GmatCommand *cmd1,
                            const std::string &title2, GmatCommand *cmd2)
{
   if (title2 == "")
   {
      if (cmd1 == NULL)
         MessageInterface::ShowMessage("%s<%p><NULL>\n", title1.c_str(), cmd1);
      else
         MessageInterface::ShowMessage
            ("%s<%p><%s>\n", title1.c_str(), cmd1, cmd1->GetTypeName().c_str());
   }
   else
   {
      if (cmd2 == NULL)
         MessageInterface::ShowMessage
            ("%s<%p><NULL>%s<%p><NULL>\n", title1.c_str(), cmd1, title2.c_str(), cmd2);
      else
         MessageInterface::ShowMessage
            ("%s<%p><%s>%s<%p><%s>\n", title1.c_str(), cmd1, cmd1->GetTypeName().c_str(),
             title2.c_str(), cmd2, cmd2->GetTypeName().c_str());
   }
}


//------------------------------------------------------------------------------
// void ShowObjectMap(const std::string &title, ObjectMap *objMap = NULL)
//------------------------------------------------------------------------------
void Moderator::ShowObjectMap(const std::string &title, ObjectMap *objMap)
{
   MessageInterface::ShowMessage(title + "\n");
   if (objMap != NULL)
   {
      MessageInterface::ShowMessage
         (" passedObjectMap = <%p>, it has %d objects\n", objMap, objMap->size());
      for (ObjectMap::iterator i = objMap->begin(); i != objMap->end(); ++i)
      {
         MessageInterface::ShowMessage
            ("   %30s  <%p><%s>\n", i->first.c_str(), i->second,
             i->second == NULL ? "NULL" : (i->second)->GetTypeName().c_str());
      }
   }
   
   if (objectMapInUse == NULL)
   {
      MessageInterface::ShowMessage("\nThe objectMapInUse is NULL\n");
      return;
   }
   
   MessageInterface::ShowMessage
      (" objectMapInUse = <%p>, it has %d objects\n", objectMapInUse, objectMapInUse->size());
   for (ObjectMap::iterator i = objectMapInUse->begin();
        i != objectMapInUse->end(); ++i)
   {
      MessageInterface::ShowMessage
         ("   %30s  <%p><%s>\n", i->first.c_str(), i->second,
          i->second == NULL ? "NULL" : (i->second)->GetTypeName().c_str());
   }
}


//------------------------------------------------------------------------------
// Moderator()
//------------------------------------------------------------------------------
/*
 * Constructor
 */
//------------------------------------------------------------------------------
Moderator::Moderator()
{
   isRunReady = false;
   showFinalState = false;
   theDefaultSolarSystem = NULL;
   theSolarSystemInUse = NULL;
   theInternalCoordSystem = NULL;
   theInternalSolarSystem = NULL;
   runState = Gmat::IDLE;
   objectManageOption = 1;
   
   theMatlabInterface = NULL;
   
   // The motivation of adding this data member was due to Parameter creation
   // in function mode. When Parameter is created, the Moderator automatically
   // sets it's refeence object. For example, Sat.X, it sets Sat object pointer
   // found from the current object map. Since we don't always want to use
   // configuration to find object, this objectMapInUse was added. (loj: 2008.05.23)
   objectMapInUse = NULL;
   currentFunction = NULL;
   
   sandboxes.reserve(Gmat::MAX_SANDBOX);
   commands.reserve(Gmat::MAX_SANDBOX);
}


//------------------------------------------------------------------------------
// ~Moderator()
//------------------------------------------------------------------------------
Moderator::~Moderator()
{
}
