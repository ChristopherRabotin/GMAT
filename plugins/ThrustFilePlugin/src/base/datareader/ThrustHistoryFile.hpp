//------------------------------------------------------------------------------
//                           ThrustHistoryFile
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 13, 2016
/**
 * 
 */
//------------------------------------------------------------------------------

#ifndef ThrustHistoryFile_hpp
#define ThrustHistoryFile_hpp

#include "ThrustFileDefs.hpp"
#include "FileReader.hpp"

class ThrustHistoryFile: public FileReader
{
public:
   ThrustHistoryFile(const std::string& theName = "");
   virtual ~ThrustHistoryFile();
   ThrustHistoryFile(const ThrustHistoryFile& thf);
   ThrustHistoryFile& operator=(const ThrustHistoryFile& thf);

   virtual bool RenameRefObject(Gmat::ObjectType type, const std::string& oldname,
         const std::string &newname);

   virtual GmatBase* Clone() const;
   virtual bool ReadData();

   DEFAULT_TO_NO_CLONES

   /// Parameter IDs
   enum
   {
      FILENAME = GmatBaseParamCount,
      BLOCKID,
      MASS_SOURCE,
      ThrustHistoryFileParamCount,
   };

   /// Interface parameter types
   static const Gmat::ParameterType PARAMETER_TYPE[ThrustHistoryFileParamCount - GmatBaseParamCount];
   /// Interface parameter labels
   static const std::string PARAMETER_LABEL[ThrustHistoryFileParamCount - GmatBaseParamCount];
};

#endif /* ThrustHistoryFile_hpp */
