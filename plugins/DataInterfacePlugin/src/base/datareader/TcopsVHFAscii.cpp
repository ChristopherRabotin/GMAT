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
#include "MessageInterface.hpp"

TcopsVHFAscii::TcopsVHFAscii(const std::string& theName) :
   TcopsVHFData         ("TVHF_ASCII", theName)
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

bool TcopsVHFAscii::ReadData()
{
   bool retval = false;

   if (clearOnRead)
   {
      realData.clear();
      rvector6Data.clear();
      stringData.clear();
   }

   if (theStream)
   {
      std::string theLine;
      Integer count = 0;
      bool finished = false;
      while (!finished)
      {
         if (ReadLine(theLine))
         {
            ++count;
            MessageInterface::ShowMessage("%d: \"%s\"\n", count, theLine.c_str());
         }
         else
            finished = true;
      }
      retval = true;
   }

   return retval;
}
