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
 * Defines the operations between GUI subsystem and the Moderator.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
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
#include "Subscriber.hpp"
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

//----- factory

//loj:Do we need this?
//------------------------------------------------------------------------------
// StringArray GetListOfContainer()
//------------------------------------------------------------------------------
//  StringArray GuiInterpreter::GetListOfContainer()
//  {
//     //loj: such as: "PropSetup", "SolarSystem" Do we need this?
//     return theModerator->GetListOfContainer();
//  }

//------------------------------------------------------------------------------
// StringArray GetListOfFactoryItems(Gmat::ObjectType type)
//------------------------------------------------------------------------------
StringArray GuiInterpreter::GetListOfFactoryItems(Gmat::ObjectType type)
{
   //loj: such as: "RungeKuttaVerner89" under Propagator
   //loj: returns names of all creatable subfactory items. if none returns empty array.
   return theModerator->GetListOfFactoryItems(type);
}

//----- configuration
//------------------------------------------------------------------------------
// StringArray GetListOfConfiguredItems(Gmat::ObjectType type)
//------------------------------------------------------------------------------
StringArray GuiInterpreter::GetListOfConfiguredItems(Gmat::ObjectType type)
{
   return theModerator->GetListOfConfiguredItems(type);
}

//------------------------------------------------------------------------------
// bool RemoveConfiguredItem(Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
bool GuiInterpreter::RemoveConfiguredItem(Gmat::ObjectType type, const std::string &name)
{
   //loj: to remove item from configuration
   return theModerator->RemoveConfiguredItem(type, name);
   return true; //for now
}

// asset
//------------------------------------------------------------------------------
// Spacecraft* CreateSpacecraft(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
Spacecraft* GuiInterpreter::CreateSpacecraft(const std::string &type,
                                             const std::string &name)
{
   //loj: for creating another Spacecraft
   Spacecraft *sc = theModerator->CreateSpacecraft(type, name);
   return sc;
}

//------------------------------------------------------------------------------
// Spacecraft* GetSpacecraft(const std::string &name)
//------------------------------------------------------------------------------
Spacecraft* GuiInterpreter::GetSpacecraft(const std::string &name)
{
   //loj: for getting Spacecraft by name
   Spacecraft *sc = theModerator->GetSpacecraft(name);
   return sc;
}

// propagator
//------------------------------------------------------------------------------
// Propagator* CreatePropagator(const std::string &name, std::string type)
//------------------------------------------------------------------------------
Propagator* GuiInterpreter::CreatePropagator(const std::string &name, std::string type)
{
   //loj: for creating an existing or a new Propagator instance.
   //loj: new Propagator should have derived from the Propagator.(source can be DLL, MATLAB)
   return theModerator->CreatePropagator(name, type);
}

//------------------------------------------------------------------------------
// Propagator* GetPropagator(const std::string &name)
//------------------------------------------------------------------------------
Propagator* GuiInterpreter::GetPropagator(const std::string &name)
{
   //loj: for getting existing propagator.
   return theModerator->GetPropagator(name);
}

// PhysicalModel
//------------------------------------------------------------------------------
// PhysicalModel* CreatePhysicalModel(const std::string &name, std::string type)
//------------------------------------------------------------------------------
PhysicalModel* GuiInterpreter::CreatePhysicalModel(const std::string &name, std::string type)
{
   //loj: for creating an existing or a new PhysicalModel.
   //loj: MyNewPhysicalModel should have derived from the PhysicalModel.(source can be DLL, MATLAB)
   return theModerator->CreatePhysicalModel(name, type);
}

//------------------------------------------------------------------------------
// PhysicalModel* GetPhysicalModel(const std::string &name)
//------------------------------------------------------------------------------
PhysicalModel* GuiInterpreter::GetPhysicalModel(const std::string &name)
{
   //loj: for getting existing PhysicalModel
   return theModerator->GetPhysicalModel(name);
}

// Parameter
//------------------------------------------------------------------------------
// Parameter* CreateParameter(const std::string &name, std::string type)
//------------------------------------------------------------------------------
Parameter* GuiInterpreter::CreateParameter(const std::string &name, std::string type)
{
   //loj: for creating an existing or a new Parameter.
   //loj: new Parameter should have derived from the Parameter.(source can be DLL, MATLAB)
   return theModerator->CreateParameter(name, type);
}

//------------------------------------------------------------------------------
// Parameter* GetParameter(const std::string &name)
//------------------------------------------------------------------------------
Parameter* GuiInterpreter::GetParameter(const std::string &name)
{
   //loj: for getting existing Parameter type
   return theModerator->GetParameter(name);
}

// CelestialBody
//------------------------------------------------------------------------------
// CelestialBody* CreateCelestialBody(const std::string &name, std::string type)
//------------------------------------------------------------------------------
CelestialBody* GuiInterpreter::CreateCelestialBody(const std::string &name, std::string type)
{
   //loj: for creating a new body type. such as "NewJupiterMoon".
   //loj: newType should have derived from the CelestialBody.(source can be DLL, MATLAB)
   //loj: assumes there is only 1 SolarSystem so container name is ignored.
   //loj: this will add a newbody into SolarSystem
   return theModerator->CreateCelestialBody(name, type);
}

//------------------------------------------------------------------------------
// CelestialBody* GetCelestialBody(const std::string &name)
//------------------------------------------------------------------------------
CelestialBody* GuiInterpreter::GetCelestialBody(const std::string &name)
{
   //loj: assumes body type from SolarSystem container
   return theModerator->GetCelestialBody(name);
}

// solar system
//------------------------------------------------------------------------------
// SolarSystem* GetDefaultSolarSystem()
//------------------------------------------------------------------------------
SolarSystem* GuiInterpreter::GetDefaultSolarSystem()
{
   //loj: for getting default SolarSystem container object
   //loj: assume GUI could use methods from SolarSystem for getting bodies from the SolarSystem
   //loj: e.g. theSolarSystem->GetEarth()
   return theModerator->GetDefaultSolarSystem();
}

// subscriber
//------------------------------------------------------------------------------
// Subscriber* CreateSubscriber(const std::string &name, std::string type)
//------------------------------------------------------------------------------
Subscriber* GuiInterpreter::CreateSubscriber(const std::string &name, std::string type)
{
   //loj: for creating new subscriber. such as "MyEphem" or "MyMovieClip"
   return theModerator->CreateSubscriber(name, type);
}

//------------------------------------------------------------------------------
// Subscriber* GetSubscriber(const std::string &name)
//------------------------------------------------------------------------------
Subscriber* GuiInterpreter::GetSubscriber(const std::string &name)
{
   //loj: for getting a Subscriber. such as "OpenGL3DPlot"
   return theModerator->GetSubscriber(name);
}

// command
//------------------------------------------------------------------------------
// Command* CreateCommand(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
Command* GuiInterpreter::CreateCommand(const std::string &type, const std::string &name)
{
   //loj: for creating a new Command, such as "MyNewCommand", "DoMyLittleThing"
   return theModerator->CreateCommand(type, name);
}

//------------------------------------------------------------------------------
// Command* GetCommand(const std::string &name)
//------------------------------------------------------------------------------
Command* GuiInterpreter::GetCommand(const std::string &name)
{
   //loj: for getting a Command object by command
   return theModerator->GetCommand(name);
}

// command sequence
//------------------------------------------------------------------------------
// Command* GetNextCommand(Integer sandboxNum)
//------------------------------------------------------------------------------
Command* GuiInterpreter::GetNextCommand(Integer sandboxNum)
{
   //loj: need by GUI to populate the command sequence
   //loj: return NULL if no command
   return theModerator->GetNextCommand(sandboxNum);
}

//------------------------------------------------------------------------------
// bool DeleteCommand(const std::string &name, Integer position,
//                    Integer sandboxNum)
//------------------------------------------------------------------------------
bool GuiInterpreter::DeleteCommand(const std::string &name, Integer position,
                                   Integer snadboxNum)
{
   //deletes command at current position
   return theModerator->DeleteCommand(name, position, snadboxNum);
}

//------------------------------------------------------------------------------
// Command* InsertCommand(const std::string &type, const std::string &name,
//                        Integer position, bool addAbove, Integer sandboxNum)
//------------------------------------------------------------------------------
Command* GuiInterpreter::InsertCommand(const std::string &type, const std::string &name,
                                       Integer position, bool addAbove,
                                       Integer sandboxNum)
{
   //inserts command above or below current position
   return theModerator->InsertCommand(type, name, position, addAbove, sandboxNum);
}

//------------------------------------------------------------------------------
// Command* AppendCommand(const std::string &type, const std::string &name,
//                        Integer sandboxNum)
//------------------------------------------------------------------------------
Command* GuiInterpreter::AppendCommand(const std::string &type, const std::string &name,
                                       Integer sandboxNum)
{
   //appends command
   return theModerator->AppendCommand(type, name, sandboxNum);
}

// sandbox
//------------------------------------------------------------------------------
// Integer RunMission(Integer sandboxNum)
//------------------------------------------------------------------------------
Integer GuiInterpreter::RunMission(Integer sandboxNum)
{
   //loj: need by GUI when "Run" button is pressed after mission sequence is setup.
   return theModerator->RunMission(sandboxNum);
}

// Script
//------------------------------------------------------------------------------
// bool InterpretScript(const std::string &scriptFilename)
//------------------------------------------------------------------------------
bool GuiInterpreter::InterpretScript(const std::string &scriptFilename)
{
   return theModerator->InterpretScript(scriptFilename);
}

//------------------------------------------------------------------------------
// bool SaveScript(const std::string &scriptFilename)
//------------------------------------------------------------------------------
bool GuiInterpreter::SaveScript(const std::string &scriptFilename)
{
   return theModerator->SaveScript(scriptFilename);
}

//------------------------------------------------------------------------------
// Integer RunScript(Integer sandboxNum)
//------------------------------------------------------------------------------
Integer GuiInterpreter::RunScript(Integer sandboxNum)
{
   return theModerator->RunMission(sandboxNum);
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
   theModerator = Moderator::Instance();
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
}
