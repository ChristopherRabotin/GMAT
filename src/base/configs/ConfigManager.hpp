//$Header$
//------------------------------------------------------------------------------
//                                ConfigManager
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/10/27
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Configuration manager used to manage configured (i.e. named) GMAT objects.
 */
//------------------------------------------------------------------------------

#ifndef ConfigManager_hpp
#define ConfigManager_hpp

#include <vector>
#include <map>

#include "ForceModel.hpp"
#include "Subscriber.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "PropSetup.hpp"
#include "Spacecraft.hpp"
#include "StopCondition.hpp"
#include "PhysicalModel.hpp"
#include "Propagator.hpp"
#include "Parameter.hpp"
#include "Command.hpp"
#include "Burn.hpp"
#include "Solver.hpp"
#include "AtmosphereModel.hpp"
#include "Function.hpp"
#include "Hardware.hpp"
#include "CoordinateSystem.hpp"
#include "CalculatedPoint.hpp"


/**
 * Class used to manage configured objects prior to cloning into the Sandbox.
 */
class GMAT_API ConfigManager
{
public:
   static ConfigManager*   Instance();
   
   void                AddPhysicalModel(PhysicalModel *pm);
   void                AddPropagator(Propagator *prop);
   void                AddForceModel(ForceModel *fm);
   void                AddSubscriber(Subscriber *subs);
   void                AddSolarSystem(SolarSystem *solarSys);
   void                AddPropSetup(PropSetup *propSetup);
   void                AddSpacecraft(SpaceObject *sc);
   void                AddHardware(Hardware *hw);
   void                AddStopCondition(StopCondition* stopCond);
   void                AddParameter(Parameter* parameter);
   void                AddBurn(Burn* burn);
   void                AddSolver(Solver *solver);
   void                AddAtmosphereModel(AtmosphereModel *atmosModel);
   void                AddFunction(Function *function);
   void                AddCoordinateSystem(CoordinateSystem *cs);
   void                AddCalculatedPoint(CalculatedPoint *cp);
   
   void                SetDefaultSolarSystem(SolarSystem *ss);
   void                SetSolarSystemInUse(SolarSystem *ss);
   bool                SetSolarSystemInUse(const std::string &name);
   
   StringArray&        GetListOfAllItems();
   StringArray&        GetListOfItems(Gmat::ObjectType itemType);
   GmatBase*           GetItem(const std::string &name);
   
   bool                RenameItem(Gmat::ObjectType itemType,
                                  const std::string &oldName,
                                  const std::string &newName);
   
   bool                RemoveAllItems();
   bool                RemoveItem(Gmat::ObjectType type, const std::string &name);
   StringArray&        GetListOfItemsHas(Gmat::ObjectType type,
                                         const std::string &name,
                                         bool includeSysParam = true);
   
   PhysicalModel*      GetPhysicalModel(const std::string &name);
   Propagator*         GetPropagator(const std::string &name);
   ForceModel*         GetForceModel(const std::string &name);
   SpaceObject*        GetSpacecraft(const std::string &name);
   Hardware*           GetHardware(const std::string &name);
   PropSetup*          GetPropSetup(const std::string &name);
   Subscriber*         GetSubscriber(const std::string &name);
   SolarSystem*        GetDefaultSolarSystem();
   SolarSystem*        GetSolarSystemInUse();
   StopCondition*      GetStopCondition(const std::string &name);
   Parameter*          GetParameter(const std::string &name);
   Burn*               GetBurn(const std::string &name);
   Solver*             GetSolver(const std::string &name);
   AtmosphereModel*    GetAtmosphereModel(const std::string &name);
   Function*           GetFunction(const std::string &name);
   CoordinateSystem*   GetCoordinateSystem(const std::string &name);
   CalculatedPoint*    GetCalculatedPoint(const std::string &name);
   
   // Methods I'm not sure we need
   void                AddCelestialBody(CelestialBody* body);
   void                AddCommand(GmatCommand *cmd);
   CelestialBody*      GetCelestialBody(const std::string &name);
   GmatCommand*        GetCommand(const std::string name);
   
   bool                HasConfigurationChanged();
   void                ConfigurationChanged(bool tf);
   
private:
   /// The singleton instance
   static ConfigManager*               theConfigManager;
   ConfigManager();
   ~ConfigManager();
   
   void                                AddObject(GmatBase* obj);
   
   /// The managed objects
   std::vector<GmatBase*>              objects;
   /// A list of the names of the managed objects
   StringArray                         listOfItems;
   /// Mapping between the object names and their pointers
   std::map<std::string, GmatBase *>   mapping;
   /// Flag indicating that a managed object has been changed by a user
   bool                                objectChanged;

   // Treat default and in use separately until we can manage solar system by name.
   // All sollar system names are "SolarSystem" for now.
   
   /// Default Solar Systems 
   SolarSystem *defaultSolarSystem;
   /// Solar Systems in use
   SolarSystem *solarSystemInUse;
};


#endif // ConfigManager_hpp

