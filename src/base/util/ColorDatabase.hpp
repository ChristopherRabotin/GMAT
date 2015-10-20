//$Id$
//------------------------------------------------------------------------------
//                                ColorDatabase
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2013/11/05
//
/**
 * Declares color database class.
 */
//------------------------------------------------------------------------------
#ifndef ColorDatabase_hpp
#define ColorDatabase_hpp

#include "gmatdefs.hpp"
//#include "RgbColor.hpp"

class RgbColor;

class GMAT_API ColorDatabase
{

public:
   
   static ColorDatabase* Instance();
   
   Integer GetNumColors() const;
   const StringArray& GetAllColorNameArray();
   std::string GetAllColorNames();
   UnsignedIntArray GetAllIntColors();
   
   bool HasColor(const std::string &colorName);
   bool HasColor(UnsignedInt intColor);
   
   UnsignedInt GetIntColor(const std::string &name);
   RgbColor GetRgbColor(const std::string &name);
   std::string GetColorName(UnsignedInt intColor);
   
   void Add(const std::string &name, UnsignedInt intColor);
   void Add(const std::string &name, const RgbColor &rgbColor);
   void Remove(const std::string &name);
   
protected:
private:
   typedef std::map<std::string, UnsignedInt> ColorMap;
   static ColorDatabase *theInstance;
   ColorMap mStringIntColorMap;
   StringArray mColorNames;
   Integer mNumColors;
   
   ColorDatabase();
   ~ColorDatabase();
};
#endif // ColorDatabase_hpp

