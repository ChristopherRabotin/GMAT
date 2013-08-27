//$Id$
//------------------------------------------------------------------------------
//                                 FileUtil
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
// Created: 2005/12/12
//
/**
 * This file provides methods to compare two output files. The compare summary
 * is written to the log file.
 */
//------------------------------------------------------------------------------

#include "FileUtil.hpp"
#include "StringTokenizer.hpp"
#include "MessageInterface.hpp"
#include "TimeTypes.hpp"           // for GmatTimeUtil::
#include "RealUtilities.hpp"       // for Abs()
#include "StringUtil.hpp"          // for ToString()
#include "FileTypes.hpp"           // for GmatFile::MAX_PATH_LEN
#include "UtilityException.hpp"
#include <algorithm>               // for set_difference()
#include <iterator>                // for back_inserter() with VC++ 2010
#include <stdlib.h>
#include <sstream>                 // for std::stringstream

#ifndef _MSC_VER  // if not Microsoft Visual C++
#include <dirent.h>
#endif

#ifdef __WIN32__
#include <windows.h>
#include <direct.h>
#endif

#ifdef __MAC__
#include <mach-o/dyld.h>
#endif

#ifdef __linux__
#include <unistd.h>		// Needed for (linux) gcc 4.7 or later
#endif

using namespace GmatStringUtil;

//#define DEBUG_FIRST_10_LINES
//#define DEBUG_REAL_COLUMNS
//#define DBGLVL_COMPARE_FILES 3
//#define DBGLVL_FUNCTION_OUTPUT 2

//------------------------------------------------------------------------------
// std::string GetPathSeparator()
//------------------------------------------------------------------------------
/**
 * @return path separator; "/" or "\\" dependends on the platform
 */
//------------------------------------------------------------------------------
std::string GmatFileUtil::GetPathSeparator()
{
   std::string sep = "/";
   
   char *buffer;
   buffer = getenv("OS");
   if (buffer != NULL)
   {
      #ifdef DEBUG_FILE_UTIL
      MessageInterface::ShowMessage
         ("GmatFileUtil::GetPathSeparator() Current OS is %s\n", buffer);
      #endif
      
      std::string osStr(buffer);
      
      if (osStr.find("Windows") != osStr.npos)
         sep = "\\";
   }
   
   return sep;
}


//------------------------------------------------------------------------------
// std::string GetWorkingDirectory()
//------------------------------------------------------------------------------
/*
 * @return  The current working directory, generally the application path.
 */
//------------------------------------------------------------------------------
std::string GmatFileUtil::GetWorkingDirectory()
{
   std::string currDir;
   
#ifdef _MSC_VER  // if Microsoft Visual C++
   char buffer[GmatFile::MAX_PATH_LEN];
   
   //if (_getcwd(buffer, sizeof(buffer) / sizeof(TCHAR)))
   if (GetCurrentDirectory(GmatFile::MAX_PATH_LEN, buffer) > 0)
      currDir = buffer;
   else
      MessageInterface::ShowMessage
         ("*** WARNING *** GmatFileUtil::GetWorkingDirectory() \n"
          "Cannot get current directory, so jsut returning empty directory\n");
   
#else
   
   char buffer[GmatFile::MAX_PATH_LEN];
   // Intentionally get the return and then ignore it to move warning from
   // system libraries to GMAT code base.  The "unused variable" warning
   // here can be safely ignored.
   // char *ch = getcwd(buffer, GmatFile::MAX_PATH_LEN);
   // This clears a warning message
   if (getcwd(buffer, GmatFile::MAX_PATH_LEN) != buffer)
      ;
   currDir = buffer;
#endif
   
   return currDir;
}


//------------------------------------------------------------------------------
// bool SetWorkingDirectory(const std::string &newDir)
//------------------------------------------------------------------------------
/*
 * Sets current working directory to newDir
 *
 * @return  true if current working is successfully set to newDir, flase otherwise
 */
//------------------------------------------------------------------------------
bool GmatFileUtil::SetWorkingDirectory(const std::string &newDir)
{
#ifdef _MSC_VER  // if Microsoft Visual C++
   if (SetCurrentDirectory(newDir.c_str()) > 0)
      return true;
   else
      return false;
#else
   if (chdir(newDir.c_str()) == -1)
      return false;
   else
      return true;
#endif
}


//------------------------------------------------------------------------------
// std::string GetApplicationPath()
//------------------------------------------------------------------------------
/*
 * @return  The application full path.
 *
 */
//------------------------------------------------------------------------------
std::string GmatFileUtil::GetApplicationPath()
{
#if defined (__WIN32__) || defined(_MSC_VER)
   
   std::string appPath;
   char buffer[GmatFile::MAX_PATH_LEN];
   int bytes = GetModuleFileName(NULL, buffer, GmatFile::MAX_PATH_LEN);
   if (bytes > 0)
      appPath = buffer;
   
   return appPath;
   
#elif __linux__

   std::string appPath;
   char buffer[GmatFile::MAX_PATH_LEN];
   char szTmp[32];
   sprintf(szTmp, "/proc/%d/exe", getpid());
   int bytes = std::min(readlink(szTmp, buffer, GmatFile::MAX_PATH_LEN),
                   GmatFile::MAX_PATH_LEN - 1);
   if(bytes >= 0)
   {
      buffer[bytes] = '\0';
      appPath = buffer;
   }
   return appPath;

#elif __MAC__
   std::string appPath = "./";
   char path[GmatFile::MAX_PATH_LEN];
   char actualPath[GmatFile::MAX_PATH_LEN];
   uint32_t size = sizeof(path);
   if (_NSGetExecutablePath(path, &size) == 0)
   {
      char *res = NULL;
      res = realpath(path, actualPath);
      if (!res)
      {
          UtilityException("ERROR getting actualPath using realpath\n");
      }
//      std::string thePath(actualPath);
//      appPath = thePath;
      appPath = actualPath;
   }
   return appPath;

#else
   MessageInterface::ShowMessage
      ("**** GmatFileUtil::GetApplicationPath() reached for unknown platform, "
       "so just returning empty string\n");
   return "";
#endif
}


//------------------------------------------------------------------------------
// std::string ParseFirstPathName(const std::string &fullPath, bool appendSep = true)
//------------------------------------------------------------------------------
/*
 * This function parses first path name from given full path name.
 *
 * @param  fullPath  input full path name
 * @param  appendSep appends path separator if true
 * @return  The file name from the full path
 *
 */
//------------------------------------------------------------------------------
std::string GmatFileUtil::ParseFirstPathName(const std::string &fullPath,
                                             bool appendSep)
{
   #ifdef DEBUG_PARSE_FILENAME
   MessageInterface::ShowMessage
      ("GmatFileUtil::ParseFirstPathName() fullPath=<%s>\n", fullPath.c_str());
   #endif
   
   std::string filePath;
   std::string::size_type firstSlash = fullPath.find_first_of("/\\");
   
   if (firstSlash != filePath.npos)
   {
      if (appendSep)
         filePath = fullPath.substr(0, firstSlash + 1);
      else
         filePath = fullPath.substr(0, firstSlash);
   }
   
   #ifdef DEBUG_PARSE_FILENAME
   MessageInterface::ShowMessage
      ("GmatFileUtil::ParseFirstPathName() returning <%s>\n", filePath.c_str());
   #endif
   
   return filePath;
}


//------------------------------------------------------------------------------
// std::string ParsePathName(const std::string &fullPath, bool appendSep = true)
//------------------------------------------------------------------------------
/*
 * This function parses whole path name from given full path name.
 *
 * @param  fullPath  input full path name
 * @param  appendSep appends path separator if true
 * @return  The file name from the full path
 *
 */
//------------------------------------------------------------------------------
std::string GmatFileUtil::ParsePathName(const std::string &fullPath,
                                        bool appendSep)
{
   #ifdef DEBUG_PARSE_FILENAME
   MessageInterface::ShowMessage
      ("GmatFileUtil::ParsePathName() fullPath=<%s>\n", fullPath.c_str());
   #endif
   
   std::string filePath("");
   std::string thePathToUse = fullPath;
   #ifdef __MAC__
      std::string appString = ".app";
      std::string::size_type appLoc = fullPath.rfind(appString);
      if (appLoc != fullPath.npos)
      {
         thePathToUse = fullPath.substr(0,appLoc);
      }
  #endif
   
   std::string::size_type lastSlash = thePathToUse.find_last_of("/\\");

   if (lastSlash != thePathToUse.npos)
   {
      if (appendSep)
         filePath = thePathToUse.substr(0, lastSlash + 1);
      else
         filePath = thePathToUse.substr(0, lastSlash);
   }
   
   #ifdef DEBUG_PARSE_FILENAME
   MessageInterface::ShowMessage
      ("GmatFileUtil::ParsePathName() returning <%s>\n", filePath.c_str());
   #endif
   
   return filePath;
}


//------------------------------------------------------------------------------
// std::string ParseFileName(const std::string &fullPath, bool removeExt = false)
//------------------------------------------------------------------------------
/*
 * This function parses file name from given full path name.
 *
 * @param  fullPath  input full path name
 * @param  removeExt  Set this flag to true if file extension to be removed [false]
 * @return  The file name from the full path
 *
 */
//------------------------------------------------------------------------------
std::string GmatFileUtil::ParseFileName(const std::string &fullPath, bool removeExt)
{
   #ifdef DEBUG_PARSE_FILENAME
   MessageInterface::ShowMessage
      ("GmatFileUtil::ParseFileName() fullPath=<%s>\n", fullPath.c_str());
   #endif
   
   std::string fileName = fullPath;
   
   std::string::size_type lastSlash = fileName.find_last_of("/\\");
   if (lastSlash != fileName.npos)
      fileName = fileName.substr(lastSlash+1);
   
   #ifdef DEBUG_PARSE_FILENAME
   MessageInterface::ShowMessage
      ("GmatFileUtil::ParseFileName() returning <%s>\n", fileName.c_str());
   #endif
   
   if (removeExt)
   {
   #ifdef DEBUG_PARSE_FILENAM
   MessageInterface::ShowMessage
      ("GmatFileUtil::ParseFileName() removing extension\n");
   #endif
      std::string::size_type index1 = fileName.find_first_of(".");
      if (index1 != fileName.npos)
         fileName = fileName.substr(0, index1);
   }
   
   return fileName;
}


//------------------------------------------------------------------------------
// std::string ParseFileExtension(const std::string &fullPath, bool prependDot)
//------------------------------------------------------------------------------
/*
 * This function parses file extension (string after .) from given full path name.
 *
 * @param  fullPath  input full path name
 * @param  prependDot  prepends dot(.) if this is true [false]
 * @return  The file extension from the full path
 *
 */
//------------------------------------------------------------------------------
std::string GmatFileUtil::ParseFileExtension(const std::string &fullPath,
                                             bool prependDot)
{
   #ifdef DEBUG_PARSE_FILENAME
   MessageInterface::ShowMessage
      ("GmatFileUtil::ParseFileExtension() fullPath=<%s>\n", fullPath.c_str());
   #endif
   
   std::string fileExt;
   
   std::string::size_type lastDot = fullPath.find_last_of(".");
   if (lastDot != fullPath.npos)
   {
      if (lastDot < fullPath.size())
      {
         fileExt = fullPath.substr(lastDot+1);
         
         #ifdef DEBUG_PARSE_FILENAME
         MessageInterface::ShowMessage("   fileExt='%s'\n", fileExt.c_str());
         #endif
         
         // Check for path separator
         if (fileExt[0] == '/' || fileExt[0] == '\\')
            fileExt = "";
      }
   }
   
   if (fileExt != "" && prependDot)
      fileExt = "." + fileExt;
   
   #ifdef DEBUG_PARSE_FILENAME
   MessageInterface::ShowMessage
      ("GmatFileUtil::ParseFileExtension() returning <%s>\n", fileExt.c_str());
   #endif
   
   return fileExt;
}


//------------------------------------------------------------------------------
// std::string GetInvalidFileNameMessage(Integer option = 1)
//------------------------------------------------------------------------------
/**
 * Returns invalid file name message.
 */
//------------------------------------------------------------------------------
std::string GmatFileUtil::GetInvalidFileNameMessage(Integer option)
{
   std::string msg;
   
   if (option == 1)
      msg = "Maximum of 232 chars of non-blank name without containing any of "
         "the following characters: \\/:*?\"<>| ";
   else if (option == 2)
      msg = "A file name cannot be blank or contain any of the following characters:\n"
      "   \\/:*?\"<>|";
   
   return msg;
}


//------------------------------------------------------------------------------
// bool GMAT_API IsPathRelative(const std::string &fullPath)
//------------------------------------------------------------------------------
/**
 * Checks if given path name has relative path.
 *
 * @return true if input path has relative path, false otherwise
 */
//------------------------------------------------------------------------------
bool GmatFileUtil::IsPathRelative(const std::string &fullPath)
{
   if (fullPath.find("./") == 0 || fullPath.find(".\\") == 0 ||
       fullPath.find("../") == 0 || fullPath.find("..\\") == 0 )
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// bool GmatFileUtil::IsValidFileName(const std::string &fname, bool blankIsOk = true)
//------------------------------------------------------------------------------
bool GmatFileUtil::IsValidFileName(const std::string &fname, bool blankIsOk)
{
   if (fname == "")
   {
      if (blankIsOk)
         return true;
      else
         return false;
   }
   
   std::string filename = ParseFileName(fname);
   bool retval = false;
   
   // Check for invalid characters
   std::string invalidChars = "\\/:*?\"<>|";
   if (filename.find_first_of(invalidChars) == filename.npos)
      retval = true;
   else
      retval = false;
   
   // Check for name too long
   if (retval)
   {
      if ((Integer) filename.size() > GmatFile::MAX_FILE_LEN)
         retval = false;
   }
   
   return retval;
}


//------------------------------------------------------------------------------
// bool GmatFileUtil::IsSameFileName(const std::string &fname1, const std::string &fname2)
//------------------------------------------------------------------------------
/*
 * @return  true  If two file names are same, false otherwise
 */
//------------------------------------------------------------------------------
bool GmatFileUtil::IsSameFileName(const std::string &fname1, const std::string &fname2)
{
   if (fname1 == "" || fname2 == "")
      return false;

   std::string name1 = fname1;
   std::string name2 = fname2;
   
   // Replace \ with /
   name1 = GmatStringUtil::Replace(name1, "\\", "/");
   name2 = GmatStringUtil::Replace(name2, "\\", "/");
   if (name1 == name2)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// bool DoesDirectoryExist(const std::string &fullPath, bool blankIsOk = true)
//------------------------------------------------------------------------------
/*
 * @return  true  If directory exist, false otherwise
 */
//------------------------------------------------------------------------------
bool GmatFileUtil::DoesDirectoryExist(const std::string &fullPath, bool blankIsOk)
{
   #ifdef DEBUG_DIR_EXIST
   MessageInterface::ShowMessage
      ("GmatFileUtil::DoesDirectoryExist() entered, fullPath='%s'\n", fullPath.c_str());
   #endif
   
   if (fullPath == "")
   {
      if (blankIsOk)
         return true;
      else
         return false;
   }
   
   bool dirExist = false;
   std::string dirName = ParsePathName(fullPath, true);
   
   // empty dir name is OK.
   if (dirName == "")
      return true;
   
   #ifdef DEBUG_DIR_EXIST
   MessageInterface::ShowMessage("   dirName='%s'\n", dirName.c_str());
   #endif
   
#ifdef _MSC_VER  // if Microsoft Visual C++

   TCHAR currDir[GmatFile::MAX_PATH_LEN];
   // Save current directory
   DWORD ret = GetCurrentDirectory(GmatFile::MAX_PATH_LEN, currDir);
   // Try setting to requested direcotry
   dirExist = (SetCurrentDirectory(dirName.c_str()) == 0 ? false : true);
   // Set back to current directory
   SetCurrentDirectory(currDir);

#else
   
   DIR *dir = NULL;
   dir = opendir(dirName.c_str());
   
   if (dir != NULL)
   {
      dirExist = true; 
      closedir(dir);
   }
#endif
   
   #ifdef DEBUG_DIR_EXIST
   MessageInterface::ShowMessage
      ("GmatFileUtil::DoesDirectoryExist() returning %d\n", dirExist);
   #endif
   
   return dirExist;
}


//------------------------------------------------------------------------------
// bool DoesFileExist(const std::string &filename)
//------------------------------------------------------------------------------
bool GmatFileUtil::DoesFileExist(const std::string &filename)
{
   #ifdef DEBUG_FILE_CHECK
   MessageInterface::ShowMessage
      ("GmatFileUtil::DoesFileExist() filename=<%s>\n",
       filename.c_str());
   #endif
   
   FILE * pFile;
   pFile = fopen (filename.c_str(), "rt+");
   bool fileExist = false;
   if (pFile!=NULL)
   {
      fclose (pFile);
      fileExist = true;
   }
   
   #ifdef DEBUG_FILE_CHECK
   MessageInterface::ShowMessage
      ("GmatFileUtil::DoesFileExist() returning %d\n", fileExist);
   #endif
   
   return fileExist;
}


//------------------------------------------------------------------------------
// bool GetLine(std::istream *is, std::string &line)
//------------------------------------------------------------------------------
/*
 * Reads a platform independent line from the input stream.
 *
 * @param  is    The input stream pointer
 * @param  line  The line read from the input stream
 *
 * @return  true if a line from the input stream was read successfully.
 */
//------------------------------------------------------------------------------
bool GmatFileUtil::GetLine(std::istream *is, std::string &line)
{
   if (is == NULL)
      return false;
   
   char ch;
   std::string result;
   
   while (is->get(ch) && ch != '\r' && ch != '\n' && ch != '\0' &&
          !is->eof()) 
      result += ch;
   
   line = result;
   return true;
}

//#define DEBUG_APP_INSTALLATION
//------------------------------------------------------------------------------
// bool IsAppInstalled(const std::string &appName)
//------------------------------------------------------------------------------
/**
 * Asks system if requested application is installed
 *
 * @param  appName  Name of the application, such as MATLAB
 * @return  true requested application is installed on the system
 * @note GMAT currently checks for only MATLAB installation
 */
//------------------------------------------------------------------------------
bool GmatFileUtil::IsAppInstalled(const std::string &appName, std::string &appLoc)
{
#ifdef __WIN32__
   #ifdef DEBUG_APP_INSTALLATION
   MessageInterface::ShowMessage
      ("GmatFileUtil::IsAppInstalled() entered, appName='%s'\n", appName.c_str());
   #endif
   
   if (appName != "MATLAB")
   {
      MessageInterface::ShowMessage
         ("GMAT currently checks for only MATLAB installation\n");
      return false;
   }
   
   long lRet;
   HKEY hKey;
   char temp[150];
   DWORD dwBufLen;
   HKEY tree = HKEY_LOCAL_MACHINE;

   // @todo
   // Should we check other versions by querying sub keys?
   // See RegEnumKeyEx(), RegEnumValue() example in
   // http://msdn.microsoft.com/en-us/library/ms724256%28VS.85%29.aspx
   //std::string ver75 = "7.5"; // 2007b
   std::string ver79 = "7.9"; // 2009b
   
   std::string matlabFolder = "Software\\MathWorks\\MATLAB\\";

   std::string folder = matlabFolder + ver79;
   std::string key = "MATLABROOT";
   
   #ifdef DEBUG_APP_INSTALLATION
   MessageInterface::ShowMessage("   About to open '%s'\n", folder.c_str());
   #endif
   
   // Open location
   lRet = RegOpenKeyEx(tree, folder.c_str(), 0, KEY_QUERY_VALUE, &hKey);
   if (lRet != ERROR_SUCCESS)
   {
      #ifdef DEBUG_APP_INSTALLATION
      MessageInterface::ShowMessage
         ("   Failed on RegOpenKeyEx(), return code is %ld\n", lRet);
      #endif
      
      return false;
   }
   else
   {
      #ifdef DEBUG_APP_INSTALLATION
      MessageInterface::ShowMessage("   hKey is %ld\n", hKey);
      #endif
   }
   
   // Get key
   dwBufLen = sizeof(temp);
   lRet = RegQueryValueEx( hKey, key.c_str(), NULL, NULL, (BYTE*)&temp, &dwBufLen );
   if (lRet != ERROR_SUCCESS)
   {
      #ifdef DEBUG_APP_INSTALLATION
      MessageInterface::ShowMessage
         ("   Failed on RegQueryValueEx(), rReturn code is %ld\n", lRet);
      #endif
      return false;
   }
   
   #ifdef DEBUG_APP_INSTALLATION
   MessageInterface::ShowMessage("   Key value: %s\n", temp);
   #endif
   
   // Close key
   lRet = RegCloseKey( hKey );
   if (lRet != ERROR_SUCCESS)
   {
      #ifdef DEBUG_APP_INSTALLATION
      MessageInterface::ShowMessage
         ("   Failed on RegCloseKey(), return code is %ld\n", lRet);
      #endif
      return false;
   }
   
   appLoc = temp;
   
   // Got this far, then key exists
   #ifdef DEBUG_APP_INSTALLATION
   MessageInterface::ShowMessage("GmatFileUtil::IsAppInstalled() returning true\n");
   #endif
   return true;
   
#else // other operating system
   return true;
#endif
}

//------------------------------------------------------------------------------
// WrapperTypeArray GetFunctionOutputTypes(std::istream *inStream,
//                     const StringArray &outputs, std::string &errMsg,
//                     IntegerArray &outputRows, IntegerArray &outputCols)
//------------------------------------------------------------------------------
/*
 * Retrives function output information from the input stream, keeping the
 * the order of outputs.
 *
 * @param  inStream  the input function stream
 * @param  inputs    the input name list
 * @param  outputs   the output name list
 * @param  errMsg    the error message to be set if any
 * @param  outputRows  the array of row count to be set
 * @param  outputRows  the array of column count to be set
 *
 * @return  return  the wrapper type array of outputs
 */
//------------------------------------------------------------------------------
WrapperTypeArray
GmatFileUtil::GetFunctionOutputTypes(std::istream *inStream, const StringArray &inputs,
                                     const StringArray &outputs, std::string &errMsg,
                                     IntegerArray &outputRows, IntegerArray &outputCols)
{
   UnsignedInt outputSize = outputs.size();
   
   #if DBGLVL_FUNCTION_OUTPUT
   MessageInterface::ShowMessage
      ("GmatFileUtil::GetFunctionOutputTypes() inputSize = %d, outputSize = %d\n",
       inputs.size(), outputSize);
   for (UnsignedInt i=0; i<inputs.size(); i++)
      MessageInterface::ShowMessage("   inputs[%d]='%s'\n", i, inputs[i].c_str());
   for (UnsignedInt i=0; i<outputs.size(); i++)
      MessageInterface::ShowMessage("   outputs[%d]='%s'\n", i, outputs[i].c_str());
   #endif
   
   WrapperTypeArray outputWrapperTypes;
   std::string line;
   StringArray outputTypes, outputNames, outputDefs, multiples, globals;
   errMsg = "";
   std::string errMsg1, errMsg2;
   std::string name;
   Integer row, col;
   
   // if no output, just return
   if (outputSize == 0)
      return outputWrapperTypes;
   
   // check for duplicate output names
   for (UnsignedInt i=0; i<outputSize; i++)
   {
      for (UnsignedInt j=0; j<outputSize; j++)
      {
         if (i == j)
            continue;
         
         if (outputs[i] == outputs[j])
            if (find(multiples.begin(), multiples.end(), outputs[i]) == multiples.end())
               multiples.push_back(outputs[i]);
      }
   }
   
   if (multiples.size() > 0)
   {
      errMsg = "Duplicate output of";
      
      for (UnsignedInt i=0; i<multiples.size(); i++)
         errMsg = errMsg + " \"" + multiples[i] + "\"";
      return outputWrapperTypes;
   }
   
   
   // Initialize arrays to be used
   for (UnsignedInt i=0; i<outputSize; i++)
   {
      outputTypes.push_back("");;
      outputNames.push_back("");;
      outputDefs.push_back("");;
   }
   
   // Go through each line in the function file, ignoring after % inline comment
   while (!inStream->eof())
   {
      if (GetLine(inStream, line))
      {
         // remove inline comments and trim
         line = GmatStringUtil::RemoveInlineComment(line, "%");         
         line = GmatStringUtil::Trim(line, GmatStringUtil::BOTH, true, true);
         
         // Skip empty line or comment line
         if (line[0] == '\0' || line[0] == '%')
            continue;
         
         StringArray parts = GmatStringUtil::SeparateBy(line, " ,", true);
         
         if (parts[0] == "Global")
         {
            #if DBGLVL_FUNCTION_OUTPUT > 1
            for (UnsignedInt i=0; i<parts.size(); i++)
               MessageInterface::ShowMessage("   parts[%d]='%s'\n", i, parts[i].c_str());
            #endif
            
            for (UnsignedInt j=1; j<parts.size(); j++)
               globals.push_back(parts[j]);
            
         }
         else if (parts[0] == "Create")
         {
            #if DBGLVL_FUNCTION_OUTPUT > 1
            for (UnsignedInt i=0; i<parts.size(); i++)
               MessageInterface::ShowMessage("   parts[%d]='%s'\n", i, parts[i].c_str());
            #endif
         
            for (UnsignedInt i=0; i<outputSize; i++)
            {
               for (UnsignedInt j=2; j<parts.size(); j++)
               {
                  GmatStringUtil::GetArrayIndex(parts[j], row, col, name, "[]");
                  
                  if (name == outputs[i])
                  {
                     // add multiple output defs
                     if (find(outputNames.begin(), outputNames.end(), name) != outputNames.end())
                        multiples.push_back(name);
                     
                     outputNames[i] = name;
                     outputTypes[i] = parts[1];
                     outputDefs[i] = parts[j];
                     
                     #if DBGLVL_FUNCTION_OUTPUT > 1
                     MessageInterface::ShowMessage
                        ("   i=%d, type='%s', name='%s', def='%s'\n", i, parts[1].c_str(),
                         name.c_str(), parts[j].c_str());
                     #endif                  
                  }
               }
            }
         }
      }
      else
      {
         errMsg = "Encountered an error reading a file";
         return outputWrapperTypes;
      }
   }
   
   // find missing output definition
   StringArray missing;
   set_difference(outputs.begin(), outputs.end(),
                  outputNames.begin(), outputNames.end(), back_inserter(missing));
   
   #if DBGLVL_FUNCTION_OUTPUT
   MessageInterface::ShowMessage
      ("   missing.size()=%d, multiples.size()=%d, globals.size()=%d\n",
       missing.size(), multiples.size(), globals.size());
   for (UnsignedInt i=0; i<globals.size(); i++)
      MessageInterface::ShowMessage("   globals[%d] = '%s'\n", i, globals[i].c_str());
   #endif
   
   if (missing.size() == 0 && multiples.size() == 0)
   {
      // if all output found, figure out the output wrapper types
      for (UnsignedInt i=0; i<outputSize; i++)
      {
         if (outputTypes[i] == "Variable")
         {
            outputWrapperTypes.push_back(Gmat::VARIABLE_WT);
            outputRows.push_back(-1);
            outputCols.push_back(-1);
         }
         else if (outputTypes[i] == "Array")
         {
            GmatStringUtil::GetArrayIndex(outputDefs[i], row, col, name, "[]");
            
            #if DBGLVL_FUNCTION_OUTPUT > 1
            MessageInterface::ShowMessage
               ("   name='%s', row=%d, col=%d\n", name.c_str(), row, col);
            #endif
            
            outputWrapperTypes.push_back(Gmat::ARRAY_WT);
            outputRows.push_back(row);
            outputCols.push_back(col);
         }
      }
   }
   else
   {
      if (missing.size() > 0)
      {
         StringArray reallyMissing;
         for (UnsignedInt i=0; i<missing.size(); i++)
         {
            // Check if missing output declarations are in the input names or
            // globals. If output names are not in the inputs or globals, it is an
            // error condition as in the GMAT Function requirements 1.6, 1.7, 1.8
            if (find(inputs.begin(), inputs.end(), missing[i]) == inputs.end() &&
                find(globals.begin(), globals.end(), missing[i]) == globals.end())
               reallyMissing.push_back(missing[i]);
         }
         
         if (reallyMissing.size() > 0)
         {
            errMsg1 = "Missing output declaration of";
            for (UnsignedInt i=0; i<reallyMissing.size(); i++)
               errMsg1 = errMsg1 + " \"" + reallyMissing[i] + "\"";
         }
      }
      
      if (multiples.size() > 0)
      {
         for (UnsignedInt i=0; i<multiples.size(); i++)
            if (multiples[i] != "")
               errMsg2 = errMsg2 + " \"" + multiples[i] + "\"";
         
         if (errMsg2 != "")
         {
            if (errMsg1 == "")
               errMsg2 = "Multiple declaration of" + errMsg2;
            else
               errMsg2 = " and multiple declaration of" + errMsg2;
         }
      }
      
      errMsg = errMsg1 + errMsg2;
   }
   
   
   #if DBGLVL_FUNCTION_OUTPUT
   MessageInterface::ShowMessage
      ("GmatFileUtil::GetFunctionOutputTypes() returning %d outputWrapperTypes\n",
       outputWrapperTypes.size());
   for (UnsignedInt i=0; i<outputWrapperTypes.size(); i++)
      MessageInterface::ShowMessage
         ("   i=%d, outputWrapperTypes=%d, outputRows=%d, outputCols=%d\n", i,
          outputWrapperTypes[i], outputRows[i], outputCols[i]);
   #endif
   
   return outputWrapperTypes;
}


//------------------------------------------------------------------------------
// StringArray GetFileListFromDirectory(const std::string &dirName, bool addPath)
//------------------------------------------------------------------------------
/*
 * Get list of files from a directory.
 * The input directory should include directory namd and file spec.
 * For example, c:\MyDir\*.txt, c:\MyFunctions\*.gmf
 *
 * @param  dirName  Directory name with file specification
 * @param  addPath  If true, it prepends path to file name (false)
 * @return  String array containing file names in the directory
 */
//------------------------------------------------------------------------------
StringArray GmatFileUtil::GetFileListFromDirectory(const std::string &dirName,
                                                   bool addPath)
{
   #ifdef DEBUG_FILELIST
   MessageInterface::ShowMessage
      ("GmatFileUtil::GetFileListFromDirectory() dirName=<%s>\n",
       dirName.c_str());
   #endif
   
   std::string pathName = ParsePathName(dirName);
   std::string fileExt = ParseFileExtension(dirName);
   std::string outFile;
   StringArray fileList;
   
   MessageInterface::ShowMessage
      ("==> pathName=<%s>, fileExt=<%s>\n", pathName.c_str(), fileExt.c_str());
   
   //-----------------------------------------------------------------
   // for windows
   //-----------------------------------------------------------------
   #ifdef __WIN32__
   
   HANDLE hFind;
   WIN32_FIND_DATA findData;
   int errorCode;
   bool hasError = false;
   
   hFind = FindFirstFile(dirName.c_str(), &findData);
   
   if(hFind == INVALID_HANDLE_VALUE)
   {
      errorCode = GetLastError();
      if (errorCode == ERROR_FILE_NOT_FOUND)
      {
         MessageInterface::ShowMessage
            ("**** ERROR **** GmatFileUtil::GetFileListFromDirectory() \n"
             "There are no directory matching \"%s\"\n", dirName.c_str());
      }
      else
      {
         MessageInterface::ShowMessage
            ("**** ERROR **** GmatFileUtil::GetFileListFromDirectory() \n"
             "FindFirstFile() returned error code %d\n", errorCode);
      }
      hasError = true;
   }
   else
   {
      outFile = findData.cFileName;

      // add if the file matches exact file extension (i.e, no backup files allowed)
      if (ParseFileExtension(outFile) == fileExt)
      {
         if (addPath)
            outFile = pathName + findData.cFileName;
         fileList.push_back(outFile);
         
         #ifdef DEBUG_FILELIST
         MessageInterface::ShowMessage
            ("   > added %s to file list\n", findData.cFileName);
         #endif
      }
   }
   
   if (!hasError)
   {
      while (FindNextFile(hFind, &findData))
      {
         outFile = findData.cFileName;
         if (ParseFileExtension(outFile) == fileExt)
         {
            if (addPath)
               outFile = pathName + findData.cFileName;
            fileList.push_back(outFile);
            
            #ifdef DEBUG_FILELIST
            MessageInterface::ShowMessage
               ("   > added %s to file list\n", findData.cFileName);
            #endif
         }
      }
      
      errorCode = GetLastError();
      
      if (errorCode != ERROR_NO_MORE_FILES)
      {
         MessageInterface::ShowMessage
            ("**** ERROR **** GmatFileUtil::GetFileListFromDirectory() \n"
             "FindNextFile() returned error code %d\n", errorCode);
      }
      
      if (!FindClose(hFind))
      {
         errorCode = GetLastError();
         MessageInterface::ShowMessage
            ("**** ERROR **** GmatFileUtil::GetFileListFromDirectory() \n"
             "FindClose() returned error code %d\n", errorCode);
      }
   }
   #else
   // add other operating system here
   #endif
   
   #ifdef DEBUG_FILELIST
   MessageInterface::ShowMessage
      ("GmatFileUtil::GetFileListFromDirectory() returning %d files\n",
       fileList.size());
   #endif
   
   return fileList;
}


//------------------------------------------------------------------------------
// StringArray GetTextLines(const std::string &fileName)
//------------------------------------------------------------------------------
/*
 * Reads a text file and returns an array of string.
 *
 * @param  dirName  Directory name
 * @return  String array containing file names in the directory
 */
//------------------------------------------------------------------------------
StringArray GmatFileUtil::GetTextLines(const std::string &fileName)
{
   StringArray lines;
   
   // check if file exist
   std::ifstream inFile(fileName.c_str());
   
   if (!(inFile))
   {
      MessageInterface::ShowMessage
         ("**** ERROR **** GmatFileUtil::GetTextLines() \n"
          "The file \"" + fileName + "\" does not exist\n");
      return lines;
   }
   
   std::string oneLine;
   
   while (!inFile.eof())
   {
      inFile >> oneLine;
      lines.push_back(oneLine);
   }
   
   inFile.close();
   return lines;
}


//------------------------------------------------------------------------------
// bool GmatFileUtil::PrepareCompare(std::ifstream &baseIn, std::ifstream &in1,
//                                   std::ifstream &in2, std::ifstream &in3)
//------------------------------------------------------------------------------
/**
 * Opens files for comparison. If the same file extention not found for in1, in2,
 * and in3, it will try open with extension .truth.
 *
 * @return  returns true if all files open successfully.
 */
//------------------------------------------------------------------------------
bool GmatFileUtil::PrepareCompare(Integer numDirsToCompare,
                                  const std::string &basefilename,
                                  const std::string &filename1,
                                  const std::string &filename2,
                                  const std::string &filename3,
                                  std::ifstream &baseIn, std::ifstream &in1,
                                  std::ifstream &in2, std::ifstream &in3)
{
   textBuffer.clear();
   textBuffer.push_back("\n======================================== Compare Utility\n");
   textBuffer.push_back("basefile =" + basefilename + "\n");
   
   textBuffer.push_back("filename1=" + filename1 + "\n");
   
   if (numDirsToCompare >= 2)
      textBuffer.push_back("filename2=" + filename2 + "\n");
   
   if (numDirsToCompare >= 3)
      textBuffer.push_back("filename3=" + filename3 + "\n");
   
   #if DBGLVL_COMPARE_FILES
   MessageInterface::ShowMessage("\n======================================== PrepareCompare() entered\n");
   MessageInterface::ShowMessage("numDirsToCompare=%d\n", numDirsToCompare);
   MessageInterface::ShowMessage("basefile =%s\n", basefilename.c_str());
   MessageInterface::ShowMessage("filename1=%s\nfilename2=%s\nfilename3=%s\n",
                                 filename1.c_str(), filename2.c_str(), filename3.c_str());
   #endif
   
   baseIn.open(basefilename.c_str());
   in1.open(filename1.c_str());
   in2.open(filename2.c_str());
   in3.open(filename3.c_str());
   std::string newfilename1, newfilename2, newfilename3;
   
   if (!in1 && filename1 != "")
   {
      newfilename1 = filename1.substr(0, filename1.find("."));
      newfilename1 = newfilename1 + ".truth";
      in1.open(newfilename1.c_str());
      if (in1)
         textBuffer.push_back("new filename1=" + newfilename1 + "\n");
   }
   else
      newfilename1 = filename1;
   
   if (!in2 && filename2 != "")
   {
      newfilename2 = filename2.substr(0, filename2.find("."));
      newfilename2 = newfilename2 + ".truth";
      in2.open(newfilename2.c_str());
      if (in2)
         textBuffer.push_back("new filename2=" + newfilename2 + "\n");
   }
   else
      newfilename2 = filename2;
   
   if (!in3 && filename3 != "")
   {
      newfilename3 = filename3.substr(0, filename3.find("."));
      newfilename3 = newfilename3 + ".truth";
      in3.open(newfilename3.c_str());
      if (in3)
         textBuffer.push_back("new filename3=" + newfilename3 + "\n");
   }
   else
      newfilename3 = filename3;
   
   if (!baseIn)
   {
      textBuffer.push_back("Cannot open base file: " +  basefilename + "\n");
      return false;
   }
   
   // Check if base file is ascii file
   if (!IsAsciiFile(baseIn, basefilename))
   {
      textBuffer.push_back("Base file: " +  basefilename + " is not an ascii file\n");
      return false;
   }
   
   if (!in1)
   {
      textBuffer.push_back("Cannot open first file: " + newfilename1 + "\n");
      return false;
   }
   
   // Check if file1 is an ascii file
   if (!IsAsciiFile(in1, newfilename1))
   {
      textBuffer.push_back("First file: " +  newfilename1 + " is not an ascii file\n");
      return false;
   }
   
   if (numDirsToCompare >= 2)
   {
      if (!in2)
      {
         textBuffer.push_back("Cannot open second file: " + newfilename2 + "\n");
         return false;
      }
      // Check if file2 is an ascii file
      if (!IsAsciiFile(in2, newfilename2))
      {
         textBuffer.push_back("Second file: " +  newfilename2 + " is not an ascii file\n");
         return false;
      }
   }
   
   if (numDirsToCompare >= 3)
   {
      if (!in3)
      {
         textBuffer.push_back("Cannot open third file: " + newfilename3 + "\n");
         return false;
      }
      // Check if file3 is an ascii file
      if (!IsAsciiFile(in3, newfilename3))
      {
         textBuffer.push_back("Third file: " +  newfilename3 + " is not an ascii file\n");
         return false;
      }
   }
   
   #if DBGLVL_COMPARE_FILES
   MessageInterface::ShowMessage("PrepareCompare() returning true\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool CompareLines(const std::string &line1, const std::string &line2, ...)
//------------------------------------------------------------------------------
/**
 * Compares numeric values in lines. It skips strings embeded in the lines.
 *
 * @return true if all items are numerically same within tolerance
 *         false if number of items between two lines are different or
 *               if all items are not numerically same within tolerance
 */
//------------------------------------------------------------------------------
bool GmatFileUtil::CompareLines(const std::string &line1, const std::string &line2,
                                Real &diff, Real tol)
{
   #if DBGLVL_COMPARE_FILES > 1
   MessageInterface::ShowMessage
      ("GmatFileUtil::CompareLines() entered\n   line1='%s'\n   line2='%s', tol=%f\n",
       line1.c_str(), line2.c_str(), tol);
   #endif
   
   // Remove inline comment (get string upto % sign)
   std::string newline1 = GmatStringUtil::RemoveInlineComment(line1, "%");
   std::string newline2 = GmatStringUtil::RemoveInlineComment(line2, "%");
   StringArray items1 = GmatStringUtil::SeparateBy(newline1, " ,:\t", true);
   StringArray items2 = GmatStringUtil::SeparateBy(newline2, " ,:\t", true);
   std::string item1, item2;
   Integer size1 = items1.size();
   Integer size2 = items2.size();
   Real real1, real2;
   Real prevDiff = 0.0;
   diff = 999.999;
   
   if (size1 != size2)
   {
      #if DBGLVL_COMPARE_FILES > 1
      MessageInterface::ShowMessage
         ("GmatFileUtil::CompareLines() returning false, it has different number of items\n");
      #endif
      return false;
   }
   
   for (Integer i = 0; i < size1; i++)
   {
      item1 = items1[i];
      item2 = items2[i];
      
      // Compare if item is a real number
      if (GmatStringUtil::ToReal(item1, real1) && GmatStringUtil::ToReal(item2, real2))
      {
         diff = real1 - real2;
         if (GmatMathUtil::Abs(diff) > tol)
            return false;
         else
         {
            if (diff > prevDiff)
               prevDiff = diff;
         }
      }
   }
   
   diff = prevDiff;
   
   #if DBGLVL_COMPARE_FILES > 1
   MessageInterface::ShowMessage
      ("GmatFileUtil::CompareLines() returning true, diff=%f\n", diff);
   #endif
   return true;
}


//------------------------------------------------------------------------------
// StringArray& CompareNumericColumns(Integer numDirsToCompare, const std::string &basefilename,
//                      const std::string &filename1, const std::string &filename2,
//                      const std::string &filename3, Real tol = COMPARE_TOLERANCE);
//------------------------------------------------------------------------------
/**
 * Numerically compares base file with up to three other files. It will compare
 * the smallest number of columns among compare files.
 */
//------------------------------------------------------------------------------
StringArray& GmatFileUtil::CompareNumericColumns(Integer numDirsToCompare,
                                   const std::string &basefilename,
                                   const std::string &filename1, const std::string &filename2,
                                   const std::string &filename3, Real tol)
{
   #if DBGLVL_COMPARE_FILES > 0
   MessageInterface::ShowMessage
      ("GmatFileUtil::CompareNumericColumns() entered, numDirsToCompare=%d, tol=%f\n"
       "   basefilename='%s'\n   filename1='%s'\n   filename2='%s'\n   filename3='%s'\n",
       numDirsToCompare, tol, basefilename.c_str(), filename1.c_str(), filename2.c_str(),
       filename3.c_str());
   #endif
   
   std::ifstream baseIn;
   std::ifstream in1;
   std::ifstream in2;
   std::ifstream in3;
   
   bool ok = PrepareCompare(numDirsToCompare, basefilename, filename1,
                            filename2, filename3, baseIn, in1, in2, in3);
   
   if (!ok)
      return textBuffer;
   
   char buffer[BUFFER_SIZE];
   Real baseItem, item, diff;
   std::string line;
   int lineCount = 1;
   int baseCols = 99, file1Cols = 99, file2Cols = 99, file3Cols = 99;
   StringTokenizer stk;
   RealArray baseRealArray, file1RealArray, file2RealArray, file3RealArray;
   
   //------------------------------------------
   // if files have header lines, skip
   //------------------------------------------
   if (!SkipHeaderLines(baseIn, baseRealArray, basefilename))
   {
      textBuffer.push_back("***Cannot compare files: Data record not found in the base file.\n");
      return textBuffer;
   }
   
   if (!SkipHeaderLines(in1, file1RealArray, filename1))
   {
      textBuffer.push_back("***Cannot compare files: Data record not found in the file 1.\n");
      return textBuffer;
   }
   
   if (numDirsToCompare >= 2)
      if (!SkipHeaderLines(in2, file2RealArray, filename2))
      {
         textBuffer.push_back("***Cannot compare files: Data record not found in the file 2.\n");
         return textBuffer;
      }
   
   if (numDirsToCompare == 3)
      if (!SkipHeaderLines(in3, file3RealArray, filename3))
      {
         textBuffer.push_back("***Cannot compare files: Data record not found in the file 3.\n");
         return textBuffer;
      }
   
   //------------------------------------------
   // Check number of columns
   //------------------------------------------
   baseCols = baseRealArray.size();
   file1Cols = file1RealArray.size();
   
   if (numDirsToCompare >= 2)
      file2Cols = file2RealArray.size();
   
   if (numDirsToCompare == 3)
      file3Cols = file3RealArray.size();
   
   Integer numCols = baseCols < file1Cols ? baseCols : file1Cols;
   
   if (numDirsToCompare >= 2)
      numCols = numCols < file2Cols ? numCols : file2Cols;
   
   if (numDirsToCompare == 3)
      numCols = numCols < file3Cols ? numCols : file3Cols;
   
   #if DBGLVL_COMPARE_FILES > 0
   MessageInterface::ShowMessage
      ("baseCols=%d, file1Cols=%d, file2Cols=%d, file3Cols=%d\n",
       baseCols, file1Cols, file2Cols, file3Cols);
   MessageInterface::ShowMessage("Minimum number of columns found : %d\n", numCols);
   #endif
   
   bool numColsDifferent = false;
   
   if (baseCols != file1Cols)
      numColsDifferent = true;
   else if (file2Cols != 99 && baseCols != file2Cols)
      numColsDifferent = true;
   else if (file3Cols != 99 && baseCols != file3Cols)
      numColsDifferent = true;
   
   if (numColsDifferent)
   {
      textBuffer.push_back("*** Number of colmuns are different.\n");
      textBuffer.push_back("basefile: " + ToString(baseCols) + ", file1: " + ToString(file1Cols));
      if (file2Cols != 99)
         textBuffer.push_back(", file2: " + ToString(file2Cols,1));
      if (file3Cols != 99)
         textBuffer.push_back(", file3: " + ToString(file3Cols,1));
      
      textBuffer.push_back
         ("\nIt will compare up to " + ToString(numCols,1) + " columns.\n");
   }
   
   // Compare first data line
   RealArray maxDiffs1, maxDiffs2, maxDiffs3;
   
   #if DBGLVL_COMPARE_FILES > 1
   MessageInterface::ShowMessage("============================== line #: %d\n", lineCount);
   #endif
   
   
   for (int i=0; i<numCols; i++)
   {
      baseItem = baseRealArray[i];
      item = file1RealArray[i];
      
      diff = GmatMathUtil::Abs(item - baseItem);
      maxDiffs1.push_back(diff);
      
      if (numDirsToCompare >= 2)
      {
         item = file2RealArray[i];
         diff = GmatMathUtil::Abs(item - baseItem);
         maxDiffs2.push_back(diff);
      }
      
      if (numDirsToCompare == 3)
      {
         item = file3RealArray[i];
         diff = GmatMathUtil::Abs(item - baseItem);
         maxDiffs3.push_back(diff);
      }
      
      #if DBGLVL_COMPARE_FILES > 1
      MessageInterface::ShowMessage
         ("column =%3d, baseItem = %e, item = %e, diff = %e, maxDiff = %e\n",
          i, baseItem, baseItem, diff, maxDiffs1[i]);
      #endif
   }
   
   
   //------------------------------------------
   // Now start compare line by line until eof
   //------------------------------------------
   #ifdef DEBUG_FIRST_10_LINES
   for (int i=0; i<10; i++)
   {
      if (baseIn.eof() || in1.eof())
         break;
   #else
      while (!baseIn.eof() && !in1.eof())
   {
   #endif
      
      if (numDirsToCompare >= 2)
         if (in2.eof())
            break;
      
      if (numDirsToCompare == 3)
         if (in3.eof())
            break;
      
      //----------------------------------------------------
      // base file
      //----------------------------------------------------
      baseIn.getline(buffer, BUFFER_SIZE-1);
      line = buffer;
      if (line == "")
         continue;
      
      GetRealColumns(line, baseRealArray);
      
      #if DBGLVL_COMPARE_FILES > 2
      MessageInterface::ShowMessage("===> base file line:\n   '%s'\n", buffer);
      MessageInterface::ShowMessage("   baseRealArray.size()=%d\n", baseRealArray.size());
      #endif
      
      // Check for number of columns in base file
      if ((Integer)(baseRealArray.size()) < numCols)
      {
         // Skip blank line
         if (baseRealArray.size() == 1)
         {
            #if DBGLVL_COMPARE_FILES > 1
            MessageInterface::ShowMessage("first item of base file = %f\n", baseRealArray[0]);
            #endif
            textBuffer.push_back
               ("The base file has less than " + ToString(numCols,1) +
                " columns; so stopping at line " + ToString(lineCount,1) + ".\n");
            break;
         }
      }
      
      #if DBGLVL_COMPARE_FILES > 2
      for (int i=0; i<numCols; i++)
         MessageInterface::ShowMessage("baseRealArray[%2d] = %f\n", i, baseRealArray[i]);
      #endif
      
      //----------------------------------------------------
      // file 1
      //----------------------------------------------------
      in1.getline(buffer, BUFFER_SIZE-1);
      line = buffer;
      if (line == "")
         continue;
      
      GetRealColumns(line, file1RealArray);
      
      #if DBGLVL_COMPARE_FILES > 2
      MessageInterface::ShowMessage("===>    file 1 line:\n   '%s'\n", buffer);
      MessageInterface::ShowMessage("   file1RealArray.size()=%d\n", file1RealArray.size());
      #endif
      
      // Check for number of columns in file1
      if ((Integer)(file1RealArray.size()) < numCols)
      {
         // Skip blank line
         if (file1RealArray.size() == 1)
         {
            #if DBGLVL_COMPARE_FILES > 1
            MessageInterface::ShowMessage("first item of file 1 = %f\n", file1RealArray[0]);
            #endif
            textBuffer.push_back
               ("The base file has less than " + ToString(numCols,1) +
                " columns; so stopping at line " + ToString(lineCount,1) + ".\n");
            break;
         }
      }
      
      //----------------------------------------------------
      // file 2
      //----------------------------------------------------
      if (numDirsToCompare >= 2)
      {
         in2.getline(buffer, BUFFER_SIZE-1);
         line = buffer;
         if (line == "")
            continue;
         
         GetRealColumns(line, file2RealArray);
         
         #if DBGLVL_COMPARE_FILES > 2
         MessageInterface::ShowMessage("===>    file 2 line:\n   '%s'\n", buffer);
         MessageInterface::ShowMessage("   file2RealArray.size()=%d\n", file2RealArray.size());
         #endif
         
         // Check for number of columns in file2
         if ((Integer)(file2RealArray.size()) < numCols)
         {
            // Skip blank line
            if (file2RealArray.size() == 1)
            {
               #if DBGLVL_COMPARE_FILES > 1
               MessageInterface::ShowMessage("first item of file 2 = %f\n", file2RealArray[0]);
               #endif
               textBuffer.push_back
                  ("The base file has less than " + ToString(numCols,1) +
                   " columns; so stopping at line " + ToString(lineCount,1) + ".\n");
               break;
            }
         }
      }
      
      //----------------------------------------------------
      // file 3
      //----------------------------------------------------      
      if (numDirsToCompare == 3)
      {
         in3.getline(buffer, BUFFER_SIZE-1);
         line = buffer;
         if (line == "")
            continue;
         
         GetRealColumns(line, file3RealArray);
         
         #if DBGLVL_COMPARE_FILES > 2
         MessageInterface::ShowMessage("===>    file 3 line:\n   '%s'\n", buffer);
         MessageInterface::ShowMessage("   file3RealArray.size()=%d\n", file3RealArray.size());
         #endif
         
         // Check for number of columns in file3
         if ((Integer)(file3RealArray.size()) < numCols)
         {
            // Skip blank line
            if (file3RealArray.size() == 1)
            {
               #if DBGLVL_COMPARE_FILES > 1
               MessageInterface::ShowMessage("first item of file 3 = %f\n", file3RealArray[0]);
               #endif
               textBuffer.push_back
                  ("The base file has less than " + ToString(numCols,1) +
                   " columns; so stopping at line " + ToString(lineCount,1) + ".\n");
               break;
            }
         }
      }
      
      lineCount++;
      
      #if DBGLVL_COMPARE_FILES > 1
      MessageInterface::ShowMessage("============================== line # = %d\n", lineCount);
      #endif
      
      for (int i=0; i<numCols; i++)
      {
         baseItem = baseRealArray[i];
         item = file1RealArray[i];
         diff = GmatMathUtil::Abs(item - baseItem);
         if (diff > maxDiffs1[i])
            maxDiffs1[i] = diff;
         
         if (numDirsToCompare >= 2)
         {
            item = file2RealArray[i];
            diff = GmatMathUtil::Abs(item - baseItem);
            if (diff > maxDiffs2[i])
               maxDiffs2[i] = diff;
         }
         
         if (numDirsToCompare == 3)
         {
            item = file3RealArray[i];
            diff = GmatMathUtil::Abs(item - baseItem);
            if (diff > maxDiffs3[i])
               maxDiffs3[i] = diff;
         }
         
         #if DBGLVL_COMPARE_FILES > 1
         MessageInterface::ShowMessage
            ("column=%3d, baseItem = %e, item = %e, diff = %e, maxDiffs1 = %e\n",
             i, baseItem, item, diff, maxDiffs1[i]);
         #endif
      }
   }
   
   // report the difference summary
   std::string outLine;
   outLine = "Total lines compared: " + ToString(lineCount) + ",   Tolerance: " +
      ToString(tol, false, true, true, 7, 6) + "\n\n";
   textBuffer.push_back(outLine);
   
   #if DBGLVL_COMPARE_FILES > 1
   MessageInterface::ShowMessage("---> %s", outLine.c_str());
   #endif
   
   if (numDirsToCompare == 1)
   {
      outLine =
         "Column   Maximum Diff1   Max1>Tol\n"
         "------   -------------   ------- \n";
   }
   else if (numDirsToCompare == 2)
   {
      outLine =
         "Column   Maximum Diff1   Max1>Tol   Maximum Diff2   Max2>Tol\n"
         "------   -------------   -------    -------------   --------\n";
   }
   else if (numDirsToCompare == 3)
   {
      outLine =
         "Column   Maximum Diff1   Max1>Tol   Maximum Diff2   Max2>Tol   Maximum Diff3   Max3>Tol\n"
         "------   -------------   -------    -------------   --------   -------------   --------\n";
   }
   
   textBuffer.push_back(outLine);
   
   #if DBGLVL_COMPARE_FILES > 1
   MessageInterface::ShowMessage("---> %s", outLine.c_str());
   #endif
   
   char maxGtTol1, maxGtTol2, maxGtTol3;
   
   for (int i=0; i<numCols; i++)
   {
      maxGtTol1 = ' ';
      maxGtTol2 = ' ';
      maxGtTol3 = ' ';
      
      if (maxDiffs1[i] > tol)
         maxGtTol1 = '*';
      
      if (numDirsToCompare >= 2)
         if (maxDiffs2[i] > tol)
            maxGtTol2 = '*';
      
      if (numDirsToCompare == 3)
         if (maxDiffs3[i] > tol)
            maxGtTol3 = '*';
      
      if (numDirsToCompare == 1)
      {
         outLine = ToString(i+1) + "     " +
            ToString(maxDiffs1[i], false, true, true, 7, 6) + "      " + maxGtTol1 + "\n";
      }
      else if (numDirsToCompare == 2)
      {
         outLine = ToString(i+1) + "     " +
            ToString(maxDiffs1[i], false, true, true, 7, 6) + "      " + maxGtTol1 + "       " +
            ToString(maxDiffs2[i], false, true, true, 7, 6) + "      " + maxGtTol2 + "\n";
      }
      else if (numDirsToCompare == 3)
      {
         outLine = ToString(i+1) + "     " +
            ToString(maxDiffs1[i], false, true, true, 7, 6) + "      " + maxGtTol1 + "       " +
            ToString(maxDiffs2[i], false, true, true, 7, 6) + "      " + maxGtTol2 + "       " +
            ToString(maxDiffs3[i], false, true, true, 7, 6) + "      " + maxGtTol3 + "\n";
      }
      
      textBuffer.push_back(outLine);
      
      #if DBGLVL_COMPARE_FILES > 1
      MessageInterface::ShowMessage("---> %s", outLine.c_str());
      #endif
   }
   
   baseIn.close();
   in1.close();
   in2.close();
   in3.close();

   return textBuffer;
}
   

//------------------------------------------------------------------------------
// StringArray& CompareTextLines(Integer numDirsToCompare, ...)
//------------------------------------------------------------------------------
/**
 * Compares ascii files line by line with the same file name in different directory.
 */
//------------------------------------------------------------------------------
StringArray& GmatFileUtil::CompareTextLines(Integer numDirsToCompare,
                                        const std::string &basefilename,
                                        const std::string &filename1,
                                        const std::string &filename2,
                                        const std::string &filename3,
                                        int &file1DiffCount, int &file2DiffCount,
                                        int &file3DiffCount)
{
   #if DBGLVL_COMPARE_FILES
   MessageInterface::ShowMessage
      ("GmatFileUtil::CompareTextLines() entered, numDirsToCompare=%d\n", numDirsToCompare);
   #endif
   std::ifstream baseIn;
   std::ifstream in1;
   std::ifstream in2;
   std::ifstream in3;
   
   bool ok = PrepareCompare(numDirsToCompare, basefilename, filename1,
                            filename2, filename3, baseIn, in1, in2, in3);
   
   if (!ok)
      return textBuffer;
   
   
   char buffer[BUFFER_SIZE];
   std::string line0, line1, line2, line3;
   file1DiffCount = 0;
   file2DiffCount = 0;
   file3DiffCount = 0;
   int lineCount = 1;
   std::stringstream diffLines1("");
   std::stringstream diffLines2("");
   std::stringstream diffLines3("");
   
   //------------------------------------------
   // now start compare
   //------------------------------------------
   while (!baseIn.eof() && !in1.eof())
   {
      if (numDirsToCompare >= 2)
         if (in2.eof())
            break;
      
      if (numDirsToCompare >= 3)
         if (in3.eof())
            break;
      
      lineCount++;
      
      #if DBGLVL_COMPARE_FILES > 1
      MessageInterface::ShowMessage("============================== line # = %d\n", lineCount);
      #endif
      
      //----------------------------------------------------
      // base file
      //----------------------------------------------------
      baseIn.getline(buffer, BUFFER_SIZE-1);
      line0 = buffer;
      
      #if DBGLVL_COMPARE_FILES > 2
      MessageInterface::ShowMessage("===> base file: line = %s\n", buffer);
      #endif
      
      //----------------------------------------------------
      // file 1
      //----------------------------------------------------
      in1.getline(buffer, BUFFER_SIZE-1);
      line1 = buffer;
      
      #if DBGLVL_COMPARE_FILES > 2
      MessageInterface::ShowMessage("===>    file 1: line = %s\n", buffer);
      #endif
      
      if (line0 != line1)
      {
         diffLines1 << " 0: " << line0 << "\n" << " 1: " << line1 << "\n";
         file1DiffCount++;
      }
      
      //----------------------------------------------------
      // file 2
      //----------------------------------------------------      
      if (numDirsToCompare >= 2)
      {
         in2.getline(buffer, BUFFER_SIZE-1);
         line2 = buffer;
      
         #if DBGLVL_COMPARE_FILES > 2
         MessageInterface::ShowMessage("===>    file 2: line = %s\n", buffer);
         #endif
         
         if (line0 != line2)
         {
            diffLines2 << " 0: " << line0 << "\n" << " 2: " << line2 << "\n";
            file2DiffCount++;
         }
      }
      
      //----------------------------------------------------
      // file 3
      //----------------------------------------------------      
      if (numDirsToCompare >= 3)
      {
         in3.getline(buffer, BUFFER_SIZE-1);
         line3 = buffer;
      
         #if DBGLVL_COMPARE_FILES > 2
         MessageInterface::ShowMessage("===>    file 3: line = %s\n", buffer);
         #endif
         
         if (line0 != line3)
         {
            diffLines3 << " 0: " << line0 << "\n" << " 3: " << line3 << "\n";
            file3DiffCount++;
         }
      }
   }
   
   // report the difference summary
   std::string outLine;
   outLine = "Total lines compared: " + ToString(lineCount) + "\n\n";
   textBuffer.push_back(outLine);

   #if DBGLVL_COMPARE_FILES
   MessageInterface::ShowMessage("%s", outLine.c_str());
   #endif
   
   outLine = "File1 - Number of Lines different: " + ToString(file1DiffCount) + "\n";
   textBuffer.push_back(outLine);
   if (file1DiffCount > 0)
      textBuffer.push_back(diffLines1.str());
   
   #if DBGLVL_COMPARE_FILES
   MessageInterface::ShowMessage("%s", outLine.c_str());
   #endif
   
   if (numDirsToCompare >= 2)
   {
      outLine = "File2 - Number of Lines different: " + ToString(file2DiffCount) + "\n";
      textBuffer.push_back(outLine);
      if (file2DiffCount > 0)
         textBuffer.push_back(diffLines2.str());
      
      #if DBGLVL_COMPARE_FILES
      MessageInterface::ShowMessage("%s", outLine.c_str());
      #endif
   }
   
   if (numDirsToCompare >= 3)
   {
      outLine = "File3 - Number of Lines different: " + ToString(file3DiffCount) + "\n";
      textBuffer.push_back(outLine);
      if (file3DiffCount > 0)
         textBuffer.push_back(diffLines3.str());
      
      #if DBGLVL_COMPARE_FILES
      MessageInterface::ShowMessage("%s", outLine.c_str());
      #endif
   }
   
   textBuffer.push_back("\n");
   
   baseIn.close();
   in1.close();
   in2.close();
   in3.close();
   
   return textBuffer;
}


//------------------------------------------------------------------------------
// StringArray& CompareNumericLines(Integer numDirsToCompare, ... )
//------------------------------------------------------------------------------
/**
 * Compares files line by line numerically using tolerance. String embeded in
 * a text line or blank line is ignored and continued with next item in the line.
 * 
 */
//------------------------------------------------------------------------------
StringArray& GmatFileUtil::CompareNumericLines(Integer numDirsToCompare,
                                               const std::string &basefilename,
                                               const std::string &filename1,
                                               const std::string &filename2,
                                               const std::string &filename3,
                                               int &file1DiffCount, int &file2DiffCount,
                                               int &file3DiffCount, Real tol)
{
   #if DBGLVL_COMPARE_FILES
   MessageInterface::ShowMessage
      ("GmatFileUtil::CompareNumericLines() entered, numDirsToCompare=%d, tol=%f\n",
       numDirsToCompare, tol);
   #endif
   
   std::ifstream baseIn;
   std::ifstream in1;
   std::ifstream in2;
   std::ifstream in3;
   
   bool ok = PrepareCompare(numDirsToCompare, basefilename, filename1,
                            filename2, filename3, baseIn, in1, in2, in3);
   
   if (!ok)
   {
      #if DBGLVL_COMPARE_FILES
      MessageInterface::ShowMessage
         ("GmatFileUtil::CompareNumericLines() returning, failed during compare prep\n");
      #endif
      return textBuffer;
   }
   
   // Now compare numeric lines
   char buffer[BUFFER_SIZE];
   std::string line0, line1, line2, line3;
   file1DiffCount = 0;
   file2DiffCount = 0;
   file3DiffCount = 0;
   int compareCount = 0;
   std::stringstream diffLines1("");
   std::stringstream diffLines2("");
   std::stringstream diffLines3("");
   Real diff = 999.999;
   
   if (baseIn.eof())
   {
      #if DBGLVL_COMPARE_FILES
      MessageInterface::ShowMessage("===> end of base file encountered\n");
      #endif
   }
   
   if (in1.eof())
   {
      #if DBGLVL_COMPARE_FILES
      MessageInterface::ShowMessage("===> end of file 1 encountered\n");
      #endif
   }
   
   //------------------------------------------
   // now start compare
   //------------------------------------------
   while (!baseIn.eof() && !in1.eof())
   {
      if (numDirsToCompare >= 2)
      {
         if (in2.eof())
         {
            #if DBGLVL_COMPARE_FILES
            MessageInterface::ShowMessage("===> end of file 2 encountered\n");
            #endif
            break;
         }
      }
      if (numDirsToCompare >= 3)
         if (in3.eof())
         {
            #if DBGLVL_COMPARE_FILES
            MessageInterface::ShowMessage("===> end of file e encountered\n");
            #endif
            break;
         }
      
      compareCount++;
      
      #if DBGLVL_COMPARE_FILES > 1
      MessageInterface::ShowMessage("============================== compare count # = %d\n", compareCount);
      #endif
      
      //----------------------------------------------------
      // base file
      //----------------------------------------------------
      baseIn.getline(buffer, BUFFER_SIZE-1);
      line0 = buffer;
      
      // Get next non-blank line
      if (line0 == "")
      {
         while (!baseIn.eof())
         {
            baseIn.getline(buffer, BUFFER_SIZE-1);
            line0 = buffer;
            
            if (line0 == "")
               continue;
            else
               break;
         }
      }
      
      #if DBGLVL_COMPARE_FILES > 2
      MessageInterface::ShowMessage("===> base file: line = '%s'\n", line0.c_str());
      #endif
      
      //----------------------------------------------------
      // file 1
      //----------------------------------------------------
      in1.getline(buffer, BUFFER_SIZE-1);
      line1 = buffer;
      
      // Get next non-blank line
      if (line1 == "")
      {
         while (!in1.eof())
         {
            in1.getline(buffer, BUFFER_SIZE-1);
            line1 = buffer;
            
            if (line1 == "")
               continue;
            else
               break;
         }
      }
      
      #if DBGLVL_COMPARE_FILES > 2
      MessageInterface::ShowMessage("===>    file 1: line = '%s'\n", line1.c_str());
      #endif
      
      if (!CompareLines(line0, line1, diff, tol))
      {
         diffLines1 << " 0: " << line0 << "\n" << " 1: " << line1 << "\n";
         file1DiffCount++;
      }
      
      
      //----------------------------------------------------
      // file 2
      //----------------------------------------------------      
      if (numDirsToCompare >= 2)
      {
         in2.getline(buffer, BUFFER_SIZE-1);
         line2 = buffer;
         
         // Get next non-blank line
         if (line2 == "")
         {
            while (!in2.eof())
            {
               in2.getline(buffer, BUFFER_SIZE-1);
               line2 = buffer;
               
               if (line2 == "")
                  continue;
               else
                  break;
            }
         }
         
         #if DBGLVL_COMPARE_FILES > 2
         MessageInterface::ShowMessage("===>    file 2: line = %s\n", line2.c_str());
         #endif
         
         if (!CompareLines(line0, line2, diff, tol))
         {
            diffLines2 << " 0: " << line0 << "\n" << " 2: " << line2 << "\n";
            file2DiffCount++;
         }
      }

      
      //----------------------------------------------------
      // file 3
      //----------------------------------------------------      
      if (numDirsToCompare >= 3)
      {
         in3.getline(buffer, BUFFER_SIZE-1);
         line3 = buffer;
      
         // Get next non-blank line
         if (line3 == "")
         {
            while (!in3.eof())
            {
               in3.getline(buffer, BUFFER_SIZE-1);
               line3 = buffer;
               
               if (line3 == "")
                  continue;
               else
                  break;
            }
         }
         
         #if DBGLVL_COMPARE_FILES > 2
         MessageInterface::ShowMessage("===>    file 3: line = %s\n", line3.c_str());
         #endif
         
         if (!CompareLines(line0, line3, diff, tol))
         {
            diffLines3 << " 0: " << line0 << "\n" << " 3: " << line3 << "\n";
            file3DiffCount++;
         }
      }
   }
   
   // report the difference summary
   std::string outLine;
   outLine = "Total lines compared: " + ToString(compareCount) + "\n\n";
   textBuffer.push_back(outLine);
   
   #if DBGLVL_COMPARE_FILES
   MessageInterface::ShowMessage("%s", outLine.c_str());
   #endif
   
   outLine = "File1 - Number of Lines different: " + ToString(file1DiffCount) + "\n";
   textBuffer.push_back(outLine);
   if (file1DiffCount > 0)
      textBuffer.push_back(diffLines1.str());
   
   #if DBGLVL_COMPARE_FILES
   MessageInterface::ShowMessage("%s", outLine.c_str());
   #endif
   
   if (numDirsToCompare >= 2)
   {
      outLine = "File2 - Number of Lines different: " + ToString(file2DiffCount) + "\n";
      textBuffer.push_back(outLine);
      if (file2DiffCount > 0)
         textBuffer.push_back(diffLines2.str());
      
      #if DBGLVL_COMPARE_FILES
      MessageInterface::ShowMessage("%s", outLine.c_str());
      #endif
   }
   
   if (numDirsToCompare >= 3)
   {
      outLine = "File3 - Number of Lines different: " + ToString(file3DiffCount) + "\n";
      textBuffer.push_back(outLine);
      if (file3DiffCount > 0)
         textBuffer.push_back(diffLines3.str());
      
      #if DBGLVL_COMPARE_FILES
      MessageInterface::ShowMessage("%s", outLine.c_str());
      #endif
   }
   
   textBuffer.push_back("\n");
   
   baseIn.close();
   in1.close();
   in2.close();
   in3.close();
   
   #if DBGLVL_COMPARE_FILES
   MessageInterface::ShowMessage
      ("GmatFileUtil::CompareNumericLines() returning, %d lines\n", textBuffer.size());
   #endif
   return textBuffer;
}


//------------------------------------------------------------------------------
// bool SkipHeaderLines(ifstream &in, StringArray &tokens, ...)
//------------------------------------------------------------------------------
bool GmatFileUtil::SkipHeaderLines(std::ifstream &in, RealArray &realArray,
                                   const std::string &filename)
{
   #if DBGLVL_COMPARE_FILES > 0
   MessageInterface::ShowMessage
      ("GmatFileUtil::SkipHeaderLines() entered, filename='%s'\n", filename.c_str());
   #endif
   
   char buffer[BUFFER_SIZE];
   bool dataFound = false;
   bool alphaFound = false;
   Real rval;
   int colCount = 0, fileCols = 0;
   char ch;
   StringTokenizer stk;
   std::string line;
   
   while (!dataFound)
   {
      if (in.eof())
      {
         #if DBGLVL_COMPARE_FILES > 3
         MessageInterface::ShowMessage("   end-of-file encountered, exiting while loop\n");
         #endif
         break;
      }
      
      in.getline(buffer, BUFFER_SIZE-1);
      line = buffer;
      
      #if DBGLVL_COMPARE_FILES > 3
      MessageInterface::ShowMessage
         ("   length=%d, line = %s\n", line.length(), line.c_str());
      #endif
      
      if (line.length() == 0)
      {
         #if DBGLVL_COMPARE_FILES > 3
         MessageInterface::ShowMessage("   zero length, so continue\n");
         #endif
         continue;
      }
      
      // skip blank spaces
      if (GmatStringUtil::Strip(line) == "")
      {
         #if DBGLVL_COMPARE_FILES > 3
         MessageInterface::ShowMessage("   blank spaces found, so continue\n");
         #endif
         continue;
      }
      
      alphaFound = false;
      
      for (unsigned int i=0; i<line.length(); i++)
      {
         ch = line[i];
         
         #if DBGLVL_COMPARE_FILES > 3
         MessageInterface::ShowMessage("%c", ch);
         #endif
         
         if (!isdigit(ch) && ch != '.' && ch != 'e' && ch != 'E' && ch != '-' &&
             ch != '+' && ch != ' ' && !isspace(ch))
         {
            alphaFound = true;
            break;
         }
      }
      
      #if DBGLVL_COMPARE_FILES > 3
      MessageInterface::ShowMessage("\n");
      #endif
      
      if (alphaFound)
      {
         // Skip ":" for UTC time format
         if (line.find(":") == line.npos)
         {
            #if DBGLVL_COMPARE_FILES > 3
            MessageInterface::ShowMessage("   non-numeric found, so continue\n");
            #endif
            continue;
         }
      }
      
      if (line.find("--") != line.npos)
      {
         #if DBGLVL_COMPARE_FILES > 3
         MessageInterface::ShowMessage("   -- found, so continue\n");
         #endif
         continue;
      }
      
      dataFound = GetRealColumns(line, realArray);
   }
   
   #if DBGLVL_COMPARE_FILES > 0
   int numCols = realArray.size();
   MessageInterface::ShowMessage("   number of columns = %d\n", numCols);
   for (int i=0; i<numCols; i++)
      MessageInterface::ShowMessage("   realArray[%d]=%f\n", i, realArray[i]);
   MessageInterface::ShowMessage
      ("GmatFileUtil::SkipHeaderLines() returning dataFound=%d\n", dataFound);
   #endif
   
   return dataFound;
}

//------------------------------------------------------------------------------
// bool IsAsciiFile(std::ifstream &file, const std::string &filename)
//------------------------------------------------------------------------------
/**
 * Checks if file is an ascii file.
 */
//------------------------------------------------------------------------------
bool GmatFileUtil::IsAsciiFile(std::ifstream &file, const std::string &filename)
{
   #if DBGLVL_COMPARE_FILES
   MessageInterface::ShowMessage
      ("GmatFileUtil::IsAsciiFile() entered, filename='%s'\n", filename.c_str());
   #endif
   
   // Get length of file
   file.seekg (0, file.end);
   long int length = file.tellg();
   file.seekg (0, file.beg);
   char *buffer = new char [length];
   
   #if DBGLVL_COMPARE_FILES > 1
   MessageInterface::ShowMessage("   Reading %d characters\n", length);
   #endif
   
   // read data as a block:
   file.read (buffer, length);
   
   if (file)
   {
      #if DBGLVL_COMPARE_FILES > 1
      MessageInterface::ShowMessage("   all characters read successfully\n");
      #endif
   }
   else
   {
      length = file.gcount();
      #if DBGLVL_COMPARE_FILES > 1
      MessageInterface::ShowMessage("   error: only %d could be read\n", length);
      #endif
   }
   
   bool isAscii = true;
   // Check if file is an ascii file
   for (long int i = 0; i < length; i++)
   {
      #if DBGLVL_COMPARE_FILES > 3
      MessageInterface::ShowMessage("   buffer[%4d]='%c'\n", i, buffer[i]);
      #endif
      if (!isprint(buffer[i]) && !isspace(buffer[i]))
      {
         isAscii = false;
         break;
      }
   }
   
   delete[] buffer;
   
   // Clear the stream state and position to beginning of the stream
   file.clear();
   //file.seekg (0, std::ios::beg);
   file.seekg (0, file.beg);
   
   #if DBGLVL_COMPARE_FILES
   MessageInterface::ShowMessage("GmatFileUtil::IsAsciiFile() returning %d\n", isAscii);
   #endif
   return isAscii;
}


//------------------------------------------------------------------------------
// bool GetRealColumns(const std::string &line, StringArray &cols)
//------------------------------------------------------------------------------
bool GmatFileUtil::GetRealColumns(const std::string &line, RealArray &cols)
{
   #ifdef DEBUG_REAL_COLUMNS
   MessageInterface::ShowMessage
      ("GmatFileUtil::GetRealColumns() entered, line:\n   '%s'\n", line.c_str());
   #endif
   
   StringTokenizer stk;
   //@note ":" is for separating UTC format time
   // Added horizontal tab (\t) to delimiter
   stk.Set(line, " :\t");
   StringArray tokens = stk.GetAllTokens();
   Integer numCols = tokens.size();
   Real rval;
   std::string item;
   bool realColFound = true;
   
   #ifdef DEBUG_REAL_COLUMNS
   MessageInterface::ShowMessage("   numCols=%2d, cols.size()=%2d\n", numCols, cols.size());
   #endif
   
   cols.clear();
   for (int i=0; i<numCols; i++)
   {
      item = tokens[i];
      if (GmatStringUtil::ToReal(item, rval))
         cols.push_back(rval);
      else
      {
         // Try UTC time format in "DD MMM YYYY HH"
         // Just convert month name to month number
         Integer month = GmatTimeUtil::GetMonth(item);
         if (month != -1)
         {
            cols.push_back((Real)month);
            continue;
         }
         
         // Try Time format in YYYY-MM-DDTHH
         // Store time in "DD MMM YYYY HH" format for compare
         StringTokenizer stk1;
         stk1.Set(item, "-T");
         StringArray tokens1 = stk1.GetAllTokens();
         if (tokens1.size() == 4)
         {
            if (GmatStringUtil::ToReal(tokens1[2], rval))
               cols.push_back(rval);
            else
               realColFound = false;
            
            if (GmatStringUtil::ToReal(tokens1[1], rval))
               cols.push_back(rval);
            else
               realColFound = false;
            
            if (GmatStringUtil::ToReal(tokens1[0], rval))
               cols.push_back(rval);
            else
               realColFound = false;
            
            if (GmatStringUtil::ToReal(tokens1[3], rval))
               cols.push_back(rval);
            else
               realColFound = false;
         }
         else
            realColFound = false;
         
         if (!realColFound)
            break;
      }
   }
   
   #ifdef DEBUG_REAL_COLUMNS
   MessageInterface::ShowMessage
      ("GmatFileUtil::GetRealColumns() returning %d, cols.size()=%d\n",
       realColFound, cols.size());
   #endif
   
   return realColFound;
}


