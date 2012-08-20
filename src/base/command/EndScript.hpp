//$Id$
//------------------------------------------------------------------------------
//                              EndScript
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
 * Script tag used to terminate a block of script that shows up verbatim in a
 * ScriptEvent panel on the GUI.
 */
//------------------------------------------------------------------------------


#ifndef EndScript_hpp
#define EndScript_hpp

#include "GmatCommand.hpp" // inherited class's header file

/**
 * Script tag used to indicate the end of a block of script that shows up
 * verbatim in a ScriptEvent panel on the GUI.
 */
class GMAT_API EndScript : public GmatCommand
{
public:
   EndScript();
   virtual ~EndScript();
   EndScript(const EndScript& copy);
   EndScript&          operator=(const EndScript& copy);
   
   bool                Execute();

   // inherited from GmatBase
   virtual GmatBase*   Clone() const;
   virtual bool        RenameRefObject(const Gmat::ObjectType type,
                                       const std::string &oldName,
                                       const std::string &newName);

   DEFAULT_TO_NO_CLONES
};

#endif // EndScript_hpp
