//$Id$
//------------------------------------------------------------------------------
//                           ScriptReadWriter
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

class GMAT_API ScriptReadWriter 
{
public:
   static ScriptReadWriter* Instance();
   ~ScriptReadWriter();
   
   void SetInStream(std::istream *is);
   void SetOutStream(std::ostream *os) { outStream = os; }
   
   Integer GetLineWidth();
   void SetLineWidth(Integer width);
   
   Integer GetLineNumber();
   std::string GetCurrentLine() { return currentLine; }
   
   void ReadFirstBlock(std::string &header, std::string &firstBlock,
                       bool skipHeader = false);
   std::string ReadLogicalBlock();
   bool WriteText(const std::string &textToWrite);
   
protected:

private:

   // These data are not created here
   std::istream *inStream;
   std::ostream *outStream;
   
   std::string currentLine;
   
   Integer lineWidth;
   Integer currentLineNumber;
   bool writeGmatKeyword;
   bool reachedEndOfFile;
   bool readFirstBlock;
   
   bool Initialize();
   std::string CrossPlatformGetLine();
   bool IsComment(const std::string &text);
   bool IsBlank(const std::string &text);
   bool HasEllipse(const std::string &text);
   std::string HandleEllipsis(const std::string &text);
   std::string HandleComments(const std::string &text);
   
   static ScriptReadWriter *instance;   
   static const std::string sectionDelimiter;
   static const std::string ellipsis;
};

#endif /*SCRIPTREADWRITER_HPP_*/
