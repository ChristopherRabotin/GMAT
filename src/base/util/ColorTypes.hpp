//$Header$
//------------------------------------------------------------------------------
//                                 ColorTypes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
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
   const UnsignedInt BLACK32    = 0x00000000;
   const UnsignedInt WHITE32    = 0x00ffffff;
   const UnsignedInt RED32      = 0x000000ff;
   const UnsignedInt GREEN32    = 0x00008000;
   const UnsignedInt YELLOW32   = 0x0000ffff;
   const UnsignedInt GRAY32     = 0x00808080;
   const UnsignedInt SILVER32   = 0x00c0c0c0;
   const UnsignedInt BLUE32     = 0x00ff0000;
   const UnsignedInt NAVY32     = 0x00800000;
   const UnsignedInt PURPLE32   = 0x00800080;
   const UnsignedInt AQUA32     = 0x00ffff00;
   const UnsignedInt LIME32     = 0x0000ff00;
   const UnsignedInt FUCHSIA32  = 0x00ff00ff;

   const UnsignedInt L_BLUE32   = 14399415;
   const UnsignedInt L_GRAY32   = 14737632;
   const UnsignedInt L_BROWN32  =  1743054;
   const UnsignedInt D_BROWN32  =  1792139;
   const UnsignedInt BEIGE32    = 11924221;
   const UnsignedInt PINK32     = 14268074;
   const UnsignedInt ORANGE32   =  4227327;
   const UnsignedInt TEAL32     =  8421440;
};

#endif // ColorTypes_hpp
