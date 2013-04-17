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
// Author: Allison Greene
// Created: 2003/08/28
/**
 * This class provides the layout of a mdi child frame
 */
//------------------------------------------------------------------------------

#include "GmatMdiChildFrame.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "SubscriberException.hpp"
#include "GuiInterpreter.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include "GuiItemManager.hpp"
#include "GmatTreeItemData.hpp"
#include "GmatSavePanel.hpp"
#include "GmatMenuBar.hpp"       // for namespace GmatMenu
#include <wx/fileconf.h>
#include <wx/config.h>

// @todo We cannot create own MenuBar yet.
// Double Window menu appears when more than one child is open and cannot
// delete theMenuBar in the destructor.
//#define __CREATE_CHILD_MENU_BAR__

//#define DEBUG_MDI_CHILD_FRAME
//#define DEBUG_UPDATE_GUI_ITEM
//#define DEBUG_CLOSE
//#define DEBUG_PERSISTENCE
//#define DEBUG_ACTIVATE
//#define DEBUG_ICONIZE


Integer GmatMdiChildFrame::maxZOrder = 0;

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
   EVT_ICONIZE(GmatMdiChildFrame::OnIconize)
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
       "name='%s'\n   title='%s'\n", type, name.c_str(), title.c_str());
   #endif
   
   relativeZOrder          = maxZOrder++;
   usingSavedConfiguration = false;
   mChildName = name;
   theParent = parent;
   mDirty = false;
   mOverrideDirty = false;
   mCanClose = true;
   mCanSaveLocation = true;
   mIsActiveChild = false;
   mCanBeDeleted = true;
   mItemType = type;
   theScriptTextCtrl = NULL;
   theMenuBar = NULL;
   theAssociatedWin = NULL;
   
   GmatAppData *gmatAppData = GmatAppData::Instance();
   theGuiInterpreter = gmatAppData->GetGuiInterpreter();

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
      ("GmatMdiChildFrame::~GmatMdiChildFrame() entered, name='%s'\n   title='%s'\n",
       GetName().c_str(), GetTitle().c_str());
   #endif
   
   #ifdef __CREATE_CHILD_MENU_BAR__
      delete theMenuBar;
   #else
      // There should be only one MenuBar associated with GmatMainFrame,
      // so we cannot delete it here. Enable or disable edit menu and tools.
      #ifdef DEBUG_UPDATE_GUI_ITEM
      MessageInterface::ShowMessage
         ("GmatMdiChildFrame() destructor calling UpdateGuiItem()\n");
      #endif
      UpdateGuiItem(0, 0);
   #endif
      
   #ifdef DEBUG_MDI_CHILD_FRAME
   MessageInterface::ShowMessage
      ("GmatMdiChildFrame::~GmatMdiChildFrame() leaving, name='%s'\n   title='%s'\n",
       GetName().c_str(), GetTitle().c_str());
   #endif
}

//------------------------------------------------------------------------------
// wxString GetPlotName()
//------------------------------------------------------------------------------
wxString GmatMdiChildFrame::GetPlotName()
{
   return mChildName;
}

//------------------------------------------------------------------------------
// void SetPlotName(const wxString &name)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::SetPlotName(const wxString &name)
{
   mChildName = name;
}

//------------------------------------------------------------------------------
// void SetSavedConfigFlag(bool isPreset)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::SetSavedConfigFlag(bool isPreset)
{
   usingSavedConfiguration = isPreset;
}

//------------------------------------------------------------------------------
// bool GetSavedConfigFlag()
//------------------------------------------------------------------------------
bool GmatMdiChildFrame::GetSavedConfigFlag()
{
   return usingSavedConfiguration;
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


//------------------------------------------------------------------------------
// void SetActiveChild(bool active)
//------------------------------------------------------------------------------
/**
 * Sets isActiveChild flag for the current mdi child frame.
 */
//------------------------------------------------------------------------------
void GmatMdiChildFrame::SetActiveChild(bool active)
{
   mIsActiveChild = active;
}


//------------------------------------------------------------------------------
// bool IsActiveChild()
//------------------------------------------------------------------------------
/**
 * Returns isActiveChild flag for the current mdi child frame.
 */
//------------------------------------------------------------------------------
bool GmatMdiChildFrame::IsActiveChild()
{
   return mIsActiveChild;
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
// void SetCanBeDeleted(bool flag)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::SetCanBeDeleted(bool flag)
{
   mCanBeDeleted = flag;
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
// wxWindow* GetMdiParent()
//------------------------------------------------------------------------------
/**
 * Returns MdiParent frame of this MdiChild frame.
 */
//------------------------------------------------------------------------------
wxWindow* GmatMdiChildFrame::GetMdiParent()
{
   return theParent;
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
      ("\nGmatMdiChildFrame::OnActivate() entered, title='%s', mItemType=%d\n",
       GetTitle().c_str(), mItemType);
   #endif
   
   // update both edit and animation tools
   #ifdef DEBUG_UPDATE_GUI_ITEM
   MessageInterface::ShowMessage
      ("GmatMdiChildFrame::OnActivate calling UpdateGuiItem()\n");
   #endif
   UpdateGuiItem(1, 1);
   UpdateActiveChild();
   
   relativeZOrder = maxZOrder++;
   #ifdef DEBUG_ACTIVATE
   MessageInterface::ShowMessage
      ("GmatMdiChildFrame::OnActivate() leaving, title='%s'\n   zOrder set to %d, and maxZOrder set to %d\n",
       GetTitle().c_str(), relativeZOrder, maxZOrder);
   #endif
   event.Skip();
}


//------------------------------------------------------------------------------
//void OnIconize(wxIconizeEvent &event)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::OnIconize(wxIconizeEvent &event)
{
   #ifdef DEBUG_ICONIZE
   MessageInterface::ShowMessage
      ("\nGmatMdiChildFrame::OnIconize() entered, title='%s', mItemType=%d\n",
       GetTitle().c_str(), mItemType);
   #endif
   
   // Update both edit and animation tools if frame is restored
   #ifdef DEBUG_UPDATE_GUI_ITEM
   MessageInterface::ShowMessage("   Calling UpdateGuiItem()\n");
   #endif
   if (event.Iconized())
   {
      #ifdef DEBUG_UPDATE_GUI_ITEM
      MessageInterface::ShowMessage("   Frame is iconized.\n");
      #endif
      UpdateGuiItem(0, 0);
   }
   else
   {
      #ifdef DEBUG_UPDATE_GUI_ITEM
      MessageInterface::ShowMessage("   Frame is restored.\n");
      #endif
      UpdateGuiItem(1, 1);
   }
   
   #ifdef DEBUG_ICONIZE
   MessageInterface::ShowMessage
      ("GmatMdiChildFrame::OnIconize() leaving, title='%s'\n", GetTitle().c_str());
   #endif
   event.Skip();
}


//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent &event)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::OnClose(wxCloseEvent &event)
{
   #ifdef DEBUG_CLOSE
   MessageInterface::ShowMessage
      ("GmatMdiChildFrame::OnClose() entered, mDirty=%d, mOverrideDirty=%d\n",
       mDirty, mOverrideDirty);
   #endif
   
   SetFocus();
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
      #ifdef DEBUG_CLOSE
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
         
         #ifdef DEBUG_CLOSE
         MessageInterface::ShowMessage
            ("GmatMdiChildFrame::OnClose() exiting, close event was vetoed\n");
         #endif
         
         return;
      }
   }
   else
   {
      #ifdef DEBUG_CLOSE
      MessageInterface::ShowMessage("   No changes made to panel\n");
      #endif
   }
   
   #ifdef DEBUG_CLOSE
   MessageInterface::ShowMessage("GmatMdiChildFrame::OnClose() Saving position and size\n");
   #endif
   SaveChildPositionAndSize();
   
   GmatSavePanel* panel = ((GmatSavePanel*) GetAssociatedWindow());
   #ifdef DEBUG_CLOSE
   MessageInterface::ShowMessage("   Associated window = <%p>\n", panel);
   #endif
   if (panel != NULL)
   {
	   if (!panel->UpdateStatusOnClose()) 
	   {
         event.Veto();
		   mCanClose = false;
         #ifdef DEBUG_CLOSE
         MessageInterface::ShowMessage
            ("GmatMdiChildFrame::OnClose() leaving, window cannot be closed\n");
         #endif
		   return;
	   }
   }
   
   // Remove from the list of mdi children, but do not delete output child in the
   // GmatMainFrame::RemoveChild() (2012.01.17 LOJ: This fixes crash when file->exit)
   #ifdef DEBUG_CLOSE
   MessageInterface::ShowMessage
      ("GmatMdiChildFrame::OnClose() calling GetMainFrame::RemoveChild()\n");
   #endif
   
   GmatAppData::Instance()->GetMainFrame()->RemoveChild(GetName(), mItemType, mCanBeDeleted);   
   wxSafeYield();
   // This causes crash on exit with Red X/button on XP and Mac - wcs 2013.03.20
//   event.Skip();
   
   #ifdef DEBUG_CLOSE
   MessageInterface::ShowMessage("GmatMdiChildFrame::OnClose() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void UpdateScriptActiveStatus(bool isActive)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::UpdateScriptActiveStatus(bool isActive)
{
   // Do nothing here
}

//------------------------------------------------------------------------------
// void SetSaveLocationFlag(bool tf)
//------------------------------------------------------------------------------
/**
 * Allows the programmer to turn off location saving for a frame
 *
 * OwnedPlot objects use this method to turn off saving, which is currently
 * not working.
 *
 * @param tf The flag value; true to enable saving, false to disable it
 */
//------------------------------------------------------------------------------
void GmatMdiChildFrame::SetSaveLocationFlag(bool tf)
{
   mCanSaveLocation = tf;
}

//------------------------------------------------------------------------------
// void SaveChildPositionAndSize()
//------------------------------------------------------------------------------
void GmatMdiChildFrame::SaveChildPositionAndSize()
{
   if (mCanSaveLocation == false)
      return;

   if (IsIconized())
      return;
   
   // Get the position and size of the window first
   #ifdef __WXMAC__
      Integer screenWidth  = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
      Integer screenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
   #else
      Integer screenWidth;
      Integer screenHeight;
      GmatAppData::Instance()->GetMainFrame()->GetActualClientSize(&screenWidth, &screenHeight, true);
      // Since GmatMainFrame::GetActualClientSize() subtracts one, add one here (LOJ: 2012.07.23)
      screenWidth++;
      screenHeight++;
   #endif

   bool isMinimized = IsIconized(), isMaximized = IsMaximized();
   if (isMinimized)	
	  Iconize(false);
   else if (isMaximized)
	  Maximize(false);

   int tmpX = -1, tmpY = -1;
   int tmpW = -1, tmpH = -1;
   GetPosition(&tmpX, &tmpY);
   GetSize(&tmpW, &tmpH);
   Rvector upperLeft(2, ((Real) tmpX /(Real)  screenWidth), ((Real) tmpY /(Real)  screenHeight));
   Rvector childSize(2,  ((Real) tmpW /(Real)  screenWidth), ((Real) tmpH /(Real)  screenHeight));

   if (isMinimized)	
	  Iconize();
   else if (isMaximized)
	  Maximize();


   #ifdef DEBUG_PERSISTENCE
   // ======================= begin temporary ==============================
   MessageInterface::ShowMessage("*** Size of SCREEN %s is: width = %d, height = %d\n", mChildName.c_str(), screenWidth, screenHeight);
   MessageInterface::ShowMessage("Position of View plot %s is: x = %d, y = %d\n", mChildName.c_str(), tmpX, tmpY);
   MessageInterface::ShowMessage("Size of View plot %s is: width = %d, height = %d\n", mChildName.c_str(), tmpW, tmpH);
   // ======================= end temporary ==============================
   #endif

   if ((mItemType == GmatTree::OUTPUT_REPORT)  ||
       (mItemType == GmatTree::OUTPUT_CCSDS_OEM_FILE ) ||
       (mItemType == GmatTree::OUTPUT_ORBIT_VIEW) ||
       (mItemType == GmatTree::OUTPUT_XY_PLOT) ||
       (mItemType == GmatTree::OUTPUT_GROUND_TRACK_PLOT)
       // We'll want to add the event reports eventually, but they are not subscriber based
       //|| (mItemType == GmatTree::EVENT_REPORT)
       )
   {
      GmatBase *obj =
         (Subscriber*)theGuiInterpreter->GetConfiguredObject(mChildName.c_str());

      if (!obj || !obj->IsOfType("Subscriber"))
      {
         std::string errmsg = "Cannot find subscriber ";
         errmsg += mChildName + "\n";
         throw SubscriberException(errmsg);
      }
      Subscriber *sub = (Subscriber*) obj;

      #ifdef DEBUG_PERSISTENCE
         MessageInterface::ShowMessage("...... Now saving plot data to %s:\n", (sub->GetName()).c_str());
         MessageInterface::ShowMessage("       Upper left             = %12.10f   %12.10f\n", upperLeft[0], upperLeft[1]);
         MessageInterface::ShowMessage("       Size                   = %12.10f   %12.10f\n", childSize[0], childSize[1]);
         MessageInterface::ShowMessage("       RelativeZOrder         = %d\n", relativeZOrder);
      #endif
      sub->SetRvectorParameter(sub->GetParameterID("UpperLeft"), upperLeft);
      sub->SetRvectorParameter(sub->GetParameterID("Size"), childSize);
      sub->SetIntegerParameter(sub->GetParameterID("RelativeZOrder"), relativeZOrder);
	  sub->SetBooleanParameter(sub->GetParameterID("Maximized"), isMaximized);
   }
   else if (mItemType == GmatTree::MISSION_TREE_UNDOCKED)
   {
      // get the config object
      wxFileConfig *pConfig;
      pConfig = (wxFileConfig *) GmatAppData::Instance()->GetPersonalizationConfig();
      std::stringstream location("");
      location << upperLeft[0] << " " << upperLeft[1];
      std::stringstream size("");
      size << childSize[0] << " " << childSize[1];
      pConfig->Write("/MissionTree/UpperLeft", location.str().c_str());
      pConfig->Write("/MissionTree/Size", size.str().c_str());
	  pConfig->Write("/MissionTree/IsMaximized", isMaximized);
	  pConfig->Write("/MissionTree/IsMinimized", isMinimized);
   }
   else if (mItemType == GmatTree::SCRIPT_FILE)
   {
      // get the config object
      wxFileConfig *pConfig;
      pConfig = (wxFileConfig *) GmatAppData::Instance()->GetPersonalizationConfig();
      std::stringstream location("");
      location << upperLeft[0] << " " << upperLeft[1];
      std::stringstream size("");
      size << childSize[0] << " " << childSize[1];
      pConfig->Write("/ScriptEditor/UpperLeft", location.str().c_str());
      pConfig->Write("/ScriptEditor/Size", size.str().c_str());
	  pConfig->Write("/ScriptEditor/IsMaximized", isMaximized);
	  pConfig->Write("/ScriptEditor/IsMinimized", isMinimized);
   }
}

#ifdef __WXMAC__
//------------------------------------------------------------------------------
// void SetTitle(wxString newTitle)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::SetTitle(wxString newTitle)
{
   childTitle = newTitle;
   // Need to set the actual window name so that Rename/Delete work properly
   SetName(newTitle);
   mChildName = newTitle;  // but is this correct?
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
 *            1, Update edit menu and tools
 *            != 1, Disable edit menu and tools
 * @param  updateAnimation
 *            1, Update animation tools
 *            != 1, Disable animation tools
 */
//------------------------------------------------------------------------------
void GmatMdiChildFrame::UpdateGuiItem(int updateEdit, int updateAnimation)
{
   #ifdef DEBUG_UPDATE_GUI_ITEM
   MessageInterface::ShowMessage
      ("GmatMdiChildFrame::UpdateGuiItem() entered, name='%s'\n   updateEdit=%d, "
       "updateAnimation=%d, iconized=%d\n", mChildName.c_str(), updateEdit,
       updateAnimation, IsIconized());
   #endif
   
   wxToolBar *toolBar = theParent->GetToolBar();
   if (toolBar == NULL)
      return;
   
   bool isAnimatable = ((GmatMainFrame*)theParent)->IsAnimatable();
   
   #ifdef DEBUG_UPDATE_GUI_ITEM
   bool isAnimationRunning = ((GmatMainFrame*)theParent)->IsAnimationRunning();
   MessageInterface::ShowMessage
      ("   IsMissionRunning=%d, IsAnimationRunning=%d, isAnimatable=%d\n",
       isMissionRunning, isAnimationRunning, isAnimatable);
   #endif
   
   int editIndex = theMenuBar->FindMenu("Edit");
   
   //------------------------------------------------------------
   // update edit from menubar and toolbar
   //------------------------------------------------------------
   // Update MenuBar for this child
   if (updateEdit == 1 && !IsIconized() &&
       (mItemType == GmatTree::SCRIPT_FILE ||
        mItemType == GmatTree::GMAT_FUNCTION ||
        mItemType == GmatTree::SCRIPT_EVENT))
   {
      theMenuBar->EnableTop(editIndex, true);
      toolBar->EnableTool(GmatMenu::MENU_EDIT_CUT, true);
      toolBar->EnableTool(GmatMenu::MENU_EDIT_COPY, true);
      toolBar->EnableTool(GmatMenu::MENU_EDIT_PASTE, true);
   }
   else
   {
      theMenuBar->EnableTop(editIndex, false);
      toolBar->EnableTool(GmatMenu::MENU_EDIT_CUT, false);
      toolBar->EnableTool(GmatMenu::MENU_EDIT_COPY, false);
      toolBar->EnableTool(GmatMenu::MENU_EDIT_PASTE, false);
   }
   
   //------------------------------------------------------------
   // update animation icons from toolbar
   //------------------------------------------------------------
   // If mission is running, ignore   
   if (updateAnimation == 1 && !IsIconized() &&
      (mItemType == GmatTree::OUTPUT_ORBIT_VIEW ||
       mItemType == GmatTree::OUTPUT_GROUND_TRACK_PLOT))
   {
      // If mission is not running, enable screen shot and animation tools
      if (isAnimatable)
      {
         toolBar->EnableTool(GmatMenu::TOOL_SCREENSHOT, true);
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
      toolBar->EnableTool(GmatMenu::TOOL_SCREENSHOT, false);
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


//------------------------------------------------------------------------------
// void UpdateActiveChild()
//------------------------------------------------------------------------------
/**
 * Updates mdi child active flag by going through all the children.
 */
//------------------------------------------------------------------------------
void GmatMdiChildFrame::UpdateActiveChild()
{
   wxList *mdiChildren = ((GmatMainFrame*)theParent)->GetListOfMdiChildren();
   GmatMdiChildFrame *theChild = NULL;
   wxNode *node = mdiChildren->GetFirst();
   while (node)
   {
      theChild = (GmatMdiChildFrame *)node->GetData();
      
      #ifdef DEBUG_UPDATE_ACTIVE_CHILD
      MessageInterface::ShowMessage
         ("   theChild=%s\n", theChild->GetName().c_str());
      #endif
      
      if (theChild->GetName().IsSameAs(mChildName))
         theChild->SetActiveChild(true);
      else
         theChild->SetActiveChild(false);
      
      node = node->GetNext();
   }
}
