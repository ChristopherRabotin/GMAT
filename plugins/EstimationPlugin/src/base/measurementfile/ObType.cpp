//$Id: ObType.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                                  ObType
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
//         Based on code by Matthew P. Wilkins, Shafer Corporation
// Created: 2009/07/06
//
/**
 * Base class used for the observation data streams.
 */
//------------------------------------------------------------------------------


#include "ObType.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// ObType(const std::string &obType, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param obType String describing the observation type.
 * @param name Name for the obtype
 */
//------------------------------------------------------------------------------
ObType::ObType(const std::string &obType, const std::string &name) :
   GmatBase          (Gmat::OBTYPE, obType, name),
   streamName        (""),
   header            (""),
   openForRead       (true),
   openForWrite      (false)
{
   objectTypes.push_back(Gmat::OBTYPE);
   objectTypeNames.push_back("ObType");

//   parameterCount = ObTypeParamCount;
}


//------------------------------------------------------------------------------
// ~ObType
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ObType::~ObType()
{
}


//------------------------------------------------------------------------------
// ObType(const ObType& ot)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ot The instance that is copied to this new instance
 */
//------------------------------------------------------------------------------
ObType::ObType(const ObType& ot) :
   GmatBase          (ot),
   streamName        (ot.streamName),
   header            (ot.header),
   openForRead       (ot.openForRead),
   openForWrite      (ot.openForWrite)
{
}


//------------------------------------------------------------------------------
// ObType& operator=(const ObType& ot)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ot The instance that is copied to this new instance
 *
 * @return This object, configured to match ot.
 */
//------------------------------------------------------------------------------
ObType& ObType::operator=(const ObType& ot)
{
   if (this != &ot)
   {
      streamName   = ot.streamName;
      header       = ot.header;
      openForRead  = ot.openForRead;
      openForWrite = ot.openForWrite;
   }

   return *this;
}


//------------------------------------------------------------------------------
// void SetStreamName(std::string name)
//------------------------------------------------------------------------------
/**
 * Sets the stream name
 *
 * @param name The stream name
 */
//------------------------------------------------------------------------------
void ObType::SetStreamName(std::string name)
{
   streamName = name;
}


//-----------------------------------------------------------------------------
// bool Initialize()
//-----------------------------------------------------------------------------
/**
 * Prepares the ObType for use in estimation or simulation.
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool ObType::Initialize()
{
   return false;
}


//-----------------------------------------------------------------------------
// bool Open(bool forRead, bool forWrite, bool append)
//-----------------------------------------------------------------------------
/**
 * Opens an ObType stream for processing
 *
 * @param forRead True to open for reading, false otherwise
 * @param forWrite True to open for writing, false otherwise
 * @param append True if data being written should be appended, false if not
 *
 * @return true if the the stream was opened, false if not
 */
//-----------------------------------------------------------------------------
bool ObType::Open(bool forRead, bool forWrite, bool append)
{
   return false;
}


//-----------------------------------------------------------------------------
// bool IsOpen()
//-----------------------------------------------------------------------------
/**
 * Returns the status of the stream
 *
 * @return true if the stream is open, false if not
 */
//-----------------------------------------------------------------------------
bool ObType::IsOpen()
{
   return false;
}


//-----------------------------------------------------------------------------
// bool Close()
//-----------------------------------------------------------------------------
/**
 * Closes the stream
 *
 * @return true if the stream was closed, false if it was not changed
 */
//-----------------------------------------------------------------------------
bool ObType::Close()
{
   return false;
}


//-----------------------------------------------------------------------------
// bool Finalize()
//-----------------------------------------------------------------------------
/**
 * Performs all actions needed to stop finish using the stream
 *
 * @return true if the stream finalization succeeded, false if not
 */
//-----------------------------------------------------------------------------
bool ObType::Finalize()
{
   return false;
}
