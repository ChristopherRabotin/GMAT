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
#include <iomanip>

//#define FM_CREATE_DEFAULT_INPUT

//#define DEBUG_FILE_MANAGER 1

//---------------------------------
// static data
//---------------------------------
const std::string
FileManager::FILE_TYPE_STRING[FileTypeCount] =
{
   // file path
   "SPLASH_PATH",
   "OUTPUT_PATH",
   "SLP_PATH",
   "DE_PATH",
   "EARTH_POT_PATH",
   "PLANETARY_COEFF_PATH",
   "TIME_PATH",
   "TEXTURE_PATH",
   
   // file name
   "LOG_FILE",
   "REPORT_FILE",
   "SPLASH_FILE",
   "TIME_COEFF_FILE",
   "SLP_FILE",
   "DE200_FILE",
   "DE202_FILE",
   "DE405_FILE",
   "JGM2_FILE",
   "JGM3_FILE",
   "EOP_FILE",
   "PLANETARY_COEFF_FILE",
   "NUTATION_COEFF_FILE",
   "LEAP_SECS_FILE",
};

FileManager* FileManager::theInstance = NULL;
const std::string FileManager::VERSION_DATE = "2005-07-13";

using namespace std;

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
// ~FileManager()
//------------------------------------------------------------------------------
FileManager::~FileManager()
{
   for (std::map<std::string, FileInfo*>::iterator pos = mFileMap.begin();
        pos != mFileMap.end(); ++pos)
   {
      if (pos->second)
      {
         #if DEBUG_FILE_MANAGER
         MessageInterface::ShowMessage
            ("FileManager::~FileManager deleting %s\n", pos->first.c_str());
         #endif
         
         delete pos->second;
      }
   }
}


//------------------------------------------------------------------------------
// void ReadStartupFile(const std::string &fileName = "")
//------------------------------------------------------------------------------
/**
 * Reads GMAT startup file.
 *
 * @param <fileName> startup file name.
 *
 * @exception thrown if file not found, or VERSION date on the startup up file
 *    does not exist or does not match with VERSION_DATE.
 */
//------------------------------------------------------------------------------
void FileManager::ReadStartupFile(const std::string &fileName)
{
   char line[MAX_LENGTH] = "";
   bool correctVersionFound = false;
   
   if (fileName != "")
      mStartupFileName = fileName;
   
   #if DEBUG_FILE_MANAGER
   MessageInterface::ShowMessage("FileManager::ReadStartupFile() reading:%s\n",
                                 mStartupFileName.c_str());
   #endif
   
   std::ifstream mInStream(mStartupFileName.c_str());
   
   if (!mInStream)
      throw GmatBaseException
         ("FileManager::ReadStartupFile() cannot open:" + fileName);
   
   while (!mInStream.eof())
   {
      line[0] = '\0';
      mInStream.getline(line, 512);
      
      #if DEBUG_FILE_MANAGER
      MessageInterface::ShowMessage("line=%s\n", line);
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
         mInStream.close();
         throw GmatBaseException
            ("FileManager::ReadStartupFile() expecting '=' at line:\n" +
             std::string(line) + "\n");
      }
      
      ss >> name;
      
      #if DEBUG_FILE_MANAGER
      MessageInterface::ShowMessage("type=%s, name=%s\n", type.c_str(), name.c_str());
      #endif
      
      if (type == "VERSION")
      {
         // check for version date
         if (name == VERSION_DATE)
         {
            correctVersionFound = true;
            continue;
         }
         else
         {
            throw GmatBaseException
               ("FileManager::ReadStartupFile() the VERSION is incorrect.\n"
                "The version date it can handle is " + VERSION_DATE + "\n");
         }
      }
      
      if (correctVersionFound)
         AddFileType(type, name);
      else
         throw GmatBaseException
            ("FileManager::ReadStartupFile() the VERSION not found.\n"
             "It no longer can read old startup file.\n");
      
   } // end While()

   // now use log file from the startup file
   MessageInterface::SetLogFile(GetFullPathname("LOG_FILE"));
   MessageInterface::logEnabled = true;
   mInStream.close();
}


//------------------------------------------------------------------------------
// void WriteStartupFile(const std::string &fileName = "")
//------------------------------------------------------------------------------
/**
 * Reads GMAT startup file.
 *
 * @param <fileName> startup file name.
 */
//------------------------------------------------------------------------------
void FileManager::WriteStartupFile(const std::string &fileName)
{
   std::string outFileName = "gmat_startup_file.new.txt";
   
   if (fileName != "")
      outFileName = fileName;
   
   #if DEBUG_FILE_MANAGER
   MessageInterface::ShowMessage
      ("FileManager::WriteStartupFile() outFileName = %s\n", outFileName.c_str());
   #endif
   
   std::ofstream outStream(outFileName.c_str());
   
   if (!outStream)
      throw GmatBaseException
         ("FileManager::WriteStartupFile() cannot open:" + fileName);

   //---------------------------------------------
   // write header
   //---------------------------------------------
   outStream << "VERSION = " << VERSION_DATE << "\n";
   outStream << "#-------------------------------------------------------------"
      "------------------\n";
   outStream << "# ! Do not remove or change VERSION date, "
      "it won't work otherwise!!\n";
   outStream << "# Only the new FileManager, version after " << VERSION_DATE <<
      " reconizes this new format.\n";
   outStream << "#-------------------------------------------------------------"
      "------------------\n";
   
   outStream.setf(std::ios::left);
   
   //---------------------------------------------
   // write paths
   //---------------------------------------------
   // write ROOT_PATH first
   outStream << setw(20) << "ROOT_PATH" << " = " << mPathMap["ROOT_PATH"] << "\n";
   
   for (std::map<std::string, std::string>::iterator pos = mPathMap.begin();
        pos != mPathMap.end(); ++pos)
   {
      if (pos->first != "ROOT_PATH")
         outStream << setw(20) << pos->first << " = " << pos->second  << "\n";
   }
   
   outStream << "#-----------------------------------------------------------\n";
   
   //---------------------------------------------
   // write files
   //---------------------------------------------
   for (std::map<std::string, FileInfo*>::iterator pos = mFileMap.begin();
        pos != mFileMap.end(); ++pos)
   {
      if (pos->second)
      {
         outStream << setw(20) << pos->first << " = " << pos->second->mPath << "/"
                   << pos->second->mFile << "\n";
      }
   }
   
   outStream << "#-----------------------------------------------------------\n";
   outStream << "\n";
   outStream.close();
}


//------------------------------------------------------------------------------
// std::string GetRootPath()
//------------------------------------------------------------------------------
/**
 * Retrives root pathname.
 *
 * @return file pathname if path type found.
 */
//------------------------------------------------------------------------------
std::string FileManager::GetRootPath()
{
   return mPathMap["ROOT_PATH"];
}


//------------------------------------------------------------------------------
// std::string GetPathname(const FileType type)
//------------------------------------------------------------------------------
/**
 * Retrives file pathname for the type. 
 *
 * @param <type> enum file type of which pathname to be returned.
 *
 * @return file pathname if path type found.
 * @exception thrown if enum type is out of bounds.
 */
//------------------------------------------------------------------------------
std::string FileManager::GetPathname(const FileType type)
{
   if (type >=0 && type < FileTypeCount)
      return GetPathname(FILE_TYPE_STRING[type]);
   
   std::stringstream ss("");
   ss << "FileManager::GetPathname() enum type: " << type
      << " is out of bounds\n";
   
   throw GmatBaseException(ss.str());
}


//------------------------------------------------------------------------------
// std::string GetPathname(const std::string &typeName)
//------------------------------------------------------------------------------
/**
 * Retrives file pathname for the type name.
 *
 * @param <typeName> file type name of which pathname to be returned.
 *
 * @return pathname if type found.
 * @exception thrown if type cannot be found.
 */
//------------------------------------------------------------------------------
std::string FileManager::GetPathname(const std::string &typeName)
{
   // typeName contains _PATH
   if (typeName.find("_PATH") != typeName.npos)
   {
      if (mPathMap.find(typeName) != mPathMap.end())
         return mPathMap[typeName];
   }
   else
   {
      if (mFileMap.find(typeName) != mFileMap.end())
      {
         // Replace ROOT_PATH with abs path
         std::string pathname = mPathMap[mFileMap[typeName]->mPath];
         if (pathname.find("ROOT_PATH") != pathname.npos)
         {
            std::string pathname2 = mPathMap["ROOT_PATH"] +
               mPathMap[mFileMap[typeName]->mPath];
            std::string::size_type pos1 = pathname2.find("ROOT_PATH");
            pathname2.replace(pos1, 10, "");
            return pathname2;
         }
         else
         {
            return mPathMap[mFileMap[typeName]->mPath];
         }
      }
   }
   
   throw GmatBaseException("FileManager::GetPathname() file type: " + typeName +
                           " is unknown\n");
}


//------------------------------------------------------------------------------
// std::string GetFilename(const FileType type)
//------------------------------------------------------------------------------
/**
 * Retrives filename for the type. 
 *
 * @param <type> enum file type of which filename to be returned.
 *
 * @return file filename if file type found
 * @exception thrown if enum type is out of bounds
 */
//------------------------------------------------------------------------------
std::string FileManager::GetFilename(const FileType type)
{
   if (type >=0 && type < FileTypeCount)
      return GetFilename(FILE_TYPE_STRING[type]);
   
   std::stringstream ss("");
   ss << "FileManager::GetFilename() enum type: " << type
      << " is out of bounds\n";
   
   throw GmatBaseException(ss.str());
}


//------------------------------------------------------------------------------
// std::string GetFilename(const std::string &typeName)
//------------------------------------------------------------------------------
/**
 * Retrives filename for the type name.
 *
 * @param <type> file type name of which filename to be returned.
 *
 * @return file filename if file type found
 * @exception thrown if type cannot be found.
 */
//------------------------------------------------------------------------------
std::string FileManager::GetFilename(const std::string &typeName)
{
   if (mFileMap.find(typeName) != mFileMap.end())
      return mFileMap[typeName]->mFile;

   //MessageInterface::ShowMessage
   //   ("FileManager::GetFilename() file type: %s is unknown\n", typeName.c_str());
   
   //return "UNKNOWN_FILE_TYPE";
   
   throw GmatBaseException("FileManager::GetFilename() file type: " + typeName +
                           " is unknown\n");
}


//------------------------------------------------------------------------------
// std::string GetFullPathname(const FileType type)
//------------------------------------------------------------------------------
/**
 * Retrives full pathname for the type.
 *
 * @param <type> file type of which filename to be returned.
 *
 * @return file pathname if file type found
 * @exception thrown if enum type is out of bounds
 */
//------------------------------------------------------------------------------
std::string FileManager::GetFullPathname(const FileType type)
{
   if (type >=0 && type < FileTypeCount)
      return GetFullPathname(FILE_TYPE_STRING[type]);
   
   std::stringstream ss("");
   ss << "FileManager::GetFullPathname() enum type: " << type <<
      " is out of bounds\n";
   
   throw GmatBaseException(ss.str());
}


//------------------------------------------------------------------------------
// std::string GetFullPathname(const std::string &typeName)
//------------------------------------------------------------------------------
/**
 * Retrives full pathname for the type name.
 *
 * @param <type> file type name of which filename to be returned.
 *
 * @return file pathname if file type name found
 * @exception thrown if type cannot be found.
 */
//------------------------------------------------------------------------------
std::string FileManager::GetFullPathname(const std::string &typeName)
{
   // typeName contains _PATH (loj: 1/9/06)
   if (typeName.find("_PATH") != typeName.npos)
   {
      if (mPathMap.find(typeName) != mPathMap.end())
      {
         // Replace ROOT_PATH with abs path
         std::string pathname = mPathMap[typeName];
         if (pathname.find("ROOT_PATH") != pathname.npos)
         {
            std::string pathname2 = mPathMap["ROOT_PATH"] + pathname;
            std::string::size_type pos1 = pathname2.find("ROOT_PATH");
            pathname2.replace(pos1, 10, "");
            return pathname2;
         }
         else
         {
            return pathname;
         }
      }
   }
   else
   {
      if (mFileMap.find(typeName) != mFileMap.end())
      {
         std::string path = GetPathname(typeName);
         return path + mFileMap[typeName]->mFile;
      }
   }
   
   //MessageInterface::ShowMessage
   //   ("FileManager::GetFullPathname() file type: %s is unknown\n",
   //    typeName.c_str());
   
   //return "UNKNOWN_FILE_TYPE";
   
   throw GmatBaseException("FileManager::GetFullPathname() file type: " +
                           typeName + " is unknown\n");
}


//------------------------------------------------------------------------------
// void SetAbsPathname(const FileType type, const std::string &newpath)
//------------------------------------------------------------------------------
/**
 * Sets absoulute pathname for the type.
 *
 * @param <type> file type of which path to be set.
 * @param <newpath> new pathname.
 *
 * @exception thrown if enum type is out of bounds
 */
//------------------------------------------------------------------------------
void FileManager::SetAbsPathname(const FileType type, const std::string &newpath)
{
   if (type >=0 && type <= TEXTURE_PATH)
   {
      SetAbsPathname(FILE_TYPE_STRING[type], newpath);
   }
   else
   {
      std::stringstream ss("");
      ss << "FileManager::GetFullPathname() enum type: " << type <<
         " is out of bounds of file path\n";
   
      throw GmatBaseException(ss.str());
   }
}


//------------------------------------------------------------------------------
// void SetAbsPathname(const std::string &type, const std::string &newpath)
//------------------------------------------------------------------------------
/**
 * Sets absolute pathname for the type.
 *
 * @param <type> type name of which path to be set.
 * @param <newpath> new pathname.
 *
 * @exception thrown if enum type is out of bounds
 */
//------------------------------------------------------------------------------
void FileManager::SetAbsPathname(const std::string &type, const std::string &newpath)
{
   if (mPathMap.find(type) != mPathMap.end())
   {
      if (type.find("_PATH") != type.npos)
      {
         std::string str2 = newpath;

         // append '/' if not there
         if (str2.find_last_of('/') != str2.length()-1)
            str2 = str2 + "/";
         
         mPathMap[type] = str2;
         //MessageInterface::ShowMessage
         //   ("==>FileManager::SetAbsPathname() %s set to %s\n", type.c_str(),
         //    str2.c_str());
      }
      else
      {
         throw GmatBaseException
            ("FileManager::SetAbsPathname() type doesn't contain _PATH");
      }
   }
}


//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void AddFileType(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Adds file type, path, name to the list. If typeName ends with _PATH, it is
 * added to path map. If typeName ends with _FILE, it is added to file map, an
 * exception is throw otherwise.
 *
 * @param <type> file type
 * @param <name> file path or name
 *
 * @excepton thrown if typeName does not end with _PATH or _FILE
 */
//------------------------------------------------------------------------------
void FileManager::AddFileType(const std::string &type, const std::string &name)
{
   #if DEBUG_FILE_MANAGER
   MessageInterface::ShowMessage
      ("FileManager::AddFileType() type=%s, name=%s\n", type.c_str(), name.c_str());
   #endif
   
   if (type.find("_PATH") != type.npos)
   {
      mPathMap[type] = name;
   }
   else if (type.find("_FILE") != type.npos)
   {
      std::string pathName;
      std::string fileName;
      
      // file name
      std::string::size_type pos = name.find("/");
      if (pos != name.npos)
      {
         std::string pathName = name.substr(0, pos);
         std::string fileName = name.substr(pos+1, name.npos);
         mFileMap[type] = new FileInfo(pathName, fileName);
      }
      else
      {
         //loj: Should we add current path?
         std::string pathName = "CURRENT_PATH";
         mPathMap[pathName] = "./";
         std::string fileName = name;
         mFileMap[type] = new FileInfo(pathName, fileName);
         
         MessageInterface::ShowMessage
            ("FileManager::AddFileType() 'PATH/' not found in line:\n% = % \n"
             "So adding CURRENT_PATH = ./\n", type.c_str(), name.c_str());
         
         //loj: Should we just throw an exception?
         //mInStream.close();
         //throw GmatBaseException
         //   ("FileManager::AddFileType() expecting 'PATH/' in line:\n" +
         //    type + " = " + name);
      }
   }
   else
   {
      throw GmatBaseException
         ("FileManager::AddFileType() file type should have '_PATH' or '_FILE'"
          " in:\n" + type);
   }
}


//------------------------------------------------------------------------------
// FileManager()
//------------------------------------------------------------------------------
/*
 * Constructor
 */
//------------------------------------------------------------------------------
FileManager::FileManager()
{
   MessageInterface::logEnabled = false; // so that debug can be written from here
   MessageInterface::ShowMessage("FileManager::FileManager() entered\n");
   
   AddFileType("ROOT_PATH", "./");
   mStartupFileName = "gmat_startup_file.txt";
   
   //-------------------------------------------------------
   // create default output paths and files
   //-------------------------------------------------------
   // output file path
   AddFileType("OUTPUT_PATH", "./files/output/");
   AddFileType("LOG_FILE", "OUTPUT_PATH/GmatLog.txt");
   AddFileType("REPORT_FILE", "OUTPUT_PATH/ReportFile.txt");

   //loj: Should we create default input files?
#ifdef FM_CREATE_DEFAULT_INPUT
   
   //-------------------------------------------------------
   // create default input paths and files
   //-------------------------------------------------------
   // output file path
   AddFileType("OUTPUT_PATH", "./files/output/");
   AddFileType("LOG_FILE", "OUTPUT_PATH/GmatLog.txt");
   AddFileType("REPORT_FILE", "OUTPUT_PATH/ReportFile.txt");
   
   // texture file path
   AddFileType("TEXTURE_PATH", "./files/plot/texture/");
   
   // slp files
   AddFileType("SLP_PATH", "./files/planetary_ephem/slp/");
   AddFileType("SLP_FILE", "SLP_PATH/mn2000.pc");
   AddFileType("SLP_TIME_COEFF_FILE", "SLP_PATH/timecof.pc");
   
   // de files
   AddFileType("DE_PATH", "./files/planetary_ephem/de/");
   AddFileType("DE200_FILE", "DE_PATH/winp1941.200");
   AddFileType("DE202_FILE", "DE_PATH/winp1950.202");
   AddFileType("DE405_FILE", "DE_PATH/winp1941.405");
   
   // earth gravity files
   AddFileType("EARTH_POT_PATH", "./files/gravity/earth/");
   AddFileType("JGM2_FILE", "EARTH_POT_PATH/JGM2.grv");
   AddFileType("JGM3_FILE", "EARTH_POT_PATH/JGM3.grv");
   
   // planetary coeff. fiels
   AddFileType("PLANETARY_COEFF_PATH", "./files/planetary_coeff/");
   AddFileType("EOP_FILE", "PLANETARY_COEFF_PATH/eopc04.62-now");
   AddFileType("PLANETARY_COEFF_FILE", "PLANETARY_COEFF_PATH/NUT85.DAT");
   AddFileType("NUTATION_COEFF_FILE", "PLANETARY_COEFF_PATH/NUTATION.DAT");
   
   // time files
   AddFileType("TIME_PATH", "./files/time/");
   AddFileType("LEAP_SECS_FILE", "TIME_PATH/tai-utc.dat");
   
#endif
      
}


