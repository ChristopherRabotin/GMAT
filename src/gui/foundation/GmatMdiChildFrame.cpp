//$Header$
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
   
   SetMenuBar(theMenuBar);
   
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
      if ( wxMessageBox(_T("Changes will be lost. \nDo you really want to close?"),
                        _T("Please confirm"), wxICON_QUESTION | wxYES_NO) != wxYES )
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


