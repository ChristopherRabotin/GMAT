//$Header$
//------------------------------------------------------------------------------
//                            SubscriberFactory
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
 *  Implementation code for the SubscriberFactory class, responsible for
 *  creating Subscriber objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "SubscriberFactory.hpp"
#include "ReportFile.hpp"
#include "MessageWindow.hpp"
#include "OpenGlPlot.hpp"
#include "XyPlot.hpp"
#include "MatlabWs.hpp" //loj: 9/4/04 added

//---------------------------------
//  public methods
//---------------------------------

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
      return new ReportFile(withName, fileName);
   else if (ofType == "MessageWindow")
      return new MessageWindow(withName);
   else if (ofType == "OpenGLPlot") //loj: 10/28/04 Changed from OpenGlPlot
      return new OpenGlPlot(withName);
   else if (ofType == "XYPlot")     //loj: 10/28/04 Changed from XyPlot
      return new XyPlot(withName);
   else if (ofType == "MatlabWS")   //loj: 10/28/04 Changed from MatlabWs
      return new MatlabWs(withName);
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
      creatables.push_back("MessageWindow");
      creatables.push_back("OpenGLPlot");
      creatables.push_back("XYPlot");
      creatables.push_back("MatlabWS");
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

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------





