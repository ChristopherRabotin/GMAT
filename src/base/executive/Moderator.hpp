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
// Author: Linda Jun
// Created: 2003/08/25
//
/**
 * Declares operations of the GMAT executive. It is a singleton class -
 * only one instance of this class can be created.
 */
//------------------------------------------------------------------------------
#ifndef Moderator_hpp
#define Moderator_hpp

#include "gmatdefs.hpp"
// executive
#include "Sandbox.hpp"
#include "ScriptInterpreter.hpp"
#include "FactoryManager.hpp"
#include "ConfigManager.hpp"
#include "Publisher.hpp"
#include "FileManager.hpp"
// core
#include "AtmosphereModel.hpp"
#include "Attitude.hpp"
#include "AxisSystem.hpp"
#include "Burn.hpp"
#include "GmatCommand.hpp"
#include "CoordinateSystem.hpp"
#include "Function.hpp"
#include "Hardware.hpp"
#include "PhysicalModel.hpp"
#include "ODEModel.hpp"
#include "Propagator.hpp"
#include "Spacecraft.hpp"
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
#include "EopFile.hpp"
#include "ItrfCoefficientsFile.hpp"
#include "LeapSecsFileReader.hpp"
// plug-in code
#include "DynamicLibrary.hpp"
#include "TriggerManager.hpp"

class DataFile;
class ObType;
class Interface;
class EventLocator;

namespace Gmat
{
   const Integer MAX_SANDBOX = 4;
};

class GMAT_API Moderator
{
public:

   static Moderator* Instance();
   bool Initialize(const std::string &startupFile = "", bool isFromGui = false);
   void Finalize();
   void SetRunReady(bool flag = true);
   void SetShowFinalState(bool flag = true);
   
   //----- Matlab engine
   Interface* GetMatlabInterface();
   bool OpenMatlabEngine();
   bool CloseMatlabEngine();
   
   //----- Plug-in code
   void LoadPlugins();
   void LoadAPlugin(std::string pluginName);
   DynamicLibrary *LoadLibrary(const std::string &libraryName);
   bool IsLibraryLoaded(const std::string &libName);
   void (*GetDynamicFunction(const std::string &funName, 
                             const std::string &libraryName))();
   
   //----- ObjectType
   std::string GetObjectTypeString(Gmat::ObjectType type);  
   
   //----- interpreter
   static ScriptInterpreter* GetUiInterpreter();
   static ScriptInterpreter* GetScriptInterpreter();
   static void SetUiInterpreter(ScriptInterpreter *uiInterp);
   static void SetScriptInterpreter(ScriptInterpreter *scriptInterp);
   
   void SetInterpreterMapAndSS(Interpreter *interp);
   
   //----- object map
   void SetObjectMap(ObjectMap *objMap);
   void SetObjectManageOption(Integer option);
   Integer GetObjectManageOption();
   void ResetObjectPointer(ObjectMap *objMap, GmatBase *newobj,
                           const std::string &name);
   
   //----- factory
   const StringArray& GetListOfFactoryItems(Gmat::ObjectType type, const std::string &qualifier = "");
   const StringArray& GetListOfAllFactoryItems();
   const StringArray& GetListOfAllFactoryItemsExcept(const ObjectTypeArray &types);
   const StringArray& GetListOfViewableItems(Gmat::ObjectType type);
   const StringArray& GetListOfViewableItems(const std::string &typeName);
   const StringArray& GetListOfUnviewableItems(Gmat::ObjectType type);
   const StringArray& GetListOfUnviewableItems(const std::string &typeName);
   bool               DoesObjectTypeMatchSubtype(
                            const Gmat::ObjectType coreType,
                            const std::string &theType,
                            const std::string &theSubtype);
   
   //----- configuration
   ObjectMap* GetConfiguredObjectMap();
   const StringArray& GetListOfObjects(Gmat::ObjectType type,
                                       bool excludeDefaultObjects = false);
   const StringArray& GetListOfObjects(const std::string &typeName,
                                       bool excludeDefaultObjects = false);
   GmatBase* GetConfiguredObject(const std::string &name);
   GmatBase* AddClone(const std::string &name, std::string &newName);
   bool ReconfigureItem(GmatBase *newobj, const std::string &name);
   std::string GetNewName(const std::string &name, Integer startCount);
   bool RenameObject(Gmat::ObjectType type, const std::string &oldName,
                     const std::string &newName);
   bool RemoveObject(Gmat::ObjectType type, const std::string &name,
                     bool delOnlyIfNotUsed);
   bool HasConfigurationChanged(Integer sandboxNum = 1);
   void ConfigurationChanged(GmatBase *obj, bool tf);
   void ResetConfigurationChanged(bool resetResource = true,
                                  bool resetCommands = true,
                                  Integer sandboxNum = 1);
   
   // SolarSystem
   SolarSystem* GetDefaultSolarSystem();
   SolarSystem* CreateSolarSystem(const std::string &name);
   SolarSystem* GetSolarSystemInUse(Integer manage = 1);
   void SetSolarSystemInUse(SolarSystem *ss);
   void SetInternalSolarSystem(SolarSystem *ss);
   bool SetSolarSystemInUse(const std::string &name);
   
   // CalculatedPoint
   CalculatedPoint* CreateCalculatedPoint(const std::string &type,
                                          const std::string &name,
                                          bool addDefaultBodies = true);
   CalculatedPoint* GetCalculatedPoint(const std::string &name);
   
   // CelestialBody
   CelestialBody* CreateCelestialBody(const std::string &type,
                                      const std::string &name);
   CelestialBody* GetCelestialBody(const std::string &name);

   // Spacecraft
   SpaceObject* CreateSpacecraft(const std::string &type,
                                 const std::string &name,
                                 bool createDefault = false);
   SpaceObject* GetSpacecraft(const std::string &name);
   std::string GetSpacecraftNotInFormation();
   
   // SpacePoints
   SpacePoint* CreateSpacePoint(const std::string &type,
                                 const std::string &name);
   SpacePoint* GetSpacePoint(const std::string &name);
   
   // Hardware
   Hardware* CreateHardware(const std::string &type,
                            const std::string &name);
   Hardware* GetHardware(const std::string &name);
   
   // Propagator
   Propagator* CreatePropagator(const std::string &type,
                                const std::string &name);
   Propagator* GetPropagator(const std::string &name);
   
   // PhysicalModel
   PhysicalModel* CreateDefaultPhysicalModel(const std::string &name);
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
                    const std::string &name, bool createDefault = false);
   Burn* GetBurn(const std::string &name);
   
   // Parameter
   bool IsParameter(const std::string &type);
   Parameter* CreateAutoParameter(const std::string &type,
                                  const std::string &name,
                                  bool &alreadyManaged,
                                  const std::string &ownerName = "",
                                  const std::string &depName = "",
                                  Integer manage = 1);
   Parameter* CreateParameter(const std::string &type,
                              const std::string &name,
                              const std::string &ownerName = "",
                              const std::string &depName = "",
                              Integer manage = 1);
   Parameter* GetParameter(const std::string &name);
   void SetParameterRefObject(Parameter *param, const std::string &type,
                              const std::string &name,
                              const std::string &ownerName,
                              const std::string &depName, Integer manage);
   
   // ODEModel
   ODEModel* CreateDefaultODEModel(const std::string &name);
   ODEModel* CreateODEModel(const std::string &type, const std::string &name);
   ODEModel* GetODEModel(const std::string &name);
   bool AddToODEModel(const std::string &odeModelName,
                        const std::string &forceName);
   
   // Solver
   Solver* CreateSolver(const std::string &type,
                        const std::string &name);
   Solver* GetSolver(const std::string &name);
   
   // PropSetup
   PropSetup* CreateDefaultPropSetup(const std::string &name);
   PropSetup* CreatePropSetup(const std::string &name);
   PropSetup* GetPropSetup(const std::string &name);
   
   // MeasurementModel
   MeasurementModel* CreateMeasurementModel(const std::string &name);
   MeasurementModel* GetMeasurementModel(const std::string &name);
   
   // TrackingSystem
   TrackingSystem* CreateTrackingSystem(const std::string &type,
                                        const std::string &name);
   TrackingSystem* GetTrackingSystem(const std::string &name);

   // TrackingData
   TrackingData* CreateTrackingData(const std::string &name);
   TrackingData* GetTrackingData(const std::string &name);

   // Core Measurement
   CoreMeasurement* CreateMeasurement(const std::string &type,
         const std::string &name);
   CoreMeasurement* GetMeasurement(const std::string &type,
         const std::string &name);
   
   // DataFile
   DataFile* CreateDataFile(const std::string &type,
                            const std::string &name);
   DataFile* GetDataFile(const std::string &name);
   
   // ObType
   ObType* CreateObType(const std::string &type,
                        const std::string &name);
   ObType* GetObType(const std::string &name);

   // EventLocator
   EventLocator* CreateEventLocator(const std::string &type,
                            const std::string &name);
   EventLocator* GetEventLocator(const std::string &name);

   // Interpolator
   Interpolator* CreateInterpolator(const std::string &type,
                                    const std::string &name);
   Interpolator* GetInterpolator(const std::string &name);
   
   // CoordinateSystem
   CoordinateSystem* CreateCoordinateSystem(const std::string &name,
                                            bool createDefault = false,
                                            bool internal = false,
                                            Integer manage = 1);
   CoordinateSystem* GetCoordinateSystem(const std::string &name);
   const StringArray& GetDefaultCoordinateSystemNames();
   bool IsDefaultCoordinateSystem(const std::string &name);
   
   // Subscriber
   Subscriber* CreateSubscriber(const std::string &type,
                                const std::string &name,
                                const std::string &fileName = "",
                                bool createDefault = false);
   Subscriber* GetSubscriber(const std::string &name);
   Integer GetNumberOfActivePlots();
   
   // EphemerisFile
   Subscriber* CreateEphemerisFile(const std::string &type,
                                   const std::string &name);
   Subscriber* GetEphemerisFile(const std::string &name);
   void        HandleCcsdsEphemerisFile(ObjectMap *objMap, bool deleteOld = false);
   
   // Function
   Function* CreateFunction(const std::string &type,
                            const std::string &name,
                            Integer manage = 1);
   Function* GetFunction(const std::string &name);
   
   // Create other object
   GmatBase* CreateOtherObject(Gmat::ObjectType objType, const std::string &type,
                               const std::string &name, bool createDefault = false);
   
   //----- Non-Configurable Items
   // StopCondition
   StopCondition* CreateStopCondition(const std::string &type,
                                      const std::string &name);
   
   // AxisSystem
   AxisSystem* CreateAxisSystem(const std::string &type,
                                const std::string &name,
                                Integer manage = 1);
   
   // MathNode
   MathNode* CreateMathNode(const std::string &type,
                            const std::string &name = "");
   
   // Attitude
   Attitude* CreateAttitude(const std::string &type,
                            const std::string &name);
   
   // GmatCommand
   GmatCommand* InterpretGmatFunction(const std::string &fileName);
   GmatCommand* InterpretGmatFunction(Function *funct,
                                      ObjectMap *objMap = NULL,
                                      SolarSystem *ss = NULL);
   GmatCommand* CreateCommand(const std::string &type,
                              const std::string &name, bool &retFlag);
   GmatCommand* CreateDefaultCommand(const std::string &type,
                                     const std::string &name = "",
                                     GmatCommand *refCmd = NULL);
   GmatCommand* AppendCommand(const std::string &type,
                              const std::string &name, bool &retFlag,
                              Integer sandboxNum = 1);
   GmatCommand* DeleteCommand(GmatCommand *cmd, Integer sandboxNum = 1);
   GmatCommand* GetFirstCommand(Integer sanboxNum = 1);
   bool AppendCommand(GmatCommand *cmd, Integer sandboxNum = 1);
   bool InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd,
                      Integer sandboxNum = 1);
   void SetCommandsUnchanged(Integer whichList = 0); 
   void ValidateCommand(GmatCommand *cmd);
   
   // CoordinateSystem
   CoordinateSystem* GetInternalCoordinateSystem();
   
   // Planetary files
   const StringArray& GetPlanetarySourceTypes();
   const StringArray& GetPlanetarySourceNames();
   const StringArray& GetPlanetarySourceTypesInUse();
//   const StringArray& GetAnalyticModelNames();
//   bool SetAnalyticModelToUse(const std::string &modelName);
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
   bool LoadMinimumResource();
   
   // Mission sequence
   bool ClearCommandSeq(bool leaveFirstCmd = true, bool callRunComplete = true,
                        Integer sandboxNum = 1);
   
   // Sandbox
   void ClearAllSandboxes();
   GmatBase* GetInternalObject(const std::string &name, Integer sandboxNum = 1);
   Integer RunMission(Integer sandboxNum = 1);
   Integer ChangeRunState(const std::string &state, Integer sandboxNum = 1);
   Gmat::RunState GetUserInterrupt();
   Gmat::RunState GetRunState();
   
   // Script
   bool InterpretScript(const std::string &filename, bool readBack = false,
                        const std::string &newPath = "");
   bool InterpretScript(std::istringstream *ss, bool clearObjs);
   bool SaveScript(const std::string &filename,
                   Gmat::WriteMode mode = Gmat::SCRIPTING);
   std::string GetScript(Gmat::WriteMode mode = Gmat::SCRIPTING);
   Integer RunScript(Integer sandboxNum = 1);
   
   // MATLAB Server Startup Interface
   bool StartMatlabServer();
   
   // Plugin GUI data
   std::vector<Gmat::PluginResource*> *GetPluginResourceList();

   bool IsSequenceStarter(const std::string &commandType);
   const std::string& GetStarterStringList();

private:
   
   // Initialization
   void CreatePlanetaryCoeffFile();
   void CreateTimeFile();
   
   // Preparing next script reading
   void PrepareNextScriptReading(bool clearObjs = true);
   
   // Minimum resource
   void CreateMinimumResource();
   
   // Creating default objects
   void CreateSolarSystemInUse();
   void CreateInternalCoordSystem();
   void CreateDefaultCoordSystems();
   void CreateDefaultBarycenter();
   void CreateDefaultParameters();
   void CreateDefaultMission();
   
   // Parameter reference object setting
   void CheckParameterType(Parameter **param, const std::string &type,
                           const std::string &ownerName);
   
   // Object map
   GmatBase* FindObject(const std::string &name);
   bool AddObject(GmatBase *obj);
   void SetSolarSystemAndObjectMap(SolarSystem *ss, ObjectMap *objMap,
                                   bool forFunction,
                                   const std::string &callFrom = "");

   // Handlers for the commands that can start a mission sequence
   StringArray sequenceStarters;
   std::string starterList;
   const StringArray& GetSequenceStarters();
   
   // Default objects
   Spacecraft* GetDefaultSpacecraft();
   PropSetup*  GetDefaultPropSetup();
   Burn*       GetDefaultBurn(const std::string &type);
   Hardware*   GetDefaultHardware(const std::string &type);
   Solver*     GetDefaultBoundaryValueSolver();
   Solver*     GetDefaultOptimizer();
   Subscriber* GetDefaultSubscriber(const std::string &type,
                                    bool addObjects = true,
                                    bool createIfNoneFound = true);
   Parameter*  GetDefaultX();
   Parameter*  GetDefaultY();
   StopCondition* CreateDefaultStopCondition();
   
   // Sandbox
   void AddSolarSystemToSandbox(Integer index);
   void AddTriggerManagersToSandbox(Integer index);
   void AddInternalCoordSystemToSandbox(Integer index);
   void AddPublisherToSandbox(Integer index);
   void AddSubscriberToSandbox(Integer index);
   void AddOtherObjectsToSandbox(Integer index);   
   void AddCommandToSandbox(Integer index);
   void InitializeSandbox(Integer index);
   void ExecuteSandbox(Integer index);
   
   // For Debug
   void ShowCommand(const std::string &title1, GmatCommand *cmd1,
                    const std::string &title2 = "", GmatCommand *cmd2 = NULL);
   void ShowObjectMap(const std::string &title, ObjectMap *objMap = NULL);
   
   Moderator();
   virtual ~Moderator();
   
   // member data
   bool isSlpAlreadyInUse;
   bool isRunReady;
   bool isFromGui;
   bool endOfInterpreter;
   bool showFinalState;
   bool loadSandboxAndPause;
   Integer objectManageOption;
   std::vector<Sandbox*> sandboxes;
   std::vector<TriggerManager*> triggerManagers;
   std::vector<GmatCommand*> commands;
   
   ObjectMap *objectMapInUse;
   Function *currentFunction;
   ObjectArray unmanagedFunctions;
   
   static Moderator *instance;
   static ScriptInterpreter *theUiInterpreter;
   static ScriptInterpreter *theScriptInterpreter;
   ConfigManager *theConfigManager;
   FactoryManager *theFactoryManager;
   FileManager *theFileManager;
   Publisher *thePublisher;
   
   SolarSystem *theDefaultSolarSystem;
   SolarSystem *theSolarSystemInUse;
   SolarSystem *theInternalSolarSystem;
   CoordinateSystem *theInternalCoordSystem;
   StringArray defaultCoordSystemNames;
   StringArray tempObjectNames;
   EopFile *theEopFile;
   ItrfCoefficientsFile *theItrfFile;
   LeapSecsFileReader *theLeapSecsFile;
   Interface *theMatlabInterface;
   Gmat::RunState runState;

   // Dynamic library data table
   std::map<std::string, DynamicLibrary*>   userLibraries;
   std::vector<Gmat::PluginResource*>  userResources;
};

#endif // Moderator_hpp

