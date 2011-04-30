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
#include "RealUtilities.hpp"       // for Abs()
#include "StringUtil.hpp"          // for ToString()
#include "FileTypes.hpp"           // for GmatFile::MAX_PATH_LEN
#include <algorithm>               // for set_difference()
#include <iterator>                // For back_inserter() with VC++ 2010

#ifndef _MSC_VER  // if not Microsoft Visual C++
#include <dirent.h>
#endif

#ifdef __WIN32__
#include <windows.h>
#endif


using namespace GmatStringUtil;

//#define DBGLVL_COMPARE_REPORT 1
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
// std::string GetCurrentPath()
//------------------------------------------------------------------------------
/*
 * Note: This function calls getcwd() which is defined in <dirent>. There is a
 *       problem compling with VC++ compiler, so until it is resolved, it will
 *       always return blank if it is compiled with VC++ compiler.
 *
 * @return  The current working directory, generally the application path.
 *
 */
//------------------------------------------------------------------------------
std::string GmatFileUtil::GetCurrentPath()
{
   std::string currPath;
   
#ifndef _MSC_VER  // if not Microsoft Visual C++
   char buffer[GmatFile::MAX_PATH_LEN];
   // Intentionally get the return and then ignore it to move warning from
   // system libraries to GMAT code base.  The "unused variable" warning
   // here can be safely ignored.
//   char *ch = getcwd(buffer, GmatFile::MAX_PATH_LEN);
   // This clears a warning message
   if (getcwd(buffer, GmatFile::MAX_PATH_LEN) != buffer)
      ;
   currPath = buffer;
#else
   MessageInterface::ShowMessage
      ("*** WARNING *** GmatFileUtil::GetCurrentPath() \n"
       "Cannot compile getcwd() with MSVC, so jsut returning empty path\n");
#endif
   
   return currPath;
   
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
   
   std::string filePath;
   std::string::size_type lastSlash = fullPath.find_last_of("/\\");
   
   if (lastSlash != filePath.npos)
   {
      if (appendSep)
         filePath = fullPath.substr(0, lastSlash + 1);
      else
         filePath = fullPath.substr(0, lastSlash);
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
   MessageInterface::ShowMessage("   ==> dirName='%s'\n", dirName.c_str());
   #endif
   
#ifndef _MSC_VER  // if not Microsoft Visual C++
   DIR *dir = NULL;
   dir = opendir(dirName.c_str());
   
   if (dir != NULL)
   {
      dirExist = true; 
      closedir(dir);
   }
#else
   TCHAR currDir[BUFFER_SIZE];
   
   // Save current directory
   DWORD ret = GetCurrentDirectory(BUFFER_SIZE, currDir);
   // Try setting to requested direcotry
   dirExist = (SetCurrentDirectory(dirName.c_str()) == 0 ? false : true);
   // Set back to current directory
   SetCurrentDirectory(currDir);
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
// StringArray& Compare(const std::string &filename1, const std::string &filename2,
//                      Real tol = 1.0e-4)
//------------------------------------------------------------------------------
StringArray& GmatFileUtil::Compare(const std::string &filename1,
                                   const std::string &filename2,
                                   const StringArray &colTitles,
                                   Real tol)
{
   textBuffer.clear();
   textBuffer.push_back("\n======================================== Compare Utility\n");
   textBuffer.push_back("filename1=" + filename1 + "\n");
   textBuffer.push_back("filename2=" + filename2 + "\n");

   #if DBGLVL_COMPARE_REPORT
   MessageInterface::ShowMessage("\n======================================== Compare Utility\n");
   MessageInterface::ShowMessage("filename1=%s\n");
   MessageInterface::ShowMessage("filename2=%s\n");
   #endif
   
   // open file
   std::ifstream in1(filename1.c_str());
   std::ifstream in2(filename2.c_str());
   
   if (!in1)
   {
      textBuffer.push_back("Cannot open first file: " +  filename1 + "\n\n");
      return textBuffer;
   }
   
   if (!in2)
   {
      textBuffer.push_back("Cannot open second file: " + filename2 + "\n\n");
      return textBuffer;
   }
   
   char buffer[BUFFER_SIZE];
   Real item1, item2, diff;
   std::string line;
   int count = 1;
   int file1Cols = 0, file2Cols = 0;
   StringTokenizer stk;
   StringArray tokens1, tokens2;
   
   //------------------------------------------
   // if files have header lines, skip
   //------------------------------------------
   if (!GmatFileUtil::SkipHeaderLines(in1, tokens1))
   {
      textBuffer.push_back("***Cannot compare files: Data record not found on file 1.\n");
      return textBuffer;
   }
   
   if (!GmatFileUtil::SkipHeaderLines(in2, tokens2))
   {
      textBuffer.push_back("***Cannot compare files: Data record not found on file 2.\n");
      return textBuffer;
   }
   
   //------------------------------------------
   // check number of columns
   //------------------------------------------
   file1Cols = tokens1.size();
   file2Cols = tokens2.size();
   Integer numCols = file1Cols<file2Cols ? file1Cols : file2Cols;
   
   if (file1Cols != file2Cols)
   {
      textBuffer.push_back
         ("*** Number of colmuns are different. file1:" + ToString(file1Cols) +
          ",  file2:" + ToString(file2Cols) + "\n*** Will compare up to" +
          ToString(numCols) + " columns\n");
   }

   // compare first data line
   RealArray minDiffs, maxDiffs;
   IntegerArray minLines, maxLines;
   
   for (int i=0; i<numCols; i++)
   {
      item1 = atof(tokens1[i].c_str());
      item2 = atof(tokens2[i].c_str());
      diff = GmatMathUtil::Abs(item1 - item2);
      minDiffs.push_back(diff);
      maxDiffs.push_back(diff);
      minLines.push_back(1);
      maxLines.push_back(1);
      
      #if DBGLVL_COMPARE_REPORT > 1
      MessageInterface::ShowMessage
         ("column=%3d, item1=% e, item2=% e, diff=% e, minDiff=% e, maxDiff=% e\n",
          i, item1, item1, diff, minDiffs[i], maxDiffs[i]);
      #endif
   }
   
   //------------------------------------------
   // now start compare
   //------------------------------------------
   #if DBGLVL_COMPARE_REPORT > 2
   for (int i=0; i<10; i++)
   {
      if (in1.eof() || in2.eof())
         break;
   #else
   while (!in1.eof() && !in2.eof())
   {
   #endif

      count++;
      
      #if DBGLVL_COMPARE_REPORT > 1
      MessageInterface::ShowMessage("============================== line # = %d\n", count);
      #endif
      
      // file 1
      in1.getline(buffer, BUFFER_SIZE-1);
      line = buffer;
      
      #if DBGLVL_COMPARE_REPORT > 2
      MessageInterface::ShowMessage("===> file 1: buffer = %s\n", buffer);
      #endif
      
      stk.Set(line, " ");
      tokens1 = stk.GetAllTokens();

      // check for blank lines in file1
      if ((Integer)(tokens1.size()) != file1Cols)
         break;
      
      #if DBGLVL_COMPARE_REPORT > 2
      for (int i=0; i<numCols; i++)
         MessageInterface::ShowMessage("tokens1[%d] = %s\n", i, tokens1[i].c_str());
      #endif
      
      // file 2
      in2.getline(buffer, BUFFER_SIZE-1);
      line = buffer;
      
      #if DBGLVL_COMPARE_REPORT > 2
      MessageInterface::ShowMessage("===> file 2: buffer = %s\n", buffer);
      #endif
      
      stk.Set(line, " ");
      tokens2 = stk.GetAllTokens();
      
      // check for blank lines in file1
      if ((Integer)(tokens2.size()) != file1Cols)
         break;
      
      #if DBGLVL_COMPARE_REPORT > 2
      for (int i=0; i<file1Cols; i++)
         MessageInterface::ShowMessage("tokens2[%d] = %s\n", i, tokens2[i].c_str());
      #endif
      
      for (int i=0; i<numCols; i++)
      {
         item1 = atof(tokens1[i].c_str());
         item2 = atof(tokens2[i].c_str());
         diff = GmatMathUtil::Abs(item1 - item2);
            
         if (diff < minDiffs[i])
         {
            minDiffs[i] = diff;
            minLines[i] = count;
         }
            
         if (diff > maxDiffs[i])
         {
            maxDiffs[i] = diff;
            maxLines[i] = count;
         }
            
         #if DBGLVL_COMPARE_REPORT > 1
         MessageInterface::ShowMessage
            ("column=%3d, item1=% e, item2=% e, diff=% e, minDiff=% e, maxDiff=% e\n",
             i, item1, item2, diff, minDiffs[i], maxDiffs[i]);
         #endif
      }
         
   }
   
   // report the difference summary
   std::string outLine;
   outLine = "Total lines compared: " + ToString(count) + ",   Tolerance: " +
      ToString(tol, false, true, true, 7, 6) + "\n\n";
   textBuffer.push_back(outLine);

   #if DBGLVL_COMPARE_REPORT
   MessageInterface::ShowMessage("%s", outLine.c_str());
   #endif

   if (colTitles.size() == 0)
   {
      outLine =
         "Column   Minimum Diff.   Line#   Maximum Diff.   Line#   Min>Tol   "
         "Max>Tol\n"
         "------   -------------   -----   -------------   -----   -------   "
         "-------\n";
   }
   else
   {
      outLine =
         "Column   Column Title                     Minimum Diff.   Line#   "
         "Maximum Diff.   Line#   Min>Tol   Max>Tol\n"
         "------   ------------                     -------------   -----   "
         "-------------   -----   -------   -------\n";
   }
   textBuffer.push_back(outLine);
   
   #if DBGLVL_COMPARE_REPORT
   MessageInterface::ShowMessage("%s", outLine.c_str());
   #endif
   
   char minGtTol, maxGtTol;
   char title[30] = "";
   
   for (int i=0; i<numCols; i++)
   {
      minGtTol = ' ';
      maxGtTol = ' ';
      
      if (minDiffs[i] > tol)
         minGtTol = '*';
      
      if (maxDiffs[i] > tol)
         maxGtTol = '*';

      if (colTitles.size() == 0)
      {
         outLine = ToString(i+1) + "     " + ToString(minDiffs[i], false, true, true, 7, 6) +
            "   " + ToString(minLines[i]) + "    " +
            ToString(maxDiffs[i], false, true, true, 7, 6) + "   " + ToString(maxLines[i]) +
            "       " + minGtTol + "         " + maxGtTol + "\n";
      }
      else
      {
         sprintf(title, "%-30.30s", colTitles[i].c_str());
         outLine = ToString(i+1) + "     " + title + "   " +
            ToString(minDiffs[i], false, true, true, 7, 6) + "   " + ToString(minLines[i]) +
            "    " + ToString(maxDiffs[i], false, true, true, 7, 6) + "   " +
            ToString(maxLines[i]) + "       " + minGtTol + "         " +
            maxGtTol + "\n";
      }
      
      textBuffer.push_back(outLine);
      
      #if DBGLVL_COMPARE_REPORT
      MessageInterface::ShowMessage("%s", outLine.c_str());
      #endif
   }
   
   in1.close();
   in2.close();

   return textBuffer;
}
   
   
//------------------------------------------------------------------------------
// StringArray& Compare(Integer numDirsToCompare, const std::string &basefilename,
//                      const std::string &filename1, const std::string &filename2,
//                      const std::string &filename3,  const StringArray &colTitles,
//                      Real tol = CompareAbsTol);
//------------------------------------------------------------------------------
StringArray& GmatFileUtil::Compare(Integer numDirsToCompare, const std::string &basefilename,
                                   const std::string &filename1, const std::string &filename2,
                                   const std::string &filename3, const StringArray &colTitles,
                                   Real tol)
{
   textBuffer.clear();
   textBuffer.push_back("\n======================================== Compare Utility\n");
   textBuffer.push_back("basefile =" + basefilename + "\n");

   textBuffer.push_back("filename1=" + filename1 + "\n");
   textBuffer.push_back("filename2=" + filename2 + "\n");
   
   if (numDirsToCompare == 3)
      textBuffer.push_back("filename3=" + filename3 + "\n");

   #if DBGLVL_COMPARE_REPORT
   MessageInterface::ShowMessage("\n======================================== Compare Utility\n");
   MessageInterface::ShowMessage("numDirsToCompare=%3\n", numDirsToCompare);
   MessageInterface::ShowMessage("basefile =%s\n", basefilename.c_str());
   MessageInterface::ShowMessage("filename1=%s\nfilename2=%s\nfilename3=%s\n",
                                 filename1.c_str(), filename2.c_str(), filename3.c_str());
   #endif
   
   // open base file
   std::ifstream baseIn(basefilename.c_str());

   // open compare files
   std::ifstream in1(filename1.c_str());
   std::ifstream in2(filename2.c_str());
   std::ifstream in3(filename3.c_str());
   
   if (!baseIn)
   {
      textBuffer.push_back("Cannot open base file: " +  basefilename + "\n\n");
      return textBuffer;
   }
   
   if (!in1)
   {
      textBuffer.push_back("Cannot open first file: " + filename1 + "\n\n");
      return textBuffer;
   }
   
   if (!in2)
   {
      textBuffer.push_back("Cannot open second file: " + filename2 + "\n\n");
      return textBuffer;
   }

   if (numDirsToCompare == 3)
      if (!in3)
      {
         textBuffer.push_back("Cannot open third file: " + filename3 + "\n\n");
         return textBuffer;
      }

   
   char buffer[BUFFER_SIZE];
   Real baseItem, item, diff;
   std::string line;
   int count = 1;
   int baseCols = 99, file1Cols = 99, file2Cols = 99, file3Cols = 99;
   StringTokenizer stk;
   StringArray baseTokens, tokens1, tokens2, tokens3;
   
   //------------------------------------------
   // if files have header lines, skip
   //------------------------------------------
   if (!GmatFileUtil::SkipHeaderLines(baseIn, baseTokens))
   {
      textBuffer.push_back("***Cannot compare files: Data record not found on base file.\n");
      return textBuffer;
   }
   
   if (!GmatFileUtil::SkipHeaderLines(in1, tokens1))
   {
      textBuffer.push_back("***Cannot compare files: Data record not found on file 1.\n");
      return textBuffer;
   }
   
   if (!GmatFileUtil::SkipHeaderLines(in2, tokens2))
   {
      textBuffer.push_back("***Cannot compare files: Data record not found on file 2.\n");
      return textBuffer;
   }
   
   if (numDirsToCompare == 3)
      if (!GmatFileUtil::SkipHeaderLines(in3, tokens3))
      {
         textBuffer.push_back("***Cannot compare files: Data record not found on file 3.\n");
         return textBuffer;
      }
   
   //------------------------------------------
   // check number of columns
   //------------------------------------------
   baseCols = baseTokens.size();
   file1Cols = tokens1.size();
   file2Cols = tokens2.size();
   
   if (numDirsToCompare == 3)
      file3Cols = tokens3.size();
   
   Integer numCols = baseCols<file1Cols ? baseCols : file1Cols;
   numCols = numCols<file2Cols ? numCols : file2Cols;
   //MessageInterface::ShowMessage("===> numCols=%d\n", numCols);
   
   if (numDirsToCompare == 3)
      numCols = numCols<file3Cols ? numCols : file3Cols;
   
   if (baseCols != file1Cols)
   {
      textBuffer.push_back
         ("*** Number of colmuns are different. file1:" + ToString(baseCols) +
          ",  file2:" + ToString(file1Cols) + "\n*** Will compare up to" +
          ToString(numCols) + " columns\n");
   }
   
   // compare first data line
   RealArray maxDiffs1, maxDiffs2, maxDiffs3;
   
   for (int i=0; i<numCols; i++)
   {
      baseItem = atof(baseTokens[i].c_str());
      item = atof(tokens1[i].c_str());
      diff = GmatMathUtil::Abs(item - baseItem);
      maxDiffs1.push_back(diff);
      
      item = atof(tokens2[i].c_str());
      diff = GmatMathUtil::Abs(item - baseItem);
      maxDiffs2.push_back(diff);
      
      if (numDirsToCompare == 3)
      {
         item = atof(tokens3[i].c_str());
         diff = GmatMathUtil::Abs(item - baseItem);
         maxDiffs3.push_back(diff);
      }
      
      #if DBGLVL_COMPARE_REPORT > 1
      MessageInterface::ShowMessage
         ("column=%3d, baseItem=% e, item=% e, diff=% e, maxDiff=% e\n",
          i, baseItem, baseItem, diff, maxDiffs1[i]);
      #endif
   }
   
   //------------------------------------------
   // now start compare
   //------------------------------------------
   #if DBGLVL_COMPARE_REPORT > 2
   for (int i=0; i<10; i++)
   {
      if (baseIn.eof() || in1.eof())
         break;
   #else
      while (!baseIn.eof() && !in1.eof() && !in2.eof())  
   {
   #endif

      if (numDirsToCompare == 3)
         if (in3.eof())
            break;
      
      count++;
      
      #if DBGLVL_COMPARE_REPORT > 1
      MessageInterface::ShowMessage("============================== line # = %d\n", count);
      #endif

      //----------------------------------------------------
      // base file
      //----------------------------------------------------
      baseIn.getline(buffer, BUFFER_SIZE-1);
      line = buffer;
      
      #if DBGLVL_COMPARE_REPORT > 2
      MessageInterface::ShowMessage("===> base file: buffer = %s\n", buffer);
      #endif
      
      stk.Set(line, " ");
      baseTokens = stk.GetAllTokens();

      // check for blank lines in base file
      if ((Integer)(baseTokens.size()) != baseCols)
         break;
      
      #if DBGLVL_COMPARE_REPORT > 2
      for (int i=0; i<numCols; i++)
         MessageInterface::ShowMessage("baseTokens[%d] = %s\n", i, baseTokens[i].c_str());
      #endif
      
      //----------------------------------------------------
      // file 1
      //----------------------------------------------------
      in1.getline(buffer, BUFFER_SIZE-1);
      line = buffer;
      
      #if DBGLVL_COMPARE_REPORT > 2
      MessageInterface::ShowMessage("===> file 1: buffer = %s\n", buffer);
      #endif
      
      stk.Set(line, " ");
      tokens1 = stk.GetAllTokens();
      
      // check for blank lines in file1
      if ((Integer)(tokens1.size()) != baseCols)
         break;
      
      //----------------------------------------------------
      // file 2
      //----------------------------------------------------      
      in2.getline(buffer, BUFFER_SIZE-1);
      line = buffer;
      
      #if DBGLVL_COMPARE_REPORT > 2
      MessageInterface::ShowMessage("===> file 2: buffer = %s\n", buffer);
      #endif
      
      stk.Set(line, " ");
      tokens2 = stk.GetAllTokens();
      
      // check for blank lines in file2
      if ((Integer)(tokens2.size()) != baseCols)
         break;
      
      //----------------------------------------------------
      // file 3
      //----------------------------------------------------      
      if (numDirsToCompare == 3)
      {
         in3.getline(buffer, BUFFER_SIZE-1);
         line = buffer;
      
         #if DBGLVL_COMPARE_REPORT > 2
         MessageInterface::ShowMessage("===> file 3: buffer = %s\n", buffer);
         #endif
      
         stk.Set(line, " ");
         tokens3 = stk.GetAllTokens();
      
         // check for blank lines in file2
         if ((Integer)(tokens3.size()) != baseCols)
            break;
      }
      
      #if DBGLVL_COMPARE_REPORT > 2
      for (int i=0; i<baseCols; i++)
         MessageInterface::ShowMessage("tokens1[%d] = %s\n", i, tokens1[i].c_str());
      #endif
      
      for (int i=0; i<numCols; i++)
      {
         baseItem = atof(baseTokens[i].c_str());
         item = atof(tokens1[i].c_str());
         diff = GmatMathUtil::Abs(item - baseItem);
         if (diff > maxDiffs1[i])
            maxDiffs1[i] = diff;
         
         item = atof(tokens2[i].c_str());
         diff = GmatMathUtil::Abs(item - baseItem);
         if (diff > maxDiffs2[i])
            maxDiffs2[i] = diff;
         
         if (numDirsToCompare == 3)
         {
            item = atof(tokens3[i].c_str());
            diff = GmatMathUtil::Abs(item - baseItem);
            if (diff > maxDiffs3[i])
               maxDiffs3[i] = diff;
         }
         
         #if DBGLVL_COMPARE_REPORT > 1
         MessageInterface::ShowMessage
            ("column=%3d, baseItem=% e, item=% e, diff=% e, maxDiff1=% e\n",
             i, baseItem, item, diff, maxDiffs1[i]);
         #endif
      }
      
   }
   
   // report the difference summary
   std::string outLine;
   outLine = "Total lines compared: " + ToString(count) + ",   Tolerance: " +
      ToString(tol, false, true, true, 7, 6) + "\n\n";
   textBuffer.push_back(outLine);

   #if DBGLVL_COMPARE_REPORT
   MessageInterface::ShowMessage("%s", outLine.c_str());
   #endif

   
   if (numDirsToCompare == 2)
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
   
   #if DBGLVL_COMPARE_REPORT
   MessageInterface::ShowMessage("%s", outLine.c_str());
   #endif
   
   char maxGtTol1, maxGtTol2, maxGtTol3;
   
   for (int i=0; i<numCols; i++)
   {
      maxGtTol1 = ' ';
      maxGtTol2 = ' ';
      maxGtTol3 = ' ';
      
      if (maxDiffs1[i] > tol)
         maxGtTol1 = '*';

      if (maxDiffs2[i] > tol)
         maxGtTol2 = '*';

      if (numDirsToCompare == 3)
         if (maxDiffs3[i] > tol)
            maxGtTol3 = '*';
      
      if (numDirsToCompare == 2)
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
      
      #if DBGLVL_COMPARE_REPORT
      MessageInterface::ShowMessage("%s", outLine.c_str());
      #endif
   }
   
   baseIn.close();
   in1.close();
   in2.close();
   in3.close();

   return textBuffer;
}
   
   
//------------------------------------------------------------------------------
// StringArray& Compare(Integer numDirsToCompare, const std::string &basefilename,
//                      const std::string &filename1, const std::string &filename2,
//------------------------------------------------------------------------------
StringArray& GmatFileUtil::CompareLines(Integer numDirsToCompare,
                                        const std::string &basefilename,
                                        const std::string &filename1,
                                        const std::string &filename2,
                                        const std::string &filename3,
                                        int &file1DiffCount, int &file2DiffCount,
                                        int &file3DiffCount)
{
   textBuffer.clear();
   textBuffer.push_back("\n======================================== Compare Utility\n");
   textBuffer.push_back("basefile =" + basefilename + "\n");
   
   textBuffer.push_back("filename1=" + filename1 + "\n");
   
   if (numDirsToCompare >= 2)
      textBuffer.push_back("filename2=" + filename2 + "\n");
   
   if (numDirsToCompare >= 3)
      textBuffer.push_back("filename3=" + filename3 + "\n");
   
   #if DBGLVL_COMPARE_REPORT
   MessageInterface::ShowMessage("\n======================================== Compare Utility\n");
   MessageInterface::ShowMessage("numDirsToCompare=%3\n", numDirsToCompare);
   MessageInterface::ShowMessage("basefile =%s\n", basefilename.c_str());
   MessageInterface::ShowMessage("filename1=%s\nfilename2=%s\nfilename3=%s\n",
                                 filename1.c_str(), filename2.c_str(), filename3.c_str());
   #endif
   
   // open base file
   std::ifstream baseIn(basefilename.c_str());

   // open compare files
   std::ifstream in1(filename1.c_str());
   std::ifstream in2(filename2.c_str());
   std::ifstream in3(filename3.c_str());
   
   if (!baseIn)
   {
      textBuffer.push_back("Cannot open base file: " +  basefilename + "\n");
      return textBuffer;
   }
   
   if (!in1)
   {
      textBuffer.push_back("Cannot open first file: " + filename1 + "\n");
      return textBuffer;
   }
   
   if (numDirsToCompare >= 2)
      if (!in2)
      {
         textBuffer.push_back("Cannot open second file: " + filename2 + "\n");
         return textBuffer;
      }
   
   if (numDirsToCompare >= 3)
      if (!in3)
      {
         textBuffer.push_back("Cannot open third file: " + filename3 + "\n");
         return textBuffer;
      }

   
   char buffer[BUFFER_SIZE];
   std::string line0, line1, line2, line3;
   file1DiffCount = 0;
   file2DiffCount = 0;
   file3DiffCount = 0;
   int count = 1;
   
   
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
      
      count++;
      
      #if DBGLVL_COMPARE_REPORT > 1
      MessageInterface::ShowMessage("============================== line # = %d\n", count);
      #endif
      
      //----------------------------------------------------
      // base file
      //----------------------------------------------------
      baseIn.getline(buffer, BUFFER_SIZE-1);
      line0 = buffer;
      
      #if DBGLVL_COMPARE_REPORT > 2
      MessageInterface::ShowMessage("===> base file: buffer = %s\n", buffer);
      #endif
      
      //----------------------------------------------------
      // file 1
      //----------------------------------------------------
      in1.getline(buffer, BUFFER_SIZE-1);
      line1 = buffer;
      
      #if DBGLVL_COMPARE_REPORT > 2
      MessageInterface::ShowMessage("===> file 1: buffer = %s\n", buffer);
      #endif

      if (line0 != line1)
         file1DiffCount++;
      
      //----------------------------------------------------
      // file 2
      //----------------------------------------------------      
      if (numDirsToCompare >= 2)
      {
         in2.getline(buffer, BUFFER_SIZE-1);
         line2 = buffer;
      
         #if DBGLVL_COMPARE_REPORT > 2
         MessageInterface::ShowMessage("===> file 2: buffer = %s\n", buffer);
         #endif

         if (line0 != line2)
            file2DiffCount++;
      }
      
      //----------------------------------------------------
      // file 3
      //----------------------------------------------------      
      if (numDirsToCompare >= 3)
      {
         in3.getline(buffer, BUFFER_SIZE-1);
         line3 = buffer;
      
         #if DBGLVL_COMPARE_REPORT > 2
         MessageInterface::ShowMessage("===> file 3: buffer = %s\n", buffer);
         #endif
         
         if (line0 != line3)
            file3DiffCount++;
      }
   }
   
   // report the difference summary
   std::string outLine;
   outLine = "Total lines compared: " + ToString(count) + "\n\n";
   textBuffer.push_back(outLine);

   #if DBGLVL_COMPARE_REPORT
   MessageInterface::ShowMessage("%s", outLine.c_str());
   #endif
   
   outLine = "File1 - Number of Lines different: " + ToString(file1DiffCount) + "\n";
   textBuffer.push_back(outLine);
   
   #if DBGLVL_COMPARE_REPORT
   MessageInterface::ShowMessage("%s", outLine.c_str());
   #endif
   
   if (numDirsToCompare >= 2)
   {
      outLine = "File2 - Number of Lines different: " + ToString(file2DiffCount) + "\n";
      textBuffer.push_back(outLine);
      
      #if DBGLVL_COMPARE_REPORT
      MessageInterface::ShowMessage("%s", outLine.c_str());
      #endif
   }
   
   if (numDirsToCompare >= 3)
   {
      outLine = "File3 - Number of Lines different: " + ToString(file3DiffCount) + "\n";
      textBuffer.push_back(outLine);
      
      #if DBGLVL_COMPARE_REPORT
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
// bool SkipHeaderLines(ifstream &in, StringArray &tokens)
//------------------------------------------------------------------------------
bool GmatFileUtil::SkipHeaderLines(std::ifstream &in, StringArray &tokens)
{
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
         break;
      
      in.getline(buffer, BUFFER_SIZE-1);
      line = buffer;
      
      #if DBGLVL_COMPARE_REPORT > 1
      MessageInterface::ShowMessage("file length=%d, line = %s\n",
                                    line.length(), line.c_str());
      #endif
      
      if (line.length() == 0)
         continue;
      
      alphaFound = false;
      
      for (unsigned int i=0; i<line.length(); i++)
      {
         ch = line[i];
         
         #if DBGLVL_COMPARE_REPORT > 1
         MessageInterface::ShowMessage("%c", ch);
         #endif
         
         if (!isdigit(ch) && ch != '.' && ch != 'e' && ch != 'E' && ch != '-' &&
             ch != ' ')
         {
            alphaFound = true;
            break;
         }
      }
      
      #if DBGLVL_COMPARE_REPORT > 1
      MessageInterface::ShowMessage("\n");
      #endif
      
      if (alphaFound)
         continue;

      if (line.find("--") != line.npos)
         continue;
      
      stk.Set(line, " ");
      tokens = stk.GetAllTokens();
      fileCols = tokens.size();

      colCount = 0;
      for (int i=0; i<fileCols; i++)
      {
         rval = atof(tokens[i].c_str());
         colCount++;
         
         #if DBGLVL_COMPARE_REPORT > 1
         MessageInterface::ShowMessage("rval=%f\n", rval);
         #endif
      }

      if (colCount == fileCols)
         dataFound = true;
   }

   return dataFound;
}
