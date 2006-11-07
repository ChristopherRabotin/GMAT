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

//#define DEBUG_PARSE
//#define DEBUG_SCRIPT_READING_AND_WRITING
//#define DEBUG_SCRIPT_READING 1

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
   
   theReadWriter = ScriptReadWriter::Instance();
   
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
// bool Interpret(void)
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
   
   bool retval = ReadScript();
   
   if (retval)
      retval = FinalPass();
   
   return retval;
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
// bool ReadScript()
//------------------------------------------------------------------------------
/**
 * Reads a script from the input stream line by line and parses it.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::ReadScript()
{
   
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
      #if DEBUG_SCRIPT_READING
      MessageInterface::ShowMessage("==========> Calling EvaluateBlock()\n");
      #endif
         
      currentBlockType = theTextParser.EvaluateBlock(currentBlock);
         
      if ( !Parse(currentBlock) )
         return false; 
         
      #if DEBUG_SCRIPT_READING
      MessageInterface::ShowMessage("===> Read next logical block\n");
      #endif
         
      currentBlock = ReadLogicalBlock();
         
      #if DEBUG_SCRIPT_READING > 1
      MessageInterface::ShowMessage
         ("===> currentBlock:\n<<<%s>>>\n", currentBlock.c_str());
      #endif
         
   }
   
   // Parse delayed blocks here
   Integer delayedCount = delayedBlocks.size();
   bool retval = true;
   inCommandMode = false;
   for (Integer i = 0; i < delayedCount; i++)
   {
      MessageInterface::ShowMessage
         ("===> delayedBlocks[%d]=%s\n", i, delayedBlocks[i].c_str());
      
      if (!Parse(delayedBlocks[i]))
         retval = false;
   }
   
   if (continueOnError)
      return true;
   else
      return (retval);

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
   lineNumber = itoa(theReadWriter->GetLineNumber(), 10);
   
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
bool ScriptInterpreter::Parse(const std::string &logicBlock)
{
   #ifdef DEBUG_PARSE
   MessageInterface::ShowMessage
      ("ScriptInterpreter::Parse() logicBlock = %s\n", logicBlock.c_str());
   #endif
   
   StringArray sarray = theTextParser.GetChunks();
   Integer count = sarray.size();
   
   #ifdef DEBUG_PARSE
   MessageInterface::ShowMessage("   currentBlockType=%d\n", currentBlockType);   
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
      if (count < 3)
         throw InterpreterException
            ("Missing parameter creating object for: \n" + lineNumber + ": " + logicBlock + "\n");

      std::string type = chunks[1];
      StringArray names = theTextParser.Decompose(chunks[2], "()");
      count = names.size();

      if (type == "Propagator")
         type = "PropSetup";
      
      std::string preStr = ""; 
      std::string inStr = ""; 
      
      preStr = theTextParser.GetPrefaceComment();
      inStr = theTextParser.GetInlineComment();
      
      Integer objCounter = 0;
      for (Integer i = 0; i < count; i++)
      {
         obj = CreateObject(type, names[i]);
         
         if (obj == NULL)
            throw InterpreterException
               ("Error encountered creating objects for: \n" + lineNumber+ ":" + logicBlock + "\n");
               
         objCounter++;     
         obj->FinalizeCreation();
         
         if (preStr != "")
            obj->SetCommentLine(preStr);
            
         if (inStr != "")
            obj->SetInlineComment(inStr);
      }

      // if not all objectes are created, return false
      if (objCounter < count)
         throw InterpreterException
            ("All objects are not created: \n\"" + lineNumber+ ":" + currentBlock + "\"\n");
         
      logicalBlockCount++;
   }
   else if (currentBlockType == Gmat::COMMAND_BLOCK)
   {
      inCommandMode = true;
      
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
            
            obj = (GmatBase*)CreateCommand(chunks[0], "");
         else if (isFunction)
            obj = (GmatBase*)CreateCommand("CallFunction", chunks[0]);
         else
            throw InterpreterException
               ("Missing parameter with command object: \n" + lineNumber + ":" + logicBlock + "\n");
      }
      else
      {
         obj = (GmatBase*)CreateCommand(chunks[0], chunks[1]);
      }
          
      if (obj == NULL)
         throw InterpreterException("Cannot Parse the line: \n\"" + lineNumber + ":" + currentBlock + "\"\n");
      
      std::string preStr = ""; 
      std::string inStr = ""; 
      
      preStr = theTextParser.GetPrefaceComment();
      inStr = theTextParser.GetInlineComment();
      
      if (preStr != "")
         obj->SetCommentLine(preStr);
        
      if (inStr != "")
         obj->SetInlineComment(inStr);

      logicalBlockCount++;
   }
   else if (currentBlockType == Gmat::ASSIGNMENT_BLOCK)
   {
      if (count < 2)
         throw InterpreterException
            ("Missing parameter assigning object for: \n" + lineNumber + ":" + logicBlock + "\n");
      
      if (inCommandMode)
         obj = (GmatBase*)CreateAssignmentCommand(chunks[0], chunks[1]);
      else
         obj = MakeAssignment(chunks[0], chunks[1]);
      
      if (obj == NULL)
         throw InterpreterException("Cannot Parse the line: \n\"" + lineNumber + ":" + currentBlock + "\"\n");
      
      GmatBase *owner = NULL;
      std::string attrStr = ""; 
      std::string attrInLineStr = ""; 
      Integer paramID = -1;
      
      // paramID will be assigned from call to Interpreter class
      if ( FindPropertyID(obj, chunks[0], &owner,paramID) )
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
         
         //MessageInterface::ShowMessage("Owner===>%s\n", owner->GetGeneratingString(Gmat::SCRIPTING).c_str());
         //MessageInterface::ShowMessage("\n\n\nObj===>%s\n", obj->GetGeneratingString(Gmat::SCRIPTING).c_str());
      }
      
      logicalBlockCount++;
   }
   return true;
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
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
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
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING 
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
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
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
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
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
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
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
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
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
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
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
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
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
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
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
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
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
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
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
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
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
   GmatCommand *cmd = theModerator->GetNextCommand();
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
      #ifdef DEBUG_SCRIPT_READING_AND_WRITING
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

