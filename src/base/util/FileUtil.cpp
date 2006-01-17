//$Header$
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
 * This file provides methods to compare two output files. The compare summary
 * is written to the log file.
 */
//------------------------------------------------------------------------------

#include "FileUtil.hpp"
#include "StringTokenizer.hpp"
#include "MessageInterface.hpp"
#include "RealUtilities.hpp"       // for Abs()
#include "Linear.hpp"              // for ToString()

using namespace std;
using namespace GmatRealUtil;

//#define DEBUG_COMPARE_REPORT 1

//------------------------------------------------------------------------------
// StringArray& Compare(const std::string &filename1, const std::string &filename2,
//                      Real tol = 1.0e-4)
//------------------------------------------------------------------------------
StringArray& GmatFileUtil::Compare(const std::string &filename1,
                                   const std::string &filename2, Real tol)
{
   textBuffer.clear();
   textBuffer.push_back("\n======================================== Compare Utility\n");
   textBuffer.push_back("filename1=" + filename1 + "\n");
   textBuffer.push_back("filename2=" + filename2 + "\n");

   #if DEBUG_COMPARE_REPORT
   MessageInterface::ShowMessage("\n======================================== Compare Utility\n");
   MessageInterface::ShowMessage("filename1=%s\n");
   MessageInterface::ShowMessage("filename2=%s\n");
   #endif
   
   // open file
   ifstream in1(filename1.c_str());
   ifstream in2(filename2.c_str());
   
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
   
   if (file1Cols != file2Cols)
   {
      textBuffer.push_back
         ("***Cannot compare outputs. Number of colmuns are different. file1: " +
          ToString(file1Cols) + ",  file2: " + ToString(file2Cols) + "\n");
      return textBuffer;
   }

   // compare first data line
   RealArray minDiffs, maxDiffs;
   IntegerArray minLines, maxLines;
   
   for (int i=0; i<file1Cols; i++)
   {
      item1 = atof(tokens1[i].c_str());
      item2 = atof(tokens2[i].c_str());
      diff = GmatMathUtil::Abs(item1 - item2);
      minDiffs.push_back(diff);
      maxDiffs.push_back(diff);
      minLines.push_back(1);
      maxLines.push_back(1);
      
      #if DEBUG_COMPARE_REPORT > 1
      MessageInterface::ShowMessage
         ("column=%3d, item1=% e, item2=% e, diff=% e, minDiff=% e, maxDiff=% e\n",
          i, item1, item1, diff, minDiffs[i], maxDiffs[i]);
      #endif
   }
   
   //------------------------------------------
   // now start compare
   //------------------------------------------
   #if DEBUG_COMPARE_REPORT > 2
   for (int i=0; i<10; i++)
   {
      if (in1.eof() || in2.eof())
         break;
   #else
   while (!in1.eof() && !in2.eof())
   {
   #endif

      count++;
      
      #if DEBUG_COMPARE_REPORT > 1
      MessageInterface::ShowMessage("============================== line # = %d\n", count);
      #endif
      
      // file 1
      in1.getline(buffer, BUFFER_SIZE-1);
      line = buffer;
      
      #if DEBUG_COMPARE_REPORT > 2
      MessageInterface::ShowMessage("===> file 1: buffer = %s\n", buffer);
      #endif
      
      stk.Set(line);
      tokens1 = stk.GetAllTokens();

      // check for blank lines in file1
      if ((Integer)(tokens1.size()) != file1Cols)
         break;
      
      #if DEBUG_COMPARE_REPORT > 2
      for (int i=0; i<file1Cols; i++)
         MessageInterface::ShowMessage("tokens1[%d] = %s\n", i, tokens1[i].c_str());
      #endif
      
      // file 2
      in2.getline(buffer, BUFFER_SIZE-1);
      line = buffer;
      
      #if DEBUG_COMPARE_REPORT > 2
      MessageInterface::ShowMessage("===> file 2: buffer = %s\n", buffer);
      #endif
      
      stk.Set(line);
      tokens2 = stk.GetAllTokens();
      
      // check for blank lines in file1
      if ((Integer)(tokens2.size()) != file1Cols)
         break;
      
      #if DEBUG_COMPARE_REPORT > 2
      for (int i=0; i<file1Cols; i++)
         MessageInterface::ShowMessage("tokens2[%d] = %s\n", i, tokens2[i].c_str());
      #endif
      
      for (int i=0; i<file1Cols; i++)
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
            
         #if DEBUG_COMPARE_REPORT > 1
         MessageInterface::ShowMessage
            ("column=%3d, item1=% e, item2=% e, diff=% e, minDiff=% e, maxDiff=% e\n",
             i, item1, item2, diff, minDiffs[i], maxDiffs[i]);
         #endif
      }
         
   }
   
   // report the difference summary
   std::string outLine;
   outLine = "Total lines compared: " + ToString(count) + ",   Tolerance: " +
      ToString(tol, true, 7, 6) + "\n\n";
   textBuffer.push_back(outLine);

   #if DEBUG_COMPARE_REPORT
   MessageInterface::ShowMessage("%s", outLine.c_str());
   #endif

   outLine = "Column   Minimum Diff.   Line#   Maximum Diff.   Line#   Min>Tol   Max>Tol\n"
      "------   -------------   -----   -------------   -----   -------   -------\n";
   
   textBuffer.push_back(outLine);
   
   #if DEBUG_COMPARE_REPORT
   MessageInterface::ShowMessage("%s", outLine.c_str());
   #endif
   
   char minGtTol, maxGtTol;
   
   for (int i=0; i<file1Cols; i++)
   {
      minGtTol = ' ';
      maxGtTol = ' ';
      
      if (minDiffs[i] > tol)
         minGtTol = '*';
      
      if (maxDiffs[i] > tol)
         maxGtTol = '*';

      outLine = ToString(i+1) + "     " + ToString(minDiffs[i], true, 7, 6) +
         "   " + ToString(minLines[i]) + "    " +
         ToString(maxDiffs[i], true, 7, 6) + "   " + ToString(maxLines[i]) +
         "       " + minGtTol + "         " + maxGtTol + "\n";
      
      textBuffer.push_back(outLine);
      
      #if DEBUG_COMPARE_REPORT
      MessageInterface::ShowMessage("%s", outLine.c_str());
      #endif
   }
   
   //textBuffer.push_back("\n");
   
   in1.close();
   in2.close();

   return textBuffer;
}
   
   
//------------------------------------------------------------------------------
// bool SkipHeaderLines(ifstream &in, StringArray &tokens)
//------------------------------------------------------------------------------
bool GmatFileUtil::SkipHeaderLines(ifstream &in, StringArray &tokens)
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
      
      #if DEBUG_COMPARE_REPORT > 1
      MessageInterface::ShowMessage("file length=%d, line = %s\n",
                                    line.length(), line.c_str());
      #endif
      
      if (line.length() == 0)
         continue;
      
      alphaFound = false;
      
      for (unsigned int i=0; i<line.length(); i++)
      {
         ch = line[i];
         
         #if DEBUG_COMPARE_REPORT > 1
         MessageInterface::ShowMessage("%c", ch);
         #endif
         
         if (!isdigit(ch) && ch != '.' && ch != 'e' && ch != 'E' && ch != '-' &&
             ch != ' ')
         {
            alphaFound = true;
            break;
         }
      }
      
      #if DEBUG_COMPARE_REPORT > 1
      MessageInterface::ShowMessage("\n");
      #endif
      
      if (alphaFound)
         continue;

      if (line.find("--") != line.npos)
         continue;
      
      stk.Set(line);
      tokens = stk.GetAllTokens();
      fileCols = tokens.size();

      colCount = 0;
      for (int i=0; i<fileCols; i++)
      {
         rval = atof(tokens[i].c_str());
         colCount++;
         
         #if DEBUG_COMPARE_REPORT > 1
         MessageInterface::ShowMessage("rval=%f\n", rval);
         #endif
      }

      if (colCount == fileCols)
         dataFound = true;
   }

   return dataFound;
}
