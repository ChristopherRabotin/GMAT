//$Header$
//------------------------------------------------------------------------------
//                            Factory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2003/09/16
//
/**
 *  Implementation code for the base class, Factory, responsible for creating
 *  objects; subclasses will create instances of specific objects.
 */
//------------------------------------------------------------------------------
#include "Factory.hpp"
#include "FactoryException.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  Spacecraft* CreateSpacecraft(std::string ofType, std::string withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create Spacecraft objects -
 * in that case, it returns a new Spacecraft object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type Spacecraft.
 *
 * @param <ofType>   specific type of Spacecraft object to create.
 * @param <withName> name to give to the newly created Spacecraft object.
 *
 * @return pointer to a new Spacecraft object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type Spacecraft.
 */
//------------------------------------------------------------------------------
Spacecraft* Factory::CreateSpacecraft(std::string ofType,
                                              std::string withName)
{
   throw FactoryException("requested object must be of type Spacecraft");
}

//------------------------------------------------------------------------------
//  GroundStation* CreateGroundStation(std::string ofType, std::string withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create GroundStation objects -
 * in that case, it returns a new GroundStation object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type GroundStation.
 *
 * @param <ofType>   specific type of GroundStation object to create.
 * @param <withName> name to give to the newly created GroundStation object.
 *
 * @return pointer to a new GroundStation object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type GroundStation.
 */
//------------------------------------------------------------------------------
//   GroundStation*ÊFactory::CreateGroundStation(std::string ofType,
//                                                   std::string withName);
//{
//   throw FactoryException("requested object must be of type GroundStation");
//}

//------------------------------------------------------------------------------
//  Propagator* CreatePropagator(std::string ofType, std::string withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create Propagator objects -
 * in that case, it returns a new Propagator object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type Propagator.
 *
 * @param <ofType>   specific type of Propagator object to create.
 * @param <withName> name to give to the newly created Propagator object.
 *
 * @return pointer to a new Propagator object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type Propagator.
 */
//------------------------------------------------------------------------------
Propagator* Factory::CreatePropagator(std::string ofType,
                                              std::string withName)
{
   throw FactoryException("requested object must be of type Propagator");
}

//------------------------------------------------------------------------------
//  ForceModel* CreateForceModel(std::string ofType, std::string withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  ForceModel objects -
 * in that case, it returns a new  ForceModel object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  ForceModel.
 *
 * @param <ofType>   specific type of  ForceModel object to create.
 * @param <withName> name to give to the newly created  ForceModel object.
 *
 * @return pointer to a new  ForceModel object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  ForceModel.
 */
//------------------------------------------------------------------------------
ForceModel* Factory::CreateForceModel(std::string ofType,
                                              std::string withName)
{
   throw FactoryException("requested object must be of type ForceModel");
}

//------------------------------------------------------------------------------
//  PhysicalModel* CreatePhysicalModel(std::string ofType, std::string withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  PhysicalModel objects -
 * in that case, it returns a new  PhysicalModel object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type PhysicalModel.
 *
 * @param <ofType>   specific type of PhysicalModel object to create.
 * @param <withName> name to give to the newly created PhysicalModel object.
 *
 * @return pointer to a new PhysicalModel object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type PhysicalModel.
 */
//------------------------------------------------------------------------------
PhysicalModel* Factory::CreatePhysicalModel(std::string ofType,
                                                    std::string withName)
{
   throw FactoryException("requested object must be of type PhysicalModel");
}

//------------------------------------------------------------------------------
//  PropSetup* CreatePropSetup(std::string ofType, std::string withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  PropSetup objects -
 * in that case, it returns a new  PropSetup object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  PropSetup.
 *
 * @param <ofType>   specific type of  PropSetup object to create.
 * @param <withName> name to give to the newly created  PropSetup object.
 *
 * @return pointer to a new  PropSetup object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  PropSetup.
 */
//------------------------------------------------------------------------------
PropSetup* Factory::CreatePropSetup(std::string ofType,
                                            std::string withName)
{
   throw FactoryException("requested object must be of type PropSetup");
}

//------------------------------------------------------------------------------
//  CalculatedParameter* CreateCalculatedParameter(std::string ofType,
//                                                 std::string withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  CalculatedParameter
 * objects - in that case, it returns a new  CalculatedParameter object.
 * Otherwise, it throws an exception indicating that the class does not create
 * objects of type  CalculatedParameter.
 *
 * @param <ofType>   specific type of  CalculatedParameter object to create.
 * @param <withName> name to give to the newly created  CalculatedParameter
 *                   object.
 *
 * @return pointer to a new  CalculatedParameter object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  CalculatedParameter.
 */
//------------------------------------------------------------------------------
//CalculatedParameter* Factory::CreateCalculatedParameter(
//                                                      std::string ofType,
//                                                      std::string withName)
//{
//   throw FactoryException(
   //                  "requested object must be of type CalculatedParameter");
//}

//------------------------------------------------------------------------------
//  StopCondition* CreateStopCondition(std::string ofType,
//                                     std::string withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  StopCondition objects -
 * in that case, it returns a new  StopCondition object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  StopCondition.
 *
 * @param <ofType>   specific type of  StopCondition object to create.
 * @param <withName> name to give to the newly created  StopCondition object.
 *
 * @return pointer to a new  StopCondition object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  StopCondition.
 */
//------------------------------------------------------------------------------
StopCondition* Factory::CreateStopCondition(std::string ofType,
                                                    std::string withName)
{
   throw FactoryException("requested object must be of type StopCondition");
}

//------------------------------------------------------------------------------
//  CelestialBody* CreateCelestialBody(std::string ofType,
//                                     std::string withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  CelestialBody objects -
 * in that case, it returns a new  CelestialBody object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  CelestialBody.
 *
 * @param <ofType>   specific type of  CelestialBody object to create.
 * @param <withName> name to give to the newly created  CelestialBody object.
 *
 * @return pointer to a new  CelestialBody object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  CelestialBody.
 */
//------------------------------------------------------------------------------
//CelestialBody* Factory::CreateCelestialBody(std::string ofType,
//                                                    std::string withName)
//{
//   throw FactoryException("requested object must be of type CelestialBody");
//}

//------------------------------------------------------------------------------
//  SolarSystem* CreateSolarSystem(std::string ofType,
//                                 std::string withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  SolarSystem objects -
 * in that case, it returns a new  SolarSystem object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  SolarSystem.
 *
 * @param <ofType>   specific type of  SolarSystem object to create.
 * @param <withName> name to give to the newly created  SolarSystem object.
 *
 * @return pointer to a new  SolarSystem object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  SolarSystem.
 */
//------------------------------------------------------------------------------
//SolarSystem* Factory::CreateSolarSystem(std::string ofType,
//                                                std::string withName)
//{
//   throw FactoryException("requested object must be of type SolarSystem");
//}

//------------------------------------------------------------------------------
//  Solver* CreateSolver(std::string ofType,
//                       std::string withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create Solver objects -
 * in that case, it returns a new  Solver object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  Solver.
 *
 * @param <ofType>   specific type of  Solver object to create.
 * @param <withName> name to give to the newly created  Solver object.
 *
 * @return pointer to a new  Solver object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  Solver.
 */
//------------------------------------------------------------------------------
//   Solver*ÊFactory::CreateSolver(std::string ofType,
//                                         std::string withName)
//{
//   throw FactoryException("requested object must be of type Solver");
//}

//------------------------------------------------------------------------------
//  Subscriber* CreateSubscriber(std::string ofType,
//                               std::string withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  Subscriber objects -
 * in that case, it returns a new  Subscriber object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  Subscriber.
 *
 * @param <ofType>   specific type of  Subscriber object to create.
 * @param <withName> name to give to the newly created  Subscriber object.
 *
 * @return pointer to a new  Subscriber object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  Subscriber.
 */
//------------------------------------------------------------------------------
Subscriber* Factory::CreateSubscriber(std::string ofType,
                                              std::string withName)
{
   throw FactoryException("requested object must be of type Subscriber");
}

//------------------------------------------------------------------------------
//  Command* CreateCommand(std::string ofType,
//                         std::string withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  Command objects -
 * in that case, it returns a new  Command object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  Command.
 *
 * @param <ofType>   specific type of  Command object to create.
 * @param <withName> name to give to the newly created  Command object.
 *
 * @return pointer to a new  Command object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  Command.
 */
//------------------------------------------------------------------------------
Command* Factory::CreateCommand(std::string ofType,
                                        std::string withName)
{
   throw FactoryException("requested object must be of type Command");
}

//------------------------------------------------------------------------------
//  StringArray GetListOfCreatableObjects(void) const
//------------------------------------------------------------------------------
/**
* This method returns the list of creatable objects for the factory.
 *
 * @return list of creatable objects of this factory.
 *
 */
//------------------------------------------------------------------------------
StringArray Factory::GetListOfCreatableObjects(void) const
{
   return creatables;
}

//------------------------------------------------------------------------------
//  bool SetListOfCreatableObjects(void) const
//------------------------------------------------------------------------------
/**
* This method sets the list of creatable objects for the factory.
 *
 * @param <newList>  list of creatable objects of this factory.
 *
 * @return flag indicating success of the operation
 *
 */
//------------------------------------------------------------------------------
bool Factory::SetListOfCreatableObjects(StringArray itsList)
{
   if (!creatables.empty())
      creatables.clear();
   creatables = itsList;
}

//------------------------------------------------------------------------------
//  bool AddCreatableObjects(void) const
//------------------------------------------------------------------------------
/**
* This method adds to the list of creatable objects for the factory.
 *
 * @param <newList>  list of additional creatable objects of this factory.
 *
 * @return flag indicating success of the operation
 *
 */
//------------------------------------------------------------------------------
bool Factory::AddCreatableObjects(StringArray newList)
{
   if (creatables.size() > 0)
   {
      StringArray::iterator si = newList.begin();
      while (si != newList.end())
      {
         creatables.push_back(*si);
         ++si;
      }
   }
   //      creatables.splice(creatables.end(),newList);
   else
      creatables = newList;
   return true;
}

//------------------------------------------------------------------------------
//  Gmat::ObjectType GetFactoryType(void) const
//------------------------------------------------------------------------------
/**
 * This method returns the type of objects created by this factory.
 *
 * @return type of objects created by this factory.
 *
 */
//------------------------------------------------------------------------------
Gmat::ObjectType Factory::GetFactoryType(void) const
{
   return itsType;
}

//---------------------------------
//  protected methods
//---------------------------------
//------------------------------------------------------------------------------
//  Factory(Gmat::ObjectType ofType)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the base class Factory (called by
 * derived classes).  (default constructor)
 *
 * @param <ofType> optional parameter indicating type of objects created by
 *                 this factory.
 */
//------------------------------------------------------------------------------
Factory::Factory(Gmat::ObjectType ofType)
{
   itsType = ofType;
   // creatables list is empty at this point
}

//------------------------------------------------------------------------------
//  Factory(StringArray createList, Gmat::ObjectType ofType)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the base class Factory (called by
 * derived classes), and sets the list of creatable objects. 
 *
 * @param <createList> list of creatable objects for this factory
 * @param <ofType> optional parameter indicating type of objects created by
 *                 this factory.
 */
//------------------------------------------------------------------------------
Factory::Factory(StringArray createList, Gmat::ObjectType ofType)
{
   itsType    = ofType;
   creatables = createList;
}

//------------------------------------------------------------------------------
//  Factory(const Factory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the base class Factory (called by
 * copy constructors of derived classes).  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
Factory::Factory(const Factory& fact) :
itsType (fact.itsType)
{
   if (!creatables.empty())
      creatables.clear();
   creatables = fact.creatables;
}

//------------------------------------------------------------------------------
//  Factory& operator= (const Factory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Factory base class.
 *
 * @param <fact> the factory object whose data to assign to "this" factory.
 *
 * @return "this" factory with data of input factory fact.
 */
//------------------------------------------------------------------------------
Factory& Factory::operator= (const Factory& fact)
{
   itsType    = fact.itsType;
   creatables = fact.creatables;
   return *this;
}

//------------------------------------------------------------------------------
// ~Factory()
//------------------------------------------------------------------------------
/**
 * Destructor for the Factory base class.
 *
 */
//------------------------------------------------------------------------------
Factory::~Factory()
{
//   delete creatables;
   creatables.~vector<std::string>();
}






