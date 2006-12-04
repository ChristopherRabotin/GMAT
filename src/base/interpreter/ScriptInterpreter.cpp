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

//#define DEBUG_SCRIPT_READING 1
//#define DEBUG_SCRIPT_WRITING
//#define DEBUG_DELAYED_BLOCK 1
//#define DEBUG_PARSE

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
   if (!initialized)
      Initialize();
   
   inCommandMode = false;
   inRealCommandMode = false;
   
   bool retval1 = ReadScript();
   bool retval2 = FinalPass();
      
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
 * Parses the input stream, line by line, into GMAT objects.
 *
 * @param  inCmd  Command which appended to
 * @return true if the stream parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::Interpret(GmatCommand *inCmd)
{

   #if DEBUG_SCRIPT_READING
   MessageInterface::ShowMessage
      ("ScriptInterpreter::Interpret(%p) Entered inCmd=%s\n", inCmd,
       inCmd->GetTypeName().c_str());
   #endif
   
   // Since this method is called from ScriptEvent, set command mode to true
   inCommandMode = true;
   inRealCommandMode = true;
   
   bool retval1 = ReadScript(inCmd);   
   bool retval2 = FinalPass();
   
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
    //else
    //   scriptFilename = "NewScript";
       
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
   
   currentBlock = ReadLogicalBlock();
   
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
      
      currentBlock = ReadLogicalBlock();
      
      #if DEBUG_SCRIPT_READING
      MessageInterface::ShowMessage
         ("===> currentBlock:\n<<<%s>>>\n", currentBlock.c_str());
      #endif
   }
   
   // Parse delayed blocks here
   Integer delayedCount = delayedBlocks.size();
   bool retval2 = true;
   inCommandMode = false;
   for (Integer i = 0; i < delayedCount; i++)
   {
      #if DEBUG_DELAYED_BLOCK
      MessageInterface::ShowMessage
         ("===> delayedBlocks[%d]=%s\n", i, delayedBlocks[i].c_str());
      #endif
      
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
// bool ReadLogicalBlock()
//------------------------------------------------------------------------------
/**
 * Reads a line from the input stream.
 * 
 * @return the logical block
 */
//------------------------------------------------------------------------------
std::string ScriptInterpreter::ReadLogicalBlock()
{
   // Get block here so we can get line number next
   std::string block = theReadWriter->ReadLogicalBlock();
   
   // Get the line number of the logical block
   //lineNumber = itoa(theReadWriter->GetLineNumber(), 10);
   
   return block;
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
            throw UtilityException("Opening bracket \"[\" not found");
         
         names = theTextParser.Decompose(chunks[2], "[]");
      }
      else
      {
         names = theTextParser.Decompose(chunks[2], "()");
      }
      
      count = names.size();
      
      if (type == "Propagator")
         type = "PropSetup";
      
//       std::string preStr = ""; 
//       std::string inStr = ""; 
      
//       preStr = theTextParser.GetPrefaceComment();
//       inStr = theTextParser.GetInlineComment();
      
      Integer objCounter = 0;
      for (Integer i = 0; i < count; i++)
      {
         obj = CreateObject(type, names[i]);
         
         if (obj == NULL)
         {
            InterpreterException ex
               ("Cannot create object for \"" + type + "\"");
            HandleError(ex);
            return false;
         }
         objCounter++;     
         obj->FinalizeCreation();

         SetComments(obj, preStr, inStr);
         
//          if (preStr != "")
//             obj->SetCommentLine(preStr);
            
//          if (inStr != "")
//             obj->SetInlineComment(inStr);
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
      
//       std::string preStr = ""; 
//       std::string inStr = ""; 
      
//       preStr = theTextParser.GetPrefaceComment();
//       inStr = theTextParser.GetInlineComment();

      SetComments(obj, preStr, inStr);
      
//       if (preStr != "")
//          obj->SetCommentLine(preStr);
        
//       if (inStr != "")
//          obj->SetInlineComment(inStr);
      
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
               if (FindPropertyID(obj, chunks[0], &owner, paramID))
               {
                  // Since string can have minus sign, check it first
                  if (obj->GetParameterType(paramID) != Gmat::STRING_TYPE)
                     inCommandMode = true;
               }
               else
               {
                  // check if LHS is a parameter
                  GmatBase *tempObj = GetObject(chunks[0]);
                  if (tempObj && tempObj->GetType() == Gmat::PARAMETER)
                     if (((Parameter*)tempObj)->GetReturnType() == Gmat::REAL_TYPE)
                        inCommandMode = true;
               }
            }
         }
         catch (BaseException &e)
         {
            #ifdef DEBUG_PARSE
            MessageInterface::ShowMessage(e.GetMessage());
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
            GmatBase *tempObj = GetObject(parts[0]);
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



      
//       if (inCommandMode)
//       {
//          // If LHS is CoordinateSystem property, Call MakeAssignment
//          StringArray parts = theTextParser.SeparateDots(chunks[0]);
//          //MessageInterface::ShowMessage("===> parts.size()=%d\n", parts.size());

//          if (parts.size() > 1)
//          {
//             GmatBase *tempObj = GetObject(parts[0]);
//             if (tempObj)
//             {
//                // Set values by calling Set*Parameter(), even in command mode
//                // for the following types
//                if (tempObj->GetType() == Gmat::COORDINATE_SYSTEM ||
//                    (!inRealCommandMode && tempObj->GetType() == Gmat::SUBSCRIBER))
//                   obj = MakeAssignment(chunks[0], chunks[1]);
//                else
//                   obj = (GmatBase*)CreateAssignmentCommand(chunks[0], chunks[1],
//                                                            retval, inCmd);
//             }
//             else
//             {
//                obj = (GmatBase*)CreateAssignmentCommand(chunks[0], chunks[1],
//                                                         retval, inCmd);
//             }
//          }
//          else
//          {
//             obj = (GmatBase*)CreateAssignmentCommand(chunks[0], chunks[1],
//                                                      retval, inCmd);
//          }
//       }
//       else
//       {
//          obj = MakeAssignment(chunks[0], chunks[1]);
//       }
      
      if (obj == NULL)
      {
         //InterpreterException ex("Cannot Parse the line: ");
         //HandleError(ex);
         return false;
      }
      
      SetComments(obj, preStr, inStr);
      
      // paramID will be assigned from call to Interpreter class
      if ( FindPropertyID(obj, chunks[0], &owner, paramID) )
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
   Integer objSize;
      
   // Spacecraft
   objs = theModerator->GetListOfObjects(Gmat::SPACECRAFT);
   // Write out the section delimiter comment
   objSize = objs.size();
   if (objSize > 0)
   {
      theReadWriter->WriteText(sectionDelimiterString[0]);
      theReadWriter->WriteText(sectionDelimiterString[1] + "Spacecraft");
      theReadWriter->WriteText(sectionDelimiterString[2]);
   }
   
   // Setup the coordinate systems on Spacecraft so they can perform conversions
   CoordinateSystem *ics = theModerator->GetInternalCoordinateSystem(), *sccs;
   #ifdef DEBUG_SCRIPT_WRITING
   std::cout << "Found " << objs.size() << " Spacecraft\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      Spacecraft *sc = (Spacecraft*)(theModerator->GetObject(*current));
      sc->SetInternalCoordSystem(ics);
      sccs = (CoordinateSystem*)
         theModerator->GetObject(sc->GetRefObjectName(Gmat::COORDINATE_SYSTEM));
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
      
   // Hardware
   objs = theModerator->GetListOfObjects(Gmat::HARDWARE);
   // Write out the section delimiter comment
   objSize = objs.size();
   if (objSize > 0)
   {
      theReadWriter->WriteText(sectionDelimiterString[0]);
      theReadWriter->WriteText(sectionDelimiterString[1] + "Hardware Components");
      theReadWriter->WriteText(sectionDelimiterString[2]);
   }
   #ifdef DEBUG_SCRIPT_WRITING 
   std::cout << "Found " << objs.size() << " Hardware Components\n";
   #endif
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
      
   // Formations
   objs = theModerator->GetListOfObjects(Gmat::FORMATION);
   // Write out the section delimiter comment
   objSize = objs.size();
   if (objSize > 0)
   {
      theReadWriter->WriteText(sectionDelimiterString[0]);
      theReadWriter->WriteText(sectionDelimiterString[1] + "Formations");
      theReadWriter->WriteText(sectionDelimiterString[2]);
   }
   #ifdef DEBUG_SCRIPT_WRITING
   std::cout << "Found " << objs.size() << " Spacecraft\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      object = FindObject(*current);
      if (object != NULL)
      {
         theReadWriter->WriteText(object->GetGeneratingString(mode));
         theReadWriter->WriteText("\n");
      }
   }
      
   // Force Models
   objs = theModerator->GetListOfObjects(Gmat::FORCE_MODEL);
   // Write out the section delimiter comment
   objSize = objs.size();
   if (objSize > 0)
   {
      theReadWriter->WriteText(sectionDelimiterString[0]);
      theReadWriter->WriteText(sectionDelimiterString[1] + "ForceModels");
      theReadWriter->WriteText(sectionDelimiterString[2]);
   }
   #ifdef DEBUG_SCRIPT_WRITING
   std::cout << "Found " << objs.size() << " Force Models\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      object = FindObject(*current);
      if (object != NULL)
      {
         theReadWriter->WriteText(object->GetGeneratingString(mode));
         theReadWriter->WriteText("\n");
      }
   }
      
   // Propagator
   objs = theModerator->GetListOfObjects(Gmat::PROP_SETUP);
   // Write out the section delimiter comment
   objSize = objs.size();
   if (objSize > 0)
   {
      theReadWriter->WriteText(sectionDelimiterString[0]);
      theReadWriter->WriteText(sectionDelimiterString[1] + "Propagators");
      theReadWriter->WriteText(sectionDelimiterString[2]);
   }
   #ifdef DEBUG_SCRIPT_WRITING
   std::cout << "Found " << objs.size() << " Propagators\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      object = FindObject(*current);
      if (object != NULL)
      {
         theReadWriter->WriteText(object->GetGeneratingString(mode));
         theReadWriter->WriteText("\n");
      }
   }
      
   // Libration Points and Barycenters
   objs = theModerator->GetListOfObjects(Gmat::CALCULATED_POINT);
   // Write out the section delimiter comment
   objSize = objs.size();
   if (objSize > 0)
   {
      theReadWriter->WriteText(sectionDelimiterString[0]);
      theReadWriter->WriteText(sectionDelimiterString[1] + "Calculated Points");
      theReadWriter->WriteText(sectionDelimiterString[2]);
   }
   #ifdef DEBUG_SCRIPT_WRITING
   MessageInterface::ShowMessage("Found %d Calculated Points\n", objs.size());
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      object = FindObject(*current);
      if (object != NULL)
      {
         theReadWriter->WriteText(object->GetGeneratingString(mode));
         theReadWriter->WriteText("\n");
      }
   }
         
   // Burn objects
   objs = theModerator->GetListOfObjects(Gmat::BURN);
   // Write out the section delimiter comment
   objSize = objs.size();
   if (objSize > 0)
   {
      theReadWriter->WriteText(sectionDelimiterString[0]);
      theReadWriter->WriteText(sectionDelimiterString[1] + "Burns");
      theReadWriter->WriteText(sectionDelimiterString[2]);
   }
   #ifdef DEBUG_SCRIPT_WRITING
   std::cout << "Found " << objs.size() << " Burns\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      object = FindObject(*current);
      if (object != NULL)
      {
         theReadWriter->WriteText(object->GetGeneratingString(mode));
         theReadWriter->WriteText("\n");
      }
   }
         
   // Array and Variable setups
   objs = theModerator->GetListOfObjects(Gmat::PARAMETER);
   // Write out the section delimiter comment
   objSize = objs.size();
   if (objSize > 0)
   {
      bool printDelimiter = false;
      for (current = objs.begin(); current != objs.end(); ++current)
      {
         object = FindObject(*current);
         if ((object->GetTypeName() == "Array") || (object->GetTypeName() == "Variable"))
            printDelimiter = true;
      }
           
      if (printDelimiter)
      {
         theReadWriter->WriteText(sectionDelimiterString[0]);
         theReadWriter->WriteText(sectionDelimiterString[1] + "Parameters");
         theReadWriter->WriteText(sectionDelimiterString[2]);
      }
   }
   #ifdef DEBUG_SCRIPT_WRITING
   std::cout << "Found " << objs.size() << " Parameters\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      object = FindObject(*current);
      if (object != NULL)
         if ((object->GetTypeName() == "Array") || (object->GetTypeName() == "Variable"))
         {
            theReadWriter->WriteText(object->GetGeneratingString(mode));
            theReadWriter->WriteText("\n");
         }         
   }
      
   // Coordinate System setups
   objs = theModerator->GetListOfObjects(Gmat::COORDINATE_SYSTEM);
   // Write out the section delimiter comment
   objSize = objs.size();
   if (objSize > 0)
   {
      theReadWriter->WriteText(sectionDelimiterString[0]);
      theReadWriter->WriteText(sectionDelimiterString[1] + "Coordinate Systems");
      theReadWriter->WriteText(sectionDelimiterString[2]);
   }
   #ifdef DEBUG_SCRIPT_WRITING
   std::cout << "Found " << objs.size() << " Coordinate Systems\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      object = FindObject(*current);
      if (object != NULL)
      {
         theReadWriter->WriteText(object->GetGeneratingString(mode));
         theReadWriter->WriteText("\n");
      }
   }
      
   // Solver objects
   objs = theModerator->GetListOfObjects(Gmat::SOLVER);
   // Write out the section delimiter comment
   objSize = objs.size();
   if (objSize > 0)
   {
      theReadWriter->WriteText(sectionDelimiterString[0]);
      theReadWriter->WriteText(sectionDelimiterString[1] + "Solvers");
      theReadWriter->WriteText(sectionDelimiterString[2]);
   }
   #ifdef DEBUG_SCRIPT_WRITING
   std::cout << "Found " << objs.size() << " Solvers\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      object = FindObject(*current);
      if (object != NULL)
      {
         theReadWriter->WriteText(object->GetGeneratingString(mode));
         theReadWriter->WriteText("\n");
      }
   }
      
   objs = theModerator->GetListOfObjects(Gmat::SUBSCRIBER);
   // Write out the section delimiter comment
   objSize = objs.size();
   if (objSize > 0)
   {
      theReadWriter->WriteText(sectionDelimiterString[0]);
      theReadWriter->WriteText(sectionDelimiterString[1] + "Subscribers");
      theReadWriter->WriteText(sectionDelimiterString[2]);
   }
   #ifdef DEBUG_SCRIPT_WRITING
   std::cout << "Found " << objs.size() << " Subscribers\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      object = FindObject(*current);
      if (object != NULL)
         if (object->GetTypeName() != "TextEphemFile")
         {
            theReadWriter->WriteText(object->GetGeneratingString(mode));
            theReadWriter->WriteText("\n");
         
         }
   }
    
   // Function setups
   objs = theModerator->GetListOfObjects(Gmat::FUNCTION);
   // Write out the section delimiter comment
   objSize = objs.size();
   if (objSize > 0)
   {
      theReadWriter->WriteText(sectionDelimiterString[0]);
      theReadWriter->WriteText(sectionDelimiterString[1] + "Functions");
      theReadWriter->WriteText(sectionDelimiterString[2]);
   }
   #ifdef DEBUG_SCRIPT_WRITING
   std::cout << "Found " << objs.size() << " Functions\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      object = FindObject(*current);
      if (object != NULL)
      {
         theReadWriter->WriteText(object->GetGeneratingString(mode));
         theReadWriter->WriteText("\n");
      }
   }
   
   // Command sequence
   GmatCommand *cmd = theModerator->GetFirstCommand();
   bool inTextMode = false;
   // Write out the section delimiter comment
   if (cmd != NULL)
   {
      theReadWriter->WriteText(sectionDelimiterString[0]);
      theReadWriter->WriteText(sectionDelimiterString[1] + "Mission Sequence");
      theReadWriter->WriteText(sectionDelimiterString[2]);
   }
   while (cmd != NULL) 
   {
      #ifdef DEBUG_SCRIPT_WRITING
      MessageInterface::ShowMessage
         ("===> ScriptInterpreter::WriteScript() before write cmd=%s, mode=%d, "
          "inTextMode=%d\n", cmd->GetTypeName().c_str(), mode, inTextMode);
      #endif
      
      if (!inTextMode)
      {
         theReadWriter->WriteText(cmd->GetGeneratingString());
         theReadWriter->WriteText("\n");
      }
         
      if (cmd->GetTypeName() == "BeginScript")
         inTextMode = true;
      if (cmd->GetTypeName() == "EndScript")
         inTextMode = false;
      
      if (cmd == cmd->GetNext())
         throw InterpreterException("Self-reference found in command stream during write.\n");
      
      cmd = cmd->GetNext();
   }
   
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
   if (preStr != "")
      obj->SetCommentLine(preStr);
            
   if (inStr != "")
      obj->SetInlineComment(inStr);
}

