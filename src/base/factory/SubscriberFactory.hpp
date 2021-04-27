//$Id$
//------------------------------------------------------------------------------
//                         SubscriberFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
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
   GmatBase*    CreateObject(const std::string &ofType,
                             const std::string &withName = "");
   Subscriber*  CreateSubscriber(const std::string &ofType,
                                 const std::string &withName = "");
   
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




