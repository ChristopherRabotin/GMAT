//$Header$
//------------------------------------------------------------------------------
//                                  ClassName
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/10/24
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Null operator for the command sequence -- typically used as a place holder
 */
//------------------------------------------------------------------------------



#ifndef NOOP_HPP
#define NOOP_HPP

#include "Command.hpp" // inheriting class's header file

/**
 * Default command used to initialize the command sequence lists in the 
 * Moderator
 */
class NoOp : public Command
{
	public:
		NoOp();
		virtual ~NoOp();
		NoOp(const NoOp& noop);
        NoOp&                         operator=(const NoOp&);

		
		bool                          Execute(void);
};


#endif // NOOP_HPP
