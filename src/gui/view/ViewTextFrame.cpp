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
 * Shows text.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "ViewTextFrame.hpp"
#include "GmatAppData.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"

BEGIN_EVENT_TABLE(ViewTextFrame, wxFrame)
   EVT_MENU(VIEW_TEXT_CLEAR, ViewTextFrame::OnClear)
   EVT_MENU(VIEW_TEXT_SAVE_AS, ViewTextFrame::OnSaveAs)
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
                             int x, int y, int w, int h, const wxString &mode,
                             const wxString &type)
   : wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h),
             wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT)
{
   CreateStatusBar(2);
   mWindowMode = mode;
   mTextType = type;
   mTextCtrl = new wxTextCtrl(this, -1, _T(""), wxPoint(0, 0), wxSize(0, 0),
                              wxTE_MULTILINE | wxTE_READONLY);
   mTextCtrl->SetMaxLength(320000); //loj: 12/07/06
   mTextCtrl->SetFont(GmatAppData::GetFont());
   
#if wxUSE_MENUS
   // create a menu bar 
   SetMenuBar(CreateMainMenu());
#endif // wxUSE_MENUS

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
   
   CenterOnScreen(wxBOTH);
}


//------------------------------------------------------------------------------
// ~ViewTextFrame()
//------------------------------------------------------------------------------
ViewTextFrame::~ViewTextFrame()
{
   if (mWindowMode != "Temporary")
   {
      GmatAppData::theCompareWindow = NULL;
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
// void OnSaveAs(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void ViewTextFrame::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{
   //MessageInterface::ShowMessage("ViewTextFrame::OnSaveAs() entered\n");

   wxString filename;
   if (mTextType == "Script")
   {
      filename =
         wxFileSelector("Choose a file to save", "", "", "script",
                        "Script files (*.script)|*.script", wxSAVE);
   }
   else
   {
      filename =
         wxFileSelector("Choose a file to save", "", "", "txt",
                        "Report files (*.report)|*.report|Text files (*.txt)|*.txt",
                        wxSAVE);
   }
   
   if (!filename.empty())
      mTextCtrl->SaveFile(filename);
   
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
   menuFile->Append(VIEW_TEXT_CLEAR, _T("&Clear"));
   menuFile->AppendSeparator();
   menuFile->Append(VIEW_TEXT_SAVE_AS, _T("&Save As..."));
   menuFile->AppendSeparator();
   menuFile->Append(VIEW_TEXT_EXIT, _T("E&xit"));
   menuBar->Append(menuFile, _T("&File"));

   return menuBar;
}
