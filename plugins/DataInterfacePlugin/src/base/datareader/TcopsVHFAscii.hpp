//$Id$
//------------------------------------------------------------------------------
//                           TcopsVHFAscii
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: May 3, 2013
/**
 * Definition of the TCOPS Vector Hold File ASCII reader
 */
//------------------------------------------------------------------------------

#ifndef TcopsVHFAscii_hpp
#define TcopsVHFAscii_hpp

#include "TcopsVHFData.hpp"

/**
 * The file reader for a TCOPS Vector Hold File in ASCII format
 *
 * This class reads and collects data from an ASCII dump of a TCOPS Vector Hold
 * File.  The code handles either the unedited dump file, or the Code 9
 * formatted file.
 */
class DATAINTERFACE_API TcopsVHFAscii: public TcopsVHFData
{
public:
   TcopsVHFAscii(const std::string& theName = "");
   virtual ~TcopsVHFAscii();
   TcopsVHFAscii(const TcopsVHFAscii& vhf);
   TcopsVHFAscii& operator=(const TcopsVHFAscii& vhf);

   virtual GmatBase* Clone() const;

   virtual bool ReadData();


   // Temporary to get things building
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

private:
   /// Flag indicating if the file is a dump file or a Task 9 file
   bool isDumpFile;
   /// The Task 9 formatted file has an opening vector, buffered here
   Real startVector[7];
   /// The UTC epoch data as found on the file
   GmatEpoch utcEpoch;

   bool CheckForHeader(const std::string& theLine);
   bool CheckForBlockBoundary(const std::string& theLine);
   void ManageStartData();
   bool ParseDataBlock();
   void ParseTime(std::string& theField);
};

#endif /* TcopsVHFAscii_hpp */
