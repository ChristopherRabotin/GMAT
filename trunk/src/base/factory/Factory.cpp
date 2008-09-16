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
//GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Returns a GmatBase object pointer to a new object. 
 *
 * @param <ofType>   specific type of Spacecraft object to create.
 * @param <withName> name to give to the newly created Spacecraft object.
 *
 * @return pointer to a new Spacecraft object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of the requested type.
 */
//------------------------------------------------------------------------------
GmatBase* Factory::CreateObject(const std::string &ofType,
                                const std::string &withName)
{
   throw FactoryException("Generic factory creation method not implemented");
}

//------------------------------------------------------------------------------
//  Spacecraft* CreateSpacecraft(const std::string &ofType, const std::string &withName)
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
SpaceObject* Factory::CreateSpacecraft(const std::string &ofType,
                                       const std::string &withName)
{
   throw FactoryException("requested object must be of type SpaceObject");
}

//------------------------------------------------------------------------------
//  GroundStation* CreateGroundStation(const std::string &ofType, const std::string &withName)
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
//   GroundStation* Factory::CreateGroundStation(const std::string &ofType,
//                                                   const std::string &withName);
//{
//   throw FactoryException("requested object must be of type GroundStation");
//}

//------------------------------------------------------------------------------
//  Propagator* CreatePropagator(const std::string &ofType, const std::string &withName)
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
Propagator* Factory::CreatePropagator(const std::string &ofType,
                                      const std::string &withName)
{
   throw FactoryException("requested object must be of type Propagator");
}


//------------------------------------------------------------------------------
//  Parameter* CreateParameter(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create Parameter objects -
 * in that case, it returns a new Parameter object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type Parameter.
 *
 * @param <ofType>   specific type of Parameter object to create.
 * @param <withName> name to give to the newly created Parameter object.
 *
 * @return pointer to a new Parameter object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type Parameter.
 */
//------------------------------------------------------------------------------
Burn* Factory::CreateBurn(const std::string &ofType, const std::string &withName)
{
   throw FactoryException("requested object must be of type Burn");
}

//------------------------------------------------------------------------------
//  Propagator* CreateParameter(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create Parameter objects -
 * in that case, it returns a new Parameter object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type Parameter.
 *
 * @param <ofType>   specific type of Parameter object to create.
 * @param <withName> name to give to the newly created Parameter object.
 *
 * @return pointer to a new Parameter object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type Parameter.
 */
//------------------------------------------------------------------------------
Parameter* Factory::CreateParameter(const std::string &ofType,
                                    const std::string &withName)
{
   throw FactoryException("requested object must be of type Parameter");
}

//------------------------------------------------------------------------------
//  ForceModel* CreateForceModel(const std::string &ofType, const std::string &withName)
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
ForceModel* Factory::CreateForceModel(const std::string &ofType,
                                      const std::string &withName)
{
   throw FactoryException("requested object must be of type ForceModel");
}

//------------------------------------------------------------------------------
//  PhysicalModel* CreatePhysicalModel(const std::string &ofType, const std::string &withName)
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
PhysicalModel* Factory::CreatePhysicalModel(const std::string &ofType,
                                            const std::string &withName)
{
   throw FactoryException("requested object must be of type PhysicalModel");
}

//------------------------------------------------------------------------------
//  PropSetup* CreatePropSetup(const std::string &ofType, const std::string &withName)
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
PropSetup* Factory::CreatePropSetup(const std::string &ofType,
                                    const std::string &withName)
{
   throw FactoryException("requested object must be of type PropSetup");
}

//------------------------------------------------------------------------------
//  CalculatedParameter* CreateCalculatedParameter(const std::string &ofType,
//                                                 const std::string &withName)
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
//                                                      const std::string &ofType,
//                                                      const std::string &withName)
//{
//   throw FactoryException(
   //                  "requested object must be of type CalculatedParameter");
//}

//------------------------------------------------------------------------------
//  StopCondition* CreateStopCondition(const std::string &ofType,
//                                     const std::string &withName)
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
StopCondition* Factory::CreateStopCondition(const std::string &ofType,
                                            const std::string &withName)
{
   throw FactoryException("requested object must be of type StopCondition");
}

//------------------------------------------------------------------------------
//  CalculatedPoint* CreateCalculatedPoint(const std::string &ofType,
//                                         const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  CalculatedPoint objects -
 * in that case, it returns a new  CalculatedPoint object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  CalculatedPoint.
 *
 * @param <ofType>   specific type of  CalculatedPoint object to create.
 * @param <withName> name to give to the newly created  CalculatedPoint object.
 *
 * @return pointer to a new  CalculatedPoint object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  CalculatedPoint.
 */
//------------------------------------------------------------------------------
CalculatedPoint* Factory::CreateCalculatedPoint(const std::string &ofType,
                                                const std::string &withName)
{
   throw FactoryException("requested object must be of type CalculatedPoint");
}


//------------------------------------------------------------------------------
//  CelestialBody* CreateCelestialBody(const std::string &ofType,
//                                     const std::string &withName)
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
CelestialBody* Factory::CreateCelestialBody(const std::string &ofType,
                                            const std::string &withName)
{
   throw FactoryException("requested object must be of type CelestialBody");
}

//------------------------------------------------------------------------------
//  SolarSystem* CreateSolarSystem(const std::string &ofType,
//                                 const std::string &withName)
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
SolarSystem* Factory::CreateSolarSystem(const std::string &ofType,
                                        const std::string &withName)
{
   throw FactoryException("requested object must be of type SolarSystem");
}

//------------------------------------------------------------------------------
//  Solver* CreateSolver(const std::string &ofType,
//                       const std::string &withName)
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
Solver* Factory::CreateSolver(const std::string &ofType,
                              const std::string &withName)
{
   throw FactoryException("requested object must be of type Solver");
}

//------------------------------------------------------------------------------
//  Subscriber* CreateSubscriber(const std::string &ofType, 
//                               const std::string &withName,
//                               const std::string &fileName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  Subscriber objects -
 * in that case, it returns a new  Subscriber object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  Subscriber.
 *
 * @param <ofType>   specific type of  Subscriber object to create.
 * @param <withName> name to give to the newly created  Subscriber object.
 * @param <fileName> file name if subscriber is ReportFile, ignored otherwise
 *
 * @return pointer to a new  Subscriber object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  Subscriber.
 */
//------------------------------------------------------------------------------
Subscriber* Factory::CreateSubscriber(const std::string &ofType,
                                      const std::string &withName,
                                      const std::string &fileName)
{
   throw FactoryException("requested object must be of type Subscriber");
}

//------------------------------------------------------------------------------
//  GmatCommand* CreateCommand(const std::string &ofType,
//                             const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  Command objects -
 * in that case, it returns a new  GmatCommand object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  GmatCommand.
 *
 * @param <ofType>   specific type of  GmatCommand object to create.
 * @param <withName> name to give to the newly created  GmatCommand object.
 *
 * @return pointer to a new  GmatCommand object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  GmatCommand.
 */
//------------------------------------------------------------------------------
GmatCommand* Factory::CreateCommand(const std::string &ofType,
                                    const std::string &withName)
{
   throw FactoryException("requested object must be of type GmatCommand");
}

//------------------------------------------------------------------------------
//  AtmosphereModel* CreateAtmosphereModel(const std::string &ofType,
//                                         const std::string &withName,
//                                         const std::string &forBody)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  AtmosphereModel objects -
 * in that case, it returns a new  AtmosphereModel object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  AtmosphereModel.
 *
 * @param <ofType>   specific type of  AtmosphereModel object to create.
 * @param <forBody>  body of specific type of AtmosphereModel object to create.
 * @param <withName> name to give to the newly created  AtmosphereModel object.
 *
 * @return pointer to a new  AtmosphereModel object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  AtmosphereModel.
 */
//------------------------------------------------------------------------------
AtmosphereModel* Factory::CreateAtmosphereModel(const std::string &ofType,
                                                const std::string &withName,
                                                const std::string &forBody)
{
   throw FactoryException
      ("Factory::CreateAtmosphereModel() must be implemented by AtmosphereFactory()\n");
}

//------------------------------------------------------------------------------
//  Function* CreateFunction(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Must be implemented by derived classes that create  Function objects -
 * in that case, it returns a new  Function object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type  Function.
 *
 * @param <ofType>   specific type of  Function object to create.
 * @param <withName> name to give to the newly created  Function object.
 *
 * @return pointer to a new  Function object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of type  Function.
 */
//------------------------------------------------------------------------------
Function* Factory::CreateFunction(const std::string &ofType,
                                  const std::string &withName)
{
   throw FactoryException
      ("Factory::CreateFunction() must be implemented by FunctionFactory()\n");
}


//------------------------------------------------------------------------------
//  Hardware* CreateHardware(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
* Creates tanks, thrusters, and other hardware attached to the GMAT spacecraft.
 * 
 * Must be implemented by derived classes that create Hardware objects -
 * in that case, it returns a new Hardware object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type Hardware.
 *
 * @param <ofType>   specific type of Hardware object to create.
 * @param <withName> name to give to the newly created Hardware object.
 *
 * @return pointer to a new Hardware object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type Hardware.
 */
//------------------------------------------------------------------------------
Hardware* Factory::CreateHardware(const std::string &ofType,
                                  const std::string &withName)
{
   throw FactoryException
   ("Factory::CreateHardware() must be implemented by the HardwareFactory\n");
}


//------------------------------------------------------------------------------
//  AxisSystem* CreateAxisSystem(const std::string &ofType,
//                               const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creates as AxisSystem object.
 * 
 * Must be implemented by derived classes that create AxisSystem objects -
 * in that case, it returns a new AxisSystem object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type AxisSystem.
 *
 * @param <ofType>   specific type of AxisSystem object to create.
 * @param <withName> name to give to the newly created AxisSystem object.
 *
 * @return pointer to a new AxisSystem object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type AxisSystem.
 */
//------------------------------------------------------------------------------
AxisSystem* Factory::CreateAxisSystem(const std::string &ofType,
                                      const std::string &withName)
{
   throw FactoryException
   ("Factory::CreateAxisSystem() must be implemented by the AxisSystemFactory\n");
}


//------------------------------------------------------------------------------
//  CoordinateSystem* CreateCoordinateSystem(const std::string &ofType,
//                                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creates as CoordinateSystem object.
 * 
 * Must be implemented by derived classes that create CoordinateSystem objects -
 * in that case, it returns a new CoordinateSystem object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type CoordinateSystem.
 *
 * @param <ofType>   specific type of CoordinateSystem object to create.
 * @param <withName> name to give to the newly created CoordinateSystem object.
 *
 * @return pointer to a new CoordinateSystem object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type CoordinateSystem.
 */
//------------------------------------------------------------------------------
CoordinateSystem* Factory::CreateCoordinateSystem(const std::string &ofType,
                                                  const std::string &withName)
{
   throw FactoryException
   ("Factory::CreateCoordinateSystem() must be implemented by the "
    "CoordinateSystemFactory\n");
}


//------------------------------------------------------------------------------
//  MathNode* CreateMathNode(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creates as MathNode object.
 * 
 * Must be implemented by derived classes that create MathNode objects -
 * in that case, it returns a new MathNode object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type MathNode.
 *
 * @param <ofType>   specific type of MathNode object to create.
 * @param <withName> name to give to the newly created MathNode object.
 *
 * @return pointer to a new MathNode object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type MathNode.
 */
//------------------------------------------------------------------------------
MathNode* Factory::CreateMathNode(const std::string &ofType,
                                  const std::string &withName)
{
   throw FactoryException
   ("Factory::CreateMathNode() must be implemented by the MathFactory\n");
}

//------------------------------------------------------------------------------
//  Attitude* CreateAttitude(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creates an Attitude object.
 * 
 * Must be implemented by derived classes that create Attitude objects -
 * in that case, it returns a new Attitude object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type Attitude.
 *
 * @param <ofType>   specific type of Attitude object to create.
 * @param <withName> name to give to the newly created Attitude object.
 *
 * @return pointer to a new Attitude object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type Attitude.
 */
//------------------------------------------------------------------------------
Attitude* Factory::CreateAttitude(const std::string &ofType,
                                  const std::string &withName)
{
   throw FactoryException
   ("Factory::CreateAttitude() must be implemented by the AttitudeFactory\n");
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
   return true;
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
//  Gmat::ObjectType GetFactoryType() const
//------------------------------------------------------------------------------
/**
 * This method returns the type of objects created by this factory.
 *
 * @return type of objects created by this factory.
 *
 */
//------------------------------------------------------------------------------
Gmat::ObjectType Factory::GetFactoryType() const
{
   return itsType;
}


//------------------------------------------------------------------------------
//  bool IsTypeCaseSensitive() const
//------------------------------------------------------------------------------
/**
 * This method returns the type of objects created by this factory.
 *
 * @return type of objects created by this factory.
 *
 */
//------------------------------------------------------------------------------
bool Factory::IsTypeCaseSensitive() const
{
   return isCaseSensitive;
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
   isCaseSensitive = true;
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
   isCaseSensitive = true;
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
   itsType (fact.itsType),
   isCaseSensitive (fact.isCaseSensitive)
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
   isCaseSensitive = fact.isCaseSensitive;
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
   // delete creatables;
   //creatables.~vector<std::string>(); //loj: 4/7/06 commented
}

