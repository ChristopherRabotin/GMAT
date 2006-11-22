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

#include "ScriptInterpreter.hpp"
#include "gmatdefs.hpp"
#include <sstream>      // for std::istringstream

class GMAT_API GuiInterpreter : public ScriptInterpreter
{
public:

   static GuiInterpreter* Instance();
   ~GuiInterpreter();

   // Interpreter abstract methods
   // virtual bool Interpret();
   virtual bool Interpret(GmatBase *obj, const std::string generator);
   virtual bool Interpret(GmatBase *obj, std::istringstream *ss);
   // virtual bool Build(Gmat::WriteMode mode);
   
   void Finalize();
   
   //----- factory
   StringArray GetListOfFactoryItems(Gmat::ObjectType type);
   
   //----- configuration
   bool RenameObject(Gmat::ObjectType type, const std::string &oldName,
                     const std::string &newName);
   bool RemoveObject(Gmat::ObjectType type, const std::string &name);
   bool RemoveObjectIfNotUsed(Gmat::ObjectType type, const std::string &name);
   bool HasConfigurationChanged(Integer sandboxNum = 1);
   void ConfigurationChanged(GmatBase *obj, bool tf);
   void ResetConfigurationChanged(bool resetResource = true,
                                  bool resetCommands = true,
                                  Integer sandboxNum = 1);
   
   // SolarSystem
   SolarSystem* GetDefaultSolarSystem();
   CoordinateSystem* GetInternalCoordinateSystem();
   bool IsDefaultCoordinateSystem(const std::string &name);
   
   Parameter* GetParameter(const std::string &name);
   Parameter* CreateParameter(const std::string &type,
                              const std::string &name,
                              const std::string &ownerName = "",
                              const std::string &depName = "");
   
   // Subscriber
   Subscriber* CreateSubscriber(const std::string &type,
                                const std::string &name,
                                const std::string &filename = "",
                                bool createDefault = true);
   
   GmatBase* CreateDefaultPropSetup(const std::string &name);
   
   // Planetary source
   StringArray& GetPlanetarySourceTypes();
   StringArray& GetPlanetarySourceNames();
   StringArray& GetPlanetarySourceTypesInUse();
   StringArray& GetAnalyticModelNames();
   bool SetAnalyticModelToUse(const std::string &modelName);
   bool SetPlanetarySourceName(const std::string &sourceType,
                               const std::string &filename);
   Integer SetPlanetarySourceTypesInUse(const StringArray &sourceTypes);
   std::string GetPlanetarySourceName(const std::string &sourceType);
   
   // Potential field files
   std::string GetPotentialFileName(const std::string &fileType);
   
   // Getting file names
   std::string GetFileName(const std::string &fileType);
   
   // StopCondition
   GmatBase* CreateStopCondition(const std::string &type,
                                 const std::string &name);
   
   // Command
   GmatCommand* CreateDefaultCommand(const std::string &type,
                                     const std::string &name = "",
                                     GmatCommand *refCmd = NULL);
   
   // Resource
   bool ClearResource();
   
   // Command sequence
   bool LoadDefaultMission();
   bool ClearCommandSeq(Integer sandboxNum = 1);
   bool AppendCommand(GmatCommand *cmd, Integer sandboxNum = 1);
   bool InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd,
                      Integer sandboxNum = 1);
   GmatCommand* AppendCommand(const std::string &type, const std::string &name,
                              Integer sandboxNum = 1);
   GmatCommand* DeleteCommand(GmatCommand *cmd, Integer sandboxNum = 1);
   GmatCommand* GetNextCommand(Integer sandboxNum = 1);
   
   // Sandbox
   void ClearAllSandboxes();
   Integer RunMission(Integer sandboxNum = 1);
   Integer ChangeRunState(const std::string &state, Integer sandboxNum = 1);

   // Script
   bool InterpretScript(const std::string &scriptFilename);
   bool SaveScript(const std::string &scriptFilename,
                   Gmat::WriteMode mode = Gmat::SCRIPTING);
   std::string GetScript(Gmat::WriteMode mode = Gmat::SCRIPTING); //loj: 6/12/06 added
   
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

