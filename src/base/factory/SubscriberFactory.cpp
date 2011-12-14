//$Id$
//------------------------------------------------------------------------------
//                            SubscriberFactory
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
// Author: Wendy Shoan
// Created: 2003/10/22
//
/**
 *  Implementation code for the SubscriberFactory class, responsible for
 *  creating Subscriber objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "SubscriberFactory.hpp"
#include "ReportFile.hpp"
#include "TextEphemFile.hpp"
#include "MessageWindow.hpp"
#include "OpenGlPlot.hpp"
#include "XyPlot.hpp"
#include "EphemerisFile.hpp"
#include "OrbitView.hpp"
#include "GroundTrackPlot.hpp"

#include "OwnedPlot.hpp"
//#include "LocatorView.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested ODEModel class
 * in generic way.
 *
 * @param <ofType> the ODEModel object to create and return.
 * @param <withName> the name to give the newly-created ODEModel object.
 *
 */
//------------------------------------------------------------------------------
Subscriber* SubscriberFactory::CreateObject(const std::string &ofType,
                                          const std::string &withName)
{
   return CreateSubscriber(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreateSubscriber(const std::string &ofType, const std::string &withName,
//                   const std::string &fileName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Subscriber class
 *
 * @param <ofType>   the Subscriber object to create and return.
 * @param <withName> the name to give the newly-created Subscriber object.
 * @param <fileName> the file name if subscriber is ReportFile, ignored otherwise
 *
 */
//------------------------------------------------------------------------------
Subscriber* SubscriberFactory::CreateSubscriber(const std::string &ofType,
                                                const std::string &withName,
                                                const std::string &fileName)
{
   if (ofType == "ReportFile")
      return new ReportFile(ofType, withName, fileName);
   else if (ofType == "TextEphemFile")
      return new TextEphemFile(ofType, withName, fileName);
   else if (ofType == "MessageWindow")
      return new MessageWindow(withName);
   else if (ofType == "XYPlot")
      return new XyPlot(withName);
   else if (ofType == "EphemerisFile")
      return new EphemerisFile(withName);
   else if (ofType == "OpenGLPlot")
      return new OrbitView(withName);
   else if (ofType == "Enhanced3DView")
      return new OrbitView(withName);
   else if (ofType == "OrbitView")
      return new OrbitView(withName);
   else if (ofType == "GroundTrackPlot")
      return new GroundTrackPlot(withName);

   // These are "owned" Subscribers
   else if (ofType == "OwnedPlot")
      return new OwnedPlot(withName);
//   else if (ofType == "LocatorView")
//      return new LocatorView(withName);
   
   return NULL;
}


//------------------------------------------------------------------------------
//  SubscriberFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SubscriberFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
SubscriberFactory::SubscriberFactory() :
Factory(Gmat::SUBSCRIBER)
{
   if (creatables.empty())
   {
      creatables.push_back("ReportFile");
      creatables.push_back("TextEphemFile");
      creatables.push_back("MessageWindow");
      creatables.push_back("XYPlot");
      creatables.push_back("EphemerisFile");
      creatables.push_back("OpenGLPlot");
      creatables.push_back("Enhanced3DView");
      creatables.push_back("OrbitView");
      creatables.push_back("GroundTrackPlot");
      creatables.push_back("OwnedPlot");
//      creatables.push_back("LocatorView");
   }
   
   // Now fill in unviewable subscribers
   // We don't want to show these items in the ResourceTree menu
   if (unviewables.empty())
   {
      // These commands do nothing
      unviewables.push_back("OpenGLPlot");
      unviewables.push_back("Enhanced3DView");

      // These are managed by other objects rather than from the tree
//      unviewables.push_back("LocatorView");
      unviewables.push_back("OwnedPlot");
   }
}


//------------------------------------------------------------------------------
// SubscriberFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SubscriberFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
SubscriberFactory::SubscriberFactory(StringArray createList) :
Factory(createList,Gmat::SUBSCRIBER)
{
}


//------------------------------------------------------------------------------
//  SubscriberFactory(const SubscriberFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SubscriberFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
SubscriberFactory::SubscriberFactory(const SubscriberFactory& fact) :
Factory(fact)
{
}


//------------------------------------------------------------------------------
// SubscriberFactory& operator= (const SubscriberFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the SubscriberFactory class.
 *
 * @param <fact> the SubscriberFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" SubscriberFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
SubscriberFactory& SubscriberFactory::operator= (const SubscriberFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}


//------------------------------------------------------------------------------
// ~SubscriberFactory()
//------------------------------------------------------------------------------
/**
* Destructor for the SubscriberFactory base class.
 *
 */
//------------------------------------------------------------------------------
SubscriberFactory::~SubscriberFactory()
{
   // deletes handled by Factory destructor
}

