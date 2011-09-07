//$Id$
//------------------------------------------------------------------------------
//                                ConfigManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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

#include "ODEModel.hpp"
#include "Subscriber.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "PropSetup.hpp"
#include "Spacecraft.hpp"
#include "StopCondition.hpp"
#include "PhysicalModel.hpp"
#include "Propagator.hpp"
#include "Parameter.hpp"
#include "GmatCommand.hpp"
#include "Burn.hpp"
#include "Solver.hpp"
#include "AtmosphereModel.hpp"
#include "Function.hpp"
#include "Hardware.hpp"
#include "CoordinateSystem.hpp"
#include "CalculatedPoint.hpp"

class MeasurementModel;
class CoreMeasurement;
class DataFile;
class ObType;
class TrackingSystem;
class TrackingData;
class EventLocator;


/**
 * Class used to manage configured objects prior to cloning into the Sandbox.
 */
class GMAT_API ConfigManager
{
public:
   static ConfigManager*   Instance();
   
   std::string         GetNewName(const std::string &name, Integer startCount);
   
   void                AddObject(Gmat::ObjectType objType, GmatBase *obj);
   std::string         AddClone(const std::string &name);
   void                AddPhysicalModel(PhysicalModel *pm);
   void                AddPropagator(Propagator *prop);
   void                AddODEModel(ODEModel *fm);
   void                AddSubscriber(Subscriber *subs);
   void                AddSolarSystem(SolarSystem *solarSys);
   void                AddPropSetup(PropSetup *propSetup);
   void                AddSpacecraft(SpaceObject *sc);
   void                AddSpacePoint(SpacePoint *sp);
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

   void                AddMeasurementModel(MeasurementModel *mModel);
   void                AddMeasurement(CoreMeasurement *meas);
   void                AddDataFile(DataFile *meas);
   void                AddObType(ObType *meas);
   void                AddEventLocator(EventLocator *el);
   void                AddTrackingSystem(TrackingSystem *ts);
   void                AddTrackingData(TrackingData *td);

   const StringArray&  GetListOfAllItems();
   const StringArray&  GetListOfItems(Gmat::ObjectType itemType);
   const StringArray&  GetListOfItems(const std::string &typeName);
   const StringArray&  GetListOfItemsHas(Gmat::ObjectType type,
                                         const std::string &name,
                                         bool includeSysParam = true);
   GmatBase*           GetFirstItemUsing(Gmat::ObjectType type,
                                         const std::string &name,
                                         bool includeSysParam = true);
   GmatBase*           GetItem(const std::string &name);
   
   bool                RenameItem(Gmat::ObjectType itemType,
                                  const std::string &oldName,
                                  const std::string &newName);
   
   bool                RemoveAllItems();
   bool                RemoveItem(Gmat::ObjectType type, const std::string &name);
   bool                ReconfigureItem(GmatBase *newobj, const std::string &name);
   
   
   PhysicalModel*      GetPhysicalModel(const std::string &name);
   Propagator*         GetPropagator(const std::string &name);
   ODEModel*           GetODEModel(const std::string &name);
   SpaceObject*        GetSpacecraft(const std::string &name);
   SpacePoint*         GetSpacePoint(const std::string &name);
   Hardware*           GetHardware(const std::string &name);
   PropSetup*          GetPropSetup(const std::string &name);
   Subscriber*         GetSubscriber(const std::string &name);
   SolarSystem*        GetDefaultSolarSystem();
   SolarSystem*        GetSolarSystemInUse();
   SolarSystem*        GetSolarSystemInUse(const std::string &name);
   StopCondition*      GetStopCondition(const std::string &name);
   Parameter*          GetParameter(const std::string &name);
   Burn*               GetBurn(const std::string &name);
   Solver*             GetSolver(const std::string &name);
   AtmosphereModel*    GetAtmosphereModel(const std::string &name);
   Function*           GetFunction(const std::string &name);
   CoordinateSystem*   GetCoordinateSystem(const std::string &name);
   CalculatedPoint*    GetCalculatedPoint(const std::string &name);
   MeasurementModel*   GetMeasurementModel(const std::string &name);
   TrackingSystem*     GetTrackingSystem(const std::string &name);
   TrackingData*       GetTrackingData(const std::string &name);

   DataFile *          GetDataStream(const std::string &name);

   EventLocator*       GetEventLocator(const std::string &name);

   bool                HasConfigurationChanged();
   void                ConfigurationChanged(bool tf);
   ObjectMap*          GetObjectMap();
   
private:
   
   /// The singleton instance
   static ConfigManager*               theConfigManager;
   /// The managed objects
   std::vector<GmatBase*>              objects;
   std::vector<GmatBase*>              newObjects;
   /// A list of the names of the managed objects
   StringArray                         listOfItems;
   /// Mapping between the object names and their pointers
   std::map<std::string, GmatBase *>   mapping;
   /// Flag indicating that a managed object has been changed by a user
   bool                                objectChanged;
   
   // Treat default and in use solar system separately until we can manage solar 
   // system by name.  All solar system names are "SolarSystem" for now.
   
   /// Default Solar Systems 
   SolarSystem *defaultSolarSystem;
   /// Solar Systems in use
   SolarSystem *solarSystemInUse;
   
   void                AddObject(GmatBase* obj);
   
   // Hide the default constructor and destructor to preserve singleton status
   ConfigManager();
   ~ConfigManager();
   
};


#endif // ConfigManager_hpp

