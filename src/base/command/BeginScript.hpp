//$Id$
//------------------------------------------------------------------------------
//                              BeginScript
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2004/02/25
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Script tag used to indicate a block of script that shows up verbatim in a
 * ScriptEvent panel on the GUI.
 */
//------------------------------------------------------------------------------



#ifndef BeginScript_hpp
#define BeginScript_hpp

#include "GmatCommand.hpp" // inheriting class's header file

/**
 * Default command used to initialize the command sequence lists in the 
 * Moderator
 */
class GMAT_API BeginScript : public GmatCommand
{
public:
   BeginScript();
   virtual ~BeginScript();
   BeginScript(const BeginScript& noop);
   BeginScript&         operator=(const BeginScript&);
   
   bool                 Execute();
   
   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual const std::string&
                        GetGeneratingString(
                           Gmat::WriteMode mode = Gmat::SCRIPTING,
                           const std::string &prefix = "",
                           const std::string &useName = "");
   
   const std::string    GetChildString(const std::string &prefix,
                                       GmatCommand *child, GmatCommand *parent);
   
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
protected:
   
   void IndentChildString(std::stringstream &gen, GmatCommand* cmd, 
                          std::string &indent, Gmat::WriteMode mode,
                          const std::string &prefix, const std::string &useName,
                          bool indentCommentOnly);
   void IndentComment(std::stringstream &gen, std::string &comment,
                      const std::string &prefix);
};

#endif // BeginScript_hpp
