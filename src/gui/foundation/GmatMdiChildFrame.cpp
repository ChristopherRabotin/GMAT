//$Id$
//------------------------------------------------------------------------------
//                             GmatMdiChildFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
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
#include "GuiInterpreter.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include "GuiItemManager.hpp"

//#define DEBUG_MDI_CHILD_FRAME

//using namespace GmatMenu;

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
                                     const wxString &title, 
                                     const wxString &name,
                                     const GmatTree::ItemType type,
                                     wxWindowID id, 
                                     const wxPoint &pos, 
                                     const wxSize &size, 
                                     long style)
   : wxMDIChildFrame(parent, id, title, pos, size, style, name)
{
   mDirty = false;
   mItemType = type;
   mCanClose = true;
   
   #ifdef __WXMAC__
   childTitle = title;
   #endif
   
   // create a menu bar
   // pass Window menu if Windows
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
   //SetMenuBar(theMenuBar);
   
   // Disble all top menu items except Help, if output plot was created
   // This will be enabled when completed mission running
   if (mItemType == GmatTree::OUTPUT_OPENGL_PLOT ||
       mItemType == GmatTree::OUTPUT_XY_PLOT)
   {
      int helpIndex = theMenuBar->FindMenu("Help");
      int numMenu = theMenuBar->GetMenuCount();
      for (int i=0; i<numMenu; i++)
      {
         if (i != helpIndex)
            theMenuBar->EnableTop(i, false);
      }
   }
   
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
      //MessageInterface::ShowMessage(e.GetMessage());
   }
}


//------------------------------------------------------------------------------
// ~GmatMdiChildFrame()
//------------------------------------------------------------------------------
GmatMdiChildFrame::~GmatMdiChildFrame()
{
   delete theMenuBar;
}


//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent &event)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::OnClose(wxCloseEvent &event)
{
   #ifdef DEBUG_MDI_CHILD_FRAME
   MessageInterface::ShowMessage("GmatMdiChildFrame::OnClose() entered\n");
   #endif
   
   mCanClose = true;
   
   // check if window is dirty?
   if (mDirty)
   {
      #ifdef DEBUG_MDI_CHILD_FRAME
      MessageInterface::ShowMessage
         ("GmatMdiChildFrame::OnClose() show exit confirm message\n");
      #endif
      
      if ( wxMessageBox(_T("There were changes made to \"" + GetTitle() + "\" panel"
                           " which will be lost on Close. \nDo you want to close anyway?"),
                        _T("Please Confirm Close"),
                        wxICON_QUESTION | wxYES_NO) != wxYES )
      {
         event.Veto();
         mCanClose = false;
         return;
      }
   }
   
   // remove from list of frames
   GmatAppData::GetMainFrame()->RemoveChild(GetTitle(), mItemType);   
   wxSafeYield();
   
   #ifdef DEBUG_MDI_CHILD_FRAME
   MessageInterface::ShowMessage("GmatMdiChildFrame::OnClose() exiting\n");
   #endif
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


