//$Header$
//------------------------------------------------------------------------------
//                            Report
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Purchase
// order MOMS418823
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2005/07/06
//
/**
 *  Class implementation for the Report command.
 */
//------------------------------------------------------------------------------


#include "Report.hpp"
#include "MessageInterface.hpp"
#include <sstream>


Report::Report() :
   GmatCommand ("Report"),
   rfName      (""),
   reporter    (NULL)
{
}


Report::~Report()
{
}


Report::Report(const Report &rep) :
   GmatCommand    (rep),
   rfName         (rep.rfName),
   reporter       (NULL)
{
}


Report& Report::operator=(const Report &rep)
{
   if (this != &rep)
   {
      rfName = rep.rfName;
      reporter = NULL;
   }
   
   return *this;
}


//bool Report::SetRefObjectName(const Gmat::ObjectType type, 
//                              const std::string &name)
//{
//   if (type == 
//}


bool Report::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                          const std::string &name, const Integer index)
{
   #ifdef DEBUG_REPORTING
      MessageInterface::ShowMessage(
         "Report::SetRefObject received a %s named '%s'\n", 
         obj->GetTypeName().c_str(), obj->GetName().c_str());
   #endif

   if (index == 0)
   {
      if (obj->GetTypeName() != "ReportFile")
         throw CommandException("Report command must have a ReportFile name "
            "as the first parameter.");
      rfName = name;
      // Tell the ReportFile object that a command has requested its services
      obj->TakeAction("PassedToReport");
   }
   else 
   {
      #ifdef DEBUG_REPORTING
         MessageInterface::ShowMessage("   Received %s as a Parameter\n", name.c_str());
      #endif
      // All remaining refs should point to Parameter objects
      if (!obj->IsOfType("Parameter"))
         throw CommandException("Report command can only have Parameters "
            "in the list of reported values.");
      parmNames.push_back(name);
   }
   
   return true;
}


GmatBase* Report::Clone() const
{
   return new Report(*this);
}



bool Report::Initialize()
{
   GmatBase *object;
   
   if (objectMap->find(rfName) == objectMap->end())
      throw CommandException(
         "Report command cannot find ReportFile named \"" + (rfName) +
         "\"\n");
   
   reporter = ((ReportFile *)((*objectMap)[rfName]));
   if (reporter->GetTypeName() != "ReportFile")
      throw CommandException(
         "Object named \"" + rfName +
         "\" is not a ReportFile; Report command cannot execute\n");

   for (StringArray::iterator i = parmNames.begin(); i != parmNames.end(); ++i)
   {
      object = ((*objectMap)[*i]);
      if (!object->IsOfType("Parameter"))
         throw CommandException("Parameter type mismatch for " + 
            object->GetName());
      parms.push_back((Parameter *)object);
   }
      
   return true;
}


bool Report::Execute()
{
   std::stringstream datastream;
   datastream.precision(15);
   
   for (std::vector<Parameter*>::iterator i = parms.begin(); i != parms.end(); ++i)
      datastream << (*i)->EvaluateReal() << " ";

//   // Build the data
//   reportID = reporter->GetProviderId();
//      MessageInterface::ShowMessage("Reporting to subscriber %d\n", reportID);
//   
//   // Publish it
//   publisher->Publish(reportID, "Here is some data");
   
   // Publisher seems broken right now -- do it by hand
   std::string data = datastream.str();
   reporter->TakeAction("ActivateForReport", "On");
   bool retval = reporter->ReceiveData(data.c_str(), data.length());
   reporter->TakeAction("ActivateForReport", "Off");
   
   return retval;
}
