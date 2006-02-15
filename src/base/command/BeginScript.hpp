//$Header$
//------------------------------------------------------------------------------
//                              BeginScript
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
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

#include "Command.hpp" // inheriting class's header file

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
   BeginScript&                  operator=(const BeginScript&);

   bool                          Execute();

   // inherited from GmatBase
   virtual GmatBase*             Clone() const;
   virtual const std::string&    GetGeneratingString(
                                       Gmat::WriteMode mode = Gmat::SCRIPTING,
                                       const std::string &prefix = "",
                                       const std::string &useName = "");

   const std::string GetChildString(const std::string &prefix,
                                    GmatCommand *child, GmatCommand *parent);
   
   virtual bool        RenameRefObject(const Gmat::ObjectType type,
                                       const std::string &oldName,
                                       const std::string &newName);
};

#endif // BeginScript_hpp
