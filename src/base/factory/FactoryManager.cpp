//$Id$
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
#include "StringUtil.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_FACTORY_CREATE

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

   //MessageInterface::ShowMessage
   //   ("FactoryManager::RegisterFactory() adding factory %d\n", fact->GetFactoryType());
   
   factoryList.push_back(fact);  // better to put it at the back of the list?
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const Gmat::ObjectType generalType,
//               const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an generic object, with the name withName.
 *
 * @param <ofType> type of the new Spacecraft object (currently defaults
 *                 to "Spacecraft")
 * @param <withName> name of the new Spacecraft object.
 *
 * @return pointer to the newly-created Spacecraft object
 */
//------------------------------------------------------------------------------
GmatBase* FactoryManager::CreateObject(const Gmat::ObjectType generalType,
                                               const std::string &ofType,
                                       const std::string &withName)
{
   Factory* f = FindFactory(generalType, ofType);
   if (f != NULL)
      return f->CreateObject(ofType,withName);
   return NULL;
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
//  CalculatedPoint* CreateCalculatedPoint(const std::string &ofType,
//                                         const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type CalculatedPoint, with the name withName,
 * and of the type ofType.
 *
 * @param <ofType>   type name of the new CalculatedPoint object.
 * @param <withName> name of the new CalculatedPoint object.
 *
 * @return pointer to the newly-created CalculatedPoint object
 */
//------------------------------------------------------------------------------
CalculatedPoint* FactoryManager::CreateCalculatedPoint(const std::string &ofType,
                                                       const std::string &withName)
{
   Factory* f = FindFactory(Gmat::CALCULATED_POINT, ofType);
   if (f != NULL)
      return f->CreateCalculatedPoint(ofType, withName);
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

   MessageInterface::ShowMessage("      Could not find Factory for %s\n", ofType.c_str());
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
//  MathNode* CreateMathNode(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an MathNode object of type ofType, with the name withName.
 *
 * @param <ofType>   type name of the MathNode.
 * @param <withName> name of the new MathNode object.
 *
 * @return pointer to the newly-created MathNode object
 */
//------------------------------------------------------------------------------
MathNode* 
FactoryManager::CreateMathNode(const std::string &ofType,
                               const std::string &withName)
{
   Factory* f = FindFactory(Gmat::MATH_NODE, ofType);
   if (f != NULL)
      return f->CreateMathNode(ofType, withName);
   return NULL;
}

//------------------------------------------------------------------------------
//  Attitude* CreateAttitude(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an Attitude object of type ofType, with the name withName.
 *
 * @param <ofType>   type name of the Attitude.
 * @param <withName> name of the new Attitude object.
 *
 * @return pointer to the newly-created Attitude object
 */
//------------------------------------------------------------------------------
Attitude* 
FactoryManager::CreateAttitude(const std::string &ofType,
                               const std::string &withName)
{
   Factory* f = FindFactory(Gmat::ATTITUDE, ofType);
   if (f != NULL)
      return f->CreateAttitude(ofType, withName);
   return NULL;
}

//----- Just container
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
//  CoordinateSystem* CreateCoordinateSystem(const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Create an object of type CoordinateSystem, with the name withName.
 *
 * @param <withName> name of the new CoordinateSystem object.
 *
 * @return pointer to the newly-created CoordinateSystem object
 */
//------------------------------------------------------------------------------
CoordinateSystem*
FactoryManager::CreateCoordinateSystem(const std::string &withName)
{
   Factory* f = FindFactory(Gmat::COORDINATE_SYSTEM, "CoordinateSystem");
   if (f != NULL)
      return f->CreateCoordinateSystem("CoordinateSystem", withName);
   return NULL;
}


//------------------------------------------------------------------------------
// const StringArray& GetListOfItems(Gmat::ObjectType byType)
//------------------------------------------------------------------------------
/**
 * Return a list of items of type byType that can be created.
 *
 * @param <byType> type of items of which to return a list.
 *
 * @return list of creatable items of type byType.
 */
//------------------------------------------------------------------------------
const StringArray& FactoryManager::GetListOfItems(Gmat::ObjectType byType)
{
   return GetList(byType);
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
   //MessageInterface::ShowMessage("~FactoryManager() size=%d\n", factoryList.size());
   
   std::list<Factory*>::iterator f = factoryList.begin();
   while (f != factoryList.end())
   {
      //MessageInterface::ShowMessage("Deleting FactoryType=%d\n", (*f)->GetFactoryType());
      delete *f;       // delete each factory first
      ++f;
   }
   
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
   #ifdef DEBUG_FACTORY_CREATE
   MessageInterface::ShowMessage(
   "Entering FactoryManager::FindFactory with type = %d and forType = %s\n",
   (Integer) ofType, forType.c_str());
   #endif
   
   // Search through factoryList for the factory that creates objects of type
   // ofType, specifically of type forType
   std::list<Factory*>::iterator f = factoryList.begin();
   while (f != factoryList.end())
   {
      if ((*f)->GetFactoryType() == ofType)
      {
         // Search through the list of creatable objects to see if one matches
         StringArray listObj = (*f)->GetListOfCreatableObjects();
         bool isCaseSensitive = (*f)->IsTypeCaseSensitive();
         
         #ifdef DEBUG_FACTORY_CREATE
         MessageInterface::ShowMessage(
         "    isCaseSensitive = %s\n",
         (isCaseSensitive?  "TRUE" : "False"));
         #endif
         
         if (!listObj.empty())
         {
            StringArray::iterator s = listObj.begin();
            std::string objType = forType;
            if (isCaseSensitive)
               objType = GmatStringUtil::Capitalize(objType);
            
            while (s != listObj.end())
            {
               #ifdef DEBUG_FACTORY_CREATE
               MessageInterface::ShowMessage(
               "    -> now comparing \"%s\" with \"%s\"\n",
               (*s).c_str(), objType.c_str());
               #endif
               
               if ((*s).compare(objType) == 0)
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
   #ifdef DEBUG_FACTORY_CREATE
   MessageInterface::ShowMessage(
   "At end of FactoryManager::FindFactory, returning NULL\n");
   #endif
   return NULL;
}

//------------------------------------------------------------------------------
// const StringArray& GetList(Gmat::ObjectType ofType) const
//------------------------------------------------------------------------------
/**
 * Return a list of items of type ofType that can be created.
 *
 * @param <ofType> type of creatable objects of which to return a list.
 *
 * @return list of creatable items of type ofType.
 */
//------------------------------------------------------------------------------
const StringArray& FactoryManager::GetList(Gmat::ObjectType ofType)
{
   entireList.clear();
   
   std::list<Factory*>::iterator f = factoryList.begin();
   while (f != factoryList.end())
   {
      if ((*f)->GetFactoryType() == ofType)
      {
         // Add the name(s) to the list 
         StringArray objs = (*f)->GetListOfCreatableObjects();
         if (!objs.empty())
            entireList.insert(entireList.end(), objs.begin(), objs.end());
         
         break;
      }
      ++f;
   }
   return entireList;
}
