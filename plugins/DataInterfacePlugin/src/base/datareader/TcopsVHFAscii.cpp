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
 * 
 */
//------------------------------------------------------------------------------

#include "TcopsVHFAscii.hpp"

TcopsVHFAscii::TcopsVHFAscii(const std::string& theTypeName, const std::string& theName) :
   TcopsVHFData         (theTypeName, theName)
{
   // Set up the engine accessor fields
   objectTypeNames.push_back("TVHF_ASCII");
   objectTypeNames.push_back("TcopsVHFAscii");
}

TcopsVHFAscii::~TcopsVHFAscii()
{
}

TcopsVHFAscii::TcopsVHFAscii(const TcopsVHFAscii& vhf) :
      TcopsVHFData         (vhf)
{
}

TcopsVHFAscii& TcopsVHFAscii::operator=(const TcopsVHFAscii& vhf)
{
   if (this == &vhf)
   {

   }

   return *this;
}

GmatBase* TcopsVHFAscii::Clone() const
{
   return new TcopsVHFAscii(*this);
}
