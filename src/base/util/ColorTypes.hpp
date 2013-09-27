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
 * Declares constant 32 bit UnsignedInt color.
 */
//------------------------------------------------------------------------------
#ifndef ColorTypes_hpp
#define ColorTypes_hpp

#include "gmatdefs.hpp"

namespace GmatColor
{
   // The value should be in 0x00BBGGRR
   const UnsignedInt AQUA       = 0x00ffff00;
   const UnsignedInt BEIGE      = 0x00b5f2fd;
   const UnsignedInt BLACK      = 0x00000000;
   const UnsignedInt BLUE       = 0x00ff0000;
   const UnsignedInt CHESTNUT   = 0x00404080;
   const UnsignedInt FUCHSIA    = 0x00ff00ff;
   const UnsignedInt GOLDTAN    = 0x00005E5E;
   const UnsignedInt GRAY       = 0x00808080;
   const UnsignedInt GREEN      = 0x00008000;
   const UnsignedInt LIME       = 0x0000ff00;
   const UnsignedInt NAVY       = 0x00800000;
   const UnsignedInt ORANGE     = 0x004080ff;
   const UnsignedInt PINK       = 0x00d9b6aa;
   const UnsignedInt PURPLE     = 0x00800080;
   const UnsignedInt RED        = 0x000000ff;
   const UnsignedInt SILVER     = 0x00c0c0c0;
   const UnsignedInt SKYBLUE    = 0x00930000; 
   const UnsignedInt TEAL       = 0x00808040;
   const UnsignedInt YELLOW     = 0x0000ffff;
   const UnsignedInt WHITE      = 0x00ffffff;
   const UnsignedInt D_BLUE     = 0x00780000; 
   const UnsignedInt D_BROWN    = 0x001b588b;
   const UnsignedInt D_GRAY     = 0x00404040;
   const UnsignedInt D_TEAL     = 0x00414121;
   const UnsignedInt L_BLUE     = 0x00dbb767;
   const UnsignedInt L_BROWN    = 0x001a98ce;
   const UnsignedInt L_CHESTNUT = 0x00303060;
   const UnsignedInt L_GRAY     = 0x00e0e0e0;
};

#endif // ColorTypes_hpp
