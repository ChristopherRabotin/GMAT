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
 * Declares opeartions of the GMAT executive.
 */
//------------------------------------------------------------------------------
#ifndef Moderator_hpp
#define Moderator_hpp

#include "gmatdefs.hpp"
// executive
#include "Sandbox.hpp"
#include "GuiInterpreter.hpp"
#include "ScriptInterpreter.hpp"
#include "FactoryManager.hpp"
#include "ConfigManager.hpp"
#include "Publisher.hpp"
// core
#include "Burn.hpp"
#include "Command.hpp"
#include "PhysicalModel.hpp"
#include "ForceModel.hpp"
#include "Propagator.hpp"
#include "Spacecraft.hpp"
#include "Parameter.hpp"
#include "StopCondition.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "PropSetup.hpp"
#include "Subscriber.hpp"
// factories
#include "BurnFactory.hpp"
#include "CommandFactory.hpp"
#include "ForceModelFactory.hpp"
#include "ParameterFactory.hpp"
#include "PhysicalModelFactory.hpp"
#include "PropSetupFactory.hpp"
#include "PropagatorFactory.hpp"
#include "SpacecraftFactory.hpp"
#include "StopConditionFactory.hpp"
#include "SubscriberFactory.hpp"

namespace Gmat
{
    const Integer MAX_SANDBOX = 4;
};

class GMAT_API Moderator
{
public:

    static Moderator* Instance();
    bool Initialize();

    //----- ObjectType
    std::string GetObjectTypeString(Gmat::ObjectType type);  

    //----- interpreter
    static GuiInterpreter* GetGuiInterpreter();
    static ScriptInterpreter* GetScriptInterpreter();
    static void SetGuiInterpreter(GuiInterpreter *guiInterp);
    static void SetScriptInterpreter(ScriptInterpreter *scriptInterp);

    //----- factory
    StringArray GetListOfFactoryItems(Gmat::ObjectType type);

    //----- configuration
    StringArray& GetListOfConfiguredItems(Gmat::ObjectType type);
    bool RemoveConfiguredItem(Gmat::ObjectType, const std::string &name);

    // Spacecraft
    Spacecraft* CreateSpacecraft(const std::string &type, const std::string &name);
    Spacecraft* GetSpacecraft(const std::string &name);
    //future build:GroundStation* CreateGroundStation(const std::string &type, const std::string &name);
    //future build:GroundStation* GetGroundStation(const std::string &name);

    // Propagator
    Propagator* CreatePropagator(const std::string &type, const std::string &name);
    Propagator* GetPropagator(const std::string &name);

    // PhysicalModel
    PhysicalModel* CreatePhysicalModel(const std::string &type, const std::string &name);
    PhysicalModel* GetPhysicalModel(const std::string &name);

    // Burn
    Burn* CreateBurn(const std::string &type, const std::string &name);
    Burn* GetBurn(const std::string &name);

    // Parameter
    Parameter* CreateParameter(const std::string &type, const std::string &name);
    Parameter* GetParameter(const std::string &name);

    // ForceModel
    ForceModel* CreateForceModel(const std::string &name);
    ForceModel* GetForceModel(const std::string &name);
    bool AddToForceModel(const std::string &forceModelName, const std::string &forceName);

    // StopCondition
    StopCondition* CreateStopCondition(const std::string &type, const std::string &name);
    StopCondition* GetStopCondition(const std::string &name);
    bool AddToStopCondition(const std::string &stopCondName, const std::string &paramName);

    // PropSetup
    PropSetup* CreateDefaultPropSetup(const std::string &name);
    PropSetup* CreatePropSetup(const std::string &name,
                               const std::string &propagatorName = "",
                               const std::string &forceModelName = "");
    PropSetup* GetPropSetup(const std::string &name);

    // CelestialBody
    CelestialBody* CreateCelestialBody(const std::string &type, const std::string &name);
    CelestialBody* GetCelestialBody(const std::string &name);

    // SolarSystem
    SolarSystem* GetDefaultSolarSystem();
    SolarSystem* CreateSolarSystem(const std::string &name);
    bool SetSolarSystemInUse(const std::string &name);
    SolarSystem* GetSolarSystemInUse();

    // Subscriber
    Subscriber* CreateSubscriber(const std::string &type, const std::string &name,
                                 const std::string &filename = "");
    Subscriber* GetSubscriber(const std::string &name);

    // Command
    Command* CreateCommand(const std::string &type, const std::string &name);
    Command* GetCommand(const std::string &name);

    Command* GetNextCommand(Integer sanboxNum = 1);
    bool DeleteCommand(const std::string &name, Integer position,
                       Integer sandboxNum = 1);
    Command* InsertCommand(const std::string &type, const std::string &name,
                           Integer position, bool addAbove = true,
                           Integer sandboxNum = 1);
    Command* AppendCommand(const std::string &type, const std::string &name,
                           Integer sandboxNum = 1);

    // Sandbox
    void ClearAllSandboxes();
    Integer RunMission(Integer sandboxNum = 1);

    // Script
    bool InterpretScript(const std::string &scriptFilename);
    bool SaveScript(const std::string &scriptFilename);
    Integer RunScript(Integer sandboxNum = 1);
    
private:

    // sandbox
    void AddSolarSysToSandbox(Integer index);
    void AddSpacecraftToSandbox(Integer index);
    void AddPropSetupToSandbox(Integer index);
    void AddBurnToSandbox(Integer index);
    void AddSubscriberToSandbox(Integer index);
    void AddCommandToSandbox(Integer index);
    void InitializeSandbox(Integer index);
    void ExecuteSandbox(Integer index);

    Moderator();
    virtual ~Moderator();
    Moderator(const Moderator&);
    Moderator& operator=(const Moderator&);

    // member data
    bool isInitialized;
    std::vector<Sandbox*> sandboxes;
    std::vector<Command*> commands;

    static Moderator *instance;
    static ConfigManager *theConfigManager;
    static FactoryManager *theFactoryManager;
    static GuiInterpreter *theGuiInterpreter;
    static ScriptInterpreter *theScriptInterpreter;

    Publisher *thePublisher;
    BurnFactory *theBurnFactory;
    CommandFactory *theCommandFactory;
    ForceModelFactory *theForceModelFactory;
    ParameterFactory *theParameterFactory;
    PhysicalModelFactory *thePhysicalModelFactory;
    PropSetupFactory *thePropSetupFactory;
    PropagatorFactory *thePropagatorFactory;
    SpacecraftFactory *theSpacecraftFactory;
    StopConditionFactory *theStopConditionFactory;
    SubscriberFactory *theSubscriberFactory;

    static const std::string OBJECT_TYPE_STRING[Gmat::UNKNOWN_OBJECT-Gmat::SPACECRAFT+1];

};

#endif // Moderator_hpp

