//$Header$
//------------------------------------------------------------------------------
//                           ScriptReadWriter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2006/07/10
//
/**
 * Implements reading and writing a script file.
 */
//------------------------------------------------------------------------------

#include "ScriptReadWriter.hpp"
#include "InterpreterException.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"
#include <string.h>
#include <sstream>

ScriptReadWriter* ScriptReadWriter::instance = NULL;
const std::string ScriptReadWriter::sectionDelimiter = "%--------";
const std::string ScriptReadWriter::ellipsis = "...";

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// static Instance()
//------------------------------------------------------------------------------
ScriptReadWriter* ScriptReadWriter::Instance()
{
   if (instance == NULL)
   {
      instance = new ScriptReadWriter();
      instance->Initialize();
   }
   
   return instance;
}

//------------------------------------------------------------------------------
// ~ScriptReadWriter()
//------------------------------------------------------------------------------
ScriptReadWriter::~ScriptReadWriter()
{
//    fileStream.flush();
//    fileStream.close();
}

// //------------------------------------------------------------------------------
// // std::string GetScriptFilename()
// //------------------------------------------------------------------------------
// std::string ScriptReadWriter::GetScriptFilename()
// {
//    return fileName;
// }

// //------------------------------------------------------------------------------
// // void SetScriptFilename(std::string fileName)
// //------------------------------------------------------------------------------
// void ScriptReadWriter::SetScriptFilename(std::string fName)
// {
//    fileName = fName;
// }
   
//------------------------------------------------------------------------------
// Integer GetLineWidth()
//------------------------------------------------------------------------------
Integer ScriptReadWriter::GetLineWidth()
{
   return lineWidth;
}

//------------------------------------------------------------------------------
// void SetLineWidth(Integer lineWidth)
//------------------------------------------------------------------------------
void ScriptReadWriter::SetLineWidth(Integer width)
{
   if ((width < 20) && (width != 0))
      throw InterpreterException(
            "Line width must either be unlimited (denoted by 0) or greater than 19 characters.\n");
   
   lineWidth = width;
}

//------------------------------------------------------------------------------
// Integer GetLineNumber()
//------------------------------------------------------------------------------
Integer ScriptReadWriter::GetLineNumber()
{
   return currentLineNumber;
}

// //------------------------------------------------------------------------------
// // bool OpenScriptFile(bool readMode)
// //------------------------------------------------------------------------------
// bool ScriptReadWriter::OpenScriptFile(bool readMode)
// {
//    currentLineNumber = 0;
//    reachedEndOfFile = false;
//    readFirstBlock = false;
  
//    // just to make sure the old one was closed out
//    fileStream.flush();
//    fileStream.close();  
//    fileStream.clear();
   
//    if (fileName == "")
//       return false;
      
//    if (readMode)
//       fileStream.open(fileName.c_str(), std::fstream::in);
//    else
//       fileStream.open(fileName.c_str(), std::fstream::out);

//    return fileStream.is_open();
// }

//------------------------------------------------------------------------------
// std::string ReadLogicalBlock()
//------------------------------------------------------------------------------
std::string ScriptReadWriter::ReadLogicalBlock()
{
   std::string result = "";
   std::string oneLine = ""; 
   
   //if (inStream->is_open())
   //{
      if (reachedEndOfFile)
         return "\0";
      
      // get 1 line of text
      //loj: oneLine = CrossPlatformGetLine(fileStream);
      oneLine = CrossPlatformGetLine();
      
      if (reachedEndOfFile && IsBlank(oneLine))
         return "\0";
      
      // keep looping till we find text or end of file
      // oneLine should be overwritten each time
      while ((!reachedEndOfFile) && (IsBlank(oneLine)))
         oneLine = CrossPlatformGetLine();
      
      // if the last lines of the file were blank
      // return eof string
      if (reachedEndOfFile && IsBlank(oneLine))
          return "\0";
            
      // we know we don't have any blank lines
      // now, so lets set result to the line read in.
      // After, we will check for comments/delimiters and ellipses
      result = oneLine;
      
      if (IsComment(oneLine))
      {
         // overwrite result with multiple lines of comments
         result = HandleComments(oneLine);
      }
      else if (HasEllipse(oneLine))
      {
         // overwrite result with mulitple lines separated 
         // by ellipsis
         result = HandleEllipsis(oneLine);
      }

      readFirstBlock = true;
   //}
   //else
   //   throw InterpreterException("File is not open for reading.\n");
   
   return result;
}

//------------------------------------------------------------------------------
// bool WriteText(const std::string &textToWrite)
//------------------------------------------------------------------------------
bool ScriptReadWriter::WriteText(const std::string &textToWrite)
{
   //if (!outStream->is_open())
   //   throw InterpreterException("File is not open for writing.\n");
   
   *outStream << textToWrite;
   outStream->flush();
   
//    if (!fileStream.is_open())
//       throw InterpreterException("File is not open for writing.\n");
   
//    //return fileStream << textToWrite;
//    fileStream << textToWrite;
//    fileStream.flush();
   return true;
}

// //------------------------------------------------------------------------------
// // bool CloseScriptFile()
// //------------------------------------------------------------------------------
// bool ScriptReadWriter::CloseScriptFile()
// {
//    fileStream.flush();
//    fileStream.close();  
//    fileStream.clear();
//    return (!fileStream.is_open());
//    return true;
// }

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool ScriptReadWriter::Initialize()
{
//    fileName = "";
   lineWidth = 80;
   currentLineNumber = 0;
   writeGmatKeyword = true;
   reachedEndOfFile = false;
   readFirstBlock = false;
   
   return true;  // need to change so if something wasn't set to return false
}

// //------------------------------------------------------------------------------
// // std::string CrossPlatformGetLine(std::istream& in)
// //------------------------------------------------------------------------------
// std::string ScriptReadWriter::CrossPlatformGetLine(std::istream& in)
// {
//    char ch;
//    std::string result;
   
//    while (in.get(ch) && ch != '\r' && ch != '\n'
//                      && ch != '\0' && !in.eof()) 
// //                     && ch != '\r\n' && ch != '\n\r')
//       result += ch;

//    if ((ch == '\0') || (in.eof()))
//    {
//       reachedEndOfFile = true;
//    }
      
//    ++currentLineNumber;
//    return result;
// }


//------------------------------------------------------------------------------
// std::string CrossPlatformGetLine()
//------------------------------------------------------------------------------
std::string ScriptReadWriter::CrossPlatformGetLine()
{
   char ch;
   std::string result;
   
   while (inStream->get(ch) && ch != '\r' && ch != '\n'
          && ch != '\0' && !inStream->eof()) 
      result += ch;
   
   if ((ch == '\0') || (inStream->eof()))
   {
      reachedEndOfFile = true;
   }
   
   ++currentLineNumber;
   return result;
}


//------------------------------------------------------------------------------
// bool IsComment(const std::string &text)
//------------------------------------------------------------------------------
bool ScriptReadWriter::IsComment(const std::string &text)
{
   std::string str = GmatStringUtil::Trim(text, GmatStringUtil::BOTH);
   return GmatStringUtil::StartsWith(str, "%");
}

//------------------------------------------------------------------------------
// bool IsBlank(const std::string &text)
//------------------------------------------------------------------------------
bool ScriptReadWriter::IsBlank(const std::string &text)
{
   std::string str = GmatStringUtil::Trim(text, GmatStringUtil::BOTH);
   
   if (str == "")
      return true;
   else
      return false;
}

//------------------------------------------------------------------------------
// bool HasEllipse(const std::string &text)
//------------------------------------------------------------------------------
bool ScriptReadWriter::HasEllipse(const std::string &text)
{
   std::string ellipsis = "...";

   int pos = text.find(ellipsis,0);
   
   if (pos < 0)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// std::string HandleEllipsis(const std::string &text)
//------------------------------------------------------------------------------
std::string ScriptReadWriter::HandleEllipsis(const std::string &text)
{
   std::string str = GmatStringUtil::Trim(text, GmatStringUtil::TRAILING);
   int pos = str.find(ellipsis,0);
   
   if (pos < 0)      // no ellipsis
      return str;
   
   // make sure ellipsis is at the end of the line
   if ((int)(str.size())-3 != pos)
   {
      std::stringstream buffer;
      buffer << currentLineNumber;
      throw InterpreterException("Script Line " +buffer.str() 
                                 +"-->Ellipses must be at the end of the line\n" );
   }
   
   std::string result = "";
     
   while (pos >= 0)
   {
      if (pos == 0)     // ellipsis were on a line by themselves
        result += " ";
      else
      {
         result += str.substr(0, pos);  // add substring to first set
         result += " ";
      }
         
      // reset str string and position
      str = "";
      pos = -1;
         
      //loj: str = CrossPlatformGetLine(fileStream);
      str = CrossPlatformGetLine();
       
      while (IsBlank(str) && !reachedEndOfFile)
         //loj: str = CrossPlatformGetLine(fileStream);
         str = CrossPlatformGetLine();
         
      if (IsBlank(str) && reachedEndOfFile)
      {
         std::stringstream buffer;
             buffer << currentLineNumber;
         throw InterpreterException("Script Line " +buffer.str() 
           +"-->Prematurely reached the end of file.\n");
      }
        
      if (IsComment(str))
      {
             std::stringstream buffer;
             buffer << currentLineNumber;
         throw InterpreterException("Script Line " +buffer.str() 
           +"-->Comments are not allowed in the middle of a block\n");
      }
      
      str = GmatStringUtil::Trim(str, GmatStringUtil::TRAILING);      
      pos = str.find(ellipsis, 0);
   }
      
   // add the last line on to result
   if (IsComment(str))
   {
          std::stringstream buffer;
          buffer << currentLineNumber;
      throw InterpreterException("Script Line " +buffer.str()
         +"-->Comments are not allowed in the middle of a block\n");
   }
      
   result += str;
   return result;
}

//------------------------------------------------------------------------------
// std::string HandleComments(const std::string &text)
//------------------------------------------------------------------------------
std::string ScriptReadWriter::HandleComments(const std::string &text)
{
   if (!readFirstBlock)
      return HandleFirstBlock(text);    
        
   std::string result = text +"\n";
   
   if (GmatStringUtil::StartsWith(GmatStringUtil::Trim(text, GmatStringUtil::BOTH),
       sectionDelimiter))
      result = "";   // overwrite result
   
   //loj: std::string newLine = CrossPlatformGetLine(fileStream);
   std::string newLine = CrossPlatformGetLine();
          
   while (((IsComment(newLine)) || (IsBlank(newLine))) &&
          (!reachedEndOfFile))
   {
      // only add comment line in, if it doesn't start with section delimiter
      if ((!GmatStringUtil::StartsWith(GmatStringUtil::Trim(newLine, GmatStringUtil::BOTH), 
                                       sectionDelimiter)) && (!IsBlank(newLine)))
         result += (newLine + "\n");
      
      //newLine = CrossPlatformGetLine(fileStream);
      newLine = CrossPlatformGetLine();
   }
   
   if (HasEllipse(newLine))
      newLine = HandleEllipsis(newLine);
      
   if (!IsBlank(newLine))
      result += newLine;
         
   return result;
}

//------------------------------------------------------------------------------
// std::string HandleFirstBlock()
//------------------------------------------------------------------------------
std::string ScriptReadWriter::HandleFirstBlock(const std::string &text)
{
   std::string result = text +"\n";
    
   // first block is a section delimiter, so must end with script line
   if (GmatStringUtil::StartsWith(GmatStringUtil::Trim(text, GmatStringUtil::BOTH), 
       sectionDelimiter))
      result = "";   // overwrite result

   //loj: std::string newLine = CrossPlatformGetLine(fileStream);
   std::string newLine = CrossPlatformGetLine();

   while (!IsBlank(newLine))
   {
      if (!IsComment(newLine))   // not a comment header
      {
         if (HasEllipse(newLine))
            newLine = HandleEllipsis(newLine);
                 
         return result += newLine;
      }
                         
      // only add comment line in, if it doesn't start with section delimiter
      if (!GmatStringUtil::StartsWith(GmatStringUtil::Trim(newLine, GmatStringUtil::BOTH),
                                      sectionDelimiter))
         result += (newLine + "\n");
      
      //newLine = CrossPlatformGetLine(fileStream);
      newLine = CrossPlatformGetLine();
   }
   
   if (HasEllipse(newLine))
      newLine = HandleEllipsis(newLine);
     
   if (!IsBlank(newLine))
      result += newLine;
   
   if (result == "")   // might be blank if section delimiter has a space after
      return ReadLogicalBlock();
     
   return result;
}

