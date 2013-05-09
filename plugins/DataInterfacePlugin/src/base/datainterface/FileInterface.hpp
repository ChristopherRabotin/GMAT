//$Id$
//------------------------------------------------------------------------------
//                           FileInterface
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
// Created: May 2, 2013
/**
 * 
 */
//------------------------------------------------------------------------------

#ifndef FileInterface_hpp
#define FileInterface_hpp

#include "DataInterfaceDefs.hpp"
#include "DataInterface.hpp"
#include <fstream>


/**
 * Data interface for data contained in a file
 */
class DATAINTERFACE_API FileInterface: public DataInterface
{
public:
   FileInterface(const std::string &name);
   virtual ~FileInterface();
   FileInterface(const FileInterface& fi);
   FileInterface operator=(const FileInterface& fi);

   virtual GmatBase *Clone() const;

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
//   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);

   virtual bool         Initialize();

   virtual Integer      Open(const std::string &name = "");
   virtual bool         LoadData();
   virtual Integer      Close(const std::string &name = "");

   // Temporary to get things building
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// Name of the file
   std::string filename;
   /// Stream containing the file data
   std::ifstream theStream;
   /// Flag indicating if the stream is binary or text (ASCII only for now)
   bool streamIsBinary;
   /// The reader used to load the requested data
   DataReader *theReader;

   /// Parameter IDs
   enum
   {
      FILENAME = DataInterfaceParamCount,
      FileInterfaceParamCount
   };

   /// File interface parameter types
   static const Gmat::ParameterType PARAMETER_TYPE[FileInterfaceParamCount - DataInterfaceParamCount];
   /// File interface parameter labels
   static const std::string PARAMETER_LABEL[FileInterfaceParamCount - DataInterfaceParamCount];
};

#endif /* FileInterface_hpp */
