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
 * Writes the specified output to a file.
 */
//------------------------------------------------------------------------------

#include "ReportFile.hpp"
#include "MessageInterface.hpp"
//#include "Moderator.hpp"         // for GetParameter()
#include "Publisher.hpp"         // for Instance()

//#define DEBUG_REPORTFILE 1
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
   "ColumnWidth",
//   "WriteStateFile",
};

const Gmat::ParameterType
ReportFile::PARAMETER_TYPE[ReportFileParamCount - SubscriberParamCount] =
{
        Gmat::STRING_TYPE,
        Gmat::INTEGER_TYPE,
        Gmat::STRINGARRAY_TYPE,
        Gmat::STRING_TYPE,
        Gmat::INTEGER_TYPE,
//      Gmat::STRING_TYPE,
};

//------------------------------------------------------------------------------
// ReportFile(const std::string &name, const std::string &fileName)
//------------------------------------------------------------------------------
ReportFile::ReportFile(const std::string &name, const std::string &fileName,
                        Parameter *firstVarParam) :
   Subscriber      ("ReportFile", name),
   filename        (fileName),
   precision       (12),
   columnWidth     (20),
   writeHeaders    (true),
//   writeStateFile  (false),
   lastUsedProvider (-1)
{
//   if (fileName != "")
//      dstream.open(fileName.c_str());
//   else {
//      filename = "ReportFile.txt";
//   }
   if (filename == "")
    filename = "ReportFile.txt";

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
   filename        (rf.filename),
   precision       (rf.precision),
   columnWidth     (rf.columnWidth),
   writeHeaders    (rf.writeHeaders),
//   writeStateFile  (true),
   lastUsedProvider (-1)
{
//   if (filename != "")
//      dstream.open(filename.c_str());
//   else {
//      filename = "ReportFile.txt";
//   }
   if (filename == "")
    filename = "ReportFile.txt";
    
//   dstream = NULL;

   mVarParams = rf.mVarParams; 
   mNumVarParams = rf.mNumVarParams;
   mVarParamNames = rf.mVarParamNames;

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
    
    filename = rf.filename;
    precision = rf.precision;
    columnWidth = rf.columnWidth;
    writeHeaders = rf.writeHeaders;
//    writeStateFile = rf.writeStateFile;

   //ag: what to do with the stream?
//    dstream = NULL;  // output data stream
    mVarParams = rf.mVarParams; 
    mNumVarParams = rf.mNumVarParams;
    mVarParamNames = rf.mVarParamNames;

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
   //loj: 11/2/04 added
   // Check if there are parameters selected for report
   if (active)
   {
      if (mNumVarParams == 0)
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "ReportFile::Initialize() Report will not be written."
             "No parameters selected for ReportFile.\n");
         return false;
      }

      if (mVarParams[0] == NULL)
      {
         active = false;
         MessageInterface::PopupMessage
            (Gmat::WARNING_,
             "ReportFile::Initialize() ReportFile will not be created.\n"
             "The first parameter selected for the report file is NULL\n");
         return false;
      }

   }
   
   Subscriber::Initialize();

   //-----------------------------------
   //@todo
   // need to set Parameter pointers
   //-----------------------------------
   // implement this later

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

   return false;
}


//loj: 11/17/04 added
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
   
   if (type == Gmat::PARAMETER)
   {
      // parameters
      for (unsigned int i=0; i<mVarParamNames.size(); i++)
      {
         if (mVarParamNames[i] == oldName)
            mVarParamNames[i] = newName;
      }
   }

   return false;
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
      return filename;
   else if (id == WRITE_HEADERS)
   {
      if (writeHeaders)
         return "On";
      else
         return "Off";
   }
//   else if (id == WRITE_STATE_FILE)
//   {
//      if (writeStateFile)
//         return "On";
//      else
//         return "Off";
//   }
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

//      if (writeStateFile)
//      {
//         if (stateStream.is_open())
//         {
//            stateStream.close();
//
//            char stateFilename[500];
//            strcpy(stateFilename, filename.c_str());
//            strcat(stateFilename, ".state");
//
//            dstream.open(stateFilename);
//         }
//      }

      return true;
   }
   else if (id == ADD)
   {
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
//   else if (id == WRITE_STATE_FILE)
//   {
//      if (strcmp(value.c_str(), "On") == 0)
//      {
//         writeStateFile = true;
//         return true;
//      }
//      else if (strcmp(value.c_str(), "Off") == 0)
//      {
//         writeStateFile = false;
//         return true;
//      }
//      else
//         return false;
//   }

      
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

//arg: 11/16/04 added
//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
//                                const std::string &name)
//------------------------------------------------------------------------------
GmatBase* ReportFile::GetRefObject(const Gmat::ObjectType type,
                               const std::string &name)
{
   // name is Y parameter
   for (int i=0; i<mNumVarParams; i++)
   {
      if (mVarParamNames[i] == name)
         return mVarParams[i];
   }

   throw GmatBaseException("ReportFile::GetRefObject() the object name: " + name +
                           "not found\n");
}

//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
bool ReportFile::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                          const std::string &name)
{
   for (int i=0; i<mNumVarParams; i++)
   {
      if (mVarParamNames[i] == name)
      {
         mVarParams[i] = (Parameter*)obj;
         return true;
      }
   }

   return false;
}

//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
const StringArray& ReportFile::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   return mVarParamNames;
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
   if (paramName != "" && index == mNumVarParams)
   {
      mVarParamNames.push_back(paramName);
      mNumVarParams = mVarParamNames.size();
      mVarParams.push_back(NULL);
      return true;
   }

   return false;

//   bool status = false;
//   Moderator *theModerator = Moderator::Instance();
//
//   if (paramName != "")
//   {
//      //-----------------------------------------------------------------
//      //loj: 11/01/04 Until SetRefObject() is implemented, add parameter
//      // if the parameter has been created and configured already
//      //-----------------------------------------------------------------
//      // get parameter pointer
//      Parameter *param = theModerator->GetParameter(paramName);
//      if (param != NULL)
//      {
//         mVarParamNames.push_back(paramName);
//         mVarParamMap[paramName] = NULL;
//         mNumVarParams = mVarParamNames.size();
//
//         mVarParamMap[paramName] = param;
//         mVarParams.push_back(param);
//         status = true;
//      }
//      else
//      {
//         MessageInterface::ShowMessage
//            ("ReportFile::AddVarParameter() Unconfigured parameter:%s will not be "
//             "added to ReportFile.\n", paramName.c_str());
//      }
//   }
//
//   return status;
}

//--------------------------------------
// protected methods
//--------------------------------------

//------------------------------------------------------------------------------
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
      return false;

   dstream.precision(precision);

//   if (writeStateFile)
//   {
//      if (stateStream.is_open())
//        stateStream.close();
//
//      char stateFilename[500];
//      strcpy(stateFilename, filename.c_str());
//      strcat(stateFilename, ".state");
//
//      stateStream.open(stateFilename);
//      if (!stateStream.is_open())
//         return false;
//
//      stateStream.precision(precision);
//   }

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
//   // ag:  Not sure what len is for...
//   
////   if (!dstream.is_open())
////      if (!OpenReportFile())
////         return false;
////
////   dstream.precision(precision);
////
//   // get var params
//   Rvector varvals = Rvector(mNumVarParams);
//   
////   if (initial)
////     WriteHeaders();
//
//   if (len == 0)
//      dstream << data;
//   else
//   {
//      for (int i=0; i < len; i++)
//      {
//          varvals[i] = mVarParams[i]->EvaluateReal();
//          dstream << varvals[i] << " ";
//      }   
//      dstream << std::endl;
//   }   
      
   return false;
}


//------------------------------------------------------------------------------
// bool Distribute(const Real * dat, Integer len)
//------------------------------------------------------------------------------
bool ReportFile::Distribute(const Real * dat, Integer len)
{   
//   if (isEndOfReceive)
//   {
//      dstream.flush();
//      dstream.close();

//      stateStream.flush();
//      stateStream.close();

//      return false;
//   }

// DJC 07/29/04 Commented out -- not sure how this works...
   // get var params
   // ag: uncommented 7/30/04
   Rvector varvals = Rvector(mNumVarParams);

   if (len == 0)
      return false;
   else {

     if (!dstream.is_open())
        if (!OpenReportFile())
           return false;

     if (initial)
        WriteHeaders();
        
     if (!dstream.good())
        dstream.clear();

    for (int i=0; i < mNumVarParams; i++)
    {
        varvals[i] = mVarParams[i]->EvaluateReal();
        dstream.width(columnWidth);
        dstream << varvals[i];
    }

    dstream << std::endl;

//    if (writeStateFile)
//    {
//        if (!stateStream.good())
//           stateStream.clear();
//
//        for (int i = 0; i < len-1; ++i)
//        {
//           stateStream.width(columnWidth);
//           stateStream << dat[i] << "  ";
//        }
//        stateStream << dat[len-1] << std::endl;
//    }
// ag: commented out 7/30/04
//      if (lastUsedProvider != currentProvider) {
//         // Write out a line with data labels
//         StringArray sar =
//             Publisher::Instance()->GetStringArrayParameter("PublishedDataMap");
//         for (StringArray::iterator i = sar.begin(); i != sar.end(); ++i)
//            dstream << (*i) << "  ";
//         dstream << "\n";
//      }
//

// DJC 07/29/04 Commented out -- not sure how this works...
      //ag:  uncommented 7/30/04


      //ag: commented 7/30/04
      //lastUsedProvider = currentProvider;
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
          dstream.width(columnWidth);
          dstream << mVarParamNames[i];
      }   
      dstream << std::endl;
      
//      if (writeStateFile)
//      {
//         if (!stateStream.is_open())
//            return;
//
//         StringArray sar =
//              Publisher::Instance()->GetStringArrayParameter("PublishedDataMap");
//         for (StringArray::iterator i = sar.begin(); i != sar.end(); ++i)
//         {
//             if (!stateStream.good())
//                stateStream.clear();
//
//             stateStream.width(columnWidth);
//             stateStream << (*i) << "  ";
//         }
//
//         stateStream << "\n";
//      }
   }
   
   initial = false;
}


