//$Id$
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

   // Interpreter abstract methods
   virtual bool Interpret(GmatCommand *inCmd, std::istringstream *ss);
   
   void Finalize();
   
   //----- running object
   GmatBase* GetRunningObject(const std::string &name);
   
   //----- factory
   const StringArray& GetListOfFactoryItems(Gmat::ObjectType type);
   
   //----- configuration
   std::string GetNewName(const std::string &name, Integer startCount);
   std::string AddClone(const std::string &name);
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
   
   // Parameter
   bool IsParameter(const std::string &type);
   Parameter* GetParameter(const std::string &name);
   virtual Parameter* CreateParameter(const std::string &type,
                                      const std::string &name,
                                      const std::string &ownerName = "",
                                      const std::string &depName = "",
                                      bool manage = true);
   
   // Subscriber
   Subscriber* CreateSubscriber(const std::string &type,
                                const std::string &name,
                                const std::string &filename = "",
                                bool createDefault = true);
   
   GmatBase* CreateDefaultPropSetup(const std::string &name);
   
   // Planetary source
   const StringArray& GetPlanetarySourceTypes();
   const StringArray& GetPlanetarySourceNames();
   const StringArray& GetPlanetarySourceTypesInUse();
   const StringArray& GetAnalyticModelNames();
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
   GmatCommand* AppendCommand(const std::string &type, const std::string &name,
                              bool &retFlag, Integer sandboxNum = 1);
   GmatCommand* DeleteCommand(GmatCommand *cmd, Integer sandboxNum = 1);
   GmatCommand* GetFirstCommand(Integer sandboxNum = 1);
   bool AppendCommand(GmatCommand *cmd, Integer sandboxNum = 1);
   bool InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd,
                      Integer sandboxNum = 1);
   
   // Resource
   bool ClearResource();
   
   // Command sequence
   bool LoadDefaultMission();
   bool ClearCommandSeq(Integer sandboxNum = 1);
   
   // Sandbox
   void ClearAllSandboxes();
   Integer RunMission(Integer sandboxNum = 1);
   Integer ChangeRunState(const std::string &state, Integer sandboxNum = 1);
   
   // Script
   bool InterpretScript(const std::string &filename, bool readBack = false,
                        const std::string &newPath = "");
   bool SaveScript(const std::string &filename,
                   Gmat::WriteMode mode = Gmat::SCRIPTING);
   std::string GetScript(Gmat::WriteMode mode = Gmat::SCRIPTING);
   
   Integer RunScript(Integer sandboxNum = 1);

   // GUI control
   virtual void SetInputFocus();
   virtual void NotifyRunCompleted();
   virtual void UpdateView(Integer type = 7);
   virtual void CloseCurrentProject();
   
   void UpdateResourceTree();
   void UpdateMissionTree();
   void UpdateOutputTree();
   virtual void StartServer();
   
private:

   GuiInterpreter();
   virtual ~GuiInterpreter();
   GuiInterpreter(const GuiInterpreter&);
   GuiInterpreter& operator=(const GuiInterpreter &guiInterpreter);

   // member data
   bool isInitialized;

   static GuiInterpreter *instance;
};


#endif // GuiInterpreter_hpp

