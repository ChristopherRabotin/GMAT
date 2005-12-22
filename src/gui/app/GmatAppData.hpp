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
#include "OutputTree.hpp"
//#include "GmatMainNotebook.hpp"
#include "GmatMainFrame.hpp"
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
        
   static wxTextCtrl* GetMessageTextCtrl()
      { return theMessageTextCtrl;};
   static void SetMessageTextCtrl(wxTextCtrl *msgTextCtrl)
      { theMessageTextCtrl = msgTextCtrl;};


   static ResourceTree* GetResourceTree()
      { return theResourceTree;};
   static void SetResourceTree(ResourceTree *resourceTree)
      { theResourceTree = resourceTree;};
        
   static MissionTree* GetMissionTree()
      { return theMissionTree;};
   static void SetMissionTree(MissionTree *missionTree)
      { theMissionTree = missionTree;};
        
   static OutputTree* GetOutputTree()
      { return theOutputTree;};
   static void SetOutputTree(OutputTree *outputTree)
      { theOutputTree = outputTree;};

//    static GmatMainNotebook* GetMainNotebook()
//        { return theMainNotebook;};
//    static void SetMainNotebook(GmatMainNotebook *mainNotebook)
//        { theMainNotebook = mainNotebook;};

   static GmatMainFrame* GetMainFrame()
      { return theMainFrame;};
   static void SetMainFrame(GmatMainFrame *mainFrame)
      { theMainFrame = mainFrame;};

   static wxFont GetFont()
      { return theFont;};
   static void SetFont(wxFont font)
      { theFont = font;};

   static ViewTextFrame *theMessageWindow;
   static ViewTextFrame *theCompareWindow;
   
#endif
    
private:
   static GuiInterpreter *theGuiInterpreter;
#if !defined __CONSOLE_APP__
   static wxTextCtrl *theMessageTextCtrl;
   static ResourceTree *theResourceTree;
   static MissionTree *theMissionTree;
   static OutputTree *theOutputTree;
//    static GmatMainNotebook *theMainNotebook;
   static GmatMainFrame *theMainFrame;
   static wxFont theFont;
#endif
    
};


#endif // GmatAppData_hpp
