//$Id$
//------------------------------------------------------------------------------
//                               ScriptInterpreter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Waka Waktola
// Created: 2006/08/25
// Copyright: (c) 2006 NASA/GSFC. All rights reserved.
//
//------------------------------------------------------------------------------
/**
 * Class implementation for the ScriptInterpreter
 */
//------------------------------------------------------------------------------

#include "ScriptInterpreter.hpp"
#include "MessageInterface.hpp"
#include "Moderator.hpp"
#include "MathParser.hpp"
#include "NoOp.hpp"
#include "CommandUtil.hpp"     // for GmatCommandUtil::GetCommandSeqString()
#include "StringUtil.hpp"      // for GmatStringUtil::
#include "TimeTypes.hpp"       // for GmatTimeUtil::FormatCurrentTime()

#include <sstream>      // For stringstream, used to check for non-ASCII chars

// to allow object creation in command mode, such as inside ScriptEvent
//#define __ALLOW_OBJECT_CREATION_IN_COMMAND_MODE__


//#define DEBUG_READ_FIRST_PASS
//#define DEBUG_DELAYED_BLOCK
//#define DEBUG_PARSE
//#define DEBUG_PARSE_ASSIGNMENT
//#define DEBUG_PARSE_FOOTER
//#define DEBUG_SET_COMMENTS
//#define DEBUG_SCRIPT_WRITING
//#define DEBUG_SCRIPT_WRITING_PARAMETER
//#define DEBUG_SECTION_DELIMITER
//#define DEBUG_SCRIPT_WRITING_COMMANDS
//#define DBGLVL_SCRIPT_READING 1
//#define DBGLVL_GMAT_FUNCTION 1
//#define DEBUG_COMMAND_MODE_TOGGLE


//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

ScriptInterpreter *ScriptInterpreter::instance = NULL;

//------------------------------------------------------------------------------
// ScriptInterpreter* Instance()
//------------------------------------------------------------------------------
/**
 * Accessor for the ScriptInterpreter singleton.
 * 
 * @return Pointer to the singleton.
 */
//------------------------------------------------------------------------------
ScriptInterpreter* ScriptInterpreter::Instance()
{
   if (!instance)
      instance = new ScriptInterpreter();
   return instance;
}


//------------------------------------------------------------------------------
// ScriptInterpreter()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
ScriptInterpreter::ScriptInterpreter() : Interpreter()
{
   logicalBlockCount = 0;
   functionDefined = false;
   ignoreRest = false;
   
   functionDef      = "";
   functionFilename = "";
   scriptFilename   = "";
   currentBlock     = "";
   headerComment    = "";
   footerComment    = "";
   
   inCommandMode = false;
   inRealCommandMode = false;
   firstTimeCommandMode = true;
   
   // Initialize the section delimiter comment
   sectionDelimiterString.clear();
   userParameterLines.clear();
   sectionDelimiterString.push_back("\n%----------------------------------------\n");
   sectionDelimiterString.push_back("%---------- ");
   sectionDelimiterString.push_back("\n%----------------------------------------\n");
   
   Initialize();
}


//------------------------------------------------------------------------------
// ~ScriptInterpreter()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ScriptInterpreter::~ScriptInterpreter()
{
}


//------------------------------------------------------------------------------
// bool Interpret()
//------------------------------------------------------------------------------
/**
 * Parses the input stream, line by line, into GMAT objects.
 * 
 * @return true if the stream parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::Interpret()
{
   #if DBGLVL_SCRIPT_READING
   MessageInterface::ShowMessage
      ("ScriptInterpreter::Interpret() entered, Calling Initialize()\n");
   #endif
   
   Initialize();
   
   StringArray defaultCSNames;
   
   inCommandMode = false;
   inRealCommandMode = false;
   firstTimeCommandMode = true;
   beginMissionSeqFound = false;
   userParameterLines.clear();
   
   // Grab the built-in coordinate system list; mark them as Do Not Modify while
   // interpreting the script
   defaultCSNames = theModerator->GetDefaultCoordinateSystemNames();
   for (StringArray::iterator i = defaultCSNames.begin(); i != defaultCSNames.end(); ++i)
   {
      CoordinateSystem *cs = (CoordinateSystem*)FindObject(*i);
      cs->SetModifyFlag(false);
   }

   // Before parsing script, check for unmatching control logic
   #if DBGLVL_SCRIPT_READING
   MessageInterface::ShowMessage("   Calling ReadFirstPass()\n");
   #endif

   bool retval0 = ReadFirstPass();
   bool retval1 = false;
   bool retval2 = false;

   if (retval0)
   {
      retval1 = ReadScript();
      if (retval1)
         retval2 = FinalPass();
      
      #if DBGLVL_SCRIPT_READING
         MessageInterface::ShowMessage("In ScriptInterpreter::Interpret(), "
               "retvals are rv1 = %s and rv2 = %s; numErrors = %d\n",
               retval1 ? "true" : "false", retval2 ? "true" : "false",
               errorList.size());
      #endif
   }

   // Mark the built-in coordinate systems as modifiable again
   for (StringArray::iterator i = defaultCSNames.begin(); i != defaultCSNames.end(); ++i)
   {
      CoordinateSystem *cs = (CoordinateSystem*)FindObject(*i);
      cs->SetModifyFlag(true);
   }

   // Write any error messages collected
   std::string errorMsg;
   std::string truncMsg = " .......... Message is too long so truncated.\n";
   Integer truncLength = truncMsg.size();
   Integer numErrors = errorList.size();
   Integer errorLength;
   #if DBGLVL_SCRIPT_READING
   MessageInterface::ShowMessage
      ("ScriptInterpreter::Interpret() There are %d errors\n", numErrors);
   #endif
   for (Integer i = 0; i < numErrors; i++)
   {
      errorMsg = errorList[i];
      errorLength = errorMsg.size();
      // If message is too long truncate it
      if (errorLength > MessageInterface::MAX_MESSAGE_LENGTH)
      {
         errorMsg = errorMsg.substr(0, MessageInterface::MAX_MESSAGE_LENGTH - truncLength);
         errorMsg = errorMsg + truncMsg;
      }
      MessageInterface::ShowMessage("%d: %s\n", i+1, errorMsg.c_str());
   }
   
   #if DBGLVL_SCRIPT_READING
   MessageInterface::ShowMessage
      ("ScriptInterpreter::Interpret() Leaving retval1=%d, retval2=%d\n",
       retval1, retval2);
   #endif
   
   return (retval1 && retval2);
}


//------------------------------------------------------------------------------
// bool Interpret(GmatCommand *inCmd, bool skipHeader, bool functionMode)
//------------------------------------------------------------------------------
/**
 * Parses and creates commands from input stream and append to input command.
 *
 * @param  inCmd  Command which appended to
 * @param  skipHeader Flag indicating first comment block is not a header (false)
 * @param  functionMode Flag indicating function mode interpretation (false)
 * @return true if the stream parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::Interpret(GmatCommand *inCmd, bool skipHeader,
                                  bool functionMode)
{
   Initialize();
   
   #if DBGLVL_SCRIPT_READING
   MessageInterface::ShowMessage
      ("ScriptInterpreter::Interpret(%p) Entered inCmd=%s, skipHeader=%d, "
       "functionMode=%d\n", inCmd, inCmd->GetTypeName().c_str(), skipHeader,
       functionMode);
   #endif
   
   #ifdef DEBUG_COMMAND_MODE_TOGGLE
      MessageInterface::ShowMessage("Line %s set inCommandMode to true\n",
            inCmd->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif

   // Since this method is called from ScriptEvent or InterpretGmatFunction,
   // set command mode to true
   inFunctionMode = functionMode;
   inCommandMode = true;
   inRealCommandMode = true;
   beginMissionSeqFound = true;
   functionDefined = false;
   ignoreRest = false;
   
   // Before parsing script, check for unmatching control logic
   bool retval0 = ReadFirstPass();
   bool retval1 = false;
   bool retval2 = false;
   
   if (retval0)
   {
      retval1 = ReadScript(inCmd, skipHeader);

      // Call FinalPass() if not in function mode or creating command inside a ScriptEvent
      // Added to check for inCmd != NULL for Bug 2436 fix (LOJ: 2011.07.05)
      if (inFunctionMode || inCmd != NULL)
         retval2 = true;
      else if (inCmd == NULL)
      {
         #if DBGLVL_SCRIPT_READING
         MessageInterface::ShowMessage("   Callilng FinalPass()\n");
         #endif
         retval2 = FinalPass();
      }
   }
   
   // Write any error messages collected
   for (UnsignedInt i=0; i<errorList.size(); i++)
      MessageInterface::ShowMessage("%d: %s\n", i+1, errorList[i].c_str());
   
   #if DBGLVL_SCRIPT_READING
   MessageInterface::ShowMessage
      ("ScriptInterpreter::Interpret(GmatCommand) Leaving retval1=%d, retval2=%d\n",
       retval1, retval2);
   #endif
   
   return (retval1 && retval2);
}


//------------------------------------------------------------------------------
// bool Interpret(const std::string &scriptfile)
//------------------------------------------------------------------------------
/**
 * Parses the input stream from a file into GMAT objects.
 * 
 * @param scriptfile The name of the input file.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::Interpret(const std::string &scriptfile)
{
   bool retval = false;
   
   scriptFilename = scriptfile;   
   std::ifstream inFile(scriptFilename.c_str());
   inStream = &inFile;
   
   if (CheckEncoding())
   {
      theReadWriter->SetInStream(inStream);
      retval = Interpret();
   }
   else
   {
      inFile.close();
      throw InterpreterException("The script \"" + scriptFilename +
            "\" contains characters outside of the ASCII character set; "
            "please fix the file before proceeding.");
   }
   inFile.close();
   inStream = NULL;
   
   return retval;
}


//------------------------------------------------------------------------------
// bool CheckEncoding()
//------------------------------------------------------------------------------
/**
 * Checks a script file to be sure all characters are ASCII characters.
 *
 * @return true if the file does not have out of range characters, false if it
 *         does.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::CheckEncoding()
{
   bool retval = true;

   // Check each character
   char theChar;
   std::stringstream badCharMsg, currentLine;
   Integer i = 0, firstFound = -1;
   theChar = inStream->get();

   // Only do this if the file is not empty
   if (inStream->good())
   {
      bool badCharInLine = false;

      do
      {
         if ((theChar == '\n') || (theChar == '\r'))
         {
            if (badCharInLine)
               badCharMsg << "in the line\n\"" << currentLine.str() <<"\"\n";
            currentLine.str("");
            badCharInLine = false;
            i = 0;
         }
         else
            currentLine << theChar;

         ++i;
         #ifdef DEBUG_ENCODING
            MessageInterface::ShowMessage("%c", theChar);
         #endif
         if ((theChar < 0x00) || (theChar > 0x7f))
         {
            badCharInLine = true;
            badCharMsg << "***ERROR*** Non-ASCII character \"" << theChar
                       << "\" found at column " << i
                       << " (non-ASCII value "
                       << (int)((unsigned char)theChar) << ") ";
            if (firstFound == -1)
               firstFound = i;
         }
         theChar = inStream->get();
      } while (inStream->good());

      #ifdef DEBUG_ENCODING
         MessageInterface::ShowMessage("%s\nTotal read %d\n", bad.str().c_str(), i);
      #endif
   }

   // Report error if there was a non-ASCII character before the eof marker
   if (firstFound != -1)
   {
      try
      {
         if (badCharMsg.str().length() < 32160)
            MessageInterface::ShowMessage("%s\n", badCharMsg.str().c_str());
         else
            MessageInterface::ShowMessage("The script error is not "
                  "displayable\n");
      }
      catch (...)
      {
         MessageInterface::ShowMessage("The script file contains non-ASCII "
               "data and cannot be parsed\n");
      }
      retval = false;
   }

   // reset the file pointer to the start if the check passed
   if (retval)
   {
      inStream->clear();
      inStream->seekg (0, std::ios::beg);
   }

   return retval;
}

//------------------------------------------------------------------------------
// GmatCommand* InterpretGmatFunction(const std::string fileName)
//------------------------------------------------------------------------------
/**
 * Builds function cmmand sequence by parsing the function file.
 *
 * @param <fileName>  Full path and name of the GmatFunction file.
 *
 * @return A command list that is executed to run the function.
 */
//------------------------------------------------------------------------------
GmatCommand* ScriptInterpreter::InterpretGmatFunction(const std::string &fileName)
{
   #if DBGLVL_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("======================================================================\n",
       "ScriptInterpreter::InterpretGmatFunction()\n   filename = %s\n",
       fileName.c_str());
   #endif
   
   // Check if ObjectMap and SolarSystem is set
   if (theObjectMap == NULL)
      throw InterpreterException("The Object Map is not set in the Interpreter.\n");
   
   if (theSolarSystem == NULL)
      throw InterpreterException("The Solar System is not set in the Interpreter.\n");
   
   std::string msg;
   if (fileName == "")
      msg = "The GMATFunction file name is empty.\n";
   
   if (currentFunction == NULL)
      msg = "The GMATFunction pointer is NULL.\n";
   
   // We don't want to continue if error found in the function file,
   // so set continueOnError to false
   continueOnError = false;
   if (!CheckFunctionDefinition(fileName, currentFunction))
   {
      #if DBGLVL_GMAT_FUNCTION
      MessageInterface::ShowMessage
         ("ScriptInterpreter::InterpretGmatFunction() returning NULL, failed to "
          "CheckFunctionDefinition()\n");
      #endif
      return NULL;
   }
   
   // Now function file is ready to parse
   functionFilename = fileName;
   continueOnError = true;
   bool retval = false;
   std::ifstream funcFile(fileName.c_str());
   SetInStream(&funcFile);
   GmatCommand *noOp = new NoOp;
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (noOp, "NoOp", "ScriptInterpreter::InterpretGmatFunction()", "*noOp = new NoOp");
   #endif
   #if DBGLVL_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("ScriptInterpreter::InterpretGmatFunction() Create <%p>NoOp\n", noOp);
   #endif
   
   // Set build function definition flag
   hasFunctionDefinition = true;
   currentFunction->SetScriptErrorFound(false);
   
   // Clear temporary object names which currently holding MatlabFunction names
   ClearTempObjectNames();
   
   // We don't want parse first comment as header, so set skipHeader to true.
   // Set function mode to true
   retval = Interpret(noOp, true, true);
   
   // Set error found to function (loj: 2008.09.09)
   // Sandbox should check error flag before interpreting Function.
   if (retval)
      currentFunction->SetScriptErrorFound(false);
   else
      currentFunction->SetScriptErrorFound(true);
   
   funcFile.close();
   
   // Reset function mode and current function
   inFunctionMode = false;
   hasFunctionDefinition = false;
   currentFunction = NULL;
   
   #if DBGLVL_GMAT_FUNCTION > 0
   MessageInterface::ShowMessage
      ("ScriptInterpreter::InterpretGmatFunction() retval=%d\n", retval);
   #endif

   // Just return noOP for now
   if (retval)
   {
      #if DBGLVL_GMAT_FUNCTION > 0
      MessageInterface::ShowMessage
         ("ScriptInterpreter::InterpretGmatFunction() returning <%p><NoOp>\n", noOp);
      #endif
      
      #if DBGLVL_GMAT_FUNCTION > 1
      std::string fcsStr = GmatCommandUtil::GetCommandSeqString(noOp, true, true);
      MessageInterface::ShowMessage("---------- FCS of '%s'\n", fileName.c_str());
      MessageInterface::ShowMessage(fcsStr); //Notes: Do not use %s for command string
      #endif
      
      return noOp;
   }
   else
   {
      #if DBGLVL_GMAT_FUNCTION > 0
      MessageInterface::ShowMessage
         ("ScriptInterpreter::InterpretGmatFunction() returning NULL\n");
      #endif
      delete noOp;
      return NULL;
   }
}


//------------------------------------------------------------------------------
// GmatCommand* InterpretGmatFunction(Function *funct)
//------------------------------------------------------------------------------
/**
 * Reads a GMATFunction file and builds the corresponding command stream.
 * 
 * @param <funct> The GmatFunction pointer
 *
 * @return The head of the generated command list.
 */
//------------------------------------------------------------------------------
GmatCommand* ScriptInterpreter::InterpretGmatFunction(Function *funct)
{
   if (funct == NULL)
      return NULL;
   
   std::string fileName = funct->GetStringParameter("FunctionPath");
   
   #if DBGLVL_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("ScriptInterpreter::InterpretGmatFunction() function=%p\n   "
       "filename = %s\n", funct, fileName.c_str());
   #endif
   
   // Set current function
   SetFunction(funct);
   
   #if DBGLVL_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("   currentFunction set to <%p>\n", currentFunction);
   #endif
   
   return InterpretGmatFunction(fileName);
   
}


//------------------------------------------------------------------------------
// bool Build()
//------------------------------------------------------------------------------
/**
 * Writes the currently configured data to an output stream.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::Build(Gmat::WriteMode mode)
{
   if (!initialized)
      Initialize();

   // set configured object map first
   SetConfiguredObjectMap();
   return WriteScript(mode);
}


//------------------------------------------------------------------------------
// bool Build(const std::string &scriptfile)
//------------------------------------------------------------------------------
/**
 * Writes the currently configured data to a file.
 * 
 * @param scriptfile Name of the output file.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::Build(const std::string &scriptfile, Gmat::WriteMode mode)
{
   bool retval = false;
   
   if (scriptfile != "")
      scriptFilename = scriptfile;
   
   std::ofstream outFile(scriptFilename.c_str());
   outStream = &outFile;
   
   theReadWriter->SetOutStream(outStream);
   retval = Build(mode);
   
   outFile.close();
   outStream = NULL;
   
   return retval;
}


//------------------------------------------------------------------------------
// bool SetInStream(std::istream *str)
//------------------------------------------------------------------------------
/**
 * Defines the input stream that gets interpreted.
 * 
 * @param str The input stream.
 * 
 * @return true on success, false on failure.  (Currently always returns true.)
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::SetInStream(std::istream *str)
{
   #if DBGLVL_SCRIPT_READING
   MessageInterface::ShowMessage
      ("ScriptInterpreter::SetInStream() entered str=<%p>\n", str);
   #endif
   
   inStream = str;
   theReadWriter->SetInStream(inStream);
   return true;
}


//------------------------------------------------------------------------------
// bool SetOutStream(std::ostream *str)
//------------------------------------------------------------------------------
/**
 * Defines the output stream for writing serialized output.
 * 
 * @param str The output stream.
 * 
 * @return true on success, false on failure.  (Currently always returns true.)
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::SetOutStream(std::ostream *str)
{
   outStream = str;
   theReadWriter->SetOutStream(outStream);
   return true;
}


//------------------------------------------------------------------------------
// bool ReadFirstPass()
//------------------------------------------------------------------------------
/**
 * Reads only contol logic command lines from the input stream and checks for
 * unmatching End
 * 
 * @return true if the file passes checking, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::ReadFirstPass()
{
   #ifdef DEBUG_READ_FIRST_PASS
   MessageInterface::ShowMessage
      ("ScriptInterpreter::ReadFirstPass() entered, inStream=<%p>\n", inStream);
   #endif
   
   // Make sure inStream is set
   if (inStream == NULL)
   {
      MessageInterface::ShowMessage
         ("**** ERROR **** ScriptInterpreter::ReadFirstPass() input stream is NULL");
      return false;
   }
   
   char ch;
   bool reachedEndOfFile = false;
   std::string line, newLine, type;
   StringArray controlLines;
   IntegerArray lineNumbers;
   Integer charCounter = -1;
   Integer lineCounter = 1;
   
   while (!reachedEndOfFile)
   {
      line = "";
      
      charCounter++;
      inStream->seekg(charCounter, std::ios::beg);
      
      while ((ch = inStream->peek()) != '\r' && ch != '\n' && ch != EOF)
      {
         line += ch;
         charCounter++;
         inStream->seekg(charCounter, std::ios::beg);
      }
      #ifdef DEBUG_READ_FIRST_PASS
         MessageInterface::ShowMessage("ch = %d, charCounter = %d\n", ch,
               charCounter);
      #endif
      
      newLine = GmatStringUtil::Trim(line, GmatStringUtil::BOTH, true);
      
      // Skip blank or comment line
      if (newLine != "" && newLine[0] != '%')
      {         
         // Remove ending % or ;
         std::string::size_type index;
         index = newLine.find_first_of("%;");
         if (index != newLine.npos)
         {
            newLine = newLine.substr(0, index);
         }
         
         #ifdef DEBUG_READ_FIRST_PASS
         MessageInterface::ShowMessage("newLine=%s\n", newLine.c_str());
         #endif
         
         type = newLine;
         // Grab only control command part from the line
         // ex) While var1 == var2, If var1 > 5
         index = newLine.find_first_of(" \t");
         if (index != newLine.npos)
         {
            type = newLine.substr(0, index);
            if (type[index-1] == ';')
               type = type.substr(0, index-1);         
         }
         
         if (type != "" && IsBranchCommand(type))
         {
            lineNumbers.push_back(lineCounter);
            controlLines.push_back(type);
         }
      }
      
      if (ch == EOF)
         break;
      
      if (ch == '\r' || ch == '\n')
      {
         lineCounter++;
         inStream->seekg(charCounter+1, std::ios::beg);
         // Why is line number incorrect for some script files?
         if (inStream->peek() == '\n')
            charCounter++;
      }
   }
   
   // Clear status flags first and then move pointer to beginning
   inStream->clear();
   inStream->seekg(std::ios::beg);
   
   #ifdef DEBUG_READ_FIRST_PASS
   for (UnsignedInt i=0; i<lineNumbers.size(); i++)
      MessageInterface::ShowMessage
         ("     %d: %s\n", lineNumbers[i], controlLines[i].c_str());
   #endif
   
   // Check for unbalanced branch command Begin/End
   bool retval = CheckBranchCommands(lineNumbers, controlLines);
   
   #ifdef DEBUG_READ_FIRST_PASS
   MessageInterface::ShowMessage
      ("ScriptInterpreter::ReadFirstPass() returning %d\n", retval);
   #endif
   
   return retval;
   
}


//------------------------------------------------------------------------------
// bool ReadScript(GmatCommand *inCmd, bool skipHeader = false)
//------------------------------------------------------------------------------
/**
 * Reads a script from the input stream line by line and parses it.
 *
 * @param *inCmd The input command to append new commands to
 * @param  skipHeader Flag indicating first comment block is not a header(false)
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::ReadScript(GmatCommand *inCmd, bool skipHeader)
{
   bool retval1 = true;
   
   if (inStream->fail() || inStream->eof())
   {
      #if DBGLVL_SCRIPT_READING
      MessageInterface::ShowMessage
         ("ScriptInterpreter::ReadScript() inStream failed or eof reached, "
          "so returning false\n");
      #endif
      return false;
   }
   
   // Empty header & footer comment data members
   headerComment = "";
   footerComment = "";
   currentBlock = "";
   
   logicalBlockCount = 0;
   theTextParser.Reset();
   
   initialized = false;
   Initialize();
   
   if (inFunctionMode)
   {
      #ifdef DEBUG_COMMAND_MODE_TOGGLE
         MessageInterface::ShowMessage("Line %s set inCommandMode to true\n",
               inCmd->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      #endif
      inCommandMode = true;
   }
   
   // Read header comment and first logical block.
   // If input command is NULL, this method is called from GUI to interpret
   // BeginScript block. We want to ignore header comment if parsing script event.
   std::string tempHeader;
   theReadWriter->ReadFirstBlock(tempHeader, currentBlock, skipHeader);
   if (inCmd == NULL)
      headerComment = tempHeader;
   
   #if DBGLVL_SCRIPT_READING
   MessageInterface::ShowMessage
      ("===> currentBlock:\n<<<%s>>>\n", currentBlock.c_str());
   MessageInterface::ShowMessage
      ("===> headerComment:\n<<<%s>>>\n", headerComment.c_str());
   #endif
   
   while (currentBlock != "")
   {
      try
      {
         #if DBGLVL_SCRIPT_READING
         MessageInterface::ShowMessage("==========> Calling EvaluateBlock()\n");
         #endif
         
         currentBlockType = theTextParser.EvaluateBlock(currentBlock);
         
         #if DBGLVL_SCRIPT_READING > 1
         MessageInterface::ShowMessage
            ("===> after EvaluateBlock() currentBlock:\n<<<%s>>>\n", currentBlock.c_str());
         #endif
         
         #if DBGLVL_SCRIPT_READING
         MessageInterface::ShowMessage
            ("==========> Calling Parse() currentBlockType=%d\n", currentBlockType);
         #endif
         
         // Keep previous retval1 value
         retval1 = Parse(inCmd) && retval1;
         
         #if DBGLVL_SCRIPT_READING > 1
         MessageInterface::ShowMessage
            ("===> after Parse() currentBlock:\n<<<%s>>>\n", currentBlock.c_str());
         MessageInterface::ShowMessage
            ("===> currentBlockType:%d, retval1=%d\n", currentBlockType, retval1);
         #endif
         
      }
      catch (BaseException &e)
      {
         // Catch exception thrown from the Command::InterpretAction()
         HandleError(e);
         retval1 = false;
      }
      catch (...)
      {
         MessageInterface::ShowMessage
            ("**** ERROR **** Unknown error occurred during parsing the line:\n%s\n",
             currentBlock.c_str());
         throw;
      }
      
      
      if (!retval1 && !continueOnError)
      {
         #if DBGLVL_SCRIPT_READING
         MessageInterface::ShowMessage
            ("ScriptInterpreter::ReadScript() Leaving retval1=%d, "
             "continueOnError=%d\n", retval1, continueOnError);
         #endif
         
         return false;
      }
      
      if (ignoreRest)
         break;
      
      #if DBGLVL_SCRIPT_READING
      MessageInterface::ShowMessage("===> Read next logical block\n");
      #endif
      
      currentBlock = theReadWriter->ReadLogicalBlock();
      
      #if DBGLVL_SCRIPT_READING
      MessageInterface::ShowMessage
         ("===> currentBlock:\n<<<%s>>>\n", currentBlock.c_str());
      #endif
   }
   
   // Parse delayed blocks here
   Integer delayedCount = delayedBlocks.size();
   bool retval2 = true;

   #ifdef DEBUG_COMMAND_MODE_TOGGLE
      MessageInterface::ShowMessage("Setting inCommandMode to false for delayed"
            " blocks\n");
   #endif

   inCommandMode = false;
   
   #ifdef DEBUG_DELAYED_BLOCK
   MessageInterface::ShowMessage
      ("===> ScriptInterpreter::ReadScript() Start parsing delayed blocks. count=%d\n",
       delayedBlocks.size());
   #endif
   
   parsingDelayedBlock = true;
   
   for (Integer i = 0; i < delayedCount; i++)
   {
      #ifdef DEBUG_DELAYED_BLOCK
      MessageInterface::ShowMessage
         ("===> delayedBlocks[%d]=%s\n", i, delayedBlocks[i].c_str());
      #endif
      
      currentLine = delayedBlocks[i];
      lineNumber = delayedBlockLineNumbers[i];
      currentBlock = delayedBlocks[i];
      currentBlockType = theTextParser.EvaluateBlock(currentBlock);
      
      #ifdef DEBUG_DELAYED_BLOCK
      MessageInterface::ShowMessage
         ("==========> Calling Parse() currentBlockType=%d\n", currentBlockType);
      #endif
      
      // Keep previous retval1 value
      retval2 = Parse(inCmd) && retval2;
      
      #ifdef DEBUG_DELAYED_BLOCK
      MessageInterface::ShowMessage("===> delayedCount:%d, retval2=%d\n", i, retval2);
      #endif
      
      if (!retval2 && !continueOnError)
      {
         #if DBGLVL_SCRIPT_READING
         MessageInterface::ShowMessage
            ("In delayed block: Leaving retval1=%d, "
             "continueOnError=%d\n", retval1, continueOnError);
         #endif
         
         return false;
      }
   }
   
   #if DBGLVL_SCRIPT_READING
   MessageInterface::ShowMessage
      ("Leaving ReadScript() retval1=%d, retval2=%d\n", retval1, retval2);
   #endif
   
   return (retval1 && retval2);
}


//------------------------------------------------------------------------------
// bool Parse(GmatCommand *inCmd)
//------------------------------------------------------------------------------
/**
 * Builds or configures GMAT objects based on the current line of script.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::Parse(GmatCommand *inCmd)
{
   #ifdef DEBUG_PARSE
   MessageInterface::ShowMessage
      ("ScriptInterpreter::Parse() inCmd=<%p>, currentBlock = \n<<<%s>>>\n",
       inCmd, currentBlock.c_str());
   #endif
   
   bool retval = true;
   
   StringArray sarray = theTextParser.GetChunks();
   Integer count = sarray.size();
   
   #ifdef DEBUG_PARSE
   MessageInterface::ShowMessage
      ("   currentBlockType=%d, inCommandMode=%d, inRealCommandMode=%d\n",
       currentBlockType, inCommandMode, inRealCommandMode);
   for (UnsignedInt i=0; i<sarray.size(); i++)
      MessageInterface::ShowMessage("   sarray[%d]=%s\n", i, sarray[i].c_str());
   #endif
   
   // check for empty chunks
   Integer emptyChunks = 0;
   for (Integer i = 0; i < count; i++)
      if (sarray[i] == "")
         emptyChunks++;
   #ifdef DEBUG_PARSE
   MessageInterface::ShowMessage
      ("   emptyChunks=%d, count=%d\n", emptyChunks, count);
   #endif
   
   // Ignore lines with just a semicolon
   if (emptyChunks == count)
   {
      #ifdef DEBUG_PARSE
      MessageInterface::ShowMessage
         ("ScriptInterpreter::Parse() returning true, empty logical block\n");
      #endif
      return true;
   }
   
   // actual script line
   std::string actualScript = sarray[count-1];
   
   // check for function definition line
   if (currentBlockType == Gmat::FUNCTION_BLOCK)
   {
      #ifdef DEBUG_PARSE
      MessageInterface::ShowMessage("   => currentBlockType is FUNCTION_BLOCK\n");
      #endif
      
      // If function keyword is used in the main script, throw an exception
      // (For GMT-1566 fix, LOJ:2012.09.28)
      if (!inFunctionMode)
         throw InterpreterException("The \"function\" keyword is not allowd in the main script");
      
      // Check if function already defined
      // GMAT function test criteria states:
      // 2.11 The system must only allow one function to be defined inside of a function file. 
      // 2.12 If more than one function is present in a file, a warning shall be thrown
      //      and only the first function in the file shall be used.
      if (functionDefined)
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "*** WARNING *** There are more than one function "
             "defined in the function file \"%s\". \nOnly the first function \"%s\" "
             "will be used and \"%s\" and the rest of the file will be ignored.\n",
             functionFilename.c_str(), functionDef.c_str(), sarray[2].c_str());
         ignoreRest = true;
         return true;
      }
      else
      {
         functionDef = sarray[2];
         
         if (BuildFunctionDefinition(sarray[count-1]))
         {
            functionDefined = true;
            return true;
         }
         else
            throw InterpreterException("Failed to interpret function definition");
      }
   }
   
   #ifdef DEBUG_PARSE
   MessageInterface::ShowMessage("   => Now start decomposing by block type\n");
   #endif
   // Decompose by block type
   StringArray chunks;
   try
   {
      chunks = theTextParser.ChunkLine();
   }
   catch (BaseException &e)
   {
      #ifdef DEBUG_PARSE
      MessageInterface::ShowMessage("   **** Caught the exception <%s>\n", e.GetFullMessage().c_str());
      #endif
      
      // Use exception to remove Visual C++ warning
      e.GetMessageType();
      
      // if in function mode, throw better message 
      if (inFunctionMode && currentFunction != NULL)
      {
         std::string funcPath = currentFunction->GetStringParameter("FunctionPath");
         InterpreterException ex
            ("In function file \"" + funcPath + "\": "
             "Invalid function definition found ");
         HandleError(ex, true, false);
         return false;
      }
      else
      {
         #ifdef DEBUG_PARSE
         MessageInterface::ShowMessage("   **** Rethrowing the exception\n");
         #endif
         throw;
      }
   }
   
   count = chunks.size();
   GmatBase *obj = NULL;
   
   #ifdef DEBUG_PARSE
   MessageInterface::ShowMessage("   There are %d chunks in the logical block\n", count);
   for (int i=0; i<count; i++)
      MessageInterface::ShowMessage("      chunks[%d]=%s\n", i, chunks[i].c_str());
   #endif
   
   // Now go through each block type
   switch (currentBlockType)
   {
   case Gmat::COMMENT_BLOCK:
      {
         footerComment = currentBlock;
         
         #ifdef DEBUG_PARSE_FOOTER
         MessageInterface::ShowMessage("footerComment=<<<%s>>>\n", footerComment.c_str());
         #endif
         
         // More to do here for a block of comments (See page 35)
         break;
      }
   case Gmat::DEFINITION_BLOCK:
      {
         retval = ParseDefinitionBlock(chunks, inCmd, obj);
         logicalBlockCount++;
         break;
      }
   case Gmat::COMMAND_BLOCK:
      {
         // if TextParser detected as function call
         if (theTextParser.IsFunctionCall())
         {
            #ifdef DEBUG_PARSE
            MessageInterface::ShowMessage
               ("   TextParser detected as CallFunction\n");
            #endif
            
            std::string::size_type index = actualScript.find_first_of("( ");
            std::string substr = actualScript.substr(0, index);
            
            #ifdef DEBUG_PARSE
            MessageInterface::ShowMessage
               ("   Checking if '%s' is predefined non-Function object, or not "
                "yet supported ElseIf/Switch\n",
                substr.c_str());
            #endif
            
            if (substr.find("ElseIf") != substr.npos ||
                substr.find("Switch") != substr.npos)
            {
               InterpreterException ex("\"" + substr + "\" is not yet supported");
               HandleError(ex);
               return false;
            }
            
            obj = FindObject(substr);
            if (obj != NULL && !(obj->IsOfType("Function")))
            {
               InterpreterException ex;
               if (actualScript.find_first_of("(") != actualScript.npos)
               {
                  ex.SetDetails("The object named \"" + substr + "\" of type \"" +
                                obj->GetTypeName() + "\" cannot be a function name");
               }
               else
               {
                  ex.SetDetails("The object named \"" + substr + "\" of type \"" +
                                obj->GetTypeName() + "\" is not a valid command");
               }
               HandleError(ex);
               return false;
            }
            
            #ifdef DEBUG_PARSE
            MessageInterface::ShowMessage
               ("   About to create CallFunction of '%s'\n", actualScript.c_str());
            #endif
            obj = (GmatBase*)CreateCommand("CallFunction", actualScript, retval, inCmd);
            
            if (obj && retval)
            {
               // Setting comments was missing (loj: 2008.08.08)
               // Get comments and set to object
               std::string preStr = theTextParser.GetPrefaceComment();
               std::string inStr = theTextParser.GetInlineComment();
               SetComments(obj, preStr, inStr);
            }
            else
            {
               #ifdef DEBUG_PARSE
               if (obj == NULL)
                  MessageInterface::ShowMessage
                     ("   *** CreateCommand() returned NULL command\n");
               if (!retval)
                  MessageInterface::ShowMessage
                     ("   *** CreateCommand() returned false\n");
               #endif
            }
            
            break;
         }
         
         retval = ParseCommandBlock(chunks, inCmd, obj);
         logicalBlockCount++;
         break;
      }
   case Gmat::ASSIGNMENT_BLOCK:
      {
         #ifdef DEBUG_PARSE_FOOTER
         MessageInterface::ShowMessage("   Now parsing assignment block\n");
         #endif
         retval = ParseAssignmentBlock(chunks, inCmd, obj);
         logicalBlockCount++;
         break;
      }
   default:
      break;
   }
   
   if (inCommandMode)
   {
      if (!beginMissionSeqFound && firstTimeCommandMode)
      {
         firstCommandStr = actualScript;
         firstTimeCommandMode = false;
         
         #ifdef DEBUG_PARSE
         MessageInterface::ShowMessage
            ("===> Command mode entered at '%s'\n", firstCommandStr.c_str());
         #endif
      }
   }
   
   #ifdef DEBUG_PARSE
   MessageInterface::ShowMessage("ScriptInterpreter::Parse() retval=%d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool WriteScript()
//------------------------------------------------------------------------------
/**
 * Writes a script -- including all configured objects -- to the output stream.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::WriteScript(Gmat::WriteMode mode)
{
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage
      ("ScriptInterpreter::WriteScript() entered, headerComment='%s'\n",
       headerComment.c_str());
   #endif

   if (outStream == NULL)
      return false;
   
   // Prep for a new write
   objectsWritten.clear();

   //-----------------------------------
   // Header Comment
   //-----------------------------------
   //if (headerComment == "")
   if (GmatStringUtil::IsBlank(headerComment, true))
      theReadWriter->WriteText
         ("%General Mission Analysis Tool(GMAT) Script\n%Created: " +
          GmatTimeUtil::FormatCurrentTime(3) + "\n\n");
   else
      theReadWriter->WriteText(headerComment);
   
   StringArray::iterator current;
   StringArray objs;
   std::string objName;
   GmatBase *object =  NULL;
   
   //-----------------------------------
   // The Solar System
   //-----------------------------------
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found Solar Systems In Use\n");
   #endif
   // Write if not modified by user
   if (!theSolarSystem->IsObjectCloaked())
   {
      objs.clear();
      objs.push_back("SolarSystem");
      WriteObjects(objs, "Solar System User-Modified Values", mode);
   }
   
   //-----------------------------------
   // Celestial Bodies (for now, only user-defined or modified ones)
   //-----------------------------------
   objs = theModerator->GetListOfObjects(Gmat::CELESTIAL_BODY);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d CelestialBodys\n", objs.size());
   #endif
   if (objs.size() > 0)
   {
      bool        foundUserDefinedBodies = false;
      bool        foundModifiedBodies    = false;
      StringArray userDefinedBodies;
      StringArray modifiedBodies;
      for (current = objs.begin(); current != objs.end(); ++current)
      {
         #ifdef DEBUG_SCRIPT_WRITING
         MessageInterface::ShowMessage("      body name = '%s'\n", (*current).c_str());
         #endif
         
         object = FindObject(*current);
         if (object == NULL)
            throw InterpreterException("Cannot write NULL object \"" + (*current) + "\"");
         
         if (!(object->IsOfType("CelestialBody")))
            throw InterpreterException("Error writing invalid celestial body \"" + (*current) + "\"");
         CelestialBody *theBody = (CelestialBody*) object;
         if (theBody->IsUserDefined())
         {
            foundUserDefinedBodies = true;
            userDefinedBodies.push_back(*current);
         }
         else if (!theBody->IsObjectCloaked())
         {
            foundModifiedBodies = true;
            modifiedBodies.push_back(*current);
         }
      }
      if (foundModifiedBodies)  
         WriteObjects(modifiedBodies, "User-Modified Default Celestial Bodies", mode);
      if (foundUserDefinedBodies) 
         WriteObjects(userDefinedBodies, "User-Defined Celestial Bodies", mode);
   }
   
   //-----------------------------------
   // Libration Points and Barycenters
   //-----------------------------------
   #ifdef DEBUG_SCRIPT_WRITING
      MessageInterface::ShowMessage("   About to ask Moderator for list of Calculated Points\n");
   #endif
   objs = theModerator->GetListOfObjects(Gmat::CALCULATED_POINT, true);
   #ifdef DEBUG_SCRIPT_WRITING
      MessageInterface::ShowMessage("   Found %d Calculated Points\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "Calculated Points", mode);
   
   
   //-----------------------------------
   // Spacecraft
   //-----------------------------------
   objs = theModerator->GetListOfObjects(Gmat::SPACECRAFT);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Spacecraft\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteSpacecrafts(objs, mode);
   
   //-----------------------------------
   // Hardware
   //-----------------------------------
   objs = theModerator->GetListOfObjects(Gmat::HARDWARE);
   #ifdef DEBUG_SCRIPT_WRITING 
   MessageInterface::ShowMessage("   Found %d Hardware Components\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteHardwares(objs, mode);
   
   //-----------------------------------
   // Formation
   //-----------------------------------
   objs = theModerator->GetListOfObjects(Gmat::FORMATION);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Formation\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "Formation", mode);
   
   //-----------------------------------
   // Ground stations
   //-----------------------------------
   objs = theModerator->GetListOfObjects(Gmat::GROUND_STATION);
   #ifdef DEBUG_SCRIPT_WRITING
     MessageInterface::ShowMessage("   Found %d GroundStations\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "GroundStations", mode);
   
   //-----------------------------------
   // Force Model
   //-----------------------------------
   StringArray odeObjs = theModerator->GetListOfObjects(Gmat::ODE_MODEL);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Force Models\n", objs.size());
   #endif
   WriteODEModels(odeObjs, mode);
   
   //-----------------------------------
   // Propagator
   //-----------------------------------
   objs = theModerator->GetListOfObjects(Gmat::PROP_SETUP);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Propagators\n", objs.size());
   #endif
   if (objs.size() > 0)
      WritePropagators(objs, "Propagators", mode, odeObjs);
   
   //-----------------------------------
   // Burn
   //-----------------------------------
   objs = theModerator->GetListOfObjects(Gmat::BURN);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Burns\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "Burns", mode);
   
   //-----------------------------------
   // Coordinate System
   //-----------------------------------
   // Don't write default coordinate systems since they are automatically created
   objs = theModerator->GetListOfObjects(Gmat::COORDINATE_SYSTEM, true);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Coordinate Systems\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "Coordinate Systems", mode);
   
   //-----------------------------------
   // Measurement Data Files
   //-----------------------------------
   objs = theModerator->GetListOfObjects(Gmat::DATASTREAM);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Datastreams\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "DataStreams", mode);

   objs = theModerator->GetListOfObjects(Gmat::DATA_FILE);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Datafiles\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "DataFiles", mode);

   //---------------------------------------------
   // DataInterface Objects
   //---------------------------------------------
   objs = theModerator->GetListOfObjects(Gmat::INTERFACE);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d DataInterfaces\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "DataInterfaces", mode);

   //---------------------------------------------
   // Measurement Models and Tracking Data/Systems
   //---------------------------------------------
   objs = theModerator->GetListOfObjects(Gmat::MEASUREMENT_MODEL);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Measurement Models\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "MeasurementModels", mode);

   objs = theModerator->GetListOfObjects(Gmat::TRACKING_DATA);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d TrackingData Objects\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "TrackingData", mode);

   objs = theModerator->GetListOfObjects(Gmat::TRACKING_SYSTEM);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Tracking Systems\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "TrackingSystems", mode);

   //---------------------------------------------
   // Event Locators
   //---------------------------------------------
   objs = theModerator->GetListOfObjects(Gmat::EVENT_LOCATOR);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Event Locators\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "EventLocators", mode);

   //-----------------------------------
   // Solver
   //-----------------------------------
   objs = theModerator->GetListOfObjects(Gmat::SOLVER);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Solvers\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "Solvers", mode);
      
   //-----------------------------------
   // Subscriber
   //-----------------------------------
   objs = theModerator->GetListOfObjects(Gmat::SUBSCRIBER);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Subscribers\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteSubscribers(objs, mode);
      
   //-----------------------------------
   // Function
   //-----------------------------------
   objs = theModerator->GetListOfObjects(Gmat::FUNCTION);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Functions\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "Functions", mode);
   
   //-----------------------------------
   // Array, Variable and String
   //-----------------------------------
   objs = theModerator->GetListOfObjects(Gmat::PARAMETER);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Parameters\n", objs.size());
   #endif
   bool foundVarsAndArrays = false;
   bool foundOtherParameter = false; // such as Create X pos; where X is Parameter name
   if (objs.size() > 0)
   {
      for (current = objs.begin(); current != objs.end(); ++current)
      {
         #ifdef DEBUG_SCRIPT_WRITING
         MessageInterface::ShowMessage("      name = '%s'\n", (*current).c_str());
         #endif

         object = FindObject(*current);
         if (object == NULL)
            throw InterpreterException("Cannot write NULL object \"" + (*current) + "\"");

         if ((object->GetTypeName() == "Array") ||
             (object->GetTypeName() == "Variable") ||
             (object->GetTypeName() == "String"))
            foundVarsAndArrays = true;
         else
            foundOtherParameter = true;
      }
   }

   if (foundVarsAndArrays)
      WriteVariablesAndArrays(objs, mode);

   if (foundOtherParameter)
      WriteOtherParameters(objs, mode);

   //-----------------------------------
   // Command sequence
   //-----------------------------------
   WriteCommandSequence(mode);
   
   //-----------------------------------
   // Footer Comment
   //-----------------------------------
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   footerComment=\n<%s>\n", footerComment.c_str());
   #endif
   
   if (footerComment != "")
      theReadWriter->WriteText(footerComment);
   //else
   //   theReadWriter->WriteText("\n");
   
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage
      ("ScriptInterpreter::WriteScript() leaving\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool ParseDefinitionBlock(const StringArray &chunks, GmatCommand *inCmd,
//                           GmatBase *obj)
//------------------------------------------------------------------------------
/*
 * Parses the definition block.
 *
 * @param  chunks  Input string array to be used in the parsing
 * @param  inCmd   Input command pointer to be used to append the new command
 * @param  obj     Ouput object pointer if created
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::ParseDefinitionBlock(const StringArray &chunks,
                                             GmatCommand *inCmd, GmatBase *obj)
{
   #ifdef DEBUG_PARSE
   WriteStringArray("ParseDefinitionBlock()", "", chunks);
   #endif
   
   // Get comments
   std::string preStr = theTextParser.GetPrefaceComment();
   std::string inStr = theTextParser.GetInlineComment();
   
   Integer count = chunks.size();
   bool retval = true;
   
   // If object creation is not allowed in command mode
   #ifndef __ALLOW_OBJECT_CREATION_IN_COMMAND_MODE__
   if (inRealCommandMode)
   {
      if (!inFunctionMode)
      {
         InterpreterException ex
            ("GMAT currently requires that all objects are created before the "
             "mission sequence begins");
         // Changed to show as an error (Fix for GMT-2958 LOJ:2012.09.25)
         #if 0
         HandleError(ex, true, true);
         return true; // just a warning, so return true
         #else
         HandleError(ex, true, false);
         return false; // error, so return false
         #endif
      }
   }
   #endif
   
   if (count < 3)
   {
      InterpreterException ex("Missing parameter creating object for");
      HandleError(ex);
      return false;
   }
   
   std::string type = chunks[1];
   StringArray names;
   if (type == "Array")
   {
      if (chunks[2].find('[') == chunks[2].npos)
         throw InterpreterException("Opening bracket \"[\" not found");
      
      names = theTextParser.Decompose(chunks[2], "[]");
   }
   else
   {
      names = theTextParser.Decompose(chunks[2], "()");
   }
   
   count = names.size();
   
   // Special case for Propagator
   if (type == "Propagator")
      type = "PropSetup";
   
   // Handle creating objects in function mode
   if (inFunctionMode)
   {
      std::string desc = chunks[1] + " " + chunks[2];
      obj = (GmatBase*)CreateCommand(chunks[0], desc, retval, inCmd);
   }
   else
   {
      if (inCommandMode)
      {
         InterpreterException ex
            (" Command mode entered at \'" + firstCommandStr + "'.  " +
             "Create command is not allowed after command mode started");
         HandleError(ex);
         return false;
      }
      
      Integer objCounter = 0;
      for (Integer i = 0; i < count; i++)
      {
         obj = CreateObject(type, names[i]);
         
         if (obj == NULL)
         {
            // Check error message from the Validator which has more
            // detailed error message
            StringArray errList = theValidator->GetErrorList();
            #ifdef DEBUG_PARSE
            MessageInterface::ShowMessage
               ("   Validator errList.size() = %d\n", errList.size());
            #endif
            if (errList.size() > 0)
            {
               for (UnsignedInt i=0; i<errList.size(); i++)
                  HandleError(InterpreterException(errList[i]), true);
               
               // Empty Validator errors now
               theValidator->ClearErrorList();
            }
            else
            {
               InterpreterException ex
                  ("Cannot create an object \"" + names[i] + "\". The \"" + type +
                   "\" is an unknown object type or invalid object name or dimension");
               HandleError(ex);
            }
            return false;
         }
         
         objCounter++;     
         obj->FinalizeCreation();
         
         SetComments(obj, preStr, inStr);
      }
      
      // if not all objectes are created, return false
      if (objCounter < count)
      {
         InterpreterException ex("All objects are not created");
         HandleError(ex);
         return false;
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool ParseCommandBlock(const StringArray &chunks, GmatCommand *inCmd,
//                        GmatBase *obj)
//------------------------------------------------------------------------------
/*
 * Parses the command block.
 *
 * @param  chunks  Input string array to be used in the parsing
 * @param  inCmd   Input command pointer to be used to append the new command
 * @param  obj     Ouput object pointer if created
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::ParseCommandBlock(const StringArray &chunks,
                                          GmatCommand *inCmd, GmatBase *obj)
{
   #ifdef DEBUG_PARSE
   WriteStringArray("ParseCommandBlock()", "", chunks);
   #endif
   
   // Get comments
   std::string preStr = theTextParser.GetPrefaceComment();
   std::string inStr = theTextParser.GetInlineComment();
   
   #ifdef DEBUG_COMMAND_MODE_TOGGLE
      if (!inCommandMode)
      {
         std::string chunkString;
         for (UnsignedInt i = 0; i < chunks.size(); ++i)
            chunkString = chunkString + chunks[i] + "  ";
         MessageInterface::ShowMessage("Line chunks %s set inCommandMode to "
               "true\n", chunkString.c_str());
      }
   #endif


   Integer count = chunks.size();
   bool retval = true;
   inCommandMode = true;
   inRealCommandMode = true;
   bool isFunction = false;
   
   // A call function doesn't have to have arguments so this code gets a list
   // of functions and checks to see if chunks[0] is a function name.
   // Only Matlab function is required to create before the use in the call function.
   StringArray functionNames = GetListOfObjects(Gmat::FUNCTION);
   
   for (Integer i=0; i<(Integer)functionNames.size(); i++)
   {
      if (functionNames[i] == chunks[0])
      {
         isFunction = true;
         break;
      }
   }
   
   if (count < 2)
   {
      // check for one-word commands
      /** @todo This code prevents plugin implementation of one word commands.
       *        It should be fixed.
       */

      if (IsOneWordCommand(chunks[0]))
      {
         obj = (GmatBase*)CreateCommand(chunks[0], "", retval, inCmd);
      }
      else if (isFunction)
      {
         #ifdef DEBUG_PARSE
         MessageInterface::ShowMessage("   Creating CallFunction\n");
         #endif
         
         obj = (GmatBase*)CreateCommand("CallFunction", chunks[0], retval, inCmd);
      }
      else
      {
         InterpreterException ex
            ("Missing parameter with \"" + chunks[0] + "\" command");
         HandleError(ex);
         return false;
      }
   }
   else
   {
      // check for extra text at the end of one-word commands
      if (IsOneWordCommand(chunks[0]))
      {
         // If second item is not a command name then throw a exception
         if (!GmatStringUtil::IsEnclosedWith(chunks[1], "'"))
         {
            InterpreterException ex
               ("Unexpected text after \"" + chunks[0] + "\" command");
            HandleError(ex);
            return false;
         }
      }
      
      // check for .. in the command block
      if (chunks[1].find("..") != currentBlock.npos)
      {
         // allow relative path using ..
         if (chunks[1].find("../") == currentBlock.npos &&
             chunks[1].find("..\\") == currentBlock.npos)
         {
            InterpreterException ex("Found invalid syntax \"..\"");
            HandleError(ex);
            return false;
         }
      }
      
      obj = (GmatBase*)CreateCommand(chunks[0], chunks[1], retval, inCmd);
   }
   
   // if in function mode just check for retval, since function definition
   // line will not create a command
   if (inFunctionMode && retval)
   {
      return true;
   }
   else
   {
      if (obj == NULL)
         return false;
   }
   
   SetComments(obj, preStr, inStr);
   return retval;
}


//------------------------------------------------------------------------------
// bool ParseAssignmentBlock(const StringArray &chunks, GmatCommand *inCmd,
//                           GmatBase *obj)
//------------------------------------------------------------------------------
/*
 * Parses the assignment block. The assignment block has equal sign, so it
 * can be either assignment or function call.
 *
 * @param  chunks  Input string array to be used in the parsing
 * @param  inCmd   Input command pointer to be used to append the new command
 * @param  obj     Ouput object pointer if created
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::ParseAssignmentBlock(const StringArray &chunks,
                                             GmatCommand *inCmd, GmatBase *obj)
{
   #ifdef DEBUG_PARSE_ASSIGNMENT
   MessageInterface::ShowMessage
      ("ParseAssignmentBlock() entered, inCmd=<%p>, obj=<%p>\n", inCmd, obj);
   WriteStringArray("ParseAssignmentBlock()", "", chunks);
   #endif
   
   #ifdef DEBUG_COMMAND_MODE_TOGGLE
      bool commandModeAtStart = inCommandMode;
   #endif


   Integer count = chunks.size();
   bool retval = true;
   
   // Get comments
   std::string preStr = theTextParser.GetPrefaceComment();
   std::string inStr = theTextParser.GetInlineComment();
   
   // check for .. in the command block
   if (chunks[0].find("..") != chunks[0].npos ||
       chunks[1].find("..") != chunks[1].npos)
   {
      // allow relative path using ..
      if (chunks[1].find("../") == currentBlock.npos &&
          chunks[1].find("..\\") == currentBlock.npos)
      {
         InterpreterException ex("Found invalid syntax \"..\"");
         HandleError(ex);
         return false;
      }
   }
   
   // check for missing RHS
   if (count < 2)
   {
      InterpreterException ex("Missing parameter assigning object for: ");
      HandleError(ex);
      return false;
   }
   
   std::string lhs = chunks[0];
   std::string rhs = chunks[1];
   
   #ifdef DEBUG_PARSE_ASSIGNMENT
   MessageInterface::ShowMessage("   lhs='%s', rhs='%s'\n", lhs.c_str(), rhs.c_str());
   #endif
   
   // check for ElseIf, since it is not yet supported
   if (lhs.find("ElseIf ") != lhs.npos ||
       rhs.find("ElseIf ") != rhs.npos)
   {
      InterpreterException ex("\"ElseIf\" is not yet supported");
      HandleError(ex);
      return false;
   }
   
   // if RHS is not enclosed with single quotes, check for unexpected symbols or space
   if (!GmatStringUtil::IsEnclosedWith(rhs, "'"))
   {
      #ifdef DEBUG_PARSE_ASSIGNMENT
      MessageInterface::ShowMessage("   Checking for unexpected symbols\n");
      #endif
      InterpreterException ex;
      std::string cmd, part;
     
      if (lhs.find_first_of("=~<>[]{}\"") != lhs.npos ||
          rhs.find_first_of("=~<>\"") != rhs.npos)
      {
         if (lhs == "")
         {
            cmd = rhs.substr(0, rhs.find_first_of(" "));
            #ifdef DEBUG_PARSE_ASSIGNMENT
            MessageInterface::ShowMessage("   lhs is blank, cmd='%s'\n", cmd.c_str());
            #endif
            if (!IsCommandType(cmd))
            {
               part = (cmd == "" ? lhs : cmd);
               ex.SetDetails("\"" + part + "\" is not a valid assignment");
            }
         }
         else
         {
            // Check for invalid symbols and brackets in lhs and rhs
            if (lhs.find_first_of("=~<>[]{}\"") != lhs.npos)
               ex.SetDetails("\"" + lhs + "\" is not a valid LHS of assignment");
            
            if (rhs.find_first_of("=~<>\"") != rhs.npos)
               ex.SetDetails("\"" + rhs + "\" is not a valid RHS of assignment");
            
         }
         
         HandleError(ex);
         return false;
      }
      else
      {
         // Check for numeric in lhs such as 3 = x and invalid name such as $abc = *y
         #ifdef DEBUG_PARSE_ASSIGNMENT
         MessageInterface::ShowMessage
            ("   Checking if lhs is a number or invalid name found with no <.(,)'> lhs='%s'\n", lhs.c_str());
         #endif
         if (GmatStringUtil::IsNumber(lhs) ||
             (lhs.find_first_of(".(,)'") == lhs.npos && !GmatStringUtil::IsValidName(lhs)))
         {
            #ifdef DEBUG_PARSE_ASSIGNMENT
            MessageInterface::ShowMessage("   number or invalid name found in lhs, lhs='%s'\n", lhs.c_str());
            #endif
            ex.SetDetails("\"" + lhs + "\" is not a valid LHS of assignment");
            HandleError(ex);
            return false;
         }
      }
   }
   
   
   #ifdef DEBUG_PARSE_ASSIGNMENT
   MessageInterface::ShowMessage("   Checking for GmatGlobal setting\n");
   #endif
   // Check for GmatGlobal setting
   if (lhs.find("GmatGlobal.") != std::string::npos)
   {
      StringArray lhsParts = theTextParser.SeparateDots(lhs);
      if (lhsParts[1] == "LogFile")
      {
         #ifdef DEBUG_PARSE_ASSIGNMENT
         MessageInterface::ShowMessage
            ("   Found Global.LogFile, so calling MI::SetLogFile(%s)\n",
             rhs.c_str());
         #endif
         
         std::string fname = rhs;
         fname = GmatStringUtil::RemoveEnclosingString(fname, "'");
         MessageInterface::SetLogFile(fname);
         return true;
      }
   }
   
   GmatBase *owner = NULL;
   std::string attrStr = ""; 
   std::string attrInLineStr = ""; 
   Integer paramID = -1;
   Gmat::ParameterType paramType;
   
   #ifdef DEBUG_PARSE_ASSIGNMENT
   MessageInterface::ShowMessage
      ("   Before checking for math, inCommandMode=%d\n", inCommandMode);
   #endif

   if (!inCommandMode)
   {
      // check for math operators/functions
      MathParser mp = MathParser();
      
      try
      {
         if (mp.IsEquation(rhs, true))
         {
            #ifdef DEBUG_PARSE
            MessageInterface::ShowMessage("   It is a math equation\n");
            #endif
            
            // check if LHS is object.property
            if (FindPropertyID(obj, lhs, &owner, paramID, paramType))
            {
               Gmat::ParameterType paramType = obj->GetParameterType(paramID);
               // Since string can have minus sign, check it first
               if (paramType != Gmat::STRING_TYPE && paramType != Gmat::ENUMERATION_TYPE &&
                   paramType != Gmat::FILENAME_TYPE)
                  inCommandMode = true;
            }
            else
            {
               // check if LHS is a parameter
               GmatBase *tempLhsObj = FindObject(lhs);
               if (tempLhsObj && tempLhsObj->GetType() == Gmat::PARAMETER)
               {
                  Parameter *tempLhsParam = (Parameter*)tempLhsObj;
                  #ifdef DEBUG_PARSE_ASSIGNMENT
                  MessageInterface::ShowMessage
                     ("   LHS return type = %d, Gmat::RMATRIX_TYPE = %d, "
                      "inRealCommandMode = %d\n", tempLhsParam->GetReturnType(),
                      Gmat::RMATRIX_TYPE, inRealCommandMode);
                  #endif
                  
                  if (tempLhsParam->GetReturnType() == Gmat::REAL_TYPE ||
                      tempLhsParam->GetReturnType() == Gmat::RMATRIX_TYPE)
                  {
                     if (inRealCommandMode)
                        inCommandMode = true;
                     // If LHS is Array, it is assignment, so commented out (LOJ: 2010.09.21)
                     //else if (tempLhsParam->GetTypeName() == "Array")
                     //   inCommandMode = true;
                     else
                        inCommandMode = false;
                  }
               }
            }
         }
      }
      catch (BaseException &e)
      {
         // Use exception to remove Visual C++ warning
         e.GetMessageType();
         #ifdef DEBUG_PARSE_ASSIGNMENT
         MessageInterface::ShowMessage(e.GetFullMessage());
         #endif
      }
   }
   
   
   #ifdef DEBUG_PARSE_ASSIGNMENT
   MessageInterface::ShowMessage
      ("    After checking for math, inCommandMode=%d, inFunctionMode=%d, "
       "beginMissionSeqFound=%d\n", inCommandMode, inFunctionMode, beginMissionSeqFound);
   #endif
   
   bool createAssignmentCommand = true;
   
   if (inCommandMode)
   {
      // If LHS is CoordinateSystem property or Subscriber Call MakeAssignment.
      // Some scripts mixed with definitions and commands
      StringArray parts = theTextParser.SeparateDots(lhs);
      
      // If in function mode, always create Assignment command
      if (!inFunctionMode)
      {
         if (parts.size() > 1)
         {
            GmatBase *tempObj = FindObject(parts[0]);
            if ((tempObj) &&
                (tempObj->GetType() == Gmat::COORDINATE_SYSTEM ||
                 (!inRealCommandMode && tempObj->GetType() == Gmat::SUBSCRIBER)))
               createAssignmentCommand = false;
         }
         
         // Since we introduced BeginMissionSequence command, check for this flag
         if (beginMissionSeqFound)
            createAssignmentCommand = true;
      }
   }
   else
   {
      // Check for the same Variable name on both LHS and RHS, (loj: 2008.08.06)
      // such as Var = Var + 1, it must be Assignment command
      
      GmatBase *lhsObj = FindObject(lhs);
      if (lhsObj != NULL && lhsObj->IsOfType("Variable"))
      {
         StringArray varNames = GmatStringUtil::GetVarNames(rhs);
         createAssignmentCommand = false; // Forgot to set to false (loj: 2008.08.08)
         for (UnsignedInt i=0; i<varNames.size(); i++)
         {
            if (varNames[i] == lhs)
            {
               createAssignmentCommand = true;
               break;
            }
         }
      }
      else
         createAssignmentCommand = false;
      
      // Since we introduced BeginMissionSequence command, check for this flag
      if (beginMissionSeqFound)
         createAssignmentCommand = true;
   }
   
   #ifdef DEBUG_PARSE_ASSIGNMENT
   MessageInterface::ShowMessage("   createAssignmentCommand=%d\n", createAssignmentCommand);
   #endif
   
   if (createAssignmentCommand)
   {
      #ifdef DEBUG_PARSE_ASSIGNMENT
      MessageInterface::ShowMessage("   Calling CreateAssignmentCommand()\n");
      #endif
      inCommandMode = true;
      obj = (GmatBase*)CreateAssignmentCommand(lhs, rhs, retval, inCmd);
   }
   else
   {
      #ifdef DEBUG_PARSE_ASSIGNMENT
      MessageInterface::ShowMessage("   Calling MakeAssignment()\n");
      #endif
      obj = MakeAssignment(lhs, rhs);
      
      // Save script if lhs is Variable, Array, and String so those can be
      // written out as they are read
      GmatBase *lhsObj = FindObject(lhs);
      if (lhsObj != NULL && (lhsObj->IsOfType("Variable") || lhsObj->IsOfType("Array") ||
                             lhsObj->IsOfType("String")))
         userParameterLines.push_back(preStr + lhs + " = " + rhs + inStr);
   }
   
   if (obj == NULL)
   {
      #ifdef DEBUG_PARSE_ASSIGNMENT
      MessageInterface::ShowMessage
         ("   obj is NULL, and %singoring the error, so returning %s\n",
          ignoreError ? "" : "NOT ", ignoreError ? "true" : "false");
      #endif
      if (ignoreError)
         return true;
      else
         return false;
   }
   
   // paramID will be assigned from call to Interpreter::FindPropertyID()
   if ( FindPropertyID(obj, lhs, &owner, paramID, paramType) )
   {
      attrStr = preStr;
      attrInLineStr = inStr;
      
      if (attrStr != "")
         owner->SetAttributeCommentLine(paramID, attrStr);
      
      if (attrInLineStr != "")
      {
         #ifdef DEBUG_PARSE_ASSIGNMENT
         MessageInterface::ShowMessage
            ("   Calling SetInlineAttributeComment() paramID=%d, attrInLineStr=<%s>\n",
             paramID, attrInLineStr.c_str());
         #endif
         owner->SetInlineAttributeComment(paramID, attrInLineStr);
      }
      
      //Reset
      attrStr = ""; 
      attrInLineStr = ""; 
      paramID = -1;
   }
   else
   {
      SetComments(obj, preStr, inStr);
   }
   
   #ifdef DEBUG_PARSE_ASSIGNMENT
   MessageInterface::ShowMessage("ParseAssignmentBlock() returning %d\n", retval);
   #endif

   #ifdef DEBUG_COMMAND_MODE_TOGGLE
   if (commandModeAtStart != inCommandMode)
   {
      std::string unchunked;
      for (UnsignedInt i = 0; i < chunks.size(); ++i)
         unchunked = unchunked + chunks[i] + "  ";
      MessageInterface::ShowMessage(
            "Parsing assignment block for chunks [  %s] "
            "set inCommandMode to %s\n",
            unchunked.c_str(), (inCommandMode ? "true" : "false"));
   }
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool IsOneWordCommand(const std::string &str)
//------------------------------------------------------------------------------
bool ScriptInterpreter::IsOneWordCommand(const std::string &str)
{
   // Note: The interpreter really should ask the command this!
   // but this information is needed before a command is created
   bool retval = false;
   
   if ((str.find("End")                  != str.npos  &&
        str.find("EndFiniteBurn")        == str.npos) ||
       (str.find("BeginScript")          != str.npos) ||
       (str.find("NoOp")                 != str.npos) ||
       (str.find("BeginMissionSequence") != str.npos) ||
       (str.find("Else")                 != str.npos  &&
        str.find("ElseIf")               == str.npos) ||
       (str.find("Stop")                 != str.npos))
   {
      retval = true;
   }

   if (theModerator->IsSequenceStarter(str))
      retval = true;
   
   #ifdef DEBUG_ONE_WORD_COMMAND
   MessageInterface::ShowMessage
      ("ScriptInterpreter::IsOneWordCommand() str='%s' returning %s\n",
       str.c_str(), retval ? "true" : "false");
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// void SetComments(GmatBase *obj, const std::string &preStr,
//                  const std::string &inStr, bool isAttributeComment)
//------------------------------------------------------------------------------
void ScriptInterpreter::SetComments(GmatBase *obj, const std::string &preStr,
                                    const std::string &inStr)
{
   #ifdef DEBUG_SET_COMMENTS
   MessageInterface::ShowMessage
      ("ScriptInterpreter::SetComments() <%s>'%s'\n   preStr=\n'%s'\n    inStr=\n'%s'\n",
       obj->GetTypeName().c_str(), obj->GetName().c_str(), preStr.c_str(),
       inStr.c_str());
   #endif
   
   // Preseve blank lines if command
   if (obj->GetType() == Gmat::COMMAND)
   {
      if (preStr != "")
         obj->SetCommentLine(preStr);
   }
   else
   {
      // If comment has only blank space or lines, ignore
      if (!GmatStringUtil::IsBlank(preStr, true))
      {
         // Handle preface comment for Parameters separately since there are
         // comments from Create line and Initialization line
         if (obj->GetType() == Gmat::PARAMETER)
            ((Parameter*)obj)->SetCommentLine(preStr);
         else
            obj->SetCommentLine(preStr);
      }
   }
   
   if (inStr != "")
      obj->SetInlineComment(inStr);
}


//------------------------------------------------------------------------------
// void WriteSectionDelimiter(const GmatBase *firstObj,
//                            const std::string &objDesc, bool forceWriting = false)
//------------------------------------------------------------------------------
void ScriptInterpreter::WriteSectionDelimiter(const GmatBase *firstObj,
                                              const std::string &objDesc,
                                              bool forceWriting)
{
   if (firstObj == NULL)
      return;
   
   std::string comment = firstObj->GetCommentLine();
   
   #ifdef DEBUG_SECTION_DELIMITER
   MessageInterface::ShowMessage
      ("WriteSectionDelimiter() PrefaceComment of %s=<%s>\n",
       firstObj->GetName().c_str(), comment.c_str());
   #endif
   
   // Write if section delimiter not found
   if (comment.find(sectionDelimiterString[0]) == comment.npos || forceWriting)
   {
      theReadWriter->WriteText(sectionDelimiterString[0]);
      theReadWriter->WriteText(sectionDelimiterString[1] + objDesc);
      theReadWriter->WriteText(sectionDelimiterString[2]);
   }
}


//------------------------------------------------------------------------------
// void WriteSectionDelimiter(const std::string &firstObj,
//                            const std::string &objDesc, bool forceWriting = false)
//------------------------------------------------------------------------------
void ScriptInterpreter::WriteSectionDelimiter(const std::string &firstObj,
                                              const std::string &objDesc,
                                              bool forceWriting)
{
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage
      ("WriteSectionDelimiter() entered, firstObj='%s', objDesc='%s'\n",
       firstObj.c_str(), objDesc.c_str());
   #endif
   
   GmatBase *object;
   object = FindObject(firstObj);
   if (object == NULL)
      return;
   
   WriteSectionDelimiter(object, objDesc, forceWriting);
}


//------------------------------------------------------------------------------
// void WriteObjects(StringArray &objs, const std::string &objDesc,
//                   Gmat::WriteMode mod)
//------------------------------------------------------------------------------
/*
 * This method writes given object.
 */
//------------------------------------------------------------------------------
void ScriptInterpreter::WriteObjects(StringArray &objs, const std::string &objDesc,
                                     Gmat::WriteMode mode)
{
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage
      ("SI::WriteObjects() entered, objs.size=%d, objDesc='%s', mode=%d\n",
       objs.size(), objDesc.c_str(), mode);
   #endif
   
   if (objs.empty())
      return;
   
   StringArray::iterator current;
   GmatBase *object =  NULL;
   
   WriteSectionDelimiter(objs[0], objDesc);
   
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      // Only write if the object is not previously written
      if (find(objectsWritten.begin(), objectsWritten.end(), *current) ==
            objectsWritten.end())
      {
         object = FindObject(*current);
         if (object != NULL)
         {
            if (object->GetCommentLine() == "")
               theReadWriter->WriteText("\n");

            theReadWriter->WriteText(object->GetGeneratingString(mode));
            objectsWritten.push_back(*current);
         }
      }
   }
   
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage
      ("SI::WriteObjects() returning for '%s'\n", objDesc.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void WriteODEModels(StringArray &objs, Gmat::WriteMode mod)
//------------------------------------------------------------------------------
/*
 * This method writes ODEModel objects.
 */
//------------------------------------------------------------------------------
void ScriptInterpreter::WriteODEModels(StringArray &objs, Gmat::WriteMode mode)
{
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage
      ("SI::WriteODEModels() entered, objs.size=%d\n", objs.size());
   #endif
   
   StringArray propOdes;
   ObjectArray props;
   
   // Since actual ODEModel used are written from the PropSetup, check for the
   // same name first to avoid duplicate writing.
   
   StringArray propNames = theModerator->GetListOfObjects(Gmat::PROP_SETUP);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Propagators\n", propNames.size());
   #endif
   
   for (StringArray::iterator i = propNames.begin(); i != propNames.end(); ++i)
   {
      PropSetup *ps = (PropSetup*)FindObject(*i);
      if (ps != NULL)
      {
         ODEModel *ode = ps->GetODEModel();
         if (ode != NULL)
         {
            std::string odeName = ode->GetName();
            bool newOde = true;
            for (UnsignedInt j = 0; j < propOdes.size(); ++j)
               if (odeName == propOdes[j])
                  newOde = false;
            if (newOde)
            {
               propOdes.push_back(ode->GetName());
               props.push_back(ps);
            }
         }
      }
   }
   
   // Make a list of configured ODEs not in PropSetups
   StringArray odes;

   // set_difference requires SORTED sets, so this does not work:
   // set_difference(objs.begin(), objs.end(), propOdes.begin(),
   //               propOdes.end(), back_inserter(odes));

   // Instead, we'll difference "by hand":
   for (UnsignedInt i = 0; i < objs.size(); ++i)
   {
      bool matchFound = false;
      for (UnsignedInt j = 0; j < propOdes.size(); ++j)
         if (objs[i] == propOdes[j])
            matchFound = true;
      if (!matchFound)
         odes.push_back(objs[i]);
   }
   
   #ifdef DEBUG_SCRIPT_WRITING
   GmatStringUtil::WriteStringArray(objs, "   Input objects", "      ");
   GmatStringUtil::WriteStringArray(propOdes, "   Propagator ODEs", "      ");
   GmatStringUtil::WriteStringArray(odes, "   Configured ODEs", "      ");
   #endif
   // Write configured ODEModels not in PropSetups
   if (odes.size() > 0)
      WriteObjects(odes, "ForceModels", mode);
   
   // Write ODEModel from PropSetup
   if (propOdes.size() > 0)
   {
      if (odes.empty())
      {
         ///WriteSectionDelimiter(props[0], "ForceModels");
         WriteSectionDelimiter(propOdes[0], "ForceModels");
      }
      
      for (ObjectArray::iterator i = props.begin(); i != props.end(); ++i)
      {
         ODEModel *ode = ((PropSetup*)(*i))->GetODEModel();
         if (ode != NULL)
         {
            theReadWriter->WriteText("\n");
            theReadWriter->WriteText(ode->GetGeneratingString(mode));
         }
      }
   }
}


//------------------------------------------------------------------------------
// void WritePropagators(StringArray &objs, const std::string &objDesc,
//       Gmat::WriteMode mode, const StringArray &odes)
//------------------------------------------------------------------------------
/**
 * This method writes out PropSetup objects, including ODEModels that were not
 * previously written
 *
 * @param objs The list of prop setup objects
 * @param objDesc The section opener for the script file
 * @param mode The scripting mode
 * @param odes The ODEModels that have been written already
 */
//------------------------------------------------------------------------------
void ScriptInterpreter::WritePropagators(StringArray &objs,
      const std::string &objDesc, Gmat::WriteMode mode, const StringArray &odes)
{
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage
      ("SI::WritePropagators() entered, objs.size=%d, objDesc='%s', mode=%d\n",
       objs.size(), objDesc.c_str(), mode);
   #endif

   if (objs.empty())
      return;

   StringArray::iterator current;
   GmatBase *object =  NULL;

   WriteSectionDelimiter(objs[0], objDesc);

   for (current = objs.begin(); current != objs.end(); ++current)
   {
      object = FindObject(*current);
      if (object != NULL)
      {
         if (object->GetCommentLine() == "")
            theReadWriter->WriteText("\n");

         if (!object->IsOfType(Gmat::PROP_SETUP))
            throw InterpreterException("In ScriptInterpreter::WritePropagators,"
                  " the object " + (*current) + " should be a PropSetup, but "
                  "it is a " + object->GetTypeName());

         PropSetup *prop = (PropSetup*)object;
         prop->TakeAction("ExcludeODEModel");  // WriteODEModels wrote them all
         theReadWriter->WriteText(object->GetGeneratingString(mode));
         prop->TakeAction("IncludeODEModel");
      }
   }

   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage
      ("SI::WritePropagators() returning for '%s'\n", objDesc.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void WriteSpacecrafts(StringArray &objs, Gmat::WriteMode mod)
//------------------------------------------------------------------------------
/*
 * This method writes Spacecraft objects.
 */
//------------------------------------------------------------------------------
void ScriptInterpreter::WriteSpacecrafts(StringArray &objs, Gmat::WriteMode mode)
{
   StringArray::iterator current;
   GmatBase *object =  NULL;
   
   WriteSectionDelimiter(objs[0], "Spacecraft");
   
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      // I cannot recall why we need to initialize spacecraft here.
      // We may not need this any more, so set to 0 for now (LOJ: 2009.09.24)
      //==============================================================
      #if 0
      //==============================================================
      // Setup the coordinate systems on Spacecraft so they can perform conversions
      CoordinateSystem *ics = theModerator->GetInternalCoordinateSystem();
      CoordinateSystem *sccs = NULL;
      object = FindObject(*current);
      if (object != NULL)
      {
         object->SetInternalCoordSystem(ics);
         sccs = (CoordinateSystem*)
            FindObject(object->GetRefObjectName(Gmat::COORDINATE_SYSTEM));
         
         if (sccs)
            object->SetRefObject(sccs, Gmat::COORDINATE_SYSTEM);
         
         object->Initialize();
      }
      //==============================================================
      #endif
      //==============================================================
      
      object = FindObject(*current);
      if (object != NULL)
      {
         if (object->GetCommentLine() == "")
            theReadWriter->WriteText("\n");
         
         theReadWriter->WriteText(object->GetGeneratingString(mode));               
      }
   }
}


//------------------------------------------------------------------------------
// void WriteHardwares(StringArray &objs, Gmat::WriteMode mod)
//------------------------------------------------------------------------------
/*
 * This method writes Hardware objects.
 */
//------------------------------------------------------------------------------
void ScriptInterpreter::WriteHardwares(StringArray &objs, Gmat::WriteMode mode)
{
   StringArray::iterator current;
   GmatBase *object =  NULL;

   WriteSectionDelimiter(objs[0], "Hardware Components");
   
   // Hardware Tanks
   for (current = objs.begin(); current != objs.end(); ++current) 
   {
      object = FindObject(*current);
      if (object != NULL)
         if (object->GetTypeName() == "FuelTank")
         {
            if (object->GetCommentLine() == "")
               theReadWriter->WriteText("\n");
            theReadWriter->WriteText(object->GetGeneratingString(mode));
         }
   }
   
   // Hardware Thrusters
   for (current = objs.begin(); current != objs.end(); ++current) 
   {
      object = FindObject(*current);
      if (object != NULL)
         if (object->GetTypeName() == "Thruster")
         {
            if (object->GetCommentLine() == "")
               theReadWriter->WriteText("\n");
            theReadWriter->WriteText(object->GetGeneratingString(mode));
         }
   }

   // Other Hardware
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      object = FindObject(*current);
      if (object != NULL)
         if ((object->GetTypeName() != "FuelTank") &&
                  (object->GetTypeName() != "Thruster"))
         {
            if (object->GetCommentLine() == "")
               theReadWriter->WriteText("\n");
            theReadWriter->WriteText(object->GetGeneratingString(mode));
         }
   }

}


//------------------------------------------------------------------------------
// void WriteSubscribers(StringArray &objs, Gmat::WriteMode mod)
//------------------------------------------------------------------------------
/*
 * This method writes Subscriber objects.
 */
//------------------------------------------------------------------------------
void ScriptInterpreter::WriteSubscribers(StringArray &objs, Gmat::WriteMode mode)
{
   StringArray::iterator current;
   GmatBase *object =  NULL;
   
   WriteSectionDelimiter(objs[0], "Subscribers");
   
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      object = FindObject(*current);
      if (object != NULL)
      {
         if (object->GetTypeName() != "TextEphemFile")
         {
            if (object->GetCommentLine() == "")
               theReadWriter->WriteText("\n");
            theReadWriter->WriteText(object->GetGeneratingString(mode));
         }
      }
   }
}


//------------------------------------------------------------------------------
// void WriteVariablesAndArrays(StringArray &objs, Gmat::WriteMode mod)
//------------------------------------------------------------------------------
/*
 * This method writes 10 variables and arrays per line.
 * If variable or array was initialzied (non zero), it writes after Create line.
 */
//------------------------------------------------------------------------------
void ScriptInterpreter::WriteVariablesAndArrays(StringArray &objs,
                                                Gmat::WriteMode mode)
{
   #ifdef DEBUG_SCRIPT_WRITING_PARAMETER
   MessageInterface::ShowMessage("WriteVariablesAndArrays() entered\n");
   #endif
   
   // Updated to write Variable and Array as they appear in the script. (LOJ: 2010.09.23)
   // It uses userParameterLines which are saved during the parsing.
   
   StringArray::iterator current;
   ObjectArray arrList;
   ObjectArray varList;
   ObjectArray strList;
   ObjectArray arrWithValList;
   ObjectArray varWithValList;
   ObjectArray strWithValList;
   std::string genStr;
   GmatBase *object =  NULL;
   std::string sectionStr = "Arrays, Variables, Strings";
   WriteSectionDelimiter(objs[0], sectionStr, true);
   
   //-----------------------------------------------------------------
   // Fill in proper arrays
   //-----------------------------------------------------------------
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      object = FindObject(*current);
      if (object != NULL)
      {
         genStr = object->GetGeneratingString(Gmat::NO_COMMENTS);
         
         if (object->GetTypeName() == "Array")
         {
            arrList.push_back(object);
            
            // if initial value found
            if (genStr.find("=") != genStr.npos)
               arrWithValList.push_back(object);            
         }
         else if (object->GetTypeName() == "Variable")
         {
            varList.push_back(object);
            
            // if initial value found
            if (genStr.find("=") != genStr.npos)
            {
               std::string::size_type equalPos = genStr.find("=");
               std::string rhs = genStr.substr(equalPos + 1);
               std::string rhsComment = "";
               if (rhs.find('%') != std::string::npos)
               {
                  rhsComment = rhs.substr(rhs.find('%'));
                  rhs = rhs.substr(0, rhs.find('%'));
                  MessageInterface::ShowMessage("Variable with value and comment\n   Value: %s\n   Comment: %s\n",
                        rhs.c_str(), rhsComment.c_str());
               }
               rhs = GmatStringUtil::Trim(rhs, GmatStringUtil::BOTH, true, true);
               Real rval;
               // check if initial value is Real number or other Variable object
               if (GmatStringUtil::ToReal(rhs, rval))
                  varWithValList.push_back(object);
            }
         }
         else if (object->GetTypeName() == "String")
         {
            strList.push_back(object);
            
            // if initial value found
            if (genStr.find("=") != genStr.npos)
            {
               std::string::size_type equalPos = genStr.find("=");
               std::string rhs = genStr.substr(equalPos + 1);
               rhs = GmatStringUtil::Trim(rhs, GmatStringUtil::BOTH, true, true);
               // check if initial value is string literal or other String object
               if (GmatStringUtil::IsEnclosedWith(rhs, "'"))
                  strWithValList.push_back(object);
            }
         }
      }
   }
   
   //-----------------------------------------------------------------
   // Write Create Array ...
   // Write 10 Arrays without initial values per line
   //-----------------------------------------------------------------
   Integer counter = 0;
   UnsignedInt size = arrList.size();
   
   #ifdef DEBUG_SCRIPT_WRITING_PARAMETER
   MessageInterface::ShowMessage
      ("WriteVariablesAndArrays() Writing %d Arrays without initial values \n", size);
   #endif
   
   for (UnsignedInt i = 0; i<size; i++)
   {
      counter++;
      
      // Write comment line
      if (i == 0)
      {
         if (((Parameter*)arrList[i])->GetCommentLine(1) == "")
            theReadWriter->WriteText("\n");
         else
         {
            // Write comment line if non section delimiter
            std::string comment = ((Parameter*)arrList[i])->GetCommentLine(1);
            if (comment.find(sectionStr) == comment.npos)
               theReadWriter->WriteText(comment);
            else
               theReadWriter->WriteText("\n");
         }
      }
      
      if (counter == 1)
         theReadWriter->WriteText("Create Array");
      
      theReadWriter->WriteText(" " + arrList[i]->GetStringParameter("Description"));
      
      if ((counter % 10) == 0 || (i == size-1))
      {
         counter = 0;
         theReadWriter->WriteText(";\n");
      }
   }
   
   //-----------------------------------------------------------------
   // Write Create Variable ...
   // Write 10 Variables without initial values per line
   //-----------------------------------------------------------------
   counter = 0;
   size = varList.size();
   
   #ifdef DEBUG_SCRIPT_WRITING_PARAMETER
   MessageInterface::ShowMessage
      ("WriteVariablesAndArrays() Writing %d Variables without initial values \n", size);
   #endif
   
   for (UnsignedInt i = 0; i<size; i++)
   {
      counter++;
      
      // Write comment line if non section delimiter
      if (i == 0)
      {
         std::string comment = ((Parameter*)varList[i])->GetCommentLine(1);
         if (comment.find(sectionStr) == comment.npos)
            theReadWriter->WriteText(comment);
      }
      
      if (counter == 1)
         theReadWriter->WriteText("Create Variable");
      
      theReadWriter->WriteText(" " + varList[i]->GetName());
      
      if ((counter % 10) == 0 || (i == size-1))
      {
         counter = 0;
         theReadWriter->WriteText(";\n");
      }
   }
   
   //-----------------------------------------------------------------
   // Write Create String ...
   // Write 10 Strings without initial values per line
   //-----------------------------------------------------------------
   counter = 0;   
   size = strList.size();
   
   #ifdef DEBUG_SCRIPT_WRITING_PARAMETER
   MessageInterface::ShowMessage
      ("WriteVariablesAndArrays() Writing %d Strings without initial values \n", size);
   #endif
   
   for (UnsignedInt i = 0; i<strList.size(); i++)
   {
      counter++;
      
      // Write comment line if non section delimiter
      if (i == 0)
      {
         std::string comment = ((Parameter*)strList[i])->GetCommentLine(1);
         if (comment.find(sectionStr) == comment.npos)
            theReadWriter->WriteText(comment);
      }
      
      if (counter == 1)
         theReadWriter->WriteText("Create String");
      
      theReadWriter->WriteText(" " + strList[i]->GetName());
      
      if ((counter % 10) == 0 || i == size-1)
      {
         counter = 0;
         theReadWriter->WriteText(";\n");
      }
   }
   
   // Write initial values created or changed via the GUI
   WriteArrayInitialValues(arrWithValList, mode);
   WriteVariableInitialValues(varWithValList, mode);
   WriteStringInitialValues(strWithValList, mode);
   theReadWriter->WriteText("\n");
     
   #ifdef DEBUG_SCRIPT_WRITING_PARAMETER
   MessageInterface::ShowMessage("WriteVariablesAndArrays() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void WriteArrayInitialValues(const ObjectArray &arrWithValList, Gmat::WriteMode mode)
//------------------------------------------------------------------------------
/*
 * This method writes initial value of Variables
 */
//------------------------------------------------------------------------------
void ScriptInterpreter::WriteArrayInitialValues(const ObjectArray &arrWithValList,
                                                Gmat::WriteMode mode)
{
   //-----------------------------------------------------------------
   // Write Arrays with initial values
   //-----------------------------------------------------------------
   
   UnsignedInt size = arrWithValList.size();
   
   #ifdef DEBUG_SCRIPT_WRITING_PARAMETER
   MessageInterface::ShowMessage
      ("WriteArrayInitialValues() Writing %d Arrays with initial values \n", size);
   #endif
   
   for (UnsignedInt i = 0; i < size; i++)
   {
      // Write comment line
      if (i == 0)
         theReadWriter->WriteText(((Parameter*)arrWithValList[0])->GetCommentLine(2));
      
      theReadWriter->WriteText(arrWithValList[i]->GetStringParameter("InitialValue"));
   }
}


//------------------------------------------------------------------------------
// void WriteVariableInitialValues(const ObjectArray &varWithValList, Gmat::WriteMode mode)
//------------------------------------------------------------------------------
/*
 * This method writes initial value of Variables
 */
//------------------------------------------------------------------------------
void ScriptInterpreter::WriteVariableInitialValues(const ObjectArray &varWithValList,
                                                   Gmat::WriteMode mode)
{
   //-----------------------------------------------------------------
   // Write Variables with initial values
   //-----------------------------------------------------------------
   UnsignedInt size = varWithValList.size();
   
   #ifdef DEBUG_SCRIPT_WRITING_PARAMETER
   MessageInterface::ShowMessage
      ("WriteVariablesAndArrays() Writing %d Variables with initial Real values \n", size);
   #endif
   
   for (UnsignedInt i = 0; i < size; i++)
   {
      if (i == 0)
         theReadWriter->WriteText(((Parameter*)varWithValList[i])->GetCommentLine(2));
      
      theReadWriter->WriteText(varWithValList[i]->GetGeneratingString(mode));
   }
}


//------------------------------------------------------------------------------
// void WriteStringInitialValues(const ObjectArray &strWithValList, Gmat::WriteMode mode)
//------------------------------------------------------------------------------
/*
 * This method writes initial value of Variables
 */
//------------------------------------------------------------------------------
void ScriptInterpreter::WriteStringInitialValues(const ObjectArray &strWithValList,
                                                 Gmat::WriteMode mode)
{
   //-----------------------------------------------------------------
   // Write Strings with initial values by string literal
   //-----------------------------------------------------------------
   UnsignedInt size = strWithValList.size();
   
   #ifdef DEBUG_SCRIPT_WRITING_PARAMETER
   MessageInterface::ShowMessage
      ("WriteStringInitialValues() Writing %d Strings with initial values \n", size);
   #endif
   
   for (UnsignedInt i = 0; i<size; i++)
   {
      // If no new value has been assigned, skip
      //if (strWithValList[i]->GetName() ==
      //    strWithValList[i]->GetStringParameter("Expression"))
      if (strWithValList[i]->GetName() == "")
         continue;
      
      // Write comment line
      if (i == 0)
         theReadWriter->WriteText(((Parameter*)strWithValList[i])->GetCommentLine(2));
      
      theReadWriter->WriteText(strWithValList[i]->GetGeneratingString(mode));
   }
}


//------------------------------------------------------------------------------
// void WriteOtherParameters(StringArray &objs, Gmat::WriteMode mode)
//------------------------------------------------------------------------------
/*
 * This method writes other Parameters, such as X in Create X pos;
 * where X is calculated (system) Parameter name.
 */
//------------------------------------------------------------------------------
void ScriptInterpreter::WriteOtherParameters(StringArray &objs,
                                             Gmat::WriteMode mode)
{
   StringArray::iterator current;
   GmatBase *object =  NULL;
   bool isFirstTime = true;
   
   //-----------------------------------------------------------------
   // Fill in proper arrays
   //-----------------------------------------------------------------
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      object = FindObject(*current);
      if (object != NULL)
      {
         std::string typeName = object->GetTypeName();
         if (typeName != "Array" && typeName != "Variable" &&
             typeName != "String")
         {
            // write only user created calculated parameters with no dots
            if (object->GetName().find(".") == std::string::npos)
            {
               if (isFirstTime)
               {
                  WriteSectionDelimiter(objs[0], "Other Parameters");
                  isFirstTime = false;
               }
               
               std::string genStr = object->GetGeneratingString(mode);
               
               #ifdef DEBUG_SCRIPT_WRITING
               MessageInterface::ShowMessage
                  ("WriteOtherParameters() writing typeName=<%s>\n", typeName.c_str());
               #endif
               
               if (object->GetCommentLine() == "")
                  theReadWriter->WriteText("\n");
               theReadWriter->WriteText(genStr);
            }
         }
      }
   }
}


//------------------------------------------------------------------------------
// void WriteCommandSequence(Gmat::WriteMode mode)
//------------------------------------------------------------------------------
void ScriptInterpreter::WriteCommandSequence(Gmat::WriteMode mode)
{
   GmatCommand *cmd = theModerator->GetFirstCommand();
   bool inTextMode = false;
   Integer scriptEventCount = 0;
   
   if (cmd == NULL)
      return;
   
   // Write out the section delimiter comment if preface comment is blank
   // The first command is always NoOp, so get next command
   cmd = cmd->GetNext();
   
   // If there is no command after NoOp, return
   if (cmd == NULL)
      return;
   
   #ifdef DEBUG_SCRIPT_WRITING_COMMANDS
   MessageInterface::ShowMessage
      ("WriteCommandSequence() Writing Command Sequence\nPrefaceComment of %s=%s\n",
       cmd->GetTypeName().c_str(), cmd->GetCommentLine().c_str());
   #endif
   
   GmatCommand *nextCmd = cmd->GetNext();
   bool writeMissionSeqDelim = false;
   
   // Since second command should be BeginMissionSequence, check for the next one for comment
   if (nextCmd != NULL &&
       GmatStringUtil::IsBlank(cmd->GetCommentLine(), true) &&
       GmatStringUtil::IsBlank(nextCmd->GetCommentLine(), true))
   {
      theReadWriter->WriteText("\n");
      writeMissionSeqDelim = true;
   }
   else
   {
      std::string comment1 = cmd->GetCommentLine();
      std::string comment2;
      
      #ifdef DEBUG_SCRIPT_WRITING_COMMANDS
      MessageInterface::ShowMessage("==> curr comment = '%s'\n", comment1.c_str());
      #endif
      
      if (nextCmd != NULL)
      {
         comment2 = nextCmd->GetCommentLine();
         
         #ifdef DEBUG_SCRIPT_WRITING_COMMANDS
         MessageInterface::ShowMessage("==> next comment = '%s'\n", comment2.c_str());
         #endif
         
         // Swap comments if second comment has Mission Sequence
         if (comment2.find("Mission Sequence") != comment2.npos)
         {
            cmd->SetCommentLine(comment2);
            nextCmd->SetCommentLine(comment1);
         }
      }
      
      // We don't want to write section delimiter multiple times, so check for it
      if (comment1.find("Mission Sequence") == comment1.npos &&
          comment2.find("Mission Sequence") == comment2.npos)
      {
         writeMissionSeqDelim = true;
      }
   }
   
   // Write section delimiter
   if (writeMissionSeqDelim)
   {
      theReadWriter->WriteText(sectionDelimiterString[0]);
      theReadWriter->WriteText(sectionDelimiterString[1] + "Mission Sequence");
      theReadWriter->WriteText(sectionDelimiterString[2]);
      theReadWriter->WriteText("\n");      
   }
   
   while (cmd != NULL) 
   {
      #ifdef DEBUG_SCRIPT_WRITING_COMMANDS
      MessageInterface::ShowMessage
         ("ScriptInterpreter::WriteCommandSequence() before write cmd=%s, mode=%d, "
          "inTextMode=%d\n", cmd->GetTypeName().c_str(), mode, inTextMode);
      #endif
      
      // EndScript is written from BeginScript
      if (!inTextMode && cmd->GetTypeName() != "EndScript")
      {
         theReadWriter->WriteText(cmd->GetGeneratingString());
         theReadWriter->WriteText("\n");
      }
      
      if (cmd->GetTypeName() == "BeginScript")
         scriptEventCount++;
      
      if (cmd->GetTypeName() == "EndScript")
         scriptEventCount--;
      
      inTextMode = (scriptEventCount == 0) ? false : true;
      
      if (cmd == cmd->GetNext())
         throw InterpreterException
            ("Self-reference found in command stream during write.\n");
      
      cmd = cmd->GetNext();
   }
}

