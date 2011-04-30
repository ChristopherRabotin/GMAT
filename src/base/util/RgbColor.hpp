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
 * Declares RgbColor class and provides conversion between RGB color and
 * unsigned int color.
 */
//------------------------------------------------------------------------------
#ifndef RgbColor_hpp
#define RgbColor_hpp

#include "gmatdefs.hpp"

class GMAT_API RgbColor
{
public:

   RgbColor();
   RgbColor(const Byte red, const Byte green, const Byte blue, const Byte alpha = 0);
   RgbColor(const UnsignedInt rgb);
   RgbColor(const RgbColor &copy);
   RgbColor& operator=(const RgbColor &right);
   virtual ~RgbColor();
   
   UnsignedInt GetIntColor();
   Byte Red();
   Byte Green();
   Byte Blue();
   Byte Alpha();
   
   void Set(const Byte red, const Byte green, const Byte blue, const Byte alpha = 0);
   void Set(const UnsignedInt rgb);
   
private:
   struct RgbType
   {
      unsigned red      : 8;
      unsigned green    : 8;
      unsigned blue     : 8;
      unsigned alpha    : 8;
   } rgbType;
   
   union ColorType
   {
      RgbType rgbColor;
      UnsignedInt intColor;
   } colorType;
   
};

#endif
