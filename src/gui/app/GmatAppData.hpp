//$Header$
//------------------------------------------------------------------------------
//                              GmatAppData
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/10/29
//
/**
 * Declares GuiInterpreter pointer.
 */
//------------------------------------------------------------------------------
#ifndef GmatAppData_hpp
#define GmatAppData_hpp

#include "GuiInterpreter.hpp"

class GmatAppData
{
public:
    static GuiInterpreter* GetGuiInterpreter()
        { return theGuiInterpreter;};
    static void SetGuiInterpreter(GuiInterpreter *guiInterp)
        { theGuiInterpreter = guiInterp;};
private:
    static GuiInterpreter *theGuiInterpreter;
};
#endif // GmatAppData_hpp
