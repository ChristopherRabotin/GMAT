//$Id$
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
 * defines application data.
 */
//------------------------------------------------------------------------------
#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"

GmatAppData* GmatAppData::theGmatAppData = NULL;

//------------------------------------------------------------------------------
// GmatAppData* Instance()
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain the singleton.
 *
 * @return the singleton instance of the GmatAppData class.
 */
//------------------------------------------------------------------------------
GmatAppData* GmatAppData::Instance()
{
   if (!theGmatAppData)
      theGmatAppData = new GmatAppData;
   
   return theGmatAppData;
}


//------------------------------------------------------------------------------
// GuiInterpreter* GetGuiInterpreter()
//------------------------------------------------------------------------------
GuiInterpreter* GmatAppData::GetGuiInterpreter()
{
   return theGuiInterpreter;
}


//------------------------------------------------------------------------------
// void SetGuiInterpreter(GuiInterpreter *guiInterp)
//------------------------------------------------------------------------------
void GmatAppData::SetGuiInterpreter(GuiInterpreter *guiInterp)
{
   theGuiInterpreter = guiInterp;
}


#if !defined __CONSOLE_APP__

//------------------------------------------------------------------------------
// void SetMainFrame(GmatMainFrame *mainFrame)
//------------------------------------------------------------------------------
void GmatAppData::SetMainFrame(GmatMainFrame *mainFrame)
{
   theMainFrame = mainFrame;
}


//------------------------------------------------------------------------------
// GmatMainFrame* GetMainFrame()
//------------------------------------------------------------------------------
GmatMainFrame* GmatAppData::GetMainFrame()
{
   return theMainFrame;
}


//------------------------------------------------------------------------------
// void SetResourceTree(ResourceTree *resourceTree)
//------------------------------------------------------------------------------
void GmatAppData::SetResourceTree(ResourceTree *resourceTree)
{
   theResourceTree = resourceTree;
}


//------------------------------------------------------------------------------
// ResourceTree* GetResourceTree()
//------------------------------------------------------------------------------
ResourceTree* GmatAppData::GetResourceTree()
{
   return theResourceTree;
}


//------------------------------------------------------------------------------
// void SetMissionTree(MissionTree *missionTree)
//------------------------------------------------------------------------------
void GmatAppData::SetMissionTree(MissionTree *missionTree)
{
   theMissionTree = missionTree;
}


//------------------------------------------------------------------------------
// MissionTree* GetMissionTree()
//------------------------------------------------------------------------------
MissionTree* GmatAppData::GetMissionTree()
{
   return theMissionTree;
}


//------------------------------------------------------------------------------
// void SetOutputTree(OutputTree *outputTree)
//------------------------------------------------------------------------------
void GmatAppData::SetOutputTree(OutputTree *outputTree)
{
   theOutputTree = outputTree;
}


//------------------------------------------------------------------------------
// OutputTree* GetOutputTree()
//------------------------------------------------------------------------------
OutputTree* GmatAppData::GetOutputTree()
{
   return theOutputTree;
}


//------------------------------------------------------------------------------
// void SetMessageWindow(ViewTextFrame *msgFrame)
//------------------------------------------------------------------------------
void GmatAppData::SetMessageWindow(ViewTextFrame *frame)
{
   theMessageWindow = frame;
}


//------------------------------------------------------------------------------
// ViewTextFrame* GetMessageWindow()
//------------------------------------------------------------------------------
ViewTextFrame* GmatAppData::GetMessageWindow()
{
   return theMessageWindow;
}


//------------------------------------------------------------------------------
// void SetCompareWindow(ViewTextFrame *frame)
//------------------------------------------------------------------------------
void GmatAppData::SetCompareWindow(ViewTextFrame *frame)
{
   theCompareWindow = frame;
}


//------------------------------------------------------------------------------
// ViewTextFrame* GetCompareWindow()
//------------------------------------------------------------------------------
ViewTextFrame* GmatAppData::GetCompareWindow()
{
   return theCompareWindow;
}


//------------------------------------------------------------------------------
// void SetMessageTextCtrl(wxTextCtrl *msgTextCtrl)
//------------------------------------------------------------------------------
void GmatAppData::SetMessageTextCtrl(wxTextCtrl *msgTextCtrl)
{
   theMessageTextCtrl = msgTextCtrl;
}


//------------------------------------------------------------------------------
// wxTextCtrl* GetMessageTextCtrl()
//------------------------------------------------------------------------------
wxTextCtrl* GmatAppData::GetMessageTextCtrl()
{
   return theMessageTextCtrl;
}


//------------------------------------------------------------------------------
// void SetFont(wxFont font)
//------------------------------------------------------------------------------
void GmatAppData::SetFont(wxFont font)
{
   theFont = font;
}


//------------------------------------------------------------------------------
// wxFont GetFont()
//------------------------------------------------------------------------------
wxFont GmatAppData::GetFont()
{
   return theFont;
}


//------------------------------------------------------------------------------
// GmatAppData()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
GmatAppData::GmatAppData()
{
#if !defined __CONSOLE_APP__
   theMainFrame = NULL;
   theResourceTree = NULL;
   theMissionTree = NULL;
   theOutputTree = NULL;
   theMessageWindow = NULL;
   theCompareWindow = NULL;
   theMessageTextCtrl = NULL;
   theFont = wxFont(10, wxMODERN, wxNORMAL, wxNORMAL);
#endif
}


//------------------------------------------------------------------------------
// ~GmatAppData()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
GmatAppData::~GmatAppData()
{
}

#endif

