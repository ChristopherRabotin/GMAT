//$Header$
//------------------------------------------------------------------------------
//                             Factory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2003/08/28
//
/**
 *  This class is the base class for the factories.  Derived classes will be
 *  responsible for creating objects of a specific type.
 */
//------------------------------------------------------------------------------
#ifndef Factory_hpp
#define Factory_hpp

#include "gmatdefs.hpp"
#include "Spacecraft.hpp"
#include "Parameter.hpp"
#include "Burn.hpp"
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
#include "AtmosphereModel.hpp" //loj: 9/14/04 - added

class GMAT_API Factory
{
public:
   // methods to return objects of specified types
   virtual SpaceObject*     CreateSpacecraft(const std::string &ofType,
                                             const std::string &withName = "");        
   virtual Propagator*      CreatePropagator(const std::string &ofType,
                                             const std::string &withName = "");
   virtual ForceModel*      CreateForceModel(const std::string &ofType,
                                             const std::string &withName = "");
   virtual PhysicalModel*   CreatePhysicalModel(const std::string &ofType,
                                                const std::string &withName = "");
   virtual PropSetup*       CreatePropSetup(const std::string &ofType,
                                            const std::string &withName = "");
   virtual Parameter*       CreateParameter(const std::string &ofType,
                                            const std::string &withName = "");
   virtual Burn*            CreateBurn(const std::string &ofType,
                                       const std::string &withName = "");
   virtual StopCondition*   CreateStopCondition(const std::string &ofType,
                                                const std::string &withName = "");
   virtual CelestialBody*   CreateCelestialBody(const std::string &ofType,
                                                const std::string &withName = "");
   virtual SolarSystem*     CreateSolarSystem(const std::string &ofType,
                                              const std::string &withName = "");
   virtual Solver*          CreateSolver(const std::string &ofType,
                                         const std::string &withName = ""); 
   virtual Subscriber*      CreateSubscriber(const std::string &ofType,
                                             const std::string &withName = "",
                                             const std::string &fileName = "");
   virtual GmatCommand*     CreateCommand(const std::string &ofType,
                                          const std::string &withName = "");
   virtual AtmosphereModel* CreateAtmosphereModel(const std::string &ofType,
                                                  const std::string &withName = "",
                                                  const std::string &forBody = "Earth");

   // method to return list of types of objects that this factory can create
   StringArray              GetListOfCreatableObjects(void) const;
   // method to set the types of objects that this factory can create
   bool                     SetListOfCreatableObjects(StringArray newList);
   // method to add types of objects that this factory can create
   bool                     AddCreatableObjects(StringArray newList);

   // method to return the type of factory this is
   Gmat::ObjectType         GetFactoryType(void) const;  

   // destructor
   virtual ~Factory();

protected:
   // constructor specifying the type of objects creatable by the factory
   Factory(Gmat::ObjectType ofType = Gmat::UNKNOWN_OBJECT);
   // constructor specifying the type of objects creatable by the factory and the
   // specific types that this factory can create
   Factory(StringArray createList, Gmat::ObjectType ofType = Gmat::UNKNOWN_OBJECT);
   // copy constructor
   Factory(const Factory& fact);
   // assignment operator
   Factory& operator= (const Factory& fact);

   // protected data
   // the type of the factory (i.e. what type of objects it can create)
   Gmat::ObjectType         itsType;
   // a list of all of the specific types of objects (of type itsType) that
   // can be created by this factory.
   StringArray              creatables;

private:
    
};

#endif // Factory_hpp




