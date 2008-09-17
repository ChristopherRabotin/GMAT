//$Header$
//------------------------------------------------------------------------------
//                            Publisher
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: LaMont Ruley
// Created: 2003/09/21
//
/**
 *  Implementation code for the Publisher class.
 */
//------------------------------------------------------------------------------

#include "Publisher.hpp"
#include "PublisherException.hpp"
#include "MessageInterface.hpp"
#include "Moderator.hpp"
#include <string>

//#define DBGLVL_PUBLISHER_SUBS 1
//#define DBGLVL_PUBLISHER_REGISTER 2
//#define DBGLVL_PUBLISHER_PUBLISH 1
//#define DBGLVL_PUBLISHER_CLEAR 1

// Initialize the singleton
Publisher* Publisher::instance = NULL;

//------------------------------------------------------------------------------
// Publisher* Instance(void)
//------------------------------------------------------------------------------
Publisher* Publisher::Instance()
{
   if (instance == NULL)
      instance = new Publisher;
   return instance;
}

//------------------------------------------------------------------------------
// Publisher(void)
//------------------------------------------------------------------------------
Publisher::Publisher() :
   providerID          (-1),
   currentProvider     (-1),
   runState            (Gmat::IDLE),
   internalCoordSystem (NULL),
   dataCoordSystem     (NULL),
   dataMJ2000EqOrigin  (NULL)
{
}

//------------------------------------------------------------------------------
// ~Publisher(void)
//------------------------------------------------------------------------------
Publisher::~Publisher()
{
   subs.clear();
   coordSysMap.clear();
}

//------------------------------------------------------------------------------
// bool Subscribe(Subscriber *s)
//------------------------------------------------------------------------------
bool Publisher::Subscribe(Subscriber *s)
{
   #if DBGLVL_PUBLISHER_SUBS
   MessageInterface::ShowMessage
      ("Publisher::Subscribe() sub = %s\n", s->GetName().c_str());
   #endif
   
   if (!s)
      return false;
   
   subs.push_back(s);
   s->SetProviderId(currentProvider);
   return true;
}

//------------------------------------------------------------------------------
// bool Unsubscribe(Subscriber *s)
//------------------------------------------------------------------------------
bool Publisher::Unsubscribe(Subscriber *s)
{
   #if DBGLVL_PUBLISHER_SUBS
   MessageInterface::ShowMessage
      ("Publisher::Unsubscribe() sub = %s\n", s->GetName().c_str());
   #endif
   
   if (!s)
      return false;
   
   subs.remove(s);
   return true;
}

//------------------------------------------------------------------------------
// bool UnsubscribeAll()
//------------------------------------------------------------------------------
bool Publisher::UnsubscribeAll()
{
   #if DBGLVL_PUBLISHER_SUBS
   MessageInterface::ShowMessage
      ("Publisher::UnsubscribeAll() entered. Clearing %d subscribers\n",
       subs.size());
   #endif
   
   subs.clear();
   
   ClearPublishedData();
   
   // delete locally created coordinate systems
   std::map<std::string, CoordinateSystem*>::iterator iter;
   for (iter = coordSysMap.begin(); iter != coordSysMap.end(); ++iter)
   {
      if ((iter->first).find("Local-") != std::string::npos)
      {
         #if DBGLVL_PUBLISHER
         MessageInterface::ShowMessage
            ("Publisher::UnsubscribeAll() deleting %s\n", iter->first.c_str());
         #endif
         
         delete iter->second;
      }
   }
   
   coordSysMap.clear();
   return true;
}

//------------------------------------------------------------------------------
// bool Publish(Real *data, Integer count = false)
//------------------------------------------------------------------------------
bool Publisher::Publish(Integer id, Real *data, Integer count)
{
   #if DBGLVL_PUBLISHER_PUBLISH > 1
   MessageInterface::ShowMessage
      ("Publisher::Publish(Real) entered, id=%d, providerID=%d, count=%d\n",
       id, providerID, count);
   #endif
   
   // No subscribers
   if (subs.empty())
   {
      //#if DBGLVL_PUBLISHER_PUBLISH
      MessageInterface::ShowMessage
         ("*** WARNING *** Publisher::Publish() There are no subscribers, "
          "so just returning false\n");
      //#endif      
      return false;
   }
   
   if ((id < 0) || (id > providerID))
   {
      //throw PublisherException
      //   ("Real data provider has not registered with the Publisher.");
      //#if DBGLVL_PUBLISHER_PUBLISH
      MessageInterface::ShowMessage
         ("*** WARNING *** Publisher::Publish() Real data provider has not "
          "registered with the Publisher, so just returning false\n");
      //#endif
      return false;
   }
   
   if (id != currentProvider) {
      currentProvider = id;
      UpdateProviderID(id);
   }
   
   // Convert the data into a string for distribution
   char stream[4096] = "";
   
   for (Integer i = 0; i < count; ++i)
   {
      sprintf(stream, "%s%12lf", stream, data[i]);
      if (i < count - 1)
         strcat(stream, ", ");
      else
         strcat(stream, "\n");
   }
   
   
   #if DBGLVL_PUBLISHER_PUBLISH
   MessageInterface::ShowMessage
      ("Publisher::Publish() calling ReceiveData() number of sub = %d\n",
       subs.size());
   #endif
   
   std::list<Subscriber*>::iterator current = subs.begin();
   while (current != subs.end())
   {
      #if DBGLVL_PUBLISHER_PUBLISH > 1
      MessageInterface::ShowMessage("Publisher::Publish() sub = %s\n",
                                    (*current)->GetName().c_str());
      #endif
      
      if (!(*current)->ReceiveData(stream))
         return false;
      if (!(*current)->ReceiveData(data, count))
         return false;
      current++;
   }
   return true;
}


//------------------------------------------------------------------------------
// bool Publish(char *data, Integer count = 0)
//------------------------------------------------------------------------------
bool Publisher::Publish(Integer id, char *data, Integer count)
{
   Integer i;
    
   // No subscribers
   if (subs.empty())
      return false;

   #if DBGLVL_PUBLISHER_PUBLISH > 1
   MessageInterface::ShowMessage("Publisher::Publish(char) id = %d\n", id);
   #endif
   
   if ((id < 0) || (id > providerID))
   {
      throw PublisherException
         ("Character data provider has not registered with the Publisher.");
   }
   
   if (id != currentProvider) {
      currentProvider = id;
      UpdateProviderID(id);
   }
   
   // Convert the data into a string for distribution
   char stream[4096];

   if (count)
   {
      for (i = 0; i < count; ++i)
         stream[i] = data[i];
      stream[i] = '\0';
   }
   else
   {
      strcpy(stream, data);
   }
   
   strcat(stream, "\n");

   std::list<Subscriber*>::iterator current = subs.begin();
   while (current != subs.end())
   {
      if (!(*current)->ReceiveData(stream))
         return false;
      current++;
   }

   return true;
}

//------------------------------------------------------------------------------
// bool Publish(Integer id, Integer *data, Integer count = false)
//------------------------------------------------------------------------------
bool Publisher::Publish(Integer id, Integer *data, Integer count)
{
   // No subscribers
   if (subs.empty())
      return false;
   
   #if DBGLVL_PUBLISHER_PUBLISH > 1
   MessageInterface::ShowMessage("Publisher::Publish(Integer) id = %d\n", id);
   #endif
   
   if ((id < 0) || (id > providerID))
      throw PublisherException
         ("Integer data provider has not registered with the Publisher.");

   if (id != currentProvider) {
      currentProvider = id;
      UpdateProviderID(id);
   }

   // Convert the data into a string for distribution
   char stream[4096];

   for(Integer i = 0; i < count; ++i)
   {
      sprintf(stream, "%s%d", stream, data[i]);
      if (i < count - 1)
         strcat(stream, ", ");
      else
         strcat(stream, "\n");
   }

   std::list<Subscriber*>::iterator current = subs.begin();
   while (current != subs.end())
   {
      if (!(*current)->ReceiveData(stream))
         return false;
      current++;
   }

   return true;
}

//------------------------------------------------------------------------------
// bool FlushBuffers()
//------------------------------------------------------------------------------
bool Publisher::FlushBuffers()
{
   // No subscribers
   if (subs.empty())
      return false;
   
   std::list<Subscriber*>::iterator current = subs.begin();
   while (current != subs.end())
   {
      if (!(*current)->FlushData())
         return false;
      current++;
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool NotifyEndOfRun()
//------------------------------------------------------------------------------
bool Publisher::NotifyEndOfRun()
{
   // No subscribers
   if (subs.empty())
      return false;
   
   std::list<Subscriber*>::iterator current = subs.begin();
   while (current != subs.end())
   {
      if (!(*current)->SetEndOfRun())
         return false;
      current++;
   }
   
   return true;
}


//------------------------------------------------------------------------------
// void ClearPublishedData()
//------------------------------------------------------------------------------
void Publisher::ClearPublishedData()
{   
   #if DBGLVL_PUBLISHER_CLEAR
   MessageInterface::ShowMessage
      ("Publisher::ClearPublishedData() Clearing %d providers\n", objectMap.size());
   #endif
   
   objectMap.clear();
   elementMap.clear();
   providerID = -1;
   currentProvider = -1;
   
   // Clear subscriber's data
   std::list<Subscriber*>::iterator current = subs.begin();
   while (current != subs.end())
   {
      (*current)->ClearDataLabels();
      current++;
   }
}


//------------------------------------------------------------------------------
// Integer RegisterPublishedData(const StringArray& owners, 
//                               const StringArray& elements)
//------------------------------------------------------------------------------
Integer Publisher::RegisterPublishedData(const StringArray& owners, 
                                         const StringArray& elements)
{
   #if DBGLVL_PUBLISHER_REGISTER > 1
   MessageInterface::ShowMessage("Publisher::RegisterPublishedData() entered\n");
   for (unsigned int i=0; i<owners.size(); i++)
      MessageInterface::ShowMessage("   owner[%d]=%s\n", i, owners[i].c_str());
   for (unsigned int i=0; i<elements.size(); i++)
      MessageInterface::ShowMessage("   elements[%d]=%s\n", i, elements[i].c_str());   
   MessageInterface::ShowMessage
      ("   providerID=%d, subs.size()=%d\n", providerID, subs.size());
   #endif
   
   if (subs.empty())
   {
      // Let's just show warning (loj: 2008.06.17)
      //throw PublisherException("There are no registered subscribers.");
      MessageInterface::ShowMessage
         ("*** WARNING *** Publisher::RegisterPublishedData() There are no "
          "subscribers to register data\n");
      return providerID;
   }
   
   // Do this only when there are subscribers (loj: 2008.06.17)
   //objectMap.push_back(StringArray(owners));
   //elementMap.push_back(StringArray(elements));
   
   // Increment provider count only when there are registered subscribers (loj: 2008.06.17)
   //++providerID;
   objectMap.push_back(StringArray(owners));
   elementMap.push_back(StringArray(elements));
   ++providerID;
   
   std::list<Subscriber*>::iterator current = subs.begin();
   while (current != subs.end())
   {
      
      #if DBGLVL_PUBLISHER_REGISTER > 1
      MessageInterface::ShowMessage
         ("   calling <%s>->SetDataLabels()\n", (*current)->GetName().c_str());
      #endif
      
      (*current)->SetDataLabels(elements);
      current++;
   }
   
   #if DBGLVL_PUBLISHER_REGISTER
   MessageInterface::ShowMessage("   objectMap.size()=%d\n", objectMap.size());
   MessageInterface::ShowMessage
      ("Publisher::RegisterPublishedData() returning %d\n", providerID);
   #endif
   
   return providerID;
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string& type)
//------------------------------------------------------------------------------
const StringArray& Publisher::GetStringArrayParameter(const std::string& type)
{
   if ((currentProvider < 0) || (currentProvider > providerID))
      throw PublisherException("Data provider id out of range.");
   
   if (type == "SpaceObjectMap")
      return objectMap[currentProvider];
   
   if (type == "PublishedDataMap")
      return elementMap[currentProvider];
   
   throw PublisherException("Unknown StringArray type requested.");
}


//------------------------------------------------------------------------------
// void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/*
 * Sets the internal coordinate system.
 *
 * @parameter  cs  Internal coordinate system (usally EarthMJ2000Eq)
 */
//------------------------------------------------------------------------------
void Publisher::SetInternalCoordSystem(CoordinateSystem *cs)
{
   if (cs != NULL)
      internalCoordSystem = cs;
}


//------------------------------------------------------------------------------
// void SetDataCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/*
 * Sets the coordinate system of data to subscribers. It also adds the coordinate
 * system to the map, if it is not found.
 *
 * @parameter  cs  Coordinate system which data represents
 */
//------------------------------------------------------------------------------
void Publisher::SetDataCoordSystem(CoordinateSystem *cs)
{
   if (cs == NULL)
      return;
   
   dataCoordSystem = cs;
   std::list<Subscriber*>::iterator current = subs.begin();
   while (current != subs.end())
   {
      (*current)->SetDataCoordSystem(cs);
      current++;
   }
   
   // if coordinate system not found in the map, add
   std::string csName = cs->GetName();
   if (coordSysMap.find(csName) != coordSysMap.end())
      coordSysMap[csName] = cs;
}


//------------------------------------------------------------------------------
// void SetDataMJ2000EqOrigin(CelestialBody *cb)
//------------------------------------------------------------------------------
/*
 * Sets origin of MJ2000Eq coordinate system which data represents. If matching
 * coordinate system name found from the map, it sets to data coordinate system,
 * otherwise, it creates new coordinate system.
 *
 * @parameter  cb  Origin of MJ2000Eq coordinate system which data represents
 */
//------------------------------------------------------------------------------
void Publisher::SetDataMJ2000EqOrigin(CelestialBody *cb)
{
   if (cb == NULL)
      return;
   
   #if DBGLVL_PUBLISHER_DATA_REP
   MessageInterface::ShowMessage
      ("Publisher::SetDataMJ2000EqOrigin() cb=%s<%p>\n", cb->GetName().c_str(),
       cb);
   #endif
   
   dataMJ2000EqOrigin = cb;
   std::string originName = cb->GetName();
   std::string csName = originName + "MJ2000Eq";
      
   std::map<std::string, CoordinateSystem*>::iterator csIter =
      coordSysMap.find(csName);
   
   if (coordSysMap.find(csName) != coordSysMap.end())
   {
      // set coordinate system from the map
      dataCoordSystem = coordSysMap.find(csName)->second;
   }
   else
   {
      // check as local name
      csName = "Local-" + csName;
      if (coordSysMap.find(csName) != coordSysMap.end())
         dataCoordSystem = coordSysMap.find(csName)->second;
      else
      {
         // Create coordinate system if not exist
         CoordinateSystem *newCs = (CoordinateSystem*)internalCoordSystem->Clone();
         newCs->SetName(csName);
         newCs->SetStringParameter("Origin", originName);
         newCs->SetRefObject(cb, Gmat::CELESTIAL_BODY, originName);
         newCs->Initialize();
         coordSysMap[csName] = newCs;
         dataCoordSystem = newCs;
         
         #if DBGLVL_PUBLISHER_DATA_REP
         MessageInterface::ShowMessage
            ("   ===> %s not found in the map, so created <%p>\n", csName.c_str(),
             newCs);
         #endif
      }
   }
   
   // set to subscribers
   std::list<Subscriber*>::iterator current = subs.begin();
   while (current != subs.end())
   {
      (*current)->SetDataMJ2000EqOrigin(cb);
      (*current)->SetDataCoordSystem(dataCoordSystem);
      current++;
   }
}


//------------------------------------------------------------------------------
// void SetRunState(const Gmat::RunState state)
//------------------------------------------------------------------------------
void Publisher::SetRunState(const Gmat::RunState state)
{
   runState = state;
   std::list<Subscriber*>::iterator current = subs.begin();
   while (current != subs.end())
   {
      (*current)->SetRunState(runState);
      current++;
   }
}


//------------------------------------------------------------------------------
// void UpdateProviderID(Integer newId)
//------------------------------------------------------------------------------
void Publisher::UpdateProviderID(Integer newId)
{
   std::list<Subscriber*>::iterator current = subs.begin();
   while (current != subs.end())
   {
      (*current)->SetProviderId(newId);
      current++;
   }
}
