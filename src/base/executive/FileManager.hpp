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
 * Declares FileManager class. This is singleton class which manages list of
 * file paths and names.
 *
 * The textures files and non-Earth gravity potential files not appear in the
 * predefined enum FileType list can be retrieved by using file naming
 * convention. The texture files should have PLANETNAME_TEXTURE_FILE.
 * e.g. "EARTH_TEXTURE_FILE", "LUNA_TEXTURE_FILE", etc. The potential files
 * should have PLANETNAME_POT_FILE.
 */
//------------------------------------------------------------------------------
#ifndef FileManager_hpp
#define FileManager_hpp

#include "gmatdefs.hpp"
#include <map>
#include <fstream>

class GMAT_API FileManager
{
public:
   
   // The following is predefined file paths/types.
   enum FileType
   {
      // file path
      SPLASH_PATH = 0,
      OUTPUT_PATH,
      SLP_PATH,
      DE_PATH,
      EARTH_POT_PATH,
      PLANETARY_COEFF_PATH,
      TIME_PATH,
      TEXTURE_PATH, //Notes: TEXTURE_PATH is used in SetPathname()
      
      // file name
      LOG_FILE,
      REPORT_FILE,
      SPLASH_FILE,
      TIME_COEFF_FILE,
      SLP_FILE,
      DE200_FILE,
      DE202_FILE,
      DE405_FILE,
      JGM2_FILE,
      JGM3_FILE,
      EOP_FILE,
      PLANETARY_COEFF_FILE,
      NUTATION_COEFF_FILE,
      LEAP_SECS_FILE,
      FileTypeCount,
   };
   
   static FileManager* Instance();
   ~FileManager();
   
   void ReadStartupFile(const std::string &fileName = "");
   void WriteStartupFile(const std::string &fileName = "");

   std::string GetRootPath();
   
   std::string GetPathname(const FileType type);
   std::string GetPathname(const std::string &typeName);
   
   std::string GetFilename(const FileType type);
   std::string GetFilename(const std::string &typeName);
   
   //loj: Why the name "GetFullPathName()" doesn't work? Reserved word?
   // I'm getting unresolved ref on GetFullPathNameA()
   std::string GetFullPathname(const FileType type);
   std::string GetFullPathname(const std::string &typeName);

   void SetAbsPathname(const FileType type, const std::string &newpath);
   void SetAbsPathname(const std::string &type, const std::string &newpath);
   
private:

   static const Integer MAX_LENGTH = 512;
   static const std::string VERSION_DATE;
   
   struct FileInfo
   {
      std::string mPath;
      std::string mFile;
      
      FileInfo(const std::string &path, const std::string &file)
         { mPath = path; mFile = file; }
   };
   
   std::string mStartupFileName;
   std::ifstream mInStream;
   std::map<std::string, std::string> mPathMap;
   std::map<std::string, FileInfo*> mFileMap;
   
   void AddFileType(const std::string &type, const std::string &name);
   
   static FileManager *theInstance;
   static const std::string FILE_TYPE_STRING[FileTypeCount];
   
   FileManager();
   
};
#endif // FileManager_hpp
