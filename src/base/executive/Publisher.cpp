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
#include <string>
#include "MessageInterface.hpp"

#define DEBUG_SUBSCRIBER 0

// Initialize the singleton
Publisher* Publisher::instance = NULL;

//------------------------------------------------------------------------------
// Publisher* Instance(void)
//------------------------------------------------------------------------------
Publisher* Publisher::Instance(void)
{
   if (instance == NULL)
      instance = new Publisher;
   return instance;
}

//------------------------------------------------------------------------------
// Publisher(void)
//------------------------------------------------------------------------------
Publisher::Publisher(void) :
   providerCount     (0),
   currentProvider   (-1),
   runState          (Gmat::IDLE)
{
}

//------------------------------------------------------------------------------
// ~Publisher(void)
//------------------------------------------------------------------------------
Publisher::~Publisher(void)
{
   subs.clear();
}

//------------------------------------------------------------------------------
// bool Subscribe(Subscriber *s)
//------------------------------------------------------------------------------
bool Publisher::Subscribe(Subscriber *s)
{
#if DEBUG_SUBSCRIBER
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
   subs.clear();
   return true;
}

//------------------------------------------------------------------------------
// bool Publish(Real *data, Integer count = false)
//------------------------------------------------------------------------------
bool Publisher::Publish(Integer id, Real *data, Integer count)
{
   // No subscribers
   if (subs.empty()) {
      return false;
   }
   
   if ((id < 0) || (id >= providerCount))
      throw PublisherException("Data provider has not registered with the Publisher.");

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


#if DEBUG_SUBSCRIBER
   MessageInterface::ShowMessage
      ("Publisher::Publish() calling ReceiveData() number of sub = %d\n",
       subs.size());
#endif

   std::list<Subscriber*>::iterator current = subs.begin();
   while (current != subs.end())
   {
#if DEBUG_SUBSCRIBER
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

   if ((id < 0) || (id >= providerCount))
      throw PublisherException("Data provider has not registered with the Publisher.");

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

   if ((id < 0) || (id >= providerCount))
      throw PublisherException("Data provider has not registered with the Publisher.");

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


Integer Publisher::RegisterPublishedData(const StringArray& owners, 
                                         const StringArray& elements)
{
   objectMap.push_back(StringArray(owners));
   elementMap.push_back(StringArray(elements));
   
   ++providerCount;
   return providerCount-1;
}


const StringArray& Publisher::GetStringArrayParameter(const std::string& type)
{
   if ((currentProvider < 0) || (currentProvider >= providerCount))
      throw PublisherException("Data provider id out of range.");
   
   if (type == "SpaceObjectMap")
      return objectMap[currentProvider];
   
   if (type == "PublishedDataMap")
      return elementMap[currentProvider];
      
   throw PublisherException("Unknown StringArray type requested.");
}


void Publisher::SetRunState(const Gmat::RunState state)
{
   runState = state;
}


void Publisher::UpdateProviderID(Integer newId)
{
   std::list<Subscriber*>::iterator current = subs.begin();
   while (current != subs.end())
   {
      (*current)->SetProviderId(newId);
      current++;
   }
}
