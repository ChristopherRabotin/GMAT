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
#include "MessageInterface.hpp"

//#define DEBUG_RUN 1
//#define DEBUG_CREATE_RESOURCE 1
//#define DEBUG_DEFAULT_MISSION 1
//#define DEBUG_PLANETARY_FILE 1
//#define DEBUG_MULTI_STOP 1
//#define DEBUG_USER_INTERRUPT 1
//#define DEBUG_ACTION_REMOVE 1

//---------------------------------
// static data
//---------------------------------
Moderator* Moderator::instance = NULL;
GuiInterpreter* Moderator::theGuiInterpreter = NULL;
ScriptInterpreter* Moderator::theScriptInterpreter = NULL;

const std::string
Moderator::PLANETARY_SOURCE_STRING[PlanetaryFileCount] =
{
   "SLP",
   "DE200",
   //"DE202", //not supported
   "DE405",
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
   try
   {
      if (!isInitialized)
      {
         MessageInterface::ShowMessage("Moderator is creating core engine ...\n");
        
         // Create interpreters and managers
         theGuiInterpreter = GuiInterpreter::Instance();
         theScriptInterpreter = ScriptInterpreter::Instance();
         theFactoryManager = FactoryManager::Instance();
         theConfigManager = ConfigManager::Instance();
         theFileManager = FileManager::Instance();
        
         // Create publisher
         thePublisher = Publisher::Instance();
        
         // Create factories
         theAtmosphereFactory = new AtmosphereFactory(); //loj: 9/14/04 - added
         theBurnFactory = new BurnFactory();
         theCommandFactory = new CommandFactory();
         theForceModelFactory = new ForceModelFactory();
         theFunctionFactory = new FunctionFactory(); //loj: 9/27/04 - added
         theParameterFactory = new ParameterFactory();
         thePhysicalModelFactory = new PhysicalModelFactory();
         thePropSetupFactory = new PropSetupFactory();
         thePropagatorFactory = new PropagatorFactory();
         theSpacecraftFactory = new SpacecraftFactory();
         theStopConditionFactory = new StopConditionFactory();
         theSubscriberFactory = new SubscriberFactory();
         theSolverFactory = new SolverFactory();

         // Register factories
         theFactoryManager->RegisterFactory(theAtmosphereFactory);
         theFactoryManager->RegisterFactory(theBurnFactory);
         theFactoryManager->RegisterFactory(theCommandFactory);
         theFactoryManager->RegisterFactory(theForceModelFactory);
         theFactoryManager->RegisterFactory(theFunctionFactory);
         theFactoryManager->RegisterFactory(theParameterFactory);
         theFactoryManager->RegisterFactory(thePhysicalModelFactory);
         theFactoryManager->RegisterFactory(thePropSetupFactory);
         theFactoryManager->RegisterFactory(thePropagatorFactory);
         theFactoryManager->RegisterFactory(theSpacecraftFactory);
         theFactoryManager->RegisterFactory(theStopConditionFactory);
         theFactoryManager->RegisterFactory(theSubscriberFactory);
         theFactoryManager->RegisterFactory(theSolverFactory);

         // Create default SolarSystem
         theDefaultSolarSystem = new SolarSystem("DefaultSolarSystem");
         //theDefaultSolarSystem = CreateSolarSystem("DefaultSolarSystem");
         //SetSolarSystemInUse("DefaultSolarSystem");
         //MessageInterface::ShowMessage
         //   ("Moderator::Initialize() theDefaultSolarSystem created\n");
         
         // Read startup file
         theFileManager->ReadStartupFile();
         InitializePlanetarySource();
            
         if (fromGui)
         {
            CreateDefaultMission();
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::WARNING_,
                                     "Error occured during initialization: " +
                                     e.GetMessage());
   }
   catch (...)
   {
      MessageInterface::PopupMessage(Gmat::WARNING_,
                                     "Unknown Error occured during initialization");
   }
    
   MessageInterface::ShowMessage("Moderator successfully created core engine\n");
   isInitialized = true;
   return isInitialized;
}

//------------------------------------------------------------------------------
// void SetRunReady(bool flag = false)
//------------------------------------------------------------------------------
void Moderator::SetRunReady(bool flag)
{
   isRunReady = flag;
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
      return GmatBase::OBJECT_TYPE_STRING[type - Gmat::SPACECRAFT];
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
// SpaceObject* CreateSpacecraft(const std::string &type, const std::string &name)
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
SpaceObject* Moderator::CreateSpacecraft(const std::string &type, const std::string &name)
{
#if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateSpacecraft() type = %s, name = %s\n",
       type.c_str(), name.c_str());
#endif

   if (GetSpacecraft(name) == NULL)
   {
      SpaceObject *sc = theFactoryManager->CreateSpacecraft(type, name);

      if (sc == NULL)
      {
         MessageInterface::ShowMessage
            ("Moderator::CreateSpacecraft() Error Creating %s.  Make sure "
             "SpacecraftFactory is registered and has correct type. \n", type.c_str());
         
         throw GmatBaseException("Error Creating Spacecraft");
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
   else
   {
#if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateSpacecraft() Unable to create Spacecraft "
          "name: %s already exist\n", name.c_str());
#endif
      return GetSpacecraft(name);
   }
}

//------------------------------------------------------------------------------
// SpaceObject* GetSpacecraft(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a spacecraft object pointer by given name and add to configuration.
 *
 * @param <name> object name
 *
 * @return a SpaceObject object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
SpaceObject* Moderator::GetSpacecraft(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetSpacecraft(name);
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
Propagator* Moderator::CreatePropagator(const std::string &type,
                                        const std::string &name)
{
#if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreatePropagator() type = %s, name = %s\n",
       type.c_str(), name.c_str());
#endif
   
   if (GetPropagator(name) == NULL)
   {
      Propagator *prop = theFactoryManager->CreatePropagator(type, name);
    
      if (prop ==  NULL)
      {
         MessageInterface::ShowMessage
            ("Moderator::CreatePropagator() Error Creating %s.  Make sure "
             "PropagatorFactory is registered and has correct type. \n", type.c_str());
         
         throw GmatBaseException("Error Creating Propagator");
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
   else
   {
#if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreatePropagator() Unable to create Propagator "
          "name: %s already exist\n", name.c_str());
#endif
      return GetPropagator(name);
   }
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
   if (name == "")
      return NULL;
   else
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
   if (GetPhysicalModel(name) == NULL)
   {
      PhysicalModel *physicalModel = theFactoryManager->CreatePhysicalModel(type, name);
    
      if (physicalModel ==  NULL)
      {
         MessageInterface::ShowMessage
            ("Moderator::CreatePhysicalModel() Error Creating %s.  Make sure "
             "PhysicalModelFactory is registered and has correct type. \n", type.c_str());

         throw GmatBaseException("Error Creating PhysicalModel");
      }
    
      // Manage it if it is a named PhysicalModel
      try
      {
         if (physicalModel->GetName() != "")
            theConfigManager->AddPhysicalModel(physicalModel);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreatePhysicalModel()\n" +
                                       e.GetMessage());
      }
        
      return physicalModel;
   }
   else
   {
#if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreatePhysicalModel() Unable to create PhysicalModel "
          "name: %s already exist\n", name.c_str());
#endif
      return GetPhysicalModel(name);
   }
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
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetPhysicalModel(name);
}

// AtmosphereModel
//------------------------------------------------------------------------------
// AtmosphereModel* CreateAtmosphereModel(const std::string &type,
//                                        const std::string &name,
//                                        const std::string &body = "Earth")
//------------------------------------------------------------------------------
/**
 * Creates an atmosphere model object by given type and name and add to configuration.
 *
 * @param <type> object type
 * @param <name> object name
 * @param <body> the body for which the atmosphere model is requested
 *
 * @return a atmosphereModel object pointer
 */
//------------------------------------------------------------------------------
AtmosphereModel* Moderator::CreateAtmosphereModel(const std::string &type,
                                                  const std::string &name,
                                                  const std::string &body)
{
#if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateAtmosphereModel() entered: type = " +
       type + ", name = " + name + "\n");
#endif

   // if AtmosphereModel name doesn't exist, create AtmosphereModel
   if (GetAtmosphereModel(name) == NULL)
   {
      AtmosphereModel *atmosphereModel =
         theFactoryManager->CreateAtmosphereModel(type, body, name);

      if (atmosphereModel ==  NULL)
      {
         MessageInterface::ShowMessage
            ("Moderator::CreateAtmosphereModel() Error Creating %s.  Make sure "
             "AtmosphereModelFactory is registered and has correct type. \n", type.c_str());

         throw GmatBaseException("Error Creating AtmosphereModel: " + type);
      }
    
      // Manage it if it is a named AtmosphereModel
      try
      {
         if (atmosphereModel->GetName() != "")
            theConfigManager->AddAtmosphereModel(atmosphereModel);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage
            ("Moderator::CreateAtmosphereModel()\n" + e.GetMessage());
      }
    
      return atmosphereModel;
   }
   else
   {
#if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateAtmosphereModel() Unable to create AtmosphereModel "
          "name: %s already exist\n", name.c_str());
#endif
      return GetAtmosphereModel(name);
   }
}

//------------------------------------------------------------------------------
// AtmosphereModel* GetAtmosphereModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves an atmosphere model object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a AtmosphereModel pointer, return null if name not found
 */
//------------------------------------------------------------------------------
AtmosphereModel* Moderator::GetAtmosphereModel(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetAtmosphereModel(name);
}

// Burn
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
#if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("Moderator::CreateBurn() entered: type = " +
                                 type + ", name = " + name + "\n");
#endif

   // if Burn name doesn't exist, create Burn -- loj: 6/30/04
   if (GetBurn(name) == NULL)
   {
      Burn *burn = theFactoryManager->CreateBurn(type, name);

      if (burn ==  NULL)
      {
         MessageInterface::ShowMessage
            ("Moderator::CreateBurn() Error Creating %s.  Make sure "
             "BurnFactory is registered and has correct type. \n", type.c_str());

         throw GmatBaseException("Error Creating Burn: " + type);
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
   else
   {
#if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateBurn() Unable to create Burn "
          "name: %s already exist\n", name.c_str());
#endif
      return GetBurn(name);
   }
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
   if (name == "")
      return NULL;
   else
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
Parameter* Moderator::CreateParameter(const std::string &type,
                                      const std::string &name)
{
#if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("Moderator::CreateParameter() type = %s, "
                                 "name = %s\n", type.c_str(), name.c_str());
#endif
   
   // if Parameter name doesn't exist, create Parameter -- loj: 6/30/04
   if (GetParameter(name) == NULL)
   {
      Parameter *parameter = theFactoryManager->CreateParameter(type, name);

      if (parameter == NULL)
      {
         MessageInterface::ShowMessage
            ("Moderator::CreateParameter() Error Creating %s.  Make sure "
             "ParameterFactory is registered and has correct type. \n", type.c_str());

         throw GmatBaseException("Error Creating Parameter: " + type);
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
   else
   {
#if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateParameter() Unable to create Parameter "
          "name: %s already exist\n", name.c_str());
#endif
      return GetParameter(name);
   }
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
   if (name == "")
      return NULL;
   else
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
      MessageInterface::ShowMessage
         ("Moderator::CreateForceModel() Error Creating %s.  Make sure "
          "ForceModelFactory is registered and has correct type. \n", name.c_str());

      throw GmatBaseException("Error Creating ForceModel");
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
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetForceModel(name);
}

//------------------------------------------------------------------------------
// bool AddToForceModel(const std::string &forceModelName,
//                      const std::string &forceName)
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
// StopCondition* CreateStopCondition(const std::string &type,
//                                    const std::string &name)
//------------------------------------------------------------------------------
StopCondition* Moderator::CreateStopCondition(const std::string &type,
                                              const std::string &name)
{
#if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("Moderator::CreateStopCondition() type = %s, "
                                 "name = %s\n", type.c_str(), name.c_str());
#endif
   
   StopCondition *stopCond = theFactoryManager->CreateStopCondition(type, name);
   
#if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateStopCondition() *stopCond type = %s\n",
       stopCond->GetTypeName().c_str());
#endif
    
   if (stopCond ==  NULL)
   {
      MessageInterface::ShowMessage
         ("Moderator::CreateStopCondition() Error Creating %s.  Make sure "
          "StopConditionFactory is registered and has correct type. \n", type.c_str());

      throw GmatBaseException("Error Creating StopCondition: " + type);
   }
   
   return stopCond;
}

//------------------------------------------------------------------------------
// StopCondition* GetStopCondition(const std::string &name)
//------------------------------------------------------------------------------
StopCondition* Moderator::GetStopCondition(const std::string &name)
{
   if (name == "")
      return NULL;
   else
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
#if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("Moderator::CreateSolver() type = %s, "
                                 "name = %s\n", type.c_str(), name.c_str());
#endif

   if (GetSolver(name) == NULL)
   {
      Solver *solver = theFactoryManager->CreateSolver(type, name);

      if (solver == NULL)
      {
         MessageInterface::ShowMessage
            ("Moderator::CreateSolver() Error Creating %s.  Make sure "
             "SolverFactory is registered and has correct type. \n", type.c_str());

         throw GmatBaseException("Error Creating Solver: " + type);
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
   else
   {
#if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateSolver() Unable to create Solver "
          "name: %s already exist\n", name.c_str());
#endif
      return GetSolver(name);
   }
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
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetSolver(name);
}

// PropSetup
//------------------------------------------------------------------------------
// PropSetup* CreateDefaultPropSetup(const std::string &name)
//------------------------------------------------------------------------------
PropSetup* Moderator::CreateDefaultPropSetup(const std::string &name)
{
#if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("====================\n");
   MessageInterface::ShowMessage("Moderator::CreateDefaultPropSetup() name=%s\n",
                                 name.c_str());
#endif
   
   //loj: 5/11/04 since PropSetup creates default Integrator(RungeKutta89)
   // and default force (PointMassForce body=Eargh)
   PropSetup *propSetup = CreatePropSetup(name);    
   return propSetup;
}

//------------------------------------------------------------------------------
// PropSetup* CreatePropSetup(const std::string &name,
//                            const std::string &propagatorName = "",
//                            const std::string &forceModelName = "")
//------------------------------------------------------------------------------
PropSetup* Moderator::CreatePropSetup(const std::string &name,
                                      const std::string &propagatorName,
                                      const std::string &forceModelName)
{
#if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("====================\n");
   MessageInterface::ShowMessage("Moderator::CreatePropSetup() name=%s\n",
                                 name.c_str());
#endif

   if (GetPropSetup(name) == NULL)
   {
      Propagator *prop = theConfigManager->GetPropagator(propagatorName);
      ForceModel *fm = theConfigManager->GetForceModel(forceModelName);
   
      PropSetup *propSetup = theFactoryManager->CreatePropSetup(name);
   
      if (prop)
         propSetup->SetPropagator(prop);
   
      if (fm)
         propSetup->SetForceModel(fm);

      if (name != "")
         theConfigManager->AddPropSetup(propSetup);
   
      return propSetup;
   }
   else
   {
#if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreatePropSetup() Unable to create PropSetup "
          "name: %s already exist\n", name.c_str());
#endif
      return GetPropSetup(name);
   }
}

//------------------------------------------------------------------------------
// PropSetup* GetPropSetup(const std::string &name)
//------------------------------------------------------------------------------
PropSetup* Moderator::GetPropSetup(const std::string &name)
{
   if (name == "")
      return NULL;
   else
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
 * @return a CelestialBody object pointer
 */
//------------------------------------------------------------------------------
CelestialBody* Moderator::CreateCelestialBody(const std::string &type,
                                              const std::string &name)
{
   if (GetCelestialBody(name) == NULL)
   {
      CelestialBody *body = theFactoryManager->CreateCelestialBody(type, name);
    
      if (body == NULL)
      {
         MessageInterface::ShowMessage
            ("Moderator::CreateCelestialBody() Error Creating %s.  Make sure "
             "CelestialBodyFactory is registered and has correct type. \n", type.c_str());

         throw GmatBaseException("Error Creating CelestialBody: " + type);
      }
    
      // Manage it if it is a named body
      //loj: Do we want to add bodies that are not in the default solar system?
      try
      {
         if (body->GetName() != "")
            theConfigManager->AddCelestialBody(body);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateCelestialBody()\n" +
                                       e.GetMessage());
      }
    
      return body;
   }
   else
   {
#if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateCelestialBody() Unable to create CelestialBody "
          "name: %s already exist\n", name.c_str());
#endif
      return GetCelestialBody(name);
   }
}

//------------------------------------------------------------------------------
// CelestialBody* GetCelestialBody(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a celestial body object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a CelestialBody object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
CelestialBody* Moderator::GetCelestialBody(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetCelestialBody(name);
}

//------------------------------------------------------------------------------
// Interpolator* CreateInterpolator(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a Interpolator object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a Interpolator object pointer
 */
//------------------------------------------------------------------------------
Interpolator* Moderator::CreateInterpolator(const std::string &type,
                                            const std::string &name)
{

   //loj: 3/22/04 theFactoryManager->CreateInterpolator() not implemented
   return NULL;
}

//------------------------------------------------------------------------------
// Interpolator* GetInterpolator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Interpolator object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a Interpolator object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Interpolator* Moderator::GetInterpolator(const std::string &name)
{
   return NULL;
}

//------------------------------------------------------------------------------
// RefFrame* CreateRefFrame(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a celestial body object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a RefFrame object pointer
 */
//------------------------------------------------------------------------------
RefFrame* Moderator::CreateRefFrame(const std::string &type,
                                    const std::string &name)
{
   //loj: 3/22/04 theFactoryManager->CreateRefFrame() not implemented
   return NULL;
}

//------------------------------------------------------------------------------
// RefFrame* GetRefFrame(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a celestial body object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a RefFrame object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
RefFrame* Moderator::GetRefFrame(const std::string &name)
{
   return NULL;
}

// Subscriber
//loj: 6/21/04 added createDefault
//------------------------------------------------------------------------------
// Subscriber* CreateSubscriber(const std::string &type, const std::string &name,
//                              const std::string &fileName = "",
//                              bool createDefault = false)
//------------------------------------------------------------------------------
/**
 * Creates a subscriber object by given type and name if not already created.
 *
 * @param <type> object type
 * @param <name> object name
 * @param <fileName> file name if used
 * @param <createDefalut> create default Subscriber by setting ref. object if true
 *
 * @return a subscriber object pointer
 */
//------------------------------------------------------------------------------
Subscriber* Moderator::CreateSubscriber(const std::string &type,
                                        const std::string &name,
                                        const std::string &fileName,
                                        bool createDefault)
{
#if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("Moderator::CreateSubscriber() entered: type = " +
                                 type + ", name = " + name + "\n");
#endif

   if (GetSubscriber(name) == NULL)
   {
      Subscriber *sub = theFactoryManager->CreateSubscriber(type, name, fileName);

      if (sub == NULL)
      {
         MessageInterface::ShowMessage
            ("Moderator::CreateSubscriber() Error Creating %s.  Make sure "
             "SubscriberFactory is registered and has correct type. \n", type.c_str());

         throw GmatBaseException("Error Creating Subscriber:" + type);
      }

      try
      {
         if (sub->GetName() != "")
            theConfigManager->AddSubscriber(sub);
      
#if DEBUG_CREATE_RESOURCE
         MessageInterface::ShowMessage
            ("Moderator::CreateSubscriber() Creating default subscriber...\n");
#endif
         
         if (createDefault)
         {
            if (type == "OpenGlPlot")
            {
               // add default spacecraft to OpenGlPlot (loj: 9/28/04 added index)
               sub->SetStringParameter("Add", GetDefaultSpacecraft()->GetName(), 0); 
            }
            else if (type == "XyPlot")
            {
               // add default x,y parameter to XyPlot (loj: 9/29/04 added index)
               sub->SetStringParameter("IndVar", GetDefaultX()->GetName());
               sub->SetStringParameter("Add", GetDefaultY()->GetName(), 0);
               sub->Activate(true);
            }
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateSubscriber()\n" +
                                       e.GetMessage());
      }
   
      return sub;
   }
   else
   {
#if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateSubscriber() Unable to create Subscriber "
          "name: %s already exist\n", name.c_str());
#endif
      return GetSubscriber(name);
   }
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
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetSubscriber(name);
}

// Function
//------------------------------------------------------------------------------
// Function* CreateFunction(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a celestial body object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a Function object pointer
 */
//------------------------------------------------------------------------------
Function* Moderator::CreateFunction(const std::string &type,
                                    const std::string &name)
{
#if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("Moderator::CreateFunction() entered: type = " +
                                 type + ", name = " + name + "\n");
#endif
   if (GetFunction(name) == NULL)
   {
      Function *function = theFactoryManager->CreateFunction(type, name);

      if (function == NULL)
      {
         MessageInterface::ShowMessage
            ("Moderator::CreateFunction() Error Creating %s.  Make sure "
             "FunctionFactory is registered and has correct type. \n", type.c_str());

         throw GmatBaseException("Error Creating Function:" + type);
      }

      // Manage it if it is a named function
      try
      {
         if (function->GetName() != "")
            theConfigManager->AddFunction(function);
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("Moderator::CreateFunction()\n" +
                                       e.GetMessage());
      }
   
      return function;
   }
   else
   {
#if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateFunction() Unable to create Function "
          "name: %s already exist\n", name.c_str());
#endif
      return GetFunction(name);
   }
}

//------------------------------------------------------------------------------
// Function* GetFunction(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a celestial body object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a Function object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Function* Moderator::GetFunction(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetFunction(name);
}

// GmatCommand
//------------------------------------------------------------------------------
// GmatCommand* CreateCommand(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
GmatCommand* Moderator::CreateCommand(const std::string &type,
                                      const std::string &name)
{
#if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("Moderator::CreateCommand() entered: type = " +
                                 type + ", name = " + name + "\n");
#endif

   GmatCommand *cmd = theFactoryManager->CreateCommand(type, name);
   return cmd;
}

//------------------------------------------------------------------------------
// GmatCommand* CreateDefaultCommand(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
GmatCommand* Moderator::CreateDefaultCommand(const std::string &type,
                                             const std::string &name)
{
#if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateDefaultCommand() entered: type = " +
       type + ", name = " + name + "\n");
#endif

   GmatCommand *cmd = theFactoryManager->CreateCommand(type, name);
   Integer id;
   
   if (type == "Propagate")
   {
      //loj: 10/4/04 switched PROP_SETUP AND SPACECRAFT
      cmd->SetObject(GetDefaultPropSetup()->GetName(), Gmat::PROP_SETUP);
      cmd->SetObject(GetDefaultSpacecraft()->GetName(), Gmat::SPACECRAFT);
      cmd->SetObject(CreateDefaultStopCondition(), Gmat::STOP_CONDITION);
      cmd->SetSolarSystem(theDefaultSolarSystem);
   }
   else if (type == "Maneuver")
   {
      // set burn
      id = cmd->GetParameterID("Burn");
      cmd->SetStringParameter(id, GetDefaultBurn()->GetName());

      // set spacecraft
      id = cmd->GetParameterID("Spacecraft");
      cmd->SetStringParameter(id, GetDefaultSpacecraft()->GetName());
   }
   else if (type == "Vary") //loj: 10/6/04 added
   {
      // set solver
      Solver *solver = CreateSolver("DifferentialCorrector", "DefaultDC");
      id = cmd->GetParameterID("TargeterName");
      cmd->SetStringParameter(id, solver->GetName());

      // set variable parameter
      id = cmd->GetParameterID("Variable");
      //----------------------------------------------------
      //@note loj: 10/6/04
      // I should create a burn parameter of V.
      // But use temp code for now.
      //----------------------------------------------------
      
      cmd->SetStringParameter(id, GetDefaultBurn()->GetName() + ".V");
      cmd->SetStringParameter(id, GetDefaultBurn()->GetName() + ".N");
      
      id = cmd->GetParameterID("InitialValue");
      cmd->SetRealParameter(id, 0.5);
      cmd->SetRealParameter(id, 0.5);
      
      id = cmd->GetParameterID("Perturbation");
      cmd->SetRealParameter(id, 0.000001);
      cmd->SetRealParameter(id, 0.00001);

      id = cmd->GetParameterID("MinimumValue");
      cmd->SetRealParameter(id, -9.000e30);
      cmd->SetRealParameter(id, -9.000e30);
      
      id = cmd->GetParameterID("MaximumValue");
      cmd->SetRealParameter(id, 9.000e30);
      cmd->SetRealParameter(id, 9.000e30);
      
      id = cmd->GetParameterID("MaximumChange");
      cmd->SetRealParameter(id, 9.000e30);
      cmd->SetRealParameter(id, 9.000e30);
   }
   else if (type == "Achieve") //loj: 10/6/04 added
   {
      // set solver
      Solver *solver = CreateSolver("DifferentialCorrector", "DefaultDC");
      id = cmd->GetParameterID("TargeterName");
      cmd->SetStringParameter(id, solver->GetName());

      // set goal parameter
      id = cmd->GetParameterID("Goal");
      cmd->SetStringParameter(id, GetDefaultSpacecraft()->GetName() + ".SMA");
      cmd->SetStringParameter(id, GetDefaultSpacecraft()->GetName() + ".INC");

      id = cmd->GetParameterID("GoalValue");
      cmd->SetRealParameter(id, 8500.0);
      cmd->SetRealParameter(id, 30.0);

      id = cmd->GetParameterID("Tolerance");
      cmd->SetRealParameter(id, 0.1);
      cmd->SetRealParameter(id, 0.1);
   }
   
   return cmd;
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
   //return theConfigManager->GetSolarSystemInUse();
   return theDefaultSolarSystem;
}

//------------------------------------------------------------------------------
// bool SetSolarSystemInUse(const std::string &name)
//------------------------------------------------------------------------------
bool Moderator::SetSolarSystemInUse(const std::string &name)
{
   return theConfigManager->SetSolarSystemInUse(name);
}

//Planetary files
//------------------------------------------------------------------------------
// StringArray& GetPlanetaryFileTypes()
//------------------------------------------------------------------------------
/**
 * @return a planetary source types
 */
//------------------------------------------------------------------------------
StringArray& Moderator::GetPlanetaryFileTypes()
{
   return thePlanetaryFileTypes;
}

//------------------------------------------------------------------------------
// StringArray& GetPlanetaryFileNames()
//------------------------------------------------------------------------------
/**
 * @return a planetary source file names
 */
//------------------------------------------------------------------------------
StringArray& Moderator::GetPlanetaryFileNames()
{
   return thePlanetaryFileNames;
}

//------------------------------------------------------------------------------
// StringArray& GetPlanetaryFileTypesInUse()
//------------------------------------------------------------------------------
/**
 * @return a planetary source types in use
 */
//------------------------------------------------------------------------------
StringArray& Moderator::GetPlanetaryFileTypesInUse()
{
   theTempFileList.clear();
   for (unsigned int i=0; i<thePlanetaryFileTypesInUse.size(); i++)
      theTempFileList.push_back(thePlanetaryFileTypesInUse[i]);

   return theTempFileList;
}

//------------------------------------------------------------------------------
// std::string GetPlanetaryFileName(const std::string &fileType)
//------------------------------------------------------------------------------
std::string Moderator::GetPlanetaryFileName(const std::string &fileType)
{
   Integer id = GetPlanetaryFileId(fileType);

   if (id >= 0)
      return thePlanetaryFileNames[id];
   else
      return "Unknown File Type";
}

//------------------------------------------------------------------------------
// bool SetPlanetaryFileName(const std::string &fileType,
//                           const std::string &fileName)
//------------------------------------------------------------------------------
bool Moderator::SetPlanetaryFileName(const std::string &fileType,
                                     const std::string &fileName)
{
   bool status = false;
   Integer id = GetPlanetaryFileId(fileType);

   if (id >= 0)
   {
      thePlanetaryFileNames[id] = fileName;
      status = true;
   }
    
   return status;
}

// Potential field files
//------------------------------------------------------------------------------
// std::string GetPotentialFileName(const std::string &fileType)
//------------------------------------------------------------------------------
std::string Moderator::GetPotentialFileName(const std::string &fileType)
{
   if (fileType == "JGM2")
      return theFileManager->GetStringParameter("FULL_EARTH_JGM2_FILE");
   else if (fileType == "JGM3")
      return theFileManager->GetStringParameter("FULL_EARTH_JGM3_FILE");
   else
      return "Unknown Potential File Type:" + fileType;
}

//loj: 6/14/04 changed bool to Integer
//------------------------------------------------------------------------------
// Integer SetPlanetaryFileTypesInUse(const StringArray &fileTypes)
//------------------------------------------------------------------------------
/*
 * @param <fileTypes> list of file type in the priority order of use
 *
 * @return 0, if error setting any of planetary file in the list.
 *         1, if error setting first planetary file in the list, but set to
 *            next available file.
 *         2, if successfuly set to first planetary file in the list
 */
//------------------------------------------------------------------------------
Integer Moderator::SetPlanetaryFileTypesInUse(const StringArray &fileTypes)
{
#if DEBUG_PLANETARY_FILE
   MessageInterface::
      ShowMessage("Moderator::SetPlanetaryFileTypesInUse() num filetypes=%d\n",
                  fileTypes.size());
#endif
   
   bool status = false;
   Integer fileTypeInUse = -1;
   Integer retCode = 0;
   
   // update planetary file types
   if (&thePlanetaryFileTypesInUse != &fileTypes)
   {
#if DEBUG_PLANETARY_FILE
      MessageInterface::ShowMessage
         ("Moderator::SetPlanetaryFileTypesInUse() updating planetary source\n");
#endif
      thePlanetaryFileTypesInUse.clear();
    
      for (unsigned int i=0; i<fileTypes.size(); i++)
      {
         thePlanetaryFileTypesInUse.push_back(fileTypes[i]);
      }
   }

   theTempFileList.clear();
   for (unsigned int i=0; i<thePlanetaryFileTypesInUse.size(); i++)
      theTempFileList.push_back(thePlanetaryFileTypesInUse[i]);
   
   // create planetary ephem file
   for (unsigned int i=0; i<thePlanetaryFileTypesInUse.size(); i++)
   {
      if (thePlanetaryFileTypesInUse[i] == PLANETARY_SOURCE_STRING[SLP])
      {
         thePlanetarySourcePriority[SLP] = 0;
         status = CreateSlpFile(thePlanetaryFileNames[SLP]);
         if (status)
         {
            thePlanetarySourcePriority[SLP] = HIGHEST_PRIORITY - i;
            isPlanetaryFileInUse[SLP] = true;
            fileTypeInUse = SLP;
            break;
         }
      }
      else if (thePlanetaryFileTypesInUse[i] == PLANETARY_SOURCE_STRING[DE200])
      {
#if DEBUG_PLANETARY_FILE
         MessageInterface::
            ShowMessage("Moderator::SetPlanetaryFileTypesInUse() create DE200\n");
#endif
         thePlanetarySourcePriority[DE200] = 0;
         status = CreateDeFile(DE200, thePlanetaryFileNames[DE200]);
         if (status)
         {
            thePlanetarySourcePriority[DE200] = HIGHEST_PRIORITY - i;
            isPlanetaryFileInUse[DE200] = true;
            fileTypeInUse = DE200;
            break;
         }
      }
      //else if (thePlanetaryFileTypesInUse[i] == PLANETARY_SOURCE_STRING[DE202])
      //{
      //   MessageInterface::ShowMessage("Moderator::SetPlanetaryFileTypesInUse() create DE202\n");
      //   thePlanetarySourcePriority[DE202] = 0;
      //   status = CreateDeFile(DE202, thePlanetaryFileNames[DE202]);
      //   if (status)
      //   {
      //      thePlanetarySourcePriority[DE202] = HIGHEST_PRIORITY - i;
      //      isPlanetaryFileInUse[DE202] = true;
      //      fileTypeInUse = DE202;
      //      break;
      //   }
      //}
      else if (thePlanetaryFileTypesInUse[i] == PLANETARY_SOURCE_STRING[DE405])
      {
#if DEBUG_PLANETARY_FILE
         MessageInterface::
            ShowMessage("Moderator::SetPlanetaryFileTypesInUse() create DE405\n");
#endif
         thePlanetarySourcePriority[DE405] = 0;
         status = CreateDeFile(DE405, thePlanetaryFileNames[DE405]);
         if (status)
         {
            thePlanetarySourcePriority[DE405] = HIGHEST_PRIORITY - i;
            isPlanetaryFileInUse[DE405] = true;
            fileTypeInUse = DE405;
            break;
         }
      }
   }

   // set SolarSystem to use the file
   if (fileTypeInUse == -1)
   {
      MessageInterface::ShowMessage("Moderator::SetPlanetaryFileTypesInUse() NO "
                                    "Planetary file is set to use \n");
      retCode = 0;
   }
   else
   {
#if DEBUG_PLANETARY_FILE
      MessageInterface::
         ShowMessage("Moderator::SetPlanetaryFileTypesInUse() "
                     "Set Planetary file to use:%d\n", fileTypeInUse);
#endif
      switch (fileTypeInUse)
      {
      case SLP:
         if (theDefaultSolarSystem->SetSource(Gmat::SLP))
            if (theDefaultSolarSystem->SetSourceFile(theDefaultSlpFile))
               retCode = 1;
         break;
      case DE200:
         if (theDefaultSolarSystem->SetSource(Gmat::DE_200))
            if (theDefaultSolarSystem->SetSourceFile(theDefaultDeFile))
               retCode = 1;
         break;
         //          case DE202:
         //              if (theDefaultSolarSystem->SetSource(Gmat::DE_202))
         //                  if (theDefaultSolarSystem->SetSourceFile(theDefaultDeFile))
         //                      retCode = 1;
         //              break;
      case DE405:
         if (theDefaultSolarSystem->SetSource(Gmat::DE_405))
            if (theDefaultSolarSystem->SetSourceFile(theDefaultDeFile))
               retCode = 1;
         break;
      default:
         break;
      }
   }

   // if planetary file is set to first type in the list
   if (retCode == 1 && PLANETARY_SOURCE_STRING[fileTypeInUse] == fileTypes[0])
      retCode = 2;


   // if error setting given planetary file, re-arrange planetary file list
   if (retCode == 1)
   {      
      thePlanetaryFileTypesInUse.clear();

      for (unsigned int i=0; i<theTempFileList.size(); i++)
      {            
         if (theTempFileList[i] == PLANETARY_SOURCE_STRING[SLP])
         {
            if (thePlanetarySourcePriority[SLP] > 0)
               thePlanetaryFileTypesInUse.push_back(PLANETARY_SOURCE_STRING[SLP]);
         }
         else if (theTempFileList[i] == PLANETARY_SOURCE_STRING[DE200])
         {
            if (thePlanetarySourcePriority[DE200] > 0)
               thePlanetaryFileTypesInUse.push_back(PLANETARY_SOURCE_STRING[DE200]);
         }
         else if (theTempFileList[i] == PLANETARY_SOURCE_STRING[DE405])
         {
            if (thePlanetarySourcePriority[DE405] > 0)
               thePlanetaryFileTypesInUse.push_back(PLANETARY_SOURCE_STRING[DE405]);
         }
      }
      
#if DEBUG_PLANETARY_FILE
      for (unsigned int i=0; i<thePlanetaryFileTypesInUse.size(); i++)
      {
         MessageInterface::ShowMessage
            ("thePlanetaryFileTypesInUse[%d]=%s\n", i,
             thePlanetaryFileTypesInUse[i].c_str());
      }
#endif
   }
   
   //#if DEBUG_PLANETARY_FILE
   if (retCode > 0)
      MessageInterface::
      ShowMessage("Moderator::SetPlanetaryFileTypesInUse() Successfully "
                  "set Planetary file to use: %s\n",
                  PLANETARY_SOURCE_STRING[fileTypeInUse].c_str());
   //#endif
   return retCode;
}

//------------------------------------------------------------------------------
// Integer GetPlanetaryFileId(const std::string &fileType)
//------------------------------------------------------------------------------
Integer Moderator::GetPlanetaryFileId(const std::string &fileType)
{
   for (int i=0; i<PlanetaryFileCount; i++)
   {
      if (fileType == PLANETARY_SOURCE_STRING[i])
         return i;
   }
    
   return -1;
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

   StringArray &subs = GetListOfConfiguredItems(Gmat::SUBSCRIBER);
   Subscriber *sub;
   std::string objTypeName;
   std::string objName;

   for (unsigned int i=0; i<subs.size(); i++)
   {
      sub = GetSubscriber(subs[i]);
      objTypeName = sub->GetTypeName();
      objName = sub->GetName();
      MessageInterface::ShowMessage
         ("Moderator::ClearResource() objTypeName = %s, objName = %s\n",
          objTypeName.c_str(), objName.c_str());
   }

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
// Integer RunMission(Integer sandboxNum)
//------------------------------------------------------------------------------
Integer Moderator::RunMission(Integer sandboxNum)
{
   MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("Moderator::RunMission() entered\n");
   Integer status = 0;

   if (isRunReady)
   {
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
         AddFormationToSandbox(sandboxNum-1);
         AddForceModelToSandbox(sandboxNum-1);
         AddPropagatorToSandbox(sandboxNum-1);
         AddPropSetupToSandbox(sandboxNum-1);
         AddBurnToSandbox(sandboxNum-1);        
         AddSolverToSandbox(sandboxNum-1);
         
         // Add Subscriber after Publisher. AddPublisherToSandbox() clears subscribers
         AddSubscriberToSandbox(sandboxNum-1); 
         AddParameterToSandbox(sandboxNum-1);
         AddCommandToSandbox(sandboxNum-1);
         
#if DEBUG_RUN
         MessageInterface::ShowMessage
            ("Moderator::RunMission() after AddCommandToSandbox() \n");
#endif
         InitializeSandbox(sandboxNum-1);
         
#if DEBUG_RUN
         MessageInterface::ShowMessage
            ("Moderator::RunMission() after InitializeSanbox() \n");
#endif

         runState = Gmat::RUNNING;
         ExecuteSandbox(sandboxNum-1);
         runState = Gmat::IDLE;
         
#if DEBUG_RUN
         MessageInterface::ShowMessage
            ("Moderator::RunMission() after ExecuteSandbox() \n");
#endif
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage
            ("Moderator::RunMission() %s\n", e.GetMessage().c_str());
         MessageInterface::PopupMessage(Gmat::ERROR_, e.GetMessage());
         // assign status
         status = -2;
      }
      catch (...)
      {
         // assign status
         throw;
      }
   }
   else
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Mission not Complete. Cannot Run Mission");
      status = -3;
   }

   //loj: 10/13/04 added run status
   if (status == 0)
      MessageInterface::ShowMessage("Mission ran successfully.\n");
   else
      MessageInterface::ShowMessage("*** Mission run failed.\n");
      
   return status;
}

//------------------------------------------------------------------------------
// Integer ChangeRunState(const std::string &state, Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Changes run state.
 *
 * @param <state> run state string ("Stop", "Pause", "Resume")
 * @param <snadobxNum> sandbox number
 *
 * @return a status code
 *    0 = successful, <0 = error (tbd)
 */
//------------------------------------------------------------------------------
Integer Moderator::ChangeRunState(const std::string &state, Integer sandboxNum)
{
#if DEBUG_USER_INTERRUPT
   MessageInterface::ShowMessage("Moderator::ChangeRunState() entered\n");
#endif
   
   if (state == "Stop")   
      runState = Gmat::IDLE;

   //else if (state == "Pause")  // Commented until build 4
   //   runState = Gmat::PAUSED;
   //
   //else if (state == "Resume") // Commented until build 4
   //   runState = Gmat::RUNNING;
   
   else
      ; // no action
   
   return 0;
}

//------------------------------------------------------------------------------
// Gmat::RunState GetUserInterrupt()
//------------------------------------------------------------------------------
/**
 * Checks to see if the user has requested that the run stop or pause.
 * 
 * This method is called by the Sandbox periodically during a run to determine
 * if the user has requested that the run terminate before the mission sequence
 * has finished executing.
 * 
 * @return The expected state of the system (RUNNING, PAUSED, or IDLE).
 */
//------------------------------------------------------------------------------
Gmat::RunState Moderator::GetUserInterrupt()
{
#if DEBUG_USER_INTERRUPT
   MessageInterface::ShowMessage("Moderator::GetUserInterrupt() entered\n");
#endif

   //loj: 8/17/04 added
   // give MainFrame input focus
   theGuiInterpreter->SetInputFocus();
   Gmat::RunState status = runState;
   
   //// Test code for the polling function -- stop on the 5th call
   //static Integer tester = 0;
   //++tester;
   //if (tester == 5)
   //   status = Gmat::IDLE;
   
   return status;
}

// Script
//------------------------------------------------------------------------------
// bool InterpretScript(const std::string &scriptFileName)
//------------------------------------------------------------------------------
/**
 * Creates objects from script file.
 *
 * @param <scriptFileName> input script file name
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::InterpretScript(const std::string &scriptFileName)
{
   bool status = false;
   isRunReady = false;
    
   MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("Moderator::InterpretScript() entered\n"
                                 "file: " + scriptFileName + "\n");

   //clear both resource and command sequence
   ClearResource();
   ClearCommandSeq();
   
   try
   {
      status = theScriptInterpreter->Interpret(scriptFileName);
      if (status)
      {
         MessageInterface::ShowMessage
            ("Moderator::InterpretScript() successfully interpreted the script\n");
         isRunReady = true;
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, e.GetMessage() +
          "\n Check Type in the appropriate Factory or parameter text");
      isRunReady = false;
   }

   return status;
}

//loj: 9/8/04 added
//------------------------------------------------------------------------------
// bool InterpretScript(std::istringstream *ss, bool clearObjs)
//------------------------------------------------------------------------------
/**
 * Creates objects from stringstream
 *
 * @param <ss> input istringstream
 * @param <clearObjs> clears objects and mission sequence if true
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::InterpretScript(std::istringstream *ss, bool clearObjs)
{
   bool status = false;
   isRunReady = false;
    
   MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("Moderator::InterpretScript(ss) entered\n");

   //clear both resource and command sequence
   if (clearObjs)
   {
      ClearResource();
      ClearCommandSeq();
   }
   
   try
   {
      theScriptInterpreter->SetInStream(ss);
      status = theScriptInterpreter->Interpret();
      if (status)
      {
         MessageInterface::ShowMessage
            ("Moderator::InterpretScript() successfully interpreted the script\n");
         isRunReady = true;
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, e.GetMessage() +
          "\n Check Type in the appropriate Factory or parameter text");
      isRunReady = false;
   }

   return status;
}

//------------------------------------------------------------------------------
// bool SaveScript(const std::string &scriptFileName)
//------------------------------------------------------------------------------
/**
 * Builds scripts from objects and write to a file.
 *
 * @param <scriptFileName> output script file name
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::SaveScript(const std::string &scriptFileName)
{
   MessageInterface::ShowMessage("Moderator::SaveScript() entered\n"
                                 "file: " + scriptFileName + "\n");
   MessageInterface::PopupMessage(Gmat::INFO_, "The Script is saved to " + scriptFileName);

   try
   {
      return theScriptInterpreter->Build(scriptFileName);
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetMessage());
      return false;
   }
}

//------------------------------------------------------------------------------
// Integer RunScript(Integer sandboxNum = 1)
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
Integer Moderator::RunScript(Integer sandboxNum)
{
   MessageInterface::ShowMessage("Moderator::RunScript() entered\n");
   return RunMission(sandboxNum);
}

//---------------------------------
//  private
//---------------------------------

// initialization
//------------------------------------------------------------------------------
// void InitializePlanetarySource()
//------------------------------------------------------------------------------
void Moderator::InitializePlanetarySource()
{
   MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("Moderator initializing planetary source...\n");

   // initialize planetary source
   for (int i=0; i<PlanetaryFileCount; i++)
   {
      thePlanetarySourcePriority[i] = 0;
      isPlanetaryFileInUse[i] = false;
   }

   // initialize in the order of enum data
   thePlanetaryFileTypes.push_back(PLANETARY_SOURCE_STRING[SLP]);
   thePlanetaryFileTypes.push_back(PLANETARY_SOURCE_STRING[DE200]);
   //thePlanetaryFileTypes.push_back(PLANETARY_SOURCE_STRING[DE202]); //loj: not supported yet
   thePlanetaryFileTypes.push_back(PLANETARY_SOURCE_STRING[DE405]);
    
   thePlanetaryFileNames.push_back(theFileManager->
                                   GetStringParameter("FULL_SLP_FILE"));
   thePlanetaryFileNames.push_back(theFileManager->
                                   GetStringParameter("FULL_DE200_FILE"));
   //      thePlanetaryFileNames.push_back(theFileManager->
   //                                      GetStringParameter("FULL_DE202_FILE")); //loj: not supported yet
   thePlanetaryFileNames.push_back(theFileManager->
                                   GetStringParameter("FULL_DE405_FILE"));

   // initialize planetary file types/names in use
   thePlanetaryFileTypesInUse.push_back(PLANETARY_SOURCE_STRING[SLP]);
   //thePlanetaryFileTypesInUse.push_back(PLANETARY_SOURCE_STRING[DE200]);
   SetPlanetaryFileTypesInUse(thePlanetaryFileTypesInUse);
}

//------------------------------------------------------------------------------
// void CreateDefaultMission()
//------------------------------------------------------------------------------
void Moderator::CreateDefaultMission()
{
   MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("Moderator creating default mission...\n");

   try
   {
      // Spacecraft
      CreateSpacecraft("Spacecraft", "DefaultSC");
#if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default Spacecraft created\n");
#endif
      // PropSetup
      CreateDefaultPropSetup("DefaultProp");
#if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default PropSetup created\n");
#endif

      // Burn
      GetDefaultBurn();
      
      // Time parameters
      CreateParameter("CurrA1MJD", "DefaultSC.CurrA1MJD"); //loj: 9/24/04 changed from CurrentTime
      CreateParameter("ElapsedSecs", "DefaultSC.ElapsedSecs");
      CreateParameter("ElapsedDays", "DefaultSC.ElapsedDays");

      // Cartesian parameters
      CreateParameter("X", "DefaultSC.X");
      CreateParameter("Y", "DefaultSC.Y");
      CreateParameter("Z", "DefaultSC.Z");
      CreateParameter("VX", "DefaultSC.VX");
      CreateParameter("VY", "DefaultSC.VY");
      CreateParameter("VZ", "DefaultSC.VZ");

      // Keplerian parameters
      CreateParameter("SMA", "DefaultSC.SMA");
      CreateParameter("ECC", "DefaultSC.ECC");
      CreateParameter("INC", "DefaultSC.INC");
      CreateParameter("RAAN", "DefaultSC.RAAN");
      CreateParameter("AOP", "DefaultSC.AOP");
      CreateParameter("TA", "DefaultSC.TA");
      CreateParameter("MA", "DefaultSC.MA");
      CreateParameter("MM", "DefaultSC.MM");

      // Orbital parameters
      CreateParameter("VelApoapsis", "DefaultSC.VelApoapsis");
      CreateParameter("VelPeriapsis", "DefaultSC.VelPeriapsis");
      CreateParameter("Apoapsis", "DefaultSC.Apoapsis");
      CreateParameter("Periapsis", "DefaultSC.Periapsis");

      // Spherical parameters
      CreateParameter("RMAG", "DefaultSC.RMAG");
      CreateParameter("RA", "DefaultSC.RA");
      CreateParameter("DEC", "DefaultSC.DEC");
      CreateParameter("VMAG", "DefaultSC.VMAG");
      CreateParameter("RAV", "DefaultSC.RAV");
      CreateParameter("DECV", "DefaultSC.DECV");

      // Angular parameters
      CreateParameter("SemilatusRectum", "DefaultSC.SemilatusRectum");

      // User variable (9/22/04 added)
      Parameter *var = CreateParameter("Variable", "DefaultSC.Xx2");
      var->SetStringParameter("Expression", "DefaultSC.X * 2.0");
      var->SetRefObjectName(Gmat::PARAMETER, "DefaultSC.X");
      
#if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default parameters created\n");
#endif   
      // Set parameter description and object name
      StringArray &params = GetListOfConfiguredItems(Gmat::PARAMETER);
      Parameter *param;
    
      for (unsigned int i=0; i<params.size(); i++)
      {
         param = GetParameter(params[i]);

         // need spacecraft if system parameter (loj: 9/22/04)
         if (param->GetKey() == Parameter::SYSTEM_PARAM)
         {
            //param->SetStringParameter("Description", param->GetName());
            //loj: 9/22/04 set user Expression from now on
            param->SetStringParameter("Expression", param->GetName());
            param->SetRefObjectName(Gmat::SPACECRAFT, "DefaultSC");
            //loj: 9/13/04 param->SetStringParameter("Object", "DefaultSC");
         }
      }
    
      // StopCondition
      StopCondition *stopOnElapsedSecs =
         CreateStopCondition("StopCondition", "StopOnDefaultSC.ElapsedSecs");
      stopOnElapsedSecs->SetStringParameter("EpochVar", "DefaultSC.CurrA1MJD");
      stopOnElapsedSecs->SetStringParameter("StopVar", "DefaultSC.ElapsedSecs");
      stopOnElapsedSecs->SetRealParameter("Goal", 8640.0);
#if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default StopCondition created\n");
#endif
      // Subscribers
      // ReportFile
      Subscriber *sub = CreateSubscriber("ReportFile", "DefaultReportFile");
      sub->SetStringParameter(sub->GetParameterID("Filename"), "DefaultReportFile.txt");
      sub->Activate(false);
    
      // XyPlot
      sub = CreateSubscriber("XyPlot", "DefaultXyPlot");
      sub->SetStringParameter("IndVar", "DefaultSC.CurrA1MJD");
      sub->SetStringParameter("Add", "DefaultSC.X", 0); //loj: 9/29/04 added index
#if DEBUG_ACTION_REMOVE
      sub->SetStringParameter("Add", "DefaultSC.Y", 1);
      sub->SetStringParameter("Add", "DefaultSC.Z", 2);
      sub->TakeAction("Remove", "DefaultSC.Y");
#endif
      sub->Activate(true);
      
      // OpenGlPlot
      sub = CreateSubscriber("OpenGlPlot", "DefaultOpenGl");
      sub->SetStringParameter("Add", "DefaultSC", 0); //loj: 9/28/04 added index
#if DEBUG_ACTION_REMOVE
      sub->SetStringParameter("Add", "Spacecraft1", 1);
      sub->TakeAction("Remove", "Spacecraft1");
#endif
      sub->Activate(true);
      
#if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default Subscribers created\n");
#endif
      
      // Propagate Command
      GmatCommand *propCommand = CreateCommand("Propagate");
      propCommand->SetObject("DefaultProp", Gmat::PROP_SETUP);
      propCommand->SetObject("DefaultSC", Gmat::SPACECRAFT);
      propCommand->SetRefObject(stopOnElapsedSecs, Gmat::STOP_CONDITION, "", 0);
      
      propCommand->SetSolarSystem(theDefaultSolarSystem);
      
#if DEBUG_MULTI_STOP
      //----------------------------------------------------
      //just for testing multiple stopping condition
      //----- StopCondition 2
      StopCondition *stopOnX =
         CreateStopCondition("StopCondition", "StopOnDefaultSC.X");
      stopOnX->SetStringParameter("EpochVar", "DefaultSC.CurrA1MJD");
      stopOnX->SetStringParameter("StopVar", "DefaultSC.X");
      stopOnX->SetRealParameter("Goal", 5000.0);
      propCommand->SetRefObject(stopOnX, Gmat::STOP_CONDITION, "", 1);
      //----------------------------------------------------
#endif

#if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default Propagate command created\n");
#endif
      
      // Add propagate command
      AppendCommand(propCommand);

      isRunReady = true;
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_,
          "Moderator::CreateDefaultMission() Error occurred during default "
          "mission creation. Default mission will not run.\n Message: " +
          e.GetMessage());
   }
}

//------------------------------------------------------------------------------
// bool CreateSlpFile(const std::string &fileName)
//------------------------------------------------------------------------------
bool Moderator::CreateSlpFile(const std::string &fileName)
{
   //MessageInterface::ShowMessage("Moderator::CreateSlpFile() fileName=%s\n",
   //                              fileName.c_str());
    
   bool status = false;
    
   if (isPlanetaryFileInUse[SLP])
   {
      MessageInterface::ShowMessage("Moderator::CreateSlpFile() SlpFile already created\n");
      status = true;
   }
   else
   {
      theDefaultSlpFile = new SlpFile(fileName);
      //MessageInterface::ShowMessage("Moderator::CreateSlpFile() SlpFile created\n");
        
      if (theDefaultSlpFile != NULL)
         status = true;
   }

   return status;
}

//------------------------------------------------------------------------------
// bool CreateDeFile(const Integer id, const std::string &fileName,
//                   Gmat::DeFileFormat format = Gmat::DE_BINARY)
//------------------------------------------------------------------------------
bool Moderator::CreateDeFile(Integer id, const std::string &fileName,
                             Gmat::DeFileFormat format)
{
   bool status = false;
   Gmat::DeFileType deFileType;
    
   if (isPlanetaryFileInUse[id])
   {
      MessageInterface::ShowMessage("Moderator::CreateDeFile() DeFile already created\n");
      status = true;
   }
   else
   {
      switch (id)
      {
      case DE200:
         deFileType = Gmat::DE200;
         break;
         //          case DE202:
         //              deFileType = Gmat::DE202;
         //              break;
      case DE405:
         deFileType = Gmat::DE405;
         break;
      default:
         MessageInterface::PopupMessage
            (Gmat::WARNING_,
             "Moderator::CreateDeFile() unsupported DE file type");
         return false;
      }
      
#if DEBUG_PLANETARY_FILE
      MessageInterface::ShowMessage
         ("Moderator::CreateDeFile() creating DeFile. type=%d, "
          "fileName=%s, format=%d\n", deFileType, fileName.c_str(),
          format);
#endif

      FILE *defile = fopen(fileName.c_str(), "rb");
      if (defile == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_,
             "Error opening DE file:%s. \n"
             "Please check file path. "
             "The next filetype in the list will be used.\n", fileName.c_str());
      }
      else
      {
         fclose(defile);
         
         try
         {
            theDefaultDeFile = new DeFile(deFileType, fileName, format);
        
            if (theDefaultDeFile != NULL)
               status = true;
         }
         catch (...)
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_,
                "Moderator::CreateDeFile() Error creating %s. "
                "The next filetype in the list will "
                "be created.\n", fileName.c_str());
         }
      }
   }
   return status;
}

// default objects
//------------------------------------------------------------------------------
// Spacecraft* GetDefaultSpacecraft()
//------------------------------------------------------------------------------
Spacecraft* Moderator::GetDefaultSpacecraft()
{
   StringArray &configList = GetListOfConfiguredItems(Gmat::SPACECRAFT);
   
   if (configList.size() > 0)
   {
      // return 1st Spacecraft from the list
      SpaceObject *so = GetSpacecraft(configList[0]);
      if (so->GetType() == Gmat::SPACECRAFT)
         return (Spacecraft*)so;
   }
   else
   {
      // create Spacecraft
      return (Spacecraft*)CreateSpacecraft("Spacecraft", "DefaultSC");
   }
   
   return NULL;
}

//------------------------------------------------------------------------------
// PropSetup* GetDefaultPropSetup()
//------------------------------------------------------------------------------
PropSetup* Moderator::GetDefaultPropSetup()
{
   StringArray &configList = GetListOfConfiguredItems(Gmat::PROP_SETUP);
   
   if (configList.size() > 0)
   {
      // return 1st PropSetup from the list
      return GetPropSetup(configList[0]);
   }
   else
   {
      // create PropSetup
      return CreateDefaultPropSetup("DefaultProp");
   }
}

//------------------------------------------------------------------------------
// Burn* GetDefaultBurn()
//------------------------------------------------------------------------------
Burn* Moderator::GetDefaultBurn()
{
   StringArray &configList = GetListOfConfiguredItems(Gmat::BURN);
   
   if (configList.size() > 0)
   {
      // return 1st Burn from the list
      return GetBurn(configList[0]);
   }
   else
   {
      // create ImpulsiveBurn
      return CreateBurn("ImpulsiveBurn", "DefaultBurn");
   }
}

//------------------------------------------------------------------------------
// StopCondition* CreateDefaultStopCondition()
//------------------------------------------------------------------------------
StopCondition* Moderator::CreateDefaultStopCondition()
{
   StopCondition *stopCond = NULL;
   
   Spacecraft *sc = GetDefaultSpacecraft();
   std::string epochVar = sc->GetName() + ".CurrA1MJD";
   std::string stopVar = sc->GetName() + ".ElapsedSecs";

   if (GetParameter(epochVar) == NULL)
      CreateParameter("CurrA1MJD", "DefaultSC.CurrA1MJD");

   if (GetParameter(stopVar) == NULL)
      CreateParameter("ElapsedSecs", "DefaultSC.ElapsedSecs");

   std::string stopCondName = "StopOn" + stopVar;
   
   stopCond = CreateStopCondition("StopCondition", "StopOn" + stopVar);
   
   stopCond->SetStringParameter("EpochVar", epochVar);
   stopCond->SetStringParameter("StopVar", stopVar);
   stopCond->SetRealParameter("Goal", 8640.0);
   return stopCond;
}

//------------------------------------------------------------------------------
// Parameter* GetDefaultX()
//------------------------------------------------------------------------------
Parameter* Moderator::GetDefaultX()
{
   Spacecraft *sc = GetDefaultSpacecraft();
   Parameter* param = GetParameter(sc->GetName() + ".CurrA1MJD");

   //loj: 10/1/04 SetRefObjectName to param
   if (param == NULL)
   {
      param = CreateParameter("CurrA1MJD", sc->GetName() + ".CurrA1MJD");
      param->SetRefObjectName(Gmat::SPACECRAFT, sc->GetName());
   }
   
   return param;
}

//------------------------------------------------------------------------------
// Parameter* GetDefaultY()
//------------------------------------------------------------------------------
Parameter* Moderator::GetDefaultY()
{
   Spacecraft *sc = GetDefaultSpacecraft();
   Parameter* param = GetParameter(sc->GetName() + ".X");
   
   //loj: 10/1/04  SetRefObjectName to param
   if (param == NULL)
   {
      param = CreateParameter("X", sc->GetName() + ".X");
      param->SetRefObjectName(Gmat::SPACECRAFT, sc->GetName());
   }
   
   return param;
}

// sandbox
//------------------------------------------------------------------------------
// void AddSolarSysToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddSolarSysToSandbox(Integer index)
{
   //SolarSystem *solarSys = theConfigManager->GetSolarSystemInUse();
   //sandboxes[index]->AddSolarSystem(solarSys);
   sandboxes[index]->AddSolarSystem(theDefaultSolarSystem);
}

//------------------------------------------------------------------------------
// void AddPublisherToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddPublisherToSandbox(Integer index)
{
   thePublisher->UnsubscribeAll();
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
      sc = (Spacecraft*)theConfigManager->GetSpacecraft(scNames[i]);
      sandboxes[index]->AddObject(sc);
   }
}

//------------------------------------------------------------------------------
// void AddFormationToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddFormationToSandbox(Integer index)
{
   Formation *form;
   StringArray formNames = theConfigManager->GetListOfItems(Gmat::FORMATION);

   for (Integer i=0; i<(Integer)formNames.size(); i++)
   {
      form = (Formation*)theConfigManager->GetSpacecraft(formNames[i]);
      sandboxes[index]->AddObject(form);
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
      sandboxes[index]->AddObject(propSetup);
   }
}

//------------------------------------------------------------------------------
// void AddPropagatorToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddPropagatorToSandbox(Integer index)
{
   Propagator *prop;
   StringArray propNames = theConfigManager->GetListOfItems(Gmat::PROPAGATOR);
    
   for (Integer i=0; i<(Integer)propNames.size(); i++)
   {
      prop = theConfigManager->GetPropagator(propNames[i]);
      sandboxes[index]->AddObject(prop);
   }
}

//------------------------------------------------------------------------------
// void AddForceModelToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddForceModelToSandbox(Integer index)
{
   ForceModel *fm;
   StringArray fmNames = theConfigManager->GetListOfItems(Gmat::FORCE_MODEL);
    
   for (Integer i=0; i<(Integer)fmNames.size(); i++)
   {
      fm = theConfigManager->GetForceModel(fmNames[i]);
      sandboxes[index]->AddObject(fm);
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
      sandboxes[index]->AddObject(burn);
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
      sandboxes[index]->AddObject(solver);
   }
}

//------------------------------------------------------------------------------
// void AddSuscriberToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddSubscriberToSandbox(Integer index)
{
   Subscriber *sub;
   StringArray subNames = theConfigManager->GetListOfItems(Gmat::SUBSCRIBER);
   for (Integer i=0; i<(Integer)subNames.size(); i++)
   {
      sub = theConfigManager->GetSubscriber(subNames[i]);
      sandboxes[index]->AddSubscriber(sub);
   }
}

//------------------------------------------------------------------------------
// void AddParameterToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddParameterToSandbox(Integer index)
{
   Parameter *param;
   StringArray paramNames = theConfigManager->GetListOfItems(Gmat::PARAMETER);
   for (Integer i=0; i<(Integer)paramNames.size(); i++)
   {
      param = theConfigManager->GetParameter(paramNames[i]);
      sandboxes[index]->AddObject(param);
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
/*
 * Constructor
 */
//------------------------------------------------------------------------------
Moderator::Moderator()
{
   isInitialized = false;
   isRunReady = false;
   theDefaultSolarSystem = NULL;
   theDefaultSlpFile = NULL;
   runState = Gmat::IDLE;
   
   theFactoryManager = FactoryManager::Instance();
   theConfigManager = ConfigManager::Instance();
   theFileManager = FileManager::Instance();

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
   /*
     if (instance != NULL)
        delete instance;
     if (theConfigManager != NULL)
        delete theConfigManager;
     if (theFactoryManager != NULL)
        delete theFactoryManager;
     if (theGuiInterpreter != NULL)
        delete theGuiInterpreter;
     if (theScriptInterpreter != NULL)
        delete theScriptInterpreter;
   
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
