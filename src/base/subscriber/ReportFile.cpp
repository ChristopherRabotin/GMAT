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
#include "Moderator.hpp"         // for GetParameter()
#include "Publisher.hpp"         // for Instance()

#define DEBUG_REPORTFILE 0

//---------------------------------
// static data
//---------------------------------
const std::string
ReportFile::PARAMETER_TEXT[ReportFileParamCount - SubscriberParamCount] =
{
   "Filename",
   "Precision",
   "VarList",
   "Add",
   "Clear",
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
	Gmat::BOOLEAN_TYPE,
	Gmat::BOOLEAN_TYPE,
	Gmat::INTEGER_TYPE,
};

////------------------------------------------------------------------------------
//// ReportFile(const std::string &name, const std::string &fileName)
////------------------------------------------------------------------------------
//ReportFile::ReportFile(const std::string &name, const std::string &fileName) :
//   Subscriber      ("ReportFile", name),
//   filename        (fileName),
//   precision       (12)
////   filenameID      (parameterCount),
////   precisionID     (parameterCount + 1)
//{
//   if (fileName != "")
//      dstream.open(fileName.c_str());
//   else {
//      filename = "ReportFile.txt";
//   }
//
//   // Added 1 parameter
////   parameterCount += 2;
//   parameterCount = ReportFileParamCount;
//}

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
//   filenameID      (parameterCount),
//   precisionID     (parameterCount + 1)
   lastUsedProvider (-1)
{
   if (fileName != "")
      dstream.open(fileName.c_str());
   else {
      filename = "ReportFile.txt";
   }
   
   mNumVarParams = 0;

   if (firstVarParam != NULL)
      AddVarParameter(firstVarParam->GetName());
   
   // Added 1 parameter
//   parameterCount += 2;
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
   columnWidth         (rf.columnWidth),
   writeHeaders    (rf.writeHeaders),
//   filenameID      (parameterCount),
//   precisionID     (parameterCount + 1)
   lastUsedProvider (-1)
{
   if (filename != "")
      dstream.open(filename.c_str());
   else {
      filename = "ReportFile.txt";
   }

   mVarParams = rf.mVarParams; 
   mVarParamMap = rf.mVarParamMap;
   mNumVarParams = rf.mNumVarParams;
   mVarParamNames = rf.mVarParamNames;

   // Added 1 parameter
//   parameterCount += 2;
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
//    filenameID = rf.filenameID;
//    precisionID = rf.precisionID;

   //ag: what to do with the stream?
//    dstream = NULL;  // output data stream
    mVarParams = rf.mVarParams; 
    mVarParamMap = rf.mVarParamMap;
    mNumVarParams = rf.mNumVarParams;
    mVarParamNames = rf.mVarParamNames;

    initial = true;
    
    return *this;
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
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string ReportFile::GetParameterText(const Integer id) const
{
//   if (id == filenameID)
//      return "Filename";
//   if (id == precisionID)
//      return "Precision";
//   return Subscriber::GetParameterText(id);
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
//   if (str == "Filename")
//      return filenameID;
//   if (str == "Precision")
//      return precisionID;
//   return Subscriber::GetParameterID(str);
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
//   if (id == filenameID)
//      return Gmat::STRING_TYPE;
//   if (id == precisionID)
//      return Gmat::INTEGER_TYPE;
//   return Subscriber::GetParameterType(id);
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
//   if (id == filenameID)
//      return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
//   if (id == precisionID)
//      return PARAM_TYPE_STRING[Gmat::INTEGER_TYPE];
//   return Subscriber::GetParameterTypeString(id);
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
   if (id == ADD)
      return "";
   return Subscriber::GetStringParameter(id);
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
      return AddVarParameter(value);
   }
      
   return Subscriber::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& ReportFile::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {
   case VAR_LIST:
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
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
bool ReportFile::GetBooleanParameter(const Integer id) const
{
   switch (id)
   {
   case CLEAR:
      return true;
   case WRITE_HEADERS:
      return writeHeaders;
   default:
         return Subscriber::GetBooleanParameter(id);
   }
}

//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
bool ReportFile::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
bool ReportFile::SetBooleanParameter(const Integer id, const bool value)
{
   switch (id)
   {
   case CLEAR:
      ClearVarParameters();
      return true;
   case WRITE_HEADERS:
      writeHeaders = value;
      return writeHeaders;
   default:
      return Subscriber::SetBooleanParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label,
//                          const bool value)
//------------------------------------------------------------------------------
bool ReportFile::SetBooleanParameter(const std::string &label,
                                 const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// Integer GetNumVarParameters()
//------------------------------------------------------------------------------
Integer ReportFile::GetNumVarParameters()
{
   return mNumVarParams;
}

//------------------------------------------------------------------------------
// bool AddVarParameter(const std::string &paramName)
//------------------------------------------------------------------------------
bool ReportFile::AddVarParameter(const std::string &paramName)
{
   bool status = false;
   Moderator *theModerator = Moderator::Instance();
    
   if (paramName != "")
   {
      mVarParamNames.push_back(paramName);
      mVarParamMap[paramName] = NULL;
      mNumVarParams = mVarParamNames.size();

      // get parameter pointer
      Parameter *param = theModerator->GetParameter(paramName);
      if (param != NULL)
      {
         mVarParamMap[paramName] = param;
         mVarParams.push_back(param);
      }
      status = true;
   }

   return status;
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
   
   dstream.open(filename.c_str());
   if (!dstream.is_open())
      return false;
   
   dstream.precision(precision);
      
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
   // ag:  Not sure what len is for...
   
   if (!dstream.is_open())
      if (!OpenReportFile())
         return false;

   dstream.precision(precision);

   // get var params
   Rvector varvals = Rvector(mNumVarParams);
   
   if (initial)
     WriteHeaders();

   if (len == 0)
      dstream << data;
   else
   {
      for (int i=0; i < len; i++)
      {
          varvals[i] = mVarParams[i]->EvaluateReal();
          dstream << varvals[i] << " ";
      }   
      dstream << std::endl;
   }   
      
   return true;
}


//------------------------------------------------------------------------------
// bool Distribute(const Real * dat, Integer len)
//------------------------------------------------------------------------------
bool ReportFile::Distribute(const Real * dat, Integer len)
{   
   if (!dstream.is_open())
      if (!OpenReportFile())
         return false;
        
   dstream.precision(precision);
   
   if (initial)
     WriteHeaders();

// DJC 07/29/04 Commented out -- not sure how this works...
   // get var params
   // ag: uncommented 7/30/04
   Rvector varvals = Rvector(mNumVarParams);

   if (len == 0)
      return false;
   else {
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
//      for (int i = 0; i < len-1; ++i)
//         dstream << dat[i] << "  ";
//      dstream << dat[len-1] << std::endl;
      
// DJC 07/29/04 Commented out -- not sure how this works...
      //ag:  uncommented 7/30/04
      
      for (int i=0; i < mNumVarParams; i++)
      {
          varvals[i] = mVarParams[i]->EvaluateReal();
          dstream.width(columnWidth);
          dstream << varvals[i];
      }   

      dstream << std::endl;

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
      // write heading for each item
      for (int i=0; i < mNumVarParams; i++)
      {
          dstream.width(columnWidth);
          dstream << mVarParamNames[i];
      }   
      dstream << std::endl;
   }
   
   initial = false;
}


