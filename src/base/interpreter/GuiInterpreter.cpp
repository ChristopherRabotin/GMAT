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
   if (instance != NULL)
      delete instance;
}

bool GuiInterpreter::Interpret(void)
{
   return false;
}

bool GuiInterpreter::Build(void)
{
   return false;
}

bool GuiInterpreter::Interpret(GmatBase *obj, const std::string generator)
{
   return Interpreter::Interpret(obj, generator);
}

//------------------------------------------------------------------------------
// bool IsInitialized()
//------------------------------------------------------------------------------
bool GuiInterpreter::IsInitialized()
{
   return isInitialized;
}

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
void GuiInterpreter::Initialize()
{
   //loj: Is there anything to initialize?
   isInitialized = true;
}


//------------------------------------------------------------------------------
// void Finalize()
//------------------------------------------------------------------------------
void GuiInterpreter::Finalize()
{
   moderator->Finalize();
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
   return moderator->GetListOfFactoryItems(type);
}


//------------------------------------------------------------------------------
// StringArray& GetListOfConfiguredItems(Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Returns names of all configured items of object type.
 *
 * @param <type> object type
 *
 * @return array of configured item names; return empty array if none
 */
//------------------------------------------------------------------------------
StringArray& GuiInterpreter::GetListOfConfiguredItems(Gmat::ObjectType type)
{
   return moderator->GetListOfConfiguredItems(type);
}


//------------------------------------------------------------------------------
// GmatBase* GetConfiguredItem(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* GuiInterpreter::GetConfiguredItem(const std::string &name)
{
   return moderator->GetConfiguredItem(name);
}


//------------------------------------------------------------------------------
// bool RenameConfiguredItem(Gmat::ObjectType type, const std::string &oldName
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
bool GuiInterpreter::RenameConfiguredItem(Gmat::ObjectType type,
                                          const std::string &oldName,
                                          const std::string &newName)
{
   return moderator->RenameConfiguredItem(type, oldName, newName);
}


//------------------------------------------------------------------------------
// bool RemoveConfiguredItem(Gmat::ObjectType type, const std::string &name)
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
bool GuiInterpreter::RemoveConfiguredItem(Gmat::ObjectType type,
                                          const std::string &name)
{
   return moderator->RemoveConfiguredItem(type, name);
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
   return moderator->GetDefaultSolarSystem();
}


//loj: 4/22/05 Added
//------------------------------------------------------------------------------
// CalculatedPoint* CreateCalculatedPoint(const std::string &type,
//                                        const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a calculated point object such as libration point, bary center
 * by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a calculated point object pointer
 */
//------------------------------------------------------------------------------
CalculatedPoint* GuiInterpreter::CreateCalculatedPoint(const std::string &type,
                                                       const std::string &name)
{
   return moderator->CreateCalculatedPoint(type, name);
}


//------------------------------------------------------------------------------
// CalculatedPoint* GetCalculatedPoint(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a calculated point object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a calculated object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
CalculatedPoint* GuiInterpreter::GetCalculatedPoint(const std::string &name)
{
   return moderator->GetCalculatedPoint(name);
}


//------------------------------------------------------------------------------
// CelestialBody* CreateCelestialBody(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a celestial body object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a celestial body object pointer
 */
//------------------------------------------------------------------------------
CelestialBody* GuiInterpreter::CreateCelestialBody(const std::string &type,
                                                   const std::string &name)
{
   return moderator->CreateCelestialBody(type, name);
}


//------------------------------------------------------------------------------
// CelestialBody* GetCelestialBody(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a celestial body object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a celestial body object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
CelestialBody* GuiInterpreter::GetCelestialBody(const std::string &name)
{
   //loj: assumes body type from SolarSystem container
   return moderator->GetCelestialBody(name);
}


//------------------------------------------------------------------------------
// Spacecraft* CreateSpacecraft(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a spacecraft object by given name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return spacecraft object pointer
 */
//------------------------------------------------------------------------------
Spacecraft* GuiInterpreter::CreateSpacecraft(const std::string &type,
                                             const std::string &name)
{
   Spacecraft *sc = (Spacecraft*)moderator->CreateSpacecraft(type, name);
   return sc;
}


//------------------------------------------------------------------------------
// Spacecraft* GetSpacecraft(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a spacecraft object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a spacecraft object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Spacecraft* GuiInterpreter::GetSpacecraft(const std::string &name)
{
   Spacecraft *sc = (Spacecraft*)moderator->GetSpacecraft(name);
   return sc;
}


//------------------------------------------------------------------------------
// Formation* CreateFormation(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a formation object by given name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return formation object pointer
 */
//------------------------------------------------------------------------------
Formation* GuiInterpreter::CreateFormation(const std::string &type,
                                           const std::string &name)
{
   Formation *f = (Formation*)moderator->CreateSpacecraft(type, name);
   return f;
}


//------------------------------------------------------------------------------
// Formation* GetFormation(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a formation object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a formation object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Formation* GuiInterpreter::GetFormation(const std::string &name)
{
   Formation *f = (Formation*)moderator->GetSpacecraft(name);
   return f;
}


//------------------------------------------------------------------------------
// Hardware* CreateHardware(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a hardware object by given name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return hardware object pointer
 */
//------------------------------------------------------------------------------
Hardware* GuiInterpreter::CreateHardware(const std::string &type,
                                         const std::string &name)
{
   return moderator->CreateHardware(type, name);
}


//------------------------------------------------------------------------------
// Hardware* GetHardware(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a hardware object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a hardware object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Hardware* GuiInterpreter::GetHardware(const std::string &name)
{
   return moderator->GetHardware(name);
}


//------------------------------------------------------------------------------
// Propagator* CreatePropagator(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a propagator object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a propagator object pointer
 */
//------------------------------------------------------------------------------
Propagator* GuiInterpreter::CreatePropagator(const std::string &type,
                                             const std::string &name)
{
   return moderator->CreatePropagator(type, name);
}


//------------------------------------------------------------------------------
// Propagator* GetPropagator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a propagator object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a propagator object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Propagator* GuiInterpreter::GetPropagator(const std::string &name)
{
   return moderator->GetPropagator(name);
}


//------------------------------------------------------------------------------
// PropSetup* CreateDefaultPropSetup(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a default PropSetup object.
 *
 * @param <name> PropSetup name
 *
 * @return a PropSetup object pointer
 */
//------------------------------------------------------------------------------
PropSetup* GuiInterpreter::CreateDefaultPropSetup(const std::string &name)
{
   return moderator->CreateDefaultPropSetup(name);
}


//------------------------------------------------------------------------------
// PropSetup* GetPropSetup(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a physical model object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a physical model object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
PropSetup* GuiInterpreter::GetPropSetup(const std::string &name)
{
   return moderator->GetPropSetup(name);
}


//------------------------------------------------------------------------------
// ForceModel* CreateForceModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a force model object by given name.
 *
 * @param <name> object name
 *
 * @return a force model object pointer
 */
//------------------------------------------------------------------------------
ForceModel* GuiInterpreter::CreateForceModel(const std::string &name)
{
   return moderator->CreateForceModel(name);
}


//------------------------------------------------------------------------------
// PhysicalModel* CreatePhysicalModel(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a physical model object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a physical model object pointer
 */
//------------------------------------------------------------------------------
PhysicalModel* GuiInterpreter::CreatePhysicalModel(const std::string &type,
                                                   const std::string &name)
{
   return moderator->CreatePhysicalModel(type, name);
}


//------------------------------------------------------------------------------
// PhysicalModel* GetPhysicalModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a physical model object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a physical model object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
PhysicalModel* GuiInterpreter::GetPhysicalModel(const std::string &name)
{
   return moderator->GetPhysicalModel(name);
}


//------------------------------------------------------------------------------
// AtmosphereModel* CreateAtmosphereModel(const std::string &type,
//                                        const std::string &body,
//                                        const std::string &name = "Earth")
//------------------------------------------------------------------------------
/**
 * Creates an atmosphere model object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 * @param <body> the body for which the atmosphere model is requested
 *
 * @return an AtmosphereModel object pointer
 */
//------------------------------------------------------------------------------
AtmosphereModel* GuiInterpreter::CreateAtmosphereModel(const std::string &type,
                                                       const std::string &name,
                                                       const std::string &body)
{
   return moderator->CreateAtmosphereModel(type, name, body);
}


//------------------------------------------------------------------------------
// AtmosphereModel* GetAtmosphereModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves an atmosphere model object pointer by given name.
 *
 * @param <name> object name
 *
 * @return an AtmosphereModel pointer, return null if name not found
 */
//------------------------------------------------------------------------------
AtmosphereModel* GuiInterpreter::GetAtmosphereModel(const std::string &name)
{
   return moderator->GetAtmosphereModel(name);
}


//------------------------------------------------------------------------------
// Burn* CreateBurn(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a burn object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a Burn object pointer
 */
//------------------------------------------------------------------------------
Burn* GuiInterpreter::CreateBurn(const std::string &type,
                                 const std::string &name)
{
   return moderator->CreateBurn(type, name);
}


//------------------------------------------------------------------------------
// Burn* GetBurn(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a burn object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a Burn pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Burn* GuiInterpreter::GetBurn(const std::string &name)
{
   return moderator->GetBurn(name);
}


//------------------------------------------------------------------------------
// Solver* CreateSolver(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a solver object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a Solver object pointer
 */
//------------------------------------------------------------------------------
Solver* GuiInterpreter::CreateSolver(const std::string &type,
                                     const std::string &name)
{
   return moderator->CreateSolver(type, name);
}


//------------------------------------------------------------------------------
// Solver* GetSolver(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a solver object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a Solver pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Solver* GuiInterpreter::GetSolver(const std::string &name)
{
   return moderator->GetSolver(name);
}


//------------------------------------------------------------------------------
// Parameter* CreateParameter(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a parameter object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a parameter object pointer
 */
//------------------------------------------------------------------------------
Parameter* GuiInterpreter::CreateParameter(const std::string &type,
                                           const std::string &name)
{
   return moderator->CreateParameter(type, name);
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
    return moderator->GetParameter(name);
}


//------------------------------------------------------------------------------
// CoordinateSystem* CreateCoordinateSystem(const std::string &name)
//------------------------------------------------------------------------------
CoordinateSystem* GuiInterpreter::CreateCoordinateSystem(const std::string &name)
{
   return moderator->CreateCoordinateSystem(name);
}


//------------------------------------------------------------------------------
// AxisSystem* CreateAxisSystem(const std::string &name)
//------------------------------------------------------------------------------
AxisSystem*  GuiInterpreter::CreateAxisSystem(const std::string &type,
                                              const std::string &name)
{
   return moderator->CreateAxisSystem(type, name);
}


//------------------------------------------------------------------------------
// CoordinateSystem* GetCoordinateSystem(const std::string &name)
//------------------------------------------------------------------------------
CoordinateSystem* GuiInterpreter::GetCoordinateSystem(const std::string &name)
{
   return moderator->GetCoordinateSystem(name);
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
   return moderator->GetInternalCoordinateSystem();
}


//loj: 6/20/05 Added
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
   return moderator->GetPlanetarySourceTypes();
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
   return moderator->GetPlanetarySourceNames();
}


//------------------------------------------------------------------------------
// StringArray& GetPlanetarySourceTypesInUse()
//------------------------------------------------------------------------------
StringArray& GuiInterpreter::GetPlanetarySourceTypesInUse()
{
   return moderator->GetPlanetarySourceTypesInUse();
}


//------------------------------------------------------------------------------
// StringArray& GetAnalyticModelNames()
//------------------------------------------------------------------------------
StringArray& GuiInterpreter::GetAnalyticModelNames()
{
   return moderator->GetAnalyticModelNames();
}


//------------------------------------------------------------------------------
// bool SetAnalyticModelToUse(const std::string &modelName)
//------------------------------------------------------------------------------
bool GuiInterpreter::SetAnalyticModelToUse(const std::string &modelName)
{
   return moderator->SetAnalyticModelToUse(modelName);
}


//------------------------------------------------------------------------------
// bool SetPlanetarySourceName(const std::string &sourceType,
//                           const std::string &filename)
//------------------------------------------------------------------------------
bool GuiInterpreter::SetPlanetarySourceName(const std::string &sourceType,
                                          const std::string &filename)
{
   return moderator->SetPlanetarySourceName(sourceType, filename);
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
   return moderator->SetPlanetarySourceTypesInUse(sourceTypes);
}


//------------------------------------------------------------------------------
// std::string GetPlanetarySourceName(const std::string &sourceType)
//------------------------------------------------------------------------------
std::string GuiInterpreter::GetPlanetarySourceName(const std::string &sourceType)
{
   return moderator->GetPlanetarySourceName(sourceType);
}


//------------------------------------------------------------------------------
// std::string GetPotentialFileName(const std::string &fileType)
//------------------------------------------------------------------------------
std::string GuiInterpreter::GetPotentialFileName(const std::string &fileType)
{
   return moderator->GetPotentialFileName(fileType);
}


//------------------------------------------------------------------------------
// std::string GetFileName(const std::string &fileType)
//------------------------------------------------------------------------------
std::string GuiInterpreter::GetFileName(const std::string &fileType)
{
   return moderator->GetFileName(fileType);
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
   return moderator->CreateSubscriber(type, name, filename, createDefault);
}


//------------------------------------------------------------------------------
// Subscriber* GetSubscriber(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a subscriber object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a subscriber object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Subscriber* GuiInterpreter::GetSubscriber(const std::string &name)
{
   return moderator->GetSubscriber(name);
}


//------------------------------------------------------------------------------
// StopCondition* CreateStopCondition(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
StopCondition* GuiInterpreter::CreateStopCondition(const std::string &type,
                                                   const std::string &name)
{
   return moderator->CreateStopCondition(type, name);
}


//------------------------------------------------------------------------------
// StopCondition* GetStopCondition(const std::string &name)
//------------------------------------------------------------------------------
StopCondition* GuiInterpreter::GetStopCondition(const std::string &name)
{
   return moderator->GetStopCondition(name);
}


//------------------------------------------------------------------------------
// Function* CreateFunction(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
Function* GuiInterpreter::CreateFunction(const std::string &type,
                                         const std::string &name)
{
   return moderator->CreateFunction(type, name);
}


//------------------------------------------------------------------------------
// Function* GetFunction(const std::string &name)
//------------------------------------------------------------------------------
Function* GuiInterpreter::GetFunction(const std::string &name)
{
   return moderator->GetFunction(name);
}


//------------------------------------------------------------------------------
// GmatCommand* CreateCommand(const std::string &type,
//                            const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Creates a command object by given type and name.
 *
 * @param <type> command type
 * @param <name> command name
 *
 * @return a command object pointer
 */
//------------------------------------------------------------------------------
GmatCommand* GuiInterpreter::CreateCommand(const std::string &type,
                                           const std::string &name)
{
   return moderator->CreateCommand(type, name);
}


//------------------------------------------------------------------------------
// GmatCommand* CreateDefaultCommand(const std::string &type,
//                                   const std::string &name = "")
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
                                                  const std::string &name)
{
   return moderator->CreateDefaultCommand(type, name);
}


//------------------------------------------------------------------------------
// bool LoadDefaultMission()
//------------------------------------------------------------------------------
bool GuiInterpreter::LoadDefaultMission()
{
   return moderator->LoadDefaultMission();
}


//------------------------------------------------------------------------------
// bool ClearResource()
//------------------------------------------------------------------------------
bool GuiInterpreter::ClearResource()
{
   return moderator->ClearResource();
}


//------------------------------------------------------------------------------
// bool ClearCommandSeq(Integer sandboxNum = 1)
//------------------------------------------------------------------------------
bool GuiInterpreter::ClearCommandSeq(Integer sandboxNum)
{
   return moderator->ClearCommandSeq(sandboxNum);
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
   return moderator->AppendCommand(cmd, sandboxNum);
}


//------------------------------------------------------------------------------
// GmatCommand* AppendCommand(const std::string &type, const std::string &name,
//                        Integer sandboxNum)
//------------------------------------------------------------------------------
GmatCommand* GuiInterpreter::AppendCommand(const std::string &type,
                                           const std::string &name,
                                           Integer sandboxNum)
{
   return moderator->AppendCommand(type, name, sandboxNum);
}


//------------------------------------------------------------------------------
// bool InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd, Integer sandboxNum)
//------------------------------------------------------------------------------
bool GuiInterpreter::InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd,
                                   Integer sandboxNum)
{
   return moderator->InsertCommand(cmd, prevCmd, sandboxNum);
}


//------------------------------------------------------------------------------
// GmatCommand* DeleteCommand(GmatCommand *cmd, Integer sandboxNum)
//------------------------------------------------------------------------------
GmatCommand* GuiInterpreter::DeleteCommand(GmatCommand *cmd, Integer snadboxNum)
{
   return moderator->DeleteCommand(cmd, snadboxNum);
}


//------------------------------------------------------------------------------
// GmatCommand* GetNextCommand(Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Retrieves a next command object.
 *
 * @param <sandboxNum> sandbox number
 *
 * @return a next command object pointer, return null if no command found
 */
//------------------------------------------------------------------------------
GmatCommand* GuiInterpreter::GetNextCommand(Integer sandboxNum)
{
   return moderator->GetNextCommand(sandboxNum);
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
   return moderator->RunMission(sandboxNum);
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
   return moderator->ChangeRunState(state, sandboxNum);
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
   return moderator->InterpretScript(scriptFilename);
}


//------------------------------------------------------------------------------
// bool SaveScript(const std::string &scriptFilename)
//------------------------------------------------------------------------------
/**
 * Builds scripts from objects and write to a file.
 *
 * @param <scriptFilename> output script file name
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool GuiInterpreter::SaveScript(const std::string &scriptFilename)
{
   return moderator->SaveScript(scriptFilename);
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
   return moderator->RunScript(sandboxNum);
}


//------------------------------------------------------------------------------
// void SetInputFocus()
//------------------------------------------------------------------------------
void GuiInterpreter::SetInputFocus()
{
#if !defined __CONSOLE_APP__
   GmatAppData::GetMainFrame()->SetFocus();
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
{
   isInitialized = false;
   moderator = Moderator::Instance();
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
