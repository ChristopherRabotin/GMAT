//$Id$
//------------------------------------------------------------------------------
//                              FindReplaceDialog
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
 * Implements FindReplaceDialog class.
 */
//------------------------------------------------------------------------------

#include "FindReplaceDialog.hpp"
#include "Editor.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_FIND_REPLACE

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE (FindReplaceDialog, wxDialog)
   EVT_BUTTON (ID_BUTTON, FindReplaceDialog::OnButtonClick)
   EVT_TEXT_ENTER (ID_COMBOBOX, FindReplaceDialog::OnComboBoxEnter)
   EVT_KEY_DOWN (FindReplaceDialog::OnKeyDown)
END_EVENT_TABLE()

//----------------------------
// public methods
//----------------------------

//------------------------------------------------------------------------------
// FindReplaceDialog(wxWindow *parent, wxWindowID id, const wxString& title,
//    GmatBase *obj = NULL, const wxPoint& pos = wxDefaultPosition,
//    const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
//------------------------------------------------------------------------------
/**
 * Constructs FindReplaceDialog object.
 *
 * @param  parent  parent window
 * @param  id      window id
 * @param  title   window title
 *
 */
//------------------------------------------------------------------------------
FindReplaceDialog::FindReplaceDialog(wxWindow *parent, wxWindowID id,
                                     const wxString& title, const wxPoint& pos,
                                     const wxSize& size, long style)
   : wxDialog(parent, id, title, pos, size, style, title)
{
   mEditor = NULL;

   Create();
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void FindReplaceDialog::Create()
{
   //---------- find
   wxStaticText *findText = new wxStaticText(this, -1, "Find What");
   mFindComboBox =
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxDefaultSize,
                     mFindArray, wxTE_PROCESS_ENTER);
   mFindNextButton =
      new wxButton(this, ID_BUTTON, wxT("Find Next"),
                   wxDefaultPosition, wxDefaultSize, 0);
   mFindPrevButton =
      new wxButton(this, ID_BUTTON, wxT("Find Previous"),
                   wxDefaultPosition, wxDefaultSize, 0);

   //---------- replace
   wxStaticText *replaceText = new wxStaticText(this, -1, "Replace With");
   mReplaceComboBox =
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxDefaultSize,
                     mReplaceArray);
   mReplaceButton =
      new wxButton(this, ID_BUTTON, wxT("Replace"),
                   wxDefaultPosition, wxDefaultSize, 0);
   mReplaceAllButton =
      new wxButton(this, ID_BUTTON, wxT("Replace All"),
                   wxDefaultPosition, wxDefaultSize, 0);

   //---------- close
   mCloseButton =
      new wxButton(this, ID_BUTTON, wxT("Close"),
                   wxDefaultPosition, wxDefaultSize, 0);

   int bsize = 3;

   //---------- sizer
   wxFlexGridSizer *findSizer = new wxFlexGridSizer(4, 0, 0);
   findSizer->Add(findText, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);
   findSizer->Add(mFindComboBox, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);
   findSizer->Add(mFindNextButton, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);
   findSizer->Add(mFindPrevButton, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);

   findSizer->Add(replaceText, 0, wxALIGN_LEFT|wxALIGN_CENTER_HORIZONTAL|wxGROW|wxALL, bsize);
   findSizer->Add(mReplaceComboBox, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);
   findSizer->Add(mReplaceButton, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);
   findSizer->Add(mReplaceAllButton, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);

   // Add to page sizer and set sizer to this dialog
   wxStaticBoxSizer *findStaticSizer = new wxStaticBoxSizer(wxVERTICAL, this);
   findStaticSizer->Add(findSizer, 0, wxALIGN_CENTRE|wxGROW|wxALL, bsize);

   wxBoxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);
   pageSizer->Add(findStaticSizer, 0, wxALIGN_CENTER|wxALL, 4);
   pageSizer->Add(mCloseButton, 0, wxALIGN_CENTER|wxALL, 4);

   SetAutoLayout(TRUE);
   SetSizer(pageSizer);
   pageSizer->Fit(this);
   pageSizer->SetSizeHints(this);
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles button click event
 */
//------------------------------------------------------------------------------
void FindReplaceDialog::OnButtonClick(wxCommandEvent &event)
{
   if (event.GetEventObject() == mFindNextButton ||
       event.GetEventObject() == mFindPrevButton)
   {
      mFindText = mFindComboBox->GetValue();
      if (mFindText != "")
      {
         #ifdef DEBUG_FIND_REPLACE
         MessageInterface::ShowMessage
            ("FindReplaceDialog::OnButtonClick() FindButton clicked, "
             "mFindText='%s', mEditor=<%p>\n", mFindText.c_str(), mEditor);
         #endif

         if (mFindComboBox->FindString(mFindText, true) == wxNOT_FOUND)
            mFindComboBox->Append(mFindText);

         if (mEditor)
         {
            mEditor->SetFindText(mFindText);
            wxCommandEvent tempEvent;
            if (event.GetEventObject() == mFindNextButton)
               mEditor->OnFindNext(tempEvent);
            else
               mEditor->OnFindPrev(tempEvent);
         }
      }
   }
   else if (event.GetEventObject() == mReplaceButton ||
            event.GetEventObject() == mReplaceAllButton)
   {
      mFindText = mFindComboBox->GetValue();
      mReplaceText = mReplaceComboBox->GetValue();

      if (mFindText != "" && mReplaceText != "")
      {
         #ifdef DEBUG_FIND_REPLACE
         MessageInterface::ShowMessage
            ("FindReplaceDialog::OnButtonClick() ReplaceButtton clicked, "
             "mFindText='%s', mReplaceText='%s', mEditor=<%p>\n", mFindText.c_str(),
             mReplaceText.c_str(), mEditor);
         #endif

         if (mFindComboBox->FindString(mFindText, true) == wxNOT_FOUND)
            mFindComboBox->Append(mFindText);

         if (mReplaceComboBox->FindString(mReplaceText, true) == wxNOT_FOUND)
            mReplaceComboBox->Append(mReplaceText);

         if (mEditor)
         {
            mEditor->SetFindText(mFindText);
            mEditor->SetReplaceText(mReplaceText);
            wxCommandEvent tempEvent;
            if (event.GetEventObject() == mReplaceButton)
               mEditor->OnReplaceNext(tempEvent);
            else
               mEditor->OnReplaceAll(tempEvent);
         }
      }
   }
   else if (event.GetEventObject() == mCloseButton)
   {
      Hide();
   }
}


//------------------------------------------------------------------------------
// void OnComboBoxEnter(wxCommandEvent& event)
//------------------------------------------------------------------------------
void FindReplaceDialog::OnComboBoxEnter(wxCommandEvent& event)
{
   if (event.GetEventObject() == mFindComboBox ||
       event.GetEventObject() == mReplaceComboBox)
   {
      #ifdef DEBUG_FIND_REPLACE
      MessageInterface::ShowMessage
         ("FindReplaceDialog::OnComboBoxEnter() Find entered\n");
      #endif

      wxCommandEvent tempEvent;
      tempEvent.SetEventObject(mFindNextButton);
      OnButtonClick(tempEvent);
   }
}


//------------------------------------------------------------------------------
// void OnKeyDown(wxKeyEvent &event)
//------------------------------------------------------------------------------
void FindReplaceDialog::OnKeyDown(wxKeyEvent &event)
{
}


