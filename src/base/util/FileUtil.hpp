//$Id$
//------------------------------------------------------------------------------
//                                 FileUtil
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2005/12/12
//
/**
 * This file provides methods to query file information and methods to compare
 * two output files. The compare summary is written to the log file.
 */
//------------------------------------------------------------------------------
#ifndef FileUtil_hpp
#define FileUtil_hpp

#include "gmatdefs.hpp"
#include <fstream>

namespace GmatFileUtil
{
   const Integer BUFFER_SIZE = 4096;
   static Real CompareAbsTol = 1.0e-4;
   
   std::string GetPathSeparator();
   std::string GetCurrentPath();
   std::string ParsePathName(const std::string &fullPath);
   std::string ParseFileName(const std::string &fullPath);
   std::string ParseFileExtension(const std::string &fullPath);
   bool DoesDirectoryExist(const std::string &dirPath);
   bool DoesFileExist(const std::string &filename);
   bool GetLine(std::istream *is, std::string &line);
   StringArray GetFileListFromDirectory(const std::string &dirName,
                                        bool addPath = false);
   StringArray GetTextLines(const std::string &fileName);
   
   StringArray& Compare(const std::string &filename1,
                        const std::string &filename2,
                        const StringArray &colTitles,
                        Real tol = CompareAbsTol);
   
   StringArray& Compare(Integer numDirsToCompare,
                        const std::string &basefilename,
                        const std::string &filename1,
                        const std::string &filename2,
                        const std::string &filename3,
                        const StringArray &colTitles,
                        Real tol = CompareAbsTol);
   
   StringArray& CompareLines(Integer numDirsToCompare,
                             const std::string &basefilename,
                             const std::string &filename1,
                             const std::string &filename2,
                             const std::string &filename3,
                             int &file1DiffCount, int &file2DiffCount,
                             int &file3DiffCount);
   
   bool SkipHeaderLines(std::ifstream &in, StringArray &tokens);
   
   static StringArray textBuffer;
}

#endif // FileUtil_hpp
