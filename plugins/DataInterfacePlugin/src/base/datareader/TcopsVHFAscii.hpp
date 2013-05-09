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

#ifndef TcopsVHFAscii_hpp
#define TcopsVHFAscii_hpp

#include "TcopsVHFData.hpp"

class TcopsVHFAscii: public TcopsVHFData
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


};

#endif /* TcopsVHFAscii_hpp */
