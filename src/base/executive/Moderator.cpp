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
 * Implements operations of the GMAT executive.
 */
//------------------------------------------------------------------------------


#include "Moderator.hpp"
#include "NoOp.hpp"
#include "PlotInterface.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// static data
//---------------------------------
Moderator* Moderator::instance = NULL;
GuiInterpreter* Moderator::theGuiInterpreter = NULL;
ScriptInterpreter* Moderator::theScriptInterpreter = NULL;
FactoryManager* Moderator::theFactoryManager = NULL;
ConfigManager* Moderator::theConfigManager = NULL;

//-----------------------------------------------------
//*** should match with ObjectType in gmatdefs.hpp
//-----------------------------------------------------
const std::string
Moderator::OBJECT_TYPE_STRING[Gmat::UNKNOWN_OBJECT-Gmat::SPACECRAFT+1] =
{
    "Spacecraft",
    "GroundStation",
    "Burn",
    "Command",
    "Propagator",
    "ForceModel",
    "PhysicalModel",
    "Interpolator",
    "SolarSystem",
    "CelestialBody",
    "Parameter",
    "StopCondition",
    "Solver",
    "Subscriber",
    "PropSetup"
};

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// Moderator* Instance()
//------------------------------------------------------------------------------
Moderator* Moderator::Instance()
{
    if (instance == NULL)
        instance = new Moderator;
    
    return instance;
}

//------------------------------------------------------------------------------
// bool Initialize(bool fromGui = false)
//------------------------------------------------------------------------------
bool Moderator::Initialize(bool fromGui)
{
    if (!isInitialized)
    {
        MessageInterface::ShowMessage("Moderator is creating core engine ...\n");
        
        // Create interpreters and managers
        theGuiInterpreter = GuiInterpreter::Instance();
        theScriptInterpreter = ScriptInterpreter::Instance();
        theFactoryManager = FactoryManager::Instance();
        theConfigManager = ConfigManager::Instance();

        // Create publisher
        thePublisher = Publisher::Instance();

        // Create factories
        theBurnFactory = new BurnFactory();
        theCommandFactory = new CommandFactory();
        theForceModelFactory = new ForceModelFactory();
        theParameterFactory = new ParameterFactory();
        thePhysicalModelFactory = new PhysicalModelFactory();
        thePropSetupFactory = new PropSetupFactory();
        thePropagatorFactory = new PropagatorFactory();
        theSpacecraftFactory = new SpacecraftFactory();
        theStopConditionFactory = new StopConditionFactory();
        theSubscriberFactory = new SubscriberFactory();
        theSolverFactory = new SolverFactory();

        // Register factories
        theFactoryManager->RegisterFactory(theBurnFactory);
        theFactoryManager->RegisterFactory(theCommandFactory);
        theFactoryManager->RegisterFactory(theForceModelFactory);
        theFactoryManager->RegisterFactory(theParameterFactory);
        theFactoryManager->RegisterFactory(thePhysicalModelFactory);
        theFactoryManager->RegisterFactory(thePropSetupFactory);
        theFactoryManager->RegisterFactory(thePropagatorFactory);
        theFactoryManager->RegisterFactory(theSpacecraftFactory);
        theFactoryManager->RegisterFactory(theStopConditionFactory);
        theFactoryManager->RegisterFactory(theSubscriberFactory);
        theFactoryManager->RegisterFactory(theSolverFactory);
        
        if (fromGui)
        {
            CreateDefaultMission();
        }
        
        MessageInterface::ShowMessage("Moderator successfully created core engine\n");
        
        isInitialized = true;
    }

    return isInitialized;
}

//----- ObjectType
//------------------------------------------------------------------------------
// std::string GetObjectTypeString(Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Returns object type name of given object type.
 *
 * @param <type> object type
 *
 * @return object type name
 */
//------------------------------------------------------------------------------
std::string Moderator::GetObjectTypeString(Gmat::ObjectType type)
{
    if (type >= Gmat::SPACECRAFT && type <= Gmat::PROP_SETUP)
        return OBJECT_TYPE_STRING[type - Gmat::SPACECRAFT];
    else
        return "UnknownObject";
}

//----- interpreter
//------------------------------------------------------------------------------
// GuiInterpreter* GetGuiInterpreter()
//------------------------------------------------------------------------------
/**
 * Returns GuiInterpreter pointer.
 *
 * @return GuiInterpreter pointer
 */
//------------------------------------------------------------------------------
GuiInterpreter* Moderator::GetGuiInterpreter()
{
    return theGuiInterpreter;
}

//------------------------------------------------------------------------------
// ScriptInterpreter* GetScriptInterpreter()
//------------------------------------------------------------------------------
/**
 * Returns ScriptInterpreter pointer.
 *
 * @return ScriptInterpreter pointer
 */
//------------------------------------------------------------------------------
ScriptInterpreter* Moderator::GetScriptInterpreter()
{
    return theScriptInterpreter;
}

//------------------------------------------------------------------------------
// void SetGuiInterpreter(GuiInterpreter *guiInterp)
//------------------------------------------------------------------------------
/**
 * Sets GuiInterpreter pointer.
 */
//------------------------------------------------------------------------------
void Moderator::SetGuiInterpreter(GuiInterpreter *guiInterp)
{
    //loj: allow setting only for the first time?
    if (theGuiInterpreter == NULL)
        theGuiInterpreter = guiInterp;
}

//------------------------------------------------------------------------------
// void SetScriptInterpreter(ScriptInterpreter *scriptInterp)
//------------------------------------------------------------------------------
/**
 * Sets ScriptInterpreter pointer.
 */
//------------------------------------------------------------------------------
void Moderator::SetScriptInterpreter(ScriptInterpreter *scriptInterp)
{
    //loj: allow setting only for the first time?
    if (theScriptInterpreter == NULL)
        theScriptInterpreter = scriptInterp;
}

//----- factory
//------------------------------------------------------------------------------
// StringArray GetListOfFactoryItems(Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Returns names of all configured items of object type.
 *
 * @param <type> object type
 *
 * @return array of configured item names; return empty array if none
 */
//------------------------------------------------------------------------------
StringArray Moderator::GetListOfFactoryItems(Gmat::ObjectType type)
{
    return theFactoryManager->GetListOfItems(type);
}

//----- configuration
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
StringArray& Moderator::GetListOfConfiguredItems(Gmat::ObjectType type)
{
    return theConfigManager->GetListOfItems(type);
}

//------------------------------------------------------------------------------
// GmatBase* GetConfiguredItem(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* Moderator::GetConfiguredItem(const std::string &name)
{
//      MessageInterface::ShowMessage("Moderator::GetConfiguredItem() entered: "
//                                    "name = " + name + "\n");
    return theConfigManager->GetItem(name);
}

//------------------------------------------------------------------------------
// bool RenameConfiguredItem(Gmat::ObjectType type, const std::string &oldName
//                           const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames configured item
 *
 * @param <type> object type
 * @param <oldName>  old object name
 * @param <newName>  new object name
 *
 * @return true if the item has been removed; false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::RenameConfiguredItem(Gmat::ObjectType type, const std::string &oldName,
                                     const std::string &newName)
{
    return theConfigManager->RenameItem(type, oldName, newName);
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
bool Moderator::RemoveConfiguredItem(Gmat::ObjectType type, const std::string &name)
{
    return theConfigManager->RemoveItem(type, name);
}

// Spacecraft
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
Spacecraft* Moderator::CreateSpacecraft(const std::string &type, const std::string &name)
{
    Spacecraft *sc =
        theFactoryManager->CreateSpacecraft(type, name);

    if (sc == NULL)
    {
        MessageInterface::ShowMessage("Moderator::CreateSpacecraft() Error Creating "
                                      "%s.  Check SpacecraftFactory. \n", type.c_str());

        throw GmatBaseException("Error Creating Spacecraft");
        //return NULL;
    }
    
    // Manage it if it is a named Spacecraft
    try
    {
        if (sc->GetName() != "")
            theConfigManager->AddSpacecraft(sc);
    }
    catch (BaseException &e)
    {
        MessageInterface::ShowMessage("Moderator::CreateSpacecraft()\n" +
                                      e.GetMessage());
    }

    return sc;
}

//------------------------------------------------------------------------------
// Spacecraft* GetSpacecraft(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a spacecraft object pointer by given name and add to configuration.
 *
 * @param <name> object name
 *
 * @return a spacecraft object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Spacecraft* Moderator::GetSpacecraft(const std::string &name)
{
    Spacecraft *sc = theConfigManager->GetSpacecraft(name);

    return sc;
}

// Propagator
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
Propagator* Moderator::CreatePropagator(const std::string &type, const std::string &name)
{
    //MessageInterface::ShowMessage("Moderator::CreatePropagator() name = %s\n",
    //                              name.c_str());
    Propagator *prop = theFactoryManager->CreatePropagator(type, name);
    
    if (prop ==  NULL)
    {
        MessageInterface::ShowMessage("Moderator::CreatePropagator() Error Creating "
                                      "%s.  Check PropagatorFactory. \n", type.c_str());

        throw GmatBaseException("Error Creating Propagator");
        //return NULL;
    }
    
    // Manage it if it is a named Propagator
    try
    {
        if (prop->GetName() != "")
            theConfigManager->AddPropagator(prop);
    }
    catch (BaseException &e)
    {
        MessageInterface::ShowMessage("Moderator::CreatePropagator()\n" +
                                      e.GetMessage());
    }
        
    return prop;
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
Propagator* Moderator::GetPropagator(const std::string &name)
{
    return theConfigManager->GetPropagator(name);
}

// PhysicalModel
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
PhysicalModel* Moderator::CreatePhysicalModel(const std::string &type,
                                              const std::string &name)
{
    PhysicalModel *physicalModel = theFactoryManager->CreatePhysicalModel(type, name);
    
    if (physicalModel ==  NULL)
    {
        MessageInterface::ShowMessage("Moderator::CreatePhysicalModel() Error Creating "
                                      "%s.  Check PhysicalModelFactory. \n", type.c_str());

        throw GmatBaseException("Error Creating PhysicalModel");
        //return NULL;
    }
    
    // Manage it if it is a named PhysicalModel
    try
    {
        if (physicalModel->GetName() != "")
            theConfigManager->AddPhysicalModel(physicalModel);
    }
    catch (BaseException &e)
    {
        MessageInterface::ShowMessage("Moderator::CreatePropagator()\n" +
                                      e.GetMessage());
    }
        
    return physicalModel;
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
PhysicalModel* Moderator::GetPhysicalModel(const std::string &name)
{
    return theConfigManager->GetPhysicalModel(name);
}

// burn
//------------------------------------------------------------------------------
// Burn* CreateBurn(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a burn object by given type and name and add to configuration.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a burn object pointer
 */
//------------------------------------------------------------------------------
Burn* Moderator::CreateBurn(const std::string &type,
                            const std::string &name)
{
    Burn *burn = theFactoryManager->CreateBurn(type, name);

    if (burn ==  NULL)
    {
        MessageInterface::ShowMessage("Moderator::CreateBurn() Error Creating "
                                      "%s.  Check BurnFactory. \n", type.c_str());

        throw GmatBaseException("Error Creating Burn");
        //return NULL;
    }
    
    // Manage it if it is a named burn
    try
    {
        if (burn->GetName() != "")
            theConfigManager->AddBurn(burn);
    }
    catch (BaseException &e)
    {
        MessageInterface::ShowMessage("Moderator::CreateBurn()\n" +
                                      e.GetMessage());
    }
    
    return burn;
}

//------------------------------------------------------------------------------
// Burn* GetBurn(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a burn object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a burn pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Burn* Moderator::GetBurn(const std::string &name)
{
    return theConfigManager->GetBurn(name);
}

// Parameter
//------------------------------------------------------------------------------
// Parameter* CreateParameter(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a parameter object by given type and name and add to configuration.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a parameter object pointer
 */
//------------------------------------------------------------------------------
Parameter* Moderator::CreateParameter(const std::string &type, const std::string &name)
{
    Parameter *parameter = theFactoryManager->CreateParameter(type, name);

    if (parameter == NULL)
    {

        MessageInterface::ShowMessage("Moderator::CreateParameter() Error Creating " +
                                      type + ".  Check ParameterFactory. \n");

        throw GmatBaseException("Error Creating Parameter");
        //return NULL;
    }

    // Manage it if it is a named parameter
    try
    {
        if (parameter->GetName() != "")
            theConfigManager->AddParameter(parameter);
    }
    catch (BaseException &e)
    {
        MessageInterface::ShowMessage("Moderator::CreateParameter()\n" +
                                      e.GetMessage());
    }
    
    return parameter;
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
Parameter* Moderator::GetParameter(const std::string &name)
{
    return theConfigManager->GetParameter(name);
}

// ForceModel
//------------------------------------------------------------------------------
// ForceModel* CreateForceModel(const std::string &name)
//------------------------------------------------------------------------------
ForceModel* Moderator::CreateForceModel(const std::string &name)
{
    ForceModel *fm = theFactoryManager->CreateForceModel(name);

    if (fm == NULL)
    {
        MessageInterface::ShowMessage("Moderator::CreateForceModel() Error Creating "
                                      "%s.  Check ForceModelFactory. \n", name.c_str());

        throw GmatBaseException("Error Creating ForceModel");
        //return NULL;
    }
    
    // Manage it if it is a named parameter
    try
    {
        if (fm->GetName() != "")
            theConfigManager->AddForceModel(fm);
    }
    catch (BaseException &e)
    {
        MessageInterface::ShowMessage("Moderator::CreateForceModel()\n" +
                                      e.GetMessage());
    }

    return fm;
}

//------------------------------------------------------------------------------
// ForceModel* GetForceModel(const std::string &name)
//------------------------------------------------------------------------------
ForceModel* Moderator::GetForceModel(const std::string &name)
{
    return theConfigManager->GetForceModel(name);
}

//------------------------------------------------------------------------------
// bool AddToForceModel(const std::string &forceModelName, const std::string &forceName)
//------------------------------------------------------------------------------
bool Moderator::AddToForceModel(const std::string &forceModelName,
                                const std::string &forceName)
{
    bool status = true;
    ForceModel *fm = theConfigManager->GetForceModel(forceModelName);
    PhysicalModel *physicalModel = theConfigManager->GetPhysicalModel(forceName);
    fm->AddForce(physicalModel);
    return status;
}

// StopCondition
//------------------------------------------------------------------------------
// StopCondition* CreateStopCondition(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
StopCondition* Moderator::CreateStopCondition(const std::string &type,
                                              const std::string &name)
{
    StopCondition *stopCond = theFactoryManager->CreateStopCondition(type, name);
    
    if (stopCond ==  NULL)
    {
        MessageInterface::ShowMessage("Moderator::CreateStopCondition() Error Creating "
                                      "%s.  Check StopConditionFactory. \n", type.c_str());

        throw GmatBaseException("Error Creating StopCondition");
        return NULL;
    }
    
    // Manage it if it is a named stopCondition
    try
    {
        if (stopCond->GetName() != "")
            theConfigManager->AddStopCondition(stopCond);
    }
    catch (BaseException &e)
    {
        MessageInterface::ShowMessage("Moderator::CreateStopCondition()\n" +
                                      e.GetMessage());
    }
    
    return stopCond;
}

//------------------------------------------------------------------------------
// StopCondition* GetStopCondition(const std::string &name)
//------------------------------------------------------------------------------
StopCondition* Moderator::GetStopCondition(const std::string &name)
{
    return theConfigManager->GetStopCondition(name);
}

// Solver
//------------------------------------------------------------------------------
// Solver* CreateSolver(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a solver object by given type and name and add to configuration.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a solver object pointer
 */
//------------------------------------------------------------------------------
Solver* Moderator::CreateSolver(const std::string &type, const std::string &name)
{
    Solver *solver = theFactoryManager->CreateSolver(type, name);

    if (solver == NULL)
    {
        MessageInterface::ShowMessage("Moderator::CreateSolver() Error Creating "
                                      "%s.  Check SolverFactory. \n", type.c_str());

        throw GmatBaseException("Error Creating Solver");
        //return NULL;
    }
    
    // Manage it if it is a named solver
    try
    {
        if (solver->GetName() != "")
            theConfigManager->AddSolver(solver);
    }
    catch (BaseException &e)
    {
        MessageInterface::ShowMessage("Moderator::CreateSolver()\n" +
                                      e.GetMessage());
    }
    
    return solver;
}

//------------------------------------------------------------------------------
// Solver* GetSolver(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a solver object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a solver object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Solver* Moderator::GetSolver(const std::string &name)
{
    return theConfigManager->GetSolver(name);
}

// PropSetup
//------------------------------------------------------------------------------
// PropSetup* CreateDefaultPropSetup(const std::string &name)
//------------------------------------------------------------------------------
PropSetup* Moderator::CreateDefaultPropSetup(const std::string &name)
{
    // assumes "RungeKutta89" is the default propagator
    std::string propName = name + "RKV89";
    Propagator *prop = CreatePropagator("RungeKutta89", propName);
    
    // creates empty ForceModel
    std::string fmName = name + "ForceModel";
    ForceModel *fm = CreateForceModel(fmName);

    // create PropSetup
    PropSetup *propSetup = theFactoryManager->CreatePropSetup(name);

    // create PointMass force and add to Force
    //loj: 3/15/04 do not configure force, force model has linked list of force
    PhysicalModel *earthGrav = CreatePhysicalModel("PointMassForce", "");
    
    //loj: 3/15/ 04 always add to force model before propSetup::SetForceModel()
    // because PropSetup::Initialize() needs at least 1 force
    fm->AddForce(earthGrav);
    
    if (prop)
    {
        propSetup->SetPropagator(prop);
        propSetup->SetUseDrag(false);   // turn off internal drag
    }
    
    if (fm)
    {
        propSetup->SetForceModel(fm); //loj: 3/12/04 added
    }
    
    theConfigManager->AddPropSetup(propSetup);
    
    return propSetup;
}

//------------------------------------------------------------------------------
// PropSetup* CreatePropSetup(const std::string &name, const std::string &propagatorName,
//                            const std::string &forceModelName)
//------------------------------------------------------------------------------
PropSetup* Moderator::CreatePropSetup(const std::string &name,
                                      const std::string &propagatorName,
                                      const std::string &forceModelName)
{
    // assumes propagatorName and forceModelName exist already.
    Propagator *prop = theConfigManager->GetPropagator(propagatorName);
    ForceModel *fm = theConfigManager->GetForceModel(forceModelName);
    PropSetup *propSetup = theFactoryManager->CreatePropSetup(name);
    if (prop)
        propSetup->SetPropagator(prop);
    if (fm)
        propSetup->SetForceModel(fm);
    theConfigManager->AddPropSetup(propSetup);
    return propSetup;
}

//------------------------------------------------------------------------------
// PropSetup* GetPropSetup(const std::string &name)
//------------------------------------------------------------------------------
PropSetup* Moderator::GetPropSetup(const std::string &name)
{
    return theConfigManager->GetPropSetup(name);
}

// CelestialBody
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
CelestialBody* Moderator::CreateCelestialBody(const std::string &type,
                                              const std::string &name)
{
    // type is such as "Earth", "Moon" - we need const string EarthString etc.
    CelestialBody *body = theFactoryManager->CreateCelestialBody(type, name);
    
    if (body == NULL)
    {
        MessageInterface::ShowMessage("Moderator::CreateCelestialBody() Error Creating "
                                      "%s.  Check CelestialBodyFactory. \n", type.c_str());

        throw GmatBaseException("Error Creating CelestialBody");
        //return NULL;
    }
    
    // Manage it if it is a named body
    //loj: Do we want to add bodies tnat are not in the default solar system?
    try
    {
        if (body->GetName() != "")
            theConfigManager->AddCelestialBody(body);
    }
    catch (BaseException &e)
    {
        MessageInterface::ShowMessage("Moderator::CreateSolver()\n" +
                                      e.GetMessage());
    }
    
    return body;
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
CelestialBody* Moderator::GetCelestialBody(const std::string &name)
{
    return theConfigManager->GetCelestialBody(name);
}

// SolarSystem
//------------------------------------------------------------------------------
// SolarSystem* GetDefaultSolarSystem()
//------------------------------------------------------------------------------
/**
 * Retrieves a default solar system object pointer.
 *
 * @return a default solar system object pointer
 */
//------------------------------------------------------------------------------
SolarSystem* Moderator::GetDefaultSolarSystem()
{
    //return theConfigManager->GetDefaultSolarSystem();
    return theDefaultSolarSystem;
}

//------------------------------------------------------------------------------
// SolarSystem* CreateSolarSystem(const std::string &name)
//------------------------------------------------------------------------------
SolarSystem* Moderator::CreateSolarSystem(const std::string &name)
{
    SolarSystem *solarSys = theFactoryManager->CreateSolarSystem(name);
    theConfigManager->AddSolarSystem(solarSys);
    return solarSys;
}

//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystemInUse()
//------------------------------------------------------------------------------
SolarSystem* Moderator::GetSolarSystemInUse()
{
    return theConfigManager->GetSolarSystemInUse();
}

//------------------------------------------------------------------------------
// bool SetSolarSystemInUse(const std::string &name)
//------------------------------------------------------------------------------
bool Moderator::SetSolarSystemInUse(const std::string &name)
{
    return theConfigManager->SetSolarSystemInUse(name);
}

//------------------------------------------------------------------------------
// StringArray& GetSolarSystemSourceList()
//------------------------------------------------------------------------------
/**
 * Retrieves solar system source list
 *
 * @return a solar system source list
 */
//------------------------------------------------------------------------------
StringArray& Moderator::GetSolarSystemSourceList()
{
    return theSolarSystemSourceList;
}

//------------------------------------------------------------------------------
// StringArray& GetSolarSystemSourceFileList()
//------------------------------------------------------------------------------
/**
 * Retrieves solar system source file list
 *
 * @return a solar system source file list
 */
//------------------------------------------------------------------------------
StringArray& Moderator::GetSolarSystemSourceFileList()
{
    return theSolarSystemSourceFileList;
}

//------------------------------------------------------------------------------
// bool SetSlpFileToUse(const std::string &filename)
//------------------------------------------------------------------------------
bool Moderator::SetSlpFileToUse(const std::string &filename)
{
    bool status = false;
    
    if (IsSlpAlreadyInUse)
    {
        MessageInterface::ShowMessage("Moderator::SetSlpFileToUse() SlpFile already set\n");
        status = true;
    }
    else
    {
        MessageInterface::ShowMessage("Moderator::SetSlpFileToUse() setting SlpFile\n");
        theDefaultSlpFile = new SlpFile(filename);
        if (theDefaultSolarSystem->SetSource(Gmat::SLP))
        {
            if (theDefaultSolarSystem->SetSourceFile(theDefaultSlpFile))
            {
                theSolarSystemSourceList.push_back("SLP");
                theSolarSystemSourceFileList.push_back(filename);
                IsSlpAlreadyInUse = true;
                status = true;
            }
        }
    }

    return status;
}

// Subscriber
//------------------------------------------------------------------------------
// Subscriber* CreateSubscriber(const std::string &type, const std::string &name,
//                              const std::string &filename)
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
Subscriber* Moderator::CreateSubscriber(const std::string &type,
                                        const std::string &name,
                                        const std::string &filename)
{
//    MessageInterface::ShowMessage("Moderator::CreateSubscriber() entered: type = " + type +
//                                  ", name = " + name + "\n");
    Subscriber *subs = theFactoryManager->CreateSubscriber(type, name, filename);
    theConfigManager->AddSubscriber(subs);
    //loj: 3/9/04 commented out. The subscribers are added in AddSubscriberToSandbox()
    //thePublisher->Subscribe(subs);
    return subs;
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
Subscriber* Moderator::GetSubscriber(const std::string &name)
{
    return theConfigManager->GetSubscriber(name);
}

// GmatCommand
//------------------------------------------------------------------------------
// GmatCommand* CreateCommand(const std::string &type,const std::string &name )
//------------------------------------------------------------------------------
GmatCommand* Moderator::CreateCommand(const std::string &type, const std::string &name)
{
    GmatCommand *cmd = theFactoryManager->CreateCommand(type, name);
    return cmd;
}

// Mission
//------------------------------------------------------------------------------
// bool LoadDefaultMission()
//------------------------------------------------------------------------------
bool Moderator::LoadDefaultMission()
{
    CreateDefaultMission();
    
    return true;
}

// Resource
//------------------------------------------------------------------------------
// bool ClearResource()
//------------------------------------------------------------------------------
bool Moderator::ClearResource()
{
    MessageInterface::ShowMessage("Moderator::ClearResource() entered\n");

    theConfigManager->RemoveAllItems();
    return true;
}

// Command Sequence
//------------------------------------------------------------------------------
// bool ClearCommandSeq(Integer sandboxNum = 1)
//------------------------------------------------------------------------------
bool Moderator::ClearCommandSeq(Integer sandboxNum)
{
    MessageInterface::ShowMessage("Moderator::ClearCommandSeq() entered\n");
    
    //djc: Maybe set to NULL if you plan to do something completely different from
    // the way GMAT acts from a script?  I think you want to do this, though:
    // commands[sandboxNum-1] = NULL;
    GmatCommand *cmd = commands[sandboxNum-1];
    DeleteCommand(cmd);
    
    //djc: if you plan on adding the gui commands to the sandbox next, using 
    // the same approach used when running a script.
    cmd = new NoOp; 
    
    return true;
}

//------------------------------------------------------------------------------
// bool AppendCommand(GmatCommand *cmd, Integer sandboxNum)
//------------------------------------------------------------------------------
bool Moderator::AppendCommand(GmatCommand *cmd, Integer sandboxNum)
{
    return commands[sandboxNum-1]->Append(cmd);
}

//------------------------------------------------------------------------------
// GmatCommand* AppendCommand(const std::string &type, const std::string &name,
//                        Integer sandboxNum)
//------------------------------------------------------------------------------
GmatCommand* Moderator::AppendCommand(const std::string &type, const std::string &name,
                                  Integer sandboxNum)
{
    bool status;
    GmatCommand *cmd = theFactoryManager->CreateCommand(type, name);

    if (cmd != NULL)
    {
        if (name != "")
            theConfigManager->AddCommand(cmd);
    
        status = commands[sandboxNum-1]->Append(cmd);
    }
    
    return cmd;
}

//------------------------------------------------------------------------------
// bool InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd, Integer sandboxNum)
//------------------------------------------------------------------------------
bool Moderator::InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd,
                              Integer sandboxNum)
{
    return commands[sandboxNum-1]->Insert(cmd, prevCmd);
}

//------------------------------------------------------------------------------
// GmatCommand* InsertCommand(const std::string &type, const std::string &currName,
//                        const std::string &prevName, Integer sandboxNum)
//------------------------------------------------------------------------------
GmatCommand* Moderator::InsertCommand(const std::string &type, const std::string &currName,
                                      const std::string &prevName, Integer sandboxNum)
{
//      bool status = false;
//      GmatCommand *currCmd = theFactoryManager->CreateCommand(type, currName);
//      GmatCommand *prevCmd = NULL;

//      if (currCmd != NULL)
//      {
//          if (currName != "")
//              theConfigManager->AddCommand(currCmd);
    
//          if (prevName != "")
//              prevCmd = theConfigManager->GetCommand(prevName);
    
//          status = commands[sandboxNum-1]->Insert(currCmd, prevCmd);
//      }
    
//      return currCmd;
    return NULL;
}

//------------------------------------------------------------------------------
// GmatCommand* DeleteCommand(GmatCommand *cmd, Integer sandboxNum)
//------------------------------------------------------------------------------
GmatCommand* Moderator::DeleteCommand(GmatCommand *cmd, Integer sandboxNum)
{
    return commands[sandboxNum-1]->Remove(cmd);
}

//------------------------------------------------------------------------------
// GmatCommand* GetNextCommand(Integer sandboxNum)
//------------------------------------------------------------------------------
GmatCommand* Moderator::GetNextCommand(Integer sandboxNum)
{
    //loj: 3/5/04 return commands[sandboxNum-1]->GetNext();
    return commands[sandboxNum-1];
}

// sandbox
//------------------------------------------------------------------------------
// void ClearAllSandboxes()
//------------------------------------------------------------------------------
void Moderator::ClearAllSandboxes()
{
    for (int i=0; i<Gmat::MAX_SANDBOX; i++)
        sandboxes[i]->Clear();
}

//------------------------------------------------------------------------------
// Integer RunMission(Integer sandboxNum, bool isFromGui = false)
//------------------------------------------------------------------------------
Integer Moderator::RunMission(Integer sandboxNum, bool isFromGui)
{
    Integer status = 0;

    // check sandbox number
    if (sandboxNum > 0 && sandboxNum <= Gmat::MAX_SANDBOX)
    {
        sandboxes[sandboxNum-1]->Clear();
    }
    else
    {
        status = -1;
        MessageInterface::PopupMessage(Gmat::ERROR_,
                                       "Invalid Sandbox number" + sandboxNum);
    }

    try
    {
        AddSolarSysToSandbox(sandboxNum-1);
        AddPublisherToSandbox(sandboxNum-1);
        AddSpacecraftToSandbox(sandboxNum-1);
        AddPropSetupToSandbox(sandboxNum-1);
        AddBurnToSandbox(sandboxNum-1);        
        AddSolverToSandbox(sandboxNum-1);        
        AddSubscriberToSandbox(sandboxNum-1);
        AddCommandToSandbox(sandboxNum-1);
        
        InitializeSandbox(sandboxNum-1);

        SetupRun(sandboxNum, isFromGui);
        //MessageInterface::ShowMessage("Moderator::RunMission() after SetupRun() \n");

        ExecuteSandbox(sandboxNum-1);
        //MessageInterface::ShowMessage("Moderator::RunMission() after ExecuteSandbox() \n");
    }
    catch (BaseException &e)
    {
        MessageInterface::PopupMessage(Gmat::ERROR_, e.GetMessage());
        // assign status
        // status = ?
    }
    catch (...)
    {
        // assign status
        throw;
    }

    return status;
}

// Script
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
bool Moderator::InterpretScript(const std::string &scriptFilename)
{
    MessageInterface::ShowMessage("Moderator::InterpretScript() entered\n"
                                  "file: " + scriptFilename + "\n");

    //loj: clear both resource and command sequence for B2
    ClearResource();
    ClearCommandSeq();
    
    try
    {
        return theScriptInterpreter->Interpret(scriptFilename);
    }
    catch (BaseException &e)
    {
        MessageInterface::PopupMessage(Gmat::ERROR_, e.GetMessage() +
                                       "\n Check type in the appropriate Factory");
        return false;
    }
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
bool Moderator::SaveScript(const std::string &scriptFilename)
{
    MessageInterface::ShowMessage("Moderator::SaveScript() entered\n"
                                  "file: " + scriptFilename + "\n");
    MessageInterface::PopupMessage(Gmat::INFO_, "The Script is saved to " + scriptFilename);

    try
    {
        return theScriptInterpreter->Build(scriptFilename);
    }
    catch (BaseException &e)
    {
        MessageInterface::PopupMessage(Gmat::ERROR_, e.GetMessage());
        return false;
    }
    
}

//------------------------------------------------------------------------------
// Integer RunScript(Integer sandboxNum, bool isFromGui)
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
Integer Moderator::RunScript(Integer sandboxNum, bool isFromGui)
{
    MessageInterface::ShowMessage("Moderator::RunScript() entered\n");
    return RunMission(sandboxNum, isFromGui);
}

//---------------------------------
//  private
//---------------------------------

// initialization
//------------------------------------------------------------------------------
// void CreateDefaultMission()
//------------------------------------------------------------------------------
void Moderator::CreateDefaultMission()
{
    MessageInterface::ShowMessage("========================================\n");
    MessageInterface::ShowMessage("Moderator creating default mission...\n");

    try
    {
        // SolarSystem
        theDefaultSolarSystem = new SolarSystem("DefaultSS");

        // Spacecraft
        Spacecraft *sc = CreateSpacecraft("Spacecraft", "DefaultSC");

        // PropSetup
        PropSetup *propSetup = CreateDefaultPropSetup("DefaultProp");

        // Parameters
        Parameter *currTime = CreateParameter("CurrA1MJD", "DefaultSC.CurrentTime");
        Parameter *elapsedSecs = CreateParameter("ElapsedSecs", "DefaultSC.ElapsedSecs");
        Parameter *cartX = CreateParameter("CartX", "DefaultSC.X");
        Parameter *cartY = CreateParameter("CartY", "DefaultSC.Y");
        Parameter *cartZ = CreateParameter("CartZ", "DefaultSC.Z");
        Parameter *cartVx = CreateParameter("CartVx", "DefaultSC.Vx");
        Parameter *cartVy = CreateParameter("CartVx", "DefaultSC.Vy");
        Parameter *cartVz = CreateParameter("CartVx", "DefaultSC.Vz");
    
        //Parameter *kepSma = CreateParameter("KepSma", "DefaultSC.SMA");
        //Parameter *kepEcc = CreateParameter("KepEcc", "DefaultSC.ECC");
        //Parameter *kepInc = CreateParameter("KepInc", "DefaultSC.INC");
        //Parameter *kepRaan = CreateParameter("KepRaan", "DefaultSC.RAAN");
        //Parameter *kepAop = CreateParameter("KepAop", "DefaultSC.AOP");
        //Parameter *kepTa = CreateParameter("KepTa", "DefaultSC.TA");
    
        currTime->AddObject(sc);
        elapsedSecs->AddObject(sc);
        cartX->AddObject(sc);
        cartY->AddObject(sc);
        cartZ->AddObject(sc);
        cartVx->AddObject(sc);
        cartVy->AddObject(sc);
        cartVz->AddObject(sc);

        currTime->SetDesc(currTime->GetName());
        elapsedSecs->SetDesc(elapsedSecs->GetName());
        cartX->SetDesc(cartX->GetName());
        cartY->SetDesc(cartY->GetName());
        cartZ->SetDesc(cartZ->GetName());
        cartVx->SetDesc(cartVx->GetName());
        cartVy->SetDesc(cartVy->GetName());
        cartVz->SetDesc(cartVz->GetName());

        //kepSma->AddObject(sc);
        //kepEcc->AddObject(sc);
        //kepInc->AddObject(sc);
        //kepRaan->AddObject(sc);
        //kepAop->AddObject(sc);
        //kepTa->AddObject(sc);
        
        // StopCondition
        //loj: 2/12/04 Propagate command knows "Duration" only
        //StopCondition *stopCond = CreateStopCondition("SingleValueStop", "StopOnElapsedSecs");
        StopCondition *stopCond = CreateStopCondition("SingleValueStop", "Duration");
        stopCond->AddParameter(elapsedSecs);
        stopCond->SetGoal(8640.0);

        // Subscribers
        // ReportFile
        Subscriber *sub = CreateSubscriber("ReportFile", "DefaultReportFile");
        sub->SetStringParameter(sub->GetParameterID("Filename"), "DefaultReportFile.txt");
        sub->Activate(false);
    
        // XyPlot
        sub = CreateSubscriber("XyPlot", "DefaultXyPlot");
        sub->SetStringParameter("IndVar", "DefaultSC.CurrentTime");
        sub->SetStringParameter("DepVar", "DefaultSC.X");
        sub->Activate(true);
    
        // OpenGlPlot
        sub = CreateSubscriber("OpenGlPlot", "DefaultOpenGl");
        sub->Activate(true);
    
        // Propagate Command
        GmatCommand *propCommand = CreateCommand("Propagate");
        propCommand->SetObject("DefaultSC", Gmat::SPACECRAFT);
        propCommand->SetObject("DefaultProp", Gmat::PROP_SETUP);    
        propCommand->SetObject(stopCond, Gmat::STOP_CONDITION);
        propCommand->SetSolarSystem(theDefaultSolarSystem);

        // Add propagate command
        AppendCommand(propCommand);
    }
    catch (BaseException &e)
    {
        MessageInterface::PopupMessage(Gmat::ERROR_,
                                       "Moderator::CreateDefaultMission() Error "
                                       "occurred during default mission creation. "
                                       "Default mission will not run");
    }
    
}

//------------------------------------------------------------------------------
// void SetupRun(Integer sandboxNum, bool isFromGui)
//------------------------------------------------------------------------------
void Moderator::SetupRun(Integer sandboxNum, bool isFromGui)
{
    MessageInterface::ShowMessage("========================================\n");
    MessageInterface::ShowMessage("Moderator setting up for run...\n");
    std::string objName;
    std::string objTypeName;

    //--------------------------------------------
    // get/set internal objects for parameters
    //--------------------------------------------
    if (isFromGui)
    {
        GmatBase *obj;
        StringArray objList;
    
        // for configured parameters use internal copy of Spacecraft
        StringArray &params = GetListOfConfiguredItems(Gmat::PARAMETER);
        Parameter *param;
        Spacecraft *sc;
    
        for (unsigned int i=0; i<params.size(); i++)
        {
            param = GetParameter(params[i]);
            objList = param->GetObjectTypeNames();
            for (unsigned int j=0; j<objList.size(); j++)
            {
                obj = param->GetObject(objList[j]);
                objName = obj->GetName(); 
                sc = sandboxes[sandboxNum-1]->GetSpacecraft(objName);
                param->SetObject(Gmat::SPACECRAFT, objName, sc);
                //MessageInterface::ShowMessage("Moderator::SetupRun() SetObject ParamName = %s, "
                //                              "ObjName = %s\n", param->GetName().c_str(),
                //                              objName.c_str());
            }
        }
    
        StringArray &stopconds = GetListOfConfiguredItems(Gmat::STOP_CONDITION);
        StopCondition *stopCond;
    
        for (unsigned int i=0; i<stopconds.size(); i++)
        {
            stopCond = GetStopCondition(stopconds[i]);
            objName = stopCond->GetName();
            //MessageInterface::ShowMessage("Moderator::SetupRun() %s:goal = %f\n",
            //                              objName.c_str(), stopCond->GetGoal());
        }    
    }
    
    //--------------------------------------------
    //create plot window
    //--------------------------------------------
    StringArray &subs = GetListOfConfiguredItems(Gmat::SUBSCRIBER);
    Subscriber *sub;
    
    for (unsigned int i=0; i<subs.size(); i++)
    {
        sub = GetSubscriber(subs[i]);
        objTypeName = sub->GetTypeName();
        objName = sub->GetName();
        //MessageInterface::ShowMessage("Moderator::SetupRun() objTypeName = %s, objName = %s\n",
        //                              objTypeName.c_str(), objName.c_str());
        sub->Initialize();
    }
}


// sandbox
//------------------------------------------------------------------------------
// void AddSolarSysToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddSolarSysToSandbox(Integer index)
{
    SolarSystem *solarSys = theConfigManager->GetSolarSystemInUse();
    sandboxes[index]->AddSolarSystem(solarSys);
}

//------------------------------------------------------------------------------
// void AddPublisherToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddPublisherToSandbox(Integer index)
{
    thePublisher->UnsubscribeAll(); //loj: 3/9/04 added
    sandboxes[index]->SetPublisher(thePublisher);
}

//------------------------------------------------------------------------------
// void AddSpacecraftToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddSpacecraftToSandbox(Integer index)
{
    Spacecraft *sc;
    StringArray scNames = theConfigManager->GetListOfItems(Gmat::SPACECRAFT);

    for (Integer i=0; i<(Integer)scNames.size(); i++)
    {
        sc = theConfigManager->GetSpacecraft(scNames[i]);
        sandboxes[index]->AddSpacecraft(sc);
    }
}

//------------------------------------------------------------------------------
// void AddPropSetupToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddPropSetupToSandbox(Integer index)
{
    PropSetup *propSetup;
    StringArray propSetupNames = theConfigManager->GetListOfItems(Gmat::PROP_SETUP);
    
    for (Integer i=0; i<(Integer)propSetupNames.size(); i++)
    {
        propSetup = theConfigManager->GetPropSetup(propSetupNames[i]);
        sandboxes[index]->AddPropSetup(propSetup);
    }
}

//------------------------------------------------------------------------------
// void AddBurnToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddBurnToSandbox(Integer index)
{
    Burn *burn;
    StringArray burnNames = theConfigManager->GetListOfItems(Gmat::BURN);
    
    for (Integer i=0; i<(Integer)burnNames.size(); i++)
    {
        burn = theConfigManager->GetBurn(burnNames[i]);
        sandboxes[index]->AddBurn(burn);
    }
}

//------------------------------------------------------------------------------
// void AddSolverToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddSolverToSandbox(Integer index)
{
    Solver *solver;
    StringArray solverNames = theConfigManager->GetListOfItems(Gmat::SOLVER);
    
    for (Integer i=0; i<(Integer)solverNames.size(); i++)
    {
        solver = theConfigManager->GetSolver(solverNames[i]);
        sandboxes[index]->AddSolver(solver);
    }
}

//------------------------------------------------------------------------------
// void AddSuscriberToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddSubscriberToSandbox(Integer index)
{
    Subscriber *subs;
    StringArray subsNames = theConfigManager->GetListOfItems(Gmat::SUBSCRIBER);
    for (Integer i=0; i<(Integer)subsNames.size(); i++)
    {
        subs = theConfigManager->GetSubscriber(subsNames[i]);
        sandboxes[index]->AddSubscriber(subs);
    }
}

//------------------------------------------------------------------------------
// void AddCommandToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddCommandToSandbox(Integer index)
{
    GmatCommand *cmd = commands[index]->GetNext();

    if (cmd != NULL)
    {
        sandboxes[index]->AddCommand(cmd);
    }
}

//------------------------------------------------------------------------------
// void InitializeSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::InitializeSandbox(Integer index)
{
    sandboxes[index]->Initialize();
}

//------------------------------------------------------------------------------
// void ExecuteSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::ExecuteSandbox(Integer index)
{
    sandboxes[index]->Execute();
}

//---------------------------------
// private
//---------------------------------

//------------------------------------------------------------------------------
// Moderator()
//------------------------------------------------------------------------------
Moderator::Moderator()
{
    isInitialized = false;
    IsSlpAlreadyInUse = false;
    theDefaultSolarSystem = NULL;
    theDefaultSlpFile = NULL;
    theFactoryManager = FactoryManager::Instance();
    theConfigManager = ConfigManager::Instance();
    sandboxes.reserve(Gmat::MAX_SANDBOX);
    commands.reserve(Gmat::MAX_SANDBOX);

    // create at least 1 Sandbox and Command
    sandboxes.push_back(new Sandbox());
    commands.push_back(new NoOp());
}

//------------------------------------------------------------------------------
// ~Moderator()
//------------------------------------------------------------------------------
Moderator::~Moderator()
{
/*   if (instance != NULL)
//      delete instance;
//   if (theConfigManager != NULL)
//      delete theConfigManager;
   if (theFactoryManager != NULL)
      delete theFactoryManager;
   if (theGuiInterpreter != NULL)
      delete theGuiInterpreter;
//   if (theScriptInterpreter != NULL)
//      delete theScriptInterpreter;
   
   if (thePublisher != NULL)
      delete thePublisher;
   if (theCommandFactory != NULL)
      delete theCommandFactory;
   if (theForceModelFactory != NULL)
      delete theForceModelFactory;
   if (thePhysicalModelFactory != NULL)
      delete thePhysicalModelFactory;
   if (thePropSetupFactory != NULL)
      delete thePropSetupFactory;
   if (thePropagatorFactory != NULL)
      delete thePropagatorFactory;
   if (theSpacecraftFactory != NULL)
      delete theSpacecraftFactory;
   if (theStopConditionFactory != NULL)
      delete theStopConditionFactory;
   if (theSubscriberFactory != NULL)
      delete theSubscriberFactory;
*/
}


