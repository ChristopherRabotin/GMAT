//$Header$
//------------------------------------------------------------------------------
//                              ParameterSelectDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Implements ParameterSelectDialog class. This class shows dialog window where a
 * user parameter can be selected.
 * 
 */
//------------------------------------------------------------------------------

#include "ParameterSelectDialog.hpp"
#include "ParameterCreateDialog.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ParameterSelectDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatDialog::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, ParameterSelectDialog::OnButton)
   EVT_LISTBOX(ID_LISTBOX, ParameterSelectDialog::OnListSelect)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ParameterSelectDialog(wxWindow *parent)
//------------------------------------------------------------------------------
ParameterSelectDialog::ParameterSelectDialog(wxWindow *parent)
   : GmatDialog(parent, -1, wxString(_T("ParameterSelectDialog")))
{
   mParamName = "";
   mIsParamSelected = false;
    
   Create();
   Show();
}

//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void ParameterSelectDialog::Create()
{
   int borderSize = 2;
   wxString emptyList[] = {};

   //wxStaticText
   wxStaticText *paramStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Available Variables"),
                        wxDefaultPosition, wxDefaultSize, 0 );
    
   wxStaticText *paramSelectedStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Variables Selected"),
                        wxDefaultPosition, wxDefaultSize, 0 );
    
   wxStaticText *emptyStaticText =
      new wxStaticText( this, ID_TEXT, wxT("  "),
                        wxDefaultPosition, wxDefaultSize, 0 );
    
   // wxButton
   mAddParamButton = new wxButton( this, ID_BUTTON, wxT("->"),
                                   wxDefaultPosition, wxSize(20,20), 0 );
   mCreateParamButton = new wxButton( this, ID_BUTTON, wxT("Create Variable"),
                                      wxDefaultPosition, wxSize(-1,-1), 0 );
   
   // wxListBox
   mParamListBox =
      theGuiManager->GetConfigParameterListBox(this, ID_LISTBOX, wxSize(150, 200), "");
   
   mParamSelectedListBox = new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                                         wxSize(150, 200), 0, emptyList, wxLB_SINGLE);
   
   
   // wxSizers
   wxBoxSizer *pageBoxSizer = new wxBoxSizer( wxVERTICAL );
   wxFlexGridSizer *paramGridSizer = new wxFlexGridSizer( 3, 0, 0 );
   
   // 2nd row
   paramGridSizer->Add(paramStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   paramGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   paramGridSizer->Add(paramSelectedStaticText, 0, wxALIGN_CENTER|wxALL, borderSize);

   // 3rd row
   paramGridSizer->Add(mParamListBox, 0, wxALIGN_CENTER|wxALL, borderSize);
   paramGridSizer->Add(mAddParamButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   paramGridSizer->Add(mParamSelectedListBox, 0, wxALIGN_CENTER|wxALL, borderSize);

   // 4th row
   paramGridSizer->Add(mCreateParamButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   
   pageBoxSizer->Add( paramGridSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);

   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);

}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ParameterSelectDialog::LoadData()
{
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ParameterSelectDialog::SaveData()
{
   //loj: Can we select multiple parameters? If so we need to use wxArrayString
   // for build2, just assume one parameter selection
   if (mParamSelectedListBox->GetCount() > 0)
   {
      mParamName = mParamSelectedListBox->GetString(0); // first selection
      mIsParamSelected = true;
   }
   else
   {
      mIsParamSelected = false;
   }
}

//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void ParameterSelectDialog::ResetData()
{
   mIsParamSelected = false;
}

//------------------------------------------------------------------------------
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnButton(wxCommandEvent& event)
{    
   if ( event.GetEventObject() == mAddParamButton )  
   {
      // empty listbox first, only one parameter is allowed
      // loj: Do we need multiple selection?
      mParamSelectedListBox->Clear();
      
      wxString s = mParamListBox->GetStringSelection();
      mParamSelectedListBox->Append(s);

      theOkButton->Enable();
   }
   else if ( event.GetEventObject() == mCreateParamButton )  
   {
      //loj: 7/28/04 added
      // show dialog to create parameter
      ParameterCreateDialog paramDlg(this);
      paramDlg.ShowModal();
      
      if (paramDlg.IsParamCreated())
      {
         mParamListBox->Set(theGuiManager->GetNumConfigParameter(),
                            theGuiManager->GetConfigParameterList());
         mAddParamButton->Disable();
      }
   }
}

//------------------------------------------------------------------------------
// void OnListSelect(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnListSelect(wxCommandEvent& event)
{    
   if ( event.GetEventObject() == mParamListBox )  
   {
      mAddParamButton->Enable();
   }
}
