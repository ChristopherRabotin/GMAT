//$Header$
//------------------------------------------------------------------------------
//                                 ViewTextFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/11/03
//
/**
 * Defines show text.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "ViewTextFrame.hpp"
#include "GmatAppData.hpp"

BEGIN_EVENT_TABLE(ViewTextFrame, wxFrame)
    EVT_MENU(VIEW_TEXT_CLEAR, ViewTextFrame::OnClear)
    EVT_MENU(VIEW_TEXT_EXIT, ViewTextFrame::OnExit)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// ViewTextFrame(wxFrame *frame, const wxString& title,
//              int x, int y, int w, int h, const wxString &mode)
//------------------------------------------------------------------------------
ViewTextFrame::ViewTextFrame(wxFrame *frame, const wxString& title,
                             int x, int y, int w, int h, const wxString &mode)
    : wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
    CreateStatusBar(2);
    mWindowMode = mode;
    mTextCtrl = new wxTextCtrl(this, -1, _T(""), wxPoint(0, 0), wxSize(0, 0),
                               wxTE_MULTILINE | wxTE_READONLY);
#if wxUSE_MENUS
    // create a menu bar 
    SetMenuBar(CreateMainMenu());
#endif // wxUSE_MENUS

}

ViewTextFrame::~ViewTextFrame()
{
    if (mWindowMode != "Temporary")
    {
        GmatAppData::SetMessageWindow(NULL);
    }
}

//------------------------------------------------------------------------------
// void AppendText(const wxString& text)
//------------------------------------------------------------------------------
void ViewTextFrame::AppendText(const wxString& text)
{
    mTextCtrl->AppendText(text);
}

//------------------------------------------------------------------------------
// void OnClear(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void ViewTextFrame::OnClear(wxCommandEvent& WXUNUSED(event))
{
    mTextCtrl->Clear();
}

//------------------------------------------------------------------------------
// void OnExit(wxCommandEvent& WXUNUSED(event) )
//------------------------------------------------------------------------------
void ViewTextFrame::OnExit(wxCommandEvent& WXUNUSED(event) )
{
    if (mWindowMode == "Temporary")
        Close(true);
    else
        Show(false);
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// wxMenuBar* CreateMainMenu()
//------------------------------------------------------------------------------
wxMenuBar* ViewTextFrame::CreateMainMenu()
{
    // Make a menubar
    wxMenuBar *menuBar = new wxMenuBar;

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(VIEW_TEXT_CLEAR, _T("&Clear\tCtrl-L"));
    menuFile->AppendSeparator();
    menuFile->Append(VIEW_TEXT_EXIT, _T("E&xit\tAlt-X"));
    menuBar->Append(menuFile, _T("&File"));

    return menuBar;
}
