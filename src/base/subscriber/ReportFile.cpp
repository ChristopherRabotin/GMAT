//$Id$
//------------------------------------------------------------------------------
//                                  ReportFile
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
// Author: LaMont Ruley
// Created: 2003/10/23
// Modified:  2004/7/16 by Allison Greene to include copy constructor
//                         and operator=
//
/**
 * Writes the specified parameter value to a file.
 */
//------------------------------------------------------------------------------

#include "ReportFile.hpp"
#include "MessageInterface.hpp"
#include "Publisher.hpp"           // for Instance()
#include "FileManager.hpp"         // for GetPathname()
#include "SubscriberException.hpp"
#include "StringUtil.hpp"          // for GetArrayName()
#include "FileUtil.hpp"            // for GmatFileUtil::
#include "RealUtilities.hpp"
#include <iomanip>
#include <sstream>

//#define DEBUG_REPORTFILE_OPEN
//#define DEBUG_REPORTFILE_SET
//#define DEBUG_REPORTFILE_GET
//#define DEBUG_REPORTFILE_INIT
//#define DEBUG_REPORTFILE_ACTION
//#define DEBUG_RENAME
//#define DEBUG_WRAPPER_CODE
//#define DEBUG_WRITE_MATRIX
//#define DBGLVL_REPORTFILE_REF_OBJ 1
//#define DBGLVL_REPORTFILE_DATA 2
//#define DBGLVL_WRITE_DATA 2
//#define DEBUG_REAL_DATA
//#define DEBUG_WRITE_HEADERS

//---------------------------------
// static data
//---------------------------------
const std::string
ReportFile::PARAMETER_TEXT[ReportFileParamCount - SubscriberParamCount] =
{
   "Filename",
   "Precision",
   "Add",
   "WriteHeaders",
   "LeftJustify",
   "ZeroFill",
   "ColumnWidth",
   "WriteReport",
};

const Gmat::ParameterType
ReportFile::PARAMETER_TYPE[ReportFileParamCount - SubscriberParamCount] =
{
   Gmat::FILENAME_TYPE,      //"Filename",
   Gmat::INTEGER_TYPE,       //"Precision",
   Gmat::OBJECTARRAY_TYPE,   //"Add",
   Gmat::BOOLEAN_TYPE,       //"WriteHeaders",
   Gmat::ON_OFF_TYPE,        //"LeftJustify",
   Gmat::ON_OFF_TYPE,        //"ZeroFill",
   Gmat::INTEGER_TYPE,       //"ColumnWidth",
   Gmat::BOOLEAN_TYPE,       //"WriteReport",
};


//------------------------------------------------------------------------------
// ReportFile(const std::string &type, const std::string &name,
//            const std::string &fileName)
//------------------------------------------------------------------------------
/**
 *  Constructor
 */
//------------------------------------------------------------------------------
ReportFile::ReportFile(const std::string &type, const std::string &name,
                       const std::string &fileName, Parameter *firstParam) :
   Subscriber      (type, name),
   outputPath      (""),
   filename        (fileName),
   defFileName     (""),
   fullPathName    (""),
   precision       (16),
   columnWidth     (23),
   writeHeaders    (true),
   headerReset     (false),
   leftJustify     (true),
   zeroFill        (false),
   lastUsedProvider(-1),
   mLastReportTime (0.0),
   usedByReport    (false),
   calledByReport  (false)
{
   objectTypes.push_back(Gmat::REPORT_FILE);
   objectTypeNames.push_back("ReportFile");
   blockCommandModeAssignment = false;

   mNumParams = 0;
   
   if (firstParam != NULL)
      AddParameter(firstParam->GetName(), mNumParams);
   
   parameterCount = ReportFileParamCount;
   initial = true;
   initialFromReport = true;
   
   #ifdef DEBUG_REPORTFILE
   MessageInterface::ShowMessage
      ("ReportFile:: constructor entered, mNumParams=%d, filename='%s'\n",
       mNumParams, filename.c_str());
   #endif
}


//------------------------------------------------------------------------------
// ~ReportFile(void)
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ReportFile::~ReportFile(void)
{
   dstream.flush();
   dstream.close();
}


//------------------------------------------------------------------------------
// ReportFile(const ReportFile &rf)
//------------------------------------------------------------------------------
/**
 * Copy Constructor
 */
//------------------------------------------------------------------------------
ReportFile::ReportFile(const ReportFile &rf) :
   Subscriber      (rf),
   outputPath      (rf.outputPath),
   filename        (rf.filename),
   defFileName     (rf.defFileName),
   fullPathName    (rf.fullPathName),
   precision       (rf.precision),
   columnWidth     (rf.columnWidth),
   writeHeaders    (rf.writeHeaders),
   headerReset     (rf.headerReset),
   leftJustify     (rf.leftJustify),
   zeroFill        (rf.zeroFill),
   lastUsedProvider(-1),
   mLastReportTime (rf.mLastReportTime),
   usedByReport    (rf.usedByReport),
   calledByReport  (rf.calledByReport)
{
   mParams = rf.mParams; 
   mNumParams = rf.mNumParams;
   mParamNames = rf.mParamNames;
   mAllRefObjectNames = rf.mAllRefObjectNames;
   
   parameterCount = ReportFileParamCount;
   initial = true;
   initialFromReport = true;
   
   #ifdef DEBUG_REPORTFILE
   MessageInterface::ShowMessage
      ("ReportFile:: copy constructor entered, r.mNumParams=%d, mNumParams=%d, filename='%s'\n",
       rf.mNumParams, mNumParams, filename.c_str());
   #endif
}


//------------------------------------------------------------------------------
// ReportFile& ReportFile::operator=(const ReportFile& rf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
//------------------------------------------------------------------------------
ReportFile& ReportFile::operator=(const ReportFile& rf)
{
   if (this == &rf)
      return *this;
   
   Subscriber::operator=(rf);
   
   outputPath = rf.outputPath;
   filename = rf.filename;
   defFileName = rf.defFileName;
   fullPathName = rf.fullPathName;
   precision = rf.precision;
   columnWidth = rf.columnWidth;
   writeHeaders = rf.writeHeaders;
   headerReset = rf.headerReset;
   leftJustify = rf.leftJustify;
   zeroFill = rf.zeroFill;
   mParams = rf.mParams; 
   mNumParams = rf.mNumParams;
   mParamNames = rf.mParamNames;
   mAllRefObjectNames = rf.mAllRefObjectNames;
   lastUsedProvider = rf.lastUsedProvider;
   mLastReportTime = rf.mLastReportTime;
   usedByReport = rf.usedByReport;
   calledByReport = rf.calledByReport;
   
   initial = true;
   initialFromReport = true;
   
   #ifdef DEBUG_REPORTFILE
   MessageInterface::ShowMessage
      ("ReportFile::operator= entered, r.mNumParams=%d, mNumParams=%d\n",
       rf.mNumParams, mNumParams);
   #endif
   
   return *this;
}

//---------------------------------
// methods for this class
//---------------------------------

//------------------------------------------------------------------------------
// std::string GetDefaultFileName()
//------------------------------------------------------------------------------
/**
 * Returns default filename without path.
 */
//------------------------------------------------------------------------------
std::string ReportFile::GetDefaultFileName()
{
   return defFileName;
}


//------------------------------------------------------------------------------
// std::string GetPathAndFileName()
//------------------------------------------------------------------------------
/**
 * Returns full file name with path
 */
//------------------------------------------------------------------------------
std::string ReportFile::GetPathAndFileName()
{
   std::string fname = filename;
   
   #ifdef DEBUG_REPORTFILE_OPEN
   MessageInterface::ShowMessage
      ("ReportFile::GetPathAndFileName() fname='%s'\n", fname.c_str());
   #endif
   
   try
   {
      FileManager *fm = FileManager::Instance();
      outputPath = fm->GetPathname(FileManager::OUTPUT_PATH);
      
      #ifdef DEBUG_REPORTFILE_OPEN
      MessageInterface::ShowMessage("   outputPath='%s'\n", outputPath.c_str());
      #endif
      
      if (filename == "")
      {
         defFileName = instanceName + ".txt";
         filename = defFileName;
         fname = outputPath + filename;
      }
      else
      {
         // add output path if there is no path
         if (filename.find("/") == filename.npos &&
             filename.find("\\") == filename.npos)
         {
            fname = outputPath + filename;
         }
      }
   }
   catch (GmatBaseException &e)
   {
      if (filename == "")
         fname = instanceName + ".txt";
      
      MessageInterface::ShowMessage(e.GetFullMessage());
   }
   
   #ifdef DEBUG_REPORTFILE_OPEN
   MessageInterface::ShowMessage
      ("ReportFile::GetPathAndFileName() returning fname=%s\n", fname.c_str());
   #endif
   
   fullPathName = fname;
   return fname;
}


//------------------------------------------------------------------------------
// Integer GetNumParameters()
//------------------------------------------------------------------------------
/**
 * Returns number of Parameters.
 */
//------------------------------------------------------------------------------
Integer ReportFile::GetNumParameters()
{
   return mNumParams;
}


//------------------------------------------------------------------------------
// bool AddParameter(const std::string &paramName, Integer index)
//------------------------------------------------------------------------------
/**
 * Adds Parameter name to array if name doesn't already exist.
 */
//------------------------------------------------------------------------------
bool ReportFile::AddParameter(const std::string &paramName, Integer index)
{
   #ifdef DEBUG_REPORTFILE_SET
   MessageInterface::ShowMessage
      ("ReportFile::AddParameter() Adding parameter '%s' to "
       "ReportFile '%s'\n", paramName.c_str(), instanceName.c_str());
   #endif
   
   if (paramName != "" && index == mNumParams)
   {
      // if paramName not found, add it
      if (find(mParamNames.begin(), mParamNames.end(), paramName) ==
          mParamNames.end())
      {
         mParamNames.push_back(paramName);
         mNumParams = mParamNames.size();
         mParams.push_back(NULL);
         yParamWrappers.push_back(NULL);
         
         #ifdef DEBUG_REPORTFILE_SET
         MessageInterface::ShowMessage
            ("   '%s' added, size=%d\n", paramName.c_str(), mNumParams);
         #endif
         
         return true;
      }
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool WriteData(WrapperArray wrapperArray)
//------------------------------------------------------------------------------
/*
 * Writes array of data wrapped with ElementWrapper to stream.
 *
 * @param  wrapperArray  data wrapper array
 */
//------------------------------------------------------------------------------
bool ReportFile::WriteData(WrapperArray wrapperArray)
{
   Integer numData = wrapperArray.size();
   UnsignedInt maxRow = 1;
   Real rval = -9999.999;
   std::string sval;
   std::string desc;
   
   // create output buffer
   StringArray *output = new StringArray[numData];
   Integer *colWidths = new Integer[numData];
   
   #if DBGLVL_WRITE_DATA > 0
   MessageInterface::ShowMessage("ReportFile::WriteData() has %d wrappers\n", numData);
   MessageInterface::ShowMessage("   ==> Now start buffering data\n");
   #endif
   
   // buffer formatted data
   for (Integer i=0; i < numData; i++)
   {
      if (wrapperArray[i] == NULL)
         continue;
      
      desc = wrapperArray[i]->GetDescription();
      
      #if DBGLVL_WRITE_DATA > 1
      MessageInterface::ShowMessage("   desc is '%s'\n", desc.c_str());
      #endif
      
      Gmat::WrapperDataType wrapperType = wrapperArray[i]->GetWrapperType();
      #if DBGLVL_WRITE_DATA > 1
      MessageInterface::ShowMessage
         ("      It's wrapper type is %d\n", wrapperType);
      #endif
      
      Integer defWidth = columnWidth;
      
      // set longer width of param names or columnWidth
      if (writeHeaders || headerReset)
      {
         defWidth = (Integer)desc.length() > columnWidth ?
            desc.length() : columnWidth;
         
         // parameter name has Gregorian, minimum width is 24
         if (desc.find("Gregorian") != desc.npos)
            if (defWidth < 24)
               defWidth = 24;
      }
      
      // if writing headers or called by Report add 3 more spaces
      // since header adds 3 more spaces
      if (writeHeaders || calledByReport || headerReset)
         defWidth = defWidth + 3;
      
      colWidths[i] = defWidth;
      
      switch (wrapperType)
      {
      case Gmat::VARIABLE_WT:
      case Gmat::ARRAY_ELEMENT_WT:
      case Gmat::OBJECT_PROPERTY_WT:
         {
            rval = wrapperArray[i]->EvaluateReal();
            if (IsNotANumber(rval))
               output[i].push_back("NaN");
            else
               output[i].push_back(GmatStringUtil::ToString(rval, precision, zeroFill));
            #ifdef DEBUG_REAL_DATA
               MessageInterface::ShowMessage("   resulting string for value of %12.10f = %s\n", rval, output[i].back().c_str());
            #endif
            break;
         }
      case Gmat::PARAMETER_WT:
         {
            Gmat::ParameterType dataType = wrapperArray[i]->GetDataType();
            #if DBGLVL_WRITE_DATA > 1
            MessageInterface::ShowMessage
               ("      It's data type is %d\n", dataType);
            #endif
            switch (dataType)
            {
            case Gmat::REAL_TYPE:
               {
                  rval = wrapperArray[i]->EvaluateReal();
                  if (IsNotANumber(rval))
                     output[i].push_back("NaN");
                  else
                     output[i].push_back(GmatStringUtil::ToString(rval, precision, zeroFill));
                  #ifdef DEBUG_REAL_DATA
                     MessageInterface::ShowMessage("   resulting string for value of %12.10f = %s\n", rval, output[i].back().c_str());
                  #endif
                  break;
               }
            case Gmat::RMATRIX_TYPE:
               {
                  Rmatrix rmat = wrapperArray[i]->EvaluateArray();
                  colWidths[i] = WriteMatrix(output, i, rmat, maxRow, defWidth);
                  break;
               }
            case Gmat::STRING_TYPE:
               {
                  sval = wrapperArray[i]->EvaluateString();
                  output[i].push_back(sval);
                  break;
               }
            default:
               throw GmatBaseException
                  ("Cannot write \"" + desc + "\" due to unimplemented "
                   "Parameter data type");
            }
            break;
         }
      case Gmat::ARRAY_WT:
         {
            Rmatrix rmat = wrapperArray[i]->EvaluateArray();
            colWidths[i] = WriteMatrix(output, i, rmat, maxRow, defWidth);
            break;
         }
      case Gmat::STRING_OBJECT_WT:
         {
            sval = wrapperArray[i]->EvaluateString();
            output[i].push_back(sval);
            #if DBGLVL_WRITE_DATA > 1
            MessageInterface::ShowMessage
               ("      Got string value of '%s'\n", sval.c_str());
            #endif
            break;
         }
      default:
         break;
      }
   }
   
   #if DBGLVL_WRITE_DATA > 0
   MessageInterface::ShowMessage
      ("   ==> Now write data to stream, maxRow is %d, first item = '%s'\n",
       maxRow, output[0][0].c_str());
   #endif
   
   if (leftJustify)
      dstream.setf(std::ios::left);
   
   // write to datastream
   for (UnsignedInt row=0; row < maxRow; row++)
   {
      for (int param=0; param < numData; param++)
      {
         dstream.width(colWidths[param]);
         
         #if DBGLVL_WRITE_DATA > 1
         MessageInterface::ShowMessage
            ("leftJustify=%d, w=%2d, %s\n", leftJustify, colWidths[param],
             output[param][row].c_str());
         #endif
         
         UnsignedInt numRow = output[param].size();
         if (numRow >= row+1)
            dstream << output[param][row];
         else if (numRow < maxRow)
            dstream << "  ";
      }
      dstream << std::endl;
      
      #if DBGLVL_WRITE_DATA > 1
      MessageInterface::ShowMessage("\n");
      #endif
   }
   
   // delete output buffer
   delete[] output;
   delete[] colWidths;
   
   if (isEndOfRun)  // close file
   {
      if (dstream.is_open())
         dstream.close();
   }
   
   #if DBGLVL_WRITE_DATA > 0
   MessageInterface::ShowMessage("ReportFile::WriteData() returning true\n");
   #endif
   
   return true;
}


//----------------------------------
// methods inherited from GmatBase
//----------------------------------

//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool ReportFile::Initialize()
{
   #ifdef DEBUG_REPORTFILE_INIT
   MessageInterface::ShowMessage
      ("ReportFile::Initialize() this=<%p> '%s', active=%d, isInitialized=%d, "
       "mNumParams=%d, usedByReport=%d\n   filename='%s'\n", this, GetName().c_str(),
       active, isInitialized, mNumParams, usedByReport, filename.c_str());
   #endif
   
   // Check if there are parameters selected for report
   if (active)
   {
      if ((mNumParams == 0) && !usedByReport)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** The ReportFile named \"%s\" will not be created.\n"
             "No parameters were added to ReportFile.\n", GetName().c_str());
         
         active = false;
         return false;
      }
      
      if ((mNumParams > 0))
         if (mParams[0] == NULL)
         {
            MessageInterface::ShowMessage
               ("*** WARNING *** The ReportFile named \"%s\" will not be created.\n"
                "The first parameter:%s added for the report file is NULL\n",
                GetName().c_str(), mParamNames[0].c_str());
            
            active = false;
            return false;
         }
   }
   
   Subscriber::Initialize();
   
   // if active and not initialized already, open report file (loj: 2008.08.20)
   if (active && !isInitialized)
   {
      // We don't want to open report file here, since ReportFile inside
      // a function can be initialized multiple times. (LOJ: 2009.10.02)
      //if (!OpenReportFile())
      //   return false;
      
      initial = true;
      isInitialized = true;
      initialFromReport = true;
   }
   
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the ReportFile.
 *
 * @return clone of the ReportFile.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ReportFile::Clone() const
{
   return (new ReportFile(*this));
}


//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void ReportFile::Copy(const GmatBase* orig)
{
   operator=(*((ReportFile *)(orig)));
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
bool ReportFile::TakeAction(const std::string &action,
                            const std::string &actionData)
{
   #ifdef DEBUG_REPORTFILE_ACTION
   MessageInterface::ShowMessage
      ("\nReportFile::TakeAction() this=<%p> entered, action='%s', actionData='%s'\n",
       this, action.c_str(), actionData.c_str());
   #endif
   
   if (action == "Clear")
   {
      ClearParameters();
   }
   else if (action == "PassedToReport")
   {
      usedByReport = true;
   }
   else if (action == "ActivateForReport")
   {
      calledByReport = ((actionData == "On") ? true : false);
      if (calledByReport)
      {
         if (!dstream.is_open())
         {
            if (!OpenReportFile())
            {
               #ifdef DEBUG_REPORTFILE_ACTION
               MessageInterface::ShowMessage
                  ("*** WARNING *** ReportFile::TakeAction() failed to open "
                   "report file '%s', so returning false\n");
               #endif
               return false;
            }
         }
      }
   }
   else if (action == "HeadersWritten")
   {
      #ifdef DEBUG_REPORTFILE_ACTION
         MessageInterface::ShowMessage("...turning off headers\n");
      #endif
      headerReset       = false;
      initialFromReport = false;
   }
   else if (action == "CheckHeaderStatus")
   {
      #ifdef DEBUG_REPORTFILE_ACTION
         MessageInterface::ShowMessage("headerReset: %s, writeHeaders: %s, "
               "initial: %s, initial (report): %s\n",
               (writeHeaders ? "true" : "false"),
               (headerReset ? "true" : "false"),
               (initial ? "true" : "false"),
               (initialFromReport ? "true" : "false"));
      #endif
      return (headerReset || (writeHeaders && initialFromReport));
   }
   else if (action == "Finalize")
   {
      if (dstream.is_open())
         dstream.close();
   }
   
   #ifdef DEBUG_REPORTFILE_ACTION
   MessageInterface::ShowMessage
      ("ReportFile::TakeAction() this=<%p> leaving, action='%s', actionData='%s'\n",
       this, action.c_str(), actionData.c_str());
   #endif
   
   return false;
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool ReportFile::RenameRefObject(const Gmat::ObjectType type,
                                 const std::string &oldName,
                                 const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("ReportFile::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   // Check for allowed object types for name change.
   if (type != Gmat::PARAMETER && type != Gmat::SPACECRAFT &&
       type != Gmat::COORDINATE_SYSTEM && type != Gmat::BURN &&
       type != Gmat::IMPULSIVE_BURN && type != Gmat::CALCULATED_POINT &&
       type != Gmat::HARDWARE)
   {
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage
         ("ReportFile::RenameRefObject() returning true, no action is required\n");
      #endif
      return true;
   }
   
   for (unsigned int i=0; i<mParamNames.size(); i++)
   {
      if (mParamNames[i].find(oldName) != oldName.npos)
      {
         #ifdef DEBUG_RENAME
         MessageInterface::ShowMessage
            ("   => Before rename, name: '%s'\n", mParamNames[i].c_str());
         #endif
         
         mParamNames[i] = GmatStringUtil::ReplaceName(mParamNames[i], oldName, newName);
      
         #ifdef DEBUG_RENAME
         MessageInterface::ShowMessage
            ("      After  rename, name: '%s'\n", mParamNames[i].c_str());
         #endif
      }
   }
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("ReportFile::RenameRefObject() returning Subscriber::RenameRefObject()\n");
   #endif
   
   return Subscriber::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string ReportFile::GetParameterText(const Integer id) const
{
   if (id >= SubscriberParamCount && id < ReportFileParamCount)
      return PARAMETER_TEXT[id - SubscriberParamCount];
   else
      return Subscriber::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer ReportFile::GetParameterID(const std::string &str) const
{
   for (Integer i = SubscriberParamCount; i < ReportFileParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SubscriberParamCount])
         return i;
   }
   
   return Subscriber::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType ReportFile::GetParameterType(const Integer id) const
{
   if (id >= SubscriberParamCount && id < ReportFileParamCount)
      return PARAMETER_TYPE[id - SubscriberParamCount];
   else
      return Subscriber::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string ReportFile::GetParameterTypeString(const Integer id) const
{
   if (id >= SubscriberParamCount && id < ReportFileParamCount)
      return Subscriber::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return Subscriber::GetParameterTypeString(id);

}


//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Tests to see if an object property can be set in Command mode
 *
 * @param id The ID of the object property
 *
 * @return true if the property can be set in command mode, false if not.
 */
//------------------------------------------------------------------------------
bool ReportFile::IsParameterCommandModeSettable(const Integer id) const
{
   // Override this one from the base class
   if (id == SOLVER_ITERATIONS)
      return true;

   // Turn these off
   if (id == ADD)
      return false;

   // Turn on the rest that are ReportFile specific (FILENAME, PRECISION, ADD,
   // WRITE_HEADERS, LEFT_JUSTIFY, ZERO_FILL, COL_WIDTH, and WRITE_REPORT)
   if (id >= SubscriberParamCount)
      return true;

   // And let the base class handle its own
   return Subscriber::IsParameterCommandModeSettable(id);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool ReportFile::GetBooleanParameter(const Integer id) const
{
   switch (id)
   {
   case WRITE_REPORT:
      return active;
   case WRITE_HEADERS:
      return writeHeaders;
   default:
      return Subscriber::GetBooleanParameter(id);
   }
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool ReportFile::SetBooleanParameter(const Integer id, const bool value)
{
   #if DBGLVL_OPENGL_PARAM
   MessageInterface::ShowMessage
      ("ReportFile::SetBooleanParameter()<%s> id=%d, value=%d\n",
       instanceName.c_str(), id, value);
   #endif
   
   switch (id)
   {
   case WRITE_REPORT:
      active = value;
      return active;
   case WRITE_HEADERS:
      writeHeaders = value;
      headerReset  = value;
      return value;
   default:
      return Subscriber::SetBooleanParameter(id, value);
   }
}


//---------------------------------------------------------------------------
//  bool GetBooleanParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool ReportFile::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}


//---------------------------------------------------------------------------
//  bool SetBooleanParameter(const std::string &label, const bool value)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool ReportFile::SetBooleanParameter(const std::string &label, const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer ReportFile::GetIntegerParameter(const Integer id) const
{
   if (id == PRECISION)
      return precision;
   else if (id == COL_WIDTH)
      return columnWidth;
   return Subscriber::GetIntegerParameter(id);
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer ReportFile::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == PRECISION)
   {
      if (value <= 0)
      {
         SubscriberException se;
         se.SetDetails(errorMessageFormat.c_str(),
                       GmatStringUtil::ToString(value, 1).c_str(),
                       GetParameterText(PRECISION).c_str(),
                       "Integer Number > 0 ");
         throw se;
      }
      
      precision = value;
      return precision;
   }
   else if (id == COL_WIDTH)
   {
      if (value <= 0)
      {
         SubscriberException se;
         se.SetDetails(errorMessageFormat.c_str(),
                       GmatStringUtil::ToString(value, 1).c_str(),
                       GetParameterText(COL_WIDTH).c_str(),
                       "Integer Number > 0 ");
         throw se;
      }
      
      columnWidth = value;
      return columnWidth;
   }
   
   return Subscriber::SetIntegerParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string ReportFile::GetStringParameter(const Integer id) const
{
   if (id == FILENAME)
   {
      return filename;
   }
   
   return Subscriber::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string ReportFile::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool ReportFile::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == FILENAME)
   {
      #ifdef DEBUG_REPORTFILE_SET
      MessageInterface::ShowMessage
         ("ReportFile::SetStringParameter() Setting filename '%s' to "
          "ReportFile '%s'\n", value.c_str(), instanceName.c_str());
      #endif
      
      // Validate filename
      if (!GmatFileUtil::IsValidFileName(value))
      {
         std::string msg = GmatFileUtil::GetInvalidFileNameMessage(1);
         SubscriberException se;
         se.SetDetails(errorMessageFormat.c_str(), value.c_str(), "Filename", msg.c_str());
         throw se;
      }
      
      // Check for non-existing directory
      if (!GmatFileUtil::DoesDirectoryExist(value))
      {
         SubscriberException se;
         se.SetDetails("Path does not exist in '%s'", value.c_str());
         throw se;
      }
      
      filename = value;
      
      // If file extension is blank, append .txt
      if (GmatFileUtil::ParseFileExtension(filename) == "")
      {
         if (filename != "")
         {
            filename = filename + ".txt";
            MessageInterface::ShowMessage
               ("*** WARNING *** Appended .txt to file name '%s'\n", value.c_str());
         }
      }
      
      std::string fullName = GetPathAndFileName();
      
      // Close the stream if it is open
      if (dstream.is_open())
      {
         dstream.close();
         dstream.open(fullPathName.c_str());
      }
      
      return true;
   }
   else if (id == ADD)
   {
      #ifdef DEBUG_REPORTFILE_SET
         MessageInterface::ShowMessage(
            "ReportFile::SetStringParameter() Adding parameter '%s' to "
            "ReportFile '%s'\n", value.c_str(), instanceName.c_str());
      #endif
      return AddParameter(value, mNumParams);
   }
   
   return Subscriber::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool ReportFile::SetStringParameter(const std::string &label,
                                    const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool ReportFile::SetStringParameter(const Integer id, const std::string &value,
                                    const Integer index)
{
   switch (id)
   {
   case ADD:
      return AddParameter(value, index);
   default:
      return Subscriber::SetStringParameter(id, value, index);
   }
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const std::string &label,
//                                 const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool ReportFile::SetStringParameter(const std::string &label,
                                    const std::string &value,
                                    const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& ReportFile::GetStringArrayParameter(const Integer id) const
{
   #ifdef DEBUG_REPORTFILE_GET
   MessageInterface::ShowMessage
      ("ReportFile::GetStringArrayParameter() id=%d, mParamNames.size()=%d, "
       "mNumParams=%d\n", id, mParamNames.size(), mNumParams);
   #endif
   
   switch (id)
   {
   case ADD:
      return mParamNames;
   default:
      return Subscriber::GetStringArrayParameter(id);
   }
}


//------------------------------------------------------------------------------
// StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const StringArray& ReportFile::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//---------------------------------------------------------------------------
//  std::string GetOnOffParameter(const Integer id) const
//---------------------------------------------------------------------------
std::string ReportFile::GetOnOffParameter(const Integer id) const
{
   std::string retStr;
   
   switch (id)
   {
   case LEFT_JUSTIFY:
      retStr = leftJustify ? "On" : "Off";
      return retStr;
   case ZERO_FILL:
      retStr = zeroFill ? "On" : "Off";
      return retStr;
   default:
      return Subscriber::GetOnOffParameter(id);
   }
}


//---------------------------------------------------------------------------
//  bool SetOnOffParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
bool ReportFile::SetOnOffParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
   case LEFT_JUSTIFY:
      leftJustify = (value == "On") ? true : false;
      return true;
   case ZERO_FILL:
      zeroFill = (value == "On") ? true : false;
      return true;
   default:
      return Subscriber::SetOnOffParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// std::string ReportFile::GetOnOffParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string ReportFile::GetOnOffParameter(const std::string &label) const
{
   return GetOnOffParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetOnOffParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool ReportFile::SetOnOffParameter(const std::string &label, 
                                   const std::string &value)
{
   return SetOnOffParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
//                                const std::string &name)
//------------------------------------------------------------------------------
GmatBase* ReportFile::GetRefObject(const Gmat::ObjectType type,
                                   const std::string &name)
{
   for (int i=0; i<mNumParams; i++)
   {
      if (mParamNames[i] == name)
         return mParams[i];
   }

   throw SubscriberException("ReportFile::GetRefObject() the object name: " + name +
                             "not found\n");
}


//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
bool ReportFile::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                              const std::string &name)
{   
   #if DBGLVL_REPORTFILE_REF_OBJ
   MessageInterface::ShowMessage
      ("ReportFile::SetRefObject() <%p>'%s' entered, obj=<%p><%s>'%s', type=%d, name='%s'\n",
       this, GetName().c_str(), obj, obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL", type, name.c_str());
   #endif
   
   if (obj == NULL)
   {
      #if DBGLVL_REPORTFILE_REF_OBJ
      MessageInterface::ShowMessage
         ("ReportFile::SetRefObject() <%p>'%s' returning false, obj is NULL\n");
      #endif
      return false;
   }
   
   if (type == Gmat::PARAMETER)
   {
      SetWrapperReference(obj, name);
      
      for (int i=0; i<mNumParams; i++)
      {
         // Handle array elements
         std::string realName = GmatStringUtil::GetArrayName(mParamNames[i]);
         
         #if DBGLVL_REPORTFILE_REF_OBJ > 1
         MessageInterface::ShowMessage("   realName=%s\n", realName.c_str());
         #endif
         
         if (realName == name)
            mParams[i] = (Parameter*)obj;
      }
      
      return true;
   }
   
   return Subscriber::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool ReportFile::HasRefObjectTypeArray()
{
   return true;
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by this class.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& ReportFile::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::PARAMETER);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
const StringArray& ReportFile::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   mAllRefObjectNames.clear();

   switch (type)
   {
   case Gmat::UNKNOWN_OBJECT:
   case Gmat::PARAMETER:
      {
         // Handle array index
         for (int i=0; i<mNumParams; i++)
         {
            std::string realName = GmatStringUtil::GetArrayName(mParamNames[i]);
            mAllRefObjectNames.push_back(realName);
         }
      }
      break;            // Clears an Eclipse warning
   default:
      break;
   }
   
   return mAllRefObjectNames;
}


//------------------------------------------------------------------------------
// const StringArray& GetWrapperObjectNameArray(bool completeSet = false)
//------------------------------------------------------------------------------
const StringArray& ReportFile::GetWrapperObjectNameArray(bool completeSet)
{
   yWrapperObjectNames.clear();
   yWrapperObjectNames.insert(yWrapperObjectNames.begin(), mParamNames.begin(),
                              mParamNames.end());
   
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("ReportFile::GetWrapperObjectNameArray() size=%d\n",  yWrapperObjectNames.size());
   for (UnsignedInt i=0; i<yWrapperObjectNames.size(); i++)
      MessageInterface::ShowMessage("   %s\n", yWrapperObjectNames[i].c_str());
   #endif
   
   return yWrapperObjectNames;
}


//--------------------------------------
// protected methods
//--------------------------------------

//------------------------------------------------------------------------------
// bool OpenReportFile(void)
//------------------------------------------------------------------------------
bool ReportFile::OpenReportFile(void)
{
   filename = GetPathAndFileName();

   // If file name is blank, use default file name (LOJ: 2009.10.02)
   if (filename == "")
      filename = defFileName;
   
   #ifdef DEBUG_REPORTFILE_OPEN
   MessageInterface::ShowMessage
      ("ReportFile::OpenReportFile() entered, filename = %s\n", filename.c_str());
   #endif
   
   if (dstream.is_open())
     dstream.close();
   
   dstream.open(filename.c_str());
   if (!dstream.is_open())
   {
      #ifdef DEBUG_REPORTFILE_OPEN
      MessageInterface::ShowMessage
         ("ReportFile::OpenReportFile() Failed to open report file: %s\n",
          filename.c_str());
      #endif
      
      throw SubscriberException("Cannot open report file: " + filename + "\n");
   }
   
   #ifdef DEBUG_REPORTFILE_OPEN
   MessageInterface::ShowMessage("ReportFile::OpenReportFile() returning true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void ClearYParameters()
//------------------------------------------------------------------------------
void ReportFile::ClearParameters()
{
   #ifdef DEBUG_REPORTFILE_SET
   MessageInterface::ShowMessage("ReportFile::ClearParameters() entered\n");
   #endif
   
   mParams.clear();
   mParamNames.clear();
   mNumParams = 0;
   ClearWrappers();
   initial = true;   
   initialFromReport = true;
}


//------------------------------------------------------------------------------
// void WriteHeaders()
//------------------------------------------------------------------------------
void ReportFile::WriteHeaders()
{
   #ifdef DEBUG_WRITE_HEADERS
   MessageInterface::ShowMessage
      ("ReportFile::WriteHeaders() entered, mNumParams=%d, columnWidth=%d\n",
       mNumParams, columnWidth);
   #endif
   
   if (writeHeaders || headerReset)
   {
      if (!dstream.is_open())
         return;
      
      // write heading for each item
      for (int i = 0; i < mNumParams; i++)
      {
          if (!dstream.good())
             dstream.clear();
          
          // set longer width of param names or columnWidth
          Integer width = (Integer)mParamNames[i].length() > columnWidth ?
             mParamNames[i].length() : columnWidth;
          
          // parameter name has Gregorian, minimum width is 24
          if (mParamNames[i].find("Gregorian") != mParamNames[i].npos)
             if (width < 24)
                width = 24;
          
          #ifdef DEBUG_WRITE_HEADERS
          MessageInterface::ShowMessage("   column %d: width = %d\n", i, width);
          #endif
          
          dstream.width(width + 3); // sets minimum field width
          dstream.fill(' ');
          
          if (leftJustify)
             dstream.setf(std::ios::left);
          
          dstream << mParamNames[i];
      }
      
      dstream << std::endl;
   }
   
   initial = false;
   headerReset = false;
   
   #ifdef DEBUG_WRITE_HEADERS
   MessageInterface::ShowMessage("ReportFile::WriteHeaders() leaving\n");
   #endif
} // WriteHeaders()


//------------------------------------------------------------------------------
// Integer WriteMatrix(StringArray *output, Integer param, const Rmatrix &rmat,
//                     Integer &maxRow, Integer defWidth)
//------------------------------------------------------------------------------
/*
 * Writes Rmatrix data to String array and returns newly computed column width.
 *
 * @param  output    Input/output String array pointer
 * @param  param     Input parameter index
 * @param  rmat      Input Rmatrix data
 * @param  maxRow    Input/output computed maximum lenth of row
 * @param  defWidth  Input default column width
 *
 * @return  newly computed column width
 */
//------------------------------------------------------------------------------
Integer ReportFile::WriteMatrix(StringArray *output, Integer param,
                                const Rmatrix &rmat, UnsignedInt &maxRow,
                                Integer defWidth)
{
   #ifdef DEBUG_WRITE_MATRIX
   MessageInterface::ShowMessage
      ("ReportFile::WriteMatrix() entered, maxRow=%d, defWidth=%d\n", maxRow, defWidth);
   #endif
   
   UnsignedInt numRow = rmat.GetNumRows();
   if (numRow > maxRow)
      maxRow = numRow;
   
   Integer w = 1;
   if (zeroFill)
      w = precision + 4;
   Integer colWidth = 0;
   
   for (UnsignedInt jj=0; jj<numRow; jj++)
   {
      std::string rowStr = rmat.ToRowString(jj, precision, w, zeroFill);
      output[param].push_back(rowStr);
      
      #ifdef DEBUG_WRITE_MATRIX
      MessageInterface::ShowMessage
         ("   rowStr='%s', rowStr.length()=%d, colWidth=%d\n", rowStr.c_str(),
          rowStr.length(), colWidth);
      #endif
      
      if (rowStr.length() > (UnsignedInt)colWidth)
         colWidth = rowStr.length();
   }
   
   // if writing headers or called by Report add 3 more spaces
   // since header adds 3 more spaces
   if (writeHeaders || calledByReport)
      colWidth = colWidth + 3;
   
   Integer newWidth = colWidth;
   
   // set longer width of param names or columnWidth
   if (writeHeaders)
      newWidth = defWidth > colWidth ? defWidth : colWidth;
   
   #ifdef DEBUG_WRITE_MATRIX
   MessageInterface::ShowMessage
      ("ReportFile::WriteMatrix() returning %d\n", newWidth);
   #endif
   return newWidth;
} // WriteMatrix()

//--------------------------------------
// methods inherited from Subscriber
//--------------------------------------

//------------------------------------------------------------------------------
// virtual bool Distribute(int len)
//------------------------------------------------------------------------------
/**
 * Distributes the data with size of len.  The distrubuted data is stored in
 * the member data char* data.
 */
//------------------------------------------------------------------------------
bool ReportFile::Distribute(int len)
{
   #if DBGLVL_REPORTFILE_DATA > 1
   MessageInterface::ShowMessage("ReportFile::Distribute(int len) called len=%d\n", len);
   if (len > 0)
      MessageInterface::ShowMessage("   data = '%s'\n", data);
   MessageInterface::ShowMessage("   usedByReport = %s, calledByReport = %s\n",
      (usedByReport ? "true" : "false"), (calledByReport ? "true" : "false"));
   #endif
   
   if (usedByReport && calledByReport)
   {
      if (len == 0)
         return false;
      else
      {
         if (!dstream.is_open())
            if (!OpenReportFile())
            {
               #if DBGLVL_REPORTFILE_DATA > 0
               MessageInterface::ShowMessage
                  ("*** WARNING *** ReportFile::Distribute() failed to open "
                   "report file '%s', so returning false\n");
               #endif
               return false;
            }
         
         if (!dstream.good())
            dstream.clear();
         
         #if DBGLVL_REPORTFILE_DATA > 1
         MessageInterface::ShowMessage("   Writing data to '%s'\n", filename.c_str());
         #endif
         
         dstream << data;
         dstream << std::endl;
      }
      return true;
   }
   
   if (isEndOfRun)  // close file
   {
      if (dstream.is_open())
         dstream.close();
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool Distribute(const Real * dat, Integer len)
//------------------------------------------------------------------------------
/**
 * Distributes the data with size of len through the Publisher.
 */
//------------------------------------------------------------------------------
bool ReportFile::Distribute(const Real * dat, Integer len)
{
   #if DBGLVL_REPORTFILE_DATA > 0
   MessageInterface::ShowMessage
      ("ReportFile::Distribute() this=<%p>'%s' called len=%d\n""   filename='%s'\n",
       this, GetName().c_str(), len, filename.c_str());
   MessageInterface::ShowMessage
      ("   active=%d, isEndOfReceive=%d, mSolverIterOption=%d, runstate=%d, prevRunState=%d\n",
       active, isEndOfReceive, mSolverIterOption, runstate, prevRunState);
   if (len > 0)
      MessageInterface::ShowMessage("   dat[0]=%f, dat[1]=%f\n", dat[0], dat[1]);
   #endif
   
   if (!active)
      return true;
   
   //------------------------------------------------------------
   // if writing current iteration only and solver is not finished,
   // just return
   //------------------------------------------------------------
   if (mSolverIterOption == SI_CURRENT && runstate == Gmat::SOLVING)
   {
      #if DBGLVL_REPORTFILE_DATA > 0
      MessageInterface::ShowMessage
         ("   ===> Just returning; writing current iteration only and solver "
          "is not finished\n");
      #endif
      return true;
   }
   
   if (len == 0)
      return true;
   
   bool writeFinalData = false;
   //------------------------------------------------------------
   // if not writing solver iteration data and solver is running, just return
   //------------------------------------------------------------
   if (mSolverIterOption == SI_NONE)
   {
      if (runstate == Gmat::SOLVING)
         writeFinalData = false;
      else if ((runstate == Gmat::SOLVEDPASS) && prevRunState == Gmat::SOLVING)
         writeFinalData = true; // To write final iteration data
      
      if (!writeFinalData)
      {
         #if DBGLVL_REPORTFILE_DATA > 0
         MessageInterface::ShowMessage
            ("   ===> Just returning; not writing solver itertion data and solver is running\n");
         #endif
         
         return true;
      }
   }
   
   if (!writeFinalData)
   {
      // If data time is the same as previous time and not reporting all solver
      // iterations, just return
      if (mLastReportTime == dat[0] &&
          (mSolverIterOption != SI_ALL && runstate != Gmat::SOLVING))
      {
         #if DBGLVL_REPORTFILE_DATA > 0
         MessageInterface::ShowMessage
            ("   ===> Just returning; current time is the same as last report time\n");
         #endif
         return true;
      }
   }
   
   #if DBGLVL_REPORTFILE_DATA > 0
   MessageInterface::ShowMessage("   ==> Start writing data\n");
   #endif
   
   //------------------------------------------------------------
   // write data to file
   //------------------------------------------------------------
   if (mNumParams > 0)
   {
      std::string sval;
      
      if (!dstream.is_open())
         if (!OpenReportFile())
            return false;
      
      if (initial || headerReset)
      {
         WriteHeaders();
      }
      
      if (!dstream.good())
         dstream.clear();
      
      // Write to report file using ReportFile::WriateData().
      // This method takes ElementWrapper array to write data to stream
      WriteData(yParamWrappers);
      mLastReportTime = dat[0];
      
      if (isEndOfRun)  // close file
      {
         if (dstream.is_open())
            dstream.close();
      }
      
      #if DBGLVL_REPORTFILE_DATA > 1
      MessageInterface::ShowMessage
         ("ReportFile::Distribute() dat=%f %f %f %f %g %g %g\n", dat[0], dat[1],
          dat[2], dat[3], dat[4], dat[5], dat[6]);
      #endif
   }
   
   return true;
}

//------------------------------------------------------------------------------
// bool IsNotANumber(Real rval)
//------------------------------------------------------------------------------
bool ReportFile::IsNotANumber(Real rval)
{
   #ifdef DEBUG_REAL_DATA
      MessageInterface::ShowMessage("Entering IsNotANumber with value = %12.10f\n", rval);
   #endif
   // zero is OK
   if (GmatMathUtil::IsEqual(rval, 0.0))   return false;

   if (GmatMathUtil::IsEqual(rval, GmatRealConstants::REAL_UNDEFINED)        ||
       GmatMathUtil::IsEqual(rval, GmatRealConstants::REAL_UNDEFINED_LARGE)  ||
       GmatMathUtil::IsNaN(rval))
   {
      #ifdef DEBUG_REAL_DATA
         MessageInterface::ShowMessage("         IsNotANumber returning true\n");
      #endif
      return true;
   }
   #ifdef DEBUG_REAL_DATA
      MessageInterface::ShowMessage("         IsNotANumber returning false\n");
   #endif
   return false;
}

