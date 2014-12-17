//$Id: DataCallbackFactory.hpp 9460 2011-04-21 22:03:28Z ravimathur $
//------------------------------------------------------------------------------
//                            DataCallbackFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Ravi Mathur
// Created: December 15, 2014
//
/**
 *  Declaration code for the DataCallbackFactory class. With this class data can
 *  be sent to a user-provided callback function as it is computed.
 */
//------------------------------------------------------------------------------
#ifndef DataCallbackFactory_hpp
#define DataCallbackFactory_hpp


#include "datacallback_defs.hpp"
#include "Factory.hpp"
#include "Subscriber.hpp"

class DATACALLBACK_API DataCallbackFactory : public Factory
{
public:
   virtual Subscriber* CreateSubscriber(const std::string &ofType,
                                        const std::string &withName = "",
                                        const std::string &fileName = "");
   
   // default constructor
   DataCallbackFactory();
   // constructor
   DataCallbackFactory(StringArray createList);
   // copy constructor
   DataCallbackFactory(const DataCallbackFactory& fact);
   // assignment operator
   DataCallbackFactory& operator=(const DataCallbackFactory& fact);
   
   virtual ~DataCallbackFactory();
   
};

#endif // DataCallbackFactory_hpp
