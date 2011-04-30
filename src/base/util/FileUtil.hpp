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
   
   std::string GMAT_API GetPathSeparator();
   std::string GMAT_API GetCurrentPath();
   std::string GMAT_API ParseFirstPathName(const std::string &fullPath, bool appendSep = true);
   std::string GMAT_API ParsePathName(const std::string &fullPath, bool appendSep = true);
   std::string GMAT_API ParseFileName(const std::string &fullPath, bool removeExt = false);
   std::string GMAT_API ParseFileExtension(const std::string &fullPath, bool prependDot = false);
   std::string GMAT_API GetInvalidFileNameMessage(Integer option = 1);
   
   bool GMAT_API IsValidFileName(const std::string &fname, bool blankIsOk = true);
   bool GMAT_API IsSameFileName(const std::string &fname1, const std::string &fname2);
   bool GMAT_API DoesDirectoryExist(const std::string &dirPath, bool blankIsOk = true);
   bool GMAT_API DoesFileExist(const std::string &filename);
   bool GMAT_API GetLine(std::istream *inStream, std::string &line);
   bool GMAT_API IsAppInstalled(const std::string &appName, std::string &appLoc);
   
   WrapperTypeArray GMAT_API 
      GetFunctionOutputTypes(std::istream *is, const StringArray &inputs,
                             const StringArray &outputs, std::string &errMsg,
                             IntegerArray &outputRows, IntegerArray &outputCols);
   
   StringArray GMAT_API GetFileListFromDirectory(const std::string &dirName,
                                        bool addPath = false);
   StringArray GMAT_API GetTextLines(const std::string &fileName);
   
   StringArray GMAT_API &Compare(const std::string &filename1,
                        const std::string &filename2,
                        const StringArray &colTitles,
                        Real tol = CompareAbsTol);
   
   StringArray GMAT_API &Compare(Integer numDirsToCompare,
                        const std::string &basefilename,
                        const std::string &filename1,
                        const std::string &filename2,
                        const std::string &filename3,
                        const StringArray &colTitles,
                        Real tol = CompareAbsTol);
   
   StringArray GMAT_API &CompareLines(Integer numDirsToCompare,
                             const std::string &basefilename,
                             const std::string &filename1,
                             const std::string &filename2,
                             const std::string &filename3,
                             int &file1DiffCount, int &file2DiffCount,
                             int &file3DiffCount);
   
   bool GMAT_API SkipHeaderLines(std::ifstream &in, StringArray &tokens);
   
   static StringArray textBuffer;
}

#endif // FileUtil_hpp
