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
 * Defines opeartions of the GMAT executive.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
// executive
#include "Sandbox.hpp"
#include "GuiInterpreter.hpp"
#include "ScriptInterpreter.hpp"
#include "FactoryManager.hpp"
#include "ConfigManager.hpp"
#include "Publisher.hpp"
// core
#include "PhysicalModel.hpp"
#include "ForceModel.hpp"
#include "Propagator.hpp"
#include "Spacecraft.hpp"
#include "Parameter.hpp"
#include "StopCondition.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "PropSetup.hpp"
#include "Command.hpp"
#include "Subscriber.hpp"
// factories
#include "CommandFactory.hpp"
#include "ForceModelFactory.hpp"
#include "PhysicalModelFactory.hpp"
#include "PropSetupFactory.hpp"
#include "PropagatorFactory.hpp"
#include "SpacecraftFactory.hpp"
#include "StopConditionFactory.hpp"
#include "SubscriberFactory.hpp"

#include "BaseException.hpp"
#include "NoOp.hpp"
#include "Moderator.hpp"

//---------------------------------
// static data
//---------------------------------
Moderator* Moderator::instance = NULL;
GuiInterpreter* Moderator::theGuiInterpreter = NULL;
ScriptInterpreter* Moderator::theScriptInterpreter = NULL;
FactoryManager* Moderator::theFactoryManager = NULL;
ConfigManager* Moderator::theConfigManager = NULL;

//-----------------------------------------------------
//*** should match with ObjectType in gmatdefs.hpp
//-----------------------------------------------------
const std::string
Moderator::OBJECT_TYPE_STRING[Gmat::UNKNOWN_OBJECT-Gmat::SPACECRAFT+1] =
{
   "Spacecraft",
   "GroundStation",
   "Command",
   "Propagator",
   "ForceModel",
   "PhysicalModel",
   "Interpolator",
   "SolarSystem",
   "CelestialBody",
   "Parameter",
   "StopCondition",
   "Solver",
   "Subscriber",
   "PropSetup"
};

//---------------------------------
// public
//---------------------------------

//loj: I'm assuming GmatMainApp::OnInit() will have something like:
//loj:-------------------------------------------------------------
//loj: GmatMainApp::OnInit()
//loj: {
//loj:    theModerator = Moderator::Instance();
//loj:    theModerator->Initialize();
//loj:    theGuiInterpreter = theModerator->GetGuiInterpreter();
//loj:    ....
//loj: }
//loj:-------------------------------------------------------------

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
// bool Initialize()
//------------------------------------------------------------------------------
bool Moderator::Initialize()
{
    if (!isInitialized)
    {
        // Create interpreters and managers
        theGuiInterpreter = GuiInterpreter::Instance();
        theScriptInterpreter = ScriptInterpreter::Instance();
        theFactoryManager = FactoryManager::Instance();
        theConfigManager = ConfigManager::Instance();

        // Create publisher
        thePublisher = Publisher::Instance();

        // Create factories
        theCommandFactory = new CommandFactory();
        theForceModelFactory = new ForceModelFactory();
        theParameterFactory = new ParameterFactory();
        thePhysicalModelFactory = new PhysicalModelFactory();
        thePropSetupFactory = new PropSetupFactory();
        thePropagatorFactory = new PropagatorFactory();
        theSpacecraftFactory = new SpacecraftFactory();
        theStopConditionFactory = new StopConditionFactory();
        theSubscriberFactory = new SubscriberFactory();

        // Register factories
        theFactoryManager->RegisterFactory(theCommandFactory);
        theFactoryManager->RegisterFactory(theForceModelFactory);
        theFactoryManager->RegisterFactory(theParameterFactory);
        theFactoryManager->RegisterFactory(thePhysicalModelFactory);
        theFactoryManager->RegisterFactory(thePropSetupFactory);
        theFactoryManager->RegisterFactory(thePropagatorFactory);
        theFactoryManager->RegisterFactory(theSpacecraftFactory);
        theFactoryManager->RegisterFactory(theStopConditionFactory);
        theFactoryManager->RegisterFactory(theSubscriberFactory);

        // create default Spacecraft
        Spacecraft *defaultSc = CreateSpacecraft("Spacecraft", "DefaultSC");

        // create default SolarSystem
     
        //loj: reads in initial files, such as, system parameters, default mission script file.
        //loj: calls ScriptInterpreter to interpret initial script and create necessary objects
        //loj: creates initial commands and add to sandbox
        //loj: at this point the sandbox is ready to run without GUI.

        isInitialized = true;
    }

    return isInitialized;
}

//----- ObjectType
//------------------------------------------------------------------------------
// std::string GetObjectTypeString(Gmat::ObjectType type)
//------------------------------------------------------------------------------
std::string Moderator::GetObjectTypeString(Gmat::ObjectType type)
{
   if (type >= Gmat::SPACECRAFT && type <= Gmat::PROP_SETUP)
      return OBJECT_TYPE_STRING[type - Gmat::SPACECRAFT];
   else
      return "UnknownObject";
}

//----- interpreter
//------------------------------------------------------------------------------
// GuiInterpreter* GetGuiInterpreter()
//------------------------------------------------------------------------------
GuiInterpreter* Moderator::GetGuiInterpreter()
{
   return theGuiInterpreter;
}

//------------------------------------------------------------------------------
// ScriptInterpreter* GetScriptInterpreter()
//------------------------------------------------------------------------------
ScriptInterpreter* Moderator::GetScriptInterpreter()
{
   return theScriptInterpreter;
}

//------------------------------------------------------------------------------
// void SetGuiInterpreter(GuiInterpreter *guiInterp)
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
StringArray Moderator::GetListOfFactoryItems(Gmat::ObjectType type)
{
   return theFactoryManager->GetListOfItems(type);
}

//----- configuration
//------------------------------------------------------------------------------
// bool RemoveConfiguredItem(Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
bool Moderator::RemoveConfiguredItem(Gmat::ObjectType type, const std::string &name)
{
   return theConfigManager->RemoveItem(type, name);
}

//------------------------------------------------------------------------------
// StringArray GetListOfConfiguredItems(Gmat::ObjectType type)
//------------------------------------------------------------------------------
StringArray Moderator::GetListOfConfiguredItems(Gmat::ObjectType type)
{
   return theConfigManager->GetListOfItems(type);
}

// Spacecraft
//------------------------------------------------------------------------------
// Spacecraft* CreateSpacecraft(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
Spacecraft* Moderator::CreateSpacecraft(const std::string &type, const std::string &name)
{
   Spacecraft *sc =
      theFactoryManager->CreateSpacecraft(type, name);
   theConfigManager->AddSpacecraft(sc);
   return sc;
}

//------------------------------------------------------------------------------
// Spacecraft* GetSpacecraft(const std::string &name)
//------------------------------------------------------------------------------
Spacecraft* Moderator::GetSpacecraft(const std::string &name)
{
   return theConfigManager->GetSpacecraft(name);
}

// Propagator
//------------------------------------------------------------------------------
// Propagator* CreatePropagator(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
Propagator* Moderator::CreatePropagator(const std::string &type, const std::string &name)
{
   Propagator *prop = theFactoryManager->CreatePropagator(type, name);
//   theConfigManager->AddPropagator(prop);
   return prop;
}

//------------------------------------------------------------------------------
// Propagator* GetPropagator(const std::string &name)
//------------------------------------------------------------------------------
Propagator* Moderator::GetPropagator(const std::string &name)
{
   return theConfigManager->GetPropagator(name);
}

// PhysicalModel
//------------------------------------------------------------------------------
// PhysicalModel* CreatePhysicalModel(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
PhysicalModel* Moderator::CreatePhysicalModel(const std::string &type,
                                              const std::string &name)
{
   PhysicalModel *physicalModel = theFactoryManager->CreatePhysicalModel(type, name);
//   theConfigManager->AddPhysicalModel(physicalModel);
   return physicalModel;
}

//------------------------------------------------------------------------------
// PhysicalModel* GetPhysicalModel(const std::string &name)
//------------------------------------------------------------------------------
PhysicalModel* Moderator::GetPhysicalModel(const std::string &name)
{
   return theConfigManager->GetPhysicalModel(name);
}

// Parameter
//------------------------------------------------------------------------------
// Parameter* CreateParameter(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
Parameter* Moderator::CreateParameter(const std::string &type, const std::string &name)
{
   Parameter *parameter = theFactoryManager->CreateParameter(type, name);
   // Manage it if it is a named parameter
   if (parameter->GetName() != "")
      theConfigManager->AddParameter(parameter);
   return parameter;
   return NULL;
}

//------------------------------------------------------------------------------
// Parameter* GetParameter(const std::string &name)
//------------------------------------------------------------------------------
Parameter* Moderator::GetParameter(const std::string &name)
{
   return theConfigManager->GetParameter(name);
}

// ForceModel
//------------------------------------------------------------------------------
// ForceModel* CreateForceModel(const std::string &name)
//------------------------------------------------------------------------------
ForceModel* Moderator::CreateForceModel(const std::string &name)
{
   ForceModel *fm = theFactoryManager->CreateForceModel(name);
   if (name != "")
       theConfigManager->AddForceModel(fm);
   return fm;
}

//------------------------------------------------------------------------------
// ForceModel* GetForceModel(const std::string &name)
//------------------------------------------------------------------------------
ForceModel* Moderator::GetForceModel(const std::string &name)
{
   return theConfigManager->GetForceModel(name);
}

//------------------------------------------------------------------------------
// bool AddToForceModel(const std::string &forceModelName, const std::string &forceName)
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

//------------------------------------------------------------------------------
// StopCondition* CreateStopCondition(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
StopCondition* Moderator::CreateStopCondition(const std::string &type,
                                              const std::string &name)
{
   StopCondition *stopCond = theFactoryManager->CreateStopCondition(type, name);
//   theConfigManager->AddStopCondition(stopCond);
   return stopCond;
}

//------------------------------------------------------------------------------
// StopCondition* GetStopCondition(const std::string &name)
//------------------------------------------------------------------------------
StopCondition* Moderator::GetStopCondition(const std::string &name)
{
   return theConfigManager->GetStopCondition(name);
}

//------------------------------------------------------------------------------
// PropSetup* CreatePropSetup(const std::string &name, const std::string &propagatorName,
//                            const std::string &forceModelName)
//------------------------------------------------------------------------------
PropSetup* Moderator::CreatePropSetup(const std::string &name,
                                      const std::string &propagatorName,
                                      const std::string &forceModelName)
{
   // assumes propagatorName and forceModelName exist alreay.
   Propagator *prop = theConfigManager->GetPropagator(propagatorName);
   ForceModel *fm = theConfigManager->GetForceModel(forceModelName);
   PropSetup *propSetup = theFactoryManager->CreatePropSetup(name);
   propSetup->SetPropagator(prop);
   propSetup->SetForceModel(fm);
   theConfigManager->AddPropSetup(propSetup);
   return propSetup;
}

//------------------------------------------------------------------------------
// PropSetup* GetPropSetup(const std::string &name)
//------------------------------------------------------------------------------
PropSetup* Moderator::GetPropSetup(const std::string &name)
{
   return theConfigManager->GetPropSetup(name);
}

// celestial body
//------------------------------------------------------------------------------
// CelestialBody* CreateCelestialBody(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
CelestialBody* Moderator::CreateCelestialBody(const std::string &type,
                                              const std::string &name)
{
   // type is such as "Earth", "Moon" - we need const string EarthString etc.
   CelestialBody *body = theFactoryManager->CreateCelestialBody(type, name);
   theConfigManager->AddCelestialBody(body);
   return body;
}

//------------------------------------------------------------------------------
// CelestialBody* GetCelestialBody(const std::string &name)
//------------------------------------------------------------------------------
CelestialBody* Moderator::GetCelestialBody(const std::string &name)
{
   return theConfigManager->GetCelestialBody(name);
}

// solar system
//------------------------------------------------------------------------------
// SolarSystem* GetDefaultSolarSystem()
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
   SolarSystem *solarSys = theFactoryManager->CreateSolarSystem(name);
   theConfigManager->AddSolarSystem(solarSys);
   return solarSys;
}

//------------------------------------------------------------------------------
// bool SetSolarSystemInUse(const std::string &name)
//------------------------------------------------------------------------------
bool Moderator::SetSolarSystemInUse(const std::string &name)
{
   return theConfigManager->SetSolarSystemInUse(name);
}

//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystemInUse()
//------------------------------------------------------------------------------
SolarSystem* Moderator::GetSolarSystemInUse()
{
   return theConfigManager->GetSolarSystemInUse();
}

// subscriber
//------------------------------------------------------------------------------
// Subscriber* CreateSubscriber(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
Subscriber* Moderator::CreateSubscriber(const std::string &type, const std::string &name)
{
   Subscriber *subs = theFactoryManager->CreateSubscriber(type, name);
   theConfigManager->AddSubscriber(subs);
   return subs;
}

//------------------------------------------------------------------------------
// Subscriber* GetSubscriber(const std::string &name)
//------------------------------------------------------------------------------
Subscriber* Moderator::GetSubscriber(const std::string &name)
{
   return theConfigManager->GetSubscriber(name);
}

// command
//------------------------------------------------------------------------------
// Command* CreateCommand(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
Command* Moderator::CreateCommand(const std::string &type, const std::string &name)
{
   Command *cmd = theFactoryManager->CreateCommand(type, name);
//   theConfigManager->AddCommand(cmd);
   return cmd;
}

//------------------------------------------------------------------------------
// Command* GetCommand(const std::string &name)
//------------------------------------------------------------------------------
Command* Moderator::GetCommand(const std::string &name)
{
   return theConfigManager->GetCommand(name);
}

// command sequence
//------------------------------------------------------------------------------
// Command* GetNextCommand(Integer sandboxNum)
//------------------------------------------------------------------------------
Command* Moderator::GetNextCommand(Integer sandboxNum)
{
   return commands[sandboxNum-1]->GetNext();
}

//loj: future build implementation
//------------------------------------------------------------------------------
// bool DeleteCommand(const std::string &name, Integer position,
//                    Integer sandboxNum)
//------------------------------------------------------------------------------
bool Moderator::DeleteCommand(const std::string &name, Integer position,
                              Integer sandboxNum)
{
   // delete command from default sandbox 1
   //commands[sandboxNum-1]->DeleteCommand(name, position);
   return false;
}

//loj: future build implementation
//------------------------------------------------------------------------------
// Command* InsertCommand(const std::string &type, const std::string &name,
//                        Integer position, bool addAbove, Integer sandboxNum)
//------------------------------------------------------------------------------
Command* Moderator::InsertCommand(const std::string &type, const std::string &name,
                                  Integer position, bool addAbove,
                                  Integer sandboxNum)
{
//     bool status = false;
//     Command *cmd = theFactoryManager->CreateCommand(type, name);
//     status = commands[sandboxNum-1]->Insert(cmd, position, addAbove);
//     return cmd;
   return NULL;
}

//------------------------------------------------------------------------------
// Command* AppendCommand(const std::string &type, const std::string &name,
//                        Integer sandboxNum)
//------------------------------------------------------------------------------
Command* Moderator::AppendCommand(const std::string &type, const std::string &name,
                                  Integer sandboxNum)
{
   bool status;
   Command *cmd = theFactoryManager->CreateCommand(type, name);
   status = commands[sandboxNum-1]->Append(cmd);
   return cmd;
}

// sandbox
//------------------------------------------------------------------------------
// void ClearAllSandboxes()
//------------------------------------------------------------------------------
void Moderator::ClearAllSandboxes()
{
   for (int i=0; i<Gmat::MAX_SANDBOX; i++)
      sandboxes[i]->Clear();
}

//------------------------------------------------------------------------------
// Integer RunMission(Integer sandboxNum)
//------------------------------------------------------------------------------
Integer Moderator::RunMission(Integer sandboxNum)
{
   Integer status = 0;

   // check sandbox number
   if (sandboxNum > 0 && sandboxNum <= Gmat::MAX_SANDBOX)
      sandboxes[sandboxNum-1]->Clear();
   else
      ;// assign status

   try
   {
      AddSolarSysToSandbox(sandboxNum-1);
      AddSpacecraftToSandbox(sandboxNum-1);
      AddPropSetupToSandbox(sandboxNum-1);
      AddSubscriberToSandbox(sandboxNum-1);
      AddCommandToSandbox(sandboxNum-1);
      InitializeSandbox(sandboxNum-1);
      ExecuteSandbox(sandboxNum-1);
   }
   catch (BaseException &e)
   {
      // assign status
      throw;
   }
   catch (...)
   {
      // assign status
      throw;
   }

   return status;
}

//---------------------------------
//  protected
//---------------------------------

// sandbox
//------------------------------------------------------------------------------
// void AddSolarSysToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddSolarSysToSandbox(Integer index)
{
   //loj: Do I get clone of SolarSystem from ConfigManager?
   SolarSystem *solarSys = theConfigManager->GetSolarSystemInUse();
   sandboxes[index]->AddSolarSystem(solarSys);
}

//------------------------------------------------------------------------------
// void AddSpacecraftToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddSpacecraftToSandbox(Integer index)
{
   Spacecraft *sc;
   StringArray scNames = theConfigManager->GetListOfItems(Gmat::SPACECRAFT);

   for (int i=0; i<scNames.size(); i++)
   {
      sc = theConfigManager->GetSpacecraft(scNames[i]);
      sandboxes[index]->AddSpacecraft(sc);
   }
}

//------------------------------------------------------------------------------
// void AddPropSetupToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddPropSetupToSandbox(Integer index)
{
   PropSetup *propSetup;
   StringArray propSetupNames = theConfigManager->GetListOfItems(Gmat::PROP_SETUP);
   
   for (int i=0; i<propSetupNames.size(); i++)
   {
      propSetup = theConfigManager->GetPropSetup(propSetupNames[i]);
      sandboxes[index]->AddPropSetup(propSetup);
   }
}

//------------------------------------------------------------------------------
// void AddSubscriberToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddSubscriberToSandbox(Integer index)
{
   Subscriber *subs;
   StringArray subsNames = theConfigManager->GetListOfItems(Gmat::SUBSCRIBER);
   
   for (int i=0; i<subsNames.size(); i++)
   {
      subs = theConfigManager->GetSubscriber(subsNames[i]);
      sandboxes[index]->AddSubscriber(subs);
   }
}

//------------------------------------------------------------------------------
// void AddCommandToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddCommandToSandbox(Integer index)
{
   Command *cmd = commands[index]->GetNext();

//   while (cmd != NULL)
   if (cmd != NULL)
   {
      sandboxes[index]->AddCommand(cmd);
//      cmd = cmd->GetNext();
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
Moderator::Moderator()
{
   isInitialized = false;
   theFactoryManager = FactoryManager::Instance();
   theConfigManager = ConfigManager::Instance();
   sandboxes.reserve(Gmat::MAX_SANDBOX);
   commands.reserve(Gmat::MAX_SANDBOX);

   // create atleast 1 Sandbox and Command
   sandboxes.push_back(new Sandbox());
   commands.push_back(new NoOp());
}

//------------------------------------------------------------------------------
// ~Moderator()
//------------------------------------------------------------------------------
Moderator::~Moderator()
{
/*   if (instance != NULL)
//      delete instance;
//   if (theConfigManager != NULL)
//      delete theConfigManager;
   if (theFactoryManager != NULL)
      delete theFactoryManager;
   if (theGuiInterpreter != NULL)
      delete theGuiInterpreter;
//   if (theScriptInterpreter != NULL)
//      delete theScriptInterpreter;
   
   if (thePublisher != NULL)
      delete thePublisher;
   if (theCommandFactory != NULL)
      delete theCommandFactory;
   if (theForceModelFactory != NULL)
      delete theForceModelFactory;
   if (thePhysicalModelFactory != NULL)
      delete thePhysicalModelFactory;
   if (thePropSetupFactory != NULL)
      delete thePropSetupFactory;
   if (thePropagatorFactory != NULL)
      delete thePropagatorFactory;
   if (theSpacecraftFactory != NULL)
      delete theSpacecraftFactory;
   if (theStopConditionFactory != NULL)
      delete theStopConditionFactory;
   if (theSubscriberFactory != NULL)
      delete theSubscriberFactory;
*/
}


