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
 * Implements the operations between GUI subsystem and the Moderator.
 */
//------------------------------------------------------------------------------
#if !defined __CONSOLE_APP__
#include "GmatAppData.hpp"
#endif

#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "Moderator.hpp"

#define DEBUG_INTERPRET 1

GuiInterpreter* GuiInterpreter::instance = NULL;

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// static Instance()
//------------------------------------------------------------------------------
GuiInterpreter* GuiInterpreter::Instance()
{
   if (instance == NULL)
      instance = new GuiInterpreter();
   return instance;
}


//------------------------------------------------------------------------------
// ~GuiInterpreter()
//------------------------------------------------------------------------------
GuiInterpreter::~GuiInterpreter()
{
}


// //------------------------------------------------------------------------------
// // bool GuiInterpreter::Interpret()
// //------------------------------------------------------------------------------
// bool GuiInterpreter::Interpret()
// {
//    return false;
// }


// //------------------------------------------------------------------------------
// // bool GuiInterpreter::Build(Gmat::WriteMode mode)
// //------------------------------------------------------------------------------
// bool GuiInterpreter::Build(Gmat::WriteMode mode)
// {
//    return false;
// }


//------------------------------------------------------------------------------
// bool Interpret(GmatBase *obj, const std::string generator)
//------------------------------------------------------------------------------
/**
 * Reads the generatingString for an object and builds the corresponding data.
 *
 * This method is used to rebuild pieces of a mission sequence when a user makes
 * changes on a ScriptEvent panel, and to build the commands that are encoded
 * in a BeginScript/EndScript block.
 *
 * @param <obj>         The object that is being reinterpreted.
 * @param <generator>   The string that gets interpreted.
 *
 * @return true on success, false on failure.
 *
 * @note Interpret only works for GmatCommands at this time.
 */
//------------------------------------------------------------------------------
bool GuiInterpreter::Interpret(GmatBase *obj, const std::string generator)
{
   if (obj->GetType() != Gmat::COMMAND)
      throw InterpreterException(
         "Interpret(GmatBase*) currently only supports GmatCommands.");

   #ifdef DEBUG_INTERPRET
      MessageInterface::ShowMessage(
         "%s\n%s\n\"%s\"\n",
         "\nInterpret(GmatBase*)is under construction.  Please be patient!",
         "String that is interpreted:", obj->GetGeneratingString().c_str());
   #endif
   
   if (obj->GetTypeName() == "BeginScript")
   {
      #ifdef DEBUG_INTERPRET
      MessageInterface::ShowMessage
         ("Parsing in-line text:\n%s\n", generator.c_str());
      #endif
      
      //return InterpretTextBlock((GmatCommand*)obj, generator);
      //loj: need work
      return false;
   }
   else
   {
      #ifdef DEBUG_INTERPRET
      MessageInterface::ShowMessage
         ("Resetting command using\n%s\n", generator.c_str());
      #endif
      
      //AssembleCommand(generator, (GmatCommand*)obj);
      //loj: need work
      return false;
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool Interpret(GmatCommand *inCmd, std::istringstream *ss)
//------------------------------------------------------------------------------
bool GuiInterpreter::Interpret(GmatCommand *inCmd, std::istringstream *ss)
{   
   SetInStream(ss);
   return ScriptInterpreter::Interpret(inCmd);
}


//------------------------------------------------------------------------------
// void Finalize()
//------------------------------------------------------------------------------
void GuiInterpreter::Finalize()
{
   theModerator->Finalize();
}


//------------------------------------------------------------------------------
// StringArray GetListOfFactoryItems(Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Returns names of all creatable items of object type.
 *
 * @param <type> object type
 *
 * @return array of item names; return empty array if none
 */
//------------------------------------------------------------------------------
StringArray GuiInterpreter::GetListOfFactoryItems(Gmat::ObjectType type)
{
   return theModerator->GetListOfFactoryItems(type);
}


//------------------------------------------------------------------------------
// bool RenameObject(Gmat::ObjectType type, const std::string &oldName
//                           const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames item from the configured list.
 *
 * @param <type> object type
 * @param <oldName>  old object name
 * @param <newName>  new object name
 *
 * @return true if the item has been removed; false otherwise
 */
//------------------------------------------------------------------------------
bool GuiInterpreter::RenameObject(Gmat::ObjectType type,
                                          const std::string &oldName,
                                          const std::string &newName)
{
   return theModerator->RenameObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// bool RemoveObject(Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Removes item from the configured list.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if the item has been removed; false otherwise
 */
//------------------------------------------------------------------------------
bool GuiInterpreter::RemoveObject(Gmat::ObjectType type,
                                          const std::string &name)
{
   return theModerator->RemoveObject(type, name, false);
}


//------------------------------------------------------------------------------
// bool RemoveItemIfNotUsed(Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Removes item from the configured list if it is not used in the mission
 * sequence.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if the item has been removed; false otherwise
 */
//------------------------------------------------------------------------------
bool GuiInterpreter::RemoveObjectIfNotUsed(Gmat::ObjectType type,
                                          const std::string &name)
{
   return theModerator->RemoveObject(type, name, true);
}


//------------------------------------------------------------------------------
// bool HasConfigurationChanged(Integer sandboxNum = 1)
//------------------------------------------------------------------------------
bool GuiInterpreter::HasConfigurationChanged(Integer sandboxNum)
{
   return theModerator->HasConfigurationChanged(sandboxNum);
}


//------------------------------------------------------------------------------
// void ConfigurationChanged(GmatBase *obj, bool tf)
//------------------------------------------------------------------------------
void GuiInterpreter::ConfigurationChanged(GmatBase *obj, bool tf)
{
   theModerator->ConfigurationChanged(obj, tf);
}


//------------------------------------------------------------------------------
// void ResetConfigurationChanged(bool resetResource = true,
//                                bool resetCommands = true,
//                                Integer sandboxNum = 1)
//------------------------------------------------------------------------------
void GuiInterpreter::ResetConfigurationChanged(bool resetResource,
                                               bool resetCommands,
                                               Integer sandboxNum)
{
   theModerator->ResetConfigurationChanged(resetResource, resetCommands, sandboxNum);
}


//------------------------------------------------------------------------------
// SolarSystem* GetDefaultSolarSystem()
//------------------------------------------------------------------------------
/**
 * Retrieves a default solar system object pointer.
 *
 * @return a default solar system object pointer
 */
//------------------------------------------------------------------------------
SolarSystem* GuiInterpreter::GetDefaultSolarSystem()
{
   return theModerator->GetDefaultSolarSystem();
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
Parameter* GuiInterpreter::GetParameter(const std::string &name)
{
   return theModerator->GetParameter(name);
}


//------------------------------------------------------------------------------
// Parameter* CreateParameter(const std::string &type, const std::string &name,
//                            const std::string &ownerName, const std::string &depName)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to create a Parameter.
 * 
 * @param  type       Type of parameter requested
 * @param  name       Name for the parameter.
 * @param  ownerName  object name of parameter requested
 * @param  depName    Dependent object name of parameter requested
 * 
 * @return Pointer to the constructed Parameter.
 */
//------------------------------------------------------------------------------
Parameter* GuiInterpreter::CreateParameter(const std::string &type, 
                                           const std::string &name,
                                           const std::string &ownerName,
                                           const std::string &depName)
{
   #if DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("NewInterptr::CreateParameter() type=%s, name=%s, ownerName=%s, depName=%s\n",
       type.c_str(), name.c_str(), ownerName.c_str(), depName.c_str());
   #endif
   
   return theModerator->CreateParameter(type, name, ownerName, depName);
}


//------------------------------------------------------------------------------
// Subscriber* CreateSubscriber(const std::string &type,
//                              const const std::string &name,
//                              const std::string &filename = "",
//                              bool createDefault = true)
//------------------------------------------------------------------------------
/**
 * Creates a subscriber object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 * @param <filename> file name if used
 *
 * @return a subscriber object pointer
 */
//------------------------------------------------------------------------------
Subscriber* GuiInterpreter::CreateSubscriber(const std::string &type,
                                             const std::string &name,
                                             const std::string &filename,
                                             bool createDefault)
{
   return theModerator->
      CreateSubscriber(type, name, filename, createDefault);
}


//------------------------------------------------------------------------------
// GmatBase* CreateDefaultPropSetup(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a default PropSetup object.
 *
 * @param <name> PropSetup name
 *
 * @return a PropSetup object pointer
 */
//------------------------------------------------------------------------------
GmatBase* GuiInterpreter::CreateDefaultPropSetup(const std::string &name)
{
   return (GmatBase*)theModerator->CreateDefaultPropSetup(name);
}


//------------------------------------------------------------------------------
// CoordinateSystem* GetInternalCoordinateSystem()
//------------------------------------------------------------------------------
/**
 * @return a internal coordinate system pointer
 */
//------------------------------------------------------------------------------
CoordinateSystem* GuiInterpreter::GetInternalCoordinateSystem()
{
   return theModerator->GetInternalCoordinateSystem();
}


//------------------------------------------------------------------------------
// bool IsDefaultCoordinateSystem(const std::string &name)
//------------------------------------------------------------------------------
bool GuiInterpreter::IsDefaultCoordinateSystem(const std::string &name)
{
   if (name == "EarthMJ2000Eq" || name == "EarthMJ2000Ec" ||
       name == "EarthFixed")
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// StringArray& GetPlanetarySourceTypes()
//------------------------------------------------------------------------------
/**
 * @return a planetary source types
 */
//------------------------------------------------------------------------------
StringArray& GuiInterpreter::GetPlanetarySourceTypes()
{
   return theModerator->GetPlanetarySourceTypes();
}


//------------------------------------------------------------------------------
// StringArray& GetPlanetarySourceNames()
//------------------------------------------------------------------------------
/**
 * @return a planetary source file names
 */
//------------------------------------------------------------------------------
StringArray& GuiInterpreter::GetPlanetarySourceNames()
{
   return theModerator->GetPlanetarySourceNames();
}


//------------------------------------------------------------------------------
// StringArray& GetPlanetarySourceTypesInUse()
//------------------------------------------------------------------------------
StringArray& GuiInterpreter::GetPlanetarySourceTypesInUse()
{
   return theModerator->GetPlanetarySourceTypesInUse();
}


//------------------------------------------------------------------------------
// StringArray& GetAnalyticModelNames()
//------------------------------------------------------------------------------
StringArray& GuiInterpreter::GetAnalyticModelNames()
{
   return theModerator->GetAnalyticModelNames();
}


//------------------------------------------------------------------------------
// bool SetAnalyticModelToUse(const std::string &modelName)
//------------------------------------------------------------------------------
bool GuiInterpreter::SetAnalyticModelToUse(const std::string &modelName)
{
   return theModerator->SetAnalyticModelToUse(modelName);
}


//------------------------------------------------------------------------------
// bool SetPlanetarySourceName(const std::string &sourceType,
//                           const std::string &filename)
//------------------------------------------------------------------------------
bool GuiInterpreter::SetPlanetarySourceName(const std::string &sourceType,
                                          const std::string &filename)
{
   return theModerator->SetPlanetarySourceName(sourceType, filename);
}


//------------------------------------------------------------------------------
// Integer SetPlanetarySourceTypesInUse(const StringArray &sourceTypes)
//------------------------------------------------------------------------------
/*
 * @param <sourceTypes> list of file type in the priority order of use
 */
//------------------------------------------------------------------------------
Integer GuiInterpreter::SetPlanetarySourceTypesInUse(const StringArray &sourceTypes)
{
   return theModerator->SetPlanetarySourceTypesInUse(sourceTypes);
}


//------------------------------------------------------------------------------
// std::string GetPlanetarySourceName(const std::string &sourceType)
//------------------------------------------------------------------------------
std::string GuiInterpreter::GetPlanetarySourceName(const std::string &sourceType)
{
   return theModerator->GetPlanetarySourceName(sourceType);
}


//------------------------------------------------------------------------------
// std::string GetPotentialFileName(const std::string &fileType)
//------------------------------------------------------------------------------
std::string GuiInterpreter::GetPotentialFileName(const std::string &fileType)
{
   return theModerator->GetPotentialFileName(fileType);
}


//------------------------------------------------------------------------------
// std::string GetFileName(const std::string &fileType)
//------------------------------------------------------------------------------
std::string GuiInterpreter::GetFileName(const std::string &fileType)
{
   return theModerator->GetFileName(fileType);
}


//------------------------------------------------------------------------------
// GmatBase* CreateStopCondition(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
GmatBase* GuiInterpreter::CreateStopCondition(const std::string &type,
                                              const std::string &name)
{
   return (GmatBase*)theModerator->CreateStopCondition(type, name);
}


//------------------------------------------------------------------------------
// GmatCommand* CreateDefaultCommand(const std::string &type,
//                                   const std::string &name = "",
//                                   const GmatCommand *refCmd = NULL)
//------------------------------------------------------------------------------
/**
 * Creates a default command object by given type and name.
 *
 * @param <type> command type
 * @param <name> command name
 *
 * @return a command object pointer
 */
//------------------------------------------------------------------------------
GmatCommand* GuiInterpreter::CreateDefaultCommand(const std::string &type,
                                                  const std::string &name,
                                                  GmatCommand *refCmd)
{
   return theModerator->CreateDefaultCommand(type, name, refCmd);
}


//------------------------------------------------------------------------------
// GmatCommand* AppendCommand(const std::string &type, const std::string &name,
//                        Integer sandboxNum)
//------------------------------------------------------------------------------
GmatCommand* GuiInterpreter::AppendCommand(const std::string &type,
                                           const std::string &name, bool &retFlag,
                                           Integer sandboxNum)
{
   return theModerator->AppendCommand(type, name, retFlag, sandboxNum);
}


//------------------------------------------------------------------------------
// GmatCommand* DeleteCommand(GmatCommand *cmd, Integer sandboxNum)
//------------------------------------------------------------------------------
GmatCommand* GuiInterpreter::DeleteCommand(GmatCommand *cmd, Integer snadboxNum)
{
   return theModerator->DeleteCommand(cmd, snadboxNum);
}


//------------------------------------------------------------------------------
// GmatCommand* GetFirstCommand(Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Retrieves a first command in the sequence.
 *
 * @param <sandboxNum> sandbox number
 *
 * @return a next command object pointer, return null if no command found
 */
//------------------------------------------------------------------------------
GmatCommand* GuiInterpreter::GetFirstCommand(Integer sandboxNum)
{
   return theModerator->GetFirstCommand(sandboxNum);
}


//------------------------------------------------------------------------------
// bool AppendCommand(GmatCommand *cmd, Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Appends a command.
 *
 * @param <cmd> command to append
 *
 * @return a command object pointer, return null if not appended
 */
//------------------------------------------------------------------------------
bool GuiInterpreter::AppendCommand(GmatCommand *cmd, Integer sandboxNum)
{
   return theModerator->AppendCommand(cmd, sandboxNum);
}


//------------------------------------------------------------------------------
// bool InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd, Integer sandboxNum)
//------------------------------------------------------------------------------
bool GuiInterpreter::InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd,
                                   Integer sandboxNum)
{
   return theModerator->InsertCommand(cmd, prevCmd, sandboxNum);
}


//------------------------------------------------------------------------------
// bool ClearResource()
//------------------------------------------------------------------------------
bool GuiInterpreter::ClearResource()
{
   return theModerator->ClearResource();
}


//------------------------------------------------------------------------------
// bool LoadDefaultMission()
//------------------------------------------------------------------------------
bool GuiInterpreter::LoadDefaultMission()
{
   return theModerator->LoadDefaultMission();
}


//------------------------------------------------------------------------------
// bool ClearCommandSeq(Integer sandboxNum = 1)
//------------------------------------------------------------------------------
bool GuiInterpreter::ClearCommandSeq(Integer sandboxNum)
{
   return theModerator->ClearCommandSeq(sandboxNum);
}


//------------------------------------------------------------------------------
// Integer RunMission(Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Calls Moderator to run mission sequence.
 *
 * @param <snadobxNum> sandbox number
 *
 * @return a status code
 *    0 = successful, <0 = error (tbd)
 */
//------------------------------------------------------------------------------
Integer GuiInterpreter::RunMission(Integer sandboxNum)
{
   return theModerator->RunMission(sandboxNum);
}


//------------------------------------------------------------------------------
// Integer ChangeRunState(const std::string &state, Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Calls Moderator to change run state.
 *
 * @param <state> run state string ("Stop", "Pause", "Resume")
 * @param <snadobxNum> sandbox number
 *
 * @return a status code
 *    0 = successful, <0 = error (tbd)
 */
//------------------------------------------------------------------------------
Integer GuiInterpreter::ChangeRunState(const std::string &state, Integer sandboxNum)
{
   return theModerator->ChangeRunState(state, sandboxNum);
}


//------------------------------------------------------------------------------
// bool InterpretScript(const std::string &scriptFilename)
//------------------------------------------------------------------------------
/**
 * Creates objects from script file.
 *
 * @param <scriptFilename> input script file name
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool GuiInterpreter::InterpretScript(const std::string &scriptFilename)
{
   return theModerator->InterpretScript(scriptFilename);
}


//------------------------------------------------------------------------------
// bool SaveScript(const std::string &scriptFilename, Gmat::WriteMode mode)
//------------------------------------------------------------------------------
/**
 * Builds scripts from objects and write to a file.
 *
 * @param <scriptFilename> output script file name
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool GuiInterpreter::SaveScript(const std::string &scriptFilename,
                                Gmat::WriteMode mode)
{
   return theModerator->SaveScript(scriptFilename, mode);
}


//------------------------------------------------------------------------------
// std::string SaveScript(Gmat::WriteMode mode)
//------------------------------------------------------------------------------
/**
 * Returns Built scripts from objects
 *
 * @return built scripts from objects
 */
//------------------------------------------------------------------------------
std::string GuiInterpreter::GetScript(Gmat::WriteMode mode)
{
   return theModerator->GetScript(mode);
}


//------------------------------------------------------------------------------
// Integer RunScript(Integer sandboxNum)
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
Integer GuiInterpreter::RunScript(Integer sandboxNum)
{
   return theModerator->RunScript(sandboxNum);
}


//------------------------------------------------------------------------------
// void SetInputFocus()
//------------------------------------------------------------------------------
void GuiInterpreter::SetInputFocus()
{
#if !defined __CONSOLE_APP__
   GmatMainFrame *mainFrame = GmatAppData::GetMainFrame();
   mainFrame->SetFocus();
   if (mainFrame->IsIconized())
      mainFrame->ProcessPendingEvent();
#endif
}


//------------------------------------------------------------------------------
// void NotifyRunCompleted()
//------------------------------------------------------------------------------
void GuiInterpreter::NotifyRunCompleted()
{
#if !defined __CONSOLE_APP__
   GmatAppData::GetMainFrame()->NotifyRunCompleted();
#endif
}


//------------------------------------------------------------------------------
// void UpdateResourceTree()
//------------------------------------------------------------------------------
void GuiInterpreter::UpdateResourceTree()
{
#if !defined __CONSOLE_APP__
   //close the open windows first
   GmatAppData::GetMainFrame()->CloseAllChildren();
   GmatAppData::GetResourceTree()->UpdateResource(true);
#endif
}


//------------------------------------------------------------------------------
// void UpdateMissionTree()
//------------------------------------------------------------------------------
void GuiInterpreter::UpdateMissionTree()
{
#if !defined __CONSOLE_APP__
   GmatAppData::GetMissionTree()->UpdateMission(true);
#endif
}


//------------------------------------------------------------------------------
// void CloseCurrentProject()
//------------------------------------------------------------------------------
void GuiInterpreter::CloseCurrentProject()
{
#if !defined __CONSOLE_APP__
   GmatAppData::GetMainFrame()->CloseCurrentProject();
#endif
}


//---------------------------------
// private
//---------------------------------

//------------------------------------------------------------------------------
// GuiInterpreter()
//------------------------------------------------------------------------------
GuiInterpreter::GuiInterpreter()
   : ScriptInterpreter()
{
   Initialize();
   isInitialized = false;
}


//------------------------------------------------------------------------------
// GuiInterpreter(const GuiInterpreter&)
//------------------------------------------------------------------------------
GuiInterpreter::GuiInterpreter(const GuiInterpreter&)
{
}


//------------------------------------------------------------------------------
// GuiInterpreter& operator=(const GuiInterpreter &guiInterpreter)
//------------------------------------------------------------------------------
GuiInterpreter& GuiInterpreter::operator=(const GuiInterpreter &guiInterpreter)
{
   return *this;
}
