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
#include "Solver.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "PropSetup.hpp"
#include "Subscriber.hpp"
#include "SlpFile.hpp"
#include "Interpolator.hpp"
#include "RefFrame.hpp"
// factories
#include "BurnFactory.hpp"
#include "CommandFactory.hpp"
#include "ForceModelFactory.hpp"
#include "ParameterFactory.hpp"
#include "PhysicalModelFactory.hpp"
#include "PropSetupFactory.hpp"
#include "PropagatorFactory.hpp"
#include "SolverFactory.hpp"
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
    bool Initialize(bool isFromGui = false);

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
    //future build:GmatBase* GetConfiguredItem(Gmat::ObjectType type, const std::string &name);
    GmatBase* GetConfiguredItem(const std::string &name);
    bool RenameConfiguredItem(Gmat::ObjectType type, const std::string &oldName,
                              const std::string &newName);
    bool RemoveConfiguredItem(Gmat::ObjectType type, const std::string &name);
    
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

    // Solver
    Solver* CreateSolver(const std::string &type, const std::string &name);
    Solver* GetSolver(const std::string &name);

    // PropSetup
    PropSetup* CreateDefaultPropSetup(const std::string &name);
    PropSetup* CreatePropSetup(const std::string &name,
                               const std::string &propagatorName = "",
                               const std::string &forceModelName = "");
    PropSetup* GetPropSetup(const std::string &name);

    // CelestialBody
    CelestialBody* CreateCelestialBody(const std::string &type, const std::string &name);
    CelestialBody* GetCelestialBody(const std::string &name);

    // Interpolator //loj: 3/23/04 added
    Interpolator* CreateInterpolator(const std::string &type, const std::string &name);
    Interpolator* GetInterpolator(const std::string &name);

    // RefFrame //loj: 3/23/04 added
    RefFrame* CreateRefFrame(const std::string &type, const std::string &name);
    RefFrame* GetRefFrame(const std::string &name);

    // SolarSystem
    SolarSystem* GetDefaultSolarSystem();
    SolarSystem* CreateSolarSystem(const std::string &name);
    SolarSystem* GetSolarSystemInUse();
    StringArray& GetSolarSystemSourceList();
    StringArray& GetSolarSystemSourceFileList();
    bool SetSolarSystemInUse(const std::string &name);
    bool SetSlpFileToUse(const std::string &filename);

    // Subscriber
    Subscriber* CreateSubscriber(const std::string &type, const std::string &name,
                                 const std::string &filename = "");
    Subscriber* GetSubscriber(const std::string &name);

    // GmatCommand
    GmatCommand* CreateCommand(const std::string &type, const std::string &name = "");

    // Mission
    bool LoadDefaultMission();
    
    // Resource
    bool ClearResource();
    
    // Mission sequence
    bool ClearCommandSeq(Integer sandboxNum = 1);
    bool AppendCommand(GmatCommand *cmd, Integer sandboxNum = 1);
    GmatCommand* AppendCommand(const std::string &type, const std::string &name,
                           Integer sandboxNum = 1);
    bool InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd,
                       Integer sandboxNum = 1);
    GmatCommand* InsertCommand(const std::string &type, const std::string &currName,
                           const std::string &prevName, Integer sandboxNum = 1);
    GmatCommand* DeleteCommand(GmatCommand *cmd, Integer sandboxNum = 1);
    GmatCommand* GetNextCommand(Integer sanboxNum = 1);

    // Sandbox
    void ClearAllSandboxes();
    Integer RunMission(Integer sandboxNum = 1, bool isFromGui = false);

    // Script
    bool InterpretScript(const std::string &scriptFilename);
    bool SaveScript(const std::string &scriptFilename);
    Integer RunScript(Integer sandboxNum = 1, bool isFromGui = false);
    
private:

    // initialization
    void CreateDefaultMission();
    void SetupRun(Integer sandboxNum, bool isFromGui = false);
    
    // sandbox
    void AddSolarSysToSandbox(Integer index);
    void AddSubscriberToSandbox(Integer index);
    void AddSpacecraftToSandbox(Integer index);
    void AddPropSetupToSandbox(Integer index);
    void AddBurnToSandbox(Integer index);
    void AddSolverToSandbox(Integer index);
    void AddPublisherToSandbox(Integer index);
    void AddCommandToSandbox(Integer index);
    void InitializeSandbox(Integer index);
    void ExecuteSandbox(Integer index);

    Moderator();
    virtual ~Moderator();
    Moderator(const Moderator&);
    Moderator& operator=(const Moderator&);

    // member data
    bool isInitialized;
    bool isSlpAlreadyInUse;
    bool isRunReady;
    
    std::vector<Sandbox*> sandboxes;
    std::vector<GmatCommand*> commands;

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
    SolverFactory *theSolverFactory;

    SolarSystem *theDefaultSolarSystem;
    SlpFile *theDefaultSlpFile;
    StringArray theSolarSystemSourceList;
    StringArray theSolarSystemSourceFileList;
    static const std::string OBJECT_TYPE_STRING[Gmat::UNKNOWN_OBJECT-Gmat::SPACECRAFT+1];

};

#endif // Moderator_hpp

