//$Header$
//------------------------------------------------------------------------------
//                                  ReportFile
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
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
#include "Publisher.hpp"         // for Instance()
#include "FileManager.hpp"       // for GetPathname()
#include "SubscriberException.hpp"
#include "StringUtil.hpp"        // for GetArrayIndex()
#include <iomanip>
#include <sstream>

//#define DEBUG_REPORTFILE 1
//#define DEBUG_REPORTFILE_DATA 1
//#define DEBUG_RENAME 1

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
};

const Gmat::ParameterType
ReportFile::PARAMETER_TYPE[ReportFileParamCount - SubscriberParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::INTEGER_TYPE,
};


//------------------------------------------------------------------------------
// ReportFile(const std::string &type, const std::string &name,
//            const std::string &fileName)
//------------------------------------------------------------------------------
ReportFile::ReportFile(const std::string &type, const std::string &name,
                       const std::string &fileName, Parameter *firstVarParam) :
   Subscriber      (type, name),
   outputPath      (""),
   filename        (fileName),
   precision       (16),
   columnWidth     (20),
   writeHeaders    (true),
   leftJustify     (true),
   zeroFill        (false),
   lastUsedProvider(-1),
   usedByReport    (false)
{
   try
   {
      FileManager *fm = FileManager::Instance();
      outputPath = fm->GetPathname(FileManager::REPORT_FILE);
      
      if (filename == "")
      {
         filename = fm->GetFullPathname(FileManager::REPORT_FILE);
      }
      else
      {
         // add output path if there is no path
         if (filename.find("/") == filename.npos &&
             filename.find("\\") == filename.npos)
         {
            filename = outputPath + filename;
         }
      }
   }
   catch (GmatBaseException &e)
   {
      if (filename == "")
         filename = "ReportFile.txt";
      
      MessageInterface::ShowMessage(e.GetMessage());
   }
   
   mNumVarParams = 0;

   if (firstVarParam != NULL)
      AddVarParameter(firstVarParam->GetName(), mNumVarParams);
   
   parameterCount = ReportFileParamCount;
   initial = true;
}


//------------------------------------------------------------------------------
// ~ReportFile(void)
//------------------------------------------------------------------------------
ReportFile::~ReportFile(void)
{
   dstream.flush();
   dstream.close();
}


//------------------------------------------------------------------------------
// ReportFile(const ReportFile &rf)
//------------------------------------------------------------------------------
ReportFile::ReportFile(const ReportFile &rf) :
   Subscriber      (rf),
   outputPath      (rf.outputPath),
   filename        (rf.filename),
   precision       (rf.precision),
   columnWidth     (rf.columnWidth),
   writeHeaders    (rf.writeHeaders),
   leftJustify     (rf.leftJustify),
   zeroFill        (rf.zeroFill),
   lastUsedProvider (-1),
   usedByReport    (rf.usedByReport),
   calledByReport  (rf.calledByReport)
{
   //if (filename == "")
   //   filename = "ReportFile.txt";

   filename = rf.filename;
   mVarParams = rf.mVarParams; 
   mNumVarParams = rf.mNumVarParams;
   mVarParamNames = rf.mVarParamNames;
   mAllRefObjectNames = rf.mAllRefObjectNames;
   
   parameterCount = ReportFileParamCount;
   initial = true;
}


//------------------------------------------------------------------------------
// ReportFile& ReportFile::operator=(const ReportFile& rf)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
ReportFile& ReportFile::operator=(const ReportFile& rf)
{
   if (this == &rf)
      return *this;

   Subscriber::operator=(rf);
   lastUsedProvider = -1;
   usedByReport = rf.usedByReport;
   
   outputPath = rf.outputPath;
   filename = rf.filename;
   precision = rf.precision;
   columnWidth = rf.columnWidth;
   writeHeaders = rf.writeHeaders;
   leftJustify = rf.leftJustify;
   zeroFill = rf.zeroFill;
   mVarParams = rf.mVarParams; 
   mNumVarParams = rf.mNumVarParams;
   mVarParamNames = rf.mVarParamNames;
   mAllRefObjectNames = rf.mAllRefObjectNames;
   lastUsedProvider = rf.lastUsedProvider;
   usedByReport = rf.usedByReport;
   calledByReport = rf.calledByReport;

   initial = true;

   return *this;
}


//----------------------------------
// methods inherited from Subscriber
//----------------------------------

//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool ReportFile::Initialize()
{
   // Check if there are parameters selected for report
   if (active)
   {
      if ((mNumVarParams == 0) && !usedByReport)
      {
         MessageInterface::ShowMessage
            ("ReportFile::Initialize() Report will not be written.\n"
             "No parameters selected for ReportFile.\n");
         
         active = false;
         return false;
      }

      if ((mNumVarParams > 0))
         if (mVarParams[0] == NULL)
         {
            MessageInterface::ShowMessage
               ("ReportFile::Initialize() ReportFile will not be created.\n"
                "The first parameter:%s selected for the report file is NULL\n",
                mVarParamNames[0].c_str());
            
            active = false;
            return false;
         }
   }
   
   Subscriber::Initialize();

   if (active)
   { 
      if (!OpenReportFile())
         return false;
      
      initial = true;
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
GmatBase* ReportFile::Clone(void) const
{
   return (new ReportFile(*this));
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
   if (action == "Clear")
   {
      ClearVarParameters();
      return true;
   }

   if (action == "PassedToReport")
   {
      usedByReport = true;
      return true;
   }

   if (action == "ActivateForReport")
   {
      calledByReport = ((actionData == "On") ? true : false);
   }
   
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
   #if DEBUG_RENAME
      MessageInterface::ShowMessage
         ("ReportFile::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
          GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type != Gmat::PARAMETER && type != Gmat::COORDINATE_SYSTEM &&
       type != Gmat::SPACECRAFT)
      return true;
   
   if (type == Gmat::PARAMETER)
   {
      // parameters
      for (unsigned int i=0; i<mVarParamNames.size(); i++)
      {
         if (mVarParamNames[i] == oldName)
            mVarParamNames[i] = newName;
      }
   }
   else
   {
      std::string::size_type pos;
      
      for (unsigned int i=0; i<mVarParamNames.size(); i++)
      {
         pos = mVarParamNames[i].find(oldName);
         
         if (pos != mVarParamNames[i].npos)
            mVarParamNames[i].replace(pos, oldName.size(), newName);
      }
   }
   
   return true;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string ReportFile::GetParameterText(const Integer id) const
{
    if (id >= FILENAME && id < ReportFileParamCount)
        return PARAMETER_TEXT[id - SubscriberParamCount];
    else
        return Subscriber::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer ReportFile::GetParameterID(const std::string &str) const
{
    for (Integer i = FILENAME; i < ReportFileParamCount; i++)
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
    if (id >= FILENAME && id < ReportFileParamCount)
        return PARAMETER_TYPE[id - SubscriberParamCount];
    else
        return Subscriber::GetParameterType(id);

}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string ReportFile::GetParameterTypeString(const Integer id) const
{
   if (id >= FILENAME && id < ReportFileParamCount)
      return Subscriber::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return Subscriber::GetParameterTypeString(id);

}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
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
Integer ReportFile::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == PRECISION)
   {
      if (value > 0)
         precision = value;
      return precision;
   }
   else if (id == COL_WIDTH)
   {
      if (value > 0)
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
   else if (id == WRITE_HEADERS)
   {
      if (writeHeaders)
         return "On";
      else
         return "Off";
   }
   else if (id == LEFT_JUSTIFY)
   {
      if (leftJustify)
         return "On";
      else
         return "Off";
   }
   else if (id == ZERO_FILL)
   {
      if (zeroFill)
         return "On";
      else
         return "Off";
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
      // Close the stream if it is open
      if (dstream.is_open())
      {
         dstream.close();
         dstream.open(value.c_str());
      }

      filename = value;
      
      if (filename.find("/") == filename.npos &&
          filename.find("\\") == filename.npos)
         filename = outputPath + filename;
      
      return true;
   }
   else if (id == ADD)
   {
      #ifdef DEBUG_REPORTFILE
         MessageInterface::ShowMessage(
            "ReportFile::SetStringParameter() Adding parameter '%s' to\n    "
            "ReportFile '%s'\n", value.c_str(), instanceName.c_str());
      #endif
      return AddVarParameter(value, mNumVarParams);
   }
   else if (id == WRITE_HEADERS)
   {
      if (strcmp(value.c_str(), "On") == 0)
      {
         writeHeaders = true;
         return true;
      }   
      else if (strcmp(value.c_str(), "Off") == 0)
      {
         writeHeaders = false;
         return true;
      }
      else
         return false;   
   }   
   else if (id == LEFT_JUSTIFY)
   {
      if (strcmp(value.c_str(), "On") == 0)
      {
         leftJustify = true;
         return true;
      }   
      else if (strcmp(value.c_str(), "Off") == 0)
      {
         leftJustify = false;
         return true;
      }
      else
         return false;   
   }   
   else if (id == ZERO_FILL)
   {
      if (strcmp(value.c_str(), "On") == 0)
      {
         zeroFill = true;
         return true;
      }   
      else if (strcmp(value.c_str(), "Off") == 0)
      {
         zeroFill = false;
         return true;
      }
      else
         return false;   
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
      return AddVarParameter(value, index);
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
   switch (id)
   {
   case ADD:
      return mVarParamNames;
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


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
//                                const std::string &name)
//------------------------------------------------------------------------------
GmatBase* ReportFile::GetRefObject(const Gmat::ObjectType type,
                                   const std::string &name)
{
   for (int i=0; i<mNumVarParams; i++)
   {
      if (mVarParamNames[i] == name)
         return mVarParams[i];
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
   #if DEBUG_REPORTFILE_SET
   MessageInterface::ShowMessage("ReportFile::SetRefObject() name=%s\n",
                                 name.c_str());
   #endif
   
   if (type == Gmat::PARAMETER)
   {
      for (int i=0; i<mNumVarParams; i++)
      {
         // Handle array index
         Integer row, col;
         std::string newName;
         GmatStringUtil::GetArrayIndex(mVarParamNames[i], row, col, newName);
         
         #if DEBUG_REPORTFILE_SET
         MessageInterface::ShowMessage("   newName=%s\n", newName.c_str());
         #endif
         
         //if (mVarParamNames[i] == name)
         if (newName == name)
         {
            mVarParams[i] = (Parameter*)obj;
            
            // Test see if it is reportable.
            // Cannot test in AddVarParameter(), because it just addes
            // the name.
            if (!mVarParams[i]->IsReportable())
            {
               MessageInterface::ShowMessage
                  ("*** WARNING *** The parameter:%s is not reportable so it "
                   "is removed from ReportFile.", name.c_str());
            }
            
            return true;
         }
      }
   }
   return false;
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
         Integer row, col;
         std::string newName;
         for (int i=0; i<mNumVarParams; i++)
         {
            GmatStringUtil::GetArrayIndex(mVarParamNames[i], row, col, newName);
            mAllRefObjectNames.push_back(newName);
         }
      }
   default:
      break;
   }
   
   return mAllRefObjectNames;
}


//------------------------------------------------------------------------------
// Integer GetNumVarParameters()
//------------------------------------------------------------------------------
Integer ReportFile::GetNumVarParameters()
{
   return mNumVarParams;
}


//------------------------------------------------------------------------------
// bool AddVarParameter(const std::string &paramName, Integer index)
//------------------------------------------------------------------------------
bool ReportFile::AddVarParameter(const std::string &paramName, Integer index)
{
   #ifdef DEBUG_REPORTFILE
      MessageInterface::ShowMessage(
         "ReportFile::AddVarParameter() Adding parameter '%s' to \n   "
         "ReportFile '%s'\n", paramName.c_str(), instanceName.c_str());
   #endif
   
   if (paramName != "" && index == mNumVarParams)
   {
      // if paramName not found, add
      if (find(mVarParamNames.begin(), mVarParamNames.end(), paramName) ==
          mVarParamNames.end())
      {
         mVarParamNames.push_back(paramName);
         mNumVarParams = mVarParamNames.size();
         mVarParams.push_back(NULL);
         return true;
      }
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool AddParameterForTitleOnly(const std::string &paramName)
//------------------------------------------------------------------------------
bool ReportFile::AddParameterForTitleOnly(const std::string &paramName)
{
   #ifdef DEBUG_REPORTFILE
   MessageInterface::ShowMessage
      ("ReportFile::AddParameterForTitle() Adding parameter '%s' to \n   "
       "ReportFile '%s'\n", paramName.c_str(), instanceName.c_str());
   #endif
   
   if (paramName != "")
   {
      if (find(mVarParamNames.begin(), mVarParamNames.end(), paramName) ==
          mVarParamNames.end())
      {
         mVarParamNames.push_back(paramName);
         return true;
      }
   }
   
   return false;
}


//--------------------------------------
// protected methods
//--------------------------------------

//------------------------------------------------------------------------------
// bool OpenReportFile(void)
//------------------------------------------------------------------------------
bool ReportFile::OpenReportFile(void)
{
   #if DEBUG_REPORTFILE
      MessageInterface::ShowMessage
         ("ReportFile::OpenReportFile filename = %s\n", filename.c_str());
   #endif

   if (dstream.is_open())
     dstream.close();

   dstream.open(filename.c_str());
   if (!dstream.is_open())
   {
      #if DEBUG_REPORTFILE
      MessageInterface::ShowMessage
         ("ReportFile::OpenReportFile() Failed to open report file: %s\n",
          filename.c_str());
      #endif
      
      throw SubscriberException("Cannot open report file: " + filename + "\n");
   }
   
   dstream.precision(precision);
   dstream.width(columnWidth);
   dstream.setf(std::ios::showpoint);
   dstream.fill(' ');

   if (leftJustify)
   {
      dstream.setf(std::ios::left);
   
      if (zeroFill)
         dstream.fill('0');
   }
   
   return true;
}


//--------------------------------------
// methods inherited from Subscriber
//--------------------------------------

//------------------------------------------------------------------------------
// bool Distribute(int len)
//------------------------------------------------------------------------------
bool ReportFile::Distribute(int len)
{
   #ifdef DEBUG_REPORTFILE_DATA
      MessageInterface::ShowMessage("ReportFile::Distribute called len=%d\n", len);
      MessageInterface::ShowMessage("   data = '%s'\n", data);
      MessageInterface::ShowMessage(
         "   usedByReport = %s, calledByReport = %s\n",
         (usedByReport ? "true" : "false"), 
         (calledByReport ? "true" : "false"));
   #endif
   
   if (usedByReport && calledByReport)
   {
      if (len == 0)
         return false;
      else {
         if (!dstream.is_open())
            if (!OpenReportFile())
               return false;
   
         if (!dstream.good())
            dstream.clear();
   
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
bool ReportFile::Distribute(const Real * dat, Integer len)
{
   if (!active)
      return true;

   if (len == 0)
      return false;
   
   if (mNumVarParams > 0)
   {
      Real rval = -9999.999;
      std::string sval;
      
      if (!dstream.is_open())
         if (!OpenReportFile())
            return false;
      
      if (initial)
         WriteHeaders();
      
      if (!dstream.good())
         dstream.clear();
      
      for (int i=0; i < mNumVarParams; i++)
      {
         if (!mVarParams[i]->IsReportable())
            continue;
         
         // set longer width of param names or columnWidth
         Integer width = (Integer)mVarParamNames[i].length() > columnWidth ?
            mVarParamNames[i].length() : columnWidth;
         
         dstream.width(width);
         dstream.fill(' ');
            
         if (leftJustify)
         {
            dstream.setf(std::ios::left);
            if (zeroFill)
               dstream.fill('0');                 
         }
         
         if (mVarParams[i]->GetReturnType() == Gmat::REAL_TYPE)
         {
            rval = mVarParams[i]->EvaluateReal();
            dstream.precision(precision);
            dstream << rval << "   "; // give space between columns
         }
         else if (mVarParams[i]->GetReturnType() == Gmat::STRING_TYPE)
         {
            sval = mVarParams[i]->EvaluateString();
            width = (Integer)sval.length() > width ? sval.length() : width;
            dstream.width(width);
            dstream << sval << "   "; // give space between columns
         }
         else if (mVarParams[i]->GetReturnType() == Gmat::RMATRIX_TYPE)
         {
            // Handle array index
            Integer row, col;
            std::string newName;
            GmatStringUtil::GetArrayIndex(mVarParamNames[i], row, col, newName);
            
            if (row != -1 && col!= -1)
            {
               rval = mVarParams[i]->EvaluateRmatrix().GetElement(row, col);
               dstream.precision(precision);
               dstream << rval << "   "; // give space between columns
            }
            else
            {
               std::stringstream ss("");
               ss << "ReportFile::Distribute() Array has invalid index. row=" << row
                  << " col=" << col << "\n";
            
               throw SubscriberException(ss.str());
            }
         }
      }
      
      dstream << std::endl;
      
      if (isEndOfRun)  // close file
      {
         if (dstream.is_open())
            dstream.close();
      }
      
      #if DEBUG_REPORTFILE_DATA
      MessageInterface::ShowMessage
         ("ReportFile::Distribute() dat=%f %f %f %f %g %g %g\n", dat[0], dat[1],
          dat[2], dat[3], dat[4], dat[5], dat[6]);
      #endif
   }
   
   return true;
}


//--------------------------------------
// private methods
//--------------------------------------

//------------------------------------------------------------------------------
// void ClearYParameters()
//------------------------------------------------------------------------------
void ReportFile::ClearVarParameters()
{
   //MessageInterface::ShowMessage("==> ReportFile::ClearVarParameters() entered\n");
   
   mVarParams.clear();
   mVarParamNames.clear();
   mNumVarParams = 0;
   initial = true;   
}


//------------------------------------------------------------------------------
// void WriteHeaders()
//------------------------------------------------------------------------------
void ReportFile::WriteHeaders()
{
   if (writeHeaders)
   {
      if (!dstream.is_open())
         return;

      // write heading for each item
      for (int i=0; i < mNumVarParams; i++)
      {
          if (!dstream.good())
             dstream.clear();
          
          // set longer width of param names or columnWidth
          Integer width = (Integer)mVarParamNames[i].length() > columnWidth ?
             mVarParamNames[i].length() : columnWidth;

          // parameter name has Gregorian, minimum width is 24
          if (mVarParamNames[i].find("Gregorian") != mVarParamNames[i].npos)
             if (width < 24)
                width = 24;
          
          dstream.width(width); // sets miminum field width
          
          //dstream.width(columnWidth);
          dstream.fill(' ');
          
          if (leftJustify)
             dstream.setf(std::ios::left);

          dstream << mVarParamNames[i] << "   ";
      }
      
      dstream << std::endl;
   }
   
   initial = false;
}


