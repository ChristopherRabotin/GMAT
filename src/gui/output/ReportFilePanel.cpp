//$Id$
//------------------------------------------------------------------------------
//                              ReportFilePanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Linda Jun
// Created: 2004/02/02
//
/**
 * Implements ReportFilePanel class.
 */
//------------------------------------------------------------------------------

#include "ReportFilePanel.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "MessageInterface.hpp"

#include <wx/file.h> // for wxFile

//#define DEBUG_REPORT_FILE_PANEL

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ReportFilePanel, wxPanel)
   EVT_BUTTON(ID_BUTTON_CLOSE, ReportFilePanel::OnClose)
   EVT_MENU (ID_MENU_COPY, ReportFilePanel::OnCopy)
   EVT_MENU (ID_MENU_SELECTALL, ReportFilePanel::OnSelectAll)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// ReportFilePanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs ReportFilePanel object.
 *
 * @param <parent> parent window.
 *
 */
//------------------------------------------------------------------------------
ReportFilePanel::ReportFilePanel(wxWindow *parent, wxString reportName)
    : wxPanel(parent)
{
   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   mReportName = reportName;
   
   theParent = parent;
   
   theReport =
      (ReportFile*) theGuiInterpreter->GetRunningObject(mReportName.c_str());
   
   Create();
   
   if (theReport != NULL)
   {
      Show();
   }
   else
   {
      // show error message
      MessageInterface::ShowMessage
         ("**** ERROR **** ReportFilePanel:Create() the running ReportFile "
          "\"%s\" is NULL\n", reportName.c_str());
   }
   
}


//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void ReportFilePanel::Create()
//------------------------------------------------------------------------------
void ReportFilePanel::Create()
{
   int borderSize = 3;
   wxStaticBox *middleStaticBox = new wxStaticBox( this, -1, wxT("") );
   wxStaticBox *bottomStaticBox = new wxStaticBox( this, -1, wxT("") );
   
   // create sizers
   thePanelSizer = new wxBoxSizer(wxVERTICAL);
   theMiddleSizer = new wxStaticBoxSizer( middleStaticBox, wxVERTICAL );
   theBottomSizer = new wxStaticBoxSizer( bottomStaticBox, wxVERTICAL );
   wxBoxSizer *theButtonSizer = new wxBoxSizer(wxHORIZONTAL);
   wxGridSizer *theGridSizer = new wxGridSizer( 1, 0, 0 );
   
   // create the text ctrl
   mFileContentsTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                                           wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP |
                                           wxTE_READONLY | wxTE_MULTILINE | wxGROW);
   mFileContentsTextCtrl->Connect(wxEVT_RIGHT_DOWN,
                        wxMouseEventHandler(ReportFilePanel::OnRightMouseDown), NULL, this);
   
   // set font
   mFileContentsTextCtrl->SetFont( GmatAppData::Instance()->GetFont() );
   // create popup menu
   mPopupMenu = new wxMenu();
   mPopupMenu->Append(ID_MENU_COPY, "Copy");
   mPopupMenu->AppendSeparator();
   mPopupMenu->Append(ID_MENU_SELECTALL, "Select All");
      
   // create bottom buttons
   theCloseButton =
      new wxButton(this, ID_BUTTON_CLOSE, "Close", wxDefaultPosition, wxDefaultSize, 0);
   theHelpButton =
      new wxButton(this, ID_BUTTON_HELP, "Help", wxDefaultPosition, wxDefaultSize, 0);
   
   theGridSizer->Add(mFileContentsTextCtrl, 0, wxGROW | wxALIGN_CENTER | wxALL,
                     borderSize);
   theMiddleSizer->Add(theGridSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, borderSize);
   
   // adds the buttons to button sizer
   theButtonSizer->Add(theCloseButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(theHelpButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   
   theBottomSizer->Add(theButtonSizer, 0, wxALIGN_CENTER | wxALL, borderSize);
   
}


//------------------------------------------------------------------------------
// void Show()
//------------------------------------------------------------------------------
/**
 * Shows the panel.
 */
//------------------------------------------------------------------------------
void ReportFilePanel::Show()
{
   thePanelSizer->Add(theMiddleSizer, 1, wxGROW | wxALL, 1);
   thePanelSizer->Add(theBottomSizer, 0, wxGROW | wxALL, 1);
   
   // tells the enclosing window to adjust to the size of the sizer
   SetAutoLayout( TRUE );
   SetSizer(thePanelSizer); //use the sizer for layout
   thePanelSizer->Fit(this); //loj: if theParent is used it doesn't show the scroll bar
   thePanelSizer->SetSizeHints(this); //set size hints to honour minimum size
   
   LoadData();
   
   theHelpButton->Disable();   //loj: for build2
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ReportFilePanel::LoadData()
{
   std::string filename = theReport->GetStringParameter("Filename");
   
   wxFile *file = new wxFile();
   bool mFileExists = file->Exists(filename.c_str());
   
   #ifdef DEBUG_REPORT_FILE_PANEL
   MessageInterface::ShowMessage
      ("===> %p, ReportFilePanel::LoadData() filename=%s, mFileExists=%d\n",
       theReport, filename.c_str(), mFileExists);
   #endif
   
   if (mFileExists)
      mFileContentsTextCtrl->LoadFile(filename.c_str());
   else
      mFileContentsTextCtrl->SetValue("");
   
   delete file;
}


//------------------------------------------------------------------------------
// void OnClose(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Close page.
 */
//------------------------------------------------------------------------------
void ReportFilePanel::OnClose(wxCommandEvent &WXUNUSED(event))
{
   GmatAppData::Instance()->GetMainFrame()->CloseActiveChild();
}

//------------------------------------------------------------------------------
// void OnHelp()
//------------------------------------------------------------------------------
/**
 * Shows Helps
 */
//------------------------------------------------------------------------------
void ReportFilePanel::OnHelp()
{
   // open separate window to show help?
}

//------------------------------------------------------------------------------
// void OnCopy(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFilePanel::OnCopy(wxCommandEvent& event)
{
    mFileContentsTextCtrl->Copy();
}


//------------------------------------------------------------------------------
// void OnMsgWinSelectAll(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFilePanel::OnSelectAll(wxCommandEvent& event)
{
	mFileContentsTextCtrl->SelectAll();
}


//------------------------------------------------------------------------------
// void OnMsgWinRightMouseDown(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportFilePanel::OnRightMouseDown(wxMouseEvent& event)
{
	long from, to;
	mFileContentsTextCtrl->GetSelection( &from, &to );
	mPopupMenu->FindItem(ID_MENU_COPY)->Enable( from != to );
	mPopupMenu->FindItem(ID_MENU_SELECTALL)->Enable( !mFileContentsTextCtrl->IsEmpty() );

	mFileContentsTextCtrl->PopupMenu(mPopupMenu);
}


