//$Header$
//------------------------------------------------------------------------------
//                              FunctionSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2004/12/15
//
/**
 * Implements FunctionSetupPanel class.
 */
//------------------------------------------------------------------------------

#include "FunctionSetupPanel.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_PARAM_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FunctionSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   
   EVT_TEXT(ID_TEXTCTRL, FunctionSetupPanel::OnTextUpdate)
   EVT_BUTTON(ID_BUTTON, FunctionSetupPanel::OnButton)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// FunctionSetupPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
FunctionSetupPanel::FunctionSetupPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent)
{
   Create();
   Show();
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void FunctionSetupPanel::Create()
{
   int bsize = 3; // border size
   
   wxStaticBox *topStaticBox = new wxStaticBox(this, -1, wxT(""));

    // create sizers
   mMiddleSizer = new wxBoxSizer(wxVERTICAL); 
   mBottomSizer = new wxGridSizer( 1, 0, 0 );
//   mPageSizer = new wxFlexGridSizer( 0, 3, 0, 0 );
   mPageSizer = new wxBoxSizer(wxVERTICAL);
   mTopSizer = new wxStaticBoxSizer(topStaticBox, wxHORIZONTAL);
   wxBoxSizer *fileSizer = new wxBoxSizer(wxHORIZONTAL);

   // create buttons
   mBrowseButton =
      new wxButton(this, ID_BUTTON, "Browse", wxDefaultPosition, wxDefaultSize, 0);

   mLoadButton =
      new wxButton(this, ID_BUTTON, "Load", wxDefaultPosition, wxDefaultSize, 0);

   //wxStaticText
   wxStaticText *fileStaticText =
      new wxStaticText(this, ID_TEXT, wxT("File: "),
                       wxDefaultPosition, wxDefaultSize, 0);
      
   // wxTextCtrl
   mFileNameTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                 wxDefaultPosition, wxSize(150,20), 0);
   mFileContentsTextCtrl = new wxTextCtrl( this, ID_TEXT, wxT(""),
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE | wxGROW);


   //------------------------------------------------------
   // add to sizer
   //------------------------------------------------------
   fileSizer->Add(fileStaticText, 0, wxALIGN_CENTER  | wxALL, bsize);
   fileSizer->Add(mFileNameTextCtrl, 0, wxALIGN_CENTER  | wxALL, bsize);
   fileSizer->Add(mBrowseButton, 0, wxALIGN_CENTER  | wxALL, bsize);
   mTopSizer->Add(fileSizer, 0, wxALIGN_CENTER, bsize);

   mMiddleSizer->Add(mLoadButton, 0, wxALIGN_CENTER | wxALL, bsize);

   mBottomSizer->Add(mFileContentsTextCtrl, 0, wxGROW | wxALIGN_CENTER | wxALL, 
                     bsize);

   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   mPageSizer->Add(mTopSizer, 0, wxALIGN_CENTER | wxALL, bsize);
   mPageSizer->Add(mMiddleSizer, 0, wxGROW | wxALIGN_CENTER | wxALL, bsize);
   mPageSizer->Add(mBottomSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
   theMiddleSizer->Add(mPageSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void FunctionSetupPanel::LoadData()
{
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void FunctionSetupPanel::SaveData()
{
}

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void FunctionSetupPanel::OnTextUpdate(wxCommandEvent& event)
{
}

//---------------------------------
// private methods
//---------------------------------
void FunctionSetupPanel::OnButton(wxCommandEvent& event)
{
   if (event.GetEventObject() == mBrowseButton)
   {
   }
   else if (event.GetEventObject() == mLoadButton)
   {
   }
   else
   {
      //Error - unknown object
   }
}

