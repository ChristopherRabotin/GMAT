//$Header$
//------------------------------------------------------------------------------
//                             RgbColor
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/06/02
//
/**
 * Implements RgbColor class and provides conversion between RGB color and
 * unsigned int color.
 */
//------------------------------------------------------------------------------

#include "RgbColor.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// RgbColor()
//------------------------------------------------------------------------------
RgbColor::RgbColor()
{
   // default to black
   colorType.rgbColor.red = 0;
   colorType.rgbColor.green = 0;
   colorType.rgbColor.blue = 0;
}

//------------------------------------------------------------------------------
// RgbColor(const Byte red, const Byte green, const Byte blue)
//------------------------------------------------------------------------------
RgbColor::RgbColor(const Byte red, const Byte green, const Byte blue)
{
   colorType.rgbColor.red = red;
   colorType.rgbColor.green = green;
   colorType.rgbColor.blue = blue;
}

//------------------------------------------------------------------------------
// RgbColor(const UnsignedInt intColor)
//------------------------------------------------------------------------------
RgbColor::RgbColor(const UnsignedInt intColor)
{
   colorType.intColor = intColor;
}

//------------------------------------------------------------------------------
// RgbColor(const RgbColor &copy)
//------------------------------------------------------------------------------
RgbColor::RgbColor(const RgbColor &copy)
{
   colorType.rgbColor.red = copy.colorType.rgbColor.red;
   colorType.rgbColor.green = copy.colorType.rgbColor.green;
   colorType.rgbColor.blue = copy.colorType.rgbColor.blue;
}

//------------------------------------------------------------------------------
// RgbColor& operator=(const RgbColor &right)
//------------------------------------------------------------------------------
RgbColor& RgbColor::operator=(const RgbColor &right)
{
   if (&right != this)
   {
      colorType.rgbColor.red = right.colorType.rgbColor.red;
      colorType.rgbColor.green = right.colorType.rgbColor.green;
      colorType.rgbColor.blue = right.colorType.rgbColor.blue;
   }
   return *this;
}

//------------------------------------------------------------------------------
// virtual ~RgbColor()
//------------------------------------------------------------------------------
RgbColor::~RgbColor()
{
}


//------------------------------------------------------------------------------
// UnsignedInt GetIntColor()
//------------------------------------------------------------------------------
UnsignedInt RgbColor::GetIntColor()
{
   return colorType.intColor;
}

//------------------------------------------------------------------------------
// Byte Red()
//------------------------------------------------------------------------------
Byte RgbColor::Red()
{
   return colorType.rgbColor.red;
}

//------------------------------------------------------------------------------
// Byte Green()
//------------------------------------------------------------------------------
Byte RgbColor::Green()
{
   return colorType.rgbColor.green;
}

//------------------------------------------------------------------------------
// Byte Blue()
//------------------------------------------------------------------------------
Byte RgbColor::Blue()
{
   return colorType.rgbColor.blue;
}

//------------------------------------------------------------------------------
// void Set(const Byte red, const Byte green, const Byte blue)
//------------------------------------------------------------------------------
void RgbColor::Set(const Byte red, const Byte green, const Byte blue)
{
   colorType.rgbColor.red = red;
   colorType.rgbColor.green = green;
   colorType.rgbColor.blue = blue;
}

//------------------------------------------------------------------------------
// void Set(const UnsignedInt intColor)
//------------------------------------------------------------------------------
void RgbColor::Set(const UnsignedInt intColor)
{
   colorType.intColor = intColor;
}
