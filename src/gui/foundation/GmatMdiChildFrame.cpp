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
#include "GuiInterpreter.hpp"
#include "FileManager.hpp"

#include "GuiItemManager.hpp"

using namespace GmatMenu;

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
GmatMdiChildFrame::GmatMdiChildFrame(wxMDIParentFrame* parent, 
                                     const wxString& name,
                                     const wxString& title, 
                                     const int type,
                                     wxWindowID id, 
                                     const wxPoint& pos, 
                                     const wxSize& size, 
                                     long style)
   : wxMDIChildFrame(parent, id, title, pos, size, style, name)
{
   mDirty = false;
   dataType = type;
   
#ifdef __WXMAC__
   //this->title.Printf("%s", title.c_str());
   this->childTitle = title.c_str();
#endif

   menuBar = new GmatMenuBar(dataType);
   SetMenuBar(menuBar);
   
   // Set icon if icon file is in the start up file
   FileManager *fm = FileManager::Instance();
   try
   {
      wxString iconfile = fm->GetFullPathname("MAIN_ICON_FILE").c_str();
      #if defined __WXMSW__
         SetIcon(wxIcon(iconfile, wxBITMAP_TYPE_ICO));
      #elif defined __WXGTK__
         SetIcon(wxIcon(iconfile, wxBITMAP_TYPE_XPM));
      #elif defined __WXMAX__
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
   // check if window is dirty?
   if (mDirty)
   {
        if ( wxMessageBox(_T("Changes will be lost. \nReally close?"), _T("Please confirm"),
                          wxICON_QUESTION | wxYES_NO) != wxYES )
        {
            event.Veto();
            return;
        }
   }
   
   // remove from list of frames
   GmatAppData::GetMainFrame()->RemoveChild(GetTitle(), dataType);

//   event.Skip();
   wxSafeYield();
}

#ifdef __WXMAC__
//------------------------------------------------------------------------------
// void SetTitle(wxString newTitle)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::SetTitle(wxString newTitle)
{
//   SetTitle(newTitle);
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
// int GetDataType()
//------------------------------------------------------------------------------
int GmatMdiChildFrame::GetDataType()
{
   return dataType;
}


//------------------------------------------------------------------------------
// void SetDirty(bool dirty)
//------------------------------------------------------------------------------
void GmatMdiChildFrame::SetDirty(bool dirty)
{
    mDirty = dirty;
}


//------------------------------------------------------------------------------
// bool IsDirty()
//------------------------------------------------------------------------------
bool GmatMdiChildFrame::IsDirty()
{
    return mDirty;
}

