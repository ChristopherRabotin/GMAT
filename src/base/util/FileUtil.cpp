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
#include "RealUtilities.hpp"       // for GmatMathUtil::

using namespace std;

//#define DEBUG_COMPARE_REPORT 1

//------------------------------------------------------------------------------
// void Compare(const std::string &filename1, const std::string &filename2,
//              Real tol = 1.0e-5))
//------------------------------------------------------------------------------
void GmatFileUtil::Compare(const std::string &filename1,
                           const std::string &filename2, Real tol)
{
   
   MessageInterface::ShowMessage("\n======================================== Compare Utility\n");
   MessageInterface::ShowMessage("filename1=%s\n", filename1.c_str());
   MessageInterface::ShowMessage("filename2=%s\n", filename2.c_str());
   
   // open file
   ifstream in1(filename1.c_str());
   ifstream in2(filename2.c_str());
   
   if (!in1)
   {
      MessageInterface::ShowMessage("Cannot open first file: %s. \n", filename1.c_str());
      return;
   }
   
   if (!in2)
   {
      MessageInterface::ShowMessage("Cannot open second file: %s. \n", filename2.c_str());
      return;
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
      MessageInterface::ShowMessage("Cannot compare files: Data record not found on file 1.");
      return;
   }
   
   if (!GmatFileUtil::SkipHeaderLines(in2, tokens2))
   {
      MessageInterface::ShowMessage("Cannot compare files: Data record not found on file 2.");
      return;
   }
   
   //------------------------------------------
   // check number of columns
   //------------------------------------------
   file1Cols = tokens1.size();
   file2Cols = tokens2.size();
   
   if (file1Cols != file2Cols)
   {
      MessageInterface::ShowMessage
         ("OutputTree::OnCompareReport() Cannot compare outputs. "
          "Number of colmuns are different.\n   file1=%d, file2=%d\n",
          file1Cols, file2Cols);
      return;
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
   #if DEBUG_COMPARE_REPORT
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
   MessageInterface::ShowMessage
       ("Total lines compared: %d    Tolerance: %e\n\n"
       "Column \tMinimum Diff.  Line# \tMaximum Diff.  Line#  Min>Tol  Max>Tol\n"
       "------ \t-------------  ----- \t-------------  -----  -------  -------\n",
        count, tol);
   
   char minGtTol, maxGtTol;
   
   for (int i=0; i<file1Cols; i++)
   {
      minGtTol = ' ';
      maxGtTol = ' ';
      
      if (minDiffs[i] > tol)
         minGtTol = '*';
      
      if (maxDiffs[i] > tol)
         maxGtTol = '*';
      
      MessageInterface::ShowMessage
         ("%3d \t%e  %3d \t%e  %3d       %c        %c\n",
          i+1, minDiffs[i], minLines[i], maxDiffs[i], maxLines[i], minGtTol,
          maxGtTol);
   }
   
   MessageInterface::ShowMessage("\n");
   
   in1.close();
   in2.close();
   
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
