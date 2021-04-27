//$Id$
//------------------------------------------------------------------------------
//                           ProgressReporter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 29, 2014
/**
 * Reporter for reports of progress data from the GMAT Navigation system
 */
//------------------------------------------------------------------------------

#ifndef ProgressReporter_hpp
#define ProgressReporter_hpp

#include "estimation_defs.hpp"
#include "gmatdefs.hpp"
#include <fstream>
#include <map>

class ProgressReporter
{
public:
   ProgressReporter(const std::string firstLine = "",
         const std::string &file = "");
   virtual ~ProgressReporter();
   ProgressReporter(const ProgressReporter& pr);
   ProgressReporter& operator=(const ProgressReporter& pr);

   UnsignedInt AddLogLevel(const std::string &newLevel);
   UnsignedInt GetLogLevel(const std::string &forDescriptor);
   std::string GetLogLevelDescriptor(UnsignedInt forLevel);
   void SetLogLevel(UnsignedInt newLevel, const std::string &forType = "");
   void SetLogLevel(const std::string &levelName, const std::string &forType = "");

   virtual void Initialize();
   virtual void Finalize();

   virtual bool WriteData(std::string dataToWrite);
   virtual bool WriteData(const std::string &label, const Real value,
         const std::string &preface = "   ", const Integer depth = 1);
   virtual bool WriteData(const StringArray &labels, const RealArray &values,
         const std::string &preface = "   ", const Integer depth = 1);
   virtual bool WriteDataArray(const StringArray &labels,
         std::vector<RealArray*> &values, const std::string &preface = "   ",
         const Integer depth = 1);
   virtual bool Flush();

protected:
   /// The name of the file holding the progress data
   std::string filename;
   /// Buffer to keep from bogging down progress by writing to file too much
   std::string buffer;
   /// Buffer size used to toggle a write
   Integer bufferTrigger;
   /// The stream receiving the data
   std::ofstream *report;
   /// Internal flag used to track when reporting is to teh message interface
   bool writeToMessageInterface;
   /// Mapping of log levels strings to supported levels
   std::map <std::string, UnsignedInt> levels;
   /// Log level for each subsystem; if not specified, the global level is used
   std::map <std::string, UnsignedInt> subsystemLogLevel;
   /// The global logging level
   UnsignedInt logLevel;
   /// The script string for the level
   std::string logLevelDescriptor;
   /// The list of available log levels
   StringArray levelDescriptors;

   virtual bool WriteBuffer();
};

#endif /* ProgressReporter_hpp */
