//$Header$
//------------------------------------------------------------------------------
//                              MatlabFunctionSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2005/02/11
//
/**
 * Implements MatlabFunctionSetupPanel class.
 */
//------------------------------------------------------------------------------

#include "MatlabFunctionSetupPanel.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_PARAM_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MatlabFunctionSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   
   EVT_TEXT(ID_TEXTCTRL, MatlabFunctionSetupPanel::OnTextUpdate)
   EVT_BUTTON(ID_BUTTON, MatlabFunctionSetupPanel::OnButton)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// MatlabFunctionSetupPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
MatlabFunctionSetupPanel::MatlabFunctionSetupPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent)
{
   theMatlabFunction = (MatlabFunction *)
            theGuiInterpreter->GetFunction(std::string(name.c_str()));

   Create();
   Show();
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void MatlabFunctionSetupPanel::Create()
{
   int bsize = 3; // border size
   
   wxStaticBox *topStaticBox = new wxStaticBox(this, -1, wxT(""));

    // create sizers
   mPageSizer = new wxBoxSizer(wxVERTICAL);
   mTopSizer = new wxStaticBoxSizer(topStaticBox, wxHORIZONTAL);
   wxBoxSizer *fileSizer = new wxBoxSizer(wxHORIZONTAL);

   // create buttons
   mBrowseButton =
      new wxButton(this, ID_BUTTON, "Browse", wxDefaultPosition, wxDefaultSize, 0);

   //wxStaticText
   wxStaticText *pathStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Path: "),
                       wxDefaultPosition, wxDefaultSize, 0);
      
   // wxTextCtrl
   mPathTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                 wxDefaultPosition, wxSize(250,20), 0);

   //------------------------------------------------------
   // add to sizer
   //------------------------------------------------------
   fileSizer->Add(pathStaticText, 0, wxALIGN_CENTER  | wxALL, bsize);
   fileSizer->Add(mPathTextCtrl, 0, wxALIGN_CENTER  | wxALL, bsize);
   fileSizer->Add(mBrowseButton, 0, wxALIGN_CENTER  | wxALL, bsize);
   mTopSizer->Add(fileSizer, 0, wxALIGN_CENTER, bsize);

   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   mPageSizer->Add(mTopSizer, 0, wxALIGN_CENTER | wxALL, bsize);
   theMiddleSizer->Add(mPageSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void MatlabFunctionSetupPanel::LoadData()
{
   // Set the pointer for the "Show Script" button
   mObject = theMatlabFunction;

   int pathId = theMatlabFunction->GetParameterID("FunctionPath");
   std::string path = theMatlabFunction->GetStringParameter(pathId);
   mPathTextCtrl->SetValue(path.c_str());
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void MatlabFunctionSetupPanel::SaveData()
{
   int pathId = theMatlabFunction->GetParameterID("FunctionPath");
   theMatlabFunction->SetStringParameter(pathId,
         mPathTextCtrl->GetValue().c_str());
}

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MatlabFunctionSetupPanel::OnTextUpdate(wxCommandEvent& event)
{
   theApplyButton->Enable();
}

//---------------------------------
// private methods
//---------------------------------
void MatlabFunctionSetupPanel::OnButton(wxCommandEvent& event)
{
   if (event.GetEventObject() == mBrowseButton)
   {
      wxDirDialog dialog(this);

      if (dialog.ShowModal() == wxID_OK)
      {
         wxString filename;
         filename = dialog.GetPath().c_str();
         mPathTextCtrl->SetValue(filename);
      }
   }
   else
   {
      //Error - unknown object
   }
}

