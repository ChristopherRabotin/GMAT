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
 * Definition for the ReportFile class.
 */
//------------------------------------------------------------------------------


#ifndef ReportFile_hpp
#define ReportFile_hpp


#include "Subscriber.hpp"
#include <fstream>


class ReportFile :
    public Subscriber
{
public:
//    ReportFile(char * filename = NULL);
   ReportFile(const std::string &name, const std::string &fileName = "");
   ReportFile(const ReportFile &rf);
   virtual ~ReportFile(void);
   
   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                     GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   
   virtual Integer     GetIntegerParameter(const Integer id) const;
   virtual Integer     SetIntegerParameter(const Integer id,
                                          const Integer value);
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id,
                                          const std::string &value);

protected:
   /// Name of the report file
   std::string         filename;
   /// Precision for output of real data
   Integer             precision;
   /// ID for the file name
   Integer             filenameID;
   /// ID for the precision information
   Integer             precisionID;
   
   std::ofstream       dstream;  // output data stream
   
   virtual bool        Distribute(Integer len);
   virtual bool        Distribute(const Real * dat, Integer len);
   virtual bool        OpenReportFile(void);
};


#endif
