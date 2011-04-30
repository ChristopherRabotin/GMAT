//$Id$
//------------------------------------------------------------------------------
//                             RgbColor
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
   colorType.rgbColor.alpha = 0;
}

//------------------------------------------------------------------------------
// RgbColor(const Byte red, const Byte green, const Byte blue, const Byte alpha = 0)
//------------------------------------------------------------------------------
RgbColor::RgbColor(const Byte red, const Byte green, const Byte blue, const Byte alpha)
{
   colorType.rgbColor.red = red;
   colorType.rgbColor.green = green;
   colorType.rgbColor.blue = blue;
   colorType.rgbColor.alpha = alpha;
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
   colorType.rgbColor.alpha = copy.colorType.rgbColor.alpha;
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
      colorType.rgbColor.alpha = right.colorType.rgbColor.alpha;
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
// Byte Alpha()
//------------------------------------------------------------------------------
Byte RgbColor::Alpha()
{
   return colorType.rgbColor.alpha;
}

//------------------------------------------------------------------------------
// void Set(const Byte red, const Byte green, const Byte blue, const Byte alpha = 0)
//------------------------------------------------------------------------------
/**
 * Sets rgb color
 *
 * @param  red    Red value (0 - 255)
 * @param  green  Green value (0 - 255)
 * @param  blue   Blue value (0 - 255)
 * @param  alpha  Alpha value (0 = Transparent, 255 = Opaque)
 */
//------------------------------------------------------------------------------
void RgbColor::Set(const Byte red, const Byte green, const Byte blue, const Byte alpha)
{
   colorType.rgbColor.red = red;
   colorType.rgbColor.green = green;
   colorType.rgbColor.blue = blue;
   colorType.rgbColor.alpha = alpha;
}

//------------------------------------------------------------------------------
// void Set(const UnsignedInt intColor)
//------------------------------------------------------------------------------
void RgbColor::Set(const UnsignedInt intColor)
{
   colorType.intColor = intColor;
}
