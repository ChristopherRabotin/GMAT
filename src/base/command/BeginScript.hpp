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
class BeginScript : public GmatCommand
{
	public:
		BeginScript();
		virtual ~BeginScript();
		BeginScript(const BeginScript& noop);
      BeginScript&                  operator=(const BeginScript&);

		bool                          Execute(void);

      // inherited from GmatBase
      virtual GmatBase* Clone(void) const;
      virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                                            const std::string &prefix = "",
                                            const std::string &useName = "");


};

#endif // BeginScript_hpp
