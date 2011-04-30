//$Id$
//------------------------------------------------------------------------------
//                         ViewTextDialog
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun, NASA/GSFC
// Created: 2007/08/22
//
/**
 * Implements the dialog used to show text.
 */
//------------------------------------------------------------------------------
#include "ViewTextDialog.hpp"
#include "FileManager.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"

BEGIN_EVENT_TABLE(ViewTextDialog, wxDialog)
   EVT_BUTTON(ID_BUTTON, ViewTextDialog::OnOK)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
//  ViewTextDialog(wxWindow *parent, const wxString& title, int w, int h)
//------------------------------------------------------------------------------
/**
 * @param parent  Parent wxWindow for the dialog.
 * @param title  Title for the dialog.
 * @param w  Width for the dialog.
 * @param h  Height for the dialog.
 */
//------------------------------------------------------------------------------
ViewTextDialog::ViewTextDialog(wxWindow *parent, const wxString& title, int w, int h)
   : wxDialog(parent, -1, title, wxDefaultPosition, wxSize(w, h),
              wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER, title)
{
   theDialogSizer = new wxBoxSizer(wxVERTICAL);
   theButtonSizer = new wxBoxSizer(wxHORIZONTAL);
   theMiddleSizer = new wxBoxSizer(wxVERTICAL);
   theBottomSizer = new wxBoxSizer(wxVERTICAL);

   // create bottom buttons
   theOkButton =
      new wxButton(this, ID_BUTTON, "OK", wxDefaultPosition, wxDefaultSize, 0);

   // adds the buttons to button sizer
   theButtonSizer->Add(theOkButton, 0, wxALIGN_CENTER | wxALL, 2);
   theBottomSizer->Add(theButtonSizer, 0, wxALIGN_CENTER | wxALL, 2);

   // Set additional style wxTE_RICH to Ctrl + mouse scroll wheel to decrease or
   // increase text size(loj: 2009.02.05)
   theText = new wxTextCtrl(this, -1, _T(""), wxDefaultPosition, wxSize(w,h),
                            wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH);

   theText->SetMaxLength(320000);
   //theText->SetFont( GmatAppData::GetFont());

   // add items to middle sizer
   theMiddleSizer->Add(theText, 1, wxGROW|wxALL, 2);

   // add items to dialog sizer
   theDialogSizer->Add(theMiddleSizer, 1, wxGROW | wxALL, 1);
   theDialogSizer->Add(theBottomSizer, 0, wxGROW | wxALL, 1);

   // tells the enclosing window to adjust to the size of the sizer
   SetAutoLayout(TRUE);
   SetSizer(theDialogSizer);
   theDialogSizer->Fit(this);
   theDialogSizer->SetSizeHints(this);

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
   catch (GmatBaseException &)
   {
      //MessageInterface::ShowMessage(e.GetMessage());
   }

   CenterOnScreen(wxBOTH);
}


//------------------------------------------------------------------------------
// void AppendText(const wxString& text)
//------------------------------------------------------------------------------
void ViewTextDialog::AppendText(const wxString& text)
{
   theText->AppendText(text);
}


//------------------------------------------------------------------------------
// void OnOK(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Closes the dialog
 */
//------------------------------------------------------------------------------
void ViewTextDialog::OnOK(wxCommandEvent &event)
{
   Close(true);
}
