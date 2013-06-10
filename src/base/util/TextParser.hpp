//$Id$
//------------------------------------------------------------------------------
//                              TextParser
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
// Created: 2006/08/10
//
/**
 * Declares the methods to parse text into parts.
 */
//------------------------------------------------------------------------------
#ifndef TextParser_hpp
#define TextParser_hpp

#include "gmatdefs.hpp"

namespace Gmat
{
   enum BlockType
   {
      COMMENT_BLOCK,
      DEFINITION_BLOCK,
      COMMAND_BLOCK,
      ASSIGNMENT_BLOCK,
      FUNCTION_BLOCK,
   };
};


class GMAT_API TextParser
{
public:

   TextParser();
   ~TextParser();

   // inline methods
   std::string GetPrefaceComment() { return prefaceComment; }
   std::string GetInlineComment() { return inlineComment; }
   std::string GetInstruction() { return theInstruction; }
   
   void Initialize(const StringArray &commandList);
   StringArray& GetChunks() { return theChunks; }
   bool IsFunctionCall() { return isFunctionCall; }
   
   void Reset();
   
   // for parsing
   Gmat::BlockType EvaluateBlock(const std::string &logicalBlock);
   StringArray DecomposeBlock(const std::string &logicalBlock);
   StringArray ChunkLine();
   
   StringArray Decompose(const std::string &chunk,
                         const std::string &bracketPair,
                         bool checkForArray = true,
                         bool removeOuterBracket = false);
   StringArray SeparateBrackets(const std::string &chunk,
                                const std::string &bracketPair,
                                const std::string &delim,
                                bool checkOuterBracket = true);
   StringArray SeparateAllBrackets(const std::string &chunk,
                                   const std::string &bracketPair);
   StringArray SeparateSpaces(const std::string &chunk);
   StringArray SeparateDots(const std::string &chunk);
   StringArray SeparateBy(const std::string &chunk, const std::string &delim);
   
   
protected:
      
   bool IsCommand(const std::string &str);
   char GetClosingBracket(const char &openBracket);
   std::string TabToSpaceExceptStrings(const std::string& startingBlock);
   
private:
   
   std::string prefaceComment;
   std::string inlineComment;
   std::string theInstruction;
   std::string whiteSpace;
   StringArray theChunks;
   StringArray theCommandList;
   Gmat::BlockType theBlockType;
   bool isFunctionCall;
   char errorMsg[1024];
};


#endif // TextParser_hpp



