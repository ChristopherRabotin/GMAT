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


//---------------------------------
// public members
//---------------------------------

//------------------------------------------------------------------------------
//  Report()
//------------------------------------------------------------------------------
/**
 * Constructs the Report Command (default constructor).
 */
//------------------------------------------------------------------------------
Report::Report() :
   GmatCommand ("Report"),
   rfName      (""),
   reporter    (NULL),
   reportID    (-1),
   needsHeaders(true)
{
}


//------------------------------------------------------------------------------
//  ~Report()
//------------------------------------------------------------------------------
/**
 * Destroys the Report Command (destructor).
 */
//------------------------------------------------------------------------------
Report::~Report()
{
}


//------------------------------------------------------------------------------
//  Report(const Report &rep)
//------------------------------------------------------------------------------
/**
 * Constructs the Report Command based on another instance (copy constructor).
 * 
 * @param rep The Report that is copied.
 */
//------------------------------------------------------------------------------
Report::Report(const Report &rep) :
   GmatCommand    (rep),
   rfName         (rep.rfName),
   reporter       (NULL),
   reportID       (-1),
   needsHeaders   (rep.needsHeaders)
{
   parmNames = rep.parmNames;
   parms.clear();
}


//------------------------------------------------------------------------------
//  Report& operator=(const Report &rep)
//------------------------------------------------------------------------------
/**
 * Sets this Report Command to match another instance (Assignment operator).
 * 
 * @param rep The Report that is copied.
 * 
 * @return This instance, configured to match the other and ready for 
 *         initialization.
 */
//------------------------------------------------------------------------------
Report& Report::operator=(const Report &rep)
{
   if (this != &rep)
   {
      rfName = rep.rfName;
      reporter = NULL;
      reportID = -1;

      parmNames = rep.parmNames;
      parms.clear();
      needsHeaders = rep.needsHeaders;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                    const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the ReportFile and Parameter objects used bt the Report command.
 * 
 * This method received the global instances of the objects used in the Report
 * command.  It checks their types and stores their names, so that the the 
 * objects can be retrieved from the local store in the Sandbox during 
 * initiialization.  It also tells the ReportFile instance that it will need
 * to be ready to receive data from a ReportCommand, so that the ReportFile does
 * not erroneously inform the user that no data will be written to the 
 * ReportFile.
 * 
 * @param <obj> Pointer to the reference object.
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 * @param <index> Index into the object array.
 *
 * @return true if object successfully set, throws otherwise.
 */
//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of this Report.
 *
 * @return clone of the Report.
 */
//------------------------------------------------------------------------------
GmatBase* Report::Clone() const
{
   return new Report(*this);
}



//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Report command.
 *
 * @return true if the Report is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * Write the report data to a ReportFile.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Report::Execute()
{
   // Build the data as a string
   std::stringstream datastream;
   
   // Set the stream to use the settings in the ReportFile
   // Note that this is done here, rather than during initialization, in case
   // the user has changed the values during the run.
   datastream.precision(reporter->GetIntegerParameter(reporter->GetParameterID("Precision")));
   int colWidth = reporter->GetIntegerParameter(reporter->GetParameterID("ColumnWidth"));

   if (needsHeaders && 
       (reporter->GetStringParameter(reporter->GetParameterID("WriteHeaders")) == "On"))
   {
      reporter->TakeAction("ActivateForReport", "On");
      for (StringArray::iterator i = parmNames.begin(); i != parmNames.end(); ++i)
      {
         datastream.width(colWidth);
         datastream << (*i);
         datastream << " ";
      }
      std::string header = datastream.str();
      reporter->ReceiveData(header.c_str(), header.length());
      datastream.str("");
   }
   needsHeaders = false;
   
   for (std::vector<Parameter*>::iterator i = parms.begin(); i != parms.end(); ++i)
   {
      datastream.width(colWidth);
      datastream << (*i)->EvaluateReal() << " ";
   }

   // Publish it
// This is how it should be done:
//   reportID = reporter->GetProviderId();
//   #ifdef DEBUG_REPORTING
//      MessageInterface::ShowMessage("Reporting to subscriber %d\n", reportID);
//   #endif
//   publisher->Publish(reportID, "Here is some data");
   
   // Publisher seems broken right now -- do it by hand
   std::string data = datastream.str();
   reporter->TakeAction("ActivateForReport", "On");
   bool retval = reporter->ReceiveData(data.c_str(), data.length());
   reporter->TakeAction("ActivateForReport", "Off");
   
   BuildCommandSummary(true);   
   return retval;
}
