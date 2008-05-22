//$Id$
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
#include "StringUtil.hpp"       // for GetArrayIndex()
#include <sstream>

//#define DEBUG_REPORTING 1
//#define DEBUG_REPORT_SET
//#define DEBUG_WRAPPERS

//---------------------------------
// static data
//---------------------------------
const std::string
Report::PARAMETER_TEXT[ReportParamCount - GmatCommandParamCount] =
{
   "ReportFile",
   "Add",
};

const Gmat::ParameterType
Report::PARAMETER_TYPE[ReportParamCount - GmatCommandParamCount] =
{
   Gmat::STRING_TYPE,        // "ReportFile",
   Gmat::OBJECTARRAY_TYPE,   // "Add",
};


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
   GmatCommand  ("Report"),
   rfName       (""),
   reporter     (NULL),
   reportID     (-1),
   numParams    (0),
   needsHeaders (true)
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
   actualParmNames = rep.actualParmNames;
   parms.clear();
   parmRows.clear();
   parmCols.clear();
   parmWrappers.clear();
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
      actualParmNames = rep.actualParmNames;
      parms.clear();
      parmRows.clear();
      parmCols.clear();
      needsHeaders = rep.needsHeaders;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer Report::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatCommandParamCount; i < ReportParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
         return i;
   }
   
   return GmatCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string Report::GetStringParameter(const Integer id) const
{
   if (id == REPORTFILE)
      return rfName;
   
   return GmatCommand::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string Report::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool Report::SetStringParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
   case REPORTFILE:
      rfName = value;
      return true;
   case ADD:
      #ifdef DEBUG_REPORT_SET
      MessageInterface::ShowMessage
         ("Report::SetStringParameter() Adding parameter '%s'\n", value.c_str());
      #endif
      return AddParameter(value, numParams);
   default:
      return GmatCommand::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool Report::SetStringParameter(const std::string &label,
                                const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool Report::SetStringParameter(const Integer id, const std::string &value,
                                const Integer index)
{
   switch (id)
   {
   case ADD:
      return AddParameter(value, index);
   default:
      return GmatCommand::SetStringParameter(id, value, index);
   }
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const std::string &label,
//                                 const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool Report::SetStringParameter(const std::string &label,
                                const std::string &value,
                                const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& Report::GetStringArrayParameter(const Integer id) const
{
   #ifdef DEBUG_REPORTFILE_GET
   MessageInterface::ShowMessage
      ("Report::GetStringArrayParameter() id=%d, actualParmNames.size()=%d, "
       "numParams=%d\n", id, actualParmNames.size(), numParams);
   #endif
   
   switch (id)
   {
   case ADD:
      return actualParmNames;
   default:
      return GmatCommand::GetStringArrayParameter(id);
   }
}


//------------------------------------------------------------------------------
// StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const StringArray& Report::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& GetWrapperObjectNameArray()
//------------------------------------------------------------------------------
const StringArray& Report::GetWrapperObjectNameArray()
{
   wrapperObjectNames.clear();
   wrapperObjectNames.insert(wrapperObjectNames.end(), actualParmNames.begin(),
                             actualParmNames.end());
   return wrapperObjectNames;
}


//------------------------------------------------------------------------------
// bool SetElementWrapper(ElementWrapper *toWrapper, const std::string &withName)
//------------------------------------------------------------------------------
bool Report::SetElementWrapper(ElementWrapper *toWrapper,
                               const std::string &withName)
{
   #ifdef DEBUG_WRAPPERS   
   MessageInterface::ShowMessage
      ("Report::SetElementWrapper() entered with toWrapper=<%p>, withName='%s'\n",
       toWrapper, withName.c_str());
   #endif
   
   if (toWrapper == NULL)
      return false;
   
   // Do we need any type checking?
   // CheckDataType(toWrapper, Gmat::REAL_TYPE, "Report", true);
   
   bool retval = false;
   ElementWrapper *ew;
   
   //-------------------------------------------------------
   // check parameter names
   //-------------------------------------------------------
   #ifdef DEBUG_WRAPPERS   
   MessageInterface::ShowMessage
      ("   Checking %d Parameters\n", actualParmNames.size());
   for (UnsignedInt i=0; i<actualParmNames.size(); i++)
      MessageInterface::ShowMessage("      %s\n", actualParmNames[i].c_str());
   #endif
   
   Integer sz = actualParmNames.size();
   for (Integer i = 0; i < sz; i++)
   {
      if (actualParmNames.at(i) == withName)
      {
         #ifdef DEBUG_WRAPPERS   
         MessageInterface::ShowMessage
            ("   Found wrapper name \"%s\" in actualParmNames\n", withName.c_str());
         #endif
         if (parmWrappers.at(i) != NULL)
         {
            ew = parmWrappers.at(i);
            parmWrappers.at(i) = toWrapper;
         }
         else
         {
            parmWrappers.at(i) = toWrapper;
         }
         
         retval = true;
      }
   }
   
   return retval;
}


//------------------------------------------------------------------------------
// virtual bool TakeAction(const std::string &action,  
//                         const std::string &actionData = "");
//------------------------------------------------------------------------------
/**
 * This method performs action.
 *
 * @param <action> action to perform
 * @param <actionData> action data associated with action
 * @return true if action successfully performed
 *
 */
//------------------------------------------------------------------------------
bool Report::TakeAction(const std::string &action, const std::string &actionData)
{
   #if DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("Report::TakeAction() action=%s, actionData=%s\n",
       action.c_str(), actionData.c_str());
   #endif
   
   if (action == "Clear")
   {
      parmNames.clear();
      actualParmNames.clear();
      parmRows.clear();
      parmCols.clear();
      
      // Why we need to clear? (loj: 2007.12.19 commented out)
      // We want to preserve parameters to report for ReportFile
      //if (reporter)
      //{
      //   reporter->TakeAction("Clear");
      //   return true;
      //}
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool GetRefObjectName(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Retrieves the reference object names.
 * 
 * @param type The type of the reference object.
 * 
 * @return the name of the object.
 */
//------------------------------------------------------------------------------
std::string Report::GetRefObjectName(const Gmat::ObjectType type) const
{
   switch (type)
   {
   case Gmat::SUBSCRIBER:
      return rfName;
      
   case Gmat::PARAMETER:
      if (parmNames.size() == 0)
         return "";
      else
         return parmNames[0];
   default:
      return GmatCommand::GetRefObjectName(type);
   }
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Accesses arrays of names for referenced objects.
 * 
 * @param type Type of object requested.
 * 
 * @return the StringArray containing the referenced object names.
 */
//------------------------------------------------------------------------------
const StringArray& Report::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   static StringArray refObjectNames;
   refObjectNames.clear();
   
   switch (type)
   {
   case Gmat::SUBSCRIBER:
      refObjectNames.push_back(rfName);
      return refObjectNames;
   case Gmat::PARAMETER:
      return parmNames;
   default:
      return GmatCommand::GetRefObjectNameArray(type);
   }
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
         "Report::SetRefObject received a %s named '%s', index=%d\n", 
         obj->GetTypeName().c_str(), obj->GetName().c_str(), index);
   #endif

   if (type == Gmat::SUBSCRIBER)
   {
      if (obj->GetTypeName() != "ReportFile")
         throw CommandException("Report command must have a ReportFile name "
            "as the first parameter.\n");
      
      rfName = name;
      // Tell the ReportFile object that a command has requested its services
      obj->TakeAction("PassedToReport");
      reporter = (ReportFile*)obj;
      
      // Why we need to clear? (loj: 2007.12.19 commented out)
      // We want to preserve parameters to report for ReportFile
      //reporter->TakeAction("Clear");
   }
   else if (type == Gmat::PARAMETER)
   {
      #ifdef DEBUG_REPORTING
         MessageInterface::ShowMessage("   Received %s as a Parameter\n", name.c_str());
      #endif
         
      // All remaining refs should point to Parameter objects
      if (!obj->IsOfType("Parameter"))
         throw CommandException("Report command can only have Parameters "
            "in the list of reported values.\n");
      
      AddParameter(name, numParams);
      
//       // Handle Array indexing
//       Integer row, col;
//       std::string newName;      
//       GmatStringUtil::GetArrayIndex(name, row, col, newName);
      
//       parmNames.push_back(newName);
//       actualParmNames.push_back(name);
      
//       parmRows.push_back(row);
//       parmCols.push_back(col);
      
      #ifdef __SHOW_NAMES_IN_REPORTFILE__
      // For compare report column header
      if (reporter)
         reporter->AddParameterForTitleOnly(name);
      else
         throw CommandException("Report command has undefined ReportFile object.\n");
      #endif
   }
   
   #ifdef DEBUG_REPORTING
   MessageInterface::ShowMessage("Report::SetRefObject() returning true\n");
   #endif
   
   return true;
}


//---------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type,
//                      const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/*
 * Renames referenced objects
 *
 * @param <type> type of the reference object.
 * @param <oldName> old name of the reference object.
 * @param <newName> new name of the reference object.
 *
 * @return always true to indicate RenameRefObject() was implemented.
 */
//---------------------------------------------------------------------------
bool Report::RenameRefObject(const Gmat::ObjectType type,
                             const std::string &oldName,
                             const std::string &newName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Report::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type == Gmat::SUBSCRIBER)
   {
      if (rfName == oldName)
         rfName = newName;
   }
   else if (type == Gmat::PARAMETER)
   {
      for (UnsignedInt i=0; i<parmNames.size(); i++)
         if (parmNames[i] == oldName)
            parmNames[i] = newName;
      
      for (UnsignedInt i=0; i<actualParmNames.size(); i++)
         if (actualParmNames[i] == oldName)
            actualParmNames[i] = newName;
   }
   // Since parameter name is composed of spacecraftName.dep.paramType or
   // burnName.dep.paramType, check the type first
   else if (type == Gmat::SPACECRAFT || type == Gmat::BURN ||
            type == Gmat::COORDINATE_SYSTEM || type == Gmat::CALCULATED_POINT)
   {
      
      for (UnsignedInt i=0; i<parmNames.size(); i++)
         if (parmNames[i].find(oldName) != std::string::npos)
            parmNames[i] = GmatStringUtil::Replace(parmNames[i], oldName, newName);
      
      for (UnsignedInt i=0; i<actualParmNames.size(); i++)
         if (actualParmNames[i].find(oldName) != std::string::npos)
            actualParmNames[i] =
               GmatStringUtil::Replace(actualParmNames[i], oldName, newName);
      
      generatingString = GmatStringUtil::Replace(generatingString, oldName, newName);
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
   return (new Report(*this));
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(Gmat::WriteMode mode,
//                                        const std::string &prefix,
//                                        const std::string &useName)
//------------------------------------------------------------------------------
const std::string& Report::GetGeneratingString(Gmat::WriteMode mode,
                                               const std::string &prefix,
                                               const std::string &useName)
{
   #ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("Report::GetGeneratingString() rfName='%s', has %d parameters to write\n",
       rfName.c_str(), actualParmNames.size());
   #endif
   
   std::string gen = prefix + "Report " + rfName + " ";
   UnsignedInt numElem = actualParmNames.size();
   
   if (numElem > 1)
   {
      for (UnsignedInt i=0; i<numElem-1; i++)
         gen += actualParmNames[i] + " ";
      
      gen += actualParmNames[numElem-1];
   }
   else if (numElem == 1)
   {
      gen += actualParmNames[0];
   }
   
   generatingString = gen + ";";
   
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
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
   if (GmatCommand::Initialize() == false)
      return false;
   
   parms.clear();
   GmatBase *object, *mapObj;
   
   if ((mapObj = FindObject(rfName)) == NULL)
      throw CommandException(
         "Report command cannot find ReportFile named \"" + (rfName) +
         "\"\n");
   
   reporter = (ReportFile *)mapObj;
   if (reporter->GetTypeName() != "ReportFile")
      throw CommandException(
         "Object named \"" + rfName +
         "\" is not a ReportFile; Report command cannot execute\n");
   
   needsHeaders =
      reporter->GetOnOffParameter(reporter->GetParameterID("WriteHeaders")) == "On";
   
   for (StringArray::iterator i = parmNames.begin(); i != parmNames.end(); ++i)
   {
      object = FindObject(*i);
      if (object == NULL)
         throw CommandException("Object named " + (*i) + 
            " cannot be found for the Report command '" +
            GetGeneratingString() + "'"); 
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
   if (parms.empty())
      throw CommandException("Report command has no parameters to write\n");
   
   // Build the data as a string
   std::stringstream datastream;
   
   // Set the stream to use the settings in the ReportFile
   // Note that this is done here, rather than during initialization, in case
   // the user has changed the values during the run.
   datastream.precision(reporter->GetIntegerParameter(reporter->GetParameterID("Precision")));
   
   bool leftJustify = false;
   if (reporter->GetOnOffParameter(reporter->GetParameterID("LeftJustify")) == "On")
      leftJustify = true;
   
   bool zeroFill = false;
   if (reporter->GetOnOffParameter(reporter->GetParameterID("ZeroFill")) == "On")
      zeroFill = true;
   
   int colWidth = reporter->GetIntegerParameter(reporter->GetParameterID("ColumnWidth"));
   
   if (leftJustify)
      datastream.setf(std::ios::left);
   
   if (needsHeaders &&
       reporter->GetOnOffParameter(reporter->GetParameterID("WriteHeaders")) == "On")
      WriteHeaders(datastream, colWidth);
   
   // if zero fill, show decimal point
   // showing decimal point automatically filles zero
   if (zeroFill)
      datastream.setf(std::ios::showpoint);
   
   std::string desc;
   for (std::vector<Parameter*>::iterator i = parms.begin(); i != parms.end(); ++i)
   {
      if (!(*i)->IsReportable())
         continue;
      
      datastream.width(colWidth);
      
      //MessageInterface::ShowMessage
      //   (">>>>> Report::Execute() parameter=%s, returnType=%d\n", (*i)->GetName().c_str(),
      //    (*i)->GetReturnType());
      
      if ((*i)->GetReturnType() == Gmat::REAL_TYPE)
      {
         datastream << (*i)->EvaluateReal() << "   ";
      }
      else if ((*i)->GetReturnType() == Gmat::RMATRIX_TYPE)
      {
         Integer index = distance(parms.begin(), i);
         if (parmRows[index] == -1 && parmCols[index] == -1)
            datastream << (*i)->EvaluateRmatrix().ToString() << "   ";
         else // do array indexing
            datastream << (*i)->EvaluateRmatrix().GetElement
               (parmRows[index], parmCols[index]) << "   ";
      }
      else if ((*i)->GetReturnType() == Gmat::STRING_TYPE)
      {
         datastream << (*i)->EvaluateString() << "   ";
      }
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


//------------------------------------------------------------------------------
// void WriteHeaders(std::stringstream &datastream, Integer colWidth)
//------------------------------------------------------------------------------
void Report::WriteHeaders(std::stringstream &datastream, Integer colWidth)
{
   reporter->TakeAction("ActivateForReport", "On");
   for (StringArray::iterator i = actualParmNames.begin();
        i != actualParmNames.end(); ++i)
   {
      datastream.width(colWidth);
      datastream << (*i);
      datastream << "   ";
   }
   
   std::string header = datastream.str();
   reporter->ReceiveData(header.c_str(), header.length());
   datastream.str("");
   needsHeaders = false;
}


//------------------------------------------------------------------------------
// bool AddParameter(const std::string &paramName, Integer index)
//------------------------------------------------------------------------------
bool Report::AddParameter(const std::string &paramName, Integer index)
{
   #ifdef DEBUG_REPORT_SET
   MessageInterface::ShowMessage
      ("Report::AddParameter() Adding parameter '%s', index=%d\n",
       paramName.c_str(), index);
   #endif
   
   if (paramName != "" && index == numParams)
   {
      // if paramName not found, add
      if (find(actualParmNames.begin(), actualParmNames.end(), paramName) ==
          actualParmNames.end())
      {
         // Handle Array indexing
         Integer row, col;
         std::string newName;      
         GmatStringUtil::GetArrayIndex(paramName, row, col, newName);
         
         parmNames.push_back(newName);
         actualParmNames.push_back(paramName);
         parmRows.push_back(row);
         parmCols.push_back(col);
         parms.push_back(NULL);
         parmWrappers.push_back(NULL);
         numParams = actualParmNames.size();
         
         #ifdef DEBUG_REPORT_SET
         MessageInterface::ShowMessage
            ("   '%s' added, size=%d\n", paramName.c_str(), numParams);
         #endif
         
         return true;
      }
   }
   
   return false;
}

