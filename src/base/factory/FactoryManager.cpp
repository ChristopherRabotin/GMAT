//$Header$
//------------------------------------------------------------------------------
//                               Factory Manager
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2003/09/11
//
/**
 * Implementation code for the FactoryManager singleton class.
 */
//------------------------------------------------------------------------------
#include "FactoryManager.hpp"
#include "Factory.hpp"

/// initialize the only instance allowed for this singleton class
FactoryManager* FactoryManager::onlyInstance = NULL;

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  Instance()
//------------------------------------------------------------------------------
/**
 * Returns (and creates initially) the only instance allowed for this singleton
 * class. 
 *
 * @return Instance of the FactoryManager.
 */
//------------------------------------------------------------------------------
FactoryManager* FactoryManager::Instance()
{
   if (onlyInstance == NULL)
      onlyInstance = new FactoryManager();

   return onlyInstance;
}

//------------------------------------------------------------------------------
//  RegisterFactory()
//------------------------------------------------------------------------------
/**
 * Registers the input factory with the FactoryManager.
 *
 * All factories that are needed to create objects needed for the system to run
 * must be registered with the FactoryManager.  The Moderator should register
 * all predefined factories (e.g. Propagator, GmatCommand, etc.); a user-defined
 * class must have a corresponding factory, which must be registered in order
 * for objects of that class to be created in GMAT.
 *
 * @param <fact> the factory to register.
 *
 * @return bool indicating success of the registration.
 *
 * @notes Will need to add code later to check to make sure that the user is not
 * trying to register an already-registered factory or one that creates objects
 * of the same type as an already-registered factory.
 */
//------------------------------------------------------------------------------
bool FactoryManager::RegisterFactory(Factory* fact)
{
   if (fact == NULL)
   {
       return false;    // write some kind of error or throw an exception?
   }
   factoryList.push_back(fact);  // better to put it at the back of the list?
   return true;
}

//------------------------------------------------------------------------------
//  Spacecraft* CreateSpacecraft(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type Spacecraft, with the name withName.
 *
 * @param <ofType> type of the new Spacecraft object (currently defaults
 *                 to "Spacecraft")
 * @param <withName> name of the new Spacecraft object.
 *
 * @return pointer to the newly-created Spacecraft object
 */
//------------------------------------------------------------------------------
SpaceObject* FactoryManager::CreateSpacecraft(const std::string &ofType,
                                              const std::string &withName)
{
   Factory* f = FindFactory(Gmat::SPACECRAFT, ofType);
   if (f != NULL)
      return f->CreateSpacecraft(ofType,withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  Parameter* CreateParameter(const std::string &ofType,
//                             const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type Parameter, with the name withName, and of the
 * specific propagator type ofType.
 *
 * @param <ofType>   type name of the new Parameter object.
 * @param <withName> name of the new Parameter object.
 *
 * @return pointer to the newly-created Parameter object
 */
//------------------------------------------------------------------------------
Parameter* FactoryManager::CreateParameter(const std::string &ofType,
                                           const std::string &withName)
{
   Factory* f = FindFactory(Gmat::PARAMETER, ofType);
   if (f != NULL)
      return f->CreateParameter(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  Propagator* CreatePropagator(const std::string &ofType,
//                               const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type Propagator, with the name withName, and of the
 * specific propagator type ofType.
 *
 * @param <ofType>   type name of the new Propagator object.
 * @param <withName> name of the new Propagator object.
 *
 * @return pointer to the newly-created Propagator object
 */
//------------------------------------------------------------------------------
Propagator* FactoryManager::CreatePropagator(const std::string &ofType,
                                             const std::string &withName)
{
   Factory* f = FindFactory(Gmat::PROPAGATOR, ofType);
   if (f != NULL)
      return f->CreatePropagator(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  ForceModel* CreateForceModel(const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type ForceModel, with the name withName.
 *
 * @param <withName> name of the new ForceModel object.
 *
 * @return pointer to the newly-created ForceModel object
 */
//------------------------------------------------------------------------------
ForceModel* FactoryManager::CreateForceModel(const std::string &withName)
{
   Factory* f = FindFactory(Gmat::FORCE_MODEL, "ForceModel");
   if (f != NULL)
      return f->CreateForceModel("ForceModel", withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  Force* CreatePhysicalModel(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type PhysicalModel, with the name withName, and of the
 * type ofType.
 *
 * @param <ofType>   type name of the new PhysicalModel object.
 * @param <withName> name of the new PhysicalModel object.
 *
 * @return pointer to the newly-created PhysicalModel object
 */
//------------------------------------------------------------------------------
PhysicalModel* FactoryManager::CreatePhysicalModel(const std::string &ofType,
                                                   const std::string &withName)
{
   Factory* f = FindFactory(Gmat::PHYSICAL_MODEL, ofType);
   if (f != NULL)
      return f->CreatePhysicalModel(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  PropSetup* CreatePropSetup(const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type PropSetup, with the name withName.
 *
 * @param <withName> name of the new PropSetup object.
 *
 * @return pointer to the newly-created PropSetup object
 */
//------------------------------------------------------------------------------
PropSetup* FactoryManager::CreatePropSetup(const std::string &withName)
{
   Factory* f = FindFactory(Gmat::PROP_SETUP, "PropSetup");
   if (f != NULL)
      return f->CreatePropSetup("PropSetup", withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  StopCondition* CreateStopCondition(const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type StopCondition, with the name withName.
 *
 * @param <ofType>   type name of the new StopCondition object.
 * @param <withName> name of the new StopCondition object.
 *
 * @return pointer to the newly-created StopCondition object
 */
//------------------------------------------------------------------------------
StopCondition* FactoryManager::CreateStopCondition(const std::string &ofType,
                                                   const std::string &withName)
{
   Factory* f = FindFactory(Gmat::STOP_CONDITION, ofType);
   if (f != NULL)
      return f->CreateStopCondition(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  CelestialBody* CreateCelestialBody(const std::string &ofType,
//                                     const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type CelestialBody, with the name withName,
 * and of the type ofType.
 *
 * @param <ofType>   type name of the new CelestialBody object.
 * @param <withName> name of the new CelestialBody object.
 *
 * @return pointer to the newly-created CelestialBody object
 */
//------------------------------------------------------------------------------
CelestialBody* FactoryManager::CreateCelestialBody(const std::string &ofType,
                                                   const std::string &withName)
{
   Factory* f = FindFactory(Gmat::CELESTIAL_BODY, ofType);
   if (f != NULL)
      return f->CreateCelestialBody(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  SolarSystem* CreateSolarSystem(const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type SolarSystem, with the name withName.
 *
 * @param <withName> name of the new SolarSystem object.
 *
 * @return pointer to the newly-created SolarSystem object
 */
//------------------------------------------------------------------------------
SolarSystem* FactoryManager::CreateSolarSystem(const std::string &withName)
{
   Factory* f = FindFactory(Gmat::SOLAR_SYSTEM, "SolarSystem");
   if (f != NULL)
      return f->CreateSolarSystem("SolarSystem", withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  Solver* CreateSolver(const std::string &ofType,
//                       const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type Solver, with the name withName,
 * and of the type ofType.
 *
 * @param <ofType>   type name of the new Solver object.
 * @param <withName> name of the new Solver object.
 *
 * @return pointer to the newly-created Solver object
 */
//------------------------------------------------------------------------------
Solver* FactoryManager::CreateSolver(const std::string &ofType,
                                     const std::string &withName)
{
   Factory* f = FindFactory(Gmat::SOLVER, ofType);
   if (f != NULL)
      return f->CreateSolver(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  Subscriber* CreateSubscriber(const std::string &ofType,
//                               const std::string &withName,
//                               const std::string &fileName)
//------------------------------------------------------------------------------
/**
 * Create an object of type Subscriber, with the name withName.
 *
 * @param <ofType>   type name of the new Subscriber object.
 * @param <withName> name of the new Subscriber object.
 * @param <fileName> file name of Subscriber object, used only if Subscriber
 *                   is ReportFile
 *
 * @return pointer to the newly-created Subscriber object
 */
//------------------------------------------------------------------------------
Subscriber* FactoryManager::CreateSubscriber(const std::string &ofType,
                                             const std::string &withName,
                                             const std::string &fileName)
{
   Factory* f = FindFactory(Gmat::SUBSCRIBER, ofType);
   if (f != NULL)
      return f->CreateSubscriber(ofType, withName, fileName);
   return NULL;
}

//------------------------------------------------------------------------------
//  GmatCommand* CreateCommand(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type GmatCommand, with the name withName.
 *
 * @param <ofType>   type name of the new GmatCommand object.
 * @param <withName> name of the new GmatCommand object.
 *
 * @return pointer to the newly-created GmatCommand object
 */
//------------------------------------------------------------------------------
GmatCommand* FactoryManager::CreateCommand(const std::string &ofType,
                                           const std::string &withName)
{
   Factory* f = FindFactory(Gmat::COMMAND, ofType);
   if (f != NULL)
      return f->CreateCommand(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  Burn* CreateBurn(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type Burn, with the name withName.
 *
 * @param <ofType>   type name of the new Burn object.
 * @param <withName> name of the new Burn object.
 *
 * @return pointer to the newly-created Burn object
 */
//------------------------------------------------------------------------------
Burn* FactoryManager::CreateBurn(const std::string &ofType,
                                 const std::string &withName)
{
   Factory* f = FindFactory(Gmat::BURN, ofType);
   if (f != NULL)
      return f->CreateBurn(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  AtmosphereModel* CreateAtmosphereModel(const std::string &ofType,
//                                         const std::string &withName,
//                                         const std::string &forBody)
//------------------------------------------------------------------------------
/**
 * Create an object of type AtmosphereModel, with the name withName.
 *
 * @param <ofType>   type name of the new AtmosphereModel object.
 * @param <withName> name of the new AtmosphereModel object.
 * @param <forBody>  body name of the new AtmosphereModel object.
 *
 * @return pointer to the newly-created AtmosphereModel object
 */
//------------------------------------------------------------------------------
AtmosphereModel* FactoryManager::CreateAtmosphereModel(const std::string &ofType,
                                                       const std::string &withName,
                                                       const std::string &forBody)
{
   Factory* f = FindFactory(Gmat::ATMOSPHERE, ofType);
   if (f != NULL)
      return f->CreateAtmosphereModel(ofType, withName, forBody);
   return NULL;
}

//loj: 9/27/04 added
//------------------------------------------------------------------------------
//  Function* CreateFunction(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type Function, with the name withName.
 *
 * @param <ofType>   type name of the new Function object.
 * @param <withName> name of the new Function object.
 *
 * @return pointer to the newly-created Function object
 */
//------------------------------------------------------------------------------
Function* FactoryManager::CreateFunction(const std::string &ofType,
                                         const std::string &withName)
{
   Factory* f = FindFactory(Gmat::FUNCTION, ofType);
   if (f != NULL)
      return f->CreateFunction(ofType, withName);
   return NULL;
}

//djc: 11/10/04 added
//------------------------------------------------------------------------------
//  Hardware* CreateHardware(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type Hardware, with the name withName.
 *
 * @param <ofType>   type name of the new Hardware object.
 * @param <withName> name of the new Hardware object.
 *
 * @return pointer to the newly-created Hardware object
 */
//------------------------------------------------------------------------------
Hardware* FactoryManager::CreateHardware(const std::string &ofType,
                                         const std::string &withName)
{
   Factory* f = FindFactory(Gmat::HARDWARE, ofType);
   if (f != NULL)
      return f->CreateHardware(ofType, withName);
   return NULL;
}

//wcs: 2004/12/09 added
//------------------------------------------------------------------------------
//  AxisSystem* CreateAxisSystem(const std::string &ofType,
//                               const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an AxisSystem object of type ofType, with the name withName.
 *
 * @param <ofType>   type name of the AxisSystem.
 * @param <withName> name of the new AxisSystem object.
 *
 * @return pointer to the newly-created AxisSystem object
 */
//------------------------------------------------------------------------------
AxisSystem* 
FactoryManager::CreateAxisSystem(const std::string &ofType,
                                 const std::string &withName)
{
   Factory* f = FindFactory(Gmat::AXIS_SYSTEM, ofType);
   if (f != NULL)
      return f->CreateAxisSystem(ofType, withName);
   return NULL;
}


//------------------------------------------------------------------------------
// StringArray  GetListOfItems(Gmat::ObjectType byType)
//------------------------------------------------------------------------------
/**
 * Return a list of items of type byType that can be created.
 *
 * @param <byType> type of items of which to return a list.
 *
 * @return list of creatable items of type byType.
 */
//------------------------------------------------------------------------------
StringArray  FactoryManager::GetListOfItems(Gmat::ObjectType byType)
{
   return GetList(byType);
}

//------------------------------------------------------------------------------
// StringArray  GetListOfSpacecraft()
//------------------------------------------------------------------------------
/**
 * Return a list of items of type Spacecraft that can be created.
 *
 * @return list of creatable items of type Spacecraft.
 */
//------------------------------------------------------------------------------
StringArray  FactoryManager::GetListOfSpacecraft()
{
   return GetList(Gmat::SPACECRAFT);
}

//------------------------------------------------------------------------------
// StringArray GetListOfGroundStation()
//------------------------------------------------------------------------------
/**
 * Return a list of items of type GroundStation that can be created.
 *
 * @return list of creatable items of type GroundStation.
 */
//------------------------------------------------------------------------------
//StringArray FactoryManager::GetListOfGroundStation() const
//{
//   return GetList(Gmat::GROUND_STATION);
//}

//------------------------------------------------------------------------------
// StringArray  GetListOfPropagator()
//------------------------------------------------------------------------------
/**
 * Return a list of items of type Propagator that can be created.
 *
 * @return list of creatable items of type Propagator.
 */
//------------------------------------------------------------------------------
StringArray  FactoryManager::GetListOfPropagator()
{
   return GetList(Gmat::PROPAGATOR);
}

//------------------------------------------------------------------------------
// StringArray  GetListOfForceModel()
//------------------------------------------------------------------------------
/**
 * Return a list of items of type ForceModel that can be created.
 *
 * @return list of creatable items of type ForceModel.
 */
//------------------------------------------------------------------------------
StringArray  FactoryManager::GetListOfForceModel()
{
   return GetList(Gmat::FORCE_MODEL);
}

//------------------------------------------------------------------------------
// StringArray  GetListOfPhysicalModel()
//------------------------------------------------------------------------------
/**
 * Return a list of items of type PhysicalModel that can be created.
 *
 * @return list of creatable items of type Force.
 */
//------------------------------------------------------------------------------
StringArray  FactoryManager::GetListOfPhysicalModel()
{
   return GetList(Gmat::PHYSICAL_MODEL);
}

//------------------------------------------------------------------------------
// StringArray  GetListOfPropSetup()
//------------------------------------------------------------------------------
/**
 * Return a list of items of type PropSetup that can be created.
 *
 * @return list of creatable items of type PropSetup.
 */
//------------------------------------------------------------------------------
StringArray  FactoryManager::GetListOfPropSetup()
{
   return GetList(Gmat::PROP_SETUP);
}

//------------------------------------------------------------------------------
// StringArray  GetListOfStopCondition()
//------------------------------------------------------------------------------
/**
 * Return a list of items of type StopCondition that can be created.
 *
 * @return list of creatable items of type StopCondition.
 */
//------------------------------------------------------------------------------
StringArray  FactoryManager::GetListOfStopCondition()
{
   return GetList(Gmat::STOP_CONDITION);
}

//------------------------------------------------------------------------------
// StringArray  GetListOfCelestialBody()
//------------------------------------------------------------------------------
/**
 * Return a list of items of type CelestialBody that can be created.
 *
 * @return list of creatable items of type CelestialBody.
 */
//------------------------------------------------------------------------------
StringArray  FactoryManager::GetListOfCelestialBody()
{
   return GetList(Gmat::CELESTIAL_BODY);
}

//------------------------------------------------------------------------------
// StringArray  GetListOfSolarSystem()
//------------------------------------------------------------------------------
/**
 * Return a list of items of type SolarSystem that can be created.
 *
 * @return list of creatable items of type SolarSystem.
 */
//------------------------------------------------------------------------------
StringArray  FactoryManager::GetListOfSolarSystem()
{
   return GetList(Gmat::SOLAR_SYSTEM);
}

//------------------------------------------------------------------------------
// StringArray  GetListOfSolver()
//------------------------------------------------------------------------------
/**
 * Return a list of items of type Solver that can be created.
 *
 * @return list of creatable items of type Solver.
 */
//------------------------------------------------------------------------------
StringArray  FactoryManager::GetListOfSolver()
{
   return GetList(Gmat::SOLVER);
}

//------------------------------------------------------------------------------
// StringArray  GetListOfSubscriber()
//------------------------------------------------------------------------------
/**
 * Return a list of items of type Subscriber that can be created.
 *
 * @return list of creatable items of type Subscriber.
 */
//------------------------------------------------------------------------------
StringArray  FactoryManager::GetListOfSubscriber()
{
   return GetList(Gmat::SUBSCRIBER);
}

//------------------------------------------------------------------------------
// StringArray  GetListOfCommand()
//------------------------------------------------------------------------------
/**
 * Return a list of items of type GmatCommand that can be created.
 *
 * @return list of creatable items of type GmatCommand.
 */
//------------------------------------------------------------------------------
StringArray  FactoryManager::GetListOfCommand()
{
   return GetList(Gmat::COMMAND);
}

//------------------------------------------------------------------------------
// StringArray  GetListOfBurn()
//------------------------------------------------------------------------------
/**
 * Return a list of items of type Burn that can be created.
 *
 * @return list of creatable items of type Burn.
 */
//------------------------------------------------------------------------------
StringArray  FactoryManager::GetListOfBurn()
{
   return GetList(Gmat::BURN);
}

//------------------------------------------------------------------------------
// StringArray  GetListOfAtmosphereModel()
//------------------------------------------------------------------------------
/**
 * Return a list of items of type AtmosphereModel that can be created.
 *
 * @return list of creatable items of type AtmosphereModel.
 */
//------------------------------------------------------------------------------
StringArray  FactoryManager::GetListOfAtmosphereModel()
{
   return GetList(Gmat::ATMOSPHERE);
}

//------------------------------------------------------------------------------
// StringArray  GetListOfFunction()
//------------------------------------------------------------------------------
/**
 * Return a list of items of type Function that can be created.
 *
 * @return list of creatable items of type Function.
 */
//------------------------------------------------------------------------------
StringArray  FactoryManager::GetListOfFunction()
{
   return GetList(Gmat::FUNCTION);
}

//------------------------------------------------------------------------------
// StringArray  GetListOfHardware()
//------------------------------------------------------------------------------
/**
* Return a list of items of type Hardware that can be created.
 *
 * @return list of creatable items of type Hardware.
 */
//------------------------------------------------------------------------------
StringArray  FactoryManager::GetListOfHardware()
{
   return GetList(Gmat::HARDWARE);
}

//------------------------------------------------------------------------------
// StringArray  GetListOfAxisSystem()
//------------------------------------------------------------------------------
/**
 * Return a list of items of type AxisSystem that can be created.
 *
 * @return list of creatable items of type AxisSystem.
 */
//------------------------------------------------------------------------------
StringArray  FactoryManager::GetListOfAxisSystem()
{
   return GetList(Gmat::AXIS_SYSTEM);
}

//------------------------------------------------------------------------------
//  ~FactoryManager()
//------------------------------------------------------------------------------
/**
* Destructs FactoryManager objects.
 *
 */
//------------------------------------------------------------------------------
FactoryManager::~FactoryManager()
{
std::list<Factory*>::iterator f = factoryList.begin();
   // Factory f = factoryList.begin();
   while (f != factoryList.end())
   {
      delete *f;       // delete each factory first
      ++f;
   }
   //delete factoryList;
   factoryList.~list<Factory*>();
};

//---------------------------------
//  protected methods
//---------------------------------
// none


//---------------------------------
//  private methods
//---------------------------------

//------------------------------------------------------------------------------
//  FactoryManager()
//------------------------------------------------------------------------------
/**
 * Constructs a FactoryManager object (default constructor).
 */
//------------------------------------------------------------------------------
FactoryManager::FactoryManager()
{
}

//------------------------------------------------------------------------------
//  FactoryManager(const FactoryManager& fact)
//------------------------------------------------------------------------------
/**
 * Constructs a FactoryManager object as a copy of the input FactoryManager
 * object (copy constructor).
 *
 * @param <fact> object to "copy" to create "this".
 */
//------------------------------------------------------------------------------
//FactoryManager::FactoryManager(const FactoryManager& fact) :
//factoryList (fact.factoryList)
//{
//}

//------------------------------------------------------------------------------
//  FactoryManager& operator=(const FactoryManager& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <fact> Command object whose values to use to set "this" Command
 * object.
 *
 * @return Command object.
 */
//------------------------------------------------------------------------------
//FactoryManager& FactoryManager::operator= (const FactoryManager& fact) :
//factoryList (fact.factoryList)
//{
//}

//------------------------------------------------------------------------------
// Factory* FindFactory(Gmat::ObjectType ofType, const std::string forType)
//------------------------------------------------------------------------------
/**
 * Return a pointer to a factory that can create objects of type forType.
 *
 * @param <ofType>  type of creatable object requested.
 * @param <forType> specific type of creatable object requested.
 *
 * @return pointer to a factory that creates objects of the requested type.
 */
//------------------------------------------------------------------------------
Factory* FactoryManager::FindFactory(Gmat::ObjectType ofType,
                                     const std::string &forType)
{
   // Search through factoryList for the factory that creates objects of type
   // ofType, specifically of type forType
   std::list<Factory*>::iterator f = factoryList.begin();
   //Factory f = factoryList.begin();     // returns a bidirectional iterator
   while (f != factoryList.end())
   {
      if ((*f)->GetFactoryType() == ofType)
      {
         // Search through the list of creatable objects to see if one matches
         StringArray listObj = (*f)->GetListOfCreatableObjects();
         if (!listObj.empty())
         {
            StringArray::iterator s = listObj.begin();
            while (s != listObj.end())
            {
               if ((*s).compare(forType) == 0)
               {
                  //MessageInterface::ShowMessage
                  //   ("FactoryManager::FindFactory() Found factory:%s\n", forType.c_str());
                  return (*f);
               }
               ++s;
            }
         }

      }
      ++f;
   }
   return NULL;
}

//------------------------------------------------------------------------------
// StringArray  GetList(Gmat::ObjectType ofType) const
//------------------------------------------------------------------------------
/**
 * Return a list of items of type ofType that can be created.
 *
 * @param <ofType> type of creatable objects of which to return a list.
 *
 * @return list of creatable items of type ofType.
 */
//------------------------------------------------------------------------------
StringArray  FactoryManager::GetList(Gmat::ObjectType ofType)
{
    entireList.clear();

   // Search through factoryList for the factory that creates objects of type
   // ofType
//   std::list<Factory*>::iterator f = ((std::list<Factory*>)factoryList).begin();

   std::list<Factory*>::iterator f = factoryList.begin();
   while (f != factoryList.end())
   {
      if ((*f)->GetFactoryType() == ofType)
      {
         // Add the name(s) to the list 
         StringArray s = (*f)->GetListOfCreatableObjects();
         if (!entireList.empty())
         {
            StringArray::iterator si = s.begin();
            while (si != s.end())
            {
               entireList.push_back(*si);
               ++si;
            }
         }
//            entireList.splice(entireList.end(),s);
         else
            entireList = s;
      }
      ++f;
   }
   return entireList;
}
