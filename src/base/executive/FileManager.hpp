//$Header$
//------------------------------------------------------------------------------
//                                  FileManager
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/04/02
/**
 * Declares FileManager class. This is singleton class which manages
 * list of file paths and names.
 */
//------------------------------------------------------------------------------
#ifndef FileManager_hpp
#define FileManager_hpp

#include "gmatdefs.hpp"

class GMAT_API FileManager
{
public:
   static FileManager* Instance();
   void ReadStartupFile(const std::string &fileName = "");

   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;

   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool SetStringParameter(const Integer id, const std::string &value);
   virtual bool SetStringParameter(const std::string &label,
                                   const std::string &value);
protected:
   std::string theStartupFileName;
    
   static FileManager *theInstance;
    
   FileManager();
   FileManager& operator=(const FileManager &right);
   virtual ~FileManager();
    
   enum
   {
      // file path
      OUTPUT_FILE_PATH = 0,
      SLP_FILE_PATH,
      DE_FILE_PATH,
      EARTH_POT_FILE_PATH,
      TEXTURE_FILE_PATH,
      PLANETARY_COEFF_FILE_PATH,
      TIME_FILE_PATH,
      
      // file name
      TIME_COEFF_FILE,
      SLP_FILE,
      DE200_FILE,
      DE202_FILE,
      DE405_FILE,
      EARTH_JGM2_FILE,
      EARTH_JGM3_FILE,
      EOP_FILE,
      PLANETARY_COEFF_FILE,
      NUTATION_COEFF_FILE,
      LEAP_SECS_FILE,
      
      SUN_TEXTURE_FILE,
      MERCURY_TEXTURE_FILE,
      VENUS_TEXTURE_FILE,
      EARTH_TEXTURE_FILE,
      MARS_TEXTURE_FILE,
      JUPITER_TEXTURE_FILE,
      SATURN_TEXTURE_FILE,
      URANUS_TEXTURE_FILE,
      NEPTUNE_TEXTURE_FILE,
      PLUTO_TEXTURE_FILE,
      MOON_TEXTURE_FILE,
      
      // full file name
      FULL_TIME_COEFF_FILE,
      FULL_SLP_FILE,
      FULL_DE200_FILE,
      FULL_DE202_FILE,
      FULL_DE405_FILE,
      FULL_EARTH_JGM2_FILE,
      FULL_EARTH_JGM3_FILE,
      FULL_EOP_FILE,
      FULL_PLANETARY_COEFF_FILE,
      FULL_NUTATION_COEFF_FILE,
      FULL_LEAP_SECS_FILE,

      FULL_SUN_TEXTURE_FILE,
      FULL_MERCURY_TEXTURE_FILE,
      FULL_VENUS_TEXTURE_FILE,
      FULL_EARTH_TEXTURE_FILE,
      FULL_MARS_TEXTURE_FILE,
      FULL_JUPITER_TEXTURE_FILE,
      FULL_SATURN_TEXTURE_FILE,
      FULL_URANUS_TEXTURE_FILE,
      FULL_NEPTUNE_TEXTURE_FILE,
      FULL_PLUTO_TEXTURE_FILE,
      FULL_MOON_TEXTURE_FILE,
      FileManagerParamCount,        
   };

   std::string theFileList[FileManagerParamCount];
   static const std::string PARAMETER_TEXT[FileManagerParamCount];

};
#endif // FileManager_hpp
