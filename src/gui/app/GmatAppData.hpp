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

#if !defined __CONSOLE_APP__
#include "ViewTextFrame.hpp"
#endif

#include "GuiInterpreter.hpp"

class GmatAppData
{
public:
    static GuiInterpreter* GetGuiInterpreter()
        { return theGuiInterpreter;};
    static void SetGuiInterpreter(GuiInterpreter *guiInterp)
        { theGuiInterpreter = guiInterp;};
#if !defined __CONSOLE_APP__
    static ViewTextFrame* GetMessageWindow()
        { return theMessageWindow;};
    static void SetMessageWindow(ViewTextFrame *msgFrame)
        { theMessageWindow = msgFrame;};
#endif
    
private:
    static GuiInterpreter *theGuiInterpreter;
    
#if !defined __CONSOLE_APP__
    static ViewTextFrame *theMessageWindow;
#endif
};
#endif // GmatAppData_hpp
