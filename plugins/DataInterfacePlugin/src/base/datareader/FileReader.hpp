//$Id$
//------------------------------------------------------------------------------
//                           FileReader
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

#ifndef FileReader_hpp
#define FileReader_hpp

#include "DataInterfaceDefs.hpp"
#include "DataReader.hpp"

class DATAINTERFACE_API FileReader: public DataReader
{
public:
   FileReader(const std::string& theTypeName, const std::string& theName);
   virtual ~FileReader();
   FileReader(const FileReader& fr);
   FileReader& operator=(const FileReader& fr);

protected:


   /// Parameter IDs
   enum
   {
      FILENAME = DataReaderParamCount,
      FileReaderParamCount,
   };

   /// Spacecraft parameter types
   static const Gmat::ParameterType PARAMETER_TYPE[FileReaderParamCount - DataReaderParamCount];
   /// Spacecraft parameter labels
   static const std::string PARAMETER_LABEL[FileReaderParamCount - DataReaderParamCount];
};

#endif /* FileReader_hpp */
