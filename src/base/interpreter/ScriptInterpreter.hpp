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
 * Class definition for the ScriptInterpreter
 */
//------------------------------------------------------------------------------

#ifndef ScriptInterpreter_hpp
#define ScriptInterpreter_hpp

#include "Interpreter.hpp"
#include "InterpreterException.hpp"
#include "Function.hpp"

/**
 * The ScriptInterpreter class manages the script reading and writing process.
 */
class GMAT_API ScriptInterpreter : public Interpreter
{
public:        
   static ScriptInterpreter*   Instance();
   
   virtual bool Interpret();
   virtual bool Interpret(GmatCommand *cmd, bool skipHeader = false,
                          bool functionMode = false);
   virtual bool Interpret(const std::string &scriptfile);
   
   GmatCommand* InterpretGmatFunction(const std::string &fileName);
   GmatCommand* InterpretGmatFunction(Function *funct);
   
   virtual bool Build(Gmat::WriteMode mode);
   
   bool ReadFirstPass();
   
   bool Build(const std::string &scriptfile,
              Gmat::WriteMode mode = Gmat::SCRIPTING);
   
   bool SetInStream(std::istream *str);
   bool SetOutStream(std::ostream *str);
   
protected:
   
   /// The script interpreter singleton
   static ScriptInterpreter *instance;
   
   std::istream *inStream;
   std::ostream *outStream;
   
   bool ReadScript(GmatCommand *cmd = NULL, bool skipHeader = false);
   bool Parse(GmatCommand *inCmd = NULL);
   bool WriteScript(Gmat::WriteMode mode = Gmat::SCRIPTING);
   
   ScriptInterpreter();
   virtual ~ScriptInterpreter();
   
private:
   
   /// A counter that counts the logical blocks of script as they are read.
   Integer logicalBlockCount; 
   /// Flag indicating function file has function definition
   bool functionDefined;
   /// Flag indicating function file has more than one function definition, so ignoring the rest
   bool ignoreRest;
   /// Flag indicating first time command mode entered
   bool firstTimeCommandMode;
   /// First command string
   std::string firstCommandStr;
   /// Function definition line
   std::string functionDef;
   /// Function file name
   std::string functionFilename;
   /// Name of the current script file
   std::string scriptFilename;
   /// Section delimiter comment
   StringArray sectionDelimiterString;
   /// Script lines with Variable, Array, and String
   StringArray userParameterLines;
   /// List of written objects, used to avoid duplicates
   StringArray objectsWritten;
   
   bool CheckEncoding();
   bool ParseDefinitionBlock(const StringArray &chunks, GmatCommand *inCmd,
                             GmatBase *obj);
   bool ParseCommandBlock(const StringArray &chunks, GmatCommand *inCmd,
                          GmatBase *obj);
   bool ParseAssignmentBlock(const StringArray &chunks, GmatCommand *inCmd,
                             GmatBase *obj);
   bool IsOneWordCommand(const std::string &str);
   
   void SetComments(GmatBase *obj, const std::string &preStr,
                    const std::string &inStr);
   
   void WriteSectionDelimiter(const GmatBase *firstObj, const std::string &objDesc,
                              bool forceWriting = false);
   void WriteSectionDelimiter(const std::string &firstObj, const std::string &objDesc,
                              bool forceWriting = false);
   void WriteObjects(StringArray &objs, const std::string &objDesc,
                     Gmat::WriteMode mode);
   void WriteODEModels(StringArray &objs, Gmat::WriteMode mode);
   void WritePropagators(StringArray &objs, const std::string &objDesc,
         Gmat::WriteMode mode, const StringArray &odes);
   void WriteSpacecrafts(StringArray &objs, Gmat::WriteMode mode);
   void WriteHardwares(StringArray &objs, Gmat::WriteMode mode);
   void WriteVariablesAndArrays(StringArray &objs, Gmat::WriteMode mode);
   void WriteArrayInitialValues(const ObjectArray &arrWithValList,
                                Gmat::WriteMode mode);
   void WriteVariableInitialValues(const ObjectArray &varWithValList,
                                   Gmat::WriteMode mode);
   void WriteStringInitialValues(const ObjectArray &strWithValList,
                                 Gmat::WriteMode mode);
   void WriteOtherParameters(StringArray &objs, Gmat::WriteMode mode);
   void WriteSubscribers(StringArray &objs, Gmat::WriteMode mode);
   void WriteCommandSequence(Gmat::WriteMode mode);
   
};

#endif // ScriptInterpreter_hpp

