//$Header$
//------------------------------------------------------------------------------
//                              GuiInterpreter
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
 * Declares the operations between GUI subsystem and the Moderator.
 */
//------------------------------------------------------------------------------
#ifndef GuiInterpreter_hpp
#define GuiInterpreter_hpp

#include "Interpreter.hpp"
#include "gmatdefs.hpp"
#include "PhysicalModel.hpp"
#include "ForceModel.hpp"
#include "Propagator.hpp"
#include "Spacecraft.hpp"
#include "Hardware.hpp"
#include "Formation.hpp"
#include "Parameter.hpp"
#include "StopCondition.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "PropSetup.hpp"
#include "Command.hpp"
#include "Subscriber.hpp"
#include "Burn.hpp"
#include "Solver.hpp"
#include "AtmosphereModel.hpp"
#include "Function.hpp"


class GMAT_API GuiInterpreter : public Interpreter
{
public:

   static GuiInterpreter* Instance();
   ~GuiInterpreter();

   virtual bool Interpret(void);
   virtual bool Build(void);
   virtual bool Interpret(GmatBase *obj, const std::string generator);

   bool IsInitialized();
   void Initialize();

   //----- factory
   StringArray GetListOfFactoryItems(Gmat::ObjectType type);
   GmatBase* GetConfiguredItem(const std::string &name);

   //----- config
   StringArray& GetListOfConfiguredItems(Gmat::ObjectType type);
   bool RenameConfiguredItem(Gmat::ObjectType type, const std::string &oldName,
                             const std::string &newName);
   bool RemoveConfiguredItem(Gmat::ObjectType type, const std::string &name);

   // Spacecraft
   Spacecraft* CreateSpacecraft(const std::string&type,
                                const std::string &name);
   Spacecraft* GetSpacecraft(const std::string &name);
   Formation* GuiInterpreter::CreateFormation(const std::string &type,
                                           const std::string &name);
   Formation* GuiInterpreter::GetFormation(const std::string &name);
   
   // Hardware (loj: 1/7/05 Added)
   Hardware* CreateHardware(const std::string &type,
                            const std::string &name);
   Hardware* GetHardware(const std::string &name);

   // Propagator
   Propagator* CreatePropagator(const std::string &type,
                                const std::string &name);
   Propagator* GetPropagator(const std::string &name);

   // PropSetup
   PropSetup* CreateDefaultPropSetup(const std::string &name);
   PropSetup* GetPropSetup(const std::string &name);

   // ForceModel/PhysicalModel
   ForceModel* CreateForceModel(const std::string &name);
   PhysicalModel* CreatePhysicalModel(const std::string &type,
                                      const std::string &name);
   PhysicalModel* GetPhysicalModel(const std::string &name);

   // AtmosphereModel
   AtmosphereModel* CreateAtmosphereModel(const std::string &type,
                                          const std::string &name,
                                          const std::string &body = "Earth");
   AtmosphereModel* GetAtmosphereModel(const std::string &name);

   // Burn
   Burn* CreateBurn(const std::string &type, const std::string &name);
   Burn* GetBurn(const std::string &name);

   // Solver
   Solver* CreateSolver(const std::string &type, const std::string &name);
   Solver* GetSolver(const std::string &name);

   // Parameter
   Parameter* CreateParameter(const std::string &type, const std::string &name);
   Parameter* GetParameter(const std::string &name);

   // Celestial body
   CelestialBody* CreateCelestialBody(const std::string &type,
                                      const std::string &name);
   CelestialBody* GetCelestialBody(const std::string &name);

   // SolarSystem
   SolarSystem* GetDefaultSolarSystem();

   // CoordinateSystem (loj: 1/31/05 Added)
   CoordinateSystem* CreateCoordinateSystem(const std::string &name);
   CoordinateSystem* GetCoordinateSystem(const std::string &name);
   CoordinateSystem* GetInternalCoordinateSystem();
   
   // Planetary files
   StringArray& GetPlanetaryFileTypes();
   StringArray& GetPlanetaryFileNames();
   StringArray& GetPlanetaryFileTypesInUse();
   std::string GetPlanetaryFileName(const std::string &filetype);
   bool SetPlanetaryFileName(const std::string &filetype,
                             const std::string &filename);
   Integer SetPlanetaryFileTypesInUse(const StringArray &filetypes);

   // Potential field files
   std::string GetPotentialFileName(const std::string &filetype);

   
   // Subscriber
   Subscriber* CreateSubscriber(const std::string &type,
                                const std::string &name,
                                const std::string &filename = "",
                                bool createDefault = true);
   Subscriber* GetSubscriber(const std::string &name);

   // StopCondition
   StopCondition* CreateStopCondition(const std::string &type,
                                      const std::string &name);
   StopCondition* GetStopCondition(const std::string &name);
    
   // Function
   Function* CreateFunction(const std::string &type,
                            const std::string &name);
   Function* GetFunction(const std::string &name);
   
   // GmatCommand
   GmatCommand* CreateCommand(const std::string &type,
                              const std::string &name = "");
   GmatCommand* CreateDefaultCommand(const std::string &type,
                                     const std::string &name = "");

   // Resource
   bool ClearResource();
   
   // Command sequence
   bool LoadDefaultMission();
   bool ClearCommandSeq(Integer sandboxNum = 1);
   bool AppendCommand(GmatCommand *cmd, Integer sandboxNum = 1);
   GmatCommand* AppendCommand(const std::string &type, const std::string &name,
                              Integer sandboxNum = 1);
   bool InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd,
                      Integer sandboxNum = 1);
   GmatCommand* InsertCommand(const std::string &type, const std::string &currName,
                              const std::string &prevName, Integer sandboxNum = 1);
   GmatCommand* DeleteCommand(GmatCommand *cmd, Integer sandboxNum = 1);
   GmatCommand* GetNextCommand(Integer sandboxNum = 1);

   // Sandbox
   void ClearAllSandboxes();
   Integer RunMission(Integer sandboxNum = 1);
   Integer ChangeRunState(const std::string &state, Integer sandboxNum = 1);

   // Script
   bool InterpretScript(const std::string &scriptFilename);
   bool SaveScript(const std::string &scriptFilename);
   Integer RunScript(Integer sandboxNum = 1);

   // GUI control
   void SetInputFocus();
   void NotifyRunCompleted();
   void UpdateResourceTree();
   void UpdateMissionTree();
   void CloseCurrentProject();
   
private:

   GuiInterpreter();
   GuiInterpreter(const GuiInterpreter&);
   GuiInterpreter& operator=(const GuiInterpreter &guiInterpreter);

   // member data
   bool isInitialized;

   static GuiInterpreter *instance;
};


#endif // GuiInterpreter_hpp

