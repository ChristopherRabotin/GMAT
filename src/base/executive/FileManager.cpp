//$Header$
//------------------------------------------------------------------------------
//                            FileManager
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
 * Implements FileManager class. This is singleton class which manages
 * list of file paths and names.
 */
//------------------------------------------------------------------------------

#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include "GmatBaseException.hpp"
#include <fstream>
#include <sstream>

//#define DEBUG_FILE_MANAGER 1

//---------------------------------
// static data
//---------------------------------
const std::string
FileManager::PARAMETER_TEXT[FileManagerParamCount] =
{
   // file path
   "OUTPUT_FILE_PATH",
   "SLP_FILE_PATH",
   "DE_FILE_PATH",
   "EARTH_POT_FILE_PATH",
   "TEXTURE_FILE_PATH",
   "PLANETARY_COEFF_FILE_PATH",
   "TIME_FILE_PATH",
   
   // file name
   "TIME_COEFF_FILE",
   "SLP_FILE",
   "DE200_FILE",
   "DE202_FILE",
   "DE405_FILE",
   "EARTH_JGM2_FILE",
   "EARTH_JGM3_FILE",
   "EOP_FILE",
   "PLANETARY_COEFF_FILE",
   "NUTATION_COEFF_FILE",
   "LEAP_SECS_FILE",
   
   "SUN_TEXTURE_FILE",
   "MERCURY_TEXTURE_FILE",
   "VENUS_TEXTURE_FILE",
   "EARTH_TEXTURE_FILE",
   "MARS_TEXTURE_FILE",
   "JUPITER_TEXTURE_FILE",
   "SATURN_TEXTURE_FILE",
   "URANUS_TEXTURE_FILE",
   "NEPTUNE_TEXTURE_FILE",
   "PLUTO_TEXTURE_FILE",
   "MOON_TEXTURE_FILE",

   // full file name
   "FULL_TIME_COEFF_FILE",
   "FULL_SLP_FILE",
   "FULL_DE200_FILE",
   "FULL_DE202_FILE",
   "FULL_DE405_FILE",
   "FULL_EARTH_JGM2_FILE",
   "FULL_EARTH_JGM3_FILE",
   "FULL_EOP_FILE",
   "FULL_PLANETARY_COEFF_FILE",
   "FULL_NUTATION_COEFF_FILE",
   "FULL_LEAP_SECS_FILE",
   
   "FULL_SUN_TEXTURE_FILE",
   "FULL_MERCURY_TEXTURE_FILE",
   "FULL_VENUS_TEXTURE_FILE",
   "FULL_EARTH_TEXTURE_FILE",
   "FULL_MARS_TEXTURE_FILE",
   "FULL_JUPITER_TEXTURE_FILE",
   "FULL_SATURN_TEXTURE_FILE",
   "FULL_URANUS_TEXTURE_FILE",
   "FULL_NEPTUNE_TEXTURE_FILE",
   "FULL_PLUTO_TEXTURE_FILE",
   "FULL_MOON_TEXTURE_FILE",
};

FileManager* FileManager::theInstance = NULL;

//---------------------------------
// public methods
//---------------------------------
   
//------------------------------------------------------------------------------
// FileManager* Instance()
//------------------------------------------------------------------------------
FileManager* FileManager::Instance()
{
   if (theInstance == NULL)
      theInstance = new FileManager;
   return theInstance;
}

//------------------------------------------------------------------------------
// void ReadStartupFile(const std::string &fileName = "")
//------------------------------------------------------------------------------
void FileManager::ReadStartupFile(const std::string &fileName)
{
   char line[200] = "";
    
   if (fileName != "")
      theStartupFileName = fileName;

#ifdef DEBUG_FILE_MANAGER
   MessageInterface::ShowMessage("FileManager::ReadStartupFile() reading:%s\n",
                                 theStartupFileName.c_str());
#endif
   
   std::ifstream instream(theStartupFileName.c_str());

   if (!instream)
      throw GmatBaseException
         ("FileManager::ReadStartupFile() cannot open:" + fileName);

   while (!instream.eof())
   {
      line[0] = '\0';
      instream.getline(line, 200);
      
#ifdef DEBUG_FILE_MANAGER
      MessageInterface::ShowMessage("FileManager::ReadStartupFile() line=%s\n",
                                    line);
#endif

      if (line[0] == '\0')
         break;
      if (line[0] == '#')
         continue;
        
      std::string type, equal, name;
      std::stringstream ss("");
        
      ss << line;
      ss >> type >> equal;
        
      if (equal != "=")
      {
         instream.close();
         throw GmatBaseException("FileManager::ReadStartupFile() expecting =");
      }
        
      ss >> name;
            
#ifdef DEBUG_FILE_MANAGER
      MessageInterface::ShowMessage("FileManager::ReadStartupFile() type=%s, "
                                    "name=%s\n", type.c_str(), name.c_str());
#endif
      
      // find string match
      // file path
      if (type == "OUTPUT_FILE_PATH")
         theFileList[OUTPUT_FILE_PATH] = name;
      else if (type == "SLP_FILE_PATH")
         theFileList[SLP_FILE_PATH] = name;
      else if (type == "DE_FILE_PATH")
         theFileList[DE_FILE_PATH] = name;
      else if (type == "EARTH_POT_FILE_PATH")
         theFileList[EARTH_POT_FILE_PATH] = name;
      else if (type == "TEXTURE_FILE_PATH")
         theFileList[TEXTURE_FILE_PATH] = name;
      else if (type == "PLANETARY_COEFF_FILE_PATH")
         theFileList[PLANETARY_COEFF_FILE_PATH] = name;
      else if (type == "TIME_FILE_PATH")
         theFileList[TIME_FILE_PATH] = name;

      // file name
      else if (type == "TIME_COEFF_FILE")
         theFileList[TIME_COEFF_FILE] = name;
      else if (type == "SLP_FILE")
         theFileList[SLP_FILE] = name;
      else if (type == "DE200_FILE")
         theFileList[DE200_FILE] = name;
      else if (type == "DE202_FILE")
         theFileList[DE202_FILE] = name;
      else if (type == "DE405_FILE")
         theFileList[DE405_FILE] = name;
      else if (type == "DE405_FILE")
         theFileList[DE405_FILE] = name;

      // potential field file
      else if (type == "EARTH_JGM2_FILE")
         theFileList[EARTH_JGM2_FILE] = name;
      else if (type == "EARTH_JGM3_FILE")
         theFileList[EARTH_JGM3_FILE] = name;

      // planetary coeff. file
      else if (type == "EOP_FILE")
         theFileList[EOP_FILE] = name;
      else if (type == "PLANETARY_COEFF_FILE")
         theFileList[PLANETARY_COEFF_FILE] = name;
      else if (type == "NUTATION_COEFF_FILE")
         theFileList[NUTATION_COEFF_FILE] = name;

      // time file
      else if (type == "LEAP_SECS_FILE")
         theFileList[LEAP_SECS_FILE] = name;
      
      // texture file
      else if (type == "SUN_TEXTURE_FILE")
         theFileList[SUN_TEXTURE_FILE] = name;
      else if (type == "MERCURY_TEXTURE_FILE")
         theFileList[MERCURY_TEXTURE_FILE] = name;
      else if (type == "VENUS_TEXTURE_FILE")
         theFileList[VENUS_TEXTURE_FILE] = name;
      else if (type == "EARTH_TEXTURE_FILE")
         theFileList[EARTH_TEXTURE_FILE] = name;
      else if (type == "MARS_TEXTURE_FILE")
         theFileList[MARS_TEXTURE_FILE] = name;
      else if (type == "JUPITER_TEXTURE_FILE")
         theFileList[JUPITER_TEXTURE_FILE] = name;
      else if (type == "SATURN_TEXTURE_FILE")
         theFileList[SATURN_TEXTURE_FILE] = name;
      else if (type == "URANUS_TEXTURE_FILE")
         theFileList[URANUS_TEXTURE_FILE] = name;
      else if (type == "NEPTUNE_TEXTURE_FILE")
         theFileList[NEPTUNE_TEXTURE_FILE] = name;
      else if (type == "PLUTO_TEXTURE_FILE")
         theFileList[PLUTO_TEXTURE_FILE] = name;
      else if (type == "MOON_TEXTURE_FILE")
         theFileList[MOON_TEXTURE_FILE] = name;
      else
      {
         instream.close();
         throw GmatBaseException("FileManager::ReadStartupFile() Invalid file type:" +
                                 type);
      }
   }

   // create full file name
   theFileList[FULL_TIME_COEFF_FILE]
      = theFileList[SLP_FILE_PATH] + theFileList[TIME_COEFF_FILE];
    
   theFileList[FULL_SLP_FILE]
      = theFileList[SLP_FILE_PATH] + theFileList[SLP_FILE];
    
   theFileList[FULL_DE200_FILE]
      = theFileList[DE_FILE_PATH] + theFileList[DE200_FILE];
    
   theFileList[FULL_DE202_FILE]
      = theFileList[DE_FILE_PATH] + theFileList[DE202_FILE];
    
   theFileList[FULL_DE405_FILE]
      = theFileList[DE_FILE_PATH] + theFileList[DE405_FILE];

   // planetary gravity files
   theFileList[FULL_EARTH_JGM2_FILE]
      = theFileList[EARTH_POT_FILE_PATH] + theFileList[EARTH_JGM2_FILE];
    
   theFileList[FULL_EARTH_JGM3_FILE]
      = theFileList[EARTH_POT_FILE_PATH] + theFileList[EARTH_JGM3_FILE];
   
   // planetary coeff. files
   theFileList[FULL_EOP_FILE]
      = theFileList[PLANETARY_COEFF_FILE_PATH] + theFileList[EOP_FILE];
   
   theFileList[FULL_PLANETARY_COEFF_FILE]
      = theFileList[PLANETARY_COEFF_FILE_PATH] + theFileList[PLANETARY_COEFF_FILE];
   
   theFileList[FULL_NUTATION_COEFF_FILE]
      = theFileList[PLANETARY_COEFF_FILE_PATH] + theFileList[NUTATION_COEFF_FILE];
   
   theFileList[FULL_LEAP_SECS_FILE]
      = theFileList[TIME_FILE_PATH] + theFileList[LEAP_SECS_FILE];
   
   // texture files
   theFileList[FULL_SUN_TEXTURE_FILE]
      = theFileList[TEXTURE_FILE_PATH] + theFileList[SUN_TEXTURE_FILE];
   
   theFileList[FULL_MERCURY_TEXTURE_FILE]
      = theFileList[TEXTURE_FILE_PATH] + theFileList[MERCURY_TEXTURE_FILE];
   
   theFileList[FULL_VENUS_TEXTURE_FILE]
      = theFileList[TEXTURE_FILE_PATH] + theFileList[VENUS_TEXTURE_FILE];

   theFileList[FULL_EARTH_TEXTURE_FILE]
      = theFileList[TEXTURE_FILE_PATH] + theFileList[EARTH_TEXTURE_FILE];
   
   theFileList[FULL_MARS_TEXTURE_FILE]
      = theFileList[TEXTURE_FILE_PATH] + theFileList[MARS_TEXTURE_FILE];
   
   theFileList[FULL_JUPITER_TEXTURE_FILE]
      = theFileList[TEXTURE_FILE_PATH] + theFileList[JUPITER_TEXTURE_FILE];
    
   theFileList[FULL_SATURN_TEXTURE_FILE]
      = theFileList[TEXTURE_FILE_PATH] + theFileList[SATURN_TEXTURE_FILE];
   
   theFileList[FULL_URANUS_TEXTURE_FILE]
      = theFileList[TEXTURE_FILE_PATH] + theFileList[URANUS_TEXTURE_FILE];
   
   theFileList[FULL_NEPTUNE_TEXTURE_FILE]
      = theFileList[TEXTURE_FILE_PATH] + theFileList[NEPTUNE_TEXTURE_FILE];
    
   theFileList[FULL_PLUTO_TEXTURE_FILE]
      = theFileList[TEXTURE_FILE_PATH] + theFileList[PLUTO_TEXTURE_FILE];
   
   theFileList[FULL_MOON_TEXTURE_FILE]
      = theFileList[TEXTURE_FILE_PATH] + theFileList[MOON_TEXTURE_FILE];
    
   instream.close();
}

//------------------------------------------------------------------------------
// virtual std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string FileManager::GetParameterText(const Integer id) const
{
   if (id >= 0 && id < FileManagerParamCount)
      return PARAMETER_TEXT[id];
   else
      return "UNKNOWN_ID";
    
}

//------------------------------------------------------------------------------
// virtual Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer FileManager::GetParameterID(const std::string &str) const
{
   for (int i=0; i<FileManagerParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i])
         return i;
   }
   
   return -1;
}

//------------------------------------------------------------------------------
// virtual std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string FileManager::GetStringParameter(const Integer id) const
{
   if (id >= 0 && id < FileManagerParamCount)
      return theFileList[id];
   else
      return "UNKNOWN_ID";
}

//------------------------------------------------------------------------------
// virtual std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string FileManager::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool FileManager::SetStringParameter(const Integer id, const std::string &value)
{
   if (id >= 0 && id < FileManagerParamCount)
   {
      theFileList[id] = value;
      return true;
   }

   return false;
}

//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const std::string &label,
//                                 const std::string &value)
//------------------------------------------------------------------------------
bool FileManager::SetStringParameter(const std::string &label,
                                     const std::string &value)
{
#ifdef DEBUG_FILE_MANAGER
   MessageInterface::ShowMessage("FileManager::SetStringParameter() entered: "
                                 "label = " + label + ", value = " + value + "\n");
#endif
   return SetStringParameter(GetParameterID(label), value);
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// FileManager()
//------------------------------------------------------------------------------
/*
 * Constructor
 *
 * @note It uses DOS file system as a default.  Handling of other file
 *       system (such as MAC) will be implemented later.
 */
//------------------------------------------------------------------------------
FileManager::FileManager()
{    
   
   // create default paths and files
   theFileList[OUTPUT_FILE_PATH] = ".\\files\\output\\";

   // slp files
   theFileList[SLP_FILE_PATH] = ".\\files\\planetary_ephem\\slp\\";
   theFileList[TIME_COEFF_FILE] = "timecof.pc";
   theFileList[SLP_FILE] = "mn2000.pc";

   // de files
   theFileList[DE_FILE_PATH] = ".\\files\\planetary_ephem\\de\\";
   theFileList[DE200_FILE] = "winp1941.200";
   theFileList[DE202_FILE] = "winp1950.202";
   theFileList[DE405_FILE] = "winp1941.405";

   // earth gravity files
   theFileList[EARTH_POT_FILE_PATH] = ".\\files\\gravity\\earth\\";
   theFileList[EARTH_JGM2_FILE] = "JGM2.grv";
   theFileList[EARTH_JGM3_FILE] = "JGM3.grv";

   // planetary coeff. fiels
   theFileList[PLANETARY_COEFF_FILE_PATH] = ".\\files\\planetary_coeff\\";
   theFileList[EOP_FILE] = "eopc04.62-now";
   theFileList[PLANETARY_COEFF_FILE] = "NUT85.DAT";
   theFileList[NUTATION_COEFF_FILE] = "NUTATION.DAT";

   // time files
   theFileList[TIME_FILE_PATH] = ".\\files\\time\\";
   theFileList[LEAP_SECS_FILE] = "tai-utc.dat";
   
   // texture files
   theFileList[TEXTURE_FILE_PATH] = ".\\files\\plot\\texture\\";
   theFileList[SUN_TEXTURE_FILE] = "sun-0512.jpg";
   theFileList[MERCURY_TEXTURE_FILE] = "mercury-0512.jpg";
   theFileList[VENUS_TEXTURE_FILE] = "venus-0512.jpg";
   theFileList[EARTH_TEXTURE_FILE] = "earth-0512.jpg";
   theFileList[MARS_TEXTURE_FILE] = "mars-0512.jpg";
   theFileList[JUPITER_TEXTURE_FILE] = "jupiter-0512.jpg";
   theFileList[SATURN_TEXTURE_FILE] = "saturn-0512.jpg";
   theFileList[URANUS_TEXTURE_FILE] = "uranus-0512.jpg";
   theFileList[NEPTUNE_TEXTURE_FILE] = "neptune-0512.jpg";
   theFileList[PLUTO_TEXTURE_FILE] = "pluto-0512.jpg";
   theFileList[MOON_TEXTURE_FILE] = "moon-0512.jpg";

   theStartupFileName = "gmat_startup_file.txt";
   
}

//------------------------------------------------------------------------------
// ~FileManager(void)
//------------------------------------------------------------------------------
FileManager::~FileManager(void)
{
}

