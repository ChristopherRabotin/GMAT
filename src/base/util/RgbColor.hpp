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
 * Declares RgbColor class and provides conversion between RGB color and
 * unsigned int color.
 */
//------------------------------------------------------------------------------
#ifndef RgbColor_hpp
#define RgbColor_hpp

#include "gmatdefs.hpp"

class RgbColor
{
public:
   RgbColor();
   RgbColor(const Byte red, const Byte green, const Byte blue);
   RgbColor(const UnsignedInt rgb);
   RgbColor(const RgbColor &copy);
   RgbColor& operator=(const RgbColor &right);
   virtual ~RgbColor();
   
   UnsignedInt GetIntColor();
   Byte Red();
   Byte Green();
   Byte Blue();
   void Set(const Byte red, const Byte green, const Byte blue);
   void Set(const UnsignedInt rgb);
   
private:
   struct RgbType
   {
      unsigned red      : 8;
      unsigned green    : 8;
      unsigned blue     : 8;
      unsigned not_used : 8;
   } rgbType;

   union ColorType
   {
      RgbType rgbColor;
      UnsignedInt intColor;
   } colorType;
   
};

#endif
