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
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ParameterSelectDialog, GmatDialog)
    EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
    EVT_BUTTON(ID_BUTTON_APPLY, GmatDialog::OnApply)
    EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
    EVT_BUTTON(ID_BUTTON, ParameterSelectDialog::OnButton)
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
   addParamButton = new wxButton( this, ID_BUTTON, wxT("->"),
                                  wxDefaultPosition, wxSize(20,20), 0 );
    
    // wxListBox
    paramListBox =
        theGuiManager->GetConfigParameterListBox(this, wxSize(150, 200), "");
    
    paramSelectedListBox = new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                                       wxSize(150, 200), 0, emptyList, wxLB_SINGLE);
    
    
    // wxSizers
    wxBoxSizer *pageBoxSizer = new wxBoxSizer( wxVERTICAL );
    wxFlexGridSizer *paramGridSizer = new wxFlexGridSizer( 3, 0, 0 );
                              
    // 2nd row
    paramGridSizer->Add(paramStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
    paramGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
    paramGridSizer->Add(paramSelectedStaticText, 0, wxALIGN_CENTER|wxALL, borderSize);

    // 3rd row
    paramGridSizer->Add(paramListBox, 0, wxALIGN_CENTER|wxALL, borderSize);
    paramGridSizer->Add(addParamButton, 0, wxALIGN_CENTER|wxALL, borderSize);
    paramGridSizer->Add(paramSelectedListBox, 0, wxALIGN_CENTER|wxALL, borderSize);
        
    pageBoxSizer->Add( paramGridSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);

    //------------------------------------------------------
    // add to parent sizer
    //------------------------------------------------------
    theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);

}

//------------------------------------------------------------------------------
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSelectDialog::OnButton(wxCommandEvent& event)
{    
    if ( event.GetEventObject() == addParamButton )  
    {
        // empty listbox first, only one parameter is allowed
        // loj: Do we need multiple selection?
        paramSelectedListBox->Clear();
        
        wxString s = paramListBox->GetStringSelection();
        paramSelectedListBox->Append(s);

        theOkButton->Enable();
    }
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
    if (paramSelectedListBox->GetCount() > 0)
    {
        mParamName = paramSelectedListBox->GetString(0); // first selection
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

