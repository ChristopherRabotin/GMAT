//$Id$
//------------------------------------------------------------------------------
//                                  Publisher
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: LaMont Ruley
// Created: 2003/10/21
/**
 * Definition for the Publisher class.
 */
//------------------------------------------------------------------------------

#ifndef Publisher_hpp
#define Publisher_hpp

#include "Subscriber.hpp"
#include <list>
#include <vector>
#include <map>


class GMAT_API Publisher
{
public:
   static Publisher*    Instance();
   
   // destructor
   virtual ~Publisher();
   
   bool Subscribe(Subscriber *s);
   bool Unsubscribe(Subscriber *s);
   bool UnsubscribeAll();
   
   bool Publish(GmatBase *provider, Integer id, Real *data, Integer count);
   bool Publish(Integer id, char *data, Integer count = 0);
   bool Publish(Integer id, Integer *data, Integer count);
   
   bool FlushBuffers(bool endOfDataBlock = true);
   bool NotifyEndOfRun();
   
   const std::list<Subscriber*> GetSubscriberList();
   
   // Interface methods used to identify the data sent to the publisher and
   // subscribers
   Integer              RegisterPublishedData(GmatBase *provider, Integer id,
                                              const StringArray& owners,
                                              const StringArray& elements);
   void                 UnregisterPublishedData(GmatBase *provider);
   void                 ClearPublishedData();
   const StringArray&   GetStringArrayParameter(const std::string& type);
   void                 SetInternalCoordSystem(CoordinateSystem *cs);
   void                 SetDataCoordSystem(CoordinateSystem *cs);
   void                 SetDataMJ2000EqOrigin(CelestialBody *cb);
   
   // Interfaces used to update the state of the running system
   void                 SetRunState(const Gmat::RunState state);
   
   void                 SetManeuvering(GmatBase *originator,
                                       bool flag, Real epoch,
                                       const std::string &satNames,
                                       const std::string &desc);
   void                 SetManeuvering(GmatBase *originator,
                                       bool flag, Real epoch,
                                       const StringArray &satNames,
                                       const std::string &desc);
   bool                 GetManeuvering();
   
   void                 SetScPropertyChanged(GmatBase *originator, Real epoch,
                                             const std::string &satName,
                                             const std::string &desc);
   
   CoordinateSystem* GetInternalCoordSystem() { return internalCoordSystem; }
   CoordinateSystem* GetDataCoordSystem() { return dataCoordSystem; }
   CelestialBody* GetDataMJ2000EqOrigin() { return dataMJ2000EqOrigin; }
   inline Gmat::RunState GetRunState();
   
private:
   /// The singleton
   static Publisher         *instance;
   /// List of the subscribers
   std::list<Subscriber*>   subscriberList;
   /// Index used to identify number of registered data providers
   Integer                  providerId;
   /// ID for the current data provider
   Integer                  currProviderId;
   /// Arrays used to track objects for published data
   std::vector<StringArray> objectArray;
   /// Arrays used to track elements for published data
   std::vector<StringArray> elementArray;
   
   /// State of the system (used to track data for display or suppression)
   Gmat::RunState           runState;
   /// Flag indicating maneuvering
   bool                     maneuvering;
   /// Internal coordinate system
   CoordinateSystem         *internalCoordSystem;
   /// Coordinate system of data
   CoordinateSystem         *dataCoordSystem;
   /// Origin of MJ2000Eq of data
   CelestialBody            *dataMJ2000EqOrigin;
   /// Map of coordinate system of data
   std::map<std::string, CoordinateSystem*> coordSysMap;
   
   /// published data info
   struct DataType
   {
      StringArray    labels;
      Integer        id;
      DataType(StringArray labs, Integer pos)
      {
         labels = labs;
         id = pos;
      };
   };
   
   /// published data map
   std::map<GmatBase*, std::vector<DataType>* > providerMap;
   
   void                 UpdateProviderId(Integer newId);
   
   // for debug
   void                 ShowSubscribers();
   
   // default constructor
   Publisher();
   // assignment operator
   Publisher& operator=(const Publisher &right);
};



inline Gmat::RunState Publisher::GetRunState()
{
   return runState; 
}

#endif // Publisher_hpp
