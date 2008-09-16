//$Id$
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
#include "GmatBase.hpp"

// Forward references
class Spacecraft;
class SpaceObject;
class Parameter;
class Burn;
class Propagator;
class ForceModel;
class PhysicalModel;
class PropSetup;
class StopCondition;
class CalculatedPoint;
class CelestialBody;
class SolarSystem;
class Solver;
class Subscriber;
class GmatCommand;
class AtmosphereModel;
class Function;
class Hardware;
class AxisSystem;
class CoordinateSystem;
class MathNode;
class Attitude;

class GMAT_API Factory
{
public:
   // method to return objects as generic type
   virtual GmatBase*        CreateObject(const std::string &ofType,
                                         const std::string &withName = "");        

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
   virtual CalculatedPoint* CreateCalculatedPoint(const std::string &ofType,
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
   virtual Function*        CreateFunction(const std::string &ofType,
                                           const std::string &withName = "");
   virtual Hardware*        CreateHardware(const std::string &ofType,
                                           const std::string &withName = "");
   virtual AxisSystem*      CreateAxisSystem(const std::string &ofType,
                                             const std::string &withName = "");
   virtual CoordinateSystem* CreateCoordinateSystem(const std::string &ofType,
                                                    const std::string &withName = "");
   virtual MathNode*        CreateMathNode(const std::string &ofType,
                                           const std::string &withName = "");
   virtual Attitude*        CreateAttitude(const std::string &ofType,
                                           const std::string &withName = "");

   
   // method to return list of types of objects that this factory can create
   StringArray              GetListOfCreatableObjects(void) const;
   // method to set the types of objects that this factory can create
   bool                     SetListOfCreatableObjects(StringArray newList);
   // method to add types of objects that this factory can create
   bool                     AddCreatableObjects(StringArray newList);

   // method to return the type of factory this is
   Gmat::ObjectType         GetFactoryType() const;  
   bool                     IsTypeCaseSensitive() const;
   
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
   // is type name case sensitive
   bool                     isCaseSensitive;

private:
    
};

#endif // Factory_hpp




