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
   "ColumnWidth",
};

const Gmat::ParameterType
ReportFile::PARAMETER_TYPE[ReportFileParamCount - SubscriberParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRING_TYPE,
   Gmat::INTEGER_TYPE,
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
   lastUsedProvider (-1)
{
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
   lastUsedProvider (-1)
{
   if (filename == "")
    filename = "ReportFile.txt";
    
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
   if (type == Gmat::PARAMETER) //loj: 4/22/04 Added
   {
      for (int i=0; i<mNumVarParams; i++)
      {
         if (mVarParamNames[i] == name)
         {
            mVarParams[i] = (Parameter*)obj;
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
   
   //loj: 4/29/05 Added UNKNOWN_OBJECT
   switch (type)
   {
   case Gmat::UNKNOWN_OBJECT:
   case Gmat::PARAMETER:
      mAllRefObjectNames = mVarParamNames;
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
   if (paramName != "" && index == mNumVarParams)
   {
      // if paramName not found, add (loj: 6/9/05 Added)
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
      return false;

   dstream.precision(precision);
   dstream.width(columnWidth);
   dstream.setf(std::ios::showpoint);

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
   return false;
}


//------------------------------------------------------------------------------
// bool Distribute(const Real * dat, Integer len)
//------------------------------------------------------------------------------
bool ReportFile::Distribute(const Real * dat, Integer len)
{   

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
        dstream.precision(precision);
        dstream.setf(std::ios::showpoint);
        dstream << varvals[i];
    }

    dstream << std::endl;

    //loj: 4/18/05 Added debug
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
   }
   
   initial = false;
}


