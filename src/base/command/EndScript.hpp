//$Header$
//------------------------------------------------------------------------------
//                              EndScript
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
 * Script tag used to terminate a block of script that shows up verbatim in a
 * ScriptEvent panel on the GUI.
 */
//------------------------------------------------------------------------------


#ifndef EndScript_hpp
#define EndScript_hpp

#include "Command.hpp" // inheriting class's header file

/**
 * Default command used to initialize the command sequence lists in the 
 * Moderator
 */
class EndScript : public GmatCommand
{
	public:
		EndScript();
		virtual ~EndScript();
		EndScript(const EndScript& noop);
      EndScript&                    operator=(const EndScript&);

		
		bool                          Execute(void);

      // inherited from GmatBase
      virtual GmatBase* Clone(void) const;

};


#endif // EndScript_hpp
