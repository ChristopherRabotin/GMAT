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
//#include "GroundStation.hpp"
#include "Propagator.hpp"
#include "ForceModel.hpp"
#include "PhysicalModel.hpp"
#include "PropSetup.hpp"
//#include "CalculatedParameter.hpp"
#include "StopCondition.hpp"
#include "CelestialBody.hpp"
#include "SolarSystem.hpp"
//#include "Solver.hpp"
#include "Subscriber.hpp"
#include "Command.hpp"

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

    // bool                   RemoveFactory(Factory& fact):     // ???      

    // methods to create and return objects of the various types
    Spacecraft *          CreateSpacecraft(std::string ofType = "Spacecraft",
                                           std::string withName = "");
//    GroundStation *       CreateGroundStation(std::string withName);
    Propagator *          CreatePropagator(std::string ofType,
                                           std::string withName = "");
    ForceModel *          CreateForceModel(std::string withName = "");
    PhysicalModel *       CreatePhysicalModel(std::string ofType,
                                              std::string withName = "");
    PropSetup *           CreatePropSetup(std::string withName = "");
//    CalculatedParameter * CreateCalculatedParameter(std::string withName
//                                                    = "Parameter",
//                                                    std::string ofType);
    StopCondition *       CreateStopCondition(std::string ofType,
                                              std::string withName = "");
    CelestialBody *       CreateCelestialBody(std::string ofType,
                                              std::string withName = "");
    SolarSystem *         CreateSolarSystem(std::string withName = "");
//    Solver *              CreateSolver(std::string ofType,
//                                       std::string withName = "");
    Subscriber *          CreateSubscriber(std::string ofType,
                                           std::string withName = "",
                                           std::string fileName = "");
    Command *             CreateCommand(std::string ofType,
                                        std::string withName = "");

    // method to return a list of strings representing the objects of the input
    // type that may be created in the system
    StringArray            GetListOfItems(Gmat::ObjectType byType) const;

    // individual methods to return a list of strings representing the types of
    // objects that may be created in the system, by Gmat::ObjectType
    StringArray            GetListOfSpacecraft(void) const;
//    StringArray            GetListofGroundStation(void) const;
    StringArray            GetListOfPropagator(void) const;
    StringArray            GetListOfForceModel(void) const;
    StringArray            GetListOfPhysicalModel(void) const;
    StringArray            GetListOfPropSetup(void) const;
//    StringArray            GetListofCalculatedParameter(void) const;
    StringArray            GetListOfStopCondition(void) const;
    StringArray            GetListOfCelestialBody(void) const;
    StringArray            GetListOfSolarSystem(void) const;
//    StringArray            GetListofSolver(void) const;
    StringArray            GetListOfSubscriber(void) const;
    StringArray            GetListOfCommand(void) const;

    // class destructor
    ~FactoryManager();
    
protected:

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
    Factory* FindFactory(Gmat::ObjectType ofType, std::string forType);
    StringArray GetList(Gmat::ObjectType ofType) const;

};

#endif // FactoryManager_hpp




