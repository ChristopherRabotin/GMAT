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

   virtual std::string  GetParameterText(const Integer id) const;
//   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const char *value);
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const char *value,
                                           const Integer index);
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id,
                                                const Integer index) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const char *value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label,
                                                const Integer index) const;

   DEFAULT_TO_NO_CLONES

protected:
   struct thrustPoint
   {
      Real epoch;
      Real magnitude;
      Real vector[3];
   };

   struct thrustProfile
   {
      std::vector<thrustPoint> nodes;
   };

   struct thrustHistory
   {
      std::string blockName;
      thrustProfile profile;
   };

   /// Name for the thrust history file
   std::string thrustFileName;
   /// Data blocks from the file
   std::vector<thrustHistory> blocks;
   /// Scripted block names
   StringArray blockNames;
   /// Mass source for each block
   std::map<std::string, StringArray> massSources;

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
