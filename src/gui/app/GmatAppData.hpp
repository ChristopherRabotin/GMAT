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
 * Declares GuiInterpreter and other GUI compoment pointers.
 */
//------------------------------------------------------------------------------
#ifndef GmatAppData_hpp
#define GmatAppData_hpp

#if !defined __CONSOLE_APP__
#include "ViewTextFrame.hpp"
#include "ResourceTree.hpp"
#include "MissionTree.hpp"
#include "GmatMainNotebook.hpp"
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

    static ResourceTree* GetResourceTree()
        { return theResourceTree;};
    static void SetResourceTree(ResourceTree *resourceTree)
        { theResourceTree = resourceTree;};
        
    static MissionTree* GetMissionTree()
        { return theMissionTree;};
    static void SetMissionTree(MissionTree *missionTree)
        { theMissionTree = missionTree;};
        
    static GmatMainNotebook* GetMainNotebook()
        { return theMainNotebook;};
    static void SetMainNotebook(GmatMainNotebook *mainNotebook)
        { theMainNotebook = mainNotebook;};

    static ViewTextFrame *theMessageWindow;
#endif
    
private:
    static GuiInterpreter *theGuiInterpreter;
#if !defined __CONSOLE_APP__
    static ResourceTree *theResourceTree;
    static MissionTree *theMissionTree;
    static GmatMainNotebook *theMainNotebook;
#endif
    
};

namespace GmatScript
{
    enum MenuType
    {
        MENU_SCRIPT_BUILD_OBJECT = 100,
        MENU_SCRIPT_RUN,
        MENU_SCRIPT_BUILD_AND_RUN
    };
}

#endif // GmatAppData_hpp
