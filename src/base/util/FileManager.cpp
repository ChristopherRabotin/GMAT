//$Id$
//------------------------------------------------------------------------------
//                            FileManager
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
// Author: Linda Jun, NASA/GSFC
// Created: 2004/04/02
/**
 * Implements FileManager class. This is singleton class which manages
 * list of file paths and names.
 */
//------------------------------------------------------------------------------

#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include "UtilityException.hpp"
#include "StringUtil.hpp"
#include "FileTypes.hpp"          // for GmatFile::MAX_PATH_LEN
#include "FileUtil.hpp"           // for GmatFileUtil::
#include "StringTokenizer.hpp"    // for StringTokenizer()
#include "GmatGlobal.hpp"         // for SetTestingMode()
#include <fstream>
#include <sstream>
#include <iomanip>

#include <algorithm>                    // Required for GCC 4.3

#ifndef _MSC_VER  // if not Microsoft Visual C++
#include <dirent.h>
#endif

// For adding default input path and files
#define __FM_ADD_DEFAULT_INPUT__

//#define DEBUG_FILE_MANAGER
//#define DEBUG_FUNCTION_PATH
//#define DEBUG_ADD_FILETYPE
//#define DEBUG_FILE_PATH
//#define DEBUG_SET_PATH
//#define DEBUG_READ_STARTUP_FILE
//#define DEBUG_WRITE_STARTUP_FILE
//#define DEBUG_PLUGIN_DETECTION
//#define DEBUG_FILE_RENAME
//#define DEBUG_MAPPING

//---------------------------------
// static data
//---------------------------------
const std::string
FileManager::FILE_TYPE_STRING[FileTypeCount] =
{
   // file path
   "BEGIN_OF_PATH",
   "OUTPUT_PATH",
   "DE_PATH",
   "SPK_PATH",
   "EARTH_POT_PATH",
   "LUNA_POT_PATH",
   "VENUS_POT_PATH",
   "MARS_POT_PATH",
   "PLANETARY_COEFF_PATH",
   "TIME_PATH",
   "TEXTURE_PATH",
   "MEASUREMENT_PATH",
   "EPHEM_PATH",
   "GUI_CONFIG_PATH",
   "SPLASH_PATH",
   "ICON_PATH",
   "STAR_PATH",
   "MODEL_PATH",
   "END_OF_PATH",
   // file name
   "LOG_FILE",
   "REPORT_FILE",
   "SPLASH_FILE",
   "TIME_COEFF_FILE",
   // specific file name
   "DE405_FILE",
   "DE421_FILE",						// made change by TUAN NGUYEN
   "DE424_FILE",						// made change by TUAN NGUYEN
   "IAUSOFA_FILE",						// made change by TUAN NGUYEN
   "ICRF_FILE",							// made change by TUAN NGUYEN
   "PLANETARY_SPK_FILE",
   "JGM2_FILE",
   "JGM3_FILE",
   "EGM96_FILE",
   "LP165P_FILE",
   "MGNP180U_FILE",
   "MARS50C_FILE",
   "EOP_FILE",
   "PLANETARY_COEFF_FILE",
   "NUTATION_COEFF_FILE",
   "LEAP_SECS_FILE",
   "LSK_FILE",
   "PERSONALIZATION_FILE",
   "MAIN_ICON_FILE",
   "STAR_FILE",
   "CONSTELLATION_FILE",
   "SPACECRAFT_MODEL_FILE",
   "HELP_FILE",
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
// ~FileManager()
//------------------------------------------------------------------------------
FileManager::~FileManager()
{
   for (std::map<std::string, FileInfo*>::iterator pos = mFileMap.begin();
        pos != mFileMap.end(); ++pos)
   {
      if (pos->second)
      {
         #ifdef DEBUG_FILE_MANAGER
         MessageInterface::ShowMessage
            ("FileManager::~FileManager deleting %s\n", pos->first.c_str());
         #endif

         delete pos->second;
      }
   }
}


//------------------------------------------------------------------------------
// std::string GetPathSeparator()
//------------------------------------------------------------------------------
/**
 * @return path separator; "/" or "\\" dependends on the platform
 */
//------------------------------------------------------------------------------
std::string FileManager::GetPathSeparator()
{
   // Just return "/" for all operating system for consistency (LOJ: 2011.03.18)
   // return GmatFileUtil::GetPathSeparator();
   
   return "/";
}


//------------------------------------------------------------------------------
// std::string GetWorkingDirectory()
//------------------------------------------------------------------------------
/**
 * @return current working directory
 */
//------------------------------------------------------------------------------
std::string FileManager::GetWorkingDirectory()
{
   return GmatFileUtil::GetWorkingDirectory();
}


//------------------------------------------------------------------------------
// bool DoesDirectoryExist(const std::string &dirPath)
//------------------------------------------------------------------------------
/*
 * @return  true  If directory exist, false otherwise
 */
//------------------------------------------------------------------------------
bool FileManager::DoesDirectoryExist(const std::string &dirPath)
{
   return GmatFileUtil::DoesDirectoryExist(dirPath);
}


//------------------------------------------------------------------------------
// bool DoesFileExist(const std::string &filename)
//------------------------------------------------------------------------------
bool FileManager::DoesFileExist(const std::string &filename)
{
   return GmatFileUtil::DoesFileExist(filename);
}


//------------------------------------------------------------------------------
// bool RenameFile(const std::string &oldName, const std::string &newName,
//                 Integer &retCode, bool overwriteIfExists = false)
//------------------------------------------------------------------------------
bool FileManager::RenameFile(const std::string &oldName,
                             const std::string &newName,
                             Integer &retCode, bool overwriteIfExists)
{
   retCode = 0;
   bool oldExists = DoesFileExist(oldName);
   bool newExists = DoesFileExist(newName);
   #ifdef DEBUG_FILE_RENAME
      MessageInterface::ShowMessage("FM::Rename, old file (%s) exists = %s\n",
            oldName.c_str(), (oldExists? "true" : "false"));
      MessageInterface::ShowMessage("FM::Rename, new file (%s) exists = %s\n",
            newName.c_str(), (newExists? "true" : "false"));
   #endif
   // if a file with the old name does not exist, we cannot do anything
   if (!oldExists)
   {
      std::string errmsg = "Error renaming file \"";
      errmsg += oldName + "\" to \"";
      errmsg += newName + "\": file \"";
      errmsg += oldName + "\" does not exist.\n";
      throw UtilityException(errmsg);
   }

   // if a file with the new name does not exist, or exists but we are
   // supposed to overwrite it, try to do the rename
   if ((!newExists) || (newExists && overwriteIfExists))
   {
      #ifdef DEBUG_FILE_RENAME
         MessageInterface::ShowMessage("FM::Rename, attempting to rename %s to %s\n",
               oldName.c_str(), newName.c_str());
      #endif
      retCode = rename(oldName.c_str(), newName.c_str()); // overwriting is platform-dependent!!!!
      if (retCode == 0)
      {
         return true;
      }
      else
      {
         return false;
      }
   }
   else // it exists but we are not to overwrite it
      return false;
}

//------------------------------------------------------------------------------
// std::string GetStartupFileDir()
//------------------------------------------------------------------------------
/*
 * Returns startup file directory without name.
 */
//------------------------------------------------------------------------------
std::string FileManager::GetStartupFileDir()
{
   return mStartupFileDir;
}


//------------------------------------------------------------------------------
// std::string GetStartupFileName()
//------------------------------------------------------------------------------
/*
 * Returns startup file name without directory.
 */
//------------------------------------------------------------------------------
std::string FileManager::GetStartupFileName()
{
   return mStartupFileName;
}


//------------------------------------------------------------------------------
// std::string GetFullStartupFilePath()
//------------------------------------------------------------------------------
/*
 * Returns startup file directory and name
 */
//------------------------------------------------------------------------------
std::string FileManager::GetFullStartupFilePath()
{
   #ifdef DEBUG_STARTUP_FILE
   MessageInterface::ShowMessage
      ("FileManager::GetFullStartupFilePath() mStartupFileDir='%s', "
       "mStartupFileName='%s'\n", mStartupFileDir.c_str(), mStartupFileName.c_str());
   #endif

   if (mStartupFileDir == "")
      return mStartupFileName;
   else
      return mStartupFileDir + mStartupFileName;
}


//------------------------------------------------------------------------------
// void ReadStartupFile(const std::string &fileName = "")
//------------------------------------------------------------------------------
/**
 * Reads GMAT startup file.
 *
 * @param <fileName> startup file name.
 *
 */
//------------------------------------------------------------------------------
void FileManager::ReadStartupFile(const std::string &fileName)
{
   #ifdef DEBUG_READ_STARTUP_FILE
   MessageInterface::ShowMessage
      ("FileManager::ReadStartupFile() entered, fileName='%s'\n", fileName.c_str());
   #endif
   
   RefreshFiles();

   // get current path and application path
   std::string currPath = GmatFileUtil::GetWorkingDirectory();
   std::string appFullPath = GmatFileUtil::GetApplicationPath();

   #ifdef DEBUG_READ_STARTUP_FILE
   MessageInterface::ShowMessage("   currPath = '%s'\n", currPath.c_str());
   MessageInterface::ShowMessage("   appFullPath = '%s'\n", appFullPath.c_str());
   #endif
   
   std::string line;
   mSavedComments.clear();

   std::string tmpStartupDir;
   std::string tmpStartupFile;
   std::string tmpStartupFilePath;

   if (GmatFileUtil::DoesFileExist(fileName))
   {
      tmpStartupFilePath = fileName;
   }
   else
   {
      // Search application directory for startup file
      std::string appPath = GmatFileUtil::ParsePathName(appFullPath);
      std::string newPath = appPath + "gmat_startup_file.txt";
      
      #ifdef DEBUG_READ_STARTUP_FILE
      MessageInterface::ShowMessage("   appPath               = '%s'\n", appPath.c_str());
      MessageInterface::ShowMessage("   new startup file path = '%s'\n", newPath.c_str());
      #endif
      
      if (GmatFileUtil::DoesFileExist(newPath))
      {
         tmpStartupFilePath = newPath;
         
         // set current directory to new path
         if (GmatFileUtil::SetWorkingDirectory(appPath))
         {
            MessageInterface::ShowMessage
               ("GMAT working directory set to '%s'\n", appPath.c_str());
         }
         else
         {
            UtilityException ue;
            ue.SetDetails("FileManager::ReadStartupFile() cannot set working "
                          "directory to: \"%s\"", appPath.c_str());
            throw ue;
         }
      }
   }
   
   tmpStartupDir = GmatFileUtil::ParsePathName(tmpStartupFilePath);
   tmpStartupFile = GmatFileUtil::ParseFileName(tmpStartupFilePath);
   #ifdef DEBUG_READ_STARTUP_FILE
   MessageInterface::ShowMessage("   tmpStartupDir  = '%s'\n", tmpStartupDir.c_str());
   MessageInterface::ShowMessage("   tmpStartupFile = '%s'\n", tmpStartupFile.c_str());
   #endif
   
   if (tmpStartupDir == "")
      tmpStartupFilePath = tmpStartupFile;
   else
      tmpStartupFilePath = tmpStartupDir + mPathSeparator + tmpStartupFile;
   
   // Reworked this part above so removed(LOJ: 2011.12.14)
   #if 0
   if (fileName == "")
   {
      tmpStartupDir = "";
      tmpStartupFile = mStartupFileName;
      tmpStartupFilePath = mStartupFileName;
   }
   else
   {
      tmpStartupDir = GmatFileUtil::ParsePathName(fileName);
      tmpStartupFile = GmatFileUtil::ParseFileName(fileName);

      if (tmpStartupDir == "")
         tmpStartupFilePath = tmpStartupFile;
      else
         tmpStartupFilePath = tmpStartupDir + mPathSeparator + tmpStartupFile;
   }
   #endif

   
   #ifdef DEBUG_READ_STARTUP_FILE
   MessageInterface::ShowMessage
      ("FileManager::ReadStartupFile() reading '%s'\n", tmpStartupFilePath.c_str());
   #endif

   std::ifstream mInStream(tmpStartupFilePath.c_str());

   if (!mInStream)
   {
      UtilityException ue;
      ue.SetDetails("FileManager::ReadStartupFile() cannot open GMAT startup "
                    "file: \"%s\"", tmpStartupFilePath.c_str());
      throw ue;
   }

   
   // Read startup file
   while (!mInStream.eof())
   {
      // Use cross-platform GetLine
      GmatFileUtil::GetLine(&mInStream, line);
      
      #ifdef DEBUG_READ_STARTUP_FILE
      MessageInterface::ShowMessage("line=%s\n", line.c_str());
      #endif

      // Skip empty line or comment line
      if (line.length() > 0)     // Crashes in VS 2010 debugger without this
      {
         if (line[0] == '\0' || line[0] == '#')
         {
            // save line with ## in the first col
            if (line.size() > 1 && line[1] == '#')
               mSavedComments.push_back(line);
            continue;
         }
      }
      else
         continue;

      std::string type, equal, name;
      std::stringstream ss("");

      ss << line;
      ss >> type >> equal;

      if (equal != "=")
      {
         mInStream.close();
         throw UtilityException
            ("FileManager::ReadStartupFile() expecting '=' at line:\n" +
             std::string(line) + "\n");
      }

      // To fix bug 1916 (LOJ: 2010.10.08)
      // Since >> uses space as deliminter, we cannot use it.
      // So use GmatStringUtil::DecomposeBy() instead.
      //ss >> name;
      
      StringArray parts = GmatStringUtil::DecomposeBy(line, "=");
      name = parts[1];
      name = GmatStringUtil::Trim(name);
      
      #ifdef DEBUG_READ_STARTUP_FILE
      MessageInterface::ShowMessage("type=%s, name=%s\n", type.c_str(), name.c_str());
      #endif
      
      
      if (type == "RUN_MODE")
      {
         mRunMode = name;
         if (name == "TESTING")
            GmatGlobal::Instance()->SetRunMode(GmatGlobal::TESTING);
         else if (name == "TESTING_NO_PLOTS")
            GmatGlobal::Instance()->SetRunMode(GmatGlobal::TESTING_NO_PLOTS);
         else if (name == "EXIT_AFTER_RUN")
            GmatGlobal::Instance()->SetRunMode(GmatGlobal::EXIT_AFTER_RUN);
      }
      else if (type == "PLOT_MODE")
      {
         mPlotMode = name;
         if (name == "TILE")
            GmatGlobal::Instance()->SetPlotMode(GmatGlobal::TILED_PLOT);
      }
      else if (type == "MATLAB_MODE")
      {
         mMatlabMode = name;
         if (name == "SINGLE")
            GmatGlobal::Instance()->SetMatlabMode(GmatGlobal::SINGLE_USE);
         else if (name == "SHARED")
            GmatGlobal::Instance()->SetMatlabMode(GmatGlobal::SHARED);
         else if (name == "NO_MATLAB")
            GmatGlobal::Instance()->SetMatlabMode(GmatGlobal::NO_MATLAB);
      }
      else if (type == "DEBUG_MATLAB")
      {
         if (name == "ON")
         {
            mDebugMatlab = name;
            GmatGlobal::Instance()->SetMatlabDebug(true);
         }
      }
      else if (type == "DEBUG_MISSION_TREE")
      {
         if (name == "ON")
         {
            mDebugMissionTree = name;
            GmatGlobal::Instance()->SetMissionTreeDebug(true);
         }
      }
      else if (type == "DEBUG_PARAMETERS")
      {
         if (name == "ON")
         {
            mWriteParameterInfo = name;
            GmatGlobal::Instance()->SetWriteParameterInfo(true);
         }
      }
      else if (type == "WRITE_GMAT_KEYWORD")
      {
         if (name == "OFF")
         {
            mWriteGmatKeyword = name;
            GmatGlobal::Instance()->SetWriteGmatKeyword(false);
         }
      }
      else if (type == "HIDE_SAVEMISSION")
      {
          if (name == "TRUE")
			GmatGlobal::Instance()->AddHiddenCommand("SaveMission");
		  else
			  GmatGlobal::Instance()->RemoveHiddenCommand("SaveMission");
      }
      else
      {
         // Ignore old VERSION specification (2011.03.18)
         if (type != "VERSION")
            AddFileType(type, name);
      }
   } // end While()
   
   // Since we set all output to ./ as default, we don't need this (LOJ: 2011.03.17)
   // Set EPHEM_PATH to OUTPUT_PATH from the startup file if not set
   // so that ./output directory is not required when writing the ephemeris file.
//    if (mPathMap["EPHEM_PATH"] == "./output/" &&
//        mPathMap["OUTPUT_PATH"] != "./files/output/")
//    {
//       mPathMap["EPHEM_PATH"] = mPathMap["OUTPUT_PATH"];
//       #ifdef DEBUG_READ_STARTUP_FILE
//       MessageInterface::ShowMessage
//          ("==> EPHEM_PATH set to '%s'\n", mPathMap["EPHEM_PATH"].c_str());
//       #endif
//    }
   
   // add potential files by type names
   AddAvailablePotentialFiles();
   
   // save good startup file
   mStartupFileDir = tmpStartupDir;
   mStartupFileName = tmpStartupFile;
   
   // now use log file from the startup file
   MessageInterface::SetLogFile(GetAbsPathname("LOG_FILE"));
   MessageInterface::SetLogEnable(true);
   mInStream.close();
   
   #ifdef DEBUG_MAPPING
   ShowMaps("In ReadStartupFile()");
   #endif
}


//------------------------------------------------------------------------------
// void WriteStartupFile(const std::string &fileName = "")
//------------------------------------------------------------------------------
/**
 * Reads GMAT startup file.
 *
 * @param <fileName> startup file name.
 *
 * @exception UtilityException thrown if file cannot be opened
 */
//------------------------------------------------------------------------------
void FileManager::WriteStartupFile(const std::string &fileName)
{
   std::string outFileName = "gmat_startup_file.new.txt";
   mPathWrittenOuts.clear();
   mFileWrittenOuts.clear();
   
   if (fileName != "")
      outFileName = fileName;
   
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage
      ("FileManager::WriteStartupFile() entered, outFileName = %s\n",
       outFileName.c_str());
   #endif

   std::ofstream outStream(outFileName.c_str());

   if (!outStream)
      throw UtilityException
         ("FileManager::WriteStartupFile() cannot open:" + fileName);

   //---------------------------------------------
   // write header
   //---------------------------------------------
   WriteHeader(outStream);
   
   // set left justified
   outStream.setf(std::ios::left);
   
   // don't write CURRENT_PATH
   mPathWrittenOuts.push_back("CURRENT_PATH");
   
   //---------------------------------------------
   // write RUN_MODE if not blank
   //---------------------------------------------
   if (mRunMode != "")
   {
      #ifdef DEBUG_WRITE_STARTUP_FILE
      MessageInterface::ShowMessage("   .....Writing RUN_MODE\n");
      #endif
      outStream << std::setw(22) << "RUN_MODE" << " = " << mRunMode << "\n";
   }
   
   // Write other option as comments
   outStream << std::setw(22) << "#RUN_MODE" << " = TESTING\n";
   outStream << std::setw(22) << "#RUN_MODE" << " = TESTING_NO_PLOTS\n";
   outStream << std::setw(22) << "#RUN_MODE" << " = EXIT_AFTER_RUN\n";
   
   //---------------------------------------------
   // write PLOT_MODE if not blank
   //---------------------------------------------
   if (mPlotMode != "")
   {
      #ifdef DEBUG_WRITE_STARTUP_FILE
      MessageInterface::ShowMessage("   .....Writing PLOT_MODE\n");
      #endif
      outStream << std::setw(22) << "PLOT_MODE" << " = " << mRunMode << "\n";
   }
   
   // Write other option as comments
   // There are no other options implemented for now
   //outStream << std::setw(22) << "#PLOT_MODE" << " = TILE\n";
   
   //---------------------------------------------
   // write MATLAB_MODE if not blank
   //---------------------------------------------
   if (mMatlabMode != "")
   {
      #ifdef DEBUG_WRITE_STARTUP_FILE
      MessageInterface::ShowMessage("   .....Writing MATLAB_MODE\n");
      #endif
      outStream << std::setw(22) << "MATLAB_MODE" << " = " << mMatlabMode << "\n";
   }
   
   // Write other option as comments
   outStream << std::setw(22) << "#MATLAB_MODE" << " = SINGLE\n";
   outStream << std::setw(22) << "#MATLAB_MODE" << " = SHARED\n";
   outStream << std::setw(22) << "#MATLAB_MODE" << " = NO_MATLAB\n";
   
   //---------------------------------------------
   // write DEBUG_MATLAB if not blank
   //---------------------------------------------
   if (mDebugMatlab != "")
   {
      #ifdef DEBUG_WRITE_STARTUP_FILE
      MessageInterface::ShowMessage("   .....Writing DEBUG_MATLAB\n");
      #endif
      outStream << std::setw(22) << "DEBUG_MATLAB" << " = " << mDebugMatlab << "\n";
   }
      
   //---------------------------------------------
   // write DEBUG_MISSION_TREE if not blank
   //---------------------------------------------
   if (mDebugMissionTree != "")
   {
      #ifdef DEBUG_WRITE_STARTUP_FILE
      MessageInterface::ShowMessage("   .....Writing DEBUG_MISSION_TREE\n");
      #endif
      outStream << std::setw(22) << "DEBUG_MISSION_TREE" << " = " << mDebugMissionTree << "\n";
   }
   
   if (mRunMode != "" || mPlotMode != "" || mMatlabMode != "" ||
       mDebugMatlab != "" || mDebugMissionTree != "")
      outStream << "#-----------------------------------------------------------\n";
   
   //---------------------------------------------
   // write DEBUG_PARAMETERS if not blank
   //---------------------------------------------
   if (mWriteParameterInfo != "")
   {
      #ifdef DEBUG_WRITE_STARTUP_FILE
      MessageInterface::ShowMessage("   .....Writing PARAMETER_INFO\n");
      #endif
      outStream << std::setw(22) << "DEBUG_PARAMETERS" << " = " << mWriteParameterInfo << "\n";
   }
   
   if (mRunMode != "" || mPlotMode != "" || mMatlabMode != "" ||
       mDebugMatlab != "" || mDebugMissionTree != "" || mWriteParameterInfo != "")
      outStream << "#-----------------------------------------------------------\n";
   
   //---------------------------------------------
   // write WRITE_GMAT_KEYWORD if not blank
   //---------------------------------------------
   if (mWriteGmatKeyword != "")
   {
      #ifdef DEBUG_WRITE_STARTUP_FILE
      MessageInterface::ShowMessage("   .....Writing PARAMETER_INFO\n");
      #endif
      outStream << std::setw(22) << "WRITE_GMAT_KEYWORD" << " = " << mWriteGmatKeyword << "\n";
   }
   
   if (mRunMode != "" || mPlotMode != "" || mMatlabMode != "" ||
       mDebugMatlab != "" || mDebugMissionTree != "" || mWriteParameterInfo != "" ||
       mWriteGmatKeyword != "" )
      outStream << "#-----------------------------------------------------------\n";
   
   //---------------------------------------------
   // write ROOT_PATH next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing ROOT_PATH path\n");
   #endif
   outStream << std::setw(22) << "ROOT_PATH" << " = " << mPathMap["ROOT_PATH"]
             << "\n";
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("ROOT_PATH");
   
   //---------------------------------------------
   // write PLUGIN next 
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing PLUGIN\n");
   #endif
   if (mPluginList.size() > 0)
   {
      for (UnsignedInt i = 0; i < mPluginList.size(); ++i)
      {
         outStream << std::setw(22) << "PLUGIN" << " = " << mPluginList[i]
                   << "\n";
      }
      outStream << "#-----------------------------------------------------------\n";
   }
   
   //---------------------------------------------
   // write OUTPUT_PATH and output files next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing OUTPUT_PATH paths\n");
   #endif
   outStream << std::setw(22) << "OUTPUT_PATH" << " = "
             << mPathMap["OUTPUT_PATH"] << "\n";
   WriteFiles(outStream, "LOG_");
   WriteFiles(outStream, "REPORT_");
   WriteFiles(outStream, "SCREENSHOT_");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("OUTPUT_PATH");
   
   //---------------------------------------------
   // write MEASUREMENT_PATH next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing MEASUREMENT_PATH paths\n");
   #endif
   outStream << std::setw(22) << "MEASUREMENT_PATH" << " = "
             << mPathMap["MEASUREMENT_PATH"] << "\n";
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("MEASUREMENT_PATH");
   
   //---------------------------------------------
   // write the EPHEM_PATH next if set
   //---------------------------------------------
   if (mPathMap["EPHEM_PATH"] != "./output/")
   {
      #ifdef DEBUG_WRITE_STARTUP_FILE
      MessageInterface::ShowMessage("   .....Writing EPHEM_PATH path\n");
      #endif
      outStream << std::setw(22) << "EPHEM_PATH" << " = "
                << mPathMap["EPHEM_PATH"];
      outStream << "\n#---------------------------------------------"
            "--------------\n";
      mPathWrittenOuts.push_back("EPHEM_PATH");
   }
   
   //---------------------------------------------
   // write GMAT_FUNCTION_PATH next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing GMAT_FUNCTION_PATH paths\n");
   #endif
   bool isEmptyPath = true;
   for (std::map<std::string, std::string>::iterator pos = mPathMap.begin();
        pos != mPathMap.end(); ++pos)
   {
      if (pos->first == "GMAT_FUNCTION_PATH")
      {
         // Write all GmatFunction paths
         std::list<std::string>::iterator listpos = mGmatFunctionPaths.begin();
         while (listpos != mGmatFunctionPaths.end())
         {
            outStream << std::setw(22) << pos->first << " = "
                      << *listpos << "\n";
            ++listpos;
         }
         isEmptyPath = false;
         break;
      }
   }
   if (isEmptyPath)
      outStream << std::setw(22) << "#GMAT_FUNCTION_PATH " << " = " << "\n";
   
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("GMAT_FUNCTION_PATH");
   
   //---------------------------------------------
   // write MATLAB_FUNCTION_PATH next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing MATLAB_FUNCTION_PATH paths\n");
   #endif
   isEmptyPath = true;
   for (std::map<std::string, std::string>::iterator pos = mPathMap.begin();
        pos != mPathMap.end(); ++pos)
   {
      if (pos->first == "MATLAB_FUNCTION_PATH")
      {
         // Write all GmatFunction paths
         std::list<std::string>::iterator listpos = mMatlabFunctionPaths.begin();
         while (listpos != mMatlabFunctionPaths.end())
         {
            outStream << std::setw(22) << pos->first << " = " << *listpos << "\n";
            ++listpos;
         }
         break;
      }
   }
   if (isEmptyPath)
      outStream << std::setw(22) << "#MATLAB_FUNCTION_PATH " << " = " << "\n";
   
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("MATLAB_FUNCTION_PATH");
   
   //---------------------------------------------
   // write DATA_PATH next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing DATA_PATH path\n");
   #endif
   outStream << std::setw(22) << "DATA_PATH" << " = " << mPathMap["DATA_PATH"]
             << "\n";
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("DATA_PATH");
   
   //---------------------------------------------
   // write any relative path used in SPK_PATH
   //---------------------------------------------
   std::string spkPath = mPathMap["SPK_PATH"];
   if (spkPath.find("_PATH") != spkPath.npos)
   {
      std::string relPath = GmatFileUtil::ParseFirstPathName(spkPath, false);
      if (find(mPathWrittenOuts.begin(), mPathWrittenOuts.end(), relPath) ==
          mPathWrittenOuts.end())
      {
         #ifdef DEBUG_WRITE_STARTUP_FILE
         MessageInterface::ShowMessage("   .....Writing %s\n", relPath.c_str());
         #endif
         outStream << std::setw(22) << relPath << " = " << mPathMap[relPath] << "\n";
         outStream << "#-----------------------------------------------------------\n";
         mPathWrittenOuts.push_back(relPath);
      }
   }
   
   //---------------------------------------------
   // write the SPK_PATH and SPK file next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing SPK path\n");
   #endif
   outStream << std::setw(22) << "SPK_PATH" << " = "
             << mPathMap["SPK_PATH"] << "\n";
   WriteFiles(outStream, "SPK");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("SPK_PATH");
   
   //---------------------------------------------
   // write the DE_PATH and DE file next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing DE path\n");
   #endif
   outStream << std::setw(22) << "DE_PATH" << " = "
             << mPathMap["DE_PATH"] << "\n";
   WriteFiles(outStream, "DE405");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("DE_PATH");
   
   //---------------------------------------------
   // write the PLANETARY_COEFF_PATH and files next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing PLANETARY_COEFF_PATH path\n");
   #endif
   outStream << std::setw(22) << "PLANETARY_COEFF_PATH" << " = "
             << mPathMap["PLANETARY_COEFF_PATH"] << "\n";
   WriteFiles(outStream, "EOP_FILE");
   WriteFiles(outStream, "PLANETARY_COEFF_FILE");
   WriteFiles(outStream, "NUTATION_COEFF_FILE");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("PLANETARY_COEFF_PATH");

   //---------------------------------------------
   // write the TIME_PATH and TIME file next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing TIME path\n");
   #endif
   outStream << std::setw(22) << "TIME_PATH" << " = " << mPathMap["TIME_PATH"] << "\n";
   WriteFiles(outStream, "LEAP_");
   WriteFiles(outStream, "LSK_");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("TIME_PATH");

   //---------------------------------------------
   // write *_POT_PATH and files next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing *_POT_PATH paths\n");
   #endif
   for (std::map<std::string, std::string>::iterator pos = mPathMap.begin();
        pos != mPathMap.end(); ++pos)
   {
      if (pos->first.find("_POT_") != std::string::npos)
      {
         outStream << std::setw(22) << pos->first << " = "
                   << pos->second << "\n";
         mPathWrittenOuts.push_back(pos->first);
      }
   }
   outStream << "#-----------------------------------------------------------\n";
   WriteFiles(outStream, "POT_FILE");
   WriteFiles(outStream, "EGM96");
   WriteFiles(outStream, "JGM");
   WriteFiles(outStream, "MARS50C");
   WriteFiles(outStream, "MGNP180U");
   WriteFiles(outStream, "LP165P");
   
   outStream << "#-----------------------------------------------------------\n";

   //---------------------------------------------
   // write the GUI_CONFIG_PATH and files next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing GUI_CONFIG_PATH path\n");
   #endif
   outStream << std::setw(22) << "GUI_CONFIG_PATH" << " = "
             << mPathMap["GUI_CONFIG_PATH"] << "\n";
   WriteFiles(outStream, "PERSONALIZATION_FILE");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("GUI_CONFIG_PATH");
   
   //---------------------------------------------
   // write the ICON_PATH and files next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing ICON_PATH path\n");
   #endif
   outStream << std::setw(22) << "ICON_PATH" << " = "
             << mPathMap["ICON_PATH"] << "\n";
   WriteFiles(outStream, "ICON_FILE");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("ICON_PATH");

   //---------------------------------------------
   // write the SPLASH_PATH and files next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing SPLASH_PATH path\n");
   #endif
   outStream << std::setw(22) << "SPLASH_PATH" << " = "
             << mPathMap["SPLASH_PATH"] << "\n";
   WriteFiles(outStream, "SPLASH_FILE");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("SPLASH_PATH");
   
   //---------------------------------------------
   // write the TEXTURE_PATH and files next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing TEXTURE_PATH path\n");
   #endif
   outStream << std::setw(22) << "TEXTURE_PATH" << " = "
             << mPathMap["TEXTURE_PATH"] << "\n";
   WriteFiles(outStream, "TEXTURE_FILE");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("TEXTURE_PATH");

   //---------------------------------------------
   // write the STAR_PATH and files next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing STAR_PATH path\n");
   #endif
   outStream << std::setw(22) << "STAR_PATH" << " = "
             << mPathMap["STAR_PATH"] << "\n";
   WriteFiles(outStream, "STAR_FILE");
   WriteFiles(outStream, "CONSTELLATION_FILE");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("STAR_PATH");

   //---------------------------------------------
   // write the MODEL_PATH and files next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing MODEL_PATH path\n");
   #endif
   outStream << std::setw(22) << "MODEL_PATH" << " = "
             << mPathMap["MODEL_PATH"] << "\n";
   WriteFiles(outStream, "SPACECRAFT_MODEL_FILE");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("MODEL_PATH");
   
   //---------------------------------------------
   // write the HELP_FILE next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing HELP_FILE\n");
   #endif
   
   if (GetFilename("HELP_FILE") == "")
      outStream << std::setw(22) << "#HELP_FILE " << " = " << "\n";
   else
      WriteFiles(outStream, "HELP_FILE");
   outStream << "#-----------------------------------------------------------\n";
   mFileWrittenOuts.push_back("HELP_FILE");
   
   //---------------------------------------------
   // write rest of paths and files
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing rest of paths and files\n");
   #endif
   WriteFiles(outStream, "-OTHER-PATH-");
   WriteFiles(outStream, "-OTHER-");
   outStream << "#-----------------------------------------------------------\n";
   
   //---------------------------------------------
   // write saved comments
   //---------------------------------------------
   if (!mSavedComments.empty())
   {
      #ifdef DEBUG_WRITE_STARTUP_FILE
      MessageInterface::ShowMessage("   .....Writing saved comments\n");
      #endif
      outStream << "# Saved Comments\n";
      outStream << "#-----------------------------------------------------------\n";
      for (UnsignedInt i=0; i<mSavedComments.size(); i++)
         outStream << mSavedComments[i] << "\n";
      outStream << "#-----------------------------------------------------------\n";
   }

   outStream << "\n";
   outStream.close();

   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("FileManager::WriteStartupFile() exiting\n");
   #endif
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
 * Retrives absolute path for the type without filename.
 *
 * @param <type> enum file type of which path to be returned.
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
   
   throw UtilityException(ss.str());
}


//------------------------------------------------------------------------------
// std::string GetPathname(const std::string &typeName)
//------------------------------------------------------------------------------
/**
 * Retrives absolute pathname for the type name without filename.
 *
 * @param <typeName> file type name of which pathname to be returned.
 *
 * @return pathname if type found.
 * @exception thrown if type cannot be found.
 */
//------------------------------------------------------------------------------
std::string FileManager::GetPathname(const std::string &typeName)
{
   std::string fileType = GmatStringUtil::ToUpper(typeName);
   
   #ifdef DEBUG_FILE_PATH
   MessageInterface::ShowMessage
      ("FileManager::GetPathname() entered, flleType='%s'\n", fileType.c_str());
   #endif

   std::string pathname;
   bool nameFound = false;
   
   // if typeName contains _PATH
   if (fileType.find("_PATH") != fileType.npos)
   {
      if (mPathMap.find(fileType) != mPathMap.end())
      {
         pathname = mPathMap[fileType];
         nameFound = true;
      }
   }
   else
   {
      // typeName contains _FILE      
      if (mFileMap.find(fileType) != mFileMap.end())
      {
         pathname = mFileMap[fileType]->mPath;
         nameFound = true;
      }
   }
   
   if (nameFound)
   {
      // Replace relative path with absolute path
      std::string abspath = ConvertToAbsPath(pathname);
      
      #ifdef DEBUG_FILE_PATH
      MessageInterface::ShowMessage
         ("FileManager::GetPathname() returning '%s'\n", abspath.c_str());
      #endif
      
      return abspath;
   }
   else
   {
      throw UtilityException("FileManager::GetPathname() file type: " + typeName +
                             " is unknown\n");
   }
}


//------------------------------------------------------------------------------
// std::string GetFilename(const FileType type)
//------------------------------------------------------------------------------
/**
 * Retrives filename for the type without path.
 *
 * @param <type> enum file type of which filename to be returned.
 *
 * @return file filename if file type found
 * @exception thrown if enum type is out of bounds
 */
//------------------------------------------------------------------------------
std::string FileManager::GetFilename(const FileType type)
{
   bool nameFound = false;
   std::string name;
   if (type >=0 && type < FileTypeCount)
   {
      name = GetFilename(FILE_TYPE_STRING[type]);
      nameFound = true;
   }
   
   if (nameFound)
   {
      name = GmatFileUtil::ParseFileName(name);
      return name;
   }
   
   std::stringstream ss("");
   ss << "FileManager::GetFilename() enum type: " << type
      << " is out of bounds\n";

   throw UtilityException(ss.str());
}


//------------------------------------------------------------------------------
// std::string GetFilename(const std::string &typeName)
//------------------------------------------------------------------------------
/**
 * Retrives filename for the type name without path.
 *
 * @param <type> file type name of which filename to be returned.
 *
 * @return file filename if file type found
 * @exception thrown if type cannot be found.
 */
//------------------------------------------------------------------------------
std::string FileManager::GetFilename(const std::string &typeName)
{
   bool nameFound = false;
   std::string name;
   if (mFileMap.find(typeName) != mFileMap.end())
   {
      name = mFileMap[typeName]->mFile;
      nameFound = true;
   }
   
   if (nameFound)
   {
      name = GmatFileUtil::ParseFileName(name);
      return name;
   }
   
   throw UtilityException("FileManager::GetFilename() file type: " + typeName +
                           " is unknown\n");
}


//------------------------------------------------------------------------------
// std::string GetFullPathname(const FileType type)
//------------------------------------------------------------------------------
/**
 * Retrieves full pathname for the type.
 *
 * @param <type> file type of which filename to be returned.
 *
 * @return file pathname if file type found
 * @exception thrown if enum type is out of bounds
 */
//------------------------------------------------------------------------------
std::string FileManager::GetFullPathname(const FileType type)
{
   return GetAbsPathname(type);
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
   return GetAbsPathname(typeName);
}


//------------------------------------------------------------------------------
// std::string GetAbsPathname(const FileType type)
//------------------------------------------------------------------------------
/**
 * Retrieves full pathname for the type.
 *
 * @param <type> file type of which filename to be returned.
 *
 * @return file pathname if file type found
 * @exception thrown if enum type is out of bounds
 */
//------------------------------------------------------------------------------
std::string FileManager::GetAbsPathname(const FileType type)
{

   if (type >=0 && type < FileTypeCount)
      return GetAbsPathname(FILE_TYPE_STRING[type]);

   std::stringstream ss("");
   ss << "FileManager::GetAbsPathname() enum type: " << type <<
      " is out of bounds\n";

   throw UtilityException(ss.str());
}


//------------------------------------------------------------------------------
// std::string GetAbsPathname(const std::string &typeName)
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
std::string FileManager::GetAbsPathname(const std::string &typeName)
{
   std::string fileType = GmatStringUtil::ToUpper(typeName);
   std::string absPath;

   #ifdef DEBUG_FILE_MANAGER
   MessageInterface::ShowMessage
      ("FileManager::GetAbsPathname() typeName='%s', fileType='%s'\n",
       typeName.c_str(), fileType.c_str());
   #endif
   
   // typeName contains _PATH
   if (fileType.find("_PATH") != fileType.npos)
   {
      if (mPathMap.find(fileType) != mPathMap.end())
      {
         absPath = ConvertToAbsPath(fileType);
                  
         #ifdef DEBUG_FILE_MANAGER
         MessageInterface::ShowMessage
            ("FileManager::GetAbsPathname() with _PATH returning '%s'\n", absPath.c_str());
         #endif
         return absPath;
      }
   }
   else
   {
      if (mFileMap.find(fileType) != mFileMap.end())
      {
         std::string path = GetPathname(fileType);
         absPath = path + mFileMap[fileType]->mFile;
      }
      else if (mFileMap.find(fileType + "_ABS") != mFileMap.end())
      {
         absPath = mFileMap[typeName]->mFile;
      }

      #ifdef DEBUG_FILE_MANAGER
      MessageInterface::ShowMessage
         ("FileManager::GetAbsPathname() without _PATH returning '%s'\n", absPath.c_str());
      #endif
      return absPath;
   }

   throw UtilityException
      (GmatStringUtil::ToUpper(typeName) + " not in the gmat_startup_file\n");

}


//------------------------------------------------------------------------------
// std::string ConvertToAbsPath(const std::string &relPath)
//------------------------------------------------------------------------------
/**
 * Converts relative path to absolute path
 */
//------------------------------------------------------------------------------
std::string FileManager::ConvertToAbsPath(const std::string &relPath)
{
   #ifdef DEBUG_FILE_PATH
   MessageInterface::ShowMessage
      ("FileManager::ConvertToAbsPath() relPath='%s'\n", relPath.c_str());
   #endif
   
   //std::string absPath = relPath;
   std::string absPath;
   StringTokenizer st(relPath, "/\\");
   StringArray allNames = st.GetAllTokens();
   Integer numNames = allNames.size();
   StringArray pathNames;
   
   #ifdef DEBUG_FILE_PATH
   MessageInterface::ShowMessage("There are %d names in relPath\n", numNames);
   for (int i = 0; i < numNames; i++)
      MessageInterface::ShowMessage("   allNames[%d] = '%s'\n", i, allNames[i].c_str());
   #endif
   
   for (UnsignedInt i = 0; i < allNames.size(); i++)
   {
      std::string name = allNames[i];
      
      #ifdef DEBUG_FILE_PATH
      MessageInterface::ShowMessage("   allNames[%d] = '%s'\n", i, name.c_str());
      #endif
      
      absPath = name;
      if (GmatStringUtil::EndsWith(name, "_PATH"))
      {
         #ifdef DEBUG_FILE_PATH
         MessageInterface::ShowMessage("   _PATH found\n");
         #endif
         
         if (mPathMap.find(name) != mPathMap.end())
            absPath = mPathMap[name];
         
         #ifdef DEBUG_FILE_PATH
         MessageInterface::ShowMessage("   1st absPath = '%s'\n", absPath.c_str());
         #endif
         
         // If _PATH found and it is not the same as original name,
         // Call ConvertToAbsPath() again
         if (absPath.find("_PATH") != absPath.npos && absPath != name)
            absPath = ConvertToAbsPath(absPath);
         
         #ifdef DEBUG_FILE_PATH
         MessageInterface::ShowMessage("   2nd absPath = '%s'\n", absPath.c_str());
         #endif
         
         pathNames.push_back(absPath);
      }
      else
      {
         #ifdef DEBUG_FILE_PATH
         MessageInterface::ShowMessage("   _PATH not found\n");
         MessageInterface::ShowMessage("   absPath = '%s'\n", absPath.c_str());
         #endif
         
         pathNames.push_back(absPath);
      }
   }
   
   absPath = "";
   for (UnsignedInt i = 0; i < pathNames.size(); i++)
   {
      if (GmatStringUtil::EndsWithPathSeparator(pathNames[i]))
         absPath = absPath + pathNames[i];
      else
         absPath = absPath + pathNames[i] + "/";
   }
   
   #ifdef DEBUG_FILE_PATH
   MessageInterface::ShowMessage
      ("FileManager::ConvertToAbsPath() returning '%s'\n", absPath.c_str());
   #endif
   
   return absPath;
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
   if (type >= BEGIN_OF_PATH && type <= END_OF_PATH)
   {
      SetAbsPathname(FILE_TYPE_STRING[type], newpath);
   }
   else
   {
      std::stringstream ss("");
      ss << "FileManager::SetAbsPathname() enum type: " << type <<
         " is out of bounds of file path\n";

      throw UtilityException(ss.str());
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
         std::string::size_type index = str2.find_last_of("/\\");
         if (index != str2.length() - 1)
         {
            str2 = str2 + mPathSeparator;
         }
         else
         {
            index = str2.find_last_not_of("/\\");
            str2 = str2.substr(0, index+1) + mPathSeparator;
         }

         mPathMap[type] = str2;

         #ifdef DEBUG_SET_PATH
         MessageInterface::ShowMessage
            ("FileManager::SetAbsPathname() %s set to %s\n", type.c_str(),
             str2.c_str());
         #endif
      }
      else
      {
         throw UtilityException
            ("FileManager::SetAbsPathname() type doesn't contain _PATH");
      }
   }
}


//------------------------------------------------------------------------------
// void ClearGmatFunctionPath()
//------------------------------------------------------------------------------
void FileManager::ClearGmatFunctionPath()
{
   mGmatFunctionPaths.clear();
}


//------------------------------------------------------------------------------
// void  AddGmatFunctionPath(const std::string &path, bool addFront, bool addFront)
//------------------------------------------------------------------------------
/*
 * If new path it adds to the GmatFunction path list.
 * If path already exist, it moves to the front or back of the list, depends on
 * addFront flag.
 *
 * @param  path  path name to be added
 * @param  addFront  if set to true, it adds to the front, else adds to the back (true)
 */
//------------------------------------------------------------------------------
void FileManager::AddGmatFunctionPath(const std::string &path, bool addFront)
{
   #ifdef DEBUG_FUNCTION_PATH
   MessageInterface::ShowMessage
      ("FileManager::AddGmatFunctionPath() Adding %s to GmatFunctionPath\n   "
       "addFront=%d\n", path.c_str(), addFront);
   #endif

   std::string pathname = path;

   // if path has full pathname (directory and filename), remove filename first
   if (path.find(".") != path.npos)
      pathname = GmatFileUtil::ParsePathName(path);

   std::list<std::string>::iterator pos =
      find(mGmatFunctionPaths.begin(), mGmatFunctionPaths.end(), pathname);

   if (pos == mGmatFunctionPaths.end())
   {
      #ifdef DEBUG_FUNCTION_PATH
      MessageInterface::ShowMessage
         ("   the pathname <%s> is new, so adding to %s\n", pathname.c_str(),
          addFront ? "front" : "back");
      #endif

      // if new pathname, add to front or back of the list
      if (addFront)
         mGmatFunctionPaths.push_front(pathname);
      else
         mGmatFunctionPaths.push_back(pathname);
   }
   else
   {
      // if existing pathname remove and add front or back of the list
      #ifdef DEBUG_FUNCTION_PATH
      MessageInterface::ShowMessage
         ("   the pathname <%s> already exists, so moving to %s\n", pathname.c_str(),
          addFront ? "front" : "back");
      #endif

      std::string oldPath = *pos;
      mGmatFunctionPaths.erase(pos);
      if (addFront)
         mGmatFunctionPaths.push_front(oldPath);
      else
         mGmatFunctionPaths.push_back(oldPath);
   }

   #ifdef DEBUG_FUNCTION_PATH
   pos = mGmatFunctionPaths.begin();
   while (pos != mGmatFunctionPaths.end())
   {
      MessageInterface::ShowMessage
         ("   mGmatFunctionPaths = %s\n", (*pos).c_str());
      ++pos;
   }
   #endif
}


//------------------------------------------------------------------------------
// std::string GetGmatFunctionPath(const std::string &funcName)
//------------------------------------------------------------------------------
/*
 * Returns the absolute path that has GmatFunction name.
 * It searches in the most recently added path first which is at the top of
 * the list.
 *
 * @param   funcName  Name of the GmatFunction to be located
 * @return  Path that has GmatFunction name
 */
//------------------------------------------------------------------------------
std::string FileManager::GetGmatFunctionPath(const std::string &funcName)
{
   return GetFunctionPath(GMAT_FUNCTION, mGmatFunctionPaths, funcName);
}


//------------------------------------------------------------------------------
// const StringArray& GetAllGmatFunctionPaths()
//------------------------------------------------------------------------------
const StringArray& FileManager::GetAllGmatFunctionPaths()
{
   mGmatFunctionFullPaths.clear();

   std::list<std::string>::iterator listpos = mGmatFunctionPaths.begin();
   while (listpos != mGmatFunctionPaths.end())
   {
      mGmatFunctionFullPaths.push_back(ConvertToAbsPath(*listpos));
      ++listpos;
   }

   return mGmatFunctionFullPaths;
}


//------------------------------------------------------------------------------
// void ClearMatlabFunctionPath()
//------------------------------------------------------------------------------
void FileManager::ClearMatlabFunctionPath()
{
   mMatlabFunctionPaths.clear();
}


//------------------------------------------------------------------------------
// void  AddMatlabFunctionPath(const std::string &pat, bool addFront)
//------------------------------------------------------------------------------
/*
 * If new path it adds to the MatlabFunction path list.
 * If path already exist, it moves to the front or back of the list, depends on
 * addFront flag.
 *
 * @param  path  path name to be added
 * @param  addFront  if set to true, it adds to the front, else adds to the back (true)
 */
//------------------------------------------------------------------------------
void FileManager::AddMatlabFunctionPath(const std::string &path, bool addFront)
{
   #ifdef DEBUG_FUNCTION_PATH
   MessageInterface::ShowMessage
      ("FileManager::AddMatlabFunctionPath() Adding %s to MatlabFunctionPath\n",
       path.c_str());
   #endif

   std::list<std::string>::iterator pos =
      find(mMatlabFunctionPaths.begin(), mMatlabFunctionPaths.end(), path);

   if (pos == mMatlabFunctionPaths.end())
   {
      // if new path, add to front or back of the list
      if (addFront)
         mMatlabFunctionPaths.push_front(path);
      else
         mMatlabFunctionPaths.push_back(path);
   }
   else
   {
      // if existing path remove and add front or back of the list
      std::string oldPath = *pos;
      mMatlabFunctionPaths.erase(pos);
      if (addFront)
         mMatlabFunctionPaths.push_front(oldPath);
      else
         mMatlabFunctionPaths.push_back(oldPath);
   }

   #ifdef DEBUG_FUNCTION_PATH
   pos = mMatlabFunctionPaths.begin();
   while (pos != mMatlabFunctionPaths.end())
   {
      MessageInterface::ShowMessage
         ("   mMatlabFunctionPaths=%s\n",(*pos).c_str());
      ++pos;
   }
   #endif
}


//------------------------------------------------------------------------------
// std::string GetMatlabFunctionPath(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Returns the absolute path that has MatlabFunction name.
 * It searches in the most recently added path first which is at the top of
 * the list.
 *
 * @param   funcName  Name of the MatlabFunction to be located
 * @return  Path that has MatlabFunction name
 */
//------------------------------------------------------------------------------
std::string FileManager::GetMatlabFunctionPath(const std::string &name)
{
   return GetFunctionPath(MATLAB_FUNCTION, mMatlabFunctionPaths, name);
}


//------------------------------------------------------------------------------
// const StringArray& GetAllMatlabFunctionPaths()
//------------------------------------------------------------------------------
const StringArray& FileManager::GetAllMatlabFunctionPaths()
{
   mMatlabFunctionFullPaths.clear();

   std::list<std::string>::iterator listpos = mMatlabFunctionPaths.begin();
   while (listpos != mMatlabFunctionPaths.end())
   {
      mMatlabFunctionFullPaths.push_back(ConvertToAbsPath(*listpos));
      ++listpos;
   }

   return mMatlabFunctionFullPaths;
}


//------------------------------------------------------------------------------
// const StringArray& GetPluginList()
//------------------------------------------------------------------------------
/**
 * Accesses the list of plug-in libraries parsed from the startup file.
 *
 * @return The list of plug-in libraries
 */
//------------------------------------------------------------------------------
const StringArray& FileManager::GetPluginList()
{
   return mPluginList;
}

//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// std::string GetFunctionPath(FunctionType type, const std::list<std::string> &pathList
//                             const std::string &funcName)
//------------------------------------------------------------------------------
/*
 * Searches proper function path list from the top and return first path found.
 *
 * @param  type  type of function (MATLAB_FUNCTION, GMAT_FUNCTION)
 * @param  pathList  function path list to use in search
 * @param  funcName  name of the function to search
 */
//------------------------------------------------------------------------------
std::string FileManager::GetFunctionPath(FunctionType type,
                                         std::list<std::string> &pathList,
                                         const std::string &funcName)
{
   #ifdef DEBUG_FUNCTION_PATH
   MessageInterface::ShowMessage
      ("FileManager::GetFunctionPath(%s) with type %d entered\n",
       funcName.c_str(), type);
   #endif

   std::string funcName1 = funcName;
   if (type == GMAT_FUNCTION)
   {
      if (funcName.find(".gmf") == funcName.npos)
         funcName1 = funcName1 + ".gmf";
   }
   else
   {
      if (funcName.find(".m") == funcName.npos)
         funcName1 = funcName1 + ".m";
   }

   // Search through pathList
   // The most recent path added to the last, so search backwards
   std::string pathName, fullPath;
   bool fileFound = false;

   // Search from the top of the list, which is the most recently added path
   // The search order goes from top to bottom. (loj: 2008.10.02)
   std::list<std::string>::iterator pos = pathList.begin();
   while (pos != pathList.end())
   {
      pathName = *pos;
      fullPath = ConvertToAbsPath(pathName) + funcName1;

      #ifdef DEBUG_FUNCTION_PATH
      MessageInterface::ShowMessage("   fullPath='%s'\n", fullPath.c_str());
      #endif

      if (GmatFileUtil::DoesFileExist(fullPath))
      {
         fileFound = true;
         break;
      }

      pos++;
   }

   if (fileFound)
      fullPath = GmatFileUtil::ParsePathName(fullPath);
   else
      fullPath = "";

   #ifdef DEBUG_FUNCTION_PATH
   MessageInterface::ShowMessage
      ("FileManager::GetFunctionPath(%s) returning '%s'\n", funcName.c_str(),
       fullPath.c_str());
   #endif

   return fullPath;

}


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
   #ifdef DEBUG_ADD_FILETYPE
   MessageInterface::ShowMessage
      ("FileManager::AddFileType() entered, type=%s, name=%s\n", type.c_str(), name.c_str());
   #endif

   if (type.find("_PATH") != type.npos)
   {
      std::string str2 = name;
      
      // append '/' if '\\' or '/' not there
      if (!GmatStringUtil::EndsWithPathSeparator(str2))
         str2 = str2 + mPathSeparator;
      
      mPathMap[type] = str2;

      #ifdef DEBUG_ADD_FILETYPE
      MessageInterface::ShowMessage
         ("   Adding %s = %s to mPathMap\n", type.c_str(), str2.c_str());
      #endif
      
      // Handle Gmat and Matlab Function path
      if (type == "GMAT_FUNCTION_PATH")
         AddGmatFunctionPath(str2, false);
      else if (type == "MATLAB_FUNCTION_PATH")
         AddMatlabFunctionPath(str2, false);

   }
   else if (type.find("_FILE_ABS") != type.npos)
   {
      mFileMap[type] = new FileInfo("", name);
   }
   else if (type.find("_FILE") != type.npos)
   {
      std::string pathName;
      std::string fileName;
      
      // file name
      std::string::size_type pos = name.find_last_of("/");
      if (pos == name.npos)
         pos = name.find_last_of("\\");
      
      if (pos != name.npos)
      {
         std::string pathName = name.substr(0, pos);
         std::string fileName = name.substr(pos+1, name.npos);
         mFileMap[type] = new FileInfo(pathName, fileName);
         
         #ifdef DEBUG_ADD_FILETYPE
         MessageInterface::ShowMessage
            ("   Adding %s and %s to mFileMap\n", pathName.c_str(), fileName.c_str());
         #endif
      }
      else
      {
         //loj: Should we add current path?
         std::string pathName = "CURRENT_PATH";
         mPathMap[pathName] = "./";
         std::string fileName = name;
         mFileMap[type] = new FileInfo(pathName, fileName);
         
         #ifdef DEBUG_ADD_FILETYPE
         MessageInterface::ShowMessage
            ("   Adding %s and %s to mFileMap\n", pathName.c_str(), fileName.c_str());
         MessageInterface::ShowMessage
            ("   'PATH/' not found in line:\n   %s = %s\n   So adding CURRENT_PATH = ./\n",
             type.c_str(), name.c_str());
         #endif
         
         //loj: Should we just throw an exception?
         //mInStream.close();
         //throw UtilityException
         //   ("FileManager::AddFileType() expecting 'PATH/' in line:\n" +
         //    type + " = " + name);
      }
   }
   else if (type == "PLUGIN")
   {
      #ifdef DEBUG_PLUGIN_DETECTION
         MessageInterface::ShowMessage("Adding plug-in %s to plugin list\n",
               name.c_str());
      #endif
      mPluginList.push_back(name);
   }
   else
   {
      throw UtilityException
         ("FileManager::AddFileType() file type should have '_PATH' or '_FILE'"
          " in:\n" + type);
   }
   
   #ifdef DEBUG_ADD_FILETYPE
   MessageInterface::ShowMessage("FileManager::AddFileType() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void AddAvailablePotentialFiles()
//------------------------------------------------------------------------------
void FileManager::AddAvailablePotentialFiles()
{
   // add available potential files

   // earth gravity files
   if (mFileMap.find("JGM2_FILE") == mFileMap.end())
      AddFileType("JGM2_FILE", "EARTH_POT_PATH/JGM2.cof");

   if (mFileMap.find("JGM3_FILE") == mFileMap.end())
      AddFileType("JGM3_FILE", "EARTH_POT_PATH/JGM3.cof");

   if (mFileMap.find("EGM96_FILE") == mFileMap.end())
      AddFileType("EGM96_FILE", "EARTH_POT_PATH/EGM96low.cof");

   // luna gravity files
   if (mFileMap.find("LP165P_FILE") == mFileMap.end())
      AddFileType("LP165P_FILE", "LUNA_POT_PATH/LP165P.cof");

   // venus gravity files
   if (mFileMap.find("MGNP180U_FILE") == mFileMap.end())
      AddFileType("MGNP180U_FILE", "VENUS_POT_PATH/MGNP180U.cof");

   // mars gravity files
   if (mFileMap.find("MARS50C_FILE") == mFileMap.end())
      AddFileType("MARS50C_FILE", "MARS_POT_PATH/Mars50c.cof");

}


//------------------------------------------------------------------------------
// void WriteHeader(std::ofstream &outStream)
//------------------------------------------------------------------------------
void FileManager::WriteHeader(std::ofstream &outStream)
{
   outStream << "#-------------------------------------------------------------------------------\n";
   outStream << "# General Mission Analysis Tool (GMAT) startup file\n";
   outStream << "#-------------------------------------------------------------------------------\n";
   outStream << "# Comment line starts with #\n";
   outStream << "# Comment line starting with ## will be saved when saving startup file.\n";
   outStream << "#\n";
   outStream << "# Path/File naming convention:\n";
   outStream << "#   - Path name should end with _PATH\n";
   outStream << "#   - File name should end with _FILE\n";
   outStream << "#   - Path/File names are case sensative\n";
   outStream << "#\n";
   outStream << "# You can add potential and texture files by following the naming convention.\n";
   outStream << "#   - Potential file should begin with planet name and end with _POT_FILE\n";
   outStream << "#   - Texture file should begin with planet name and end with _TEXTURE_FILE\n";
   outStream << "#\n";
   outStream << "# If same _FILE is specified multiple times, it will use the last one.\n";
   outStream << "#\n";
   outStream << "# You can have more than one line containing GMAT_FUNCTION_PATH. GMAT will store \n";
   outStream << "# the multiple paths you specify and scan for GMAT Functions using the paths \n";
   outStream << "# in top to bottom order and use the first function found from the search paths.\n";
   outStream << "#\n";
   outStream << "# In order for an object plugin to work inside GMAT, the plugin dynamic link libraries; \n";
   outStream << "# Windows(.dll), Linux(.so) and Mac(.dylib), must be placed in the folder containing\n";
   outStream << "# the GMAT executable or application. Once placed in the correct folder \n";
   outStream << "# the PLUGIN line below must be set equal to the plugin name without the dynamic link \n";
   outStream << "# library extension with the comment (#) removed from the front of the line.\n";
   outStream << "#\n";
   outStream << "# Some available PLUGINs are:\n";
   outStream << "# PLUGIN = libMatlabInterface\n";
   outStream << "# PLUGIN = libFminconOptimizer\n";
   outStream << "# PLUGIN = libVF13Optimizer\n";
   outStream << "# PLUGIN = libDataFile\n";
   outStream << "# PLUGIN = libCcsdsEphemerisFile\n";
   outStream << "# PLUGIN = libGmatEstimation\n";
   outStream << "#\n";
   outStream << "#===============================================================================\n";
}

//------------------------------------------------------------------------------
// void WriteFiles(std::ofstream &outStream, const std::string &type)
//------------------------------------------------------------------------------
void FileManager::WriteFiles(std::ofstream &outStream, const std::string &type)
{
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing %s file\n", type.c_str());
   #endif
   
   std::string realPath;
   
   // Write remainder of path
   if (type == "-OTHER-PATH-")
   {
      for (std::map<std::string, std::string>::iterator pos = mPathMap.begin();
           pos != mPathMap.end(); ++pos)
      {
         // if name not found in already written out list, then write
         if (find(mPathWrittenOuts.begin(), mPathWrittenOuts.end(), pos->first) ==
             mPathWrittenOuts.end())
         {
            if (pos->second != "")
            {
               realPath = pos->second;
               mPathWrittenOuts.push_back(pos->first);
               outStream << std::setw(22) << pos->first << " = "
                         << realPath << "\n";
            }
         }
      }
      return;
   }
   
   // Write remainder of files
   if (type == "-OTHER-")
   {
      for (std::map<std::string, FileInfo*>::iterator pos = mFileMap.begin();
           pos != mFileMap.end(); ++pos)
      {
         // if name not found in already written out list, then write
         if (find(mFileWrittenOuts.begin(), mFileWrittenOuts.end(), pos->first) ==
             mFileWrittenOuts.end())
         {
            if (pos->second)
            {
               realPath = pos->second->mPath;
               if (realPath == "CURRENT_PATH")
                  realPath = "";
               else
                  realPath = realPath + mPathSeparator;
               
               mFileWrittenOuts.push_back(pos->first);
               outStream << std::setw(22) << pos->first << " = "
                         << realPath << pos->second->mFile << "\n";
            }
         }
      }
      return;
   }
   
   for (std::map<std::string, FileInfo*>::iterator pos = mFileMap.begin();
        pos != mFileMap.end(); ++pos)
   {
      if (pos->first.find(type) != std::string::npos)
      {
         if (pos->second)
         {
            realPath = pos->second->mPath;
            if (realPath == "CURRENT_PATH")
               realPath = "";
            else
               realPath = realPath + mPathSeparator;
            
            mFileWrittenOuts.push_back(pos->first);
            outStream << std::setw(22) << pos->first << " = "
                      << realPath << pos->second->mFile << "\n";
         }
      }
   }
}


//------------------------------------------------------------------------------
// void RefreshFiles()
//------------------------------------------------------------------------------
void FileManager::RefreshFiles()
{
   mRunMode = "";
   mPlotMode = "";
   mMatlabMode = "";
   mDebugMatlab = "";
   mDebugMissionTree = "";
   mWriteParameterInfo = "";
   mWriteGmatKeyword = "";
   mPathMap.clear();
   mGmatFunctionPaths.clear();
   mMatlabFunctionPaths.clear();
   mGmatFunctionFullPaths.clear();
   mSavedComments.clear();
   mPluginList.clear();

   for (std::map<std::string, FileInfo*>::iterator iter = mFileMap.begin();
        iter != mFileMap.begin(); ++iter)
      delete iter->second;

   mFileMap.clear();
   
   //-------------------------------------------------------
   // add root and data path
   //-------------------------------------------------------
   AddFileType("ROOT_PATH", "../");
   AddFileType("DATA_PATH", "ROOT_PATH/data");
   
   //-------------------------------------------------------
   // add default output paths and files
   //-------------------------------------------------------
   std::string defOutPath = "../output";
   if (!DoesDirectoryExist(defOutPath))
      defOutPath = "./";
   
   AddFileType("OUTPUT_PATH", defOutPath);
   AddFileType("LOG_FILE", "OUTPUT_PATH/GmatLog.txt");
   AddFileType("REPORT_FILE", "OUTPUT_PATH/GmatReport.txt");
   AddFileType("MEASUREMENT_PATH", "OUTPUT_PATH");
   AddFileType("EPHEM_PATH", "OUTPUT_PATH");
   AddFileType("SCREENSHOT_FILE", "OUTPUT_PATH");
   AddFileType("EPHEM_PATH", "OUTPUT_PATH");
   
   
   // Should we add default input paths and files?
   // Yes, for now in case of startup file doesn't specify all the required
   // input path and files (LOJ: 2011.03.21)
#ifdef __FM_ADD_DEFAULT_INPUT__

   //-------------------------------------------------------
   // create default input paths and files
   //-------------------------------------------------------
   
   // de files
   AddFileType("DE_PATH", "DATA_PATH/planetary_ephem/de/");
   AddFileType("DE405_FILE", "DE_PATH/leDE1941.405");

   // spk files
   AddFileType("SPK_PATH", "DATA_PATH/planetary_ephem/spk/");
   AddFileType("PLANETARY_SPK_FILE", "SPK_PATH/de421.bsp");

   // earth gravity files
   AddFileType("EARTH_POT_PATH", "DATA_PATH/gravity/earth/");
   AddFileType("JGM2_FILE", "EARTH_POT_PATH/JGM2.cof");
   AddFileType("JGM3_FILE", "EARTH_POT_PATH/JGM3.cof");
   AddFileType("EGM96_FILE", "EARTH_POT_PATH/EGM96.cof");

   // luna gravity files
   AddFileType("LUNA_POT_PATH", "DATA_PATH/gravity/luna/");
   AddFileType("LP165P_FILE", "LUNA_POT_PATH/lp165p.cof");

   // venus gravity files
   AddFileType("VENUS_POT_PATH", "DATA_PATH/gravity/venus/");
   AddFileType("MGNP180U_FILE", "VENUS_POT_PATH/MGNP180U.cof");

   // mars gravity files
   AddFileType("MARS_POT_PATH", "DATA_PATH/gravity/mars/");
   AddFileType("MARS50C_FILE", "MARS_POT_PATH/Mars50c.cof");

   // planetary coeff. files
   AddFileType("PLANETARY_COEFF_PATH", "DATA_PATH/planetary_coeff/");
   AddFileType("EOP_FILE", "PLANETARY_COEFF_PATH/eopc04.62-now");
   // wcs 2013.03.04 PLANETARY_COEFF_FILE is CURRENTLY not used, since our
   // default is PLANETARY_1980 and PLANETARY_1996 is not allowed; however,
   // we will leave this here as a placeholder anyway
   AddFileType("PLANETARY_COEFF_FILE", "PLANETARY_COEFF_PATH/NUT85.DAT");
   AddFileType("NUTATION_COEFF_FILE", "PLANETARY_COEFF_PATH/NUTATION.DAT");

   // time path and files
   AddFileType("TIME_PATH", "DATA_PATH/time/");
   AddFileType("LEAP_SECS_FILE", "TIME_PATH/tai-utc.dat");
   AddFileType("LSK_FILE", "TIME_PATH/naif0010.tls");
   
   // gui config file path
   AddFileType("GUI_CONFIG_PATH", "DATA_PATH/gui_config/");
   
   // personalization file
   AddFileType("PERSONALIZATION_FILE", "OUTPUT_PATH/MyGmat.ini");
   
   // icon path and main icon file
   AddFileType("ICON_PATH", "DATA_PATH/graphics/icons/");
   
   #if defined __WXMSW__
   AddFileType("MAIN_ICON_FILE", "ICON_PATH/GMATWin32.ico");
   #elif defined __WXGTK__
   AddFileType("MAIN_ICON_FILE", "ICON_PATH/GMATLinux48.xpm");
   #elif defined __WXMAC__
   AddFileType("MAIN_ICON_FILE", "ICON_PATH/GMATIcon.icns");
   #endif

   // splash file path
   AddFileType("SPLASH_PATH", "DATA_PATH/graphics/splash/");
   AddFileType("SPLASH_FILE", "SPLASH_PATH/GMATSplashScreen.tif");
   
   // texture file path
   AddFileType("TEXTURE_PATH", "DATA_PATH/graphics/texture/");
   AddFileType("SUN_TEXTURE_FILE", "TEXTURE_PATH/Sun.jpg");
   AddFileType("MERCURY_TEXTURE_FILE", "TEXTURE_PATH/Mercury_JPLCaltech.jpg");
   AddFileType("EARTH_TEXTURE_FILE", "TEXTURE_PATH/ModifiedBlueMarble.jpg");
   AddFileType("MARS_TEXTURE_FILE", "TEXTURE_PATH/Mars_JPLCaltechUSGS.jpg");
   AddFileType("JUPITER_TEXTURE_FILE", "TEXTURE_PATH/Jupiter_HermesCelestiaMotherlode.jpg");
   AddFileType("SATRUN_TEXTURE_FILE", "TEXTURE_PATH/Saturn_gradiusCelestiaMotherlode.jpg");
   AddFileType("URANUS_TEXTURE_FILE", "TEXTURE_PATH/Uranus_JPLCaltech.jpg");
   AddFileType("NEPTUNE_TEXTURE_FILE", "TEXTURE_PATH/Neptune_BjornJonsson.jpg");
   AddFileType("PLUTO_TEXTURE_FILE", "TEXTURE_PATH/Pluto_JPLCaltech.jpg");
   AddFileType("LUNA_TEXTURE_FILE", "TEXTURE_PATH/Moon_HermesCelestiaMotherlode.jpg");
   
   // star path and files
   AddFileType("STAR_PATH", "DATA_PATH/graphics/stars/");
   AddFileType("STAR_FILE", "STAR_PATH/inp_StarCatalog.txt");
   AddFileType("CONSTELLATION_FILE", "STAR_PATH/inp_Constellation.txt");
   
   // models
   AddFileType("MODEL_PATH", "DATA_PATH/vehicle/models/");
   AddFileType("SPACECRAFT_MODEL_FILE", "MODEL_PATH/aura.3ds");
   
   // help file
   AddFileType("HELP_FILE", "");
   
#endif

}


//------------------------------------------------------------------------------
// void ShowMaps(const std::string &msg)
//------------------------------------------------------------------------------
void FileManager::ShowMaps(const std::string &msg)
{
   MessageInterface::ShowMessage("%s\n", msg.c_str());
   MessageInterface::ShowMessage("Here is path map, there are %d items\n", mPathMap.size());
   for (std::map<std::string, std::string>::iterator pos = mPathMap.begin();
        pos != mPathMap.end(); ++pos)
   {
      MessageInterface::ShowMessage("%20s: %s\n", (pos->first).c_str(), (pos->second).c_str());
   }
   
   MessageInterface::ShowMessage("Here is file map, there are %d items\n", mFileMap.size());
   for (std::map<std::string, FileInfo*>::iterator pos = mFileMap.begin();
        pos != mFileMap.end(); ++pos)
   {
      MessageInterface::ShowMessage
         ("%20s: %20s  %s\n", (pos->first).c_str(), (pos->second)->mPath.c_str(), (pos->second)->mFile.c_str());
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
   MessageInterface::SetLogEnable(false); // so that debug can be written from here

   #ifdef DEBUG_FILE_MANAGER
   MessageInterface::ShowMessage("FileManager::FileManager() entered\n");
   #endif

   mPathSeparator = GetPathSeparator();
   mStartupFileDir = GmatFileUtil::GetWorkingDirectory() + mPathSeparator;
   mStartupFileName = "gmat_startup_file.txt";
   GmatGlobal::Instance()->AddHiddenCommand("SaveMission");

   #ifdef DEBUG_STARTUP_FILE
   MessageInterface::ShowMessage
      ("FileManager::FileManager() entered, mPathSeparator='%s', "
       "mStartupFileDir='%s', mStartupFileName='%s'\n", mPathSeparator.c_str(),
       mStartupFileDir.c_str(), mStartupFileName.c_str());
   #endif

   RefreshFiles();

}


