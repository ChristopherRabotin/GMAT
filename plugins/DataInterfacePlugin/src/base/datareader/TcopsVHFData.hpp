//$Id$
//------------------------------------------------------------------------------
//                           TcopsVHFData
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
 * Definition of the TCOPS Vector Hold File data elements
 */
//------------------------------------------------------------------------------

#ifndef TcopsVHFData_hpp
#define TcopsVHFData_hpp

#include "FileReader.hpp"

/**
 * Defines the fields GMAT supports for the TCOPS vector hold file
 *
 * This intermediate class is in place between the generic FileReader class and
 * the TcopsVHFAscii class so that is GMAT support for binary TVHFs is needed,
 * the data elements won't need to be refactorored into an intermediary.
 */
class DATAINTERFACE_API TcopsVHFData: public FileReader
{
public:
   TcopsVHFData(const std::string& theTypeName, const std::string& theName);
   virtual ~TcopsVHFData();
   TcopsVHFData(const TcopsVHFData& vhf);
   TcopsVHFData& operator=(const TcopsVHFData& vhf);

   const StringArray& GetSupportedFieldNames();

   virtual bool         UsesCoordinateSystem(const std::string& forField);
   virtual std::string  GetCoordinateSystemName(const std::string& forField);
   virtual bool         UsesOrigin(const std::string& forField);
   virtual std::string  GetOriginName(const std::string& forField);
   virtual bool         UsesTimeSystem(const std::string& forField);
   virtual std::string  GetTimeSystemName(const std::string& forField);

protected:
   /// Name of the coordinate system origin
   std::string          origin;
   /// Name of the coordinate system (without origin)
   std::string          csSuffix;
   /// Name of the ModJulian time system used
   std::string          timeSystem;

   void BuildOriginName();
   void BuildCSName();
};

#endif /* TcopsVHFData_hpp */
