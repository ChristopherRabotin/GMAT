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

#ifndef SCRIPTREADWRITER_HPP_
#define SCRIPTREADWRITER_HPP_

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include <iostream>
// #include <fstream>

class GMAT_API ScriptReadWriter 
{
public:
   static ScriptReadWriter* Instance();
   ~ScriptReadWriter();

   void SetInStream(std::istream *is) { 
   	  inStream = is; 
      reachedEndOfFile = false;
      readFirstBlock = false;  }
   void SetOutStream(std::ostream *os) { outStream = os; }
   
//    std::string GetScriptFilename();
//    void SetScriptFilename(std::string fName);
   
   Integer GetLineWidth();
   void SetLineWidth(Integer width);
   
   Integer GetLineNumber();
   
//    bool OpenScriptFile(bool readMode);
   
   std::string ReadLogicalBlock();
   bool WriteText(const std::string &textToWrite);
   
//    bool CloseScriptFile();
   
protected:

private:

   // These data are not created here
   std::istream *inStream;
   std::ostream *outStream;
   
   //std::string fileName;
   //std::fstream fileStream;
   Integer lineWidth;
   Integer currentLineNumber;
   bool writeGmatKeyword;
   bool reachedEndOfFile;
   bool readFirstBlock;

   bool Initialize();
   //std::string CrossPlatformGetLine(std::istream& in);
   std::string CrossPlatformGetLine();
   bool IsComment(const std::string &text);
   bool IsBlank(const std::string &text);
   bool HasEllipse(const std::string &text);
   std::string HandleEllipsis(const std::string &text);
   std::string HandleComments(const std::string &text);
   std::string HandleFirstBlock(const std::string &text); 
   
   static ScriptReadWriter *instance;   
   static const std::string sectionDelimiter;
   static const std::string ellipsis;
};

#endif /*SCRIPTREADWRITER_HPP_*/
