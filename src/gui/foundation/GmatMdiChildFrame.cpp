//$Header$
//------------------------------------------------------------------------------
//                             GmatNotebook
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

GmatMdiChildFrame::GmatMdiChildFrame(wxMDIParentFrame* parent, 
                       wxWindowID id, 
                       const wxString& title, 
                       const wxPoint& pos, 
                       const wxSize& size, 
                       long style, 
                       const wxString& name)
                 :wxMDIChildFrame(parent, id, title, pos, size, style, name)
{
   this->title.Printf("%s", title.c_str());
}

GmatMdiChildFrame::~GmatMdiChildFrame()
{
}


void GmatMdiChildFrame::OnClose()
{
   // check if window is dirty?
  
   // remove from list of frames
   GmatAppData::GetMainFrame()->RemoveChild(title);

//   event.Skip();
}

wxString GmatMdiChildFrame::GetTitle()
{
   return title;
}

