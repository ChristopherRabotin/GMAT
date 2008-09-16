//$Header$
//------------------------------------------------------------------------------
//                         SubscriberFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2003/10/22
//
/**
*  This class is the factory class for Subscribers.
 */
//------------------------------------------------------------------------------
#ifndef SubscriberFactory_hpp
#define SubscriberFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "Subscriber.hpp"

class GMAT_API SubscriberFactory : public Factory
{
public:
   Subscriber*  CreateSubscriber(const std::string &ofType,
                                 const std::string &withName = "",
                                 const std::string &fileName = "");

   // default constructor
   SubscriberFactory();
   // constructor
   SubscriberFactory(StringArray createList);
   // copy constructor
   SubscriberFactory(const SubscriberFactory& fact);
   // assignment operator
   SubscriberFactory& operator= (const SubscriberFactory& fact);

   // destructor
   ~SubscriberFactory();

protected:
      // protected data

private:
      // private data


};

#endif // SubscriberFactory_hpp




