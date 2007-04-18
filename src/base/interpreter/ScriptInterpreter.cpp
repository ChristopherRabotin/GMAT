//$Header$
//------------------------------------------------------------------------------
//                               ScriptInterpreter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
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
#include "StringUtil.hpp"      // for GmatStringUtil::

//#define DEBUG_READ_FIRST_PASS 1
//#define DEBUG_SCRIPT_READING 1
//#define DEBUG_SCRIPT_WRITING
//#define DEBUG_DELAYED_BLOCK 1
//#define DEBUG_PARSE 1

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
    
   scriptFilename = "";
   currentBlock   = "";
   headerComment  = "";
   footerComment  = "";
   
   sectionDelimiterString.clear();
   
   inCommandMode = false;
   inRealCommandMode = false;
   
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
   //if (!initialized)
   Initialize();
   
   inCommandMode = false;
   inRealCommandMode = false;

   // Before parsing script, check for unmatching control logic
   bool retval0 = ReadFirstPass();
   
   bool retval1 = false;
   bool retval2 = false;
   
   if (retval0)
   {
      retval1 = ReadScript();
      retval2 = FinalPass();
   }
   
   // Write any error messages collected
   for (UnsignedInt i=0; i<errorList.size(); i++)
      MessageInterface::ShowMessage("%d: %s\n", i+1, errorList[i].c_str());
   
   #if DEBUG_SCRIPT_READING
   MessageInterface::ShowMessage
      ("ScriptInterpreter::Interpret() Leaving retval1=%d, retval2=%d\n",
       retval1, retval2);
   #endif
   
   return (retval1 && retval2);
}


//------------------------------------------------------------------------------
// bool Interpret(GmatCommand *inCmd)
//------------------------------------------------------------------------------
/**
 * Parses and creates commands from input stream and append to input command.
 *
 * @param  inCmd  Command which appended to
 * @return true if the stream parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::Interpret(GmatCommand *inCmd)
{
   Initialize();
   
   #if DEBUG_SCRIPT_READING
   MessageInterface::ShowMessage
      ("ScriptInterpreter::Interpret(%p) Entered inCmd=%s\n", inCmd,
       inCmd->GetTypeName().c_str());
   #endif
   
   // Since this method is called from ScriptEvent, set command mode to true
   inCommandMode = true;
   inRealCommandMode = true;
   
   // Before parsing script, check for unmatching control logic
   bool retval0 = ReadFirstPass();
   
   bool retval1 = false;
   bool retval2 = false;
   
   if (retval0)
   {
      retval1 = ReadScript(inCmd);   
      retval2 = FinalPass();
   }
   
   // Write any error messages collected
   for (UnsignedInt i=0; i<errorList.size(); i++)
      MessageInterface::ShowMessage("%d: %s\n", i+1, errorList[i].c_str());
   
   #if DEBUG_SCRIPT_READING
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

   theReadWriter->SetInStream(inStream);
   retval = Interpret();
    
   inFile.close();
   inStream = NULL;
    
   return retval;
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
   #if DEBUG_READ_FIRST_PASS
   MessageInterface::ShowMessage("ScriptInterpreter::ReadFirstPass() entered\n");
   #endif
   
   char ch;
   bool reachedEndOfFile = false;
   std::string line, newLine, type;
   StringArray controlLines;
   IntegerArray lineNumbers;
   Integer charCounter = -1;
   Integer lineCounter = 0;
   
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
      
      newLine = GmatStringUtil::Trim(line, GmatStringUtil::BOTH, true);
      
      #if DEBUG_READ_FIRST_PASS
      MessageInterface::ShowMessage("newLine=%s\n", newLine.c_str());
      #endif
      
      type = newLine;
      UnsignedInt index = newLine.find_first_of(" \t");
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
   
   // Clear staus flags first and then move pointer to beginning
   inStream->clear();
   inStream->seekg(std::ios::beg);
   
   #if DEBUG_READ_FIRST_PASS
   for (UnsignedInt i=0; i<lineNumbers.size(); i++)
      MessageInterface::ShowMessage
         ("     %d: %s\n", lineNumbers[i], controlLines[i].c_str());
   #endif
   
   // Check for unbalaced branch command Begin/End
   bool retval = CheckBranchCommands(lineNumbers, controlLines);
   
   #if DEBUG_READ_FIRST_PASS
   MessageInterface::ShowMessage
      ("ScriptInterpreter::ReadFirstPass() returning %d\n", retval);
   #endif
   
   return retval;
   
}


//------------------------------------------------------------------------------
// bool ReadScript(GmatCommand *inCmd)
//------------------------------------------------------------------------------
/**
 * Reads a script from the input stream line by line and parses it.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::ReadScript(GmatCommand *inCmd)
{
   bool retval1 = true;
   
   if (inStream->fail() || inStream->eof()) 
      return false;
   
   // Empty header & footer comment data members
   headerComment = "";
   footerComment = "";
   
   currentBlock = "";
   
   logicalBlockCount = 0;
   theTextParser.Reset();
   
   initialized = false;
   Initialize();
   
   currentBlock = theReadWriter->ReadLogicalBlock();
   
   #if DEBUG_SCRIPT_READING
   MessageInterface::ShowMessage
      ("===> currentBlock:\n<<<%s>>>\n", currentBlock.c_str());
   #endif
   
   while (currentBlock != "")
   {
      try
      {
         #if DEBUG_SCRIPT_READING
         MessageInterface::ShowMessage("==========> Calling EvaluateBlock()\n");
         #endif
         
         currentBlockType = theTextParser.EvaluateBlock(currentBlock);
         
         //MessageInterface::ShowMessage
         //   ("===> after EvaluateBlock() currentBlock:\n<<<%s>>>\n", currentBlock.c_str());
         
         #if DEBUG_SCRIPT_READING
         MessageInterface::ShowMessage
            ("==========> Calling Parse() currentBlockType=%d\n", currentBlockType);
         #endif
         
         // Keep previous retval1 value
         retval1 = Parse(currentBlock, inCmd) && retval1;
         
         //MessageInterface::ShowMessage
         //   ("===> after Parse() currentBlock:\n<<<%s>>>\n", currentBlock.c_str());
         //MessageInterface::ShowMessage
         //   ("===> currentBlockType:%d, retval1=%d\n", currentBlockType, retval1);
         
      }
      catch (BaseException &e)
      {
         // Catch exception thrown from the Command::InterpretAction()
         HandleError(e);
         retval1 = false;
      }
      
      if (!retval1 && !continueOnError)
      {
         #if DEBUG_SCRIPT_READING
         MessageInterface::ShowMessage
            ("ScriptInterpreter::ReadScript() Leaving retval1=%d, "
             "continueOnError=%d\n", retval1, continueOnError);
         #endif
         
         return false;
      }
      
      #if DEBUG_SCRIPT_READING
      MessageInterface::ShowMessage("===> Read next logical block\n");
      #endif
      
      currentBlock = theReadWriter->ReadLogicalBlock();
      
      #if DEBUG_SCRIPT_READING
      MessageInterface::ShowMessage
         ("===> currentBlock:\n<<<%s>>>\n", currentBlock.c_str());
      #endif
   }
   
   // Parse delayed blocks here
   Integer delayedCount = delayedBlocks.size();
   bool retval2 = true;
   inCommandMode = false;
   
   #if DEBUG_DELAYED_BLOCK
   MessageInterface::ShowMessage
      ("===> ScriptInterpreter::ReadScript() Start parsing delayed blocks. count=%d\n",
       delayedBlocks.size());
   #endif
   
   parsingDelayedBlock = true;
   
   for (Integer i = 0; i < delayedCount; i++)
   {
      #if DEBUG_DELAYED_BLOCK
      MessageInterface::ShowMessage
         ("===> delayedBlocks[%d]=%s\n", i, delayedBlocks[i].c_str());
      #endif

      currentLine = delayedBlocks[i];
      lineNumber = delayedBlockLineNumbers[i];
      currentBlock = delayedBlocks[i];
      currentBlockType = theTextParser.EvaluateBlock(currentBlock);
      
      // Keep previous retval1 value
      retval2 = Parse(currentBlock, inCmd) && retval2;
      
      #if DEBUG_DELAYED_BLOCK
      MessageInterface::ShowMessage("===> delayedCount:%d, retval2=%d\n", i, retval2);
      #endif
      
      if (!retval2 && !continueOnError)
      {
         #if DEBUG_SCRIPT_READING
         MessageInterface::ShowMessage
            ("In delayed block: Leaving retval1=%d, "
             "continueOnError=%d\n", retval1, continueOnError);
         #endif
         
         return false;
      }
   }
   
   #if DEBUG_SCRIPT_READING
   MessageInterface::ShowMessage
      ("Leaving ReadScript() retval1=%d, retval2=%d\n", retval1, retval2);
   #endif
   
   return (retval1 && retval2);
}


//------------------------------------------------------------------------------
// bool Parse(const std::string &logicBlock)
//------------------------------------------------------------------------------
/**
 * Builds or configures GMAT objects based on the current line of script.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::Parse(const std::string &logicBlock, GmatCommand *inCmd)
{
   #ifdef DEBUG_PARSE
   MessageInterface::ShowMessage
      ("ScriptInterpreter::Parse() logicBlock = %s\n", logicBlock.c_str());
   #endif

   bool retval = true;
   
   StringArray sarray = theTextParser.GetChunks();
   Integer count = sarray.size();
   
   #ifdef DEBUG_PARSE
   MessageInterface::ShowMessage
      ("   currentBlockType=%d, inCommandMode=%d, inRealCommandMode=%d\n",
       currentBlockType, inCommandMode, inRealCommandMode);
   for (UnsignedInt i=0; i<sarray.size(); i++)
      MessageInterface::ShowMessage("   chunks[%d]=%s\n", i, sarray[i].c_str());
   #endif
   
   // check for empty chunks
   Integer emptyChunks = 0;
   for (Integer i = 0; i < count; i++)
      if (sarray[i] == "")
         emptyChunks++;
   
   if (emptyChunks == count)
      return false;

   // for comments
   std::string preStr = ""; 
   std::string inStr = ""; 
      
   preStr = theTextParser.GetPrefaceComment();
   inStr = theTextParser.GetInlineComment();
   
   // Decompose by block type
   StringArray chunks = theTextParser.ChunkLine();
   count = chunks.size();
   GmatBase *obj = NULL;
   
   if (currentBlockType == Gmat::COMMENT_BLOCK)
   {
      if (logicalBlockCount == 0)
         headerComment = currentBlock;
      else
         footerComment = currentBlock;
         
      // More to do here for a block of comments (See page 35)
   }
   else if (currentBlockType == Gmat::DEFINITION_BLOCK)
   {
      //loj: 11/21/06 Uncomment the lines when all scripts are ready
      // Do not allow object creation on command mode
      //if (inRealCommandMode)
      //{
      //   InterpreterException ex
      //      ("Creating Object is not allowed in command mode");
      //   HandleError(ex);
      //   return false;
      //}
      
      if (count < 3)
      {
         InterpreterException ex
            ("Missing parameter creating object for");
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
      
      if (type == "Propagator")
         type = "PropSetup";
      
      Integer objCounter = 0;
      for (Integer i = 0; i < count; i++)
      {
         obj = CreateObject(type, names[i]);
         
         if (obj == NULL)
         {
            InterpreterException ex
               ("Cannot create an object \"" + names[i] + "\". The \"" +
                type + "\" is unknown object type");
            HandleError(ex);
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
      
      logicalBlockCount++;
   }
   else if (currentBlockType == Gmat::COMMAND_BLOCK)
   {
      inCommandMode = true;
      inRealCommandMode = true;
      
      // a call function doesn't have to have parameters
      // so this code gets a list of the functions and 
      // checks to see if chunks[0] is a function name
      StringArray functionNames = GetListOfObjects(Gmat::FUNCTION);
      bool isFunction = false;
      
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
         if ((logicBlock.find("End")           != logicBlock.npos  &&
              logicBlock.find("EndFiniteBurn") == logicBlock.npos) ||
             (logicBlock.find("BeginScript")   != logicBlock.npos) ||
             (logicBlock.find("Else")          != logicBlock.npos) ||
             (logicBlock.find("Stop")          != logicBlock.npos))
            
            obj = (GmatBase*)CreateCommand(chunks[0], "", retval, inCmd);
         else if (isFunction)
         {
            MessageInterface::ShowMessage("===> Creating CallFunction\n");
            obj = (GmatBase*)CreateCommand("CallFunction", chunks[0], retval, inCmd);
         }
         else
         {
            InterpreterException ex("Missing parameter with command object: ");
            HandleError(ex);
            return false;
         }
      }
      else
      {
         obj = (GmatBase*)CreateCommand(chunks[0], chunks[1], retval, inCmd);
      }
      
      if (obj == NULL)
      {
         return false;
      }
      
      SetComments(obj, preStr, inStr);
            
      logicalBlockCount++;
   }
   else if (currentBlockType == Gmat::ASSIGNMENT_BLOCK)
   {
      if (count < 2)
      {
         InterpreterException ex("Missing parameter assigning object for: ");
         HandleError(ex);
         return false;
      }
      
      GmatBase *owner = NULL;
      std::string attrStr = ""; 
      std::string attrInLineStr = ""; 
      Integer paramID = -1;
      Gmat::ParameterType paramType;
      
      //MessageInterface::ShowMessage("===> inCommandMode=%d\n", inCommandMode);
      
      if (!inCommandMode)
      {
         // check for math operators/functions
         MathParser mp = MathParser();
         
         try
         {
            //MessageInterface::ShowMessage
            //   ("===> chunks[0]=%s, chunks[1]=%s\n", chunks[0].c_str(), chunks[1].c_str());
            
            if (mp.IsEquation(chunks[1]))
            {
               // check if LHS is object.property
               if (FindPropertyID(obj, chunks[0], &owner, paramID, paramType))
               {
                  // Since string can have minus sign, check it first
                  if (obj->GetParameterType(paramID) != Gmat::STRING_TYPE)
                     inCommandMode = true;
               }
               else
               {
                  // check if LHS is a parameter
                  GmatBase *tempObj = GetConfiguredObject(chunks[0]);
                  if (tempObj && tempObj->GetType() == Gmat::PARAMETER)
                     if (((Parameter*)tempObj)->GetReturnType() == Gmat::REAL_TYPE)
                        inCommandMode = true;
               }
            }
         }
         catch (BaseException &e)
         {
            #ifdef DEBUG_PARSE
            MessageInterface::ShowMessage(e.GetFullMessage());
            #endif
         }
      }
      
      //MessageInterface::ShowMessage("===> inCommandMode=%d\n", inCommandMode);
      
      bool createAssignment = true;

      if (inCommandMode)
      {
         // If LHS is CoordinateSystem property or Subscriber Call MakeAssignment.
         // Some scripts mixed with definitions and commands
         StringArray parts = theTextParser.SeparateDots(chunks[0]);
         //MessageInterface::ShowMessage("===> parts.size()=%d\n", parts.size());
         
         if (parts.size() > 1)
         {
            GmatBase *tempObj = GetConfiguredObject(parts[0]);
            if ((tempObj) &&
                (tempObj->GetType() == Gmat::COORDINATE_SYSTEM ||
                 (!inRealCommandMode && tempObj->GetType() == Gmat::SUBSCRIBER)))
               createAssignment = false;
         }
      }
      else
         createAssignment = false;

      
      if (createAssignment)
         obj = (GmatBase*)CreateAssignmentCommand(chunks[0], chunks[1], retval, inCmd);
      else
         obj = MakeAssignment(chunks[0], chunks[1]);
      
      if (obj == NULL)
      {
         return false;
      }
      
      SetComments(obj, preStr, inStr);
      
      // paramID will be assigned from call to Interpreter class
      if ( FindPropertyID(obj, chunks[0], &owner, paramID, paramType) )
      {
         attrStr = theTextParser.GetPrefaceComment();
         attrInLineStr = theTextParser.GetInlineComment();
         
         if (attrStr != "")
            owner->SetAttributeCommentLine(paramID, attrStr);
         
         if (attrInLineStr != "")
            owner->SetInlineAttributeComment(paramID, attrInLineStr);
         
         //Reset
         attrStr = ""; 
         attrInLineStr = ""; 
         paramID = -1;
         
         //MessageInterface::ShowMessage
         //   ("Owner===>%s\n", owner->GetGeneratingString(Gmat::SCRIPTING).c_str());
         //MessageInterface::ShowMessage
         //   ("\n\n\nObj===>%s\n", obj->GetGeneratingString(Gmat::SCRIPTING).c_str());
      }
      
      logicalBlockCount++;
   }
   
   #if DEBUG_PARSE
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
   if (outStream == NULL)
      return false;
   
   // Header  Comment
   if (headerComment != "")
      theReadWriter->WriteText(headerComment);
   
   // Initialize the section delimiter comment
   sectionDelimiterString.push_back("\n%----------------------------------------\n");
   sectionDelimiterString.push_back("%---------- ");
   sectionDelimiterString.push_back("\n%----------------------------------------\n\n");
   
   StringArray::iterator current;
   StringArray objs;
   std::string objName;
   GmatBase *object =  NULL;
   
   // Spacecraft
   objs = theModerator->GetListOfObjects(Gmat::SPACECRAFT);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Spacecraft\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteSpacecrafts(objs, mode);
   
   
   // Hardware
   objs = theModerator->GetListOfObjects(Gmat::HARDWARE);
   #ifdef DEBUG_SCRIPT_WRITING 
   MessageInterface::ShowMessage("   Found %d Hardware Components\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteHardwares(objs, mode);

   
   // Formation
   objs = theModerator->GetListOfObjects(Gmat::FORMATION);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Formation\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "Formation", mode);
   
   
   // Force Model
   objs = theModerator->GetListOfObjects(Gmat::FORCE_MODEL);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Force Models\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "ForceModels", mode);
   
   
   // Propagator
   objs = theModerator->GetListOfObjects(Gmat::PROP_SETUP);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Propagators\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "Propagators", mode);
   
   
   // Libration Points and Barycenters
   objs = theModerator->GetListOfObjects(Gmat::CALCULATED_POINT);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Calculated Points\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "Calculated Points", mode);

   
   // Burn
   objs = theModerator->GetListOfObjects(Gmat::BURN);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Burns\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "Burns", mode);

   
   // Array and Variable
   objs = theModerator->GetListOfObjects(Gmat::PARAMETER);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Parameters\n", objs.size());
   #endif
   bool foundVarsAndArrays = false;
   if (objs.size() > 0)
   {
      for (current = objs.begin(); current != objs.end(); ++current)
      {
         object = FindObject(*current);
         if ((object->GetTypeName() == "Array") ||
             (object->GetTypeName() == "Variable") ||
             (object->GetTypeName() == "String"))
            foundVarsAndArrays = true;
      }
   }
   
   if (foundVarsAndArrays > 0)
      WriteVariablesAndArrays(objs, mode);
   
   
   // Coordinate System
   objs = theModerator->GetListOfObjects(Gmat::COORDINATE_SYSTEM);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Coordinate Systems\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "Coordinate Systems", mode);

   
   // Solver
   objs = theModerator->GetListOfObjects(Gmat::SOLVER);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Solvers\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "Solvers", mode);
   
   
   // Subscriber
   objs = theModerator->GetListOfObjects(Gmat::SUBSCRIBER);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Subscribers\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteSubscribers(objs, mode);
   
   
   // Function
   objs = theModerator->GetListOfObjects(Gmat::FUNCTION);
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("   Found %d Functions\n", objs.size());
   #endif
   if (objs.size() > 0)
      WriteObjects(objs, "Functions", mode);
   
   
   // Command sequence
   WriteCommandSequence(mode);
   
   
   // Footer Comment
   if (footerComment != "")
      theReadWriter->WriteText(footerComment);
   else
      theReadWriter->WriteText("\n");
     
   return true;
}


//------------------------------------------------------------------------------
// void SetComments(GmatBase *obj, const std::string &preStr,
//                  const std::string &inStr)
//------------------------------------------------------------------------------
void ScriptInterpreter::SetComments(GmatBase *obj, const std::string &preStr,
                                    const std::string &inStr)
{
   #if DEBUG_SET_COMMENTS
   MessageInterface::ShowMessage
      ("ScriptInterpreter::SetComments() preStr=%s, inStr=%s\n",
       preStr.c_str(), inStr.c_str());
   #endif
   
   if (preStr != "")
      obj->SetCommentLine(preStr);
   
   if (inStr != "")
      obj->SetInlineComment(inStr);
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
   StringArray::iterator current;
   GmatBase *object =  NULL;
   
   // Write out the section delimiter comment
   theReadWriter->WriteText(sectionDelimiterString[0]);
   theReadWriter->WriteText(sectionDelimiterString[1] + objDesc);
   theReadWriter->WriteText(sectionDelimiterString[2]);

   for (current = objs.begin(); current != objs.end(); ++current)
   {
      object = FindObject(*current);
      if (object != NULL)
      {
         theReadWriter->WriteText(object->GetGeneratingString(mode));
         theReadWriter->WriteText("\n");
      }
   }
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
   
   // Write out the section delimiter comment
   theReadWriter->WriteText(sectionDelimiterString[0]);
   theReadWriter->WriteText(sectionDelimiterString[1] + "Spacecraft");
   theReadWriter->WriteText(sectionDelimiterString[2]);
   
   // Setup the coordinate systems on Spacecraft so they can perform conversions
   CoordinateSystem *ics = theModerator->GetInternalCoordinateSystem(), *sccs;
   
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      Spacecraft *sc = (Spacecraft*)(GetConfiguredObject(*current));
      sc->SetInternalCoordSystem(ics);
      sccs = (CoordinateSystem*)
         GetConfiguredObject(sc->GetRefObjectName(Gmat::COORDINATE_SYSTEM));
      if (sccs)
         sc->SetRefObject(sccs, Gmat::COORDINATE_SYSTEM);
      sc->Initialize();
      
      object = FindObject(*current);
      if (object != NULL)
      {       
         theReadWriter->WriteText(object->GetGeneratingString(mode));               
         theReadWriter->WriteText("\n");
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
   
   // Write out the section delimiter comment
   theReadWriter->WriteText(sectionDelimiterString[0]);
   theReadWriter->WriteText(sectionDelimiterString[1] + "Hardware Components");
   theReadWriter->WriteText(sectionDelimiterString[2]);
   
   // Hardware Tanks
   for (current = objs.begin(); current != objs.end(); ++current) 
   {
      object = FindObject(*current);
      if (object != NULL)
         if (object->GetTypeName() == "FuelTank")
         {
            theReadWriter->WriteText(object->GetGeneratingString(mode));
            theReadWriter->WriteText("\n");
         }
   }
   
   // Hardware Thrusters
   for (current = objs.begin(); current != objs.end(); ++current) 
   {
      object = FindObject(*current);
      if (object != NULL)
         if (object->GetTypeName() == "Thruster")
         {
            theReadWriter->WriteText(object->GetGeneratingString(mode));
            theReadWriter->WriteText("\n");
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
   StringArray::iterator current;
   std::vector<GmatBase*> arrList;
   std::vector<GmatBase*> arrWithValList;
   std::vector<GmatBase*> varList;
   std::vector<GmatBase*> varWithValList;
   std::vector<GmatBase*> strList;
   std::vector<GmatBase*> strWithValList;
   std::string genStr;
   GmatBase *object =  NULL;
   
   // Write out the section delimiter comment
   theReadWriter->WriteText(sectionDelimiterString[0]);
   theReadWriter->WriteText(sectionDelimiterString[1] + "Parameters");
   theReadWriter->WriteText(sectionDelimiterString[2]);

   //-----------------------------------------------------------------
   // Fill in proper arrays
   //-----------------------------------------------------------------
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      object = FindObject(*current);
      if (object != NULL)
      {
         if (object->GetTypeName() == "Array")
         {
            genStr = object->GetGeneratingString(mode);
            arrList.push_back(object);
            
            // if initial value found
            if (genStr.find("=") != genStr.npos)
               arrWithValList.push_back(object);

         }
         else if (object->GetTypeName() == "Variable")
         {
            genStr = object->GetGeneratingString(mode);            
            varList.push_back(object);
            
            // if initial value found
            if (genStr.find("=") != genStr.npos)
               varWithValList.push_back(object);
         }
         else if (object->GetTypeName() == "String")
         {
            genStr = object->GetGeneratingString(mode);            
            strList.push_back(object);
            
            // if initial value found
            if (genStr.find("=") != genStr.npos)
               strWithValList.push_back(object);
         }
      }
   }

   
   //-----------------------------------------------------------------
   // Write 10 Variables without initial values per line;
   //-----------------------------------------------------------------
   Integer counter = 0;
   
   for (UnsignedInt i = 0; i<varList.size(); i++)
   {
      counter++;

      // Write comment line
      if (counter == 1)
         theReadWriter->WriteText(((Parameter*)varList[i])->GetCommentLine(2));
      
      if (counter == 1 || (counter % 11) == 0)
         theReadWriter->WriteText("Create Variable ");
      
      theReadWriter->WriteText(varList[i]->GetName() + " ");
      
      if ((counter % 10) == 0)
         theReadWriter->WriteText("\n");
   }
   
   
   //-----------------------------------------------------------------
   // Write Variables with initial values
   //-----------------------------------------------------------------
   for (UnsignedInt i = 0; i<varWithValList.size(); i++)
   {
      if (i == 0)
      {
         theReadWriter->WriteText("\n");
         theReadWriter->WriteText(((Parameter*)varWithValList[i])->GetCommentLine(1));
      }
      
      theReadWriter->WriteText(varWithValList[i]->GetGeneratingString(mode));
   }
   
   
   //-----------------------------------------------------------------
   // Write 10 Arrays without initial values per line;
   //-----------------------------------------------------------------
   counter = 0;
   
   for (UnsignedInt i = 0; i<arrList.size(); i++)
   {
      counter++;
      
      // Write comment line
      if (counter == 1)
      {
         theReadWriter->WriteText("\n");
         theReadWriter->WriteText(((Parameter*)arrList[i])->GetCommentLine(2));
      }
      
      if (counter == 1 || (counter % 11) == 0)
         theReadWriter->WriteText("Create Array ");
      
      theReadWriter->WriteText(arrList[i]->GetStringParameter("Description") + " ");
      
      if ((counter % 10) == 0)
         theReadWriter->WriteText("\n");
   }
   
   //-----------------------------------------------------------------
   // Write Arrays with initial values
   //-----------------------------------------------------------------
   for (UnsignedInt i = 0; i<arrWithValList.size(); i++)
   {
      if (i == 0)
      {
         theReadWriter->WriteText("\n");
         theReadWriter->WriteText(arrWithValList[0]->GetCommentLine());
      }
      
      theReadWriter->WriteText(arrWithValList[i]->GetStringParameter("InitialValue"));
   }
   
   
   //-----------------------------------------------------------------
   // Write 10 Strings without initial values per line;
   //-----------------------------------------------------------------
   counter = 0;   
   for (UnsignedInt i = 0; i<strList.size(); i++)
   {
      counter++;
      
      // Write comment line
      if (counter == 1)
      {
         theReadWriter->WriteText("\n");
         theReadWriter->WriteText(((Parameter*)strList[i])->GetCommentLine(2));
      }
      
      if (counter == 1 || (counter % 11) == 0)
         theReadWriter->WriteText("Create String ");
      
      theReadWriter->WriteText(strList[i]->GetName() + " ");
      
      if ((counter % 10) == 0)
         theReadWriter->WriteText("\n");
   }
   
   
   //-----------------------------------------------------------------
   // Write Strings with initial values
   //-----------------------------------------------------------------
   for (UnsignedInt i = 0; i<strWithValList.size(); i++)
   {
      if (i == 0)
      {
         theReadWriter->WriteText("\n");
         theReadWriter->WriteText(((Parameter*)strWithValList[i])->GetCommentLine(1));
      }
      
      theReadWriter->WriteText(strWithValList[i]->GetGeneratingString(mode));
   }
   
   theReadWriter->WriteText("\n");
   
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
   
   // Write out the section delimiter comment
   theReadWriter->WriteText(sectionDelimiterString[0]);
   theReadWriter->WriteText(sectionDelimiterString[1] + "Plots and Reports");
   theReadWriter->WriteText(sectionDelimiterString[2]);

   for (current = objs.begin(); current != objs.end(); ++current)
   {
      object = FindObject(*current);
      if (object != NULL)
      {
         if (object->GetTypeName() != "TextEphemFile")
         {
            theReadWriter->WriteText(object->GetGeneratingString(mode));
            theReadWriter->WriteText("\n");
         
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
   
   // Write out the section delimiter comment
   theReadWriter->WriteText(sectionDelimiterString[0]);
   theReadWriter->WriteText(sectionDelimiterString[1] + "Mission Sequence");
   theReadWriter->WriteText(sectionDelimiterString[2]);
   
   while (cmd != NULL) 
   {
      #ifdef DEBUG_SCRIPT_WRITING
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
