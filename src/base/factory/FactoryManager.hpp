//$Header$
//------------------------------------------------------------------------------
//                             FactoryManager
//------------------------------------------------------------------------------

// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2003/08/27
//
/**
 * This class is the interface between the Moderator and Factories.  It is a
 * singleton - only one instance of this class can be created.
 */
//------------------------------------------------------------------------------
#ifndef FactoryManager_hpp
#define FactoryManager_hpp

#include <list>
#include <string>
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "Spacecraft.hpp"
#include "Parameter.hpp"
#include "Propagator.hpp"
#include "ForceModel.hpp"
#include "PhysicalModel.hpp"
#include "PropSetup.hpp"
#include "StopCondition.hpp"
#include "CelestialBody.hpp"
#include "SolarSystem.hpp"
#include "Solver.hpp"
#include "Subscriber.hpp"
#include "Command.hpp"
#include "Burn.hpp"
#include "AtmosphereModel.hpp"

/**
 * GMAT Factory Manager Class, the interface between the Moderator and the
 * factories.
 *
 * The Factory Manager manages all of the factories that are needed for
 * execution of the system for a particular run.  Each factory must be
 * registered with the FactoryManager in order for its objects to be able to be
 * created.  The Moderator will register all of the predefined factories;
 * user-defined factories must be registered as they are added to the system.
 */
class GMAT_API FactoryManager
{
public:
   // class instance method (this is a singleton class)
   static FactoryManager* Instance();
   // all factories must be registered via this method
   bool                   RegisterFactory(Factory* fact);

   // methods to create and return objects of the various types
   SpaceObject*           CreateSpacecraft(const std::string &ofType,
                                           const std::string &withName = "");
   Parameter*             CreateParameter(const std::string &ofType,
                                          const std::string &withName = "");
   Propagator*            CreatePropagator(const std::string &ofType,
                                           const std::string &withName = "");
   ForceModel*            CreateForceModel(const std::string &withName = "");
   PhysicalModel*         CreatePhysicalModel(const std::string &ofType,
                                              const std::string &withName = "");
   PropSetup*             CreatePropSetup(const std::string &withName = "");
   StopCondition*         CreateStopCondition(const std::string &ofType,
                                              const std::string &withName = "");
   CelestialBody*         CreateCelestialBody(const std::string &ofType,
                                              const std::string &withName = "");
   SolarSystem*           CreateSolarSystem(const std::string &withName = "");
   Solver*                CreateSolver(const std::string &ofType,
                                       const std::string &withName = "");
   Subscriber*            CreateSubscriber(const std::string &ofType,
                                           const std::string &withName = "",
                                           const std::string &fileName = "");
   GmatCommand*           CreateCommand(const std::string &ofType,
                                        const std::string &withName = "");
   Burn*                  CreateBurn(const std::string &ofType,
                                     const std::string &withName = "");
   AtmosphereModel*       CreateAtmosphereModel(const std::string &ofType,
                                                const std::string &withName = "",
                                                const std::string &forBody = "Earth");
   
   // method to return a list of strings representing the objects of the input
   // type that may be created in the system
   StringArray            GetListOfItems(Gmat::ObjectType byType);// const;

   // individual methods to return a list of strings representing the types of
   // objects that may be created in the system, by Gmat::ObjectType
   StringArray            GetListOfSpacecraft(void);// const;
   StringArray            GetListOfPropagator(void);// const;
   StringArray            GetListOfForceModel(void);// const;
   StringArray            GetListOfPhysicalModel(void);// const;
   StringArray            GetListOfPropSetup(void);// const;
   StringArray            GetListOfStopCondition(void);// const;
   StringArray            GetListOfCelestialBody(void);// const;
   StringArray            GetListOfSolarSystem(void);// const;
   StringArray            GetListOfSolver(void);// const;
   StringArray            GetListOfSubscriber(void);// const;
   StringArray            GetListOfCommand(void);// const;
   StringArray            GetListOfBurn(void);// const;
   StringArray            GetListOfAtmosphereModel(void);// const;

   // class destructor
   ~FactoryManager();
    
protected:
   StringArray            entireList;

private:

   // default constructor
   FactoryManager();
   // copy constructor
   FactoryManager(const FactoryManager& fact);
   // assignment operator
   FactoryManager& operator= (const FactoryManager& fact);
    
   // private class data
   /// the list of factories that have been registered and which are available
   /// to create objects
   std::list<Factory*> factoryList;
   /// pointer to the only instance allowed for this singleton class
   static FactoryManager* onlyInstance;
   
   // private methods 
   Factory* FindFactory(Gmat::ObjectType ofType, const std::string &forType);
   StringArray  GetList(Gmat::ObjectType ofType);// const;

};

#endif // FactoryManager_hpp

