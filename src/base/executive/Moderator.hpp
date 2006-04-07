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
 * Declares opeartions of the GMAT executive. It is a singleton class -
 * only one instance of this class can be created.
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
#include "FileManager.hpp"
// core
#include "AtmosphereModel.hpp"
#include "AxisSystem.hpp"
#include "Burn.hpp"
#include "GmatCommand.hpp"
#include "CoordinateSystem.hpp"
#include "Function.hpp"
#include "Hardware.hpp"
#include "PhysicalModel.hpp"
#include "ForceModel.hpp"
#include "Propagator.hpp"
#include "Spacecraft.hpp"
#include "Formation.hpp"
#include "Parameter.hpp"
#include "StopCondition.hpp"
#include "Solver.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "PropSetup.hpp"
#include "Subscriber.hpp"
#include "Interpolator.hpp"
#include "CalculatedPoint.hpp"
#include "MathNode.hpp"
// files
#include "SlpFile.hpp"
#include "DeFile.hpp"
#include "EopFile.hpp"
#include "ItrfCoefficientsFile.hpp"
#include "LeapSecsFileReader.hpp"

namespace Gmat
{
   const Integer MAX_SANDBOX = 4;
};

class GMAT_API Moderator
{
public:

   static Moderator* Instance();
   bool Initialize(bool isFromGui = false);
   void Finalize();
   void SetRunReady(bool flag = true);
   void SetShowFinalState(bool flag = true);
   
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
   GmatBase* GetConfiguredItem(const std::string &name);
   bool RenameConfiguredItem(Gmat::ObjectType type, const std::string &oldName,
                             const std::string &newName);
   bool RemoveConfiguredItem(Gmat::ObjectType type, const std::string &name,
                             bool delOnlyIfNotUsed);
   bool HasConfigurationChanged(Integer sandboxNum = 1);
   void ConfigurationChanged(GmatBase *obj, bool tf);
   void ResetConfigurationChanged(bool resetResource = true,
                                  bool resetCommands = true,
                                  Integer sandboxNum = 1);
   
   // SolarSystem
   SolarSystem* GetDefaultSolarSystem();
   SolarSystem* CreateSolarSystem(const std::string &name);
   SolarSystem* GetSolarSystemInUse();
   bool SetSolarSystemInUse(const std::string &name);
   void SetSolarSystemInUse(SolarSystem *ss);
   
   // CalculatedPoint
   CalculatedPoint* CreateCalculatedPoint(const std::string &type,
                                          const std::string &name);
   CalculatedPoint* GetCalculatedPoint(const std::string &name);

   // CelestialBody
   CelestialBody* CreateCelestialBody(const std::string &type,
                                      const std::string &name);
   CelestialBody* GetCelestialBody(const std::string &name);

   // Spacecraft
   SpaceObject* CreateSpacecraft(const std::string &type,
                                 const std::string &name);
   SpaceObject* GetSpacecraft(const std::string &name);

   // Hardware
   Hardware* CreateHardware(const std::string &type,
                            const std::string &name);
   Hardware* GetHardware(const std::string &name);

   // Propagator
   Propagator* CreatePropagator(const std::string &type,
                                const std::string &name);
   Propagator* GetPropagator(const std::string &name);

   // PhysicalModel
   PhysicalModel* CreatePhysicalModel(const std::string &type,
                                      const std::string &name);
   PhysicalModel* GetPhysicalModel(const std::string &name);

   // AtmosphereModel
   AtmosphereModel* CreateAtmosphereModel(const std::string &type,
                                          const std::string &name,
                                          const std::string &body = "Earth");
   AtmosphereModel* GetAtmosphereModel(const std::string &name);
   
   // Burn
   Burn* CreateBurn(const std::string &type,
                    const std::string &name);
   Burn* GetBurn(const std::string &name);

   // Parameter
   bool IsParameter(const std::string &type);
   Parameter* CreateParameter(const std::string &type,
                              const std::string &name,
                              const Gmat::ObjectType ownerType = Gmat::UNKNOWN_OBJECT,
                              const std::string &ownerName = "",
                              const std::string &depName = "");
   Parameter* GetParameter(const std::string &name);

   // ForceModel
   ForceModel* CreateForceModel(const std::string &name);
   ForceModel* GetForceModel(const std::string &name);
   bool AddToForceModel(const std::string &forceModelName,
                        const std::string &forceName);

   // Solver
   Solver* CreateSolver(const std::string &type,
                        const std::string &name);
   Solver* GetSolver(const std::string &name);

   // PropSetup
   PropSetup* CreateDefaultPropSetup(const std::string &name);
   PropSetup* CreatePropSetup(const std::string &name,
                              const std::string &propagatorName = "",
                              const std::string &forceModelName = "");
   PropSetup* GetPropSetup(const std::string &name);
   
   // Interpolator
   Interpolator* CreateInterpolator(const std::string &type,
                                    const std::string &name);
   Interpolator* GetInterpolator(const std::string &name);
   
   // CoordinateSystem
   CoordinateSystem* CreateCoordinateSystem(const std::string &name,
                                            bool createDefault = false);
   CoordinateSystem* GetCoordinateSystem(const std::string &name);
   
   // Subscriber
   Subscriber* CreateSubscriber(const std::string &type,
                                const std::string &name,
                                const std::string &fileName = "",
                                bool createDefault = false);
   Subscriber* GetSubscriber(const std::string &name);
   
   // Function
   Function* CreateFunction(const std::string &type,
                            const std::string &name);
   Function* GetFunction(const std::string &name);

   //----- Non-Configurable Items
   // StopCondition
   StopCondition* CreateStopCondition(const std::string &type,
                                      const std::string &name);

   // AxisSystem
   AxisSystem* CreateAxisSystem(const std::string &type,
                                const std::string &name);
   
   // MathNode
   MathNode* CreateMathNode(const std::string &type,
                            const std::string &name = "");
   
   // GmatCommand
   GmatCommand* InterpretGmatFunction(const std::string &functionFilename);
   GmatCommand* CreateCommand(const std::string &type,
                              const std::string &name = "");
   GmatCommand* CreateDefaultCommand(const std::string &type,
                                     const std::string &name = "");
   void         SetCommandsUnchanged(Integer whichList = 0); 

   // CoordinateSystem
   CoordinateSystem* GetInternalCoordinateSystem();
   
   // Planetary files
   StringArray& GetPlanetarySourceTypes();
   StringArray& GetPlanetarySourceNames();
   StringArray& GetPlanetarySourceTypesInUse();
   StringArray& GetAnalyticModelNames();
   bool SetAnalyticModelToUse(const std::string &modelName);
   bool SetPlanetarySourceName(const std::string &sourceType,
                               const std::string &fileName);
   Integer SetPlanetarySourceTypesInUse(const StringArray &sourceTypes); 
   Integer GetPlanetarySourceId(const std::string &sourceType);
   std::string GetPlanetarySourceName(const std::string &sourceType);
   std::string GetCurrentPlanetarySource();
   
   // Potential field files
   std::string GetPotentialFileName(const std::string &fileType);
   
   // Getting file names
   // This will eventually replace Get*FileName() above (loj: 7/7/05)
   std::string GetFileName(const std::string &fileType);
   
   // Mission
   bool LoadDefaultMission();
   
   // Resource
   bool ClearResource();
    
   // Mission sequence
   bool ClearCommandSeq(Integer sandboxNum = 1);
   bool AppendCommand(GmatCommand *cmd, Integer sandboxNum = 1);
   GmatCommand* AppendCommand(const std::string &type,
                              const std::string &name,
                              Integer sandboxNum = 1);
   bool InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd,
                      Integer sandboxNum = 1);
   GmatCommand* DeleteCommand(GmatCommand *cmd, Integer sandboxNum = 1);
   GmatCommand* GetNextCommand(Integer sanboxNum = 1);
   
   // Sandbox
   void ClearAllSandboxes();
   GmatBase* GetInternalObject(const std::string &name, Integer sandboxNum = 1);
   Integer RunMission(Integer sandboxNum = 1);
   Integer ChangeRunState(const std::string &state, Integer sandboxNum = 1);
   Gmat::RunState GetUserInterrupt();
   Gmat::RunState GetRunState();
   
   // Script
   bool InterpretScript(const std::string &scriptFileName);
   bool InterpretScript(std::istringstream *ss, bool clearObjs);
   bool SaveScript(const std::string &scriptFileName,
                   Gmat::WriteMode mode = Gmat::SCRIPTING);
   Integer RunScript(Integer sandboxNum = 1);
   
private:

   // initialization
   void InitializePlanetarySource();
   void InitializePlanetaryCoeffFile();
   void InitializeTimeFile();
   
   void SetDefaultPlanetarySource();
   void CreateSolarSystemInUse();
   void CreateDefaultCoordSystems();
   void CreateDefaultMission();
   bool CreateSlpFile(const std::string &fileName);
   bool CreateDeFile(const Integer id, const std::string &fileName,
                     Gmat::DeFileFormat format = Gmat::DE_BINARY);

   // default objects
   Spacecraft* GetDefaultSpacecraft();
   PropSetup* GetDefaultPropSetup();
   Burn* GetDefaultBurn();
   Solver* GetDefaultSolver();
   Subscriber* GetDefaultSubscriber();
   StopCondition* CreateDefaultStopCondition();
   Parameter* GetDefaultX();
   Parameter* GetDefaultY();
   
   // sandbox
   void AddSolarSystemToSandbox(Integer index);
   void AddInternalCoordSystemToSandbox(Integer index);
   void AddPublisherToSandbox(Integer index);
   void AddCoordSystemToSandbox(Integer index);
   void AddSpacecraftToSandbox(Integer index);
   void AddFormationToSandbox(Integer index);
   void AddPropSetupToSandbox(Integer index);
   void AddPropagatorToSandbox(Integer index);
   void AddForceModelToSandbox(Integer index);
   void AddBurnToSandbox(Integer index);
   void AddSolverToSandbox(Integer index);
   void AddSubscriberToSandbox(Integer index);
   void AddParameterToSandbox(Integer index);
   void AddFunctionToSandbox(Integer index);
   void AddCommandToSandbox(Integer index);
   void InitializeSandbox(Integer index);
   void ExecuteSandbox(Integer index);
   
   Moderator();
   virtual ~Moderator();
   
   // member data
   bool isSlpAlreadyInUse;
   bool isRunReady;
   bool isFromGui;
   bool endOfInterpreter;
   bool showFinalState;
   std::vector<Sandbox*> sandboxes;
   std::vector<GmatCommand*> commands;
   
   static Moderator *instance;
   static GuiInterpreter *theGuiInterpreter;
   static ScriptInterpreter *theScriptInterpreter;
   ConfigManager *theConfigManager;
   FactoryManager *theFactoryManager;
   FileManager *theFileManager;
   Publisher *thePublisher;
   
   SolarSystem *theDefaultSolarSystem;
   SolarSystem *theSolarSystemInUse;
   CoordinateSystem *theInternalCoordSystem;
   SlpFile *theDefaultSlpFile;
   DeFile *theDefaultDeFile;
   StringArray thePlanetarySourceTypes;
   StringArray thePlanetarySourceNames;
   StringArray thePlanetarySourceTypesInUse;
   StringArray theAnalyticModelNames;
   StringArray theTempFileList;
   StringArray theSpacePointList;
   EopFile *theEopFile;
   ItrfCoefficientsFile *theItrfFile;
   LeapSecsFileReader *theLeapSecsFile;
   Gmat::AnalyticMethod theAnalyticMethod;
   
   enum
   {
      ANALYTIC = 0,
      SLP,
      DE200,
      DE405,
      PlanetarySourceCount,
   };
   
   enum
   {
      LOW_FIDELITY = 0,
      AnalyticModelCount,
   };

   std::string theCurrentPlanetarySource;
   Integer thePlanetarySourcePriority[PlanetarySourceCount];
   bool isPlanetarySourceInUse[PlanetarySourceCount];
   static const std::string PLANETARY_SOURCE_STRING[PlanetarySourceCount];
   static const std::string ANALYTIC_MODEL_STRING[AnalyticModelCount];
   static const Integer HIGHEST_PRIORITY = 10;
   Gmat::RunState runState;
};

#endif // Moderator_hpp

