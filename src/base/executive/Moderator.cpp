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
#include "BaseException.hpp"
#include "Moderator.hpp"
#include "GuiInterpreter.hpp"
#include "ScriptInterpreter.hpp"
#include "Sandbox.hpp"
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
#include "CommandSequence.hpp"
#include "Subscriber.hpp"
#include "FactoryManager.hpp"
#include "ConfigManager.hpp"

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
   if (instance = NULL)
      instance = new Moderator;
   return instance;
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool Moderator::Initialize()
{
   // Create GuiInterpreter, ScriptInterpreter (assuming singleton)
   // Do we need more than one interpretes?
   theGuiInterpreter = GuiInterpreter::Instance();
   theScriptInterpreter = ScriptInterpreter::Instance();
   theFactoryManager = FactoryManager::Instance();
   theConfigManager = ConfigManager::Instance();
   
   //loj: reads in initial files, such as, system parameters, default mission senario in script format
   //loj: calls ScriptInterpreter to interpret initial script and create necessary objects
   //loj: creates initial commands and add to sandbox
   //loj: at this point the sandbox is ready to run without GUI.
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
// Spacecraft* CreateSpacecraft(std::string type, const std::string &name)
//------------------------------------------------------------------------------
Spacecraft* Moderator::CreateSpacecraft(std::string type, const std::string &name)
{
   Spacecraft *sc =
      theFactoryManager->CreateSpacecraft(type, name);
   theConfigManager->AddSpacecraft(sc, name);
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
// Propagator* CreatePropagator(std::string type, const std::string &name)
//------------------------------------------------------------------------------
Propagator* Moderator::CreatePropagator(std::string type, const std::string &name)
{
   Propagator *prop = theFactoryManager->CreatePropagator(type, name);
   theConfigManager->AddPropagator(prop, name);
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
// PhysicalModel* CreatePhysicalModel(std::string type, const std::string &name)
//------------------------------------------------------------------------------
PhysicalModel* Moderator::CreatePhysicalModel(std::string type,
                                              const std::string &name)
{
   PhysicalModel *physicalModel = theFactoryManager->CreatePhysicalModel(type, name);
   theConfigManager->AddPhysicalModel(physicalModel, name);
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
// Parameter* CreateParameter(std::string type, const std::string &name)
//------------------------------------------------------------------------------
Parameter* Moderator::CreateParameter(std::string type, const std::string &name)
{
   //loj: not implemented in build 1
//     Parameter *parameter = theFactoryManager->CreateParameter(type, name);
//     theConfigManager->AddParameter(parameter, name);
//     return parameter;
   return NULL;
}

//------------------------------------------------------------------------------
// Parameter* GetParameter(const std::string &name)
//------------------------------------------------------------------------------
Parameter* Moderator::GetParameter(const std::string &name)
{
   //loj: not implemented in build 1
//     return theConfigManager->GetParameter(name);
   return NULL;
}

// ForceModel
//------------------------------------------------------------------------------
// ForceModel* CreateForceModel(const std::string &name)
//------------------------------------------------------------------------------
ForceModel* Moderator::CreateForceModel(const std::string &name)
{
   ForceModel *fm = theFactoryManager->CreateForceModel(name);
   theConfigManager->AddForceModel(fm, name);
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
// bool AddToForceModel(std::string forceModelName, std::string forceName)
//------------------------------------------------------------------------------
bool Moderator::AddToForceModel(std::string forceModelName, std::string forceName)
{
   bool status = true;
   ForceModel *fm = theConfigManager->GetForceModel(forceModelName);
   PhysicalModel *physicalModel = theConfigManager->GetPhysicalModel(forceName);
   fm->AddForce(physicalModel);
   return status;
}

//------------------------------------------------------------------------------
// StopCondition* CreateStopCondition(const std::string &name)
//------------------------------------------------------------------------------
StopCondition* Moderator::CreateStopCondition(const std::string &name)
{
   StopCondition *stopCond = theFactoryManager->CreateStopCondition(name);
   theConfigManager->AddStopCondition(stopCond, name);
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
// PropSetup* CreatePropSetup(const std::string &name, std::string propagatorName,
//                                       std::string forceModelName)
//------------------------------------------------------------------------------
PropSetup* Moderator::CreatePropSetup(const std::string &name, std::string propagatorName,
                                      std::string forceModelName)
{
   // assumes propagatorName and forceModelName exist alreay.
   Propagator *prop = theConfigManager->GetPropagator(propagatorName);
   ForceModel *fm = theConfigManager->GetForceModel(forceModelName);
   PropSetup *propSetup = theFactoryManager->CreatePropSetup(name);
   propSetup->SetPropagator(prop);
   propSetup->SetForceModel(fm);
   theConfigManager->AddPropSetup(propSetup, name);
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
// CelestialBody* CreateCelestialBody(std::string type, const std::string &name)
//------------------------------------------------------------------------------
CelestialBody* Moderator::CreateCelestialBody(std::string type,
                                              const std::string &name)
{
   // type is such as "Earth", "Moon" - we need const string EarthString etc.
   CelestialBody *body = theFactoryManager->CreateCelestialBody(type, name);
   theConfigManager->AddCelestialBody(body, name);
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
   theConfigManager->AddSolarSystem(solarSys, name);
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
// Subscriber* CreateSubscriber(std::string type, const std::string &name)
//------------------------------------------------------------------------------
Subscriber* Moderator::CreateSubscriber(std::string type, const std::string &name)
{
   Subscriber *subs = theFactoryManager->CreateSubscriber(type, name);
   theConfigManager->AddSubscriber(subs, name);
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
// Command* CreateCommand(std::string type, const std::string &name)
//------------------------------------------------------------------------------
Command* Moderator::CreateCommand(std::string type, const std::string &name)
{
   Command *cmd = theFactoryManager->CreateCommand(type, name);
   theConfigManager->AddCommand(cmd, name);
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
   return commandSeqs[sandboxNum-1]->GetNextCommand();
}

//------------------------------------------------------------------------------
// bool DeleteCommand(const std::string &name, Integer position,
//                    Integer sandboxNum)
//------------------------------------------------------------------------------
bool Moderator::DeleteCommand(const std::string &name, Integer position,
                              Integer sandboxNum)
{
   // delete command from default sandbox 1
   commandSeqs[sandboxNum-1]->DeleteCommand(name, position);
}

//------------------------------------------------------------------------------
// Command* InsertCommand(std::string type, const std::string &name,
//                        Integer position, bool addAbove, Integer sandboxNum)
//------------------------------------------------------------------------------
Command* Moderator::InsertCommand(std::string type, const std::string &name,
                                  Integer position, bool addAbove,
                                  Integer sandboxNum)
{
   bool status;
   Command *cmd = theFactoryManager->CreateCommand(type, name);
   status = commandSeqs[sandboxNum-1]->InsertCommand(cmd, position, addAbove);
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
      InitializeSandbox(sandboxNum-1);
      AddCommandToSandbox(sandboxNum-1);
      ExecuteSandbox(sandboxNum-1);
   }
   catch (BaseException &e)
   {
      // assign status
   }
   catch (...)
   {
      // assign status
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
   Command *cmd;
   Integer numCommand = commandSeqs[index]->GetNumCommands();

   for (int i=0; i<numCommand; i++)
   {
      cmd = commandSeqs[index]->GetNextCommand();
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
Moderator::Moderator()
{
   isInitialized = false;
   theFactoryManager = FactoryManager::Instance();
   theConfigManager = ConfigManager::Instance();
   sandboxes.reserve(Gmat::MAX_SANDBOX);
   commandSeqs.reserve(Gmat::MAX_SANDBOX);

   // create atleast 1 Sandbox and CommandSeq
   sandboxes.push_back(new Sandbox());
   commandSeqs.push_back(new CommandSequence());
}

//------------------------------------------------------------------------------
// ~Moderator()
//------------------------------------------------------------------------------
Moderator::~Moderator()
{
   if (theFactoryManager != NULL)
      delete theFactoryManager;
   if (theConfigManager != NULL)
      delete theConfigManager;
   if (theGuiInterpreter != NULL)
      delete theGuiInterpreter;
   if (theScriptInterpreter != NULL)
      delete theScriptInterpreter;
}


