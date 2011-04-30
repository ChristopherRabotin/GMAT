//$Id$
//------------------------------------------------------------------------------
//                              SinglePathSetupPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Linda Jun
// Created: 2006/03/29
//
/**
 * Shows dialog for setting path for various files used in the system.
 */
//------------------------------------------------------------------------------

#include "SinglePathSetupPanel.hpp"
#include "MessageInterface.hpp"

#include "bitmaps/OpenFolder.xpm"

//#define DEBUG_SETPATH

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(SinglePathSetupPanel, wxPanel)
   EVT_BUTTON(ID_BROWSE_BUTTON, SinglePathSetupPanel::OnBrowseButtonClick)
   EVT_TEXT(ID_TEXTCTRL, SinglePathSetupPanel::OnTextChange)
END_EVENT_TABLE()

   
//------------------------------------------------------------------------------
// SinglePathSetupPanel(wxWindow *parent, const wxString &filepath)
//------------------------------------------------------------------------------
SinglePathSetupPanel::SinglePathSetupPanel(wxWindow *parent, const wxString &filepath)
   : wxPanel(parent)
{
   mFullPathName = filepath;
   Create();
   LoadData();
   
   mHasDataChanged = false;
}


//------------------------------------------------------------------------------
// GetFullPathName()
//------------------------------------------------------------------------------
wxString SinglePathSetupPanel::GetFullPathName()
{
   mFullPathName = mFileTextCtrl->GetValue();
   return mFullPathName;
}


//------------------------------------------------------------------------------
// ~SinglePathSetupPanel()
//------------------------------------------------------------------------------
SinglePathSetupPanel::~SinglePathSetupPanel()
{
}


//------------------------------------------------------------------------------
// bool HasDataChanged()
//------------------------------------------------------------------------------
bool SinglePathSetupPanel::HasDataChanged()
{
   return mHasDataChanged;
}


//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void SinglePathSetupPanel::Create()
{
   #ifdef DEBUG_SETPATH
   MessageInterface::ShowMessage("SinglePathSetupPanel::Create() entered.\n");
   #endif
   
   int bsize = 5;
   
   #if __WXMAC__
   int buttonWidth = 40;
   #else
   int buttonWidth = 25;
   #endif
   
   wxBitmap openBitmap = wxBitmap(OpenFolder_xpm);
   
   //------------------------------------------------------
   // Create components
   //------------------------------------------------------
   
   //----- file name and browse button
   mFileTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                  wxDefaultPosition, wxSize(350, 20), 0);
   wxBitmapButton *browseButton =
      new wxBitmapButton(this, ID_BROWSE_BUTTON, openBitmap, wxDefaultPosition,
                         wxSize(buttonWidth, 20));
   
   //----- add to the sizer
   wxFlexGridSizer *pathSizer = new wxFlexGridSizer(2, 0, 0);   
   pathSizer->Add(mFileTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   pathSizer->Add(browseButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   //------------------------------------------------------
   // add to page sizer
   //------------------------------------------------------
   wxBoxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);   
   pageSizer->Add(pathSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   SetSizer(pageSizer);
   
   #ifdef DEBUG_SETPATH
   MessageInterface::ShowMessage("SinglePathSetupPanel::Create() exiting.\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void SinglePathSetupPanel::LoadData()
{
   #ifdef DEBUG_SETPATH
   MessageInterface::ShowMessage
      ("SinglePathSetupPanel::LoadData() mFullPathName='%s'\n", mFullPathName.c_str());
   #endif
   
   mFileTextCtrl->SetValue(mFullPathName);
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void SinglePathSetupPanel::SaveData()
{
}


//------------------------------------------------------------------------------
// void OnBrowseButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void SinglePathSetupPanel::OnBrowseButtonClick(wxCommandEvent& event)
{
   #ifdef DEBUG_SETPATH
   MessageInterface::ShowMessage("SinglePathSetupPanel::OnBrowseButtonClick() entered\n");
   #endif
   
   wxDirDialog dialog(this);
   
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString filename;
      filename = dialog.GetPath().c_str();
      mFileTextCtrl->SetValue(filename);
      mHasDataChanged = true;
   }
}


//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
void SinglePathSetupPanel::OnTextChange(wxCommandEvent &event)
{
   mHasDataChanged = true;
}
