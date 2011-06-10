//$Id$
//------------------------------------------------------------------------------
//                             GmatMdiChildFrame
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// ** Legal **
//
// Author: Allison Greene
// Created: 2003/08/28
/**
 * This class provides the layout of a mdi child frame
 */
//------------------------------------------------------------------------------

#include "GmatMdiChildFrame.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "GuiInterpreter.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include "GuiItemManager.hpp"
#include "GmatMenuBar.hpp"       // for namespace GmatMenu

// @todo We cannot create own MenuBar yet.
// Double Window menu appears when more than one child is open and cannot
// delete theMenuBar in the destructor.
//#define __CREATE_CHILD_MENU_BAR__

//#define DEBUG_MDI_CHILD_FRAME
//#define DEBUG_UPDATE_GUI_ITEM

//------------------------------
// event tables for wxWindows
//------------------------------

//------------------------------------------------------------------------------
// EVENT_TABLE(GmatMdiChildFrame, wxMDIChildFrame)
//------------------------------------------------------------------------------
/**
 * Events Table for the menu and tool bar
 *
 * @note Indexes event handler functions.
 */
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(GmatMdiChildFrame, wxMDIChildFrame)
   EVT_CLOSE(GmatMdiChildFrame::OnClose) 
   EVT_ACTIVATE(GmatMdiChildFrame::OnActivate)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// GmatMdiChildFrame::GmatMdiChildFrame(...)
//------------------------------------------------------------------------------
GmatMdiChildFrame::GmatMdiChildFrame(wxMDIParentFrame *parent, 
                                     const wxString &name,
                                     const wxString &title, 
                                     const GmatTree::ItemType type,
                                     wxWindowID id, 
                                     const wxPoint &pos, 
                                     const wxSize &size, 
                                     long style)
   : wxMDIChildFrame(parent, id, title, pos, size, style, name)
{
   #ifdef DEBUG_MDI_CHILD_FRAME
   MessageInterface::ShowMessage
      ("GmatMdiChildFrame::GmatMdiChildFrame() entered, type=%d\n    "
       "name='%s'\n   title='%s', ", type, name.c_str(), title.c_str());
   #endif
   
   theParent = parent;
   mDirty = false;
   mOverrideDirty = false;
   mCanClose = true;
   mItemType = type;
   theScriptTextCtrl = NULL;
   theMenuBar = NULL;
   theAssociatedWin = NULL;
   
   #ifdef __USE_STC_EDITOR__
   theEditor = NULL;
   #endif
   
   #ifdef __WXMAC__
   childTitle = title;
   #endif
   
   #ifdef __CREATE_CHILD_MENU_BAR__
      // create a menu bar, pass Window menu if Windows
      #ifdef __WXMSW__
         theMenuBar = new GmatMenuBar(mItemType, parent->GetWindowMenu());
      #else
         theMenuBar = new GmatMenuBar(mItemType, NULL);
      #endif
   
      #ifdef DEBUG_MENUBAR
      MessageInterface::ShowMessage
         ("GmatMdiChildFrame::GmatMdiChildFrame() theMenuBarCreated %p\n", theMenuBar);
      #endif
      
      // Commented out so that Window menu works for MdiChildFrame (loj: 2008.02.08)
      // Double Window menu appears when more than one child is open and cannot
      // delete theMenuBar in the destructor.
      //SetMenuBar(theMenuBar);
   #else
      theMenuBar = (GmatMenuBar*)(parent->GetMenuBar());      
   #endif
   
   // Enable Edit menu and tools if ScriptFile
   #ifdef DEBUG_UPDATE_GUI_ITEM
   MessageInterface::ShowMessage
      ("GmatMdiChildFrame() constructor calling UpdateGuiItem()\n");
   #endif
   UpdateGuiItem(1, 0);
   
   // Set icon if icon file is in the start up file
   FileManager *fm = FileManager::Instance();
   try
   {
      wxString iconfile = fm->GetFullPathname("MAIN_ICON_FILE").c_str();
      #if defined __WXMSW__
         SetIcon(wxIcon(iconfile, wxBITMAP_TYPE_ICO));
      #elif defined __WXGTK__
         SetIcon(wxIcon(iconfile, wxBITMAP_TYPE_XPM));
      #elif defined __WXMAC__
         SetIcon(wxIcon(iconfile, wxBITMAP_TYPE_PICT_RESOURCE));
      #endif
   }
   catch (GmatBaseException &e)
   {
      MessageInterface::ShowMessage(e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// ~GmatMdiChildFrame()
//------------------------------------------------------------------------------
GmatMdiChildFrame::~GmatMdiChildFrame()
{
   #ifdef DEBUG_MDI_CHILD_FRAME
   MessageInterface::ShowMessage
      ("GmatMdiChildFrame::~GmatMdiChildFrame() name='%s', title='%s' entered\n",
       GetName().c_str(), GetTitle().c_str());
   #endif
   
   #ifdef __CREATE_CHILD_MENU_BAR__
      delete theMenuBar;
   #else
      // There should be only one MenuBar associated with GmatMainFrame,
      // so we cannot delete it here.
      // Disable Edit menu and tools
      #ifdef DEBUG_UPDATE_GUI_ITEM
      MessageInterface::ShowMessage
         ("GmatMdiChildFrame() destructor calling UpdateGuiItem()\n");
      #endif
      UpdateGuiItem(2, 0);
   #endif
      
   #ifdef DEBUG_MDI_CHILD_FRAME
   MessageInterface::ShowMessage
      ("GmatMdiChildFrame::~GmatMdiChildFrame() name='%s', title='%s' exiting\n",
       GetName().c_str(), GetTitle().c_str());
   #endif
}


//------------------------------------------------------------------------------
// wxMenuBar* GetMenuBar()
//------------------------------------------------------------------------------
wxMenuBar* GmatMdiChildFrame::GetMenuBar()
{
   return theMenuBar;
}


//------------------------------------------------------------------------------
// GmatTree::ItemType GetItemType()
//------------------------------------------------------------------------------
GmatTree::ItemType GmatMdiChildFrame::GetItemType()
{
   return mItemType;
}


//------------------------------------------------------------------------------
// void SetDataType(GmatTree::ItemType type)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::SetDataType(GmatTree::ItemType type)
{
   mItemType = type;
}


//------------------------------------------------------------------------------
// wxTextCtrl *GetScriptTextCtrl()
//------------------------------------------------------------------------------
wxTextCtrl* GmatMdiChildFrame::GetScriptTextCtrl()
{
   return theScriptTextCtrl;
}


//------------------------------------------------------------------------------
// void SetScriptTextCtrl(wxTextCtrl *textCtrl)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::SetScriptTextCtrl(wxTextCtrl *textCtrl)
{
   theScriptTextCtrl = textCtrl;
}


#ifdef __USE_STC_EDITOR__
//------------------------------------------------------------------------------
// Editor* GetEditor()
//------------------------------------------------------------------------------
Editor* GmatMdiChildFrame::GetEditor()
{
   return theEditor;
}


//------------------------------------------------------------------------------
// void SetEditor(Editor *editor)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::SetEditor(Editor *editor)
{
   theEditor = editor;
}
#endif


//------------------------------------------------------------------------------
// void SetDirty(bool dirty)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::SetDirty(bool dirty)
{
   mDirty = dirty;
}


//------------------------------------------------------------------------------
// void OverrideDirty(bool flag)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::OverrideDirty(bool flag)
{
   mOverrideDirty = flag;
}


//------------------------------------------------------------------------------
// bool IsDirty()
//------------------------------------------------------------------------------
bool GmatMdiChildFrame::IsDirty()
{
   return mDirty;
}


//------------------------------------------------------------------------------
// bool CanClose()
//------------------------------------------------------------------------------
bool GmatMdiChildFrame::CanClose()
{
   return mCanClose;
}


//------------------------------------------------------------------------------
// wxWindow* GetAssociatedWindow()
//------------------------------------------------------------------------------
wxWindow* GmatMdiChildFrame::GetAssociatedWindow()
{
   return theAssociatedWin;
}


//------------------------------------------------------------------------------
// void SetAssociatedWindow(wxWindow *win)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::SetAssociatedWindow(wxWindow *win)
{
   theAssociatedWin = win;
}


//------------------------------------------------------------------------------
//void OnActivate(wxActivateEvent &event)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::OnActivate(wxActivateEvent &event)
{
   #ifdef DEBUG_ACTIVATE
   MessageInterface::ShowMessage
      ("GmatMdiChildFrame::OnActivate() entered, title='%s', mItemType=%d\n",
       GetTitle().c_str(), mItemType);
   #endif
   
   // update both edit and animation tools
   #ifdef DEBUG_UPDATE_GUI_ITEM
   MessageInterface::ShowMessage
      ("GmatMdiChildFrame()::OnActivate calling UpdateGuiItem()\n");
   #endif
   UpdateGuiItem(1, 1);
   
   event.Skip();
}


//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent &event)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::OnClose(wxCloseEvent &event)
{
   #ifdef DEBUG_MDI_CHILD_FRAME
   MessageInterface::ShowMessage
      ("GmatMdiChildFrame::OnClose() entered, mDirty=%d, mOverrideDirty=%d\n",
       mDirty, mOverrideDirty);
   #endif
   
   mCanClose = true;
   
   // We don't want to show duplicate save message, so check for override dirty flag
   if (mOverrideDirty)
   {
      #ifdef __USE_STC_EDITOR__
      if (theEditor)
      {
         if (theEditor->IsModified())
            mDirty = true;
         else
            mDirty = false;
      }
      #else
      if (theScriptTextCtrl)
      {
         if (theScriptTextCtrl->IsModified())
            mDirty = true;
         else
            mDirty = false;
      }
      #endif
   }
   
   // check if window is dirty?
   if (mDirty)
   {
      #ifdef DEBUG_MDI_CHILD_FRAME
      MessageInterface::ShowMessage
         ("   Panel has been changed, show exit confirm message\n");
      #endif
      
      if ( wxMessageBox(_T("There were changes made to \"" + GetTitle() + "\" panel"
                           " which will be lost on Close. \nDo you want to close anyway?"),
                        _T("Please Confirm Close"),
                        wxICON_QUESTION | wxYES_NO) != wxYES )
      {
         event.Veto();
         mCanClose = false;
         
         #ifdef DEBUG_MDI_CHILD_FRAME
         MessageInterface::ShowMessage
            ("GmatMdiChildFrame::OnClose() exiting, close event was vetoed\n");
         #endif
         
         return;
      }
   }
   else
   {
      #ifdef DEBUG_MDI_CHILD_FRAME
      MessageInterface::ShowMessage("   No changes made to panel\n");
      #endif
   }
   
   // remove from list of frames
   GmatAppData::Instance()->GetMainFrame()->RemoveChild(GetName(), mItemType);   
   wxSafeYield();
   
   #ifdef DEBUG_MDI_CHILD_FRAME
   MessageInterface::ShowMessage("GmatMdiChildFrame::OnClose() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void UpdateScriptActiveStatus(bool isActive)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::UpdateScriptActiveStatus(bool isActive)
{
   // Do nothing here
}


#ifdef __WXMAC__
//------------------------------------------------------------------------------
// void SetTitle(wxString newTitle)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::SetTitle(wxString newTitle)
{
   childTitle = newTitle;
}


//------------------------------------------------------------------------------
// wxString GetTitle()
//------------------------------------------------------------------------------
wxString GmatMdiChildFrame::GetTitle()
{
   if (childTitle.IsNull())
      return "";
   else
      return childTitle;
}
#endif


//------------------------------------------------------------------------------
// void UpdateEditGuiItem(int updateEdit, int updateAnimation)
//------------------------------------------------------------------------------
/**
 * Updates Edit menu items and tools.
 *
 * @param  updateEdit
 *            0, Ignore edit menu and tools
 *            1, Update edit menu and tools
 *            2, Disable edit menu and tools
 * @param  updateAnimation
 *            0, Ignore animation tools
 *            1, Update animation tools
 *            2, Disable animation tools
 */
//------------------------------------------------------------------------------
void GmatMdiChildFrame::UpdateGuiItem(int updateEdit, int updateAnimation)
{
   #ifdef DEBUG_UPDATE_GUI_ITEM
   MessageInterface::ShowMessage
      ("GmatMdiChildFrame::UpdateGuiItem() updateEdit=%d, updateAnimation=%d\n",
       updateEdit, updateAnimation);
   #endif
   
   wxToolBar *toolBar = theParent->GetToolBar();
   if (toolBar == NULL)
      return;
   
   int editIndex = theMenuBar->FindMenu("Edit");
   
   //------------------------------------------------------------
   // update edit from menubar and toolbar
   //------------------------------------------------------------
   // Update MenuBar for this child
   if (updateEdit == 1 && (mItemType == GmatTree::SCRIPT_FILE ||
                           mItemType == GmatTree::GMAT_FUNCTION ||
                           mItemType == GmatTree::SCRIPT_EVENT))
   {
      theMenuBar->EnableTop(editIndex, true);
      toolBar->EnableTool(GmatMenu::MENU_EDIT_CUT, TRUE);
      toolBar->EnableTool(GmatMenu::MENU_EDIT_COPY, TRUE);
      toolBar->EnableTool(GmatMenu::MENU_EDIT_PASTE, TRUE);
   }
   else
   {
      theMenuBar->EnableTop(editIndex, false);
      toolBar->EnableTool(GmatMenu::MENU_EDIT_CUT, FALSE);
      toolBar->EnableTool(GmatMenu::MENU_EDIT_COPY, FALSE);
      toolBar->EnableTool(GmatMenu::MENU_EDIT_PASTE, FALSE);
   }
   
   //------------------------------------------------------------
   // update animation icons from toolbar
   //------------------------------------------------------------
   // If mission is running, ignore   
   if (updateAnimation == 1 && mItemType == GmatTree::OUTPUT_ORBIT_VIEW)
   {
      // If Play button is enabled, mission is not running
      if (toolBar->GetToolEnabled(GmatMenu::TOOL_RUN))
      {
         toolBar->EnableTool(GmatMenu::TOOL_ANIMATION_PLAY, true);
         toolBar->EnableTool(GmatMenu::TOOL_ANIMATION_STOP, true);
         toolBar->EnableTool(GmatMenu::TOOL_ANIMATION_FAST, true);
         toolBar->EnableTool(GmatMenu::TOOL_ANIMATION_SLOW, true);
         #ifdef DEBUG_UPDATE_GUI_ITEM
         MessageInterface::ShowMessage("   Animation tools enabled\n");
         #endif
      }
      else
      {
         #ifdef DEBUG_UPDATE_GUI_ITEM
         MessageInterface::ShowMessage
            ("   Cannot enable animation tools, mission is still running\n");
         #endif
      }
   }
   else
   {
      toolBar->EnableTool(GmatMenu::TOOL_ANIMATION_PLAY, false);
      toolBar->EnableTool(GmatMenu::TOOL_ANIMATION_STOP, false);
      toolBar->EnableTool(GmatMenu::TOOL_ANIMATION_FAST, false);
      toolBar->EnableTool(GmatMenu::TOOL_ANIMATION_SLOW, false);
   }
   
   #ifdef DEBUG_UPDATE_GUI_ITEM
   MessageInterface::ShowMessage
      ("GmatMdiChildFrame::UpdateGuiItem() exiting\n");
   #endif
}
