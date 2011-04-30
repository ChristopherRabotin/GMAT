//$Id$
//------------------------------------------------------------------------------
//                                 ColorTypes
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/6/2
//
/**
 * Declares constant 32bit UnsignedInt color.
 */
//------------------------------------------------------------------------------
#ifndef ColorTypes_hpp
#define ColorTypes_hpp

#include "gmatdefs.hpp"

namespace GmatColor
{
   // The value should be in 0x00BBGGRR
   const UnsignedInt BLACK32    = 0x00000000;
   const UnsignedInt WHITE32    = 0x00ffffff;
   const UnsignedInt RED32      = 0x000000ff;
   const UnsignedInt GREEN32    = 0x00008000;
   const UnsignedInt YELLOW32   = 0x0000ffff;
   const UnsignedInt GRAY32     = 0x00808080;
   const UnsignedInt D_GRAY32   = 0x00404040;
   const UnsignedInt SILVER32   = 0x00c0c0c0;
   const UnsignedInt BLUE32     = 0x00ff0000;
   const UnsignedInt NAVY32     = 0x00800000;
   const UnsignedInt PURPLE32   = 0x00800080;
   const UnsignedInt AQUA32     = 0x00ffff00;
   const UnsignedInt LIME32     = 0x0000ff00;
   const UnsignedInt FUCHSIA32  = 0x00ff00ff;
   const UnsignedInt L_CHESTNUT = 0x00303060;
   const UnsignedInt CHESTNUT   = 0x00404080;
   const UnsignedInt GOLDTAN    = 0x00005E5E;
   
   const UnsignedInt SKYBLUE    = 0x00930000; 
   const UnsignedInt D_BLUE     = 0x00780000; 
   const UnsignedInt L_BLUE32   = 0x00dbb767;
   const UnsignedInt L_GRAY32   = 0x00e0e0e0;
   const UnsignedInt L_BROWN32  = 0x001a98ce;
   const UnsignedInt D_BROWN32  = 0x001b588b;
   const UnsignedInt BEIGE32    = 0x00b5f2fd;
   const UnsignedInt PINK32     = 0x00d9b6aa;
   const UnsignedInt ORANGE32   = 0x004080ff;
   const UnsignedInt TEAL32     = 0x00808040;
   const UnsignedInt D_TEAL32   = 0x00414121;
};

#endif // ColorTypes_hpp
