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
//
/**
 * Writes the specified output to a file.
 */
//------------------------------------------------------------------------------

#include "ReportFile.hpp"
#include "MessageInterface.hpp"

#define DEBUG_REPORTFILE 0

//------------------------------------------------------------------------------
// ReportFile(const std::string &name, const std::string &fileName)
//------------------------------------------------------------------------------
ReportFile::ReportFile(const std::string &name, const std::string &fileName) :
   Subscriber      ("ReportFile", name),
   filename        (fileName),
   precision       (12),
   filenameID      (parameterCount),
   precisionID     (parameterCount + 1)
{
   if (fileName != "")
      dstream.open(fileName.c_str());
   else {
      filename = "ReportFile.txt";
   }

   // Added 1 parameter
   parameterCount += 2;
}

//------------------------------------------------------------------------------
// ReportFile(const ReportFile &rf)
//------------------------------------------------------------------------------
ReportFile::ReportFile(const ReportFile &rf) :
   Subscriber      (rf),
   filename        (rf.filename),
   precision       (rf.precision),
   filenameID      (parameterCount),
   precisionID     (parameterCount + 1)
{
   if (filename != "")
      dstream.open(filename.c_str());
   else {
      filename = "ReportFile.txt";
   }

   // Added 1 parameter
   parameterCount += 2;
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
   if (id == filenameID)
      return "Filename";
   if (id == precisionID)
      return "Precision";
   return Subscriber::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer ReportFile::GetParameterID(const std::string &str) const
{
   if (str == "Filename")
      return filenameID;
   if (str == "Precision")
      return precisionID;
   return Subscriber::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType ReportFile::GetParameterType(const Integer id) const
{
   if (id == filenameID)
      return Gmat::STRING_TYPE;
   if (id == precisionID)
      return Gmat::INTEGER_TYPE;
   return Subscriber::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string ReportFile::GetParameterTypeString(const Integer id) const
{
   if (id == filenameID)
      return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
   if (id == precisionID)
      return PARAM_TYPE_STRING[Gmat::INTEGER_TYPE];
   return Subscriber::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer ReportFile::GetIntegerParameter(const Integer id) const
{
   if (id == precisionID)
      return precision;
   return Subscriber::GetIntegerParameter(id);
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer ReportFile::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == precisionID)
   {
      if (value > 0)
         precision = value;
      return precision;
   }
   return Subscriber::SetIntegerParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string ReportFile::GetStringParameter(const Integer id) const
{
   if (id == filenameID)
      return filename;
   return Subscriber::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool ReportFile::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == filenameID)
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
   return Subscriber::SetStringParameter(id, value);
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
   if (!dstream.is_open())
      if (!OpenReportFile())
         return false;

   if (len == 0)
      dstream << data;
   else
      for (int i = 0; i < len; ++i)
         dstream << data[i];

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

   if (len == 0)
      return false;
   else {
      for (int i = 0; i < len-1; ++i)
         dstream << dat[i] << "  ";
      dstream << dat[len-1] << std::endl;
   }

   return true;
}


